#include "RecordDownloadService.h"
#include <QDebug>
#include <Winsock2.h>
#include <service/messager/message_base.h>
#include "common/config.h"
#include "service/service_thread.h"
#include "messager/binary_reader.h"
#include "storage/database_impl.h"
#include "storage/include/clip_file_database.h"
#include "storage/include/conferencedatabase.h"
#include "RecordDownloadReceiver.h"


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
    m_iDownloadStatusTimerId(0)
{
    m_pTempRecordInfo = new RecordBlockInfo();
    connect(m_pMessage, SIGNAL(notify_binary(QByteArray)), this, SLOT(on_binary_received(QByteArray)), Qt::QueuedConnection);
    AddActionProc(MB_CONFERENCE_MODE, RFDS_DOWNLOAD_FILE, &RecordDownloadService::DownloadRecordReply);
    AddActionProc(MB_PERSONAL_MODE, RFDS_DOWNLOAD_FILE, &RecordDownloadService::DownloadRecordReply);
}

RecordDownloadService * RecordDownloadService::GetInstance()
{
    if (nullptr == RecordDownloadService::s_pSingleton)
    {
        ServiceThread *pService = ServiceThread::GetInstance();
        s_pSingleton = new RecordDownloadService(pService->GetMessager());
        s_pSingleton->moveToThread(pService);
    }

    return RecordDownloadService::s_pSingleton;
}

bool RecordDownloadService::DownloadRecord(RecordDownloadReceiver *pDownloadReceiver,
    int iType,
    const QString &qstrFileUuid,
    const QString &qstrConferenceUuid,
    const QString &qstrDeviceUuid,
    const QString &qstrCreateTime,
    const QString &qstrFileExtension)
{
    int iStartPos = 0;
    if (!pDownloadReceiver->CreateReciveData(iType,
                            qstrFileExtension.isEmpty() ? "amr" : qstrFileExtension, // 服务端给的文件扩展名为空时，录音为amr格式
                            qstrFileUuid,
                            qstrConferenceUuid,
                            qstrDeviceUuid,
                            qstrCreateTime,
                            &iStartPos))
    {
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
    jsData.insert("userId", m_pConfig->GetUser().user_id);
    jsData.insert("startPos", iStartPos);

    switch (iType)
    {
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

    if (m_mapReceiver.size() == 1)
    {
        m_iDownloadStatusTimerId = startTimer(1000);
    }

    return true;
}


void RecordDownloadService::SetConfServiceImpl(ConfServiceImpl *pConfServiceImpl)
{
    m_pConfService = pConfServiceImpl;
}

void RecordDownloadService::DownloadAck(int iType, const char *pFileUuid, const char *pConferenceUuid, const char *pDeviceId, int iStartPos)
{
    QJsonObject jsData;
    jsData.insert("fileUuid", pFileUuid);
    jsData.insert("conferenceUuid", pConferenceUuid);
    jsData.insert("deviceUuid", pDeviceId);
    jsData.insert("userId", m_pConfig->GetUser().user_id);
    jsData.insert("startPos", iStartPos);

    switch (iType)
    {
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

void RecordDownloadService::DownloadRecordReply(bool bResult, const QJsonObject &jsData)
{
    if (m_mapReceiver.empty())
    {
        return;
    }

    QJsonDocument jsDoc(jsData);
    QString qstr = jsDoc.toJson();

    QByteArray baLocator;
    baLocator.append(jsData["conferenceUuid"].toString());
    baLocator.append(jsData["deviceUuid"].toString());
    baLocator.append(jsData["fileUuid"].toString());
    ReciverMap::iterator itrFond = m_mapReceiver.find(baLocator);
    if (itrFond != m_mapReceiver.end())
    {
        RecordDownloadReceiver *&pReceiver = itrFond->second;
        if (bResult)
        {
            int iResult = jsData["result"].toInt();
            if (RecordDownloadReceiver::EC_DOWNLOADING == iResult) // 参见接口文档，0:可以正常下载   1:文件不存在   2: 参数不合法
            {
                pReceiver->StartReceiveTrigger(iResult, jsData["totalSize"].toInt());
            }
            else 
            {
                pReceiver->StartReceiveTrigger(iResult, 0);
                m_mapReceiver.erase(itrFond);
            }
        }
        else
        {
            pReceiver->StartReceiveTrigger(jsData["result"].toInt(), 0);
            m_mapReceiver.erase(itrFond);
        }

        if (m_mapReceiver.empty())
        {
            killTimer(m_iDownloadStatusTimerId);
            m_iDownloadStatusTimerId = 0;
        }
    }
}

void RecordDownloadService::on_binary_received(QByteArray binary)
{
    char *pData = binary.data();
    int iLocator = 0;
    m_pTempRecordInfo->iRecordType = *(int*)pData + iLocator; // type
    iLocator += sizeof(int);

    memcpy(m_pTempRecordInfo->szUserId,pData+ iLocator,17); // userId
    m_pTempRecordInfo->szUserId[17] = '\0';
    iLocator += 17;

    memcpy(m_pTempRecordInfo->szDeviceUuid, pData + iLocator, 32); // deviceuuid
    m_pTempRecordInfo->szDeviceUuid[32] = '\0';
    iLocator += 32;

    memcpy(m_pTempRecordInfo->szFileUuid, pData + iLocator, 32); // fileUuid
    m_pTempRecordInfo->szFileUuid[32] = '\0';
    iLocator += 32;

    memcpy(m_pTempRecordInfo->szConferenceUuid, pData + iLocator, 32); // conferenceUuid
    m_pTempRecordInfo->szConferenceUuid[32] = '\0';
    iLocator += 32;

    iLocator += sizeof(int); // meetingTime， // 目前没用，所以不处理。

    m_pTempRecordInfo->iStartPos = htonl(*(int*)(pData + iLocator)); // startPos
    iLocator += sizeof(int);

    m_pTempRecordInfo->iStatus = htonl(*(int*)(pData + iLocator)); // status
    iLocator += sizeof(int);

    m_pTempRecordInfo->iDataSize = htonl(*(int*)(pData + iLocator)); // sizeByte
    iLocator += sizeof(int);

    QByteArray baLocator;
    baLocator.append(m_pTempRecordInfo->szConferenceUuid);
    baLocator.append(m_pTempRecordInfo->szDeviceUuid);
    baLocator.append(m_pTempRecordInfo->szFileUuid);

    ReciverMap::iterator itrFond = m_mapReceiver.find(baLocator);
    if (itrFond != m_mapReceiver.end())
    {
        RecordDownloadReceiver *&pReceiver = itrFond->second;
        pReceiver->WriteData(pData + iLocator, m_pTempRecordInfo->iDataSize, m_pTempRecordInfo->iStatus == 1);
        if (m_pTempRecordInfo->iStatus == 0)
        {
            DownloadAck(m_pTempRecordInfo->iRecordType,
                m_pTempRecordInfo->szFileUuid,
                m_pTempRecordInfo->szConferenceUuid, 
                m_pTempRecordInfo->szDeviceUuid, 
                m_pTempRecordInfo->iStartPos);
        }
        else
        {
            m_mapReceiver.erase(itrFond);
        }

        if (m_mapReceiver.empty())
        {
            killTimer(m_iDownloadStatusTimerId);
        }
    }
    else
    {
        //创建一个 list_form
    }
}

void RecordDownloadService::timerEvent(QTimerEvent *pEvent)
{
    for (auto itrBegin = m_mapReceiver.begin(),itrEnd = m_mapReceiver.end();
        itrBegin != itrEnd; ++itrBegin)
    {
        itrBegin->second->TickDownloadStatus();
    }
}
