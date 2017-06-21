#pragma once

#include <service/messager/CommandBase.h>
#include <QMap>

class Config;
class MessageBase;
class DataBase_Impl;
class ConferenceDatabase;
class DownloadDatabase;
class ClipFileDatabase;
class TemplateDatabase;
class ConfServiceImpl;
class RecorderShared;
class ConferenceMode : public CommandBase {
  friend class ServiceThread;

  Q_OBJECT

  ConferenceMode(MessageBase *pMessage);

 public:
  ~ConferenceMode();

  void SetDataBase(DataBase_Impl *pDatabase);

 public:
  void GetConferenceList();
  void GetTemplateList();

 private:
  struct DownLoadInfo {
    int type;
    QString uuid;
    QString identity;
    int sample_rate;
    int channal;

    bool DownLoadInfo::operator==(const DownLoadInfo &rhs) {
      return (rhs.uuid == uuid && rhs.identity == identity);
    }
  };

 private:
  // void DownloadFileReply(bool bResult, QJsonObject jsData);

  void GetTemplateListReply(bool bResult, QJsonObject jsData);
  void GetConferenceListReply(bool bResult, QJsonObject jsData);
  //  void SendAction(const char *pAction, const QJsonObject &jsData);

  RecorderShared *m_pRecrodShared;

  DownloadDatabase *m_pDownloadDB;

  ClipFileDatabase *m_pClipFileDB;
  TemplateDatabase *m_pTemplateDB;
  Config *m_pConfig;
  ConfServiceImpl *m_pConfService;

  QMap<QString, int> _conf_total_size_map;
};
