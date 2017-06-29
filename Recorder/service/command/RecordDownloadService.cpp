#include "RecordDownloadService.h"
#include <Winsock2.h>
#include <service/messager/message_base.h>
#include <QDebug>
#include "RecordDownloadReceiver.h"
#include "common/config.h"
#include "messager/binary_reader.h"
#include "service/service_thread.h"
#include "storage/database_impl.h"
#include "storage/include/clip_file_database.h"
#include "storage/include/conferencedatabase.h"

#define RFDS_DOWNLOAD_FILE "downloadFile"
#define RFDS_DOWNLOAD_ACK "downloadFileAck"

RecordDownloadService *RecordDownloadService::s_pSingleton = nullptr;

RecordDownloadService::RecordDownloadService(MessageBase *pMessager)
    : CommandBase(pMessager),
      m_pConfig(Config::GetInstance()),
      m_pConfService(Q_NULLPTR),
      m_pDownloadDB(ServiceThread::GetInstance()->GetDownloadDB()),
      m_pConferenceDB(Q_NULLPTR),
      m_pClipFileDB(Q_NULLPTR),
      m_iDownloadStatusTimerId(0) {
  m_pTempRecordInfo = new RecordBlockInfo();
  connect(m_pMessage, SIGNAL(notify_binary(QByteArray)), this,
          SLOT(on_binary_received(QByteArray)), Qt::QueuedConnection);
  connect(this, &RecordDownloadService::startDownloadTick, this,
          [=]() { m_iDownloadStatusTimerId = startTimer(1000); });
  AddActionProc(MB_CONFERENCE_MODE, RFDS_DOWNLOAD_FILE,
                &RecordDownloadService::DownloadRecordReply);
  AddActionProc(MB_PERSONAL_MODE, RFDS_DOWNLOAD_FILE,
                &RecordDownloadService::DownloadRecordReply);
  AddActionProc(MB_MOBILE_MODE, RFDS_DOWNLOAD_FILE,
                &RecordDownloadService::DownloadRecordReply);
}

RecordDownloadReceiver *RecordDownloadService::GetDownloadReciver(
    const QString &qstrFileUuid, const QString &qstrConferenceUuid,
    const QString &qstrDeviceUuid) {
  QByteArray baLocator;
  baLocator.append(qstrConferenceUuid);
  baLocator.append(qstrDeviceUuid);
  baLocator.append(qstrFileUuid);
  auto itrFound = m_mapReceiver.find(baLocator);
  if (itrFound != m_mapReceiver.end()) {
    return itrFound->second;
  }

  return nullptr;
}

RecordDownloadService *RecordDownloadService::GetInstance() {
  if (nullptr == RecordDownloadService::s_pSingleton) {
    ServiceThread *pService = ServiceThread::GetInstance();
    s_pSingleton = new RecordDownloadService(pService->GetMessager());
    s_pSingleton->moveToThread(pService);
  }

  return RecordDownloadService::s_pSingleton;
}

bool RecordDownloadService::DownloadRecord(
    RecordDownloadReceiver *pDownloadReceiver, int iType,
    const QString qstrConferenceTitle, const QString &qstrUserId,
    const QString &qstrFileUuid, const QString &qstrConferenceUuid,
    const QString &qstrDeviceUuid, const QString &qstrCreateTime,
    const QString &qstrFileExtension) {
  int iStartPos = 0;
  if (!pDownloadReceiver->CreateReciveData(
          iType, qstrConferenceTitle, qstrUserId,
          qstrFileExtension.isEmpty() ? "amr" : qstrFileExtension, qstrFileUuid,
          qstrConferenceUuid, qstrDeviceUuid, qstrCreateTime, &iStartPos)) {
    return false;
  }

  QByteArray baLocator;
  baLocator.append(qstrConferenceUuid);
  baLocator.append(qstrDeviceUuid);
  baLocator.append(qstrFileUuid);
  m_mapReceiver.insert(ReciverMap::value_type(baLocator, pDownloadReceiver));

  QJsonObject jsData;
  jsData.insert("fileUuid", qstrFileUuid);
  jsData.insert("conferenceUuid", qstrConferenceUuid);
  jsData.insert("deviceUuid", qstrDeviceUuid);
  jsData.insert("userId", qstrUserId);
  jsData.insert("startPos", iStartPos);

  switch (iType) {
    case RecorderShared::RT_PERSONAL:
      m_pMessage->sendMessage(MB_PERSONAL_MODE, RFDS_DOWNLOAD_FILE, jsData);
      break;
    case RecorderShared::RT_CONFERENCE:
      m_pMessage->sendMessage(MB_CONFERENCE_MODE, RFDS_DOWNLOAD_FILE, jsData);
      break;
    case RecorderShared::RT_MOBILE:
      m_pMessage->sendMessage(MB_MOBILE_MODE, RFDS_DOWNLOAD_FILE, jsData);
      break;
    default:
      break;
  }

  if (m_mapReceiver.size() == 1) Q_EMIT startDownloadTick();

  return true;
}

