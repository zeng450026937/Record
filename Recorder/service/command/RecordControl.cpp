
#include "RecordControl.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <recorder_shared.h>
#include "service/service_thread.h"
#include "service/user_service_impl.h"
#include "common/config.h"
#include "service/messager/message_base.h"

#define RC_GET_DEVICE_LIST      "getDeviceList"
#define RC_GET_TEMPLATE_LIST    "getTemplateList"

#define  RC_CREATE_CONFERENCE   "createConference"
#define  RC_START_CONFERENCE    "startConference"
#define  RC_PAUSE_CONFERENCE    "pauseConference"
#define  RC_STOP_CONFERENCE     "stopConference"


#define RC_UPDATE_DEVICE_INFO "updateDeviceInfo"

RecordControl::RecordControl(RecorderShared *pRecorderShare, MessageBase *pMessager) : CommandBase(pMessager),m_pRecordShared(pRecorderShare)
{
    AddActionProc(MB_INFO_MODE, RC_GET_DEVICE_LIST, &RecordControl::GetDeviceListReply);
    AddActionProc(MB_CONFERENCE_MODE, RC_GET_TEMPLATE_LIST, &RecordControl::GetTemplateListReply);

    AddActionProc(MB_INFO_MODE, RC_UPDATE_DEVICE_INFO,&RecordControl::UpdateDeviceInfo);

    AddActionProc(MB_CONFERENCE_MODE, RC_CREATE_CONFERENCE, &RecordControl::CreateConferenceReply);
    AddActionProc(MB_CONFERENCE_MODE, RC_START_CONFERENCE, &RecordControl::StartConferenceReply);
    AddActionProc(MB_CONFERENCE_MODE, RC_PAUSE_CONFERENCE, &RecordControl::PauseConferenceReply);
    AddActionProc(MB_CONFERENCE_MODE, RC_STOP_CONFERENCE, &RecordControl::StopConferenceReply);
}

RecordControl::~RecordControl()
{}


void RecordControl::GetDeviceList()
{
    m_pMessage->sendMessage(MB_INFO_MODE, RC_GET_DEVICE_LIST, QJsonObject());
}

void RecordControl::GetDeviceListReply(bool bResult, const QJsonObject &jsData)
{
    QJsonDocument jsDoc(jsData);
    QString qstr = jsDoc.toJson();
    if (bResult) {
        QVariantList lsRecordInfo = jsData["list"].toVariant().toList();
        m_pRecordShared->receive_deviceInfoListGetted(true, lsRecordInfo);
    }
}


void RecordControl::UpdateDeviceInfo(bool bResult, const QJsonObject &jsData) {

    if (bResult)
    {
        QVariantMap lsRecordInfo = jsData.toVariantMap();
        m_pRecordShared->receive_deviceInfoUpdate(lsRecordInfo);
    }
}

void RecordControl::GetTemplateList()
{
    m_pMessage->sendMessage(MB_CONFERENCE_MODE, RC_GET_TEMPLATE_LIST, QJsonObject());
}

void RecordControl::CreateConference(const QVariantMap &vmCreateInfo)
{
    QJsonDocument jsDoc = QJsonDocument::fromVariant(QVariant(vmCreateInfo));
    m_pMessage->sendMessage(MB_INFO_MODE, RC_CREATE_CONFERENCE, jsDoc.object());
}

void RecordControl::CreateConferenceReply(bool bResult, const QJsonObject &jsData)
{
    m_pRecordShared->receive_conferenceCreated(bResult, jsData.toVariantMap());
}

void RecordControl::StartConference(const QString &qstrConferenceUuid)
{
    QJsonObject jsData;
    jsData.insert("uuid", qstrConferenceUuid);
    m_pMessage->sendMessage(MB_INFO_MODE, RC_START_CONFERENCE, QJsonObject());
}

void RecordControl::StartConferenceReply(bool bResult, const QJsonObject &jsData)
{
    m_pRecordShared->receive_conferenceStarted(bResult, jsData.toVariantMap());
}

void RecordControl::PauseConference(const QString &qstrConferenceUuid)
{
    QJsonObject jsData;
    jsData.insert("uuid", qstrConferenceUuid);
    m_pMessage->sendMessage(MB_INFO_MODE, RC_PAUSE_CONFERENCE, QJsonObject());
}

void RecordControl::PauseConferenceReply(bool bResult, const QJsonObject &jsData)
{
    m_pRecordShared->receive_conferencePaused(bResult, jsData.toVariantMap());
}

void RecordControl::StopConference(const QString &qstrConferenceUuid)
{
    QJsonObject jsData;
    jsData.insert("uuid", qstrConferenceUuid);
    m_pMessage->sendMessage(MB_INFO_MODE, RC_STOP_CONFERENCE, jsData);
}

void RecordControl::StopConferenceReply(bool bResult, const QJsonObject &jsData)
{
    m_pRecordShared->receive_conferenceStoped(bResult, jsData.toVariantMap());
}

void RecordControl::GetTemplateListReply(bool bResult, const QJsonObject &jsData)
{
    QJsonDocument jsDoc(jsData);
    QString qstr = jsDoc.toJson();
    if (bResult)
    {
        QVariantList lsRecordInfoes = jsData["list"].toVariant().toList();
        foreach(const auto &varInfo, lsRecordInfoes)
        {
//            m_pRecrodShared->AddTemplateInfo(varInfo.toMap());
        }
    }
    else
    {

    }

}
