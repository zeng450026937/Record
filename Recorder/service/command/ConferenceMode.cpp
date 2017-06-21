#include "ConferenceMode.h"

#include <recorder_shared.h>
#include "common/config.h"
#include "service/messager/message_base.h"

#define PSL_GET_CONFERENCE_LIST "getConferenceList"

ConferenceMode::ConferenceMode(MessageBase *pMessage) : 
    CommandBase(pMessage),
    m_pConfig(Config::GetInstance())
{
    AddActionProc(MB_CONFERENCE_MODE,PSL_GET_CONFERENCE_LIST, &ConferenceMode::GetConferenceListReply);
   
}

ConferenceMode::~ConferenceMode()
{}

void ConferenceMode::GetConferenceList()
{
    m_pMessage->sendMessage(MB_CONFERENCE_MODE, PSL_GET_CONFERENCE_LIST, QJsonObject());
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
