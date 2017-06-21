#include "conferencedatabase_impl.h"
#include "database_impl.h"
#include "shareddata.h"
#include <QtSql>
#include <QDebug>

ConferenceDatabase* ConferenceDatabase::GetInterface(DataBase* db)
{
    DataBase_Impl* s = static_cast<DataBase_Impl*>(db);
    s->AddRef();
    return s;
}

int ConferenceDatabase_Impl::AddConference(QVariantMap& conference)
{
    QMutexLocker locker(_shared->_apiLock);

    QString text;
    QDateTime dateTime;

    text = conference.value("createTime").toString();
    dateTime = QDateTime::fromString(text, "yyyy-MM-dd hh:mm:ss");

    conference.insert("date",dateTime.date().toString("yyyyMMdd"));
    conference.insert("time",dateTime.time().toString("hhmmss"));

    QSqlQuery query(_shared->SqlDatabase());
    query.prepare("insert or replace into tb_conference (uuid, title, content, members, "
                  "devices, create_time, update_time, date, time, completed) "
                "values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

    // TODO id 和 status 两个字段没有记录
    query.addBindValue( conference.value("uuid").toString() );
    query.addBindValue( conference.value("title").toString() );
    query.addBindValue( conference.value("content").toString() );
    query.addBindValue( conference.value("members").toString() );   
    query.addBindValue( conference.value("devices").toString() );
    query.addBindValue( conference.value("createTime").toString() );
    query.addBindValue( conference.value("updateTime").toString() );
    query.addBindValue( conference.value("date").toString() );
    query.addBindValue( conference.value("time").toString() );
    query.addBindValue( conference.value("completed").toInt() );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}
QVariantMap ConferenceDatabase_Impl::ConferenceInfo(const QString& uuid)
{
    QMutexLocker locker(_shared->_apiLock);

    QVariantMap conf;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select * from tb_conference where uuid = ?");
    query.addBindValue( uuid );

    if(query.exec()){
        QSqlRecord rec = query.record();
        int Col = rec.indexOf("devices");

        while(query.next()){

            for(int i = 0; i < rec.count(); i ++){
                conf.insert(rec.fieldName(i), query.value(i));
            }
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return conf;
}
QVariantMap ConferenceDatabase_Impl::ConferenceInfo(int row)
{   
    QMutexLocker locker(_shared->_apiLock);

    QVariantMap conf;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select * from tb_conference where id = ?");
    query.addBindValue( row );

    if(query.exec()){
        QSqlRecord rec = query.record();

        while(query.next()){

            for(int i = 0; i < rec.count(); i ++){
                conf.insert(rec.fieldName(i), query.value(i));
            }
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return conf;
}

QVariantList ConferenceDatabase_Impl::ConfList()
{
    QMutexLocker locker(_shared->_apiLock);

    QVariantList list;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select * from tb_conference order by id");

    if(query.exec()){
        QSqlRecord rec = query.record();

        while(query.next()){
            QVariantMap conf;

            for(int i = 0; i < rec.count(); i ++){
                conf.insert(rec.fieldName(i), query.value(i));
            }

            list<<conf;
        }

    }
    else
    {
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return list;
}

int ConferenceDatabase_Impl::ConfCount()
{
    QMutexLocker locker(_shared->_apiLock);

    int count(0);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select count(*) from tb_conference");
    if(query.exec())
    {
        while(query.next())
        {
            count = query.size();
        }
    }
    else
    {
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return count;
}

int ConferenceDatabase_Impl::ConfCount(QString uuid)
{
    QMutexLocker locker(_shared->_apiLock);

    int count(0);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select count(*) from tb_conference where uuid = ?");
    query.addBindValue( uuid );

    if(query.exec()){
        while(query.next()){
            count = query.size();
        }
    }
    else
    {
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return count;
}

int ConferenceDatabase_Impl::DelConference(QString& uuid)
{
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("delete from tb_conference where uuid = ?" );
    query.addBindValue( uuid );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}
int ConferenceDatabase_Impl::DelConference(int row)
{
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("delete from tb_conference where id = ?" );
    query.addBindValue( row );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}

int ConferenceDatabase_Impl::ConfRow(QString uuid)
{
    QMutexLocker locker(_shared->_apiLock);

    int row(-1);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select id from tb_conference where uuid = ?");
    query.addBindValue( uuid );

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
int ConferenceDatabase_Impl::SetConfTitle(int row, QString& title)
{
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("update tb_conference set title = ? where id = ?");
    query.addBindValue( title );
    query.addBindValue( row );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}
int ConferenceDatabase_Impl::SetConfTitle(QString& uuid, QString& title)
{
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("update tb_conference set title = ? where uuid = ?");
    query.addBindValue( title );
    query.addBindValue( uuid );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}
QString ConferenceDatabase_Impl::ConfTitle(QString& uuid)
{
    QMutexLocker locker(_shared->_apiLock);

    QString title;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select * from tb_conference where uuid = ?");
    query.addBindValue( uuid );

    if(query.exec()){
        QSqlRecord rec = query.record();

        int Col = rec.indexOf("title");

        while(query.next()){
            title = query.value(Col).toString();
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return title;
}
int ConferenceDatabase_Impl::SetConfContent(int row, QString& content)
{
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("update tb_conference set content = ? where id = ?");
    query.addBindValue( content );
    query.addBindValue( row );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}
int ConferenceDatabase_Impl::SetConfContent(QString& uuid, QString& content)
{
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("update tb_conference set content = ? where uuid = ?");
    query.addBindValue( content );
    query.addBindValue( uuid );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}
QString ConferenceDatabase_Impl::ConfContent(QString& uuid)
{
    QMutexLocker locker(_shared->_apiLock);

    QString content;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select * from tb_conference where uuid = ?");
    query.addBindValue( uuid );

    if(query.exec()){
        QSqlRecord rec = query.record();

        int Col = rec.indexOf("content");

        while(query.next()){
            content = query.value(Col).toString();
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return content;
}
int ConferenceDatabase_Impl::SetConfMember(int row, QString& member)
{
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("update tb_conference set members = ? where id = ?");
    query.addBindValue( member );
    query.addBindValue( row );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}
int ConferenceDatabase_Impl::SetConfMember(QString& uuid, QString& member)
{
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("update tb_conference set members = ? where uuid = ?");
    query.addBindValue( member );
    query.addBindValue( uuid );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}
QString ConferenceDatabase_Impl::ConfMember(QString& uuid)
{
    QMutexLocker locker(_shared->_apiLock);

    QString members;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select * from tb_conference where uuid = ?");
    query.addBindValue( uuid );

    if(query.exec()){
        QSqlRecord rec = query.record();

        int Col = rec.indexOf("members");

        while(query.next()){
            members = query.value(Col).toString();
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return members;
}
int ConferenceDatabase_Impl::SetConfDevice(int row, QString& devices)
{
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("update tb_conference set devices = ? where id = ?");
    query.addBindValue( devices );
    query.addBindValue( row );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}
int ConferenceDatabase_Impl::SetConfDevice(QString& uuid, QString& devices)
{
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("update tb_conference set devices = ? where uuid = ?");
    query.addBindValue( devices );
    query.addBindValue( uuid );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}
QString ConferenceDatabase_Impl::ConfDevices(QString& uuid)
{
    QMutexLocker locker(_shared->_apiLock);

    QString devices;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select * from tb_conference where uuid = ?");
    query.addBindValue( uuid );

    if(query.exec()){
        QSqlRecord rec = query.record();

        int Col = rec.indexOf("devices");

        while(query.next()){
            devices = query.value(Col).toString();
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return devices;
}

