#include "Conference.h"

#include "service/messager/message_base.h"

#define PSL_GET_Conference_LIST "getConferenceList"

Conference::Conference(MessageBase *pMessage) : CommandModeBase(pMessage, "Conference")
{
    AddActionProc(PSL_GET_Conference_LIST, &Conference::GetConferenceListReply);
}

Conference::~Conference()
{
}

void Conference::GetConferenceList()
{
}

void Conference::GetConferenceListReply(QString qstrAction, bool bResult, QJsonObject jsData)
{
    if (bResult)
    {
    }
    else
    {

    }
}

void Conference::SendAction(const char *pAction, const QJsonObject &jsData)
{
    m_pMessage->sendMessage("Conference", pAction, jsData);
}
