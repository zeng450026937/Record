#include "ConferenceMode.h"

#include "common/config.h"
#include "service/messager/message_base.h"

#define PSL_GET_CONFERENCE_LIST "getConferenceList"
#define PSL_GET_TEMPLATE_LIST "getTemplateList"

ConferenceMode::ConferenceMode(MessageBase *pMessage) : 
    CommandModeBase(pMessage, "conference"),
    m_pConfig(Config::GetInstance())
{
    AddActionProc(PSL_GET_CONFERENCE_LIST, &ConferenceMode::GetConferenceListReply);
    AddActionProc(PSL_GET_TEMPLATE_LIST, &ConferenceMode::GetTemplateListReply);
}

ConferenceMode::~ConferenceMode()
{
}

void ConferenceMode::GetConferenceList()
{
    SendAction(PSL_GET_CONFERENCE_LIST, QJsonObject());
}

void ConferenceMode::GetTemplateList()
{
    SendAction(PSL_GET_TEMPLATE_LIST, QJsonObject());
}

#include <QJsonDocument>
void ConferenceMode::GetTemplateListReply(bool bResult, QJsonObject jsData)
{

    QJsonDocument jsDoc(jsData);
    QString qstr = jsDoc.toJson();
    if (bResult)
    {

    }
    else
    {

    }
}

void ConferenceMode::GetConferenceListReply(bool bResult, QJsonObject jsData)
{
    QJsonDocument jsDoc(jsData);
    QString qstr = jsDoc.toJson();
    if (bResult)
    {

    }
    else
    {

    }
}

void ConferenceMode::SendAction(const char *pAction, const QJsonObject &jsData)
{
    m_pMessage->sendMessage("conference", pAction, jsData);
}
