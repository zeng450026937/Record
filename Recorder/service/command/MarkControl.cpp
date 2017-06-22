
#include "MarkControl.h"
#include <service\storage\include\markdatabase.h>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include "common/config.h"
#include "recorder_shared.h"
#include "service/messager/message_base.h"
#include "service/service_thread.h"

#define MC_ADD_MARK_INFO "addMarkInfo"
#define MC_SET_MARK_INFO "setMarkInfo"
#define MC_DELETE_MARK_INFO "delMarkInfo"
#define MC_GET_MARK_INFO "getMarkinfo"
#define MC_GET_MARK_LIST "getMarkList"

MarkControl::MarkControl(MarkDatabase *pDB, MessageBase *pMessager)
    : CommandBase(pMessager), m_pMarkDB(pDB) {
  AddActionProc(MB_CONFERENCE_MODE, MC_ADD_MARK_INFO,
                &MarkControl::AddMarkInfoReplay);
  AddActionProc(MB_CONFERENCE_MODE, MC_SET_MARK_INFO,
                &MarkControl::SetMarkInfoReplay);
  AddActionProc(MB_CONFERENCE_MODE, MC_DELETE_MARK_INFO,
                &MarkControl::DeleteConferenceMarkReplay);
  AddActionProc(MB_CONFERENCE_MODE, MC_GET_MARK_INFO,
                &MarkControl::GetConferenceMarkReplay);
  AddActionProc(MB_CONFERENCE_MODE, MC_GET_MARK_LIST,
                &MarkControl::GetAllConferenceMarkListReplay);
}

MarkControl::~MarkControl() {}

void MarkControl::AddMarkInfo(const QString &qstrConferenceUuid,
                              const QString &qstrContent, int iTime) {
  QJsonObject jsData;
  jsData.insert("conferenceUuid", qstrConferenceUuid);
  jsData.insert("content", qstrContent);
  jsData.insert("time", iTime);
  m_pMessage->sendMessage(MB_CONFERENCE_MODE, MC_ADD_MARK_INFO, jsData);
}

void MarkControl::AddMarkInfoReplay(bool bResult, const QJsonObject &jsData) {
  m_pMarkDB->AddMark(jsData.toVariantMap());
}

void MarkControl::SetMarkInfo(const QString &qstrMarkUuid,
                              const QString &qstrContent, int iTime) {
  QJsonObject jsData;
  jsData.insert("markUuid", qstrMarkUuid);
  jsData.insert("content", qstrContent);
  jsData.insert("time", iTime);
  m_pMessage->sendMessage(MB_CONFERENCE_MODE, MC_SET_MARK_INFO, jsData);
}

void MarkControl::SetMarkInfoReplay(bool bResult, const QJsonObject &jsData) {
  m_pMarkDB->AddMark(jsData.toVariantMap());
}

void MarkControl::DeleteConferenceMark(const QString &qstrConferenceUuid,
                                       int iIndex) {
  QJsonObject jsData;
  jsData.insert("conferenceUuid", qstrConferenceUuid);
  jsData.insert("index", iIndex);
  m_pMessage->sendMessage(MB_CONFERENCE_MODE, MC_DELETE_MARK_INFO, jsData);
}

void MarkControl::DeleteConferenceMarkReplay(bool bResult,
                                             const QJsonObject &jsData) {
  if (bResult) {
    // 有indexl
    // m_pMarkDB->DelMark(jsData["conferenceUuid"].toString());
  }
}

void MarkControl::GetConferenceMark(const QString &qstrConferenceUuid) {
  QJsonObject jsData;
  jsData.insert("conferenceUuid", qstrConferenceUuid);
  m_pMessage->sendMessage(MB_CONFERENCE_MODE, MC_GET_MARK_INFO, jsData);
}

void MarkControl::GetConferenceMarkReplay(bool bResult,
                                          const QJsonObject &jsData) {
  if (bResult) m_pMarkDB->AddMark(jsData.toVariantMap());
}

void MarkControl::GetAllConferenceMarkList() {
  m_pMessage->sendMessage(MB_CONFERENCE_MODE, MC_GET_MARK_LIST, QJsonObject());
}

void MarkControl::GetAllConferenceMarkListReplay(bool bResult,
                                                 const QJsonObject &jsData) {
  if (bResult) {
    QVariantList list = jsData["list"].toVariant().toList();
    foreach (QVariant var, list) { m_pMarkDB->AddMark(var.toMap()); }
  }
}
