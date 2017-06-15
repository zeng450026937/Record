#include "ConferenceMode.h"

#include <QFile>
#include <QJsonArray>
#include <QDebug>
#include <recorder_shared.h>
#include "common/config.h"
#include "service/messager/message_base.h"
#include "storage/include/clip_file_database.h"
#include "storage/include/conferencedatabase.h"
#include "storage/include/download_database.h"
#include "storage/include/templatedatabase.h"
#include "service/conf_service_impl.h"
#include "storage/database_impl.h"

#define PSL_GET_CONFERENCE_LIST "getConferenceList"
#define CM_GET_TEMPLATE_LIST    "getTemplateList"
// #define CM_DOWNLOAD_FILE        "downloadFile"

ConferenceMode::ConferenceMode(MessageBase *pMessage) : 
    CommandModeBase(pMessage, "conference"),
    m_pConfig(Config::GetInstance()),
    m_pClipFileDB(NULL),
    m_pDownloadDB(NULL),
    m_pTemplateDB(nullptr)
{
    AddActionProc(PSL_GET_CONFERENCE_LIST, &ConferenceMode::GetConferenceListReply);
    AddActionProc(CM_GET_TEMPLATE_LIST, &ConferenceMode::GetTemplateListReply);
}

ConferenceMode::~ConferenceMode()
{
}

void ConferenceMode::SetDataBase(DataBase_Impl *pDatabase)
{
    m_pClipFileDB = ClipFileDatabase::GetInterface(pDatabase);
    m_pDownloadDB = DownloadDatabase::GetInterface(pDatabase);
    m_pTemplateDB = TemplateDatabase::GetInterface(pDatabase);
}

void ConferenceMode::GetConferenceList()
{
    SendAction(PSL_GET_CONFERENCE_LIST, QJsonObject());
}

void ConferenceMode::GetTemplateList()
{
    SendAction(CM_GET_TEMPLATE_LIST, QJsonObject());
}

void ConferenceMode::GetTemplateListReply(bool bResult, QJsonObject jsData)
{
    if (bResult)
    {
        QVariantList lsTemplateInfoes = jsData["list"].toVariant().toList();
        foreach(const auto &varInfo, lsTemplateInfoes)
        {
            m_pTemplateDB->AddTemplate(varInfo.toMap());
        }
    }

}

void ConferenceMode::GetConferenceListReply(bool bResult, QJsonObject jsData)
{
    if (bResult)
    {
        QVariantList lsRecordInfoes = jsData["list"].toVariant().toList();
        foreach(const auto &varInfo, lsRecordInfoes)
        {
            m_pRecrodShared->AddConferenceRecordInfo(varInfo.toMap());
        }
    }
}

void ConferenceMode::SendAction(const char *pAction, const QJsonObject &jsData)
{
    m_pMessage->sendMessage("conference", pAction, jsData);
}
