#pragma once

#include <QString>
#include "../messager/message_base.h"

class MessageBase;
class InfoMode : ICommandMode
{
public:
	InfoMode(MessageBase *pMessager);
	~InfoMode();

	void ConnectToServer(const QString &qstrHeader,const QString &qstrServerAddress);
protected:
	virtual void on_action_trigger(QJsonObject jsRoot) override;


private:
	MessageBase  *_messager;
};

