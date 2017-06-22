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
 signals:
  void getConferenceFiles(const QVariantList &list);

 public:
  ~ConferenceMode();
  void GetConferenceList();
  void GetConferenceFiles(const QString &uuid);

 private:
  void GetConferenceListReply(bool bResult, QJsonObject jsData);
  void GetConferenceFilesReply(bool bResult, QJsonObject jsData);

  RecorderShared *m_pRecrodShared;

  Config *m_pConfig;
};
