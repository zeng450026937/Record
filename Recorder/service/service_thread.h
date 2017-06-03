#ifndef SERVICETHREAD_H
#define SERVICETHREAD_H

#include <QThread>

class ConfService;
class UserService;
class ServiceThreadPrivate;

class ServiceThread : public QThread
{
    Q_OBJECT
public:
    explicit ServiceThread(QObject *parent = 0);
    ~ServiceThread();

    ConfService*    GetConfService();
    UserService*    GetUserService();

signals:
    void service_ready();

protected slots:
    void ThreadRelease();

protected:
    void run() override;

protected:
    ServiceThreadPrivate* _private;
};

#endif // SERVICETHREAD_H
