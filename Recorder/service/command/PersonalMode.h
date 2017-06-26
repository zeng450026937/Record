#pragma once

#include <service/messager/CommandBase.h>
#include <QVariantList>

class Config;
class ConfServiceImpl;
class MessageBase;
class RecorderShared;
class PersonalMode : public CommandBase {
  friend class ServiceThread;

  Q_OBJECT

  PersonalMode(MessageBase *pMessage);

 public:
  ~PersonalMode();

  void GetPersonalList();
  void GetAllPersoanlList();

private:
    void GetPersonalListReply(bool bResult, const QJsonObject &jsData);

    void NotifyConferenceRecordAddTrigger(bool bResult, const QJsonObject &jsData);
    void NotifyConferenceRecordUpdateTrigger(bool bResult, const QJsonObject &jsData);
    void NotifyPersonRecordAddTrigger(bool bResult, const QJsonObject &jsData);
    void NotifyMobileRecordAddTrigger(bool bResult, const QJsonObject &jsData);
    void NotifyPersonRecordDeleteTrigger(bool bResult, const QJsonObject &jsData);
 private:
  RecorderShared *m_pRecordShared;
  Config *m_pConfig;
  ConfServiceImpl *m_pConfService;
};
