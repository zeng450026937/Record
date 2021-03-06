
#include "recorder_shared.h"
#include "common/config.h"
#include "service/conf_service_impl.h"
#include "service/service_thread.h"
#include "service/user_service_impl.h"

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QStandardItem>

static QFile* gDebugLogInstance = 0;

void RecorderShared::customMessageHandler(QtMsgType type,
                                          const QMessageLogContext& context,
                                          const QString& str) {
  Q_UNUSED(context);

  QString txt;
  switch (type) {
    //调试信息提示
    case QtDebugMsg:
      txt = QString("Debug: %1").arg(str);
      break;
    //一般的warning提示
    case QtWarningMsg:
      txt = QString("Warning: %1").arg(str);
      break;
    //严重错误提示
    case QtCriticalMsg:
      txt = QString("Critical: %1").arg(str);
      break;
    //致命错误提示
    case QtFatalMsg:
      txt = QString("Fatal: %1").arg(str);
      abort();
  }

  QTextStream ts(gDebugLogInstance);
  ts << txt << endl;
}

RecorderShared::RecorderShared(QObject* parent)
    : QObject(parent),
      _service(ServiceThread::GetInstance()),
      _config(Config::GetInstance()) {
  _conf_service = _service->GetConfService();
  // init debug output
  gDebugLogInstance = new QFile();
  gDebugLogInstance->setFileName("debugLog.txt");
  gDebugLogInstance->open(QIODevice::WriteOnly | QIODevice::Append);
  QTextStream ts(gDebugLogInstance);
  ts << endl << QDateTime::currentDateTime().toString() << endl << endl;

  this->initialize();
}

RecorderShared::~RecorderShared() {
  delete _config;

  if (gDebugLogInstance) {
    gDebugLogInstance->flush();
    gDebugLogInstance->close();
    delete gDebugLogInstance;
    gDebugLogInstance = 0;
  }
}

void RecorderShared::aboutToQuit() {}

// conference interface
// void RecorderShared::CreateConf(QVariantMap& conf) {
//   if (_service_ready) _service->GetConfService()->createConference(conf);
// }
// void RecorderShared::StartConf(QString uuid) {
//   if (_service_ready) _service->GetConfService()->startConference(uuid);
// }
// void RecorderShared::PauseConf(QString uuid) {
//   if (_service_ready) _service->GetConfService()->pauseConference(uuid);
// }
// void RecorderShared::StopConf(QString uuid) {
//   if (_service_ready) _service->GetConfService()->stopConference(uuid);
// }

// void RecorderShared::SetConfInfo(QString uuid, QVariantMap& conf) {
//   _service->GetConfService()->setConferenceInfo(uuid, conf);
// }
void RecorderShared::SetFolder(QString folder) {
  _service->GetConfService()->setDownloadFolder(folder);
}
QString RecorderShared::GetFolder(int type, QVariantMap& conf) {
  QDir dir;

  QString folder = Config::GetInstance()->_output_dir;
  QString folderName;
  folderName = conf.value("createTime").toString();
  folderName.replace(":", "");
  folderName.replace("-", "");
  folderName.replace(" ", "_");
  folderName += "_";
  if (type == RT_PERSONAL) folderName += conf.value("userName").toString();

  folderName += conf.value("title").toString();

  dir.setPath(folder + "/" + folderName);

  return dir.dirName();
}
void RecorderShared::AddClipFile(QVariantMap& file) {
  _service->GetConfService()->addClipFile(file);
}

QVariantList RecorderShared::GetFileList(QString uuid) {
  return _service->GetConfService()->fileList(uuid);
}

// void RecorderShared::DownLoadConference(int type, QString uuid)
// {
//     _service->GetConfService()->downloadConference(type, uuid);
// }

