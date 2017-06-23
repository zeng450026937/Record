
#include "PersonalMode.h"
#include <QJsonArray>
#include <recorder_shared.h>
#include "service/messager/message_base.h"
#include "common/config.h"
#include "service/storage/personal_database_impl.h"
#include "service/storage/database_impl.h"
#include "service/conf_service_impl.h"

#define PM_GET_PERSONAL_LIST        "getPersonalList"
#define PM_GET_ALL_PERSONAL_LIST    "getAllPersonalList"

PersonalMode::PersonalMode(MessageBase *pMessage) : 
    CommandBase(pMessage),
    m_pConfig(Config::GetInstance()),
    m_pConfService(NULL),
    m_pRecordShared(nullptr)
{
    AddActionProc(MB_PERSONAL_MODE,PM_GET_PERSONAL_LIST, &PersonalMode::GetPersonalListReply);
    AddActionProc(MB_PERSONAL_MODE,PM_GET_ALL_PERSONAL_LIST, &PersonalMode::GetPersonalListReply);
}

PersonalMode::~PersonalMode()
{
}

void PersonalMode::GetPersonalList()
{
    QJsonObject jsData;
    jsData.insert("userId", m_pConfig->GetUser().user_id);
    m_pMessage->sendMessage("personal", PM_GET_PERSONAL_LIST, jsData);
}

void PersonalMode::GetAllPersoanlList()
{
    m_pMessage->sendMessage("personal", PM_GET_ALL_PERSONAL_LIST, QJsonObject());
}

void PersonalMode::GetPersonalListReply(bool bResult, const QJsonObject &jsData)
{
    if (bResult)
    {
        QJsonDocument jsDoc(jsData);
        QString qstr = jsDoc.toJson();
        QVariantList lsRecordInfo = jsData["list"].toVariant().toList();
        foreach(const auto &varInfo , lsRecordInfo)
        {
            m_pRecordShared->AddPersonalRecordInfo(varInfo.toMap());
        }
    }
}