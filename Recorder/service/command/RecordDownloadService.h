#ifndef BINARY_MODE_H
#define BINARY_MODE_H

#include <QWidget>
#include <QString>
#include <QByteArray>
#include <map>
#include <service/messager/CommandBase.h>
#include <recorder_shared.h>

class Config;
class ConfServiceImpl;
class DataBase_Impl;
class DownloadDatabase;
class ConferenceDatabase;
class ClipFileDatabase;

class RecordDownloadReceiver;
class MessageBase;
class RecordDownloadService :public CommandBase
{
    Q_OBJECT

public:
    static RecordDownloadService * GetInstance();
    RecordDownloadService(MessageBase *pMessager);

    bool DownloadRecord(RecordDownloadReceiver *pDownloadReceiver,
        int iType,
        const QString qstrConferenceTitle,
        const QString &qstrUserId,
        const QString &qstrFileUuid,
        const QString &qstrConferenceUuid,
        const QString &qstrDeviceUuid,
        const QString &qstrCreateTime,
        const QString &qstrFileExtension);

    RecordDownloadReceiver *GetDownloadReciver(
        const QString &qstrFileUuid,
        const QString &qstrConferenceUuid,
        const QString &qstrDeviceUuid);

    bool ResumeDownload(
            int iType,
            const QString &qstrUserId,
            const QString &qstrFileUuid,
            const QString &qstrConferenceUuid,
            const QString &qstrDeviceUuid);

    bool IsExsitReceiver(
        const QString &qstrFileUuid,
        const QString &qstrConferenceUuid,
        const QString &qstrDeviceUuid);

    void SetConfServiceImpl(ConfServiceImpl *pConfServiceImpl);

signals:

    void  conference_receive_data_notify(QString qstrUuid, int iPercent);
    void  startDownloadTick();

protected slots:
    void on_binary_received(QByteArray binary);

protected:
    void timerEvent(QTimerEvent *pEvent);

private:
    void DownloadAck(int iType,const char *pUserId,const char *pFileUuid, const char *pConferenceUuid, const char *pDeviceId, int iStartPos);
    void DownloadRecordReply(bool bResult, const QJsonObject &jsData);

private:
    DownloadDatabase *m_pDownloadDB;
    ConferenceDatabase *m_pConferenceDB;
    ClipFileDatabase *m_pClipFileDB;
    Config *m_pConfig;
    ConfServiceImpl *m_pConfService;

    struct RecordBlockInfo{
        int iRecordType;
        char szUserId[18];
        char szDeviceUuid[33];
        char szFileUuid[33];
        char szConferenceUuid[33];
        int iStartPos;
        int iStatus;
        int iDataSize;
    } *m_pTempRecordInfo;

typedef std::map<QByteArray, RecordDownloadReceiver*> ReciverMap;
    ReciverMap m_mapReceiver;

    int m_iDownloadStatusTimerId;
    static RecordDownloadService *s_pSingleton;
};

#endif  // BINARY_MODE_H
