#include "filedatabase_impl.h"
#include "database_impl.h"
#include "shareddata.h"
#include <QtSql>
#include <QDebug>

FileDatabase* FileDatabase::GetInterface(DataBase* db)
{
    DataBase_Impl* s = static_cast<DataBase_Impl*>(db);
    s->AddRef();
    return s;
}

int FileDatabase_Impl::AddFile(QVariantMap& file)
{  
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());
    query.prepare("insert or replace into FILE (uuid, account, device_mac, start_time, status, fullpath) "
                "values (?, ?, ?, ?, ?)");

    query.addBindValue( file.value("uuid").toString() );
    query.addBindValue( file.value("account").toString() );
    query.addBindValue( file.value("device_mac").toString() );
    query.addBindValue( file.value("start_time").toInt() );
    query.addBindValue( file.value("status").toInt() );
    query.addBindValue( file.value("fullpath").toString() );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}
QVariantMap FileDatabase_Impl::FileInfo(QString& path)
{
    QMutexLocker locker(_shared->_apiLock);

    QVariantMap file;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select * from FILE where fullpath = ?");
    query.addBindValue( path );

    if(query.exec()){
        QSqlRecord rec = query.record();

        while(query.next()){

            for(int i = 0; i < rec.count(); i ++){
                file.insert(rec.fieldName(i), query.value(i));
            }
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return file;
}

QVariantList  FileDatabase_Impl::GetFileList()
{
    QMutexLocker locker(_shared->_apiLock);

    QVariantList list;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select * from FILE order by uuid,rowid");

    if(query.exec()){
        QSqlRecord rec = query.record();

        while(query.next()){
            QVariantMap file;

            for(int i = 0; i < rec.count(); i ++){
                file.insert(rec.fieldName(i), query.value(i));
            }

            list<<file;
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return list;
}
QVariantList  FileDatabase_Impl::GetFileList(QString& uuid)
{
    QMutexLocker locker(_shared->_apiLock);

    QVariantList list;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select * from FILE where uuid = ? order by rowid");
    query.addBindValue( uuid );

    if(query.exec()){
        QSqlRecord rec = query.record();

        while(query.next()){
            QVariantMap file;

            for(int i = 0; i < rec.count(); i ++){
                file.insert(rec.fieldName(i), query.value(i));
            }

            list<<file;
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return list;
}

int FileDatabase_Impl::FileCount()
{
    QMutexLocker locker(_shared->_apiLock);

    int count(0);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select count(*) from FILE");

    if(query.exec()){
        count = query.size();
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return count;
}
int FileDatabase_Impl::FileCount(QString& uuid)
{
    QMutexLocker locker(_shared->_apiLock);

    int count(0);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select count(*) from FILE where uuid = ?");
    query.addBindValue( uuid );

    if(query.exec()){
        count = query.size();
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return count;
}

int FileDatabase_Impl::DelFile(QString& path)
{
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("delete from FILE where fullpath = ?" );
    query.addBindValue( path );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}

int FileDatabase_Impl::FileRow(QString path)
{
    QMutexLocker locker(_shared->_apiLock);

    int row(0);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select rowid from FILE where fullpath = ?");
    query.addBindValue( path );

    if(query.exec()){
        while(query.next()){
            row = query.value(0).toInt();
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return row;
}

int FileDatabase_Impl::SetFileUuid(QString& path, QString& uuid)
{
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("update FILE set uuid = ? where fullpath = ?");
    query.addBindValue( uuid );
    query.addBindValue( path );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}
QString FileDatabase_Impl::FileUuid(QString& path)
{
    QMutexLocker locker(_shared->_apiLock);

    QString uuid;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select * from FILE where fullpath = ?");
    query.addBindValue( path );

    if(query.exec()){
        QSqlRecord rec = query.record();

        int Col = rec.indexOf("uuid");

        while(query.next()){
            uuid = query.value(Col).toString();
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return uuid;
}

int FileDatabase_Impl::SetFileDeviceName(QString& path, QString& device_name)
{
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("update FILE set device_name = ? where fullpath = ?");
    query.addBindValue( device_name );
    query.addBindValue( path );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}
QString FileDatabase_Impl::FileDeviceName(QString& path)
{
    QMutexLocker locker(_shared->_apiLock);

    QString device;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select * from FILE where fullpath = ?");
    query.addBindValue( path );

    if(query.exec()){
        QSqlRecord rec = query.record();

        int Col = rec.indexOf("device_name");

        while(query.next()){
            device = query.value(Col).toString();
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return device;
}

int FileDatabase_Impl::SetFilePath(QString& path_old, QString& path_new)
{
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("update FILE set fullpath = ? where fullpath = ?");
    query.addBindValue( path_new );
    query.addBindValue( path_old );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}
QString FileDatabase_Impl::FilePath(QString& path)
{
    QMutexLocker locker(_shared->_apiLock);

    QString fullpath;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select * from FILE where fullpath = ?");
    query.addBindValue( path );

    if(query.exec()){
        QSqlRecord rec = query.record();

        int Col = rec.indexOf("fullpath");

        while(query.next()){
            fullpath = query.value(Col).toString();
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return fullpath;
}
