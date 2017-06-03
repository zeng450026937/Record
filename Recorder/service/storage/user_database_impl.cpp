#include "user_database_impl.h"
#include "database_impl.h"
#include "shareddata.h"
#include <QtSql>
#include <QDebug>

UserDatabase* UserDatabase::GetInterface(DataBase* db)
{
    DataBase_Impl* s = static_cast<DataBase_Impl*>(db);
    s->AddRef();
    return s;
}

int UserDatabase_Impl::AddUser(QVariantMap& User)
{
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());
    query.prepare("insert or replace into USER (account, display_name) "
                "values (?, ?)");

    query.addBindValue( User.value("account").toString() );
    query.addBindValue( User.value("display_name").toString() );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}
QVariantMap UserDatabase_Impl::GetUserInfo(QString& account)
{
    QMutexLocker locker(_shared->_apiLock);

    QVariantMap user;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select * from USER where account = ?");
    query.addBindValue( account );

    if(query.exec()){
        QSqlRecord rec = query.record();

        while(query.next()){

            for(int i = 0; i < rec.count(); i ++){
                user.insert(rec.fieldName(i), query.value(i));
            }
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return user;
}
QVariantList UserDatabase_Impl::GetUserList()
{
    QMutexLocker locker(_shared->_apiLock);

    QVariantList list;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select * from USER order by account");

    if(query.exec()){
        QSqlRecord rec = query.record();

        while(query.next()){
            QVariantMap user;

            for(int i = 0; i < rec.count(); i ++){
                user.insert(rec.fieldName(i), query.value(i));
            }

            list<<user;
        }

    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return list;
}

int UserDatabase_Impl::DelUser(QString& account)
{
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("delete from USER where account = ?" );
    query.addBindValue( account );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}

int UserDatabase_Impl::SetUserName(QString& account, QString& name)
{
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("update USER set display_name = ? where account = ?");
    query.addBindValue( name );
    query.addBindValue( account );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}
QString UserDatabase_Impl::UserName(QString& account)
{
    QMutexLocker locker(_shared->_apiLock);

    QString text;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select * from USER where account = ?");
    query.addBindValue( account );

    if(query.exec()){
        QSqlRecord rec = query.record();

        int Col = rec.indexOf("display_name");

        while(query.next()){
            text = query.value(Col).toString();
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return text;
}
