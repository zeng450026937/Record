
#include "info_mode.h"
#include <QJsonObject>
#include "service/service_thread.h"
#include "service/user_service_impl.h"
#include "common/config.h"
#include "service/messager/message_base.h"

#define IMAC_HEART_BEAT "heartBeat"

InfoMode::InfoMode(MessageBase *pMessager) : CommandModeBase(pMessager,"info")
{
    _AddActionProc(IMAC_HEART_BEAT, (HandleActionProc)&InfoMode::HandleHeartBeat);
}

InfoMode::~InfoMode()
{}

void InfoMode::ConnectToServer(const QString &qstrHeader, const QString &qstrServerAddress)
{
	m_pMessage->connectTo(qstrHeader, qstrServerAddress);
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
