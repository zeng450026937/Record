#ifndef MARKDATABASE_IMPL_H
#define MARKDATABASE_IMPL_H

#include "include/markdatabase.h"

class SharedData;

class MarkDatabase_Impl : public MarkDatabase
{
public:
    int AddMark(QVariantMap& mark) override;
    QVariantList GetMark(QString& conference_uuid) override;
    //return uuid list
    QStringList GetMarkList() override;

    int DelMark(QString& uuid) override;

    int SetMarkSeconds(QString& uuid, int time) override;
    int MarkSeconds(QString& uuid) override;

    int SetMarkText(QString& uuid, QString& mark) override;
    QString MarkText(QString& uuid) override;

protected:
    MarkDatabase_Impl(SharedData* shared):_shared(NULL){ _shared = shared; }
    ~MarkDatabase_Impl() override {}

private:
    SharedData* _shared;
};

#endif // MARKDATABASE_IMPL_H
