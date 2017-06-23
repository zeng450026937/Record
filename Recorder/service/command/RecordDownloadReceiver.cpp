#include "RecordDownloadReceiver.h"

#include <QDebug>
#include <QFile>
#include <QString>
#include "common/config.h"
#include "service/service_thread.h"
#include "storage/download_database.h"
#include "storage/include/clip_file_database.h"

struct ReceiverData {
  int iRecordId;
  QFile recordFile;
  QString qstrFilePath;
  int iFileSize;
  int iWritten;

  int iDownloadPerSecond;

  ReceiverData()
      : iRecordId(0), iFileSize(0.0), iWritten(0), iDownloadPerSecond(0) {}
};

class RecordDownloadReceiverPrivate {
  friend class RecordDownloadReceiver;

  ReceiverData *pData;

  RecordDownloadReceiverPrivate() : pData(nullptr) {}
};

RecordDownloadReceiver::RecordDownloadReceiver()
    : m(new RecordDownloadReceiverPrivate()) {
  connect(this, SIGNAL(download_prompt(QString)), this,
          SLOT(onDownloadPrompt(QString)));
}

RecordDownloadReceiver::~RecordDownloadReceiver() { delete m; }

bool RecordDownloadReceiver::StartReceiveTrigger(int iResult, int iFileSize) {
  if (iResult == RecordDownloadReceiver::EC_DOWNLOADING) {
    if (!m->pData->recordFile.open(QIODevice::Append)) {
      return false;
    }

    m->pData->iFileSize = iFileSize;
    emit downloading_tick(
        ((double)m->pData->iWritten / m->pData->iFileSize) * 100, 0);
    emit download_prompt(QString());  // 提示UI切到正在下载状态
  } else {
    switch (iResult) {
      case RecordDownloadReceiver::EC_ARGS_INVALID:
        emit download_prompt(QString::fromUtf16(
            (const ushort *)L"下载失败，下载请求参数无效。"));
        break;

      case RecordDownloadReceiver::EC_FILE_UNEXSISTS:
        emit download_prompt(
            QString::fromUtf16((const ushort *)L"下载失败，下载文件不存在。"));
        break;

      default:
        emit download_prompt(
            QString::fromUtf16((const ushort *)L"下载失败，未知的异常错误."));
        break;
    }
  }

  return true;
}

bool RecordDownloadReceiver::WriteData(const char *pData, int iDataSize,
                                       bool bCompleted) {
  m->pData->recordFile.write(pData, iDataSize);
  m->pData->iWritten += iDataSize;
  m->pData->iDownloadPerSecond += iDataSize;
  if (bCompleted) {
    DownloadDatabase *pDownloadDB =
        ServiceThread::GetInstance()->GetDownloadDB();

    m->pData->recordFile.close();
    pDownloadDB->SetDownloadCompeletedById(m->pData->iRecordId);
    delete m->pData;
    emit downloading_tick(100, 0);
  }

  return true;
}

void RecordDownloadReceiver::TickDownloadStatus() {
  emit downloading_tick(
      ((double)m->pData->iWritten / m->pData->iFileSize) * 100,
      m->pData->iDownloadPerSecond);
  m->pData->iDownloadPerSecond = 0;
}

int RecordDownloadReceiver::GetDownloadStatus(const QString &qstrFileUuid,
                                              const QString &qstrConferenceUuid,
                                              const QString &qstrDeviceUuid) {
  DownloadDatabase *pDownloadDB = ServiceThread::GetInstance()->GetDownloadDB();

  QString qstrFilePath;
  bool bCompeleted = false;
  if (pDownloadDB->GetCompeleteStatus(qstrFileUuid, qstrConferenceUuid,
                                      qstrDeviceUuid, &qstrFilePath,
                                      &bCompeleted)) {
    if (QFile(qstrFilePath).exists()) {
      return bCompeleted ? DS_COMPELETED : DS_UNCOMPLETED;
    }
  }
  return DS_UNEXSITS;
}

bool RecordDownloadReceiver::CreateReciveData(
    int iType, const QString &qstrFileExtension, const QString &qstrFileUuid,
    const QString &qstrConferenceUuid, const QString &qstrDeviceUuid,
    QString qstrCreateTime, int *iStartPos) {
  m->pData = new ReceiverData();
  DownloadDatabase *pDownloadDB = ServiceThread::GetInstance()->GetDownloadDB();

  if (pDownloadDB->GetDownloadInfo(qstrFileUuid, qstrConferenceUuid,
                                   qstrDeviceUuid, &m->pData->iRecordId,
                                   &m->pData->qstrFilePath)) {
    m->pData->recordFile.setFileName(m->pData->qstrFilePath);
    if (m->pData->recordFile.exists()) {
      m->pData->iWritten = m->pData->recordFile.size();
    } else {
      m->pData->iWritten = 0;
    }

    *iStartPos = m->pData->iWritten;
  } else {
    Config *pConfig = Config::GetInstance();
    QRegExp regExp("[-: ]");
    qstrCreateTime.replace(regExp, "");
    m->pData->qstrFilePath =
        QString("%1\\%2_%3.%4")
            .arg(pConfig->_output_dir, pConfig->GetUser().user_name,
                 qstrCreateTime, qstrFileExtension);

    m->pData->iRecordId =
        pDownloadDB->InsertDownloadInfo(iType, qstrFileUuid, qstrConferenceUuid,
                                        qstrDeviceUuid, m->pData->qstrFilePath);
    if (m->pData->iRecordId == 0) {
      delete m->pData;
      return false;
    }

    m->pData->recordFile.setFileName(m->pData->qstrFilePath);
    *iStartPos = 0;

    ClipFileDatabase *pClipDB = ServiceThread::GetInstance()->GetClipDB();
    pClipDB->AddFile(qstrConferenceUuid, -1, m->pData->qstrFilePath);
  }

  return true;
}
