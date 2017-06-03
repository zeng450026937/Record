#ifndef PERSONAL_DATABASE_H
#define PERSONAL_DATABASE_H

#include <QVariant>

class DataBase;

class PersonalDatabase {
public:
    static PersonalDatabase* GetInterface(DataBase* db);
    virtual int Release() = 0;

    virtual int AddConference(QVariantMap& conference) = 0;
    virtual int DelConference(QString& uuid) = 0;

    virtual QVariantMap ConferenceInfo(QString& uuid) = 0;

    virtual QVariantList ConfList() = 0;
    virtual QVariantList ConfList(QString& user_id) = 0;

protected:
    PersonalDatabase(){}
    virtual ~PersonalDatabase(){}

};

#endif // PERSONAL_DATABASE_H