// void RecorderShared::CheckConferenceFile(QVariantMap& conf) {
//   QString uuid = conf.value("uuid").toString();
//
//   int index = _conf_uuid_list.indexOf(uuid);
//
//   if (index == -1) {
//     return;
//   } else {
//     _service->GetConfService()->checkConference(conf);
//     ModelUpdater::UpdateRow(ModelUpdater::ConferenceRecordInfoModel, index,
//                             conf);
//   }
// }
// void RecorderShared::CheckPersonalFile(QVariantMap& conf) {
//   QString uuid = conf.value("conference_uuid").toString();
//
//   int index = _conference_uuid_list.indexOf(uuid);
//
//   if (index == -1) {
//     return;
//   } else {
//     _service->GetConfService()->checkPersonal(conf);
//     ModelUpdater::UpdateRow(ModelUpdater::ConferenceRecordInfoModel, index,
//                             conf);
//   }
// }

void RecorderShared::AddPersonalRecordInfo(QVariantMap& vmRecordInfo) {
  vmRecordInfo.insert("recordType", RT_PERSONAL);
  QString qstrCreateTime = vmRecordInfo["createTime"].toString();
  vmRecordInfo.insert(
      "date",
      qstrCreateTime.left(10).remove('-'));  // 10 == strlen("yyyy-MM-dd")
  vmRecordInfo.insert(
      "time",
      qstrCreateTime.right(8).remove(':'));  // 8 == strlen("hh:mm:ss")

  ModelUpdater::AppendRow(ModelUpdater::ConferenceRecordInfoModel,
                          vmRecordInfo);
  _conference_uuid_list << vmRecordInfo.value("conferenceUuid").toString();
}

void RecorderShared::AddConferenceRecordInfo(QVariantMap& vmRecordInfo) {
  vmRecordInfo.insert("recordType", RT_CONFERENCE);
  QString qstrCreateTime = vmRecordInfo["createTime"].toString();
  vmRecordInfo.insert(
      "date",
      qstrCreateTime.left(10).remove('-'));  // 10 == strlen("yyyy-MM-dd")
  vmRecordInfo.insert(
      "time",
      qstrCreateTime.right(8).remove(':'));  // 8 == strlen("hh:mm:ss")
  ModelUpdater::AppendRow(ModelUpdater::ConferenceRecordInfoModel,
                          vmRecordInfo);
  _conference_uuid_list << vmRecordInfo.value("conferenceUuid").toString();

  // TODO: 下载接口暂未实现，后续处理。
  //_conf_service->getMarkInfo(vmRecordInfo.value("uuid").toString());
}

void RecorderShared::AddMobileRecordInfo(QVariantMap& vmRecordInfo) {
  vmRecordInfo.insert("recordType", RT_MOBILE);
  QString qstrCreateTime = vmRecordInfo["createTime"].toString();
  vmRecordInfo.insert(
      "date",
      qstrCreateTime.left(10).remove('-'));  // 10 == strlen("yyyy-MM-dd")
  vmRecordInfo.insert(
      "time",
      qstrCreateTime.right(8).remove(':'));  // 8 == strlen("hh:mm:ss")
  ModelUpdater::AppendRow(ModelUpdater::ConferenceRecordInfoModel,
                          vmRecordInfo);
  _conference_uuid_list << vmRecordInfo.value("conferenceUuid").toString();
}

// mark interface
QVariantList RecorderShared::GetMark(QString conf_uuid) {
  return _service->GetConfService()->markInfo(conf_uuid);
}
void RecorderShared::AddMark(ModelType type, QVariantMap& mark) {
  ModelUpdater::AppendRow(type, mark);
}

// device interface
void RecorderShared::RefreshDeviceList() {
  _service->GetConfService()->getDeviceInfoList();
}
void RecorderShared::SelectDevice(bool selected, QVariantMap& device) {
  if (selected)
    _selected_device << device.value("deviceUuid").toString();
  else
    _selected_device.removeAll(device.value("deviceUuid").toString());

  this->receive_deviceInfoUpdate(device);
}
QVariantMap RecorderShared::DeviceInfo(QString mac) {
  return _service->GetConfService()->deviceInfo(mac);
}

QVariantMap RecorderShared::ConferenceInfo(QString uuid) {
  QStandardItemModel* pModel =
      ModelUpdater::GetModel(ConferenceRecordInfoModel);

  int index = _conference_uuid_list.indexOf(uuid);

  if (index == -1) return QVariantMap();

  QStandardItem* pItem = pModel->item(index);

  return pItem->data(Qt::UserRole).toMap();
}

