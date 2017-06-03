#ifndef CONFSERVICE_H
#define CONFSERVICE_H

#include <QObject>
#include <QVariant>

class ConfService : public QObject
{
    Q_OBJECT
signals:
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

    void downloadConferenceCompleted(int type, QString uuid, int percentage, int speed, int completed);

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
    virtual void createConference(QVariantMap info) = 0;
    virtual void startConference(QString uuid) = 0;
    virtual void pauseConference(QString uuid) = 0;
    virtual void stopConference(QString uuid) = 0;

    virtual void setConferenceInfo(QString uuid, QVariantMap info) = 0;
    virtual void delConferenceInfo(QString uuid) = 0;
    virtual void getConferenceInfo(QString uuid) = 0;
    virtual void getConferenceList() = 0;

    virtual void getPersonalList(QString user_id) = 0;
    virtual void getAllPersonalList() = 0;

    virtual void addTemplateInfo(QVariantMap info) = 0;
    virtual void setTemplateInfo(QVariantMap info) = 0;
    virtual void delTemplateInfo(QString uuid) = 0;
    virtual void getTemplateInfo(QString uuid) = 0;
    virtual void getTemplateList() = 0;

    virtual void addMarkInfo(QString uuid, QVariantMap mark) = 0;
    virtual void setMarkInfo(QString uuid, QVariantMap mark) = 0;
    virtual void delMarkInfo(QString uuid, QVariantMap mark) = 0;
    virtual void getMarkInfo(QString uuid) = 0;
    virtual void getMarkList() = 0;

    virtual void getDeviceInfoList() = 0;

    virtual void downloadConference(int type, QString uuid) = 0;

    virtual void setDownloadFolder(QString folder) = 0;

public:
    virtual void checkConference(QVariantMap& conf) = 0;
    virtual void checkPersonal(QVariantMap& conf) = 0;

    virtual QString downloadFolder() const = 0;
    virtual QString outputFolder(int type, QString uuid) const = 0;

    //return local data immediately
    virtual QVariantMap conferenceInfo(QString uuid) const = 0;
    virtual QVariantList conferenceList() const = 0;
    virtual QVariantMap personalConfInfo(QString uuid) const = 0;
    virtual QVariantList personalConfList() const = 0;
    virtual QVariantMap templateInfo(QString uuid) const = 0;
    virtual QVariantList templateList() const = 0;
    virtual QVariantList markInfo(QString uuid) const = 0;
    virtual QVariantList deviceInfoList() const = 0;
    virtual QVariantMap deviceInfo(QString mac) const = 0;
    virtual QVariantList fileList(QString uuid) const = 0;

    virtual int addClipFile(QVariantMap& file) = 0;

protected:
    explicit ConfService(QObject *parent = 0) : QObject(parent){ }
    ~ConfService() { }
};


#endif // CONFSERVICE_H
