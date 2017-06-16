#ifndef BINARY_MODE_H
#define BINARY_MODE_H

#include <QString>
#include <QByteArray>

class Config;
class ConfServiceImpl;
class DataBase_Impl;
class DownloadDatabase;
class ConferenceDatabase;
class ClipFileDatabase;

class RecordFileDownloadService
{

public:
    static RecordFileDownloadService * GetInstance();
    RecordFileDownloadService();

//     void DownloadRecord(const QString &qstrFileUuid,
//         const QString &qstrConferenceUuid,
//         const QString &qstrUserId,
//         const QString &qstrFileUuid)
//     {
//         InsertData()
//         SendMessage();
//     }
  void SetDataBase(DataBase_Impl *pDatabase);
  void SetConfServiceImpl(ConfServiceImpl *pConfServiceImpl);

  void DownloadRecordReply();
protected:
  void on_binary_received(QByteArray binary);

 private:
  DownloadDatabase *m_pDownloadDB;
  ConferenceDatabase *m_pConferenceDB;
  ClipFileDatabase *m_pClipFileDB;
  Config *m_pConfig;
  ConfServiceImpl *m_pConfService;

  static RecordFileDownloadService *m_pSingleton;
};

#endif  // BINARY_MODE_H
