#ifndef MESSAGEBASE_H
#define MESSAGEBASE_H

#include <QObject>

class MessageBasePrivate;

class MessageBase : public QObject
{
    Q_OBJECT
public:
    explicit MessageBase(QObject *parent = 0);
    ~MessageBase();

	bool sendMessage(const QString &qstrMode,const QString &qstrAction, const QVariantMap &qvmData);
    enum CommandList{
        //common command
        CreateConference = 0,
        StartConference,
        PauseConference,
        StopConference,
        SetConferenceInfo,
        DelConferenceInfo,
        GetConferenceInfo,
        GetConferenceList,

        NotifyPersonRecordAdd,
        NotifyPersonRecordUpdate,
        NotifyPersonRecordDelete,
        GetPersonalList,
        GetAllPersonalList,

        AddDevice,
        RemoveDevice,
        GetDeviceList,

        AddTemplateInfo,
        SetTemplateInfo,
        DelTemplateInfo,
        GetTemplateInfo,
        GetTemplateList,

        AddMarkInfo,
        SetMarkInfo,
        DelMarkInfo,
        GetMarkinfo,
        GetMarkList,

        DownloadFile,
        DownloadFileAck,
        NotifyNewDataRecv,
        //special command
        LoginDevice,
        HeartBeat,
        NotifyDeviceInfoChange,
        DeviceEnvironmentException,
        //costumer command
        ConnectOpen,
        ConnectFail,
        ConnectClose,
        CommandCount
    };

signals:
    void notify_command(int command, bool result, QVariantMap& data);
    void notify_binary(unsigned int size, QByteArray& content);

public slots:
    void connectTo(QString uri = "");
    void stopConnection();
    void heartBeat();
    void userLogin(QString account, QString password);  
    void sendCommand(CommandList command, QString receiver, const QVariantMap& data);

private slots:
    void on_connect_open();
    void on_connect_fail();
    void on_connect_close();
    void on_text_message(QString text);
    void on_binary_message(unsigned int size,QByteArray content);

protected:
    void parseText(const QString text);

private:
    MessageBasePrivate* d;
};

#endif // MESSAGEBASE_H
