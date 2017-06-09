#pragma once

#include <QString>
#include "../messager/CommandModeBase.h"

class MessageBase;
class InfoMode : public CommandModeBase
{
public:
	InfoMode(MessageBase *pMessager);
	~InfoMode();

    void ConnectToServer();

    void GetDeviceList();
protected:
    
private:
    void GetDeviceListReply(bool bResult, const QJsonObject &jsData);

    void HandleHeartBeat(bool bResult,const QJsonObject &jsData);

    void SendAction(const char *pAction, const QJsonObject &jsData);

private:
};

