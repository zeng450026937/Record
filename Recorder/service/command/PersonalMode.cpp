
#include "PersonalMode.h"
#include <QJsonArray>
#include "service/messager/message_base.h"
#include "common/config.h"
#include "service/storage/personal_database_impl.h"
#include "service/storage/database_impl.h"
#include "service/conf_service_impl.h"

#define PM_GET_PERSONAL_LIST        "getPersonalList"
#define PM_GET_ALL_PERSONAL_LIST    "getAllPersonalList"
#define PM_DOWNLOAD_FILE            "downloadFile"

PersonalMode::PersonalMode(MessageBase *pMessage) : 
    CommandModeBase(pMessage,"personal"),
    m_pConfig(Config::GetInstance()),
    m_pConfService(NULL),
    m_pPersonalDB(nullptr)
{
    AddActionProc(PM_GET_PERSONAL_LIST, &PersonalMode::GetPersonalListReply);
    AddActionProc(PM_GET_ALL_PERSONAL_LIST, &PersonalMode::GetAllPersonalListReply);
    AddActionProc(PM_DOWNLOAD_FILE, &PersonalMode::DownloadFileReply);
}

PersonalMode::~PersonalMode()
{
}

void PersonalMode::SetDataBase(DataBase_Impl *pDatabase)
{
    m_pPersonalDB = PersonalDatabase::GetInterface(pDatabase);
}

void PersonalMode::SetConfServiceImpl(ConfServiceImpl *pConfServiceImpl)
{
    m_pConfService = pConfServiceImpl;
}

void PersonalMode::GetPersonalList()
{
    QJsonObject jsData;
    jsData.insert("userId", m_pConfig->GetUser().user_id);
    SendAction(PM_GET_PERSONAL_LIST, jsData);
}

void PersonalMode::GetAllPersoanlList()
{    
    SendAction(PM_GET_ALL_PERSONAL_LIST, QJsonObject());
}

void PersonalMode::DownloadFile(const QString &qstrConferenceUuid, const QString &qstrDeviceUuid, int iStartpos)
{
    QJsonObject jsData;
    jsData.insert("conferenceUuid", qstrConferenceUuid);
    jsData.insert("deviceUuid", qstrDeviceUuid);
    jsData.insert("startpos", iStartpos);
    jsData.insert("userId", m_pConfig->GetUser().user_id);
    SendAction(PM_DOWNLOAD_FILE, jsData);
}

void PersonalMode::DownloadFileReply(bool bResult, QJsonObject jsData)
{
    if (bResult)
    {
        if (jsData.value("result").toInt() == 0) 
        {
            QString qstrConferenceUuid = jsData.value("conferenceUuid").toString();

            int iConferenceTotalSize = m_mapConferenceRecordTotalSize.value(qstrConferenceUuid);
            iConferenceTotalSize += (jsData.value("totalSize").toInt() - jsData.value("startpos").toInt());

            m_mapConferenceRecordTotalSize.insert(qstrConferenceUuid,
                iConferenceTotalSize);

            // TODO : 优化过程中，暂不知道解析出来的数据要干嘛，暂不处理。
        }
        else
        {
            qDebug() << "downlad file error response:" << jsData;
        }
    }
}

void PersonalMode::GetAllPersonalListReply(bool bResult, QJsonObject jsData)
{
    if (bResult)
    {
        QVariantList lsConferenceInfoes = jsData["list"].toVariant().toList();
        foreach(const auto &varInfo, lsConferenceInfoes)
        {
            this->checkPersonal(varInfo.toMap());
        }
    }

    emit all_personal_list_got_trigger(bResult);
}

void PersonalMode::GetPersonalListReply(bool bResult, QJsonObject jsData)
{

    if (bResult)
    {
        QVariantList lsConferenceInfoes = jsData["list"].toVariant().toList();
        foreach(const auto &varInfo , lsConferenceInfoes)
        {
            this->checkPersonal(varInfo.toMap());
        }
    }

    emit personal_list_got_trigger(bResult);
}

void PersonalMode::SendAction(const char *pAction, const QJsonObject &jsData)
{
    m_pMessage->sendMessage("personal", pAction, jsData);
}

void PersonalMode::checkPersonal(QVariantMap& vmRecordInfo)
{    
    QString qstrConferenceUuid = vmRecordInfo.value("conferenceUuid").toString();
    QStringList needed_list;
    needed_list << vmRecordInfo.value("userId").toString();

    bool completed = m_pConfService->checkConferenceFile(qstrConferenceUuid, needed_list);
    vmRecordInfo.insert("completed", completed);
    m_pPersonalDB->AddConference(vmRecordInfo);

    if (!completed && Config::GetInstance()->_auto_download)
    {  
        // TODO:待测试效果
        DownloadFile(qstrConferenceUuid,
            vmRecordInfo["deviceUuid"].toString(),
            vmRecordInfo["startpos"].toInt());
    }
}
