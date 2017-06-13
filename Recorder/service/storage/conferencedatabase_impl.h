#ifndef CONFERENCEDATABASE_IMPL_H
#define CONFERENCEDATABASE_IMPL_H

#include "include/conferencedatabase.h"

class SharedData;

class ConferenceDatabase_Impl : public ConferenceDatabase
{
public:
    int AddConference(QVariantMap& conference) override;
    QVariantMap ConferenceInfo(const QString& uuid) override;
    QVariantMap ConferenceInfo(int row) override;

    QVariantList ConfList() override;

    int ConfCount() override;
    int ConfCount(QString uuid) override;

    int DelConference(QString& uuid) override;
    int DelConference(int row) override;

    int ConfRow(QString uuid) override;

    int SetConfTitle(int row, QString& title) override;
    int SetConfTitle(QString& uuid, QString& title) override;
    QString ConfTitle(QString& uuid) override;

    int SetConfContent(int row, QString& content) override;
    int SetConfContent(QString& uuid, QString& content) override;
    QString ConfContent(QString& uuid) override;

    int SetConfMember(int row, QString& member) override;
    int SetConfMember(QString& uuid, QString& member) override;
    QString ConfMember(QString& uuid) override;

    int SetConfDevice(int row, QString& devices) override;
    int SetConfDevice(QString& uuid, QString& devices) override;
    QString ConfDevices(QString& uuid) override;

protected:
    ConferenceDatabase_Impl(SharedData* shared):_shared(NULL){ _shared = shared; }
    ~ConferenceDatabase_Impl() override {}

private:
    SharedData* _shared;
};

#endif // CONFERENCEDATABASE_IMPL_H
