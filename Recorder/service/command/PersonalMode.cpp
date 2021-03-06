
#include "PersonalMode.h"
#include <recorder_shared.h>
#include <QJsonArray>
#include "RecordDownloadReceiver.h"
#include "RecordDownloadService.h"
#include "common/config.h"
#include "service/conf_service_impl.h"
#include "service/messager/message_base.h"
#include "service/storage/database_impl.h"
#include "service/storage/personal_database_impl.h"

#define PM_GET_PERSONAL_LIST "getPersonalList"
#define PM_GET_ALL_PERSONAL_LIST "getAllPersonalList"

#define PM_NOTYFY_NEW_DATA_RECV "notifyNewDataRecv"
#define PM_NOTYFY_RECORD_ADD "notifyPersonRecordAdd"
#define PM_NOTYFY_RECORD_UPDATE "notifyPersonRecordUpdate"
#define PM_NOTYFY_RECORD_DELETE "notifyPersonRecordDelete"
#define PM_GET_CONFERENCE_INFO "getConferenceInfo"

PersonalMode::PersonalMode(MessageBase *pMessage)
    : CommandBase(pMessage),
      m_pConfig(Config::GetInstance()),
      m_pConfService(NULL),
      m_pRecordShared(nullptr) {
  AddActionProc(MB_PERSONAL_MODE, PM_GET_PERSONAL_LIST,
                &PersonalMode::GetPersonalListReply);
  AddActionProc(MB_PERSONAL_MODE, PM_GET_ALL_PERSONAL_LIST,
                &PersonalMode::GetPersonalListReply);

  AddActionProc(MB_PERSONAL_MODE, PM_NOTYFY_RECORD_ADD,
                &PersonalMode::NotifyPersonRecordAddTrigger);
  AddActionProc(MB_PERSONAL_MODE, PM_NOTYFY_RECORD_UPDATE,
                &PersonalMode::NotifyPersonRecordAddTrigger);
  AddActionProc(MB_PERSONAL_MODE, PM_NOTYFY_RECORD_DELETE,
                &PersonalMode::NotifyPersonRecordDeleteTrigger);
  AddActionProc(MB_PERSONAL_MODE, PM_GET_CONFERENCE_INFO,
                &PersonalMode::GetPersonalInfoTrigger);

  AddActionProc(MB_MOBILE_MODE, PM_NOTYFY_RECORD_ADD,
                &PersonalMode::NotifyMobileRecordAddTrigger);
  AddActionProc(MB_MOBILE_MODE, PM_NOTYFY_RECORD_UPDATE,
                &PersonalMode::NotifyMobileRecordAddTrigger);
  AddActionProc(MB_MOBILE_MODE, PM_NOTYFY_RECORD_DELETE,
                &PersonalMode::NotifyPersonRecordDeleteTrigger);
  AddActionProc(MB_MOBILE_MODE, PM_GET_CONFERENCE_INFO,
                &PersonalMode::GetMobileInfoTrigger);

  AddActionProc(MB_CONFERENCE_MODE, PM_NOTYFY_RECORD_ADD,
                &PersonalMode::NotifyConferenceRecordAddTrigger);
  AddActionProc(MB_CONFERENCE_MODE, PM_NOTYFY_RECORD_UPDATE,
                &PersonalMode::NotifyConferenceRecordUpdateTrigger);
  AddActionProc(MB_CONFERENCE_MODE, PM_NOTYFY_RECORD_DELETE,
                &PersonalMode::NotifyPersonRecordDeleteTrigger);
  AddActionProc(MB_CONFERENCE_MODE, PM_GET_CONFERENCE_INFO,
                &PersonalMode::GetConferenceInfoTrigger);
}

PersonalMode::~PersonalMode() {}

void PersonalMode::GetPersonalList() {
  QJsonObject jsData;
  jsData.insert("userId", m_pConfig->GetUser().user_id);
  m_pMessage->sendMessage("personal", PM_GET_PERSONAL_LIST, jsData);
}