bool RecordDownloadService::ResumeDownload(int iType, const QString &qstrUserId,
                                           const QString &qstrFileUuid,
                                           const QString &qstrConferenceUuid,
                                           const QString &qstrDeviceUuid) {
  QByteArray baLocator;
  baLocator.append(qstrConferenceUuid);
  baLocator.append(qstrDeviceUuid);
  baLocator.append(qstrFileUuid);

  ReciverMap::iterator iter = m_mapReceiver.find(baLocator);
  if (iter != m_mapReceiver.end()) {
    RecordDownloadReceiver *pReciver = iter->second;
    int iStartPos = pReciver->GetWriteSize();
    QJsonObject jsData;
    jsData.insert("fileUuid", qstrFileUuid);
    jsData.insert("conferenceUuid", qstrConferenceUuid);
    jsData.insert("deviceUuid", qstrDeviceUuid);
    jsData.insert("userId", qstrUserId);
    jsData.insert("startPos", iStartPos);

    switch (iType) {
      case RecorderShared::RT_PERSONAL:
        m_pMessage->sendMessage(MB_PERSONAL_MODE, RFDS_DOWNLOAD_FILE, jsData);
        break;
      case RecorderShared::RT_CONFERENCE:
        m_pMessage->sendMessage(MB_CONFERENCE_MODE, RFDS_DOWNLOAD_FILE, jsData);
        break;
      case RecorderShared::RT_MOBILE:
        m_pMessage->sendMessage(MB_MOBILE_MODE, RFDS_DOWNLOAD_FILE, jsData);
        break;
      default:
        break;
    }
    return true;
  }
  return false;
}

bool RecordDownloadService::IsExsitReceiver(const QString &qstrFileUuid,
                                            const QString &qstrConferenceUuid,
                                            const QString &qstrDeviceUuid) {
  QByteArray baLocator;
  baLocator.append(qstrConferenceUuid);
  baLocator.append(qstrDeviceUuid);
  baLocator.append(qstrFileUuid);

  return m_mapReceiver.end() != m_mapReceiver.find(baLocator);
}

void RecordDownloadService::SetConfServiceImpl(
    ConfServiceImpl *pConfServiceImpl) {
  m_pConfService = pConfServiceImpl;
}

void RecordDownloadService::DownloadAck(int iType, const char *pUserId,
                                        const char *pFileUuid,
                                        const char *pConferenceUuid,
                                        const char *pDeviceId, int iStartPos) {
  QJsonObject jsData;
  jsData.insert("userId", pUserId);
  jsData.insert("fileUuid", pFileUuid);
  jsData.insert("conferenceUuid", pConferenceUuid);
  jsData.insert("deviceUuid", pDeviceId);
  jsData.insert("startPos", iStartPos);

  switch (iType) {
    case RecorderShared::RT_PERSONAL:
      m_pMessage->sendMessage(MB_PERSONAL_MODE, RFDS_DOWNLOAD_ACK, jsData);
      break;
    case RecorderShared::RT_CONFERENCE:
      m_pMessage->sendMessage(MB_CONFERENCE_MODE, RFDS_DOWNLOAD_ACK, jsData);
      break;
    case RecorderShared::RT_MOBILE:
      m_pMessage->sendMessage(MB_MOBILE_MODE, RFDS_DOWNLOAD_ACK, jsData);
      break;
    default:
      break;
  }
}

