#ifndef USERSERVICEIMPL_H
#define USERSERVICEIMPL_H

#include <QObject>
#include "include/user_service.h"

class InfoMode;
class AccountCenter;
class MessageBase;
class ServiceThread;
class ServiceThreadPrivate;

class UserServiceImpl : public QObject
{
    Q_OBJECT
public:
	enum ServiceResultCode
	{
		SRC_LOGIN_SUCCESS,
		SRC_LOGIN_FAILED,
		SRC_CONNECT_CLOSED,
	};
public:
    explicit UserServiceImpl(ServiceThread* pService, QObject *parent = 0);
    ~UserServiceImpl() {}

    void userLogin(QString account, QString password);

signals:
    void loginResult(QString qstrResultInfo);
	
public slots:
   // void connectTo(QString uri = "") override;
    void stopConnection();

protected slots:
    void on_connection_status(int iStatus);

    void logined(bool bOk, const QString & reason);

protected:
    virtual void timerEvent(QTimerEvent *e);

private:
	void DoWebsocketConnect();

private:
    ServiceThreadPrivate* _private;
	ServiceThread		*_service;
	MessageBase			 *_message;
	AccountCenter		 *_account;
	InfoMode			 *_info_mode;
    QString               _uri;
    bool                  _warning;
    int                   _connect_monitor;
};

#endif // USERSERVICEIMPL_H
