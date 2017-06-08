#pragma once

#include <service/messager/CommandModeBase.h>

class MessageBase;
class Personal : public CommandModeBase
{
public:
    Personal(MessageBase *pMessage);
    ~Personal();

    void GetPersonalList();
    void GetPersonalListReply(QString qstrAction, bool bResult, QJsonObject jsData);

private:
    void SendAction(const char *pAction,const QJsonObject &jsData);
};

