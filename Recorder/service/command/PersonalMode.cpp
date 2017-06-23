
#include "PersonalMode.h"
#include <recorder_shared.h>
#include <QJsonArray>
#include "common/config.h"
#include "service/conf_service_impl.h"
#include "service/messager/message_base.h"
#include "service/storage/database_impl.h"
#include "service/storage/personal_database_impl.h"

#define PM_GET_PERSONAL_LIST "getPersonalList"
#define PM_GET_ALL_PERSONAL_LIST "getAllPersonalList"

#define PM_NOTYFY_NEW_DATA_RECV "notifyNewDataRecv"
#define PM_NOTYFY_RECORD_ADD    "notifyPersonRecordAdd"
#define PM_NOTYFY_RECORD_UPDATE "notifyPersonRecordUpdate"
#define PM_NOTYFY_RECORD_DELETE "notifyPersonRecordDelete"

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
      &PersonalMode::NotifyPersonRecordUpdateTrigger);
  AddActionProc(MB_PERSONAL_MODE, PM_NOTYFY_RECORD_DELETE,
      &PersonalMode::NotifyPersonRecordDeleteTrigger);

  AddActionProc(MB_MOBILE_MODE, PM_NOTYFY_RECORD_ADD,
      &PersonalMode::NotifyPersonRecordAddTrigger);
  AddActionProc(MB_MOBILE_MODE, PM_NOTYFY_RECORD_UPDATE,
      &PersonalMode::NotifyPersonRecordUpdateTrigger);
  AddActionProc(MB_MOBILE_MODE, PM_NOTYFY_RECORD_DELETE,
      &PersonalMode::NotifyPersonRecordDeleteTrigger);

  AddActionProc(MB_CONFERENCE_MODE, PM_NOTYFY_RECORD_ADD,
      &PersonalMode::NotifyConferenceRecordAddTrigger);
  AddActionProc(MB_CONFERENCE_MODE, PM_NOTYFY_RECORD_UPDATE,
      &PersonalMode::NotifyPersonRecordUpdateTrigger);
  AddActionProc(MB_CONFERENCE_MODE, PM_NOTYFY_RECORD_DELETE,
      &PersonalMode::NotifyPersonRecordDeleteTrigger);
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
    QJsonDocument jsDoc(jsData);
    QString qstr = jsDoc.toJson();
    QVariantList lsRecordInfo = jsData["list"].toVariant().toList();
    foreach (const auto &varInfo, lsRecordInfo) {
      m_pRecordShared->AddPersonalRecordInfo(varInfo.toMap());
    }
  }
}

void PersonalMode::NotifyPersonRecordAddTrigger(bool bResult, const QJsonObject &jsData)
{
    m_pRecordShared->receive_personalConfCreated(bResult, jsData.toVariantMap());
}

void PersonalMode::NotifyConferenceRecordAddTrigger(bool bResult, const QJsonObject &jsData)
{
    QVariantMap vmInfo = jsData.toVariantMap();
    vmInfo.insert("autoDownload", true);
    m_pRecordShared->receive_personalConfCreated(bResult, jsData.toVariantMap());
}

void PersonalMode::NotifyPersonRecordDeleteTrigger(bool bResult, const QJsonObject &jsData)
{
    m_pRecordShared->receive_personalConfDeleted(bResult, jsData.toVariantMap());
}

void PersonalMode::NotifyPersonRecordUpdateTrigger(bool bResult, const QJsonObject &jsData)
{
    m_pRecordShared->receive_personalConfSetted(bResult, jsData.toVariantMap());
}
