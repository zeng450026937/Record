#pragma once

#include <QString>
#include "../messager/CommandModeBase.h"

class MessageBase;
class InfoMode : public CommandModeBase
{
public:
	InfoMode(MessageBase *pMessager);
	~InfoMode();

	void ConnectToServer(const QString &qstrHeader,const QString &qstrServerAddress);
protected:
    
private:

    void HandleHeartBeat(bool bResult,const QJsonObject &jsData);

    void SendAction(const char *pAction, const QJsonObject &jsData);

private:
};

