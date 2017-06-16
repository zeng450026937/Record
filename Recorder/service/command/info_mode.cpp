
#include "info_mode.h"
#include <QJsonDocument>
#include <QJsonObject>
#include "service/service_thread.h"
#include "service/user_service_impl.h"
#include "common/config.h"
#include "service/messager/message_base.h"

#define IMAC_HEART_BEAT         "heartBeat"
#define IMAC_GET_DEVICE_LIST    "getDeviceList"

InfoMode::InfoMode(MessageBase *pMessager) : CommandBase(pMessager)
{
    AddActionProc(MB_INFO_MODE,IMAC_HEART_BEAT, &InfoMode::HandleHeartBeat);
    AddActionProc(MB_INFO_MODE,IMAC_GET_DEVICE_LIST, &InfoMode::GetDeviceListReply);
}

InfoMode::~InfoMode()
{}

void InfoMode::ConnectToServer()
{
    Config *pConfig = Config::GetInstance();
    Config::USER &user = pConfig->GetUser();

    QJsonObject jsCommand;
    jsCommand.insert("mode", "auth");
    jsCommand.insert("action", "login");

    QJsonObject jsData;
    jsData.insert("userId", user.user_id);
    jsData.insert("userGroup", user.user_group);
    jsData.insert("userName", user.user_name);
    jsData.insert("deviceType", pConfig->DEVICE_TYPE);
    jsData.insert("deviceUuid", pConfig->_device_uuid);

    QJsonObject jsRoot;
    jsRoot.insert("version", MB_MESSAGE_VERSION);

    jsRoot.insert("authorization", "");
    jsRoot.insert("from", user.user_id);
    jsRoot.insert("to", "");
    jsRoot.insert("command", jsCommand);
    jsRoot.insert("data", jsData);

    QJsonDocument jsDoc(jsRoot);
	m_pMessage->connectTo(jsDoc.toJson(), pConfig->TEST_SERVER);
}

void InfoMode::GetDeviceList()
{
    SendAction(IMAC_GET_DEVICE_LIST,QJsonObject());
}

void InfoMode::GetDeviceListReply(bool bResult, const QJsonObject &jsData)
{
    QJsonDocument jsDoc(jsData);
    QString qstr = jsDoc.toJson();
    if (bResult)
    {
    }
    else
    {

    }
}

void InfoMode::HandleHeartBeat(bool bResult, const QJsonObject &jsData)
{
    Q_UNUSED(bResult);

    QString &qstrUserId = Config::GetInstance()->GetUser().user_id;

    QJsonObject jsActionData;
    if (jsData["userId"].toString() == qstrUserId)
    {
        jsActionData.insert("userId",qstrUserId);
        SendAction(IMAC_HEART_BEAT, jsActionData);
    }
}

void InfoMode::SendAction(const char *pAction, const QJsonObject &jsData)
{
    m_pMessage->sendMessage("info", pAction, jsData);
}
