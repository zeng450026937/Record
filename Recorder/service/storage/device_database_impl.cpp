#include "device_database_impl.h"
#include "database_impl.h"
#include "shareddata.h"
#include <QtSql>
#include <QDebug>

DeviceDatabase* DeviceDatabase::GetInterface(DataBase* db)
{
    DataBase_Impl* s = static_cast<DataBase_Impl*>(db);
    s->AddRef();
    return s;
}


int DeviceDatabase_Impl::AddDevice(QVariantMap& device)
{
    QMutexLocker locker(_shared->_apiLock);

    int selected = this->DeviceSelected(device.value("device_mac").toString());
    int used = this->DeviceUsed(device.value("device_mac").toString());

    QSqlQuery query(_shared->SqlDatabase());
    query.prepare("insert or replace into DEVICE (mac, name, account, selected, used, battery, battery_time, status) "
                "values (?, ?, ?, ?, ?, ?, ?, ?)");

    query.addBindValue( device.value("device_mac").toString() );
    query.addBindValue( device.value("device_name").toString() );
    query.addBindValue( device.value("user_id").toString() );
    query.addBindValue( selected );
    query.addBindValue( used );
    query.addBindValue( device.value("battery").toString() );
    query.addBindValue( device.value("battery_time").toString() );
    query.addBindValue( device.value("status").toString() );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}
