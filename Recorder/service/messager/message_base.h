#ifndef MESSAGEBASE_H
#define MESSAGEBASE_H

#include <QObject>
#include <QVariantMap>
#include <QJsonObject>

#define MB_MESSAGE_VERSION 1

class ICommandMode : public QObject
{
	Q_OBJECT
signals:
	void action_trigger(QJsonObject);

protected slots:
	virtual void on_action_trigger(QJsonObject jsRoot) = 0;
};

class MessageBasePrivate;
class MessageBase : public QObject
{
	Q_OBJECT
public:
    explicit MessageBase(QObject *parent = 0);
    ~MessageBase();

	bool AddMode(const QString &qstrModeName,ICommandMode &msgMode);

	bool sendMessage(const QString &qstrAction, const QString &qstrMode, const QVariantMap &qvmData = QVariantMap());
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
//         LoginDevice,
//         HeartBeat,
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
    void connectTo(const QString &qstrHeader,QString uri);
    void stopConnection();
   // void heartBeat();
    // void userLogin(QString account, QString password);  
    void sendCommand(CommandList command, QString receiver, const QVariantMap& data);

private slots:
    void on_connect_open();
    void on_connect_fail();
    void on_connect_close();
    void on_message_reply(QString qstrMessage);
    void on_binary_message(unsigned int size,QByteArray content);

private:
    MessageBasePrivate* d;
};


#endif // MESSAGEBASE_H
