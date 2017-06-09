#include "PersonalMode.h"
#include "service/messager/message_base.h"

#define PSL_GET_PERSONAL_LIST "getPersonalList"

PersonalMode::PersonalMode(MessageBase *pMessage) : CommandModeBase(pMessage,"personal")
{
    AddActionProc(PSL_GET_PERSONAL_LIST, &PersonalMode::GetPersonalListReply);
}


PersonalMode::~PersonalMode()
{
}

void PersonalMode::GetPersonalList()
{

}

void PersonalMode::GetPersonalListReply(QString qstrAction, bool bResult, QJsonObject jsData)
{
    if (bResult)
    {
    }
    else
    {

    }
}

void PersonalMode::SendAction(const char *pAction, const QJsonObject &jsData)
{
    m_pMessage->sendMessage("personal", pAction, jsData);
}
