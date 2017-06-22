#include "ConferenceMode.h"

#include <recorder_shared.h>
#include "common/config.h"
#include "service/messager/message_base.h"

#define PM_GET_CONFERENCE_LIST "getConferenceList"
#define PM_GET_CONFERENCE_FILES "getConferenceFiles"
#define PM_SET_CONVERENCE_INFO  "setConferenceInfo"

ConferenceMode::ConferenceMode(MessageBase *pMessage)
    : CommandBase(pMessage), m_pConfig(Config::GetInstance()) {
  AddActionProc(MB_CONFERENCE_MODE, PM_GET_CONFERENCE_LIST,
      &ConferenceMode::GetConferenceListReply);
  AddActionProc(MB_CONFERENCE_MODE, PM_GET_CONFERENCE_FILES,
      &ConferenceMode::GetConferenceFilesReply);
  AddActionProc(MB_CONFERENCE_MODE, PM_SET_CONVERENCE_INFO,
      &ConferenceMode::SetConferenceInfoReply);
}

ConferenceMode::~ConferenceMode() {}

void ConferenceMode::GetConferenceList() {
  m_pMessage->sendMessage(MB_CONFERENCE_MODE, PM_GET_CONFERENCE_LIST,
                          QJsonObject());
}

void ConferenceMode::SetConferenceInfo(const QString &qstrConferenceUuid,
    const QString &qstrTitle,
    const QString &qstrContent,
    const QString &qstrMembers)
{
    QJsonObject data;
    data.insert("conferenceUuid", qstrConferenceUuid);
    data.insert("title", qstrTitle);
    data.insert("content", qstrContent);
    data.insert("members", qstrMembers);
    m_pMessage->sendMessage(MB_CONFERENCE_MODE, PM_SET_CONVERENCE_INFO, data);
}

void ConferenceMode::SetConferenceInfoReply(bool bResult, QJsonObject jsData)
{
    m_pRecrodShared->receive_conferenceInfoSetted(bResult, jsData.toVariantMap());
}

void ConferenceMode::GetConferenceFiles(const QString &uuid) {
  QJsonObject data;
  data.insert("conferenceUuid", uuid);
  m_pMessage->sendMessage(MB_CONFERENCE_MODE, PM_GET_CONFERENCE_FILES, data);
}

void ConferenceMode::GetConferenceListReply(bool bResult, QJsonObject jsData) {
  if (bResult) {
    QVariantList lsRecordInfoes = jsData["list"].toVariant().toList();
    foreach (const auto &varInfo, lsRecordInfoes) {
      m_pRecrodShared->AddConferenceRecordInfo(varInfo.toMap());
    }
  }
}

void ConferenceMode::GetConferenceFilesReply(bool bResult, QJsonObject jsData) {
  if (bResult) {
    QVariantList lsRecordFiles = jsData["list"].toVariant().toList();
    Q_EMIT getConferenceFiles(lsRecordFiles);
  }
}
