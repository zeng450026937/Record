#pragma once

#include <QList>
#include <QVariant>
#include <service/messager/CommandModeBase.h>

class ConfServiceImpl;

class Config;
class MessageBase;
class DataBase_Impl;
class PersonalDatabase;
class PersonalMode : public CommandModeBase
{
    friend class ServiceThread;

    Q_OBJECT

    PersonalMode(MessageBase *pMessage);
public:
    ~PersonalMode();

    void SetDataBase(DataBase_Impl *pDatabase);
    void SetConfServiceImpl(ConfServiceImpl *pConfServiceImpl);

    void GetPersonalList();
    void GetAllPersoanlList();
    void DownloadFile(const QString &qstrConferenceUuid, const QString &qstrDeviceUuid, int iStartpos);

signals:
    void personal_list_got_trigger(bool);
    void all_personal_list_got_trigger(bool);

private:
    void GetAllPersonalListReply(bool bResult, QJsonObject jsData);
    void GetPersonalListReply(bool bResult, QJsonObject jsData); 
    void DownloadFileReply(bool bResult, QJsonObject jsData);
    void SendAction(const char *pAction,const QJsonObject &jsData);

private:
    void checkPersonal(QVariantMap& vmRecordInfo);


    Config              *m_pConfig;
    ConfServiceImpl     *m_pConfService;
    PersonalDatabase    *m_pPersonalDB;
    QMap<QString, int>  m_mapConferenceRecordTotalSize;
};

