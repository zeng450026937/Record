#pragma once

#include <QString>
class DataBase;
class SharedData;

class DownloadDatabase
{
public:
    static DownloadDatabase* GetInterface(DataBase* db);

    int InsertDownloadInfo(int iRecordType,
                            const QString &qstrFileUuid,
                            const QString &qstrConferenceUuid,
                            const QString &qstrDeviceUuid);

    int AddSegment(int type, QString identity,
                   QString uuid, int meeting_time,
                   int startpos, int status,
                   int data_size, QByteArray& data);

    int RemoveFile(int type, QString uuid, QString identity);
    int RemoveConf(int type, QString uuid);

    int GetFileSize(int type, QString uuid, QString identity);
    int GetConfSize(int type, QString uuid);
    QByteArray GetFile(int type, QString uuid, QString identity);

    QStringList GetCompletedIdentity(int type, QString uuid);

    virtual int Release() = 0;
protected:
    DownloadDatabase(SharedData* shared):_shared(shared)
    {}
    virtual ~DownloadDatabase()
    {}

private:
    SharedData* _shared;
};
