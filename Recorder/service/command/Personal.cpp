#include "Personal.h"
#include "service/messager/message_base.h"

#define PSL_GET_PERSONAL_LIST "getPersonalList"

Personal::Personal(MessageBase *pMessage) : CommandModeBase(pMessage,"personal")
{
    AddActionProc(PSL_GET_PERSONAL_LIST, &Personal::GetPersonalListReply);
}


Personal::~Personal()
{
}

void Personal::GetPersonalList()
{

}

void Personal::GetPersonalListReply(QString qstrAction, bool bResult, QJsonObject jsData)
{
    if (bResult)
    {
    }
    else
    {

    }
}

void Personal::SendAction(const char *pAction, const QJsonObject &jsData)
{
    m_pMessage->sendMessage("personal", pAction, jsData);
}
