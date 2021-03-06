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
  void getConferenceFiles(int type, const QVariantList &list);

 public:
  ~ConferenceMode();
  void GetConferenceList();
  void SetConferenceInfo(const QString &qstrConferenceUuid,
                         const QString &qstrTitle, const QString &qstrContent,
                         const QString &qstrMembers);
  void GetConferenceFiles(const QString &uuid);
  void GetMobileConferenceList();
  void GetMobileConferenceFiles(const QString &uuid);

 private:
  void GetConferenceListReply(bool bResult, const QJsonObject &jsData);
  void GetConferenceFilesReply(bool bResult, const QJsonObject &jsData);
  void SetConferenceInfoReply(bool bResult, const QJsonObject &jsData);
  void GetMobileConferenceListReply(bool bResult, const QJsonObject &jsData);
  void GetMobileConferenceFilesReply(bool bResult, const QJsonObject &jsData);

  RecorderShared *m_pRecrodShared;

  Config *m_pConfig;
};
