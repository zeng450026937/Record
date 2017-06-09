#ifndef MESSAGEBASE_H
#define MESSAGEBASE_H

#include <QObject>
#include <QVariantMap>
#include <QJsonObject>
#include <map>

#define MB_MESSAGE_VERSION 1

class MessageBasePrivate;
class MessageBase : public QObject
{
    friend class CommandModeBase;
	Q_OBJECT
public:
    explicit MessageBase(QObject *parent = 0);
    ~MessageBase();

	bool sendMessage(const QString &qstrMode, const QString &qstrAction, const QJsonObject &jsData = QJsonObject());
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

        NotifyDeviceInfoChange,
        DeviceEnvironmentException,
        //costumer command
        ConnectOpen,
        ConnectFail,
        ConnectClose,
        CommandCount
    };

    enum ConnectionStatus
    {
        CS_CONNECTION_OPENED,
        CS_CONNECTION_CLOSED,
        CS_CONNECTION_FAILED
    };

    void connectTo(const QString &qstrHeader, QString uri);
signals:
    void connection_status(int iStatus);
    void notify_binary(unsigned int size, QByteArray& content);

public slots:
    void stopConnection(); 
    void sendCommand(CommandList command, QString receiver, const QVariantMap& data);

private slots:
    void on_message_reply(QString qstrMessage);
    void on_binary_message(unsigned int size,QByteArray content);

private:
    void AddMode(const QString &qstrModeName, CommandModeBase &commandMode);

private:
    MessageBasePrivate* d;
};


#endif // MESSAGEBASE_H
