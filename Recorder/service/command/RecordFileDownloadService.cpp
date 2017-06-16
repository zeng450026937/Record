#include "RecordFileDownloadService.h"
#include <QDebug>
#include "common/config.h"
#include "messager/binary_reader.h"
#include "storage/database_impl.h"
#include "storage/include/clip_file_database.h"
#include "storage/include/conferencedatabase.h"

RecordFileDownloadService *RecordFileDownloadService::m_pSingleton = nullptr;

RecordFileDownloadService * RecordFileDownloadService::GetInstance()
{
    if (nullptr == RecordFileDownloadService::m_pSingleton)
    {
        RecordFileDownloadService::m_pSingleton = new RecordFileDownloadService();
    }

    return RecordFileDownloadService::m_pSingleton;
}

RecordFileDownloadService::RecordFileDownloadService()
    : m_pConfig(Config::GetInstance()),
      m_pConfService(Q_NULLPTR),
      m_pDownloadDB(Q_NULLPTR),
      m_pConferenceDB(Q_NULLPTR),
      m_pClipFileDB(Q_NULLPTR)
{

}

void RecordFileDownloadService::SetDataBase(DataBase_Impl *pDatabase)
{
  m_pDownloadDB = DownloadDatabase::GetInterface(pDatabase);
  m_pConferenceDB = ConferenceDatabase::GetInterface(pDatabase);
  m_pClipFileDB = ClipFileDatabase::GetInterface(pDatabase);
}

void RecordFileDownloadService::SetConfServiceImpl(ConfServiceImpl *pConfServiceImpl) {
  m_pConfService = pConfServiceImpl;
}

void RecordFileDownloadService::on_binary_received(QByteArray binary) 
{

}
