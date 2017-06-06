#ifndef CONFSERVICEIMPL_H
#define CONFSERVICEIMPL_H

#include "include/conf_service.h"
#include <QTime>

class ServiceThreadPrivate;

class ConfServiceImpl : public ConfService
{
	Q_OBJECT
public:
    explicit ConfServiceImpl(ServiceThreadPrivate* shared, QObject *parent = 0);
    ~ConfServiceImpl(){}

public slots:
    void createConference(QVariantMap info) override;
    void startConference(QString uuid) override;
    void pauseConference(QString uuid) override;
    void stopConference(QString uuid) override;

    void setConferenceInfo(QString uuid, QVariantMap info) override;
    void delConferenceInfo(QString uuid) override;
    void getConferenceInfo(QString uuid) override;
    void getConferenceList() override;

    void getPersonalList(QString user_id) override;
    void getAllPersonalList() override;

    void addTemplateInfo(QVariantMap info) override;
    void setTemplateInfo(QVariantMap info) override;
    void delTemplateInfo(QString uuid) override;
    void getTemplateInfo(QString uuid) override;
    void getTemplateList() override;

    void addMarkInfo(QString uuid, QVariantMap mark) override;
    void setMarkInfo(QString uuid, QVariantMap mark) override;
    void delMarkInfo(QString uuid, QVariantMap mark) override;
    void getMarkInfo(QString uuid) override;
    void getMarkList() override;

    void getDeviceInfoList() override;

    void downloadConference(int type, QString uuid) override;

    void setDownloadFolder(QString folder) override;

public:
    void checkConference(QVariantMap& conf) override;
    void checkPersonal(QVariantMap& conf) override;

    QString downloadFolder() const override;
    QString outputFolder(int type, QString uuid) const override;
    //return local data immediately
    QVariantMap conferenceInfo(QString uuid) const override;
    QVariantList conferenceList() const override;
    QVariantMap personalConfInfo(QString uuid) const override;
    QVariantList personalConfList() const override;
    QVariantMap templateInfo(QString uuid) const override;
    QVariantList templateList() const override;
    QVariantList markInfo(QString uuid) const override;
    QVariantList deviceInfoList() const override;
    QVariantMap deviceInfo(QString mac) const override;
    QVariantList fileList(QString uuid) const override;

    int addClipFile(QVariantMap& file) override;

protected slots:
    void parseCommand(int command, bool result, QVariantMap& info);
    void parseBinary(unsigned int size, QByteArray& content);

protected:
    void Execute(int command, QVariantMap param);
    void downloadFile(int type, QString uuid, QString identity);
    void downloadNextFile(QString uuid, QString identity);
    void queryBinary(int type, QString uuid, QString identity);
    void monitorDownload(int type, QString uuid, QString identity, int data_size);
    void monitorDisconnect(int type, QString uuid, QString identity, int percentage, int data_size, bool completed);
    bool checkConferenceAlive(int type, QString uuid);
    bool checkConferenceFile(QString uuid, QStringList needed);
    int  checkConferenceFile(QString uuid, QStringList& exists, QStringList& missing);

protected:
    virtual void timerEvent(QTimerEvent *e);

private:
    enum{ DISCONNECT_INTERVAL = 1000 };//ms
    bool    auto_download;
    ServiceThreadPrivate* _shared;
    struct DownLoadInfo{
        int type;
        QString uuid;
        QString identity;
        int sample_rate;
        int channal;

        bool DownLoadInfo::operator==(const DownLoadInfo &rhs){
            bool equal(false);
            if(rhs.uuid == uuid && rhs.identity == identity)
                equal = true;
            return equal;
        }
    };
    QList<DownLoadInfo> _download_list;

    QMap<QString, QList<DownLoadInfo>> _download_info_map;

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
