#include "ConferenceMode.h"

#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include "common/config.h"
#include "service/conf_service_impl.h"
#include "service/messager/message_base.h"
#include "storage/database_impl.h"
#include "storage/include/clip_file_database.h"
#include "storage/include/conferencedatabase.h"
#include "storage/include/download_database.h"
#include "storage/include/templatedatabase.h"

#define PSL_GET_CONFERENCE_LIST "getConferenceList"
#define CM_GET_TEMPLATE_LIST "getTemplateList"
#define CM_DOWNLOAD_FILE "downloadFile"

ConferenceMode::ConferenceMode(MessageBase *pMessage)
    : CommandModeBase(pMessage, "conference"),
      m_pConfig(Config::GetInstance()),
      m_pClipFileDB(NULL),
      m_pConferenceDB(NULL),
      m_pDownloadDB(NULL),
      m_pTemplateDB(nullptr) {
  AddActionProc(PSL_GET_CONFERENCE_LIST,
                &ConferenceMode::GetConferenceListReply);
  AddActionProc(CM_GET_TEMPLATE_LIST, &ConferenceMode::GetTemplateListReply);
  AddActionProc(CM_DOWNLOAD_FILE, &ConferenceMode::DownloadFileReply);
}

ConferenceMode::~ConferenceMode() {}

void ConferenceMode::SetDataBase(DataBase_Impl *pDatabase) {
  m_pClipFileDB = ClipFileDatabase::GetInterface(pDatabase);
  m_pDownloadDB = DownloadDatabase::GetInterface(pDatabase);
  m_pConferenceDB = ConferenceDatabase::GetInterface(pDatabase);
  m_pTemplateDB = TemplateDatabase::GetInterface(pDatabase);
}

void ConferenceMode::GetConferenceList() {
  SendAction(PSL_GET_CONFERENCE_LIST, QJsonObject());
}

void ConferenceMode::GetTemplateList() {
  SendAction(CM_GET_TEMPLATE_LIST, QJsonObject());
}

void ConferenceMode::DownloadFile(const QString &qstrConferenceUuid,
                                  const QString &qstrDeviceUuid,
                                  int iStartpos) {
  QJsonObject jsData;
  jsData.insert("conferenceUuid", qstrConferenceUuid);
  jsData.insert("deviceUuid", qstrDeviceUuid);
  jsData.insert("startpos", iStartpos);
  jsData.insert("userId", m_pConfig->GetUser().user_id);
  SendAction(CM_DOWNLOAD_FILE, jsData);
}

void ConferenceMode::GetTemplateListReply(bool bResult, QJsonObject jsData) {
  if (bResult) {
    QVariantList lsTemplateInfoes = jsData["list"].toVariant().toList();
    foreach (const auto &varInfo, lsTemplateInfoes) {
      m_pTemplateDB->AddTemplate(varInfo.toMap());
    }
  }

  template_list_got_trigger(bResult);
}

void ConferenceMode::GetConferenceListReply(bool bResult, QJsonObject jsData) {
  if (bResult) {
    QVariantList lsRecordInfoes = jsData["list"].toVariant().toList();
    foreach (const auto &varInfo, lsRecordInfoes) {
      this->checkConference(varInfo.toMap());
    }
  }

  emit conference_list_got_trigger(bResult);
}

void ConferenceMode::DownloadFileReply(bool bResult, QJsonObject jsData) {
  if (bResult) {
    if (jsData.value("result").toInt() != 0) {
      qDebug() << "downlad file error response:" << jsData;
    } else {
      QString qstrConferenceUuid = jsData.value("conferenceUuid").toString();

      int total_size = _conf_total_size_map.value(qstrConferenceUuid);
      total_size += (jsData.value("totalSize").toInt() -
                     jsData.value("startpos").toInt());

      _conf_total_size_map.insert(qstrConferenceUuid, total_size);

      // TODO : 优化过程中，暂不知道解析出来的数据要干嘛，暂不处理。
    }
  }
}

void ConferenceMode::checkConference(QVariantMap &vmRecordInfo) {
  QString qstrConferenceUuid = vmRecordInfo.value("uuid").toString();
  QStringList needed_list = vmRecordInfo.value("devices").toString().split(",");
  needed_list.removeDuplicates();

  bool completed =
      m_pConfService->checkConferenceFile(qstrConferenceUuid, needed_list);
  vmRecordInfo.insert("completed", completed);
  m_pConferenceDB->AddConference(vmRecordInfo);

  if (!completed && Config::GetInstance()->_auto_download) {
    DownloadFile(qstrConferenceUuid, vmRecordInfo["deviceUuid"].toString(),
                 vmRecordInfo["startpos"].toInt());
  }
}

void ConferenceMode::SendAction(const char *pAction,
                                const QJsonObject &jsData) {
  m_pMessage->sendMessage("conference", pAction, jsData);
}