void PersonalMode::GetAllPersoanlList() {
  m_pMessage->sendMessage("personal", PM_GET_ALL_PERSONAL_LIST, QJsonObject());
}

void PersonalMode::GetPersonalListReply(bool bResult,
                                        const QJsonObject &jsData) {
  if (bResult) {
    QVariantList lsRecordInfo = jsData["list"].toVariant().toList();
    foreach (const auto &varInfo, lsRecordInfo) {
      m_pRecordShared->AddPersonalRecordInfo(varInfo.toMap());
    }
  }
}

void PersonalMode::NotifyPersonRecordAddTrigger(bool bResult,
                                                const QJsonObject &jsData) {
  m_pRecordShared->receive_ConfCreated(RecorderShared::RT_PERSONAL, bResult,
                                       jsData.toVariantMap());
}

void PersonalMode::NotifyConferenceRecordAddTrigger(bool bResult,
                                                    const QJsonObject &jsData) {
  QVariantMap vmInfo = jsData.toVariantMap();
  RecordDownloadService *pDownloadService =
      RecordDownloadService::GetInstance();
  QString qstrFileUuid = vmInfo["fileUuid"].toString();
  QString qstrConferenceUuid = vmInfo["conferenceUuid"].toString();
  QString qstrDeviceUuid = vmInfo["deviceUuid"].toString();
  QString qstrUserId = vmInfo["userId"].toString();
  if (pDownloadService->IsExsitReceiver(qstrFileUuid, qstrConferenceUuid,
                                        qstrDeviceUuid)) {
    pDownloadService->ResumeDownload(RecorderShared::RT_CONFERENCE, qstrUserId,
                                     qstrFileUuid, qstrConferenceUuid,
                                     qstrDeviceUuid);
  } else {
    QVariantMap confInfo = m_pRecordShared->ConferenceInfo(qstrConferenceUuid);
    if (confInfo.isEmpty()) return;
    RecordDownloadReceiver *pReceiver = new RecordDownloadReceiver();
    pDownloadService->DownloadRecord(
        pReceiver, RecorderShared::RT_CONFERENCE, confInfo["title"].toString(),
        qstrUserId, qstrFileUuid, qstrConferenceUuid, qstrDeviceUuid,
        vmInfo["createTime"].toString(), vmInfo["fileExtension"].toString());
  }
}

void PersonalMode::NotifyMobileRecordAddTrigger(bool bResult,
                                                const QJsonObject &jsData) {
  m_pRecordShared->receive_ConfCreated(RecorderShared::RT_MOBILE, bResult,
                                       jsData.toVariantMap());

  m_pMessage->sendMessage("mobile", PM_GET_CONFERENCE_INFO, jsData);
}

void PersonalMode::NotifyPersonRecordDeleteTrigger(bool bResult,
                                                   const QJsonObject &jsData) {
  m_pRecordShared->receive_ConfDeleted(bResult, jsData.toVariantMap());
}

void PersonalMode::GetConferenceInfoTrigger(bool bResult,
                                            const QJsonObject &jsData) {
  m_pRecordShared->receive_ConfCreated(RecorderShared::RT_CONFERENCE, bResult,
                                       jsData.toVariantMap());
}

void PersonalMode::GetPersonalInfoTrigger(bool bResult,
                                          const QJsonObject &jsData) {
  m_pRecordShared->receive_ConfCreated(RecorderShared::RT_PERSONAL, bResult,
                                       jsData.toVariantMap());
}

void PersonalMode::GetMobileInfoTrigger(bool bResult,
                                        const QJsonObject &jsData) {
  m_pRecordShared->receive_ConfCreated(RecorderShared::RT_MOBILE, bResult,
                                       jsData.toVariantMap());
}

void PersonalMode::NotifyConferenceRecordUpdateTrigger(
    bool bResult, const QJsonObject &jsData) {
  m_pRecordShared->receive_ConfCreated(RecorderShared::RT_CONFERENCE, bResult,
                                       jsData.toVariantMap());
}
