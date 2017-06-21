#pragma once

#include <stdio.h>
#include <QWidget>



class RecordDownloadReceiver : public QWidget
{
    friend class RecordDownloadReceiverPrivate;
    friend class RecordDownloadService;

    Q_OBJECT
public:
    enum ErrorCode
    {
        EC_DOWNLOADING,
        EC_FILE_UNEXSISTS,
        EC_ARGS_INVALID
    };

    enum DownloadStatus
    {
        DS_UNEXSITS,
        DS_UNCOMPLETED,
        DS_COMPELETED
    };

signals:
    void download_prompt(QString qstrInfo);
    void downloading_tick(int iPercent,int iDownloadPerSecond);

protected:
    RecordDownloadReceiver();
    virtual ~RecordDownloadReceiver();

    int GetDownloadStatus(const QString &qstrFileUuid,
        const QString &qstrConferenceUuid,
        const QString &qstrDeviceUuid);
protected slots:
    virtual void onDownloadPrompt(QString qstrInfo) = 0;

private:

    bool CreateReciveData(int iType,
                        const QString &qstrFileExtension,
                        const QString &qstrFileUuid,
                        const QString &qstrConferenceUuid,
                        const QString &qstrDeviceUuid,
                        QString qstrCreateTime,
                        int *iStartPos);


    bool StartReceiveTrigger(int iResult,int iFileSize);

    bool WriteData(const char *pData,int iDataSize,bool bCompleted);

    void TickDownloadStatus();

    RecordDownloadReceiverPrivate *m;
};