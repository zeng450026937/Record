#include "ConferenceMode.h"

#include <recorder_shared.h>
#include <QDebug>
#include "common/config.h"
#include "service/messager/message_base.h"

#define PSL_GET_CONFERENCE_LIST "getConferenceList"
#define PSL_GET_CONFERENCE_FILES "getConferenceFiles"

ConferenceMode::ConferenceMode(MessageBase *pMessage)
    : CommandBase(pMessage), m_pConfig(Config::GetInstance()) {
  AddActionProc(MB_CONFERENCE_MODE, PSL_GET_CONFERENCE_LIST,
                &ConferenceMode::GetConferenceListReply);
  AddActionProc(MB_CONFERENCE_MODE, PSL_GET_CONFERENCE_FILES,
                &ConferenceMode::GetConferenceFilesReply);
}

ConferenceMode::~ConferenceMode() {}

void ConferenceMode::GetConferenceList() {
  m_pMessage->sendMessage(MB_CONFERENCE_MODE, PSL_GET_CONFERENCE_LIST,
                          QJsonObject());
}

void ConferenceMode::GetConferenceFiles(const QString &uuid) {
  QJsonObject data;
  data.insert("conferenceUuid", uuid);
  m_pMessage->sendMessage(MB_CONFERENCE_MODE, PSL_GET_CONFERENCE_FILES, data);
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
