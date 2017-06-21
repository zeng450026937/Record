#pragma once

#include <QString>
class DataBase;
class SharedData;

class DownloadDatabase
{
public:
    static DownloadDatabase* GetInterface(DataBase* db);

    bool GetCompeleteStatus(const QString &qstrFileUuid,
        const QString &qstrConferenceUuid,
        const QString &qstrDeviceUuid,
        QString *qstrFile,
        bool     *pCompleted);

    bool GetDownloadInfo(const QString &qstrFileUuid,
            const QString &qstrConferenceUuid,
            const QString &qstrDeviceUuid,
            int     *pid,
            QString *qstrFile);

    int InsertDownloadInfo(int iRecordType,
                            const QString &qstrFileUuid,
                            const QString &qstrConferenceUuid,
                            const QString &qstrDeviceUuid,
                            const QString &qstrDownloadPath);

    bool SetDownloadCompeletedById(int iId);

    virtual int Release() = 0;
protected:
    DownloadDatabase(SharedData* shared):_shared(shared)
    {}
    virtual ~DownloadDatabase()
    {}

private:
    SharedData* _shared;
};
