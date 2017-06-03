#ifndef CONFERENCEDATABASE_H
#define CONFERENCEDATABASE_H

#include <QVariant>

class DataBase;

class ConferenceDatabase {
public:
    static ConferenceDatabase* GetInterface(DataBase* db);
    virtual int Release() = 0;

    virtual int AddConference(QVariantMap& conference) = 0;
    virtual QVariantMap ConferenceInfo(QString& uuid) = 0;
    virtual QVariantMap ConferenceInfo(int row) = 0;

    virtual QVariantList ConfList() = 0;

    virtual int ConfCount() = 0;
    virtual int ConfCount(QString uuid) = 0;

    virtual int DelConference(QString& uuid) = 0;
    virtual int DelConference(int row) = 0;

    virtual int ConfRow(QString uuid) = 0;

    virtual int SetConfTitle(int row, QString& title) = 0;
    virtual int SetConfTitle(QString& uuid, QString& title) = 0;
    virtual QString ConfTitle(QString& uuid) = 0;

    virtual int SetConfContent(int row, QString& content) = 0;
    virtual int SetConfContent(QString& uuid, QString& content) = 0;
    virtual QString ConfContent(QString& uuid) = 0;

    virtual int SetConfMember(int row, QString& member) = 0;
    virtual int SetConfMember(QString& uuid, QString& member) = 0;
    virtual QString ConfMember(QString& uuid) = 0;

    virtual int SetConfDevice(int row, QString& devices) = 0;
    virtual int SetConfDevice(QString& uuid, QString& devices) = 0;
    virtual QString ConfDevices(QString& uuid) = 0;

protected:
    ConferenceDatabase(){}
    virtual ~ConferenceDatabase(){}

};

#endif // CONFERENCEDATABASE_H
