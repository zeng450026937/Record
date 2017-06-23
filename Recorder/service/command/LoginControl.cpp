
#include "LoginControl.h"
#include <QJsonDocument>
#include <QJsonObject>

#include "common/config.h"
#include "recorder_shared.h"
#include "service/messager/message_base.h"

#define IMAC_HEART_BEAT "heartBeat"

LoginControl::LoginControl(MessageBase *pMessager) : CommandBase(pMessager) {
  AddActionProc(MB_INFO_MODE, IMAC_HEART_BEAT, &LoginControl::HandleHeartBeat);
}

LoginControl::~LoginControl() {}

void LoginControl::ConnectToServer() {
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

void LoginControl::HandleHeartBeat(bool bResult, const QJsonObject &jsData) {
  Q_UNUSED(bResult);

  QString &qstrUserId = Config::GetInstance()->GetUser().user_id;

  QJsonObject jsActionData;
  if (jsData["userId"].toString() == qstrUserId) {
    jsActionData.insert("userId", qstrUserId);
    m_pMessage->sendMessage("info", IMAC_HEART_BEAT, jsActionData);
  }
}
