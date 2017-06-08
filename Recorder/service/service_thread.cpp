#include "service_thread.h"
#include "conf_service_impl.h"
#include "user_service_impl.h"
#include "service_thread_private.h"
#include "command/info_mode.h"

ServiceThread *ServiceThreadPrivate::s_pServiceSingleton = nullptr;

ServiceThread::ServiceThread(QObject *parent) :
    QThread(parent),
	_private(nullptr)
{
}

ServiceThread::~ServiceThread()
{
    if(this->isRunning()){
        this->quit();
        this->wait();
    }

	if (_private)
		delete _private;
}

ServiceThread * ServiceThread::ServiceThread::GetInstance()
{
	if (nullptr == ServiceThreadPrivate::s_pServiceSingleton)
	{
		ServiceThreadPrivate::s_pServiceSingleton = new ServiceThread();
	}

	return ServiceThreadPrivate::s_pServiceSingleton;
}

InfoMode * ServiceThread::GetInfoMode()
{
	return _private->_info_mode;
}

AccountCenter * ServiceThread::GetAccountCenter()
{
	return _private->_account;
}

WhiteList	* ServiceThread::GetLoginWhiteList()
{
	return _private->_white_list;
}

MessageBase * ServiceThread::GetMessager()
{
	return _private->_messager;
}

ConfService* ServiceThread::GetConfService()
{
    return _private->_conf_service;
}

UserServiceImpl* ServiceThread::GetUserService()
{
    return _private->_user_service;
}

void ServiceThread::ThreadRelease()
{
}

void ServiceThread::run()
{
	_private = new ServiceThreadPrivate(); // 在这里初始化是为了让对象的槽在这个线程中处理。

    _private->_info_mode = new InfoMode(_private->_messager);
	_private->_conf_service = new ConfServiceImpl(_private);
	_private->_user_service = new UserServiceImpl(this);

    QObject::connect(this,SIGNAL(finished()),this,SLOT(ThreadRelease()),Qt::DirectConnection);
	if(_private != nullptr)
		emit service_ready();

    this->exec();
}
