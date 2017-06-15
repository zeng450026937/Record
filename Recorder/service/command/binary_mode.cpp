#include "binary_mode.h"
#include <QDebug>
#include "common/config.h"
#include "messager/binary_reader.h"
#include "storage/database_impl.h"
#include "storage/include/clip_file_database.h"
#include "storage/include/conferencedatabase.h"
#include "storage/include/download_database.h"

#define CM_BIANRY_RECEIVED "binaryReceived"

BinaryMode::BinaryMode(MessageBase *pMessage)
    : CommandModeBase(pMessage, "binary"),
      m_pConfig(Config::GetInstance()),
      m_pConfService(Q_NULLPTR),
      m_pDownloadDB(Q_NULLPTR),
      m_pConferenceDB(Q_NULLPTR),
      m_pClipFileDB(Q_NULLPTR) {}

void BinaryMode::SetDataBase(DataBase_Impl *pDatabase) {
  m_pDownloadDB = DownloadDatabase::GetInterface(pDatabase);
  m_pConferenceDB = ConferenceDatabase::GetInterface(pDatabase);
  m_pClipFileDB = ClipFileDatabase::GetInterface(pDatabase);
}

void BinaryMode::SetConfServiceImpl(ConfServiceImpl *pConfServiceImpl) {
  m_pConfService = pConfServiceImpl;
}

void BinaryMode::on_binary_received(QByteArray binary) {
  qDebug() << "binary received.";
}
