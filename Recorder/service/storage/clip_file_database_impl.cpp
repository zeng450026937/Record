#include "clip_file_database_impl.h"
#include <QDebug>
#include <QtSql>
#include "database_impl.h"
#include "shareddata.h"

// static
ClipFileDatabase* ClipFileDatabase::GetInterface(DataBase* db) {
  DataBase_Impl* s = static_cast<DataBase_Impl*>(db);
  s->AddRef();
  return s;
}

int ClipFileDatabaseImpl::AddFile(QString uuid, int start_time, QString path) {
  QMutexLocker locker(_shared->_apiLock);

  QSqlQuery query(_shared->SqlDatabase());
  query.prepare(
      "insert or replace into tb_clip_file (uuid, start_time, path) "
      "values (?, ?, ?)");

  query.addBindValue(uuid);
  query.addBindValue(start_time);
  query.addBindValue(path);

  if (!query.exec()) {
    qDebug() << query.executedQuery();
    qDebug() << query.lastError();
    return -1;
  }

  return 0;
}
int ClipFileDatabaseImpl::RemoveConferenceFile(QString uuid) {
  QMutexLocker locker(_shared->_apiLock);

  QSqlQuery query(_shared->SqlDatabase());

  query.prepare("delete from tb_clip_file where uuid = ?");
  query.addBindValue(uuid);

  if (!query.exec()) {
    qDebug() << query.executedQuery();
    qDebug() << query.lastError();
    return -1;
  }

  return 0;
}

QVariantList ClipFileDatabaseImpl::GetFileList(QString uuid) {
  QMutexLocker locker(_shared->_apiLock);

  QVariantList list;

  QSqlQuery query(_shared->SqlDatabase());

  query.prepare(
      "select * from tb_clip_file where uuid = ? order by start_time");
  query.addBindValue(uuid);

  if (query.exec()) {
    QSqlRecord rec = query.record();

    while (query.next()) {
      QVariantMap file;
      for (int i = 0; i < rec.count(); i++) {
        file.insert(rec.fieldName(i), query.value(i));
      }
      list << file;
    }

  } else {
    qDebug() << query.executedQuery();
    qDebug() << query.lastError();
  }

  return list;
}

QVariantList ClipFileDatabaseImpl::GetConferenceFile(QString uuid) {
  QMutexLocker locker(_shared->_apiLock);

  QVariantList list;

  QSqlQuery query(_shared->SqlDatabase());

  query.prepare(
      "select * from tb_clip_file where uuid = ? AND start_time < '0' order by "
      "start_time");
  query.addBindValue(uuid);

  if (query.exec()) {
    QSqlRecord rec = query.record();

    while (query.next()) {
      QVariantMap file;
      for (int i = 0; i < rec.count(); i++) {
        file.insert(rec.fieldName(i), query.value(i));
      }
      list << file;
    }
  } else {
    qDebug() << query.executedQuery();
    qDebug() << query.lastError();
  }

  return list;
}
QVariantList ClipFileDatabaseImpl::GetClipFile(QString uuid) {
  QMutexLocker locker(_shared->_apiLock);

  QVariantList list;

  QSqlQuery query(_shared->SqlDatabase());

  query.prepare(
      "select * from tb_clip_file where uuid = ? AND start_time >= '0' order "
      "by start_time");
  query.addBindValue(uuid);

  if (query.exec()) {
    QSqlRecord rec = query.record();

    while (query.next()) {
      QVariantMap file;
      for (int i = 0; i < rec.count(); i++) {
        file.insert(rec.fieldName(i), query.value(i));
      }
      list << file;
    }
  } else {
    qDebug() << query.executedQuery();
    qDebug() << query.lastError();
  }

  return list;
}