QVariantList RecorderShared::GetTemplateList() {
  QStandardItemModel* pModel = ModelUpdater::GetModel(TemplateModel);
  QVariantList lsTemplateInfo;

  QStandardItem* pItem;
  for (int iCount = pModel->rowCount(), i = 0; i < iCount; ++i) {
    pItem = pModel->item(i);
    lsTemplateInfo << pItem->data(Qt::UserRole);
  }

  return lsTemplateInfo;
}

void RecorderShared::receive_service_ready() {
  //     QObject::connect(_service->GetConfService(),
  //                    SIGNAL(conferenceCreated(bool, QVariantMap)), this,
  //                    SLOT(receive_conferenceCreated(bool, QVariantMap)),
  //                    Qt::QueuedConnection);
  //   QObject::connect(_service->GetConfService(),
  //                    SIGNAL(conferenceStarted(bool, QVariantMap)), this,
  //                    SLOT(receive_conferenceStarted(bool, QVariantMap)),
  //                    Qt::QueuedConnection);
  //   QObject::connect(_service->GetConfService(),
  //                    SIGNAL(conferencePaused(bool, QVariantMap)), this,
  //                    SLOT(receive_conferencePaused(bool, QVariantMap)),
  //                    Qt::QueuedConnection);
  //   QObject::connect(_service->GetConfService(),
  //                    SIGNAL(conferenceStoped(bool, QVariantMap)), this,
  //                    SLOT(receive_conferenceStoped(bool, QVariantMap)),
  //                    Qt::QueuedConnection);

  //
  //   QObject::connect(_service->GetConfService(),
  //                    SIGNAL(personalConfCreated(bool, QVariantMap)), this,
  //                    SLOT(receive_personalConfCreated(bool, QVariantMap)),
  //                    Qt::QueuedConnection);
  //   QObject::connect(_service->GetConfService(),
  //                    SIGNAL(personalConfSetted(bool, QVariantMap)), this,
  //                    SLOT(receive_personalConfSetted(bool, QVariantMap)),
  //                    Qt::QueuedConnection);
  //   QObject::connect(_service->GetConfService(),
  //                    SIGNAL(personalConfDeleted(bool, QVariantMap)), this,
  //                    SLOT(receive_personalConfDeleted(bool, QVariantMap)),
  //                    Qt::QueuedConnection);

  //   QObject::connect(
  //       _service->GetConfService(),
  //       SIGNAL(downloadConferenceCompleted(int, QString, int, int, int)),
  //       this,
  //       SLOT(receive_downloadConferenceCompleted(int, QString, int, int,
  //       int)),
  //       Qt::QueuedConnection);

  //   QObject::connect(_service->GetConfService(),
  //                    SIGNAL(deviceInfoListGetted(bool, QVariantList)), this,
  //                    SLOT(receive_deviceInfoListGetted(bool, QVariantList)),
  //                    Qt::QueuedConnection);
  //   QObject::connect(
  //       _service->GetConfService(), SIGNAL(deviceInfoUpdate(QVariantMap)),
  //       this,
  //       SLOT(receive_deviceInfoUpdate(QVariantMap)), Qt::QueuedConnection);
  QObject::connect(_service->GetConfService(),
                   SIGNAL(deviceEnvironmentUpdate(QVariantMap)), this,
                   SLOT(receive_deviceEnvironmentUpdate(QVariantMap)),
                   Qt::QueuedConnection);
}

