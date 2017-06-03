#ifndef DOWNLOADDATABASEIMPL_H
#define DOWNLOADDATABASEIMPL_H

#include "include/download_database.h"

class SharedData;

class DownloadDatabaseImpl : public DownloadDatabase
{
public:
    int AddSegment(int type, QString identity,
                   QString uuid, int meeting_time,
                   int startpos, int status,
                   int data_size, QByteArray& data) override;

    int RemoveFile(int type, QString uuid, QString identity) override;
    int RemoveConf(int type, QString uuid) override;

    int GetFileSize(int type, QString uuid, QString identity) override;
    int GetConfSize(int type, QString uuid) override;
    QByteArray GetFile(int type, QString uuid, QString identity) override;

    QStringList GetCompletedIdentity(int type, QString uuid) override;

protected:
    DownloadDatabaseImpl(SharedData* shared):_shared(NULL){ _shared = shared; }
    ~DownloadDatabaseImpl() override {}

private:
    SharedData* _shared;
};

#endif // DOWNLOADDATABASEIMPL_H
