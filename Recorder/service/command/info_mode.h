#pragma once

#include <QString>
#include "../messager/CommandBase.h"

class MessageBase;
class RecorderShared;
class InfoMode : public CommandBase {
  friend class ServiceThread;

 public:
  InfoMode(MessageBase *pMessager);
  ~InfoMode();

  void ConnectToServer();

  void GetDeviceList();

 protected:
 private:
  void GetDeviceListReply(bool bResult, const QJsonObject &jsData);
  void GetDeviceInfoReply(bool bResult, const QJsonObject &jsData);

  void HandleHeartBeat(bool bResult, const QJsonObject &jsData);

  void SendAction(const char *pAction, const QJsonObject &jsData);

 private:
  RecorderShared *m_pRecordShared;
};