void RecorderShared::receive_conferenceCreated(bool result, QVariantMap info) {
  if (result) {
    QString uuid = info.value("conferenceUuid").toString();

    info.insert("recordType", RT_CONFERENCE);
    QString qstrCreateTime = info["createTime"].toString();
    info.insert("date", qstrCreateTime.left(10).remove(
                            '-'));  // 10 == strlen("yyyy-MM-dd")
    info.insert("time", qstrCreateTime.right(8).remove(
                            ':'));  // 8 == strlen("hh:mm:ss")

    int index = _conference_uuid_list.indexOf(uuid);

    if (index == -1) {
      _conference_uuid_list << uuid;
      ModelUpdater::AppendRow(ModelUpdater::ConferenceRecordInfoModel, info);
    } else {
      ModelUpdater::UpdateRow(ModelUpdater::ConferenceRecordInfoModel, index,
                              info);
    }
  } else {
    qDebug() << "create conference failed";
  }
  emit conference_notify(kConfCreated, result, info);
}
void RecorderShared::receive_conferenceStarted(bool result, QVariantMap info) {
  if (!result) qDebug() << "start conference failed";
  emit conference_notify(kConfStarted, result, info);
}

void RecorderShared::receive_conferencePaused(bool result, QVariantMap info) {
  if (!result) qDebug() << "pause conference failed";
  emit conference_notify(kConfPaused, result, info);
}
void RecorderShared::receive_conferenceStoped(bool result, QVariantMap info) {
  if (!result) qDebug() << "stop conference failed";
  emit conference_notify(kConfStoped, result, info);
}

void RecorderShared::receive_ConfCreated(int type, bool result,
                                         QVariantMap& info) {
  if (result) {
    QString uuid = info.value("conferenceUuid").toString();
    int index =
        _conference_uuid_list.indexOf(info.value("conferenceUuid").toString());

    info.insert("recordType", type);
    QString qstrCreateTime = info["createTime"].toString();
    info.insert("date", qstrCreateTime.left(10).remove(
                            '-'));  // 10 == strlen("yyyy-MM-dd")
    info.insert("time", qstrCreateTime.right(8).remove(
                            ':'));  // 8 == strlen("hh:mm:ss")

    if (index == -1) {
      _conference_uuid_list << uuid;
      ModelUpdater::AppendRow(ModelUpdater::ConferenceRecordInfoModel, info);
    } else {
      ModelUpdater::UpdateRow(ModelUpdater::ConferenceRecordInfoModel, index,
                              info);
    }
  } else {
    qDebug() << "create personal conference failed";
  }
}

void RecorderShared::receive_ConfDeleted(bool result, QVariantMap& info) {
  QString uuid = info.value("conferenceUuid").toString();

  if (result) {
    int index = _conference_uuid_list.indexOf(uuid);

    if (index == -1) {
    } else {
      _conference_uuid_list.removeAll(uuid);
      ModelUpdater::RemoveRow(ModelUpdater::ConferenceRecordInfoModel, index);
    }
  } else {
    qDebug() << "delete personal conference failed";
  }
}

// void RecorderShared::receive_conferenceInfoSetted(bool result,
//                                                  QVariantMap info) {
//  QString uuid = info.value("conferenceUuid").toString();
//  if (result) {
//    int index = _conf_uuid_list.indexOf(uuid);

//    if (index == -1) {
//      _conf_uuid_list << uuid;
//      ModelUpdater::AppendRow(ModelUpdater::ConferenceRecordInfoModel, info);
//    } else {
//      ModelUpdater::UpdateRow(ModelUpdater::ConferenceRecordInfoModel, index,
//                              info);
//    }
//  } else {
//    qDebug() << "set conference info failed";
//  }
//  emit conference_notify(kConfCreated, result, info);
//}

// void RecorderShared::on_conference_list_got_trigger(bool result) {
//   QVariantList list = _service->GetConfService()->conferenceList();
//
//   if (result) {
//     _conf_uuid_list.clear();
//     foreach (const QVariant& conf, list) {
//       _conf_uuid_list << conf.toMap().value("uuid").toString();
//       _service->GetConfService()->getMarkInfo(
//           conf.toMap().value("uuid").toString());
//     }
//     ModelUpdater::ListToModel(ModelUpdater::ConferenceRecordInfoModel, list);
//   } else {
//     qDebug() << "get conference list failed";
//   }
// }

