#pragma once

#include <service/messager/CommandModeBase.h>

class Config;
class MessageBase;
class PersonalMode : public CommandModeBase
{
    friend class ServiceThread;

    PersonalMode(MessageBase *pMessage);
public:
    ~PersonalMode();

    void GetPersonalList();
    void GetAllPersoanlList();

private:
    void GetAllPersonalListReply(bool bResult, QJsonObject jsData);
    void GetPersonalListReply(bool bResult, QJsonObject jsData);
    void SendAction(const char *pAction,const QJsonObject &jsData);

    Config *m_pConfig;
};