void RecordDownloadService::DownloadRecordReply(bool bResult,
                                                const QJsonObject &jsData) {
  if (m_mapReceiver.empty()) {
    return;
  }

  QJsonDocument jsDoc(jsData);
  QString qstr = jsDoc.toJson();

  QByteArray baLocator;
  baLocator.append(jsData["conferenceUuid"].toString());
  baLocator.append(jsData["deviceUuid"].toString());
  baLocator.append(jsData["fileUuid"].toString());
  ReciverMap::iterator itrFond = m_mapReceiver.find(baLocator);
  if (itrFond != m_mapReceiver.end()) {
    RecordDownloadReceiver *&pReceiver = itrFond->second;
    if (bResult) {
      int iResult = jsData["result"].toInt();
      if (RecordDownloadReceiver::EC_DOWNLOADING == iResult) {
        pReceiver->StartReceiveTrigger(iResult, jsData["totalSize"].toInt());
      } else {
        pReceiver->StartReceiveTrigger(iResult, 0);
        delete pReceiver;
        m_mapReceiver.erase(itrFond);
      }
    } else {
      pReceiver->StartReceiveTrigger(jsData["result"].toInt(), 0);
      delete pReceiver;
      m_mapReceiver.erase(itrFond);
    }

    if (m_mapReceiver.empty()) {
      killTimer(m_iDownloadStatusTimerId);
      m_iDownloadStatusTimerId = 0;
    }
  }
}

void RecordDownloadService::on_binary_received(QByteArray binary) {
  char *pData = binary.data();
  int iLocator = 0;
  m_pTempRecordInfo->iRecordType = htonl(*(int *)pData + iLocator);  // type
  iLocator += sizeof(int);

  memcpy(m_pTempRecordInfo->szUserId, pData + iLocator, 17);  // userId
  m_pTempRecordInfo->szUserId[17] = '\0';
  iLocator += 17;

  memcpy(m_pTempRecordInfo->szDeviceUuid, pData + iLocator, 32);  // deviceuuid
  m_pTempRecordInfo->szDeviceUuid[32] = '\0';
  iLocator += 32;

  memcpy(m_pTempRecordInfo->szFileUuid, pData + iLocator, 32);  // fileUuid
  m_pTempRecordInfo->szFileUuid[32] = '\0';
  iLocator += 32;

  memcpy(m_pTempRecordInfo->szConferenceUuid, pData + iLocator,
         32);  // conferenceUuid
  m_pTempRecordInfo->szConferenceUuid[32] = '\0';
  iLocator += 32;

  iLocator += sizeof(int);  // meetingTime锛?// 鐩墠娌＄敤锛屾墍浠ヤ笉澶勭悊銆?

  m_pTempRecordInfo->iStartPos = htonl(*(int *)(pData + iLocator));  // startPos
  iLocator += sizeof(int);

  m_pTempRecordInfo->iStatus = htonl(*(int *)(pData + iLocator));  // status
  iLocator += sizeof(int);

  m_pTempRecordInfo->iDataSize = htonl(*(int *)(pData + iLocator));  // sizeByte
  iLocator += sizeof(int);

  QByteArray baLocator;
  baLocator.append(m_pTempRecordInfo->szConferenceUuid);
  baLocator.append(m_pTempRecordInfo->szDeviceUuid);
  baLocator.append(m_pTempRecordInfo->szFileUuid);

  ReciverMap::iterator itrFond = m_mapReceiver.find(baLocator);
  if (itrFond != m_mapReceiver.end()) {
    bool bDownloadCompleted = m_pTempRecordInfo->iStatus == 1;
    RecordDownloadReceiver *&pReceiver = itrFond->second;
    pReceiver->WriteData(pData + iLocator, m_pTempRecordInfo->iDataSize,
                         bDownloadCompleted);

    if (RecorderShared::RT_CONFERENCE == m_pTempRecordInfo->iRecordType) {
      emit conference_receive_data_notify(
          m_pTempRecordInfo->szConferenceUuid,
          bDownloadCompleted ? 100 : pReceiver->GetDownloadedPercent());
    }

    if (bDownloadCompleted) {
      delete pReceiver;
      m_mapReceiver.erase(itrFond);
    } else {
      DownloadAck(
          m_pTempRecordInfo->iRecordType, m_pTempRecordInfo->szUserId,
          m_pTempRecordInfo->szFileUuid, m_pTempRecordInfo->szConferenceUuid,
          m_pTempRecordInfo->szDeviceUuid, m_pTempRecordInfo->iStartPos);
    }

    if (m_mapReceiver.empty()) {
      killTimer(m_iDownloadStatusTimerId);
    }
  }
}

void RecordDownloadService::timerEvent(QTimerEvent *pEvent) {
  auto itrBegin = m_mapReceiver.begin();
  while (itrBegin != m_mapReceiver.end()) {
    itrBegin->second->TickDownloadStatus();
    if (itrBegin->second->isTimeOut()) {
      itrBegin = m_mapReceiver.erase(itrBegin);
    } else {
      ++itrBegin;
    }
  }
}
