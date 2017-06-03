#ifndef FILEDATABASE_IMPL_H
#define FILEDATABASE_IMPL_H

#include "include/filedatabase.h"

class SharedData;

class FileDatabase_Impl : public FileDatabase
{
public:
    int AddFile(QVariantMap& file) override;
    QVariantMap FileInfo(QString& path) override;

    QVariantList GetFileList() override;
    QVariantList GetFileList(QString& uuid) override;

    int FileCount() override;
    int FileCount(QString& uuid) override;

    int DelFile(QString& path) override;

    int FileRow(QString path) override;

    int SetFileUuid(QString& path, QString& uuid) override;
    QString FileUuid(QString& path) override;

    int SetFileDeviceName(QString& path, QString& device_name) override;
    QString FileDeviceName(QString& path) override;

    int SetFilePath(QString& path_old, QString& path_new) override;
    QString FilePath(QString& path) override;

protected:
    FileDatabase_Impl(SharedData* shared):_shared(NULL){ _shared = shared; }
    ~FileDatabase_Impl() override {}

private:
    SharedData* _shared;
};

#endif // FILEDATABASE_IMPL_H
