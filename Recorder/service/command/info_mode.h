#pragma once

#include "../messager/CommandBase.h"

class MessageBase;
class InfoMode : public CommandBase
{
public:
	InfoMode(MessageBase *pMessager);
	~InfoMode();

    void ConnectToServer();

protected:
    
private:
    void HandleHeartBeat(bool bResult,const QJsonObject &jsData);
};

