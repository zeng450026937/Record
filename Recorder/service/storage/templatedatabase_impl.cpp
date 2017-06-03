#include "templatedatabase_impl.h"
#include "database_impl.h"
#include "shareddata.h"
#include <QtSql>
#include <QDebug>

TemplateDatabase* TemplateDatabase::GetInterface(DataBase* db)
{
    DataBase_Impl* s = static_cast<DataBase_Impl*>(db);
    s->AddRef();
    return s;
}

int TemplateDatabase_Impl::AddTemplate(QVariantMap& Template)
{
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());
    query.prepare("insert or replace into TEMPLATE (uuid, title, content, members) "
                "values (?, ?, ?, ?)");

    query.addBindValue( Template.value("uuid").toString() );
    query.addBindValue( Template.value("title").toString() );
    query.addBindValue( Template.value("content").toString() );
    query.addBindValue( Template.value("members").toString() );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}
QVariantMap TemplateDatabase_Impl::TemplateInfo(QString uuid)
{
    QMutexLocker locker(_shared->_apiLock);

    QVariantMap templ;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select * from TEMPLATE where uuid = ?");
    query.addBindValue( uuid );

    if(query.exec()){
        QSqlRecord rec = query.record();

        while(query.next()){

            for(int i = 0; i < rec.count(); i ++){
                templ.insert(rec.fieldName(i), query.value(i));
            }
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return templ;
}

QVariantMap TemplateDatabase_Impl::TemplateInfo(int row)
{
    QMutexLocker locker(_shared->_apiLock);

    QVariantMap templ;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select * from TEMPLATE where id = ?");
    query.addBindValue( row );

    if(query.exec()){
        QSqlRecord rec = query.record();

        while(query.next()){

            for(int i = 0; i < rec.count(); i ++){
                templ.insert(rec.fieldName(i), query.value(i));
            }
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return templ;
}

QVariantList TemplateDatabase_Impl::TemplateList()
{
    QMutexLocker locker(_shared->_apiLock);

    QVariantList list;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select * from TEMPLATE order by id");

    if(query.exec()){
        QSqlRecord rec = query.record();

        while(query.next()){
            QVariantMap templ;

            for(int i = 0; i < rec.count(); i ++){
                templ.insert(rec.fieldName(i), query.value(i));
            }

            list<<templ;
        }

    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return list;
}

int TemplateDatabase_Impl::TemplateCount()
{
    QMutexLocker locker(_shared->_apiLock);

    int count(0);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select count(*) from TEMPLATE");

    if(query.exec()){
        while(query.next()){
            count = query.size();
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return count;
}

int TemplateDatabase_Impl::TemplateCount(QString uuid)
{
    QMutexLocker locker(_shared->_apiLock);

    int count(0);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select count(*) from TEMPLATE where uuid = ?");

    if(query.exec()){
        while(query.next()){
            count = query.size();
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return count;
}

int TemplateDatabase_Impl::DelTemplate(QString uuid)
{
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    int row = this->TemplateRow(uuid);

    query.prepare("delete from TEMPLATE where uuid = ?" );
    query.addBindValue( uuid );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    query.prepare("update TEMPLATE set id = id-1 where id > ?" );
    query.addBindValue( row );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}

int TemplateDatabase_Impl::DelTemplate(int row)
{
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("delete from TEMPLATE where id = ?" );
    query.addBindValue( row );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    query.prepare("update TEMPLATE set id = id-1 where id > ?" );
    query.addBindValue( row );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}

int TemplateDatabase_Impl::TemplateRow(QString uuid)
{
    QMutexLocker locker(_shared->_apiLock);

    int row(-1);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select id from TEMPLATE where uuid = ?");
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

int TemplateDatabase_Impl::SetTemplateTitle(QString uuid, QString title)
{
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("update TEMPLATE set title = ? where uuid = ?");
    query.addBindValue( title );
    query.addBindValue( uuid );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}
QString TemplateDatabase_Impl::TemplateTitle(QString uuid)
{
    QMutexLocker locker(_shared->_apiLock);

    QString title;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select * from TEMPLATE where uuid = ?");
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

int TemplateDatabase_Impl::SetTemplateContent(QString uuid, QString content)
{
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("update TEMPLATE set content = ? where uuid = ?");
    query.addBindValue( content );
    query.addBindValue( uuid );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}
QString TemplateDatabase_Impl::TemplateContent(QString uuid)
{
    QMutexLocker locker(_shared->_apiLock);

    QString content;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select * from TEMPLATE where uuid = ?");
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

int TemplateDatabase_Impl::SetTemplateMember(QString uuid, QString member)
{
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("update TEMPLATE set members = ? where uuid = ?");
    query.addBindValue( member );
    query.addBindValue( uuid );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}
QString TemplateDatabase_Impl::TemplateMember(QString uuid)
{
    QMutexLocker locker(_shared->_apiLock);

    QString member;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select * from TEMPLATE where uuid = ?");
    query.addBindValue( uuid );

    if(query.exec()){
        QSqlRecord rec = query.record();

        int Col = rec.indexOf("member");

        while(query.next()){
            member = query.value(Col).toString();
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return member;
}
