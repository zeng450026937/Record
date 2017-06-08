#include "user_service_impl.h"
#include "service_thread_private.h"
#include "service_thread.h"
#include "messager/message_base.h"
#include "account/account_center.h"
#include "white_list.h"
#include "common/config.h"

#include "../command/info_mode.h"
#include "storage/include/user_database.h"

#include <QTimerEvent>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

UserServiceImpl::UserServiceImpl(ServiceThread* pService, QObject *parent) :
	_service(pService),
    _private(pService->_private),
    _warning(true),
    _connect_monitor(0)
{
	_message = _private->Messager();
	_account = _private->Account();

    QObject::connect(_message,SIGNAL(notify_command(int,bool,QVariantMap&)),this,SLOT(parseCommand(int,bool,QVariantMap&)),Qt::DirectConnection);
    QObject::connect(_account,SIGNAL(loginResult(bool, const QString&)),this,SLOT(logined(bool, const QString&)),Qt::DirectConnection);
    // QObject::connect(_account,SIGNAL(logoutResult(QString,bool,QString)),this,SLOT(logouted(QString,bool,QString)),Qt::DirectConnection);
}

void UserServiceImpl::stopConnection()
{
    _message->stopConnection();
    _uri.clear();
}

void UserServiceImpl::userLogin(QString account, QString password)
{
    _account->UserLogin(account, password);
}

void UserServiceImpl::parseCommand(int command, bool result, QVariantMap& info)
{
    switch(command){
    //common command
    case MessageBase::ConnectOpen:
        emit loginResult(QString());
        _warning = true;
        break;
    case MessageBase::ConnectFail:
		if (_warning) {
			emit loginResult(QObject::tr("连接服务失败"));
            _warning = false;
        }
		_message->stopConnection();
		DoWebsocketConnect();
        break;
    case MessageBase::ConnectClose:
		if (_warning) {
			emit loginResult(QObject::tr("服务器断开连接"));
            _warning = false;
        }
		_message->stopConnection();
		DoWebsocketConnect();
        break;
//     case MessageBase::HeartBeat:
//         _message->heartBeat();
//         break;
//     case MessageBase::LoginDevice:
//         this->killTimer(_connect_monitor);
//         _connect_monitor = 0;
//         emit userLogined(result, info.value("message","").toString());
//         break;
    }
}

void UserServiceImpl::logined(bool ok, const QString& reason)
{
    if(ok)
	{
		DoWebsocketConnect();
        if(_connect_monitor)
		{
            this->killTimer(_connect_monitor);
            _connect_monitor = 0;
        }
        _connect_monitor = this->startTimer(10000);
    }
    else{
        emit userLogined(false, reason);
    }
}
//void UserServiceImpl::logouted(QString account, bool ok, QString reason)
//{
//    Q_UNUSED(account);
//
//    emit userLogouted(ok, reason);
//}

void UserServiceImpl::timerEvent(QTimerEvent *e)
{
    if(_connect_monitor = e->timerId()){

        emit userLogined(false, QObject::tr("服务器无响应"));

        this->killTimer(_connect_monitor);
        _connect_monitor = 0;
    }
}

void UserServiceImpl::DoWebsocketConnect()
{
    Config *pConfig = Config::GetInstance();
    Config::USER &user = pConfig->GetUser();

	QJsonObject jsCommand;
	jsCommand.insert("mode", "auth");
	jsCommand.insert("action", "login");

	QJsonObject jsData;
	jsData.insert("userId", user.user_id);
	jsData.insert("userGroup", user.user_group);
	jsData.insert("userName", user.user_name);
	jsData.insert("deviceType", pConfig->DEVICE_TYPE);
	jsData.insert("deviceUuid", pConfig->_device_uuid);

	QJsonObject jsRoot;
	jsRoot.insert("version", MB_MESSAGE_VERSION);

	jsRoot.insert("from", user.user_id);
	jsRoot.insert("command", jsCommand);
	jsRoot.insert("data", jsData);

	QJsonDocument jsDoc(jsRoot);
	_service->GetInfoMode()->ConnectToServer(jsDoc.toJson(), pConfig->_server_address);
}
