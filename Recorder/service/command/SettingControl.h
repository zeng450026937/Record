#pragma once

#include <QString>
#include <service/messager/CommandBase.h>

class MessageBase;
class RecorderShared;
class SettingControl : public CommandBase {
  friend class ServiceThread;
  
 public:
  SettingControl(RecorderShared *pRecorderShared,MessageBase *pMessager);
  ~SettingControl();

  void AddTemplateInfo(const QString &qstrTitle, 
      const QString &qstrContent, 
      const QString &qstrMembers);

  void SetTemplateinfo(
      const QString &qstrTemplateUuid,
      const QString &qstrTitle,
      const QString &qstrContent,
      const QString &qstrMembers);

  void DeleteTemplateInfo(const QString &qstrTemplateUuid);

  void GetTemplateInfo(const QString &qstrTemplateUuid);

  void GetTemplateList();
 protected:

  void AddTemplateInfoReply(bool bResult, const QJsonObject &jsData);
  void SetTemplateinfoReply(bool bResult, const QJsonObject &jsData);
  void DeleteTemplateInfoReply(bool bResult, const QJsonObject &jsData);
  void GetTemplateInfoReply(bool bResult, const QJsonObject &jsData);
  void GetTemplateListReply(bool bResult, const QJsonObject &jsData);
 private:
  RecorderShared *m_pRecordShared;
};
