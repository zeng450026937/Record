#ifndef DOWNLOAD_DATABASE_H
#define DOWNLOAD_DATABASE_H

#include <QVariant>

class DataBase;

class DownloadDatabase {
public:
    static DownloadDatabase* GetInterface(DataBase* db);
    virtual int Release() = 0;

    virtual int AddSegment(int type, QString identity,
                           QString uuid, int meeting_time,
                           int startpos, int status,
                           int data_size, QByteArray& data) = 0;

    virtual int RemoveFile(int type, QString uuid, QString identity) = 0;
    virtual int RemoveConf(int type, QString uuid) = 0;

    virtual int GetFileSize(int type, QString uuid, QString identity) = 0;
    virtual int GetConfSize(int type, QString uuid) = 0;
    virtual QByteArray GetFile(int type, QString uuid, QString identity) = 0;

    virtual QStringList GetCompletedIdentity(int type, QString uuid) = 0;

protected:
    DownloadDatabase(){}
    virtual ~DownloadDatabase(){}

};

#endif // DOWNLOAD_DATABASE_H
