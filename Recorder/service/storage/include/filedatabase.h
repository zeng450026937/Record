#ifndef FILEDATABASE_H
#define FILEDATABASE_H

#include <QVariant>

class DataBase;

class FileDatabase {
public:
    static FileDatabase* GetInterface(DataBase* db);
    virtual int Release() = 0;

    virtual int AddFile(QVariantMap& file) = 0;
    virtual QVariantMap FileInfo(QString& path) = 0;

    virtual QVariantList GetFileList() = 0;
    virtual QVariantList GetFileList(QString& uuid) = 0;

    virtual int FileCount() = 0;
    virtual int FileCount(QString& uuid) = 0;

    virtual int DelFile(QString& path) = 0;

    virtual int FileRow(QString path) = 0;

    virtual int SetFileUuid(QString& path, QString& uuid) = 0;
    virtual QString FileUuid(QString& path) = 0;

    virtual int SetFileDeviceName(QString& path, QString& device_name) = 0;
    virtual QString FileDeviceName(QString& path) = 0;

    virtual int SetFilePath(QString& path_old, QString& path_new) = 0;
    virtual QString FilePath(QString& path) = 0;

protected:
    FileDatabase(){}
    virtual ~FileDatabase(){}

};

#endif // FILEDATABASE_H
