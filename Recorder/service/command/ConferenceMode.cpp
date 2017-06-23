#include "ConferenceMode.h"

#include <recorder_shared.h>
#include <QDebug>
#include "common/config.h"
#include "service/messager/message_base.h"

#define PM_GET_CONFERENCE_LIST "getConferenceList"
#define PM_GET_CONFERENCE_FILES "getConferenceFiles"
#define PM_SET_CONVERENCE_INFO "setConferenceInfo"
#define PSL_GET_MOBILE_CONFERENCE_LIST "getConferences"
#define PSL_GET_MOBILE_CONFERENCE_FILES "getConferenceFiles"

ConferenceMode::ConferenceMode(MessageBase *pMessage)
    : CommandBase(pMessage), m_pConfig(Config::GetInstance()) {
  AddActionProc(MB_CONFERENCE_MODE, PM_GET_CONFERENCE_LIST,
                &ConferenceMode::GetConferenceListReply);
  AddActionProc(MB_CONFERENCE_MODE, PM_GET_CONFERENCE_FILES,
                &ConferenceMode::GetConferenceFilesReply);
  AddActionProc(MB_CONFERENCE_MODE, PM_SET_CONVERENCE_INFO,
                &ConferenceMode::SetConferenceInfoReply);
  AddActionProc(MB_MOBILE_MODE, PSL_GET_MOBILE_CONFERENCE_LIST,
                &ConferenceMode::GetMobileConferenceListReply);
  AddActionProc(MB_MOBILE_MODE, PSL_GET_MOBILE_CONFERENCE_FILES,
                &ConferenceMode::GetMobileConferenceFilesReply);
}

ConferenceMode::~ConferenceMode() {}

void ConferenceMode::GetConferenceList() {
  m_pMessage->sendMessage(MB_CONFERENCE_MODE, PM_GET_CONFERENCE_LIST,
                          QJsonObject());
}

void ConferenceMode::SetConferenceInfo(const QString &qstrConferenceUuid,
                                       const QString &qstrTitle,
                                       const QString &qstrContent,
                                       const QString &qstrMembers) {
  QJsonObject data;
  data.insert("conferenceUuid", qstrConferenceUuid);
  data.insert("title", qstrTitle);
  data.insert("content", qstrContent);
  data.insert("members", qstrMembers);
  m_pMessage->sendMessage(MB_CONFERENCE_MODE, PM_SET_CONVERENCE_INFO, data);
}

void ConferenceMode::SetConferenceInfoReply(bool bResult, const QJsonObject &jsData)
{
    m_pRecrodShared->receive_conferenceInfoSetted(bResult, jsData.toVariantMap());
}

void ConferenceMode::GetConferenceFiles(const QString &uuid) {
  QJsonObject data;
  data.insert("conferenceUuid", uuid);
  m_pMessage->sendMessage(MB_CONFERENCE_MODE, PM_GET_CONFERENCE_FILES, data);
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

void ConferenceMode::GetConferenceListReply(bool bResult, const QJsonObject &jsData) {
                                            const QJsonObject &jsData) {
  if (bResult) {
    QVariantList lsRecordInfoes = jsData["list"].toVariant().toList();
    foreach (const auto &varInfo, lsRecordInfoes) {
      m_pRecrodShared->AddConferenceRecordInfo(varInfo.toMap());
    }
  }
}

void ConferenceMode::GetConferenceFilesReply(bool bResult, const QJsonObject &jsData) {
  if (bResult) {
    QVariantList lsRecordFiles = jsData["list"].toVariant().toList();
    Q_EMIT getConferenceFiles(RecorderShared::RT_CONFERENCE, lsRecordFiles);
  }
}

void ConferenceMode::GetMobileConferenceListReply(bool bResult,
                                                  const QJsonObject &jsData) {
  if (bResult) {
    QVariantList lsRecordInfoes = jsData["list"].toVariant().toList();
    foreach (const auto &varInfo, lsRecordInfoes) {
      m_pRecrodShared->AddMobileRecordInfo(varInfo.toMap());
    }
  }
}

void ConferenceMode::GetMobileConferenceFilesReply(bool bResult,
                                                   const QJsonObject &jsData) {
  if (bResult) {
    QVariantList lsRecordFiles = jsData["list"].toVariant().toList();
    Q_EMIT getConferenceFiles(RecorderShared::RT_MOBILE, lsRecordFiles);
  }
}
