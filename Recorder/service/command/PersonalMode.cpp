
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
    CommandModeBase(pMessage,"personal"),
    m_pConfig(Config::GetInstance()),
    m_pConfService(NULL),
    m_pRecordShared(nullptr)
{
    AddActionProc(PM_GET_PERSONAL_LIST, &PersonalMode::GetPersonalListReply);
    AddActionProc(PM_GET_ALL_PERSONAL_LIST, &PersonalMode::GetPersonalListReply);
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

void PersonalMode::GetPersonalListReply(bool bResult, QJsonObject jsData)
{
    if (bResult)
    {
        QVariantList lsRecordInfo = jsData["list"].toVariant().toList();
        foreach(const auto &varInfo , lsRecordInfo)
        {
            m_pRecordShared->AddPersonalRecordInfo(varInfo.toMap());
        }
    }
}