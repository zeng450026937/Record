#include "download_database.h"
#include "database_impl.h"
#include "shareddata.h"
#include "include\data_base.h"
#include <QtSql>
#include <QDebug>

//static
DownloadDatabase* DownloadDatabase::GetInterface(DataBase* db)
{
    DataBase_Impl* s = static_cast<DataBase_Impl*>(db);
    s->AddRef();
    return s;
}

bool DownloadDatabase::GetCompeleteStatus(const QString &qstrFileUuid, 
    const QString &qstrConferenceUuid, const QString &qstrDeviceUuid, QString *qstrFile, bool *pCompleted)
{
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());
    query.prepare("select path,completed from tb_record_download where file_uuid is ? and conference_uuid is ?"
        " and device_uuid is ?;");

    query.addBindValue(qstrFileUuid);
    query.addBindValue(qstrConferenceUuid);
    query.addBindValue(qstrDeviceUuid);
    if (query.exec())
    {
        if (query.next())
        {
            *qstrFile = query.value(0).toString(); 
            *pCompleted = query.value(1).toInt() != 0;
            return true;
        }
    }

    return false;
}

bool DownloadDatabase::GetDownloadInfo(const QString &qstrFileUuid, const QString &qstrConferenceUuid, 
    const QString &qstrDeviceUuid,
    int     *pid, 
    QString *qstrFile)
{

    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());
    query.prepare("select id,path from tb_record_download where file_uuid is ? and conference_uuid is ?"
        " and device_uuid is ?;");

    query.addBindValue(qstrFileUuid.isEmpty() ? QVariant() : qstrFileUuid);
    query.addBindValue(qstrConferenceUuid.isEmpty() ? QVariant() : qstrConferenceUuid);
    query.addBindValue(qstrDeviceUuid.isEmpty() ? QVariant() : qstrDeviceUuid);
    if (query.exec())
    {
        if (query.next())
        {
            *pid = query.value(0).toInt();
            *qstrFile = query.value(1).toString();
            return true;
        }
    }

    return false;
}

int DownloadDatabase::InsertDownloadInfo(int iRecordType,
    const QString &qstrFileUuid, 
    const QString &qstrConferenceUuid, 
    const QString &qstrDeviceUuid,
    const QString &qstrDownloadPath)
{
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());
    query.prepare("insert or replace into tb_record_download (recordType, file_uuid, "
        "conference_uuid, device_uuid, path) "
        "values (?, ?, ?, ?, ?);");

    query.addBindValue(iRecordType);
    query.addBindValue(qstrFileUuid);
    query.addBindValue(qstrConferenceUuid);
    query.addBindValue(qstrDeviceUuid);
    query.addBindValue(qstrDownloadPath);

    if (!query.exec()) {
        qDebug() << query.executedQuery();
        qDebug() << query.lastError();
        return 0;
    }

    return query.lastInsertId().toInt();
}

bool DownloadDatabase::SetDownloadCompeletedById(int iId)
{
    QMutexLocker locker(_shared->_apiLock);

    QSqlQuery query(_shared->SqlDatabase());
    query.prepare("update tb_record_download set completed = 1 where id=?; ");

    query.addBindValue(iId);

    if (!query.exec()) {
        qDebug() << query.executedQuery();
        qDebug() << query.lastError();
        return false;
    }

    return true;
}