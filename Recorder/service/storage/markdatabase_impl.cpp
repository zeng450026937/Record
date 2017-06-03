#include "markdatabase_impl.h"
#include "database_impl.h"
#include "shareddata.h"
#include <QtSql>
#include <QDebug>

MarkDatabase* MarkDatabase::GetInterface(DataBase* db)
{
    DataBase_Impl* s = static_cast<DataBase_Impl*>(db);
    s->AddRef();
    return s;
}

int MarkDatabase_Impl::AddMark(QVariantMap& mark)
{
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());
    query.prepare("insert or replace into MARK (uuid, conference_uuid, time, mark) "
                "values (?, ?, ?, ?)");

    query.addBindValue( mark.value("uuid").toString() );
    query.addBindValue( mark.value("conference_uuid").toString() );
    query.addBindValue( mark.value("time").toInt() );
    query.addBindValue( mark.value("content").toString() );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}

QVariantList MarkDatabase_Impl::GetMark(QString& conference_uuid)
{
    QMutexLocker locker(_shared->_apiLock);

    QVariantList list;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select * from MARK where conference_uuid = ? order by time");
    query.addBindValue( conference_uuid );

    if(query.exec()){
        QSqlRecord rec = query.record();

        int index(1);
        while(query.next()){
            QVariantMap mark;

            for(int i = 0; i < rec.count(); i ++){
                mark.insert(rec.fieldName(i), query.value(i));
                mark.insert("index", (int)(index) );
            }
            index++;

            list << mark;
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return list;
}
QStringList MarkDatabase_Impl::GetMarkList()
{
    QMutexLocker locker(_shared->_apiLock);

    QStringList list;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select DISTINCT conference_uuid from MARK");

    if(query.exec()){
        while(query.next()){

            list << query.value(0).toString();
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return list;
}

int MarkDatabase_Impl::DelMark(QString& uuid)
{
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("delete from MARK where uuid = ?" );
    query.addBindValue( uuid );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}

int MarkDatabase_Impl::SetMarkSeconds(QString& uuid, int time)
{
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("update MARK set time = ? where uuid = ?");
    query.addBindValue( time );
    query.addBindValue( uuid );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}
int MarkDatabase_Impl::MarkSeconds(QString& uuid)
{
    QMutexLocker locker(_shared->_apiLock);

    int seconds;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select * from MARK where uuid = ?");
    query.addBindValue( uuid );

    if(query.exec()){
        QSqlRecord rec = query.record();

        int Col = rec.indexOf("time");

        while(query.next()){
            seconds = query.value(Col).toInt();
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return seconds;
}

int MarkDatabase_Impl::SetMarkText(QString& uuid, QString& mark)
{
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("update MARK set mark = ? where uuid = ?");
    query.addBindValue( mark );
    query.addBindValue( uuid );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}
QString MarkDatabase_Impl::MarkText(QString& uuid)
{
    QMutexLocker locker(_shared->_apiLock);

    QString text;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select * from MARK where uuid = ?");
    query.addBindValue( uuid );

    if(query.exec()){
        QSqlRecord rec = query.record();

        int Col = rec.indexOf("mark");

        while(query.next()){
            text = query.value(Col).toString();
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return text;
}
