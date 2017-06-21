#ifndef SHAREDDATA_H
#define SHAREDDATA_H

#include <QtSql>

class QMutex;

class SharedData
{
public:
    QMutex*         _apiLock;
    QSqlDatabase&   SqlDatabase(){ return _db; }

protected:
    bool InitDb();
    QSqlDatabase _db;

    SharedData();
    virtual ~SharedData();
};

#endif // SHAREDDATA_H
