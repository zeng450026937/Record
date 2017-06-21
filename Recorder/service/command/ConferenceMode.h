#pragma once

#include <service/messager/CommandBase.h>
#include <QMap>

class Config;
class MessageBase;
class RecorderShared;
class ConferenceMode : public CommandBase {
  friend class ServiceThread;

  Q_OBJECT

  ConferenceMode(MessageBase *pMessage);


public:
    ~ConferenceMode();
    void GetConferenceList();

private:
    void GetConferenceListReply(bool bResult, QJsonObject jsData);

  RecorderShared *m_pRecrodShared;

    Config              *m_pConfig;
};
