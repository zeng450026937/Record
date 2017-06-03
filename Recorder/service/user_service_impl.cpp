#include "user_service_impl.h"
#include "service_thread_private.h"
#include "messager/message_base.h"
#include "account/account_center.h"
#include "white_list.h"
#include "common/config.h"

#include "storage/include/user_database.h"

#include <QTimerEvent>
#include <QDebug>

UserServiceImpl::UserServiceImpl(ServiceThreadPrivate* shared, QObject *parent) :
    _shared(shared),
    _warning(true),
    _connect_monitor(0),
    UserService(parent)
{
    QObject::connect(_shared->Messager(),SIGNAL(notify_command(int,bool,QVariantMap&)),this,SLOT(parseCommand(int,bool,QVariantMap&)),Qt::DirectConnection);

    QObject::connect(_shared->Account(),SIGNAL(loginResult(QString,bool,QString)),this,SLOT(logined(QString,bool,QString)),Qt::DirectConnection);
    QObject::connect(_shared->Account(),SIGNAL(logoutResult(QString,bool,QString)),this,SLOT(logouted(QString,bool,QString)),Qt::DirectConnection);
}

void UserServiceImpl::connectTo(QString uri)
{
    _uri = uri;
    _shared->Messager()->connectTo(_uri);
}
void UserServiceImpl::stopConnection()
{
    _shared->Messager()->stopConnection();
    _uri.clear();
}
void UserServiceImpl::userLogin(QString account, QString password)
{
    _shared->Account()->UserLogin(account, password);
}

void UserServiceImpl::parseCommand(int command, bool result, QVariantMap& info)
{
    switch(command){
    //common command
    case MessageBase::ConnectOpen:
        emit connectOpened();
        _warning = true;
        break;
    case MessageBase::ConnectFail:
        if(_warning){
            emit connectFailed();
            _warning = false;
        }
        _shared->Messager()->stopConnection();
        _shared->Messager()->connectTo(_uri);
        break;
    case MessageBase::ConnectClose:
        if(_warning){
            emit connectClosed();
            _warning = false;
        }
        _shared->Messager()->stopConnection();
        _shared->Messager()->connectTo(_uri);
        break;
    case MessageBase::HeartBeat:
        _shared->Messager()->heartBeat();
        break;
    case MessageBase::LoginDevice:
        this->killTimer(_connect_monitor);
        _connect_monitor = 0;
        emit userLogined(result, info.value("message","").toString());
        break;
    }
}

void UserServiceImpl::logined(QString account, bool ok, QString reason)
{
    if(ok){
        QStringList user_list;
        user_list = _shared->AppUser()->whiteList();

        QScopedPointer<Config> config(new Config);

        if(config->_hackers)
            user_list<<account;

        if(user_list.contains(account)){
            _shared->Messager()->userLogin(account,"");
            if(_connect_monitor){
                this->killTimer(_connect_monitor);
                _connect_monitor = 0;
            }
            _connect_monitor = this->startTimer(10000);
        }
        else
            emit userLogined(false, QObject::tr("您暂时无权限使用"));
    }
    else{
        emit userLogined(false, reason);
    }
}
void UserServiceImpl::logouted(QString account, bool ok, QString reason)
{
    Q_UNUSED(account);

    emit userLogouted(ok, reason);
}

void UserServiceImpl::timerEvent(QTimerEvent *e)
{
    if(_connect_monitor = e->timerId()){

        emit userLogined(false, QObject::tr("服务器无响应"));

        this->killTimer(_connect_monitor);
        _connect_monitor = 0;
    }
}
