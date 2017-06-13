#ifndef CONFSERVICEIMPL_H
#define CONFSERVICEIMPL_H

#include <QTime>
#include <QObject>
#include <QMap>
#include <QVariant>

class ServiceThreadPrivate;

class ConfServiceImpl : public QObject
{
	Q_OBJECT
public:
    explicit ConfServiceImpl(ServiceThreadPrivate* shared, QObject *parent = 0);
    ~ConfServiceImpl(){}


  signals :
    void conferenceCreated(bool result, QVariantMap info);
    void conferenceStarted(bool result, QVariantMap info);
    void conferencePaused(bool result, QVariantMap info);
    void conferenceStoped(bool result, QVariantMap info);

    void conferenceInfoSetted(bool result, QVariantMap info);
    void conferenceInfoDeleted(bool result, QVariantMap info);
    void conferenceInfoGot(bool result, QVariantMap info);
    void conferenceListGot(bool result, QVariantList list);

    void personalConfCreated(bool result, QVariantMap info);
    void personalConfSetted(bool result, QVariantMap info);
    void personalConfDeleted(bool result, QVariantMap info);
    void personalListGot(bool result, QVariantList list);
    void allPersonalListGot(bool result, QVariantList list);

    void downloadConferenceCompleted(int type, QString qstrConferenceUuid, int percentage, int speed, int completed);

    void templateInfoAdded(bool result, QVariantMap info);
    void templateInfoSetted(bool result, QVariantMap info);
    void templateInfoDeleted(bool result, QVariantMap info);
    void templateInfoGot(bool result, QVariantMap info);
    void templateListGot(bool result, QVariantList list);

    void markInfoAdded(bool result, QVariantMap info);
    void markInfoSetted(bool result, QVariantMap info);
    void markInfoDeleted(bool result, QVariantMap info);
    void markInfoGot(bool result, QVariantList info);
    void markListGot(bool result, QVariantList info);

    void deviceInfoUpdate(QVariantMap info);
    void deviceInfoListGetted(bool result, QVariantList list);

    void deviceEnvironmentUpdate(QVariantMap info);

public slots:
    void createConference(QVariantMap info);
    void startConference(QString qstrConferenceUuid);
    void pauseConference(QString qstrConferenceUuid);
    void stopConference(QString qstrConferenceUuid);

    void setConferenceInfo(QString qstrConferenceUuid, QVariantMap info);
    void delConferenceInfo(QString qstrConferenceUuid);
    void getConferenceInfo(QString qstrConferenceUuid);
    void getConferenceList();

    // void getPersonalList(QString user_id) ;
    void getAllPersonalList();

    void addTemplateInfo(QVariantMap info);
    void setTemplateInfo(QVariantMap info);
    void delTemplateInfo(QString qstrConferenceUuid) ;
    void getTemplateInfo(QString qstrConferenceUuid) ;
    void getTemplateList() ;

    void addMarkInfo(QString qstrConferenceUuid, QVariantMap mark) ;
    void setMarkInfo(QString qstrConferenceUuid, QVariantMap mark) ;
    void delMarkInfo(QString qstrConferenceUuid, QVariantMap mark) ;
    void getMarkInfo(QString qstrConferenceUuid) ;
    void getMarkList() ;

    void getDeviceInfoList() ;

    void downloadConference(int type, QString qstrConferenceUuid) ;

    void setDownloadFolder(QString folder) ;

public:
    void checkConference(QVariantMap& conf) ;
    void checkPersonal(QVariantMap& conf) ;

    QString downloadFolder() const ;
    QString outputFolder(int type, QString qstrConferenceUuid) const ;
    //return local data immediately
    QVariantMap conferenceInfo(QString qstrConferenceUuid) const ;
    QVariantList conferenceList() const ;
    QVariantMap personalConfInfo(QString qstrConferenceUuid) const ;
    QVariantList personalConfList() const ;
    QVariantMap templateInfo(QString qstrConferenceUuid) const ;
    QVariantList templateList() const ;
    QVariantList markInfo(QString qstrConferenceUuid) const ;
    QVariantList deviceInfoList() const ;
    QVariantMap deviceInfo(QString mac) const ;
    QVariantList fileList(QString qstrConferenceUuid) const ;

    int addClipFile(QVariantMap& file) ;

public:

    bool checkConferenceFile(QString qstrConferenceUuid, QStringList needed);

protected slots:
    void parseCommand(int command, bool result, QVariantMap& info);
    void parseBinary(unsigned int size, QByteArray& content);

protected:
    void Execute(int command, QVariantMap param);
    // void downloadFile(int type, QString uuid, QString identity);
    // void downloadNextFile(QString uuid, QString identity);
    void queryBinary(int type, QString qstrConferenceUuid, QString identity);
    void monitorDownload(int type, QString qstrConferenceUuid, QString identity, int data_size);
    void monitorDisconnect(int type, QString qstrConferenceUuid, QString identity, int percentage, int data_size, bool completed);
    bool checkConferenceAlive(int type, QString qstrConferenceUuid);
    int  checkConferenceFile(QString qstrConferenceUuid, QStringList& exists, QStringList& missing);

protected:
    virtual void timerEvent(QTimerEvent *e);

private:
    enum{ DISCONNECT_INTERVAL = 1000 };//ms
    bool    auto_download;
    ServiceThreadPrivate* _shared;
    struct DownLoadInfo{
        int type;
        QString qstrConferenceUuid;
        QString identity;
        int sample_rate;
        int channal;

        bool DownLoadInfo::operator==(const DownLoadInfo &rhs){
            bool equal(false);
            if(rhs.qstrConferenceUuid == qstrConferenceUuid && rhs.identity == identity)
                equal = true;
            return equal;
        }
    };
    QList<DownLoadInfo> _download_list;

    // QMap<QString, QList<DownLoadInfo>> _download_info_map;

    QMap<QString, int>  _conf_size_map;
    QMap<QString, int>  _conf_total_size_map;

    QString _folder;
    struct Monitor{
        QTime last_time;
        bool completed;
        int saved_data;
        int idle;
        int alive;
        int percentage;
        int speed;
        int type;
        QString uuid;
        QString identity;
    };
    QMap<QString, Monitor*>  _monitor_map;

    int _monitor_timer;
    int _update_timer;
};

#endif // CONFSERVICEIMPL_H
