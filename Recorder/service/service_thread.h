#ifndef SERVICETHREAD_H
#define SERVICETHREAD_H

#include <QThread>

class WhiteList;
class MessageBase;
class AccountCenter;
class ConfService;
class UserServiceImpl;
class InfoMode;

class ServiceThreadPrivate;
class ServiceThread : public QThread
{
	Q_OBJECT
private:
	explicit ServiceThread(QObject *parent = 0);
	ServiceThread(ServiceThread &) {}
	ServiceThread & operator=(ServiceThread*) {}

public:
	~ServiceThread();

	static ServiceThread * GetInstance();

	InfoMode		*GetInfoMode();
	AccountCenter *GetAccountCenter();

	WhiteList	*	GetLoginWhiteList();
	MessageBase *	GetMessager();
	ConfService*    GetConfService();
	UserServiceImpl*    GetUserService();



	ServiceThreadPrivate* _private;
signals:
    void service_ready();

protected slots:
    void ThreadRelease();

protected:
    void run() override;

protected:
};

#endif // SERVICETHREAD_H
