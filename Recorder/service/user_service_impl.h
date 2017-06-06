#ifndef USERSERVICEIMPL_H
#define USERSERVICEIMPL_H

#include <QObject>
#include "include/user_service.h"

class ServiceThreadPrivate;

class UserServiceImpl : public UserService
{
    Q_OBJECT
public:
    explicit UserServiceImpl(ServiceThreadPrivate* shared, QObject *parent = 0);
    ~UserServiceImpl() {}

public slots:
    void connectTo(QString uri = "") override;
    void stopConnection() override;
    void userLogin(QString account = "", QString password = "") override;

protected slots:
    void parseCommand(int command, bool result, QVariantMap& info);

    void logined(QString account, bool ok, QString reason);
    void logouted(QString account, bool ok, QString reason);

protected:
    virtual void timerEvent(QTimerEvent *e);

private:
    ServiceThreadPrivate* _shared;
    QString               _uri;
    bool                  _warning;
    int                   _connect_monitor;
};

#endif // USERSERVICEIMPL_H
