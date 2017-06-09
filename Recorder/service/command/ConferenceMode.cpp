#include "ConferenceMode.h"

#include "service/messager/message_base.h"

#define PSL_GET_Conference_LIST "getConferenceList"

ConferenceMode::ConferenceMode(MessageBase *pMessage) : CommandModeBase(pMessage, "Conference")
{
    AddActionProc(PSL_GET_Conference_LIST, &ConferenceMode::GetConferenceListReply);
}

ConferenceMode::~ConferenceMode()
{
}

void ConferenceMode::GetConferenceList()
{
}

void ConferenceMode::GetConferenceListReply(QString qstrAction, bool bResult, QJsonObject jsData)
{
    if (bResult)
    {

    }
    else
    {

    }
}

void ConferenceMode::SendAction(const char *pAction, const QJsonObject &jsData)
{
    m_pMessage->sendMessage("Conference", pAction, jsData);
}
