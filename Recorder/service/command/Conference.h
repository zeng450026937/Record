#pragma once

#include <service/messager/CommandModeBase.h>

class MessageBase;
class Conference : public CommandModeBase
{
public:
    Conference(MessageBase *pMessage);
    ~Conference();

    void GetConferenceList();
    void GetConferenceListReply(QString qstrAction, bool bResult, QJsonObject jsData);

private:
    void SendAction(const char *pAction, const QJsonObject &jsData);
};

