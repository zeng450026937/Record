#ifndef USER_DATABASE_H
#define USER_DATABASE_H

#include <QVariant>

class DataBase;

class UserDatabase {
public:
    static UserDatabase* GetInterface(DataBase* db);
    virtual int Release() = 0;

    virtual int AddUser(QVariantMap& User) = 0;
    virtual QVariantMap GetUserInfo(QString& account) = 0;
    virtual QVariantList GetUserList() = 0;

    virtual int DelUser(QString& account) = 0;

    virtual int SetUserName(QString& account, QString& name) = 0;
    virtual QString UserName(QString& account) = 0;

protected:
    UserDatabase(){}
    virtual ~UserDatabase(){}

};

#endif // USER_DATABASE_H
