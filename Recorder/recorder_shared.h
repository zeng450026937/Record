#ifndef RECORDERSHARED_H
#define RECORDERSHARED_H

#include <QObject>
#include "model/model_updater.h"

class Config;
class WhiteList;
class ServiceThread;
class ConfServiceImpl;
class RecorderShared : public QObject, public ModelUpdater {
  Q_OBJECT
 public:
  RecorderShared(QObject* parent = 0);
  ~RecorderShared();

  enum RecordType {
    // 根据服务端定义的值来设定的。 2是 performance，但目前好像跟录音文件无关。
    RT_PERSONAL = 0,
    RT_CONFERENCE = 1,
    RT_MOBILE = 3
  };

  enum {
    kConnectOpened = 0,
    kConnectFailed,
    kConnectClosed,
    kLoginOk,
    kLoginFailed,
    kLogoutOk,
    kLogoutFailed
  };

  enum {
    kConfCreated = 0,
    kConfStarted,
    kConfPaused,
    kConfStoped,
    kConfSetted
  };

  void aboutToQuit();

  Config* GetConfig() { return _config; }

  // conference interface
  //   void CreateConf(QVariantMap& conf);
  //   void StartConf(QString uuid);
  //   void PauseConf(QString uuid);
  //   void StopConf(QString uuid);
  void SetConfInfo(QString uuid, QVariantMap& conf);
  void SetFolder(QString folder);
  QString GetFolder(int type, QVariantMap& conf);
  void AddClipFile(QVariantMap& file);
  QVariantList GetFileList(QString uuid);
  // void     DownLoadConference(int type, QString uuid);
  //   void CheckConferenceFile(QVariantMap& conf);
  //   void CheckPersonalFile(QVariantMap& conf);

  void AddPersonalRecordInfo(QVariantMap& vmRecordInfo);
  void AddConferenceRecordInfo(QVariantMap& vmRecordInfo);

  // mark interface
  QVariantList GetMark(QString conf_uuid);
  void AddMark(ModelType type, QVariantMap& mark);

  // device interface
  void RefreshDeviceList();
  void SelectDevice(bool selected, QVariantMap& device);
  QVariantMap DeviceInfo(QString mac);

  // template interface
  // void AddTemplate(QVariantMap& Template);
  void RemoveTemplate(QString& uuid);
  void SetTemplate(QVariantMap& Template);
  QVariantList GetTemplateList();

 signals:
  void connection_notify(int state, QString text);
  void conference_notify(int state, bool result, QVariantMap conf);
  void record_notify(QString device_mac, QString device_name, QString status);

  void download_notify(int type, QString uuid, int percentage, bool completed);

 public slots:
  void receive_service_ready();

  void receive_conferenceCreated(bool result, QVariantMap info);
  void receive_conferenceStarted(bool result, QVariantMap info);
  void receive_conferencePaused(bool result, QVariantMap info);
  void receive_conferenceStoped(bool result, QVariantMap info);

  void receive_conferenceInfoSetted(bool result, QVariantMap info);
  // void on_conference_list_got_trigger(bool result);

  void receive_downloadConferenceCompleted(int type, QString uuid,
                                           int percentage, int speed,
                                           int completed);

  void receive_personalConfCreated(bool result, QVariantMap info);
  void receive_personalConfSetted(bool result, QVariantMap info);
  void receive_personalConfDeleted(bool result, QVariantMap info);
  //   void ReciveRecordInfoes(QVariantList& lsRecordInfoes);
  //   void on_all_personal_list_got_trigger(bool result);

  void receive_deviceInfoUpdate(QVariantMap info);
  void receive_deviceInfoListGetted(bool result, QVariantList list);

  void receive_deviceEnvironmentUpdate(QVariantMap info);

  void receive_addMarkInfo(bool result, QVariantMap info);

  void receive_templateInfoAdded(bool result, QVariantMap info);
  void receive_templateInfoSetted(bool result, QVariantMap info);
  void receive_templateInfoDeleted(bool result, QVariantMap info);
  void receive_templateInfoGot(bool result, QVariantMap info);
  void receiver_templateInfoList(bool result, QVariantList list);

 private:
  void initialize();
  void request_data();
  static void customMessageHandler(QtMsgType type,
                                   const QMessageLogContext& context,
                                   const QString& str);

 private:
  Config* _config;
  ServiceThread* _service;

  bool _server_available;

  QStringList _template_uuid_list;
  QStringList _device_mac_list;
  QStringList _selected_device;
  QStringList _conf_uuid_list;
  QStringList _conference_uuid_list;

  ConfServiceImpl* _conf_service;
};

#endif  // RECORDERSHARED_H
