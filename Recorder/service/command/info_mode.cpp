
#include "info_mode.h"
#include <QJsonDocument>
#include <QJsonObject>
#include "../service/service_thread.h"
#include "../service/messager/message_base.h"


InfoMode::InfoMode(MessageBase *pMessager) : _messager(pMessager)
{
	_messager->AddMode("info", *this);
}


InfoMode::~InfoMode()
{
}

void InfoMode::ConnectToServer(const QString &qstrHeader, const QString &qstrServerAddress)
{
	_messager->connectTo(qstrHeader, qstrServerAddress);
}

void InfoMode::on_action_trigger(QJsonObject jsRoot)
{
	QJsonDocument jsDoc(jsRoot);
	QString qstr = jsDoc.toJson();
	qstr = qstr;
}
