#include "service_thread.h"
#include "conf_service_impl.h"
#include "user_service_impl.h"
#include "service_thread_private.h"
#include "command/info_mode.h"
#include "command/ConferenceMode.h"
#include "command/PersonalMode.h"
#include "storage/database_impl.h"
#include <recorder_shared.h>

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

void ServiceThread::Uninstance()
{
    delete ServiceThreadPrivate::s_pServiceSingleton;
    ServiceThreadPrivate::s_pServiceSingleton = nullptr;
}

ConferenceMode * ServiceThread::GetConferenceMode()
{
    if (_private->_pConferenceMode == nullptr)
    {
        _private->_pConferenceMode = new ConferenceMode(GetMessager());
        _private->_pConferenceMode->m_pRecrodShared = GetRecordShared();
        _private->_pConferenceMode->moveToThread(this);
    }

    return _private->_pConferenceMode;
}

PersonalMode * ServiceThread::GetPersonalMode()
{
    if (_private->_pPersonalMode == nullptr)
    {
        _private->_pPersonalMode = new PersonalMode(GetMessager());
        _private->_pPersonalMode->m_pConfService = GetConfService();
        _private->_pPersonalMode->m_pRecordShared = GetRecordShared();
        _private->_pPersonalMode->moveToThread(this);
    }

    return _private->_pPersonalMode;
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
    if (nullptr == _private->_info_mode)
    {
        _private->_info_mode = new InfoMode(GetMessager());
        _private->_info_mode->moveToThread(this);
    }

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

ConfServiceImpl* ServiceThread::GetConfService()
{
    return _private->_conf_service;
}

UserServiceImpl* ServiceThread::GetUserService()
{
    return _private->_user_service;
}

RecorderShared * ServiceThread::GetRecordShared()
{
    if (nullptr == _private->_record_shared)
    {
        _private->_record_shared = new RecorderShared();
    }

    return _private->_record_shared;
}

DownloadDatabase * ServiceThread::GetDownloadDB()
{
    return _private->_download_db;
}

void ServiceThread::run()
{
	_private = new ServiceThreadPrivate(); // 在这里初始化是为了让对象的槽在这个线程中处理。

	_private->_conf_service = new ConfServiceImpl(_private);
	_private->_user_service = new UserServiceImpl(this);
    
    if(_private != nullptr)
		emit service_ready();

    this->exec();
}
