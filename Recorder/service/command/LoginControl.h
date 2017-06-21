#pragma once

#include <QString>
#include <service/messager/CommandBase.h>

class MessageBase;
class RecorderShared;
class LoginControl : public CommandBase {
  friend class ServiceThread;

 public:
  LoginControl(MessageBase *pMessager);
  ~LoginControl();

  void ConnectToServer();

 protected:

  void HandleHeartBeat(bool bResult, const QJsonObject &jsData);

 private:
  RecorderShared *m_pRecordShared;
};
