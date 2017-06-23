#ifndef SCENE_RECORD_H
#define SCENE_RECORD_H

#include <QAction>
#include <QGroupBox>
#include <QMenu>
#include <QModelIndex>
#include <QTime>

class QSortFilterProxyModel;
class RecorderShared;
class Scene_Record_ModelList;
class Scene_Record_Finish;
class Scene_Record_Warning;
class NotifyDialog;
class RecordControl;
class MarkControl;

namespace Ui {
class Scene_Record;
}

class Scene_Record : public QGroupBox {
  Q_OBJECT
 public:
  explicit Scene_Record(RecorderShared *sharedData, QWidget *parent = 0);
  ~Scene_Record();

  bool event(QEvent *e);
  void timerEvent(QTimerEvent *event) override;

  int aboutToStop();

 private:
  void refresh_more_menu();
  void init_model();
  void on_add_mark(QString text);

 private slots:
  // from _shareddata
  void receive_conference_notify(int state, bool result, QVariantMap conf);
  void receive_record_notify(QString deviceUuid, QString deviceName,
                             QString status);

  void reject_notify();
  void accept_notify();

  // from modelist
  void get_modellist_selected(QVariant info);
  // from template model
  void get_template_update(QModelIndex parent, int first, int last);

  void updata_time_slider();

 private slots:
  void conference_info_changed(QString text);
  void more_menu_triggered(QAction *action);
  void on_infoButton_pressed();

  void receive_device_select_stateChanged(bool checked, QVariantMap device);
  void receive_download_notify(QString uuid, int percentage);

  void on_rec_start_btn_clicked();
  void on_stop_rec_btn_clicked();
  void on_pause_btn_clicked(bool checked);

 private:
  Ui::Scene_Record *ui;

  RecorderShared *_sharedData;

  Scene_Record_ModelList *_scene_record_modellist;
  Scene_Record_Finish *_scene_record_finish;
  NotifyDialog *_notify_dialog;

  QSortFilterProxyModel *sortFilter_proxyModel;

  QMenu *more_menu;
  QAction *more_action[4];

  QVariantMap _conf_device;
  QVariantMap _conf_info;
  QString _conf_uuid;
  bool _is_resume;

  int infoButton_timer;
  int conference_timer;
  int reminder_timer;
  enum { REMINDER_INTERVAL = 10000 };
  enum { INFO_INTERVAL = 1000 };

  QTime _current_time;
  QTime _total_time;

  QTime _last_time;

  MarkControl   *m_pMarkControl;
  RecordControl *m_pRecordControl;
};

#endif  // SCENE_RECORD_H
