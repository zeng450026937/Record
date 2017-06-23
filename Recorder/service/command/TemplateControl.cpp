
#include "TemplateControl.h"
#include <QJsonObject>

#include "common/config.h"
#include "recorder_shared.h"
#include "service/messager/message_base.h"

#define TC_ADD_TEMPLATE_INFO "addTemplateInfo"
#define TC_SET_TEMPLATE_INFO "setTemplateInfo"
#define TC_DELETE_TEMPLATE_INFO "deleteTemplateInfo"
#define TC_GET_TEMPLATE_INFO "getTemplateInfo"
#define TC_GET_TEMPLATE_LIST "getTemplateList"

TemplateControl::TemplateControl(RecorderShared *pRecorderShared,
                                 MessageBase *pMessager)
    : CommandBase(pMessager), m_pRecordShared(pRecorderShared) {
  AddActionProc(MB_CONFERENCE_MODE, TC_ADD_TEMPLATE_INFO,
                &TemplateControl::AddTemplateInfoReply);
  AddActionProc(MB_CONFERENCE_MODE, TC_SET_TEMPLATE_INFO,
                &TemplateControl::SetTemplateinfoReply);
  AddActionProc(MB_CONFERENCE_MODE, TC_DELETE_TEMPLATE_INFO,
                &TemplateControl::DeleteTemplateInfoReply);
  AddActionProc(MB_CONFERENCE_MODE, TC_GET_TEMPLATE_INFO,
                &TemplateControl::GetTemplateInfoReply);
  AddActionProc(MB_CONFERENCE_MODE, TC_GET_TEMPLATE_LIST,
                &TemplateControl::GetTemplateListReply);
}

TemplateControl::~TemplateControl() {}

void TemplateControl::AddTemplateInfo(const QString &qstrTitle,
                                      const QString &qstrContent,
                                      const QString &qstrMembers) {
  QJsonObject jsData;
  jsData.insert("title", qstrTitle);
  jsData.insert("content", qstrContent);
  jsData.insert("members", qstrMembers);

  m_pMessage->sendMessage(MB_CONFERENCE_MODE, TC_ADD_TEMPLATE_INFO, jsData);
}

void TemplateControl::AddTemplateInfoReply(bool bResult,
                                           const QJsonObject &jsData) {
  m_pRecordShared->receive_templateInfoAdded(bResult, jsData.toVariantMap());
}

void TemplateControl::SetTemplateinfo(const QString &qstrTemplateUuid,
                                      const QString &qstrTitle,
                                      const QString &qstrContent,
                                      const QString &qstrMembers) {
  QJsonObject jsData;
  jsData.insert("templateUuid", qstrTemplateUuid);
  jsData.insert("title", qstrTitle);
  jsData.insert("content", qstrContent);
  jsData.insert("members", qstrMembers);

  m_pMessage->sendMessage(MB_CONFERENCE_MODE, TC_SET_TEMPLATE_INFO, jsData);
}

void TemplateControl::SetTemplateinfoReply(bool bResult,
                                           const QJsonObject &jsData) {
  m_pRecordShared->receive_templateInfoSetted(bResult, jsData.toVariantMap());
}

void TemplateControl::DeleteTemplateInfo(const QString &qstrTemplateUuid) {
  QJsonObject jsData;
  jsData.insert("templateUuid", qstrTemplateUuid);
  m_pMessage->sendMessage(MB_CONFERENCE_MODE, TC_DELETE_TEMPLATE_INFO, jsData);
}

void TemplateControl::DeleteTemplateInfoReply(bool bResult,
                                              const QJsonObject &jsData) {
  m_pRecordShared->receive_templateInfoDeleted(bResult, jsData.toVariantMap());
}

void TemplateControl::GetTemplateInfo(const QString &qstrTemplateUuid) {
  QJsonObject jsData;
  jsData.insert("templateUuid", qstrTemplateUuid);
  m_pMessage->sendMessage(MB_CONFERENCE_MODE, TC_GET_TEMPLATE_INFO, jsData);
}

void TemplateControl::GetTemplateInfoReply(bool bResult,
                                           const QJsonObject &jsData) {
  m_pRecordShared->receive_templateInfoGot(bResult, jsData.toVariantMap());
}

void TemplateControl::GetTemplateList() {
  QJsonObject jsData;
  m_pMessage->sendMessage(MB_CONFERENCE_MODE, TC_GET_TEMPLATE_LIST, jsData);
}

void TemplateControl::GetTemplateListReply(bool bResult,
                                           const QJsonObject &jsData) {
  m_pRecordShared->receiver_templateInfoList(
      bResult, jsData["list"].toVariant().toList());
}
