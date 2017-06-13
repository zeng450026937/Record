#include "ConferenceMode.h"

#include <QFile>
#include <QJsonArray>
#include <QDebug>
#include "common/config.h"
#include "service/messager/message_base.h"
#include "storage/include/clip_file_database.h"
#include "storage/include/conferencedatabase.h"
#include "storage/include/download_database.h"
#include "storage/database_impl.h"

#define PSL_GET_CONFERENCE_LIST "getConferenceList"
#define CM_GET_TEMPLATE_LIST    "getTemplateList"
#define CM_DOWNLOAD_FILE        "downloadFile"

ConferenceMode::ConferenceMode(MessageBase *pMessage) : 
    CommandModeBase(pMessage, "conference"),
    m_pConfig(Config::GetInstance()),
    m_pClipFileDB(NULL),
    m_pConferenceDB(NULL),
    m_pDownloadDB(NULL)
{
    AddActionProc(PSL_GET_CONFERENCE_LIST, &ConferenceMode::GetConferenceListReply);
    AddActionProc(CM_GET_TEMPLATE_LIST, &ConferenceMode::GetTemplateListReply);
    AddActionProc(CM_DOWNLOAD_FILE, &ConferenceMode::DownloadFileReply);
}

ConferenceMode::~ConferenceMode()
{
}

void ConferenceMode::SetDataBase(DataBase_Impl *pDatabase)
{
    m_pClipFileDB = ClipFileDatabase::GetInterface(pDatabase);
    m_pDownloadDB = DownloadDatabase::GetInterface(pDatabase);
    m_pConferenceDB = ConferenceDatabase::GetInterface(pDatabase);    
}

void ConferenceMode::GetConferenceList()
{
    SendAction(PSL_GET_CONFERENCE_LIST, QJsonObject());
}

void ConferenceMode::GetTemplateList()
{
    SendAction(CM_GET_TEMPLATE_LIST, QJsonObject());
}

void ConferenceMode::DownloadFile(const QString &qstrConferenceUuid, const QString &qstrDeviceUuid, int iStartpos)
{
    QJsonObject jsData;
    jsData.insert("conferenceUuid", qstrConferenceUuid);
    jsData.insert("deviceUuid", qstrDeviceUuid);
    jsData.insert("startpos", iStartpos);
    jsData.insert("userId", m_pConfig->GetUser().user_id);
    SendAction(CM_DOWNLOAD_FILE, jsData);
}

void ConferenceMode::GetTemplateListReply(bool bResult, QJsonObject jsData)
{
    if (bResult)
    {

    }
    else
    {

    }
}

#include <QJsonDocument>
void ConferenceMode::GetConferenceListReply(bool bResult, QJsonObject jsData)
{
    if (bResult)
    {
        QJsonDocument jsDoc(jsData);
        QString qstr = jsDoc.toJson();

        QJsonObject jsInfo;
        QJsonArray jsInfoArray = jsData["list"].toArray();
        foreach(QJsonValue &jsValue, jsInfoArray)
        {
            jsInfo = jsValue.toObject();
 
            QString uuid = jsInfo.value("uuid").toString();
            QStringList needed_list = jsInfo.value("devices").toString().split(",");
            needed_list.removeDuplicates();

             bool completed = checkConferenceFile(uuid, needed_list);
 
             jsInfo.insert("completed", completed);
             m_pConferenceDB->AddConference(jsInfo.toVariantMap());

            if (!completed && Config::GetInstance()->_auto_download) {
                this->downloadConference(1, uuid);
            }
        }
    }
    else
    {

    }
}

void ConferenceMode::DownloadFileReply(bool bResult, QJsonObject jsData)
{
    if (bResult)
    {
        if (jsData.value("result").toInt() != 0) {

            qDebug() << "downlad file error response:" << jsData;
        }
        else 
        {
            QString qstrConferenceUuid = jsData.value("conferenceUuid").toString();

            int total_size = _conf_total_size_map.value(qstrConferenceUuid);
            total_size += (jsData.value("totalSize").toInt() - jsData.value("startpos").toInt());

            _conf_total_size_map.insert(qstrConferenceUuid,
                total_size);

            // TODO : 优化过程中，暂不知道解析出来的数据要干嘛，暂不处理。
        }
    }
}

bool ConferenceMode::checkConferenceFile(const QString &uuid, QStringList needed)
{
    bool completed = false;

    if (needed.count() == 0) {
        completed = true;
        return completed;
    }

    QStringList exist_list;
    QStringList missing_list;

    this->checkConferenceFile(uuid, exist_list, missing_list);

    if (missing_list.count() > 0) {
        completed = false;
        return completed;
    }

    foreach(QString each, exist_list) {

        if (needed.contains(each)) {
            needed.removeAll(each);
        }
    }

    if (needed.count() == 0) {
        completed = true;
        return completed;
    }

    return completed;
}

int ConferenceMode::checkConferenceFile(const QString &uuid, QStringList &exists, QStringList &missing)
{
    QVariantList list;
    list = m_pClipFileDB->GetConferenceFile(uuid);

    if (list.count() > 0) {
        QString path;
        foreach(QVariant file, list) {
            path = file.toMap().value("path").toString();
            if (QFile(path).exists() && QFile(path).size() > 0) {
                exists << file.toMap().value("identity").toString();
            }
            else {
                missing << file.toMap().value("identity").toString();
            }
        }
    }

    return list.count();
}


void ConferenceMode::downloadConference(int type, const QString &uuid)
{
    QStringList exists;
    QStringList missing;
    QStringList needed;

    //needed total file
    if (type == 1) {
        needed = m_pConferenceDB->ConferenceInfo(uuid).value("devices").toString().split(",");
    }
//     else if (type == 0) {
//         needed << _shared->PersonalDB()->ConferenceInfo(uuid).value("user_id").toString();
//     }

    needed.removeDuplicates();

    //check exists from file database
    this->checkConferenceFile(uuid, exists, missing);
    //check exists from download buffer
    exists += m_pDownloadDB->GetCompletedIdentity(type, uuid);

    exists.removeDuplicates();

    qDebug() << "download conference for:" << uuid;
    qDebug() << "needed list:" << needed;
    qDebug() << "exist file:" << exists;

    foreach(QString item, needed) {
        if (!exists.contains(item)) {

            this->queryBinary(type, uuid, item);
        }
    }
}


void ConferenceMode::queryBinary(int type, QString uuid, QString identity)
{
    int binary_size(0);
    binary_size = m_pDownloadDB->GetFileSize(type, uuid, identity);

    QVariantMap param;


    DownloadFile(uuid, identity, binary_size);
    // this->Execute(MessageBase::DownloadFile, param);
}


void ConferenceMode::SendAction(const char *pAction, const QJsonObject &jsData)
{
    m_pMessage->sendMessage("conference", pAction, jsData);
}
