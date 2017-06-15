#ifndef BINARY_MODE_H
#define BINARY_MODE_H

#include <service/messager/CommandModeBase.h>

class Config;
class ConfServiceImpl;
class DataBase_Impl;
class DownloadDatabase;
class ConferenceDatabase;
class ClipFileDatabase;

class BinaryMode : public CommandModeBase {
 public:
  BinaryMode(MessageBase *pMessage);

  void SetDataBase(DataBase_Impl *pDatabase);
  void SetConfServiceImpl(ConfServiceImpl *pConfServiceImpl);

 protected:
  void on_binary_received(QByteArray binary) override;

 private:
  DownloadDatabase *m_pDownloadDB;
  ConferenceDatabase *m_pConferenceDB;
  ClipFileDatabase *m_pClipFileDB;
  Config *m_pConfig;
  ConfServiceImpl *m_pConfService;
};

#endif  // BINARY_MODE_H
