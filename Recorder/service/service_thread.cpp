#include "service_thread.h"
#include "service_thread_private.h"

ServiceThread::ServiceThread(QObject *parent) :
    QThread(parent),
    _private(nullptr)
{
    this->start();
}
ServiceThread::~ServiceThread()
{
    if(this->isRunning()){
        this->quit();
        this->wait();
    }
}

ConfService* ServiceThread::GetConfService()
{
    return _private->GetConfService();
}

UserService* ServiceThread::GetUserService()
{
    return _private->GetUserService();
}

void ServiceThread::ThreadRelease()
{
    if(_private)
        delete _private;
}

void ServiceThread::run()
{
    _private = new ServiceThreadPrivate();

    QObject::connect(this,SIGNAL(finished()),this,SLOT(ThreadRelease()),Qt::DirectConnection);

    if(_private != nullptr)
        emit service_ready();

    this->exec();
}