// void RecorderShared::receive_downloadConferenceCompleted(int type, QString
// uuid,
//                                                          int percentage,
//                                                          int speed,
//                                                          int completed) {
//   int index(-1);
//   QVariantMap info;
//
//   if (type == 1) {
//     info = _service->GetConfService()->conferenceInfo(uuid);
//     info.insert("speed", speed);
//     info.insert("percentage", percentage);
//
//     index = _conf_uuid_list.indexOf(uuid);
//
//     if (index != -1) {
//       ModelUpdater::UpdateRow(ModelUpdater::ConferenceRecordInfoModel,
//       index,
//                               info);
//     }
//   } else if (type == 0) {
//     info = _service->GetConfService()->personalConfInfo(uuid);
//     info.insert("speed", speed);
//     info.insert("percentage", percentage);
//
//     index = _conference_uuid_list.indexOf(uuid);
//
//     if (index != -1) {
//       ModelUpdater::UpdateRow(ModelUpdater::ConferenceRecordInfoModel,
//       index,
//                               info);
//     }
//   }

//   emit download_notify(type, uuid, percentage, completed);
// }

// #include <service/command/RecordDownloadService.h>
// #include <service/command/RecordDownloadReceiver.h>
// void RecorderShared::receive_donwloadConfNotify(bool bResult, const QString
// &qstrConferenceUuid)
// {
//     if (bResult)
//     {
//
//         int index = _conference_uuid_list.indexOf(qstrConferenceUuid);
//         if (index != -1)
//         {
//             download_data_notify(index);
// //             QStandardItemModel *pModel =
// ModelUpdater::GetModel(ConferenceRecordInfoModel);
// //
// //             QModelIndex modelIndex = pModel->index(index,0);
// //             QStandardItem *pItem = pModel->item(index);
// //             QVariantMap _info = pItem->data(Qt::UserRole).toMap();
// //
// //             RecordDownloadReceiver *pDownloadReceiver = new
// RecordDownloadReceiver();
// //             RecordDownloadService *pDownloadService =
// RecordDownloadService::GetInstance();
// //             pDownloadService->DownloadRecord(pDownloadReceiver,
// //                 _info["recordType"].toInt(),
// //                 _info["title"].toString(),
// //                 _info["userName"].toString(),
// //                 _info["fileUuid"].toString(),
// //                 _info["conferenceUuid"].toString(),
// //                 _info["deviceUuid"].toString(),
// //                 _info["createTime"].toString(),
// //                 _info["fileExtension"].toString());
//
//             //ui->file_listView->indexWidget(modelIndex);
//
//         }
//     }
// }

// #include <model/model_editor.h>
// void RecorderShared::ReciveRecordInfoes(QVariantList& lsRecordInfoes) {
//   QVariantMap mapRecordinfo;
//   foreach (const auto& varInfo, lsRecordInfoes) {
//     mapRecordinfo = varInfo.toMap();
//     _conf_service->checkConference(mapRecordinfo);
//     _conference->append_model_row(_conference_model, mapRecordinfo);
//   }
//
//   _conference_uuid_list.clear();
//   //          foreach (const QVariant &conf, list)
//   //          {
//   //              _personal_uuid_list <<
//   //              conf.toMap().value("conference_uuid").toString();
//   //          }
//
//   ModelUpdater::AppendList(ModelUpdater::ConferenceRecordInfoModel,
//                            lsRecordInfoes);
// }

// void RecorderShared::on_all_personal_list_got_trigger(bool result) {
//   QVariantList list = _service->GetConfService()->personalConfList();
//
//   if (result) {
//     _conference_uuid_list.clear();
//     foreach (QVariant conf, list) {
//       _conference_uuid_list <<
//       conf.toMap().value("conference_uuid").toString();
//     }
//     ModelUpdater::ListToModel(ModelUpdater::ConferenceRecordInfoModel, list);
//   } else {
//     qDebug() << "get all personal list failed";
//   }
// }s

