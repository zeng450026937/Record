#pragma once

#include <QMap>
#include <service/messager/CommandModeBase.h>

class Config;
class MessageBase;
class DataBase_Impl;
class ConferenceDatabase;
class DownloadDatabase;
class ClipFileDatabase;
class TemplateDatabase;
class ConfServiceImpl;
class ConferenceMode : public CommandModeBase
{
    friend class ServiceThread;

    Q_OBJECT

    ConferenceMode(MessageBase *pMessage);
public:
    ~ConferenceMode();

    void SetDataBase(DataBase_Impl *pDatabase);
//        DownloadDatabase    *m_pDownloadDB;
//    ConferenceDatabase *m_pConferenceDB;
//    ClipFileDatabase *m_pClipFileDB;

signals:
    void conference_list_got_trigger(bool result);
    void template_list_got_trigger(bool bResult);

public:
    void GetConferenceList();
    void GetTemplateList();
    void DownloadFile(const QString &qstrConferenceUuid,const QString &qstrDeviceUuid,int iStartpos);

private:
    struct DownLoadInfo {
        int type;
        QString uuid;
        QString identity;
        int sample_rate;
        int channal;

        bool DownLoadInfo::operator==(const DownLoadInfo &rhs) {            
            return (rhs.uuid == uuid && rhs.identity == identity);
        }
    };

private:
    void checkConference(QVariantMap& vmRecordInfo);
//     bool checkConferenceFile(const QString &uuid,QStringList needed);
//     int checkConferenceFile(const QString &uuid, QStringList &exists, QStringList &missing);
    

    void DownloadFileReply(bool bResult, QJsonObject jsData); 

    void GetTemplateListReply(bool bResult, QJsonObject jsData);
    void GetConferenceListReply(bool bResult, QJsonObject jsData);
    void SendAction(const char *pAction, const QJsonObject &jsData);

    DownloadDatabase    *m_pDownloadDB;
    ConferenceDatabase  *m_pConferenceDB;
    ClipFileDatabase    *m_pClipFileDB;
    TemplateDatabase    *m_pTemplateDB;
    Config              *m_pConfig;
    ConfServiceImpl     *m_pConfService;

    QMap<QString, int>  _conf_total_size_map;
};

