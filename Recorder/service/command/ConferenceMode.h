#pragma once

#include <service/messager/CommandModeBase.h>

class Config;
class MessageBase;
class ConferenceMode : public CommandModeBase
{
    friend class ServiceThread;
    ConferenceMode(MessageBase *pMessage);

public:
    ~ConferenceMode();

    void GetConferenceList();
    void GetTemplateList();

private:
    void GetTemplateListReply(bool bResult, QJsonObject jsData);
    void GetConferenceListReply(bool bResult, QJsonObject jsData);
    void SendAction(const char *pAction, const QJsonObject &jsData);

    Config *m_pConfig;
};

