#pragma once

#include <stdio.h>
#include <QObject>

class RecordDownloadReceiver : public QObject {
    friend class RecordDownloadReceiverPrivate;
    friend class RecordDownloadService;

    Q_OBJECT
public:
    enum ErrorCode { EC_DOWNLOADING, EC_FILE_UNEXSISTS, EC_ARGS_INVALID };

    enum DownloadStatus { DS_UNEXSITS, DS_UNCOMPLETED, DS_COMPELETED };

public:
    RecordDownloadReceiver();
    ~RecordDownloadReceiver();

    static int GetDownloadStatus(const QString &qstrFileUuid,
        const QString &qstrConferenceUuid,
        const QString &qstrDeviceUuid);

signals:
    void download_prompt(QString qstrInfo);
    void downloading_tick(int iPercent, int iDownloadPerSecond);


private:
    bool CreateReciveData(int iType,
        const QString &qstrTitle,
        const QString &qstrUserName,
        const QString &qstrFileExtension,
        const QString &qstrFileUuid,
        const QString &qstrConferenceUuid,
        const QString &qstrDeviceUuid, QString qstrCreateTime,
        int *iStartPos);

    int GetDownloadedPercent();

    bool StartReceiveTrigger(int iResult, int iFileSize);

    bool WriteData(const char *pData, int iDataSize, bool bCompleted);

    void TickDownloadStatus();

    RecordDownloadReceiverPrivate *m;
};