void RecorderShared::receive_deviceInfoUpdate(QVariantMap info) {
  int index = _device_mac_list.indexOf(info.value("deviceUuid").toString());
  QString uuid = info.value("deviceUuid").toString();

  if (index == -1) {
    _device_mac_list << uuid;
    ModelUpdater::AppendRow(ModelUpdater::DeviceModel, info);
  } else {
    if (info.value("status").toString() != "offline") {
      if (_selected_device.contains(info.value("deviceUuid").toString()))
        info.insert("selected", true);
      else
        info.insert("selected", false);

      ModelUpdater::UpdateRow(ModelUpdater::DeviceModel, index, info);
    } else {
      _device_mac_list.removeAll(uuid);
      ModelUpdater::RemoveRow(ModelUpdater::DeviceModel, index);
    }
  }
}
void RecorderShared::receive_deviceInfoListGetted(bool result,
                                                  QVariantList list) {
  if (result) {
    _device_mac_list.clear();
    foreach (QVariant device, list) {
      _device_mac_list << device.toMap().value("deviceUuid").toString();
    }
    ModelUpdater::ListToModel(ModelUpdater::DeviceModel, list);
  } else {
    qDebug() << "get all device list failed";
  }
}

void RecorderShared::receive_deviceEnvironmentUpdate(QVariantMap info) {
  emit record_notify(info.value("deviceUuid").toString(),
                     info.value("deviceName").toString(),
                     info.value("status").toString());
}

void RecorderShared::receive_addMarkInfo(bool result, QVariantMap info) {
  if (result) {
  } else {
    qDebug() << "add mark info failed";
  }
}

void RecorderShared::receive_templateInfoAdded(bool result, QVariantMap info) {
  if (result) {
    _template_uuid_list << info.value("templateUuid").toString();
    ModelUpdater::AppendRow(ModelUpdater::TemplateModel, info);
  } else {
    qDebug() << "add template info failed";
  }
}
void RecorderShared::receive_templateInfoSetted(bool result, QVariantMap info) {
  if (result) {
    int index =
        _template_uuid_list.indexOf(info.value("templateUuid").toString());
    if (index != -1) {
      ModelUpdater::UpdateRow(ModelUpdater::TemplateModel, index, info);
    }
  } else {
    qDebug() << "set template info failed";
  }
}
void RecorderShared::receive_templateInfoDeleted(bool result,
                                                 QVariantMap info) {
  if (result) {
    int index =
        _template_uuid_list.indexOf(info.value("templateUuid").toString());
    if (index != -1) {
      _template_uuid_list.removeAt(index);
      ModelUpdater::RemoveRow(ModelUpdater::TemplateModel, index);
    }
  } else {
    qDebug() << "delete template info failed";
  }
}
void RecorderShared::receive_templateInfoGot(bool result, QVariantMap info) {
  Q_UNUSED(result);
  Q_UNUSED(info);
}
void RecorderShared::receiver_templateInfoList(bool result, QVariantList list) {
  if (result) {
    _template_uuid_list.clear();
    foreach (QVariant Template, list) {
      _template_uuid_list << Template.toMap().value("templateUuid").toString();
    }
    ModelUpdater::ListToModel(ModelUpdater::TemplateModel, list);
  } else {
    qDebug() << "get template list failed";
  }
}

void RecorderShared::initialize() {
  // init config
  if (_config->_ffmpeg_path.isEmpty()) _config->_ffmpeg_path = "./ffmpeg.exe";

  if (_config->_clip_combo.isEmpty())
    _config->_clip_combo << "0s"
                         << "2s"
                         << "5s"
                         << "10s";

  if (_config->_type_combo.isEmpty())
    _config->_type_combo << "全部录音"
                         << "会议录音列表"
                         << "个人录音列表"
                         << "移动会议";

  if (_config->_file_extention.isEmpty()) _config->_file_extention = ".wav";

  if (_config->_output_dir.isEmpty() ||
      !QDir(_config->_output_dir).isReadable()) {
    QDir dir("./OUT");
    _config->_output_dir = dir.absolutePath();
  }

  if (_config->_debug_output) {
    qInstallMessageHandler(&RecorderShared::customMessageHandler);
  }

  QObject::connect(_service, SIGNAL(service_ready()), this,
                   SLOT(receive_service_ready()), Qt::QueuedConnection);

  if (_service->isRunning()) receive_service_ready();
}
