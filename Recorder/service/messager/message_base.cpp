
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include "message_base.h"
#include "message_base_private.h"

#include "websocketclient.h"
#include "network_info.h"


MessageBase::MessageBase(QObject *parent) :
    QObject(parent),
    d(new MessageBasePrivate)
{
    qRegisterMetaType<QByteArray>();
    connect(d->_client,SIGNAL(binary_message(uint,QByteArray)),this,SLOT(on_binary_message(uint,QByteArray)),Qt::QueuedConnection);
    connect(d->_client,SIGNAL(text_message(QString)),this,SLOT(on_text_message(QString)),Qt::QueuedConnection);
    connect(d->_client,SIGNAL(connect_open()),this,SLOT(on_connect_open()),Qt::QueuedConnection);
    connect(d->_client,SIGNAL(connect_fail()),this,SLOT(on_connect_fail()),Qt::QueuedConnection);
    connect(d->_client,SIGNAL(connect_close()),this,SLOT(on_connect_close()),Qt::QueuedConnection);
}

MessageBase::~MessageBase()
{
    delete d;
}

bool MessageBase::sendMessage(const QString &qstrMode, const QString &qstrAction, const QVariantMap &qvmData)
{
	QJsonObject jsObj;
	jsObj.insert("mode", qstrMode);
	jsObj.insert("action", qstrAction);
	jsObj.insert("data", QJsonObject::fromVariantMap(qvmData));

	QJsonDocument jsonDocument;
	jsonDocument.setObject(jsObj);	
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
void MessageBase::on_text_message(QString text)
{
    this->parseText(text);
}
void MessageBase::on_binary_message(unsigned int size,QByteArray content)
{
    emit notify_binary(size, content);
}

void MessageBase::parseText(const QString text)
{
    QJsonParseError error;
    QJsonDocument jsonDocument  = QJsonDocument::fromJson(text.toUtf8(), &error);

    if(error.error == QJsonParseError::NoError){
        if(jsonDocument .isObject()){
            QVariantMap content = jsonDocument.toVariant().toMap();
            QString result = content.value("RESULT").toString();
            QVariant data = content.value("DATA");

            MessageBase::CommandList command = (MessageBase::CommandList)(-1);
            command = d->_command_map.value(content.value("COMMAND").toString(),(MessageBase::CommandList)(-1));

            emit notify_command(command, (result=="true")?true:false, data.toMap());
        }
    }
    else{
        qDebug()<<"json command parse failed"<<error.errorString();
    }
}

void MessageBase::connectTo(QString uri)
{
    d->_client->connect_to("",uri.toStdString());
}
void MessageBase::stopConnection()
{
    d->_client->stop_connect();
}
void MessageBase::userLogin(QString account, QString password)
{
    QVariantMap param;
    param.insert("user_id",account);
    param.insert("user_name",d->_info->HOSTNAME());
    param.insert("user_nick_name",d->_info->HOSTNAME());
    param.insert("user_pass",password);
    param.insert("device_mac",d->_info->MAC());
    param.insert("device_type","PC");
    param.insert("device_name",d->_info->HOSTNAME());
    param.insert("battery",(int)-1);
    param.insert("battery_time",(int)-1);
    param.insert("status","online");
    param.insert("display_name",d->_info->HOSTNAME());

    this->sendCommand(MessageBase::LoginDevice, "", param);
}

void MessageBase::heartBeat()
{
    QVariantMap param;

    param.insert("device_mac",d->_info->MAC());

    this->sendCommand(MessageBase::HeartBeat, "", param);
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

