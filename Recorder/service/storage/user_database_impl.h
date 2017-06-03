#ifndef USERDATABASEIMPL_H
#define USERDATABASEIMPL_H

#include "include/user_database.h"

class SharedData;

class UserDatabase_Impl : public UserDatabase
{
public:
    int AddUser(QVariantMap& User) override;
    QVariantMap GetUserInfo(QString& account) override;
    QVariantList GetUserList() override;

    int DelUser(QString& account) override;

    int SetUserName(QString& account, QString& name) override;
    QString UserName(QString& account) override;

protected:
    UserDatabase_Impl(SharedData* shared):_shared(NULL){ _shared = shared; }
    ~UserDatabase_Impl() override {}

private:
    SharedData* _shared;
};


#endif // USERDATABASEIMPL_H
