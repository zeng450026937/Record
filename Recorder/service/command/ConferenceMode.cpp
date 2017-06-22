#include "ConferenceMode.h"

#include <recorder_shared.h>
#include <QDebug>
#include "common/config.h"
#include "service/messager/message_base.h"

#define PSL_GET_CONFERENCE_LIST "getConferenceList"
#define PSL_GET_CONFERENCE_FILES "getConferenceFiles"
#define PSL_GET_MOBILE_CONFERENCE_LIST "getConferences"
#define PSL_GET_MOBILE_CONFERENCE_FILES "getConferenceFiles"

ConferenceMode::ConferenceMode(MessageBase *pMessage)
    : CommandBase(pMessage), m_pConfig(Config::GetInstance()) {
  AddActionProc(MB_CONFERENCE_MODE, PSL_GET_CONFERENCE_LIST,
                &ConferenceMode::GetConferenceListReply);
  AddActionProc(MB_CONFERENCE_MODE, PSL_GET_CONFERENCE_FILES,
                &ConferenceMode::GetConferenceFilesReply);
  AddActionProc(MB_MOBILE_MODE, PSL_GET_MOBILE_CONFERENCE_LIST,
                &ConferenceMode::GetMobileConferenceListReply);
  AddActionProc(MB_MOBILE_MODE, PSL_GET_MOBILE_CONFERENCE_FILES,
                &ConferenceMode::GetMobileConferenceFilesReply);
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

void ConferenceMode::GetMobileConferenceList() {
  m_pMessage->sendMessage(MB_MOBILE_MODE, PSL_GET_MOBILE_CONFERENCE_LIST,
                          QJsonObject());
}

void ConferenceMode::GetMobileConferenceFiles(const QString &uuid) {
  QJsonObject data;
  data.insert("conferenceUuid", uuid);
  m_pMessage->sendMessage(MB_MOBILE_MODE, PSL_GET_MOBILE_CONFERENCE_FILES,
                          data);
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
    Q_EMIT getConferenceFiles(RecorderShared::RT_CONFERENCE, lsRecordFiles);
  }
}

void ConferenceMode::GetMobileConferenceListReply(bool bResult,
                                                  QJsonObject jsData) {
  if (bResult) {
    QVariantList lsRecordInfoes = jsData["list"].toVariant().toList();
    foreach (const auto &varInfo, lsRecordInfoes) {
      m_pRecrodShared->AddMobileRecordInfo(varInfo.toMap());
    }
  }
}

void ConferenceMode::GetMobileConferenceFilesReply(bool bResult,
                                                   QJsonObject jsData) {
  if (bResult) {
    QVariantList lsRecordFiles = jsData["list"].toVariant().toList();
    Q_EMIT getConferenceFiles(RecorderShared::RT_MOBILE, lsRecordFiles);
  }
}
