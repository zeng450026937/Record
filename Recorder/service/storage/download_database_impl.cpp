#include "download_database_impl.h"
#include "database_impl.h"
#include "shareddata.h"
#include <QtSql>
#include <QDebug>

//static
DownloadDatabase* DownloadDatabase::GetInterface(DataBase* db)
{
    DataBase_Impl* s = static_cast<DataBase_Impl*>(db);
    s->AddRef();
    return s;
}

int DownloadDatabaseImpl::AddSegment(int type, QString identity,
               QString uuid, int meeting_time,
               int startpos, int status,
               int data_size, QByteArray& data)
{
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());
    query.prepare("insert or replace into tb_download (type, identity, "
                  "uuid, meeting_time, startpos, status, data_size, data) "
                  "values (?, ?, ?, ?, ?, ?, ?, ?)");

    query.addBindValue( type );
    query.addBindValue( identity );
    query.addBindValue( uuid );
    query.addBindValue( meeting_time );
    query.addBindValue( startpos );
    query.addBindValue( status );
    query.addBindValue( data_size );
    query.addBindValue( data, QSql::In | QSql::Binary );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}

int DownloadDatabaseImpl::RemoveFile(int type, QString uuid, QString identity)
{
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("delete from tb_download where type = ? AND uuid = ? AND identity = ?" );
    query.addBindValue( type );
    query.addBindValue( uuid );
    query.addBindValue( identity );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}
int DownloadDatabaseImpl::RemoveConf(int type, QString uuid)
{
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("delete from tb_download where type = ? AND uuid = ?" );
    query.addBindValue( type );
    query.addBindValue( uuid );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}
int DownloadDatabaseImpl::GetFileSize(int type, QString uuid, QString identity)
{
    QMutexLocker locker(_shared->_apiLock);

    int file_size(0);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select SUM(data_size) from tb_download where type = ? AND uuid = ? AND identity = ?");
    query.addBindValue( type );
    query.addBindValue( uuid );
    query.addBindValue( identity );

    if(query.exec()){
        while(query.next()){
            file_size = query.value(0).toInt();
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return file_size;
}

int DownloadDatabaseImpl::GetConfSize(int type, QString uuid)
{
    QMutexLocker locker(_shared->_apiLock);

    int conf_size(0);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select SUM(data_size) from tb_download where type = ? AND uuid = ?");
    query.addBindValue( type );
    query.addBindValue( uuid );

    if(query.exec()){
        while(query.next()){
            conf_size = query.value(0).toInt();
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return conf_size;
}
QByteArray DownloadDatabaseImpl::GetFile(int type, QString uuid, QString identity)
{
    QMutexLocker locker(_shared->_apiLock);

    QByteArrayList file_data;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select data from tb_download where type = ? AND uuid = ? AND identity = ? order by startpos");
    query.addBindValue( type );
    query.addBindValue( uuid );
    query.addBindValue( identity );

    if(query.exec()){
        while(query.next()){
            file_data.append( query.value(0).toByteArray() );
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return file_data.join();
}

QStringList DownloadDatabaseImpl::GetCompletedIdentity(int type, QString uuid)
{
    QMutexLocker locker(_shared->_apiLock);

    QStringList list;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select DISTINCT identity from tb_download where type = ? AND uuid = ? AND status >= '1'");
    query.addBindValue( type );
    query.addBindValue( uuid );

    if(query.exec()){
        QSqlRecord rec = query.record();

        while(query.next()){
            QString identity = query.value(0).toString();

            list<<identity;
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return list;
}
