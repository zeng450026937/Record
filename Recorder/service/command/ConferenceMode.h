#pragma once

#include <service/messager/CommandModeBase.h>

class MessageBase;
class ConferenceMode : public CommandModeBase
{
public:
    ConferenceMode(MessageBase *pMessage);
    ~ConferenceMode();

    void GetConferenceList();
    void GetConferenceListReply(QString qstrAction, bool bResult, QJsonObject jsData);

private:
    void SendAction(const char *pAction, const QJsonObject &jsData);
};

