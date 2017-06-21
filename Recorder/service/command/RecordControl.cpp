
#include "RecordControl.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <recorder_shared.h>
#include "service/service_thread.h"
#include "service/user_service_impl.h"
#include "common/config.h"
#include "service/messager/message_base.h"

#define RC_GET_DEVICE_LIST    "getDeviceList"
#define RC_GET_TEMPLATE_LIST    "getTemplateList"

RecordControl::RecordControl(MessageBase *pMessager) : CommandBase(pMessager)
{
    AddActionProc(MB_INFO_MODE, RC_GET_DEVICE_LIST, &RecordControl::GetDeviceListReply);
    AddActionProc(MB_CONFERENCE_MODE, RC_GET_TEMPLATE_LIST, &RecordControl::GetTemplateListReply);
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
    if (bResult)
    {
        QVariantList lsRecordInfoes = jsData["list"].toVariant().toList();
        foreach(const auto &varInfo, lsRecordInfoes)
        {
            m_pRecrodShared->AddTemplateInfo(varInfo.toMap());
        }
    }
    else
    {
        // TODO:提示获取失败
    }
}

void RecordControl::GetTemplateList()
{
    m_pMessage->sendMessage(MB_CONFERENCE_MODE, RC_GET_TEMPLATE_LIST, QJsonObject());
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
            m_pRecrodShared->AddTemplateInfo(varInfo.toMap());
        }
    }
    else
    {

    }

}
