#include "PersonalMode.h"
#include "service/messager/message_base.h"
#include "common/config.h"

#define PSL_GET_PERSONAL_LIST       "getPersonalList"
#define PSL_GET_ALL_PERSONAL_LIST   "getAllPersonalList"

PersonalMode::PersonalMode(MessageBase *pMessage) : 
    CommandModeBase(pMessage,"personal"),
    m_pConfig(Config::GetInstance())
{
    AddActionProc(PSL_GET_PERSONAL_LIST, &PersonalMode::GetPersonalListReply);
    AddActionProc(PSL_GET_ALL_PERSONAL_LIST, &PersonalMode::GetAllPersonalListReply);
}


PersonalMode::~PersonalMode()
{
}

void PersonalMode::GetPersonalList()
{
    QJsonObject jsData;
    jsData.insert("userId", m_pConfig->GetUser().user_id);
    SendAction(PSL_GET_PERSONAL_LIST, jsData);
}

void PersonalMode::GetAllPersoanlList()
{    
    SendAction(PSL_GET_ALL_PERSONAL_LIST, QJsonObject());
}
#include <QJsonDocument>
void PersonalMode::GetAllPersonalListReply(bool bResult, QJsonObject jsData)
{
    QJsonDocument jsDoc(jsData);
    QString qstr = jsDoc.toJson();

}

void PersonalMode::GetPersonalListReply(bool bResult, QJsonObject jsData)
{
    QJsonDocument jsDoc(jsData);
    QString qstr = jsDoc.toJson();
    if (bResult)
    {
        jsData["list"].toVariant().toMap();
    }
    else
    {

    }
}

void PersonalMode::SendAction(const char *pAction, const QJsonObject &jsData)
{
    m_pMessage->sendMessage("personal", pAction, jsData);
}