QVariantMap DeviceDatabase_Impl::DeviceInfo(QString& mac)
{
    QMutexLocker locker(_shared->_apiLock);

    QVariantMap device;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select * from DEVICE left join USER on DEVICE.account=USER.account where mac = ?");
    query.addBindValue( mac );

    if(query.exec()){
        QSqlRecord rec = query.record();

        while(query.next()){

            for(int i = 0; i < rec.count(); i ++){
                device.insert(rec.fieldName(i), query.value(i));
            }
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return device;
}

QVariantMap DeviceDatabase_Impl::DeviceInfo(int row)
{
    QMutexLocker locker(_shared->_apiLock);

    QVariantMap device;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select * from DEVICE left join USER on DEVICE.account=USER.account where id = ?");
    query.addBindValue( row );

    if(query.exec()){
        QSqlRecord rec = query.record();

        while(query.next()){

            for(int i = 0; i < rec.count(); i ++){
                device.insert(rec.fieldName(i), query.value(i));
            }
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return device;
}

QVariantList DeviceDatabase_Impl::DeviceList()
{
    QMutexLocker locker(_shared->_apiLock);

    QVariantList list;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select * from DEVICE left join USER on DEVICE.account=USER.account order by id");

    if(query.exec()){
        QSqlRecord rec = query.record();

        while(query.next()){
            QVariantMap device;

            for(int i = 0; i < rec.count(); i ++){
                device.insert(rec.fieldName(i), query.value(i));
            }

            list<<device;
        }

    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return list;
}

int DeviceDatabase_Impl::DeviceCount()
{
    QMutexLocker locker(_shared->_apiLock);

    int count(0);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select count(*) from DEVICE");

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
int DeviceDatabase_Impl::DeviceCount(QString mac)
{
    QMutexLocker locker(_shared->_apiLock);

    int count(0);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select count(*) from DEVICE where mac = ?");
    query.addBindValue( mac );

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

int DeviceDatabase_Impl::DelDevice(QString& mac)
{
    QMutexLocker locker(_shared->_apiLock);

    int row = this->DeviceRow(mac);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("delete from DEVICE where mac = ?" );
    query.addBindValue( mac );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    query.prepare("update DEVICE set id = id-1 where id > ?" );
    query.addBindValue( row );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}

int DeviceDatabase_Impl::DelDevice(int row)
{   
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("delete from DEVICE where id = ?" );
    query.addBindValue( row );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    query.prepare("update DEVICE set id = id-1 where id > ?" );
    query.addBindValue( row );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}

int DeviceDatabase_Impl::DeviceRow(QString mac)
{
    QMutexLocker locker(_shared->_apiLock);

    int row(-1);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select id from DEVICE where mac = ?");
    query.addBindValue( mac );

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
int DeviceDatabase_Impl::SetDeviceSelected(int row, int selected)
{   
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("update DEVICE set selected = ? where id = ?");
    query.addBindValue( selected );
    query.addBindValue( row );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;

}
int DeviceDatabase_Impl::SetDeviceSelected(QString& mac, int selected)
{   
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("update DEVICE set selected = ? where mac = ?");
    query.addBindValue( selected );
    query.addBindValue( mac );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}
int DeviceDatabase_Impl::DeviceSelected(QString& mac)
{
    QMutexLocker locker(_shared->_apiLock);

    int selected(0);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select * from DEVICE where mac = ?");
    query.addBindValue( mac );

    if(query.exec()){
        QSqlRecord rec = query.record();

        int Col = rec.indexOf("selected");

        while(query.next()){
            selected = query.value(Col).toInt();
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return selected;
}
int DeviceDatabase_Impl::SetDeviceUsed(int row, int used)
{   
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("update DEVICE set used = ? where id = ?");
    query.addBindValue( used );
    query.addBindValue( row );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;

}
int DeviceDatabase_Impl::SetDeviceUsed(QString& mac, int used)
{
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("update DEVICE set used = ? where mac = ?");
    query.addBindValue( used );
    query.addBindValue( mac );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}
int DeviceDatabase_Impl::DeviceUsed(QString& mac)
{
    QMutexLocker locker(_shared->_apiLock);

    int used(0);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select * from DEVICE where mac = ?");
    query.addBindValue( mac );

    if(query.exec()){
        QSqlRecord rec = query.record();

        int Col = rec.indexOf("used");

        while(query.next()){
            used = query.value(Col).toInt();
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return used;
}

int DeviceDatabase_Impl::SetDeviceName(int row, QString& name)
{   
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("update DEVICE set name = ? where id = ?");
    query.addBindValue( name );
    query.addBindValue( row );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;

}
int DeviceDatabase_Impl::SetDeviceName(QString& mac, QString& name)
{    
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("update DEVICE set name = ? where mac = ?");
    query.addBindValue( name );
    query.addBindValue( mac );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}
QString DeviceDatabase_Impl::DeviceName(QString& mac)
{
    QMutexLocker locker(_shared->_apiLock);

    QString name;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select * from DEVICE where mac = ?");
    query.addBindValue( mac );

    if(query.exec()){
        QSqlRecord rec = query.record();

        int Col = rec.indexOf("name");

        while(query.next()){
            name = query.value(Col).toString();
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return name;
}

int DeviceDatabase_Impl::SetDeviceBatteryP(int row, int battery)
{   
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("update DEVICE set battery = ? where id = ?");
    query.addBindValue( battery );
    query.addBindValue( row );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;

}
int DeviceDatabase_Impl::SetDeviceBatteryP(QString& mac, int battery)
{    
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("update DEVICE set battery = ? where mac = ?");
    query.addBindValue( battery );
    query.addBindValue( mac );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}
int DeviceDatabase_Impl::DeviceBatteryP(QString& mac)
{
    QMutexLocker locker(_shared->_apiLock);

    int batteryP;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select * from DEVICE where mac = ?");
    query.addBindValue( mac );

    if(query.exec()){
        QSqlRecord rec = query.record();

        int Col = rec.indexOf("battery");

        while(query.next()){
            batteryP = query.value(Col).toInt();
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return batteryP;
}
int DeviceDatabase_Impl::SetDeviceBatteryS(int row, int battery_time)
{   
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("update DEVICE set battery_time = ? where id = ?");
    query.addBindValue( battery_time );
    query.addBindValue( row );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;

}
int DeviceDatabase_Impl::SetDeviceBatteryS(QString& mac, int battery_time)
{   
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("update DEVICE set battery_time = ? where mac = ?");
    query.addBindValue( battery_time );
    query.addBindValue( mac );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}
int DeviceDatabase_Impl::DeviceBatteryS(QString& mac)
{
    QMutexLocker locker(_shared->_apiLock);

    int batteryS;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select * from DEVICE where mac = ?");
    query.addBindValue( mac );

    if(query.exec()){
        QSqlRecord rec = query.record();

        int Col = rec.indexOf("battery_time");

        while(query.next()){
            batteryS = query.value(Col).toInt();
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return batteryS;
}
int DeviceDatabase_Impl::SetDeviceStatus(int row, QString status)
{    
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("update DEVICE set status = ? where id = ?");
    query.addBindValue( status );
    query.addBindValue( row );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;

}
int DeviceDatabase_Impl::SetDeviceStatus(QString mac, QString status)
{  
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("update DEVICE set status = ? where mac = ?");
    query.addBindValue( status );
    query.addBindValue( mac );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}
QString DeviceDatabase_Impl::DeviceStatus(QString mac)
{
    QMutexLocker locker(_shared->_apiLock);

    QString name;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select * from DEVICE where mac = ?");
    query.addBindValue( mac );

    if(query.exec()){
        QSqlRecord rec = query.record();

        int Col = rec.indexOf("status");

        while(query.next()){
            name = query.value(Col).toString();
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return name;
}

int DeviceDatabase_Impl::SetDeviceAccount(int row, QString account)
{
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("update DEVICE set account = ? where id = ?");
    query.addBindValue( account );
    query.addBindValue( row );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;

}
int DeviceDatabase_Impl::SetDeviceAccount(QString mac, QString account)
{
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("update DEVICE set account = ? where mac = ?");
    query.addBindValue( account );
    query.addBindValue( mac );

    if(!query.exec()){
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
        return -1;
    }

    return 0;
}
QString DeviceDatabase_Impl::DeviceAccount(QString mac)
{
    QMutexLocker locker(_shared->_apiLock);

    QString account;

    QSqlQuery query(_shared->SqlDatabase());

    query.prepare("select * from DEVICE where mac = ?");
    query.addBindValue( mac );

    if(query.exec()){
        QSqlRecord rec = query.record();

        int Col = rec.indexOf("account");

        while(query.next()){
            account = query.value(Col).toString();
        }
    }else{
        qDebug()<<query.executedQuery();
        qDebug()<<query.lastError();
    }

    return account;
}
