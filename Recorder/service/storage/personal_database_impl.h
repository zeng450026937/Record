#ifndef PERSONALDATABASEIMPL_H
#define PERSONALDATABASEIMPL_H

#include "include/personal_database.h"

class SharedData;

class PersonalDatabaseImpl : public PersonalDatabase
{
public:
    int AddConference(QVariantMap& conference) override;
    int DelConference(QString& uuid) override;

    QVariantMap ConferenceInfo(QString& uuid) override;

    QVariantList ConfList() override;
    QVariantList ConfList(QString& user_id) override;

protected:
    PersonalDatabaseImpl(SharedData* shared):_shared(NULL){ _shared = shared; }
    ~PersonalDatabaseImpl() override {}

private:
    SharedData* _shared;
};

#endif // PERSONALDATABASEIMPL_H
