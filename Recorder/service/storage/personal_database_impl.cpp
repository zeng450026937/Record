#include "personal_database_impl.h"
#include "database_impl.h"
#include "shareddata.h"
#include <QtSql>
#include <QDebug>

//static
PersonalDatabase* PersonalDatabase::GetInterface(DataBase* db)
{
    DataBase_Impl* s = static_cast<DataBase_Impl*>(db);
    s->AddRef();
    return s;
}

int PersonalDatabaseImpl::AddConference(QVariantMap& conference)
{
    QMutexLocker locker(_shared->_apiLock);

    QString text;
    QDateTime dateTime;

    text = conference.value("createTime").toString();
    dateTime = QDateTime::fromString(text, "yyyy-MM-dd hh:mm:ss");

    conference.insert("date",dateTime.date().toString("yyyyMMdd"));
    conference.insert("time",dateTime.time().toString("hhmmss"));

    QSqlQuery query(_shared->SqlDatabase());
    query.prepare("insert or replace into tb_personal (conference_uuid, title, user_id, user_name, "
                  "type, gps, gps_address, tag, device_mac, "
                  "device_name, state, create_time, update_time, date, time, completed) "
                  "values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

    query.addBindValue( conference.value("conferenceUuid").toString() );
    query.addBindValue( conference.value("title").toString() );
    query.addBindValue( conference.value("userId").toString() );
    query.addBindValue( conference.value("userName").toString() );
    query.addBindValue( conference.value("type").toString() );
    query.addBindValue( conference.value("gps").toString() );
    query.addBindValue( conference.value("gpsAddress").toString() );
    query.addBindValue( conference.value("tag").toString() );
    query.addBindValue( conference.value("deviceUuid").toString() );
    query.addBindValue( conference.value("deviceName").toString() );
    query.addBindValue( conference.value("state").toString() );
    query.addBindValue( conference.value("createTime").toString() );
    query.addBindValue( conference.value("updateTime").toString() );
    query.addBindValue( conference.value("date").toString() );
    query.addBindValue( conference.value("time").toString() );
    query.addBindValue( conference.value("completed").toInt() );

    if(!query.exec())
    {
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}

int PersonalDatabaseImpl::DelConference(QString& uuid)
{
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("delete from tb_personal where conference_uuid = ?" );
    query.addBindValue( uuid );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}

QVariantMap PersonalDatabaseImpl::ConferenceInfo(QString& uuid)
{
    QMutexLocker locker(_shared->_apiLock);

    QVariantMap info;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select * from tb_personal where conference_uuid = ?");
    query.addBindValue( uuid );

    if(query.exec()){
        QSqlRecord rec = query.record();

        while(query.next()){
            for(int i = 0; i < rec.count(); i ++){
                info.insert(rec.fieldName(i), query.value(i));
            }
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return info;
}

QVariantList PersonalDatabaseImpl::ConfList()
{
    QMutexLocker locker(_shared->_apiLock);

    QVariantList list;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select * from tb_personal order by user_id");

    if(query.exec()){
        QSqlRecord rec = query.record();

        while(query.next()){
            QVariantMap conf;
            for(int i = 0; i < rec.count(); i ++){
                conf.insert(rec.fieldName(i), query.value(i));
            }
            list<<conf;
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return list;
}
QVariantList PersonalDatabaseImpl::ConfList(QString& user_id)
{
    QMutexLocker locker(_shared->_apiLock);

    QVariantList list;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select * from tb_personal where user_id = ? order by user_id");
    query.addBindValue( user_id );

    if(query.exec()){
        QSqlRecord rec = query.record();

        while(query.next()){
            QVariantMap conf;
            for(int i = 0; i < rec.count(); i ++){
                conf.insert(rec.fieldName(i), query.value(i));
            }
            list<<conf;
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return list;
}


