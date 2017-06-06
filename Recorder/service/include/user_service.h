#ifndef USER_SERVICE_H
#define USER_SERVICE_H

#include <QObject>

class UserService : public QObject
{
	Q_OBJECT
signals:
    void connectOpened();
    void connectFailed();
    void connectClosed();
    void userLogined(bool result, QString error);
    void userLogouted(bool result, QString error);

public slots:
    virtual void connectTo(QString uri = "") = 0;
    virtual void stopConnection() = 0;
    virtual void userLogin(QString account, QString password) = 0;

protected:
    explicit UserService(QObject *parent = 0) : QObject(parent) { }
    ~UserService() { }
};

#endif // USER_SERVICE_H
