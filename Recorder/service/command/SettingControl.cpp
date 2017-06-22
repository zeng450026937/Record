
#include "SettingControl.h"
#include <QJsonObject>

#include "common/config.h"
#include "recorder_shared.h"
#include "service/messager/message_base.h"

#define SC_ADD_TEMPLATE_INFO "addTemplateInfo"
#define SC_SET_TEMPLATE_INFO "setTemplateInfo"
#define SC_DELETE_TEMPLATE_INFO "deleteTemplateInfo"
#define SC_GET_TEMPLATE_INFO "getTemplateInfo"
#define SC_GET_TEMPLATE_LIST "getTemplateList"

SettingControl::SettingControl(RecorderShared *pRecorderShared, MessageBase *pMessager) :
    CommandBase(pMessager),m_pRecordShared(pRecorderShared)
{
    AddActionProc(MB_CONFERENCE_MODE, SC_ADD_TEMPLATE_INFO, &SettingControl::AddTemplateInfoReply);
    AddActionProc(MB_CONFERENCE_MODE, SC_SET_TEMPLATE_INFO, &SettingControl::SetTemplateinfoReply);
    AddActionProc(MB_CONFERENCE_MODE, SC_DELETE_TEMPLATE_INFO, &SettingControl::DeleteTemplateInfoReply);
    AddActionProc(MB_CONFERENCE_MODE, SC_GET_TEMPLATE_INFO, &SettingControl::GetTemplateInfoReply);
    AddActionProc(MB_CONFERENCE_MODE, SC_GET_TEMPLATE_LIST, &SettingControl::GetTemplateListReply);
}

SettingControl::~SettingControl() 
{

}

void SettingControl::AddTemplateInfo(const QString &qstrTitle,
    const QString &qstrContent, 
    const QString &qstrMembers)
{
    QJsonObject jsData;
    jsData.insert("title", qstrTitle);
    jsData.insert("content", qstrContent);
    jsData.insert("members", qstrMembers);
    
    m_pMessage->sendMessage(MB_CONFERENCE_MODE, SC_ADD_TEMPLATE_INFO, jsData);
}

void SettingControl::AddTemplateInfoReply(bool bResult, const QJsonObject &jsData)
{
    m_pRecordShared->receive_templateInfoAdded(bResult, jsData.toVariantMap());
}

void SettingControl::SetTemplateinfo(const QString &qstrTemplateUuid, const QString &qstrTitle, const QString &qstrContent, const QString &qstrMembers)
{
    QJsonObject jsData;
    jsData.insert("title", qstrTitle);
    jsData.insert("content", qstrContent);
    jsData.insert("members", qstrMembers);

    m_pMessage->sendMessage(MB_CONFERENCE_MODE, SC_SET_TEMPLATE_INFO, jsData);
}

void SettingControl::DeleteTemplateInfo(const QString &qstrTemplateUuid)
{
    QJsonObject jsData;
    jsData.insert("templateUuid", qstrTemplateUuid);
    m_pMessage->sendMessage(MB_CONFERENCE_MODE, SC_DELETE_TEMPLATE_INFO, jsData);
}

void SettingControl::GetTemplateInfo(const QString &qstrTemplateUuid)
{
    QJsonObject jsData;
    jsData.insert("templateUuid", qstrTemplateUuid);
    m_pMessage->sendMessage(MB_CONFERENCE_MODE, SC_GET_TEMPLATE_INFO, jsData);

}

void SettingControl::GetTemplateList()
{
    m_pMessage->sendMessage(MB_CONFERENCE_MODE, SC_GET_TEMPLATE_LIST, QJsonObject());
}

void SettingControl::SetTemplateinfoReply(bool bResult, const QJsonObject &jsData)
{
    m_pRecordShared->receive_templateInfoSetted(bResult, jsData.toVariantMap());
}

void SettingControl::DeleteTemplateInfoReply(bool bResult, const QJsonObject &jsData)
{
    m_pRecordShared->receive_templateInfoDeleted(bResult, jsData.toVariantMap());
}

void SettingControl::GetTemplateInfoReply(bool bResult, const QJsonObject &jsData)
{
    m_pRecordShared->receive_templateInfoGot(bResult, jsData.toVariantMap());
}

void SettingControl::GetTemplateListReply(bool bResult, const QJsonObject &jsData)
{
    m_pRecordShared->receiver_templateInfoList(bResult,jsData["list"].toVariant().toList());
}
