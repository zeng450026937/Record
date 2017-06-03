#ifndef MAKEDATABASE_H
#define MAKEDATABASE_H

#include <QVariant>

class DataBase;

class MarkDatabase {
public:
    static MarkDatabase* GetInterface(DataBase* db);
    virtual int Release() = 0;

    virtual int AddMark(QVariantMap& mark) = 0;
    virtual QVariantList GetMark(QString& conference_uuid) = 0;
    //return uuid list
    virtual QStringList GetMarkList() = 0;

    virtual int DelMark(QString& uuid) = 0;

    virtual int SetMarkSeconds(QString& uuid, int time) = 0;
    virtual int MarkSeconds(QString& uuid) = 0;

    virtual int SetMarkText(QString& uuid, QString& mark) = 0;
    virtual QString MarkText(QString& uuid) = 0;

protected:
    MarkDatabase(){}
    virtual ~MarkDatabase(){}

};

#endif // MAKEDATABASE_H
