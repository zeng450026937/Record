#pragma once

#include <service/messager/CommandModeBase.h>

class MessageBase;
class PersonalMode : public CommandModeBase
{
public:
    PersonalMode(MessageBase *pMessage);
    ~PersonalMode();

    void GetPersonalList();
    void GetPersonalListReply(QString qstrAction, bool bResult, QJsonObject jsData);

private:
    void SendAction(const char *pAction,const QJsonObject &jsData);
};

