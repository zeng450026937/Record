
#include "message_base.h"
#include <assert.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include "CommandModeBase.h"
#include "message_base_private.h"

#include "websocketclient.h"
#include "network_info.h"

MessageBase::MessageBase(QObject *parent) :
    QObject(parent),
    d(new MessageBasePrivate)
{
    qRegisterMetaType<QByteArray>();
    connect(d->_client,SIGNAL(binary_message(uint,QByteArray)),this,SLOT(on_binary_message(uint,QByteArray)),Qt::QueuedConnection);
    connect(d->_client,SIGNAL(text_message(QString)),this,SLOT(on_message_reply(QString)),Qt::QueuedConnection);
    connect(d->_client,SIGNAL(connect_open()),this,SLOT(on_connect_open()),Qt::QueuedConnection);
    connect(d->_client,SIGNAL(connect_fail()),this,SLOT(on_connect_fail()),Qt::QueuedConnection);
    connect(d->_client,SIGNAL(connect_close()),this,SLOT(on_connect_close()),Qt::QueuedConnection);
}

MessageBase::~MessageBase()
{
    delete d;
}

bool MessageBase::sendMessage(const QString &qstrMode, const QString &qstrAction, const QJsonObject &jsData)
{
	QJsonObject jsRoot;
	jsRoot.insert("version", MB_MESSAGE_VERSION);
	jsRoot.insert("data", jsData);

    QJsonObject jsCommand;
    jsCommand.insert("mode", qstrMode);
	jsCommand.insert("action", qstrAction);
	jsRoot.insert("command", jsCommand);

	QJsonDocument jsonDocument(jsRoot);
	jsonDocument.setObject(jsRoot);	
	QString qstrJson = QString::fromUtf8(jsonDocument.toJson(QJsonDocument::Compact));
	d->_client->sendText(qstrJson.toLocal8Bit().data());

	return true;
}

void MessageBase::on_connect_open()
{
    emit notify_command(ConnectOpen, true, QVariantMap());
}
void MessageBase::on_connect_fail()
{
    emit notify_command(ConnectFail, true, QVariantMap());
}
void MessageBase::on_connect_close()
{
    emit notify_command(ConnectClose, true, QVariantMap());
}
void MessageBase::on_message_reply(QString qstrMessage)
{
	QJsonParseError error;
	QJsonDocument jsonDocument = QJsonDocument::fromJson(qstrMessage.toUtf8(), &error);
	if (error.error == QJsonParseError::NoError) 
    {
        QJsonObject jsRoot = jsonDocument.object();
        if (jsRoot["version"].toInt() == MB_MESSAGE_VERSION)
        {
            QJsonObject jsCommand = jsRoot["command"].toObject();
            auto itrFound = d->mapMode.find(jsCommand["mode"].toString());
            if (itrFound == d->mapMode.end())
            {
                // 所有不符合规则的响应都会在这里结束，包括 不存在"command" json对象时。
                qDebug() << jsCommand["mode"].toString() << " command mode is not found." << error.errorString();
                return;
            }

           emit itrFound->second->action_trigger(jsCommand["action"].toString(),
                                                   jsRoot["result"].toBool(),
                                                   jsRoot["data"].toObject());
        }
        else
        {
            // TODO: 提示协议与服务端通讯协议版本不匹配。
        }
	}
	else {
		qDebug() << "json command parse failed" << error.errorString();
	}
}
void MessageBase::on_binary_message(unsigned int size,QByteArray content)
{
	emit notify_binary(size, content);
}

void MessageBase::connectTo(const QString &qstrHeader, QString uri)
{
    d->_client->connect_to(qstrHeader.toStdString(),uri.toStdString());
}
void MessageBase::stopConnection()
{
    d->_client->stop_connect();
}

void MessageBase::sendCommand(CommandList command, QString receiver, const QVariantMap& data)
{
    QVariantMap json_command;

    json_command.insert("SENDER",d->_info->MAC());
    json_command.insert("RECEIVER",receiver);
    json_command.insert("COMMAND",d->_command_map.key(command));
    json_command.insert("DATA",data);

    QJsonDocument jsonDocument  = QJsonDocument::fromVariant(json_command);

    d->_client->sendText( jsonDocument.toJson().toStdString() );
}


void MessageBase::AddMode(const QString &qstrModeName, CommandModeBase &commandMode)
{
    assert(d->mapMode.find(qstrModeName) == d->mapMode.end());
    d->mapMode.insert(std::map<QString, CommandModeBase *>::value_type(QString(qstrModeName), &commandMode));
}
