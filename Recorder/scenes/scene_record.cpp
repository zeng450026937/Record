#include "scene_record.h"
#include "qqslider.h"
#include "ui_scene_record.h"

#include "device_list_delegate.h"
#include "recorder_shared.h"
#include "scene_record_finish.h"
#include "scene_record_modellist.h"
#include "scene_record_warning.h"
#include "scenes/notify_dialog.h"
#include "service/command/ConferenceMode.h"
#include "service/command/MarkControl.h"
#include "service/command/RecordControl.h"
#include "service/command/RecordDownloadService.h"
#include "service/service_thread.h"

#include <QMessageBox>
#include <QSortFilterProxyModel>

Scene_Record::Scene_Record(RecorderShared *sharedData, QWidget *parent)
    : QGroupBox(parent),
      _sharedData(sharedData),
      ui(new Ui::Scene_Record),
      _scene_record_modellist(new Scene_Record_ModelList(sharedData)),
      _scene_record_finish(new Scene_Record_Finish),
      _notify_dialog(new NotifyDialog),
      sortFilter_proxyModel(new QSortFilterProxyModel(this)),
      infoButton_timer(0),
      conference_timer(0),
      reminder_timer(0),
      _is_resume(false),
      m_pMarkControl(ServiceThread::GetInstance()->GetMarkControl()) {
  ui->setupUi(this);

  this->setWindowFlags(Qt::FramelessWindowHint);  //去掉标题栏

  // enable show subvalue
  ui->time_Slider->showSubValue(true);
  ui->mark_box->combine_slider(ui->time_Slider);
  ui->time_Slider->setRange(0, 3600);

  more_menu = new QMenu(ui->more_btn);
  for (int i = 0; i < 4; i++) {
    more_action[i] = new QAction(more_menu);
    more_action[i]->setVisible(false);
    more_menu->setStyleSheet(
        "QMenu{ font: 9pt;border:0;margin:0;padding:0;}"
        "QMenu::item:selected{ color:rgb(85, 170, 255);}");
    more_menu->addAction(more_action[i]);

    if (i == 3) {
      more_action[i]->setVisible(true);
      more_action[i]->setText("更多");
    }
  }

  ui->more_btn->setMenu(more_menu);

  ui->ControlStackedWidget->setCurrentWidget(ui->Rec_Start_Page);

  this->init_model();

  connect(_notify_dialog, SIGNAL(notifyAccepted()), this,
          SLOT(accept_notify()));
  connect(_notify_dialog, SIGNAL(notifyRejected()), this,
          SLOT(reject_notify()));

  connect(more_menu, SIGNAL(triggered(QAction *)), this,
          SLOT(more_menu_triggered(QAction *)));
  connect(_scene_record_modellist, SIGNAL(modelList_selected(QVariant)), this,
          SLOT(get_modellist_selected(QVariant)));

  connect(ui->title, SIGNAL(textChanged(QString)), this,
          SLOT(conference_info_changed(QString)));
  connect(ui->context, SIGNAL(textChanged(QString)), this,
          SLOT(conference_info_changed(QString)));
  connect(ui->member, SIGNAL(textChanged(QString)), this,
          SLOT(conference_info_changed(QString)));
  connect(_sharedData->GetModel(ModelUpdater::TemplateModel),
          SIGNAL(rowsRemoved(QModelIndex, int, int)), this,
          SLOT(get_template_update(QModelIndex, int, int)));
  connect(_sharedData->GetModel(ModelUpdater::TemplateModel),
          SIGNAL(rowsInserted(QModelIndex, int, int)), this,
          SLOT(get_template_update(QModelIndex, int, int)));

  connect(_sharedData, SIGNAL(conference_notify(int, bool, QVariantMap)), this,
          SLOT(receive_conference_notify(int, bool, QVariantMap)));
  //   connect(_sharedData, SIGNAL(download_notify(int, QString, int, bool)),
  //   this,
  //           SLOT(receive_download_notify( QString, int)));
  connect(_sharedData, SIGNAL(record_notify(QString, QString, QString)), this,
          SLOT(receive_record_notify(QString, QString, QString)));

  connect(RecordDownloadService::GetInstance(),
          SIGNAL(conference_receive_data_notify(QString, int)), this,
          SLOT(receive_download_notify(QString, int)));

  ServiceThread *pService = ServiceThread::GetInstance();

  m_pRecordControl = pService->GetRecordControl();
  m_pRecordControl->GetDeviceList();
  m_pMarkControl->GetAllConferenceMarkList();
}

Scene_Record::~Scene_Record() {
  if (_scene_record_modellist) {
    delete _scene_record_modellist;
  }
  if (_scene_record_finish) {
    delete _scene_record_finish;
  }
  if (_notify_dialog) delete _notify_dialog;

  delete ui;
}

bool Scene_Record::event(QEvent *e) {
  int i = e->type();
  switch (e->type()) {
    case QEvent::KeyPress:
      if (const QKeyEvent *key_event = static_cast<const QKeyEvent *>(e)) {
        switch (key_event->key()) {
          case Qt::Key_Return:
          case Qt::Key_Enter:
            if (this->focusWidget() == ui->mark_lineEdit) {
              this->on_add_mark(ui->mark_lineEdit->text().trimmed());
            }
        }
      }
      break;

    default:
      break;
  }

  return QGroupBox::event(e);
}

void Scene_Record::timerEvent(QTimerEvent *event) {
  int timerId = event->timerId();

  if (timerId == infoButton_timer) {
    ui->infoButton->hide();
    this->killTimer(infoButton_timer);
    infoButton_timer = 0;
  } else if (timerId == conference_timer) {
    this->updata_time_slider();
  } else if (timerId == reminder_timer) {
    this->killTimer(reminder_timer);
    reminder_timer = 0;
  }
}

int Scene_Record::aboutToStop() {
  int result(-1);
  if (!_conf_uuid.isEmpty()) {
    if (_scene_record_finish == nullptr) {
      _scene_record_finish = new Scene_Record_Finish();
    }

    QPoint pos = this->parentWidget()
                     ->parentWidget()
                     ->parentWidget()
                     ->geometry()
                     .topLeft() +
                 ui->ControlStackedWidget->geometry().center();
    _scene_record_finish->move(pos.x() - _scene_record_finish->width() / 2,
                               pos.y() - _scene_record_finish->height() / 2);

    _scene_record_finish->set_file_name(ui->rec_file_name_lable->text());

    ui->ControlStackedWidget->setEnabled(false);

    if (_scene_record_finish->exec() == QDialog::Accepted) {
      m_pRecordControl->StopConference(_conf_uuid);
    } else {
      result = -1;
    }

    ui->ControlStackedWidget->setEnabled(true);
  } else {
    result = 0;
  }

  return result;
}

void Scene_Record::refresh_more_menu() {
  QVariantList list = _sharedData->GetTemplateList();

  int count = list.count();
  if (count > 3) {
    count = 3;
  }

  for (int i = 0; i < 4; i++) {
    more_action[i]->setVisible(false);
  }

  for (int i = 0; i <= count; i++) {
    if (i == count) {
      more_action[i]->setText("更多");
    } else {
      more_action[i]->setText(list.at(i).toMap().value("title").toString());
      more_action[i]->setData(list.at(i));
    }
    more_action[i]->setVisible(true);
  }
}

void Scene_Record::init_model() {
  // display selected device
  sortFilter_proxyModel->setSourceModel(
      _sharedData->GetModel(ModelUpdater::DeviceModel));
  sortFilter_proxyModel->setFilterKeyColumn(2);
  sortFilter_proxyModel->setFilterRole(Qt::CheckStateRole);

  ui->deviceListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->deviceListView->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->deviceListView->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->deviceListView->setWrapping(false);
  ui->deviceListView->setModel(sortFilter_proxyModel);

  DeviceListDelegate *device_delegate = new DeviceListDelegate(this);
  ui->deviceListView->setItemDelegateForColumn(0, device_delegate);
  QObject::connect(device_delegate,
                   SIGNAL(device_select_stateChanged(bool, QVariantMap)), this,
                   SLOT(receive_device_select_stateChanged(bool, QVariantMap)));

  QRegExp::PatternSyntax Psyntax = QRegExp::PatternSyntax(QRegExp::FixedString);
  QRegExp syntax =
      QRegExp(QString("%1").arg(Qt::Checked), Qt::CaseInsensitive, Psyntax);
  sortFilter_proxyModel->setFilterRegExp(syntax);  //设置过滤语法

  ui->MarkTable->verticalHeader()->hide();
  ui->MarkTable->setWordWrap(true);
  ui->MarkTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  // no edit
  ui->MarkTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->MarkTable->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->MarkTable->verticalHeader()->setSectionResizeMode(
      QHeaderView::ResizeToContents);
  ui->MarkTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  ui->MarkTable->horizontalHeader()->hide();

  ui->MarkTable->setModel(_sharedData->GetModel(ModelUpdater::RecordMarkModel));
  ui->MarkTable->hideColumn(3);
}

void Scene_Record::on_add_mark(QString text) {
  if (text.isEmpty()) return;

  if (!_conf_uuid.isEmpty()) {
    QVariantMap mark_info = ui->mark_box->add_mark(text);
    mark_info.insert("uuid", _conf_uuid);

    m_pMarkControl->AddMarkInfo(_conf_uuid, text, mark_info["time"].toInt());
    _sharedData->AddMark(RecorderShared::RecordMarkModel, mark_info);

    ui->MarkTable->resizeColumnToContents(0);
    ui->MarkTable->horizontalHeader()->setSectionResizeMode(
        1, QHeaderView::Stretch);
    ui->MarkTable->resizeColumnToContents(2);

    ui->mark_lineEdit->clear();
  }
}

void Scene_Record::get_template_update(QModelIndex parent, int first,
                                       int last) {
  Q_UNUSED(parent);
  Q_UNUSED(first);
  Q_UNUSED(last);
  this->refresh_more_menu();
}

void Scene_Record::on_rec_start_btn_clicked() {
  if (_conf_uuid.isEmpty()) {
    // repair for recording
    QPoint pos = this->parentWidget()
                     ->parentWidget()
                     ->parentWidget()
                     ->geometry()
                     .topLeft() +
                 ui->ControlStackedWidget->geometry().center();

    if (_conf_device.count() <= 0) {
      Scene_Record_Warning::ShowMessage(pos, QString("请选择使用设备"));
      return;
    }

    _conf_info.insert("devices", _conf_device);
    if (_conf_info.value("title").isNull())
      _conf_info.insert("title", "Default");

    m_pRecordControl->CreateConference(
        _conf_info["title"].toString(), _conf_info["content"].toString(),
        _conf_info["members"].toString(), _conf_device.values());

    ui->deviceListView->setEnabled(false);
  } else {
    m_pRecordControl->StartConference(_conf_uuid);
  }
}

void Scene_Record::get_modellist_selected(QVariant info) {
  QVariantMap info_map = info.toMap();

  ui->title->setText(info_map.value("title").toString());
  ui->context->setText(info_map.value("content").toString());
  ui->member->setText(info_map.value("members").toString());
}

void Scene_Record::more_menu_triggered(QAction *action) {
  if (action->text() == "更多") {
    if (_scene_record_modellist == nullptr) {
      _scene_record_modellist = new Scene_Record_ModelList(_sharedData);
    }

    QPoint pos = this->parentWidget()
                     ->parentWidget()
                     ->parentWidget()
                     ->geometry()
                     .topLeft() +
                 ui->InfoArea->geometry().center();
    _scene_record_modellist->move(
        pos.x() - _scene_record_modellist->width() / 2,
        pos.y() - _scene_record_modellist->height() / 2);

    // ui->InfoArea->setEnabled(false);

    _scene_record_modellist->show();
  } else {
    this->get_modellist_selected(action->data());
  }
}

void Scene_Record::on_stop_rec_btn_clicked() { this->aboutToStop(); }

void Scene_Record::conference_info_changed(QString text) {
  Q_UNUSED(text);
  ui->infoButton->setText("确认");
  ui->infoButton->show();
}

void Scene_Record::receive_device_select_stateChanged(bool checked,
                                                      QVariantMap device) {
  QString uuid = device.value("deviceUuid").toString();
  if (checked)
    _conf_device.insert(uuid, device);
  else
    _conf_device.remove(uuid);

  qDebug() << _conf_device;
}

void Scene_Record::receive_download_notify(QString uuid, int percentage) {
  if (uuid == _conf_uuid) {
    ui->time_Slider->setSubValue(ui->time_Slider->value() * percentage / 100);
  }
}

void Scene_Record::receive_conference_notify(int state, bool result,
                                             QVariantMap conf) {
  switch (state) {
    case RecorderShared::kConfCreated:
      if (result) {
        _conf_info = conf;
        _conf_uuid = conf.value("conferenceUuid").toString();

        ui->rec_file_name_lable->setText(
            _sharedData->GetFolder(RecorderShared::RT_CONFERENCE, _conf_info));

        m_pRecordControl->StartConference(_conf_uuid);
      } else {
        qDebug() << "kConfCreated failed";
        ui->deviceListView->setEnabled(true);
      }
      break;
    case RecorderShared::kConfStarted:
      if (result) {
        if (!_is_resume) {
          _total_time.setHMS(1, 0, 0, 0);
          _current_time.setHMS(0, 0, 0, 0);

          ui->total_time_lable->setText(_total_time.toString("hh:mm:ss"));
          ui->current_time_lable->setText(_current_time.toString("hh:mm:ss"));

          ui->time_Slider->setRange(0, _current_time.secsTo(_total_time));
          ui->time_Slider->setValue(0);

          ui->mark_box->clear_marks();
          ui->ControlStackedWidget->setCurrentWidget(ui->Rec_Started_Page);
          this->on_add_mark("录音起点");

          _is_resume = true;
        }

        conference_timer = this->startTimer(1000);
        _last_time.start();

        ui->pause_btn->setText("暂停");
        ui->pause_btn->setChecked(false);
        ui->pause_btn->setEnabled(true);
      } else {
        qDebug() << "kConfStarted failed";
        ui->pause_btn->setText("继续录制");
        ui->pause_btn->setChecked(true);
        ui->pause_btn->setEnabled(true);
      }
      break;
    case RecorderShared::kConfPaused:
      if (result) {
        if (conference_timer) {
          this->killTimer(conference_timer);
          conference_timer = 0;

          _current_time = _current_time.addMSecs(_last_time.elapsed());
          ui->current_time_lable->setText(_current_time.toString("hh:mm:ss"));
        }
        ui->pause_btn->setText("继续录制");
        ui->pause_btn->setChecked(true);
        ui->pause_btn->setEnabled(true);
      } else {
        qDebug() << "kConfPaused failed";
        ui->pause_btn->setText("暂停");
        ui->pause_btn->setChecked(false);
        ui->pause_btn->setEnabled(true);
      }
      break;
    case RecorderShared::kConfStoped:
      if (result) {
        _conf_info.clear();
        _conf_uuid.clear();

        _is_resume = false;

        if (conference_timer) {
          this->killTimer(conference_timer);
          conference_timer = 0;
          ui->time_Slider->setSubValue(0);
        }

        ui->mark_box->clear_marks();
        _sharedData->ClearModel(ModelUpdater::RecordMarkModel);

        ui->deviceListView->setEnabled(true);
        ui->ControlStackedWidget->setCurrentWidget(ui->Rec_Start_Page);
      } else {
        qDebug() << "kConfStoped failed";
      }
      break;
    case RecorderShared::kConfSetted:
      if (result) {
        ui->infoButton->setText("保存成功");

        if (!infoButton_timer) {
          infoButton_timer = this->startTimer(INFO_INTERVAL);
        } else {
          this->killTimer(infoButton_timer);
          infoButton_timer = this->startTimer(INFO_INTERVAL);
        }

        ui->rec_file_name_lable->setText(
            _sharedData->GetFolder(RecorderShared::RT_CONFERENCE, _conf_info));
      } else {
        qDebug() << "kConfSetted failed";
      }
      break;
  }
}

void Scene_Record::receive_record_notify(QString deviceUuid, QString deviceName,
                                         QString status) {
  if (!_conf_device.contains(deviceUuid)) return;

  QString message;

  if (status == "SpeechWithNoConference" && !!!reminder_timer &&
      _conf_uuid.isEmpty()) {
    message = QString("好像会议开始了，是否开始录音？");
    _notify_dialog->NotifyMessage(
        message, true,
        QObject::tr("\345\274\200\345\247\213\345\275\225\351\237\263"),
        QObject::tr("\347\250\215\345\220\216\345\275\225\351\237\263"));
  } else if (status == "SpeechVoiceTooLow" && !!!reminder_timer &&
             !_conf_uuid.isEmpty()) {
    message = QString(
                  "\"%"
                  "1\"\345\275\225\351\237\263\350\256\276\345\244\207\345\243"
                  "\260\351\237\263\350\276\203\345\260\217\357\274\214\350\257"
                  "\267\346\243\200\346\237\245\351\272\246\345\205\213\351\243"
                  "\216\346\210\226\345\260\206\350\256\276\345\244\207\351\235"
                  "\240\350\277\221\345\217\221\350\250\200\344\272\272")
                  .arg(deviceName);
    _notify_dialog->NotifyMessage(message);
  } else if (status == "SpeechWithNoise" && !!!reminder_timer &&
             !_conf_uuid.isEmpty()) {
    message =
        QString(
            "\"%"
            "1\"\345\275\225\351\237\263\350\256\276\345\244\207\344\271\213"
            "\345\210\253\345\210\260\346\235\202\351\237\263\357\274\214\350"
            "\257\267\350\260\203\346\225\264\344\270\213\350\256\276\345\244"
            "\207\350\277\234\347\246\273\345\231\252\345\243\260\346\272\220")
            .arg(deviceName);
    _notify_dialog->NotifyMessage(message);
  } else if (status == "SpeechNormal") {
    if (_notify_dialog->isVisible()) _notify_dialog->hide();
  }
}

void Scene_Record::reject_notify() {
  reminder_timer = this->startTimer(REMINDER_INTERVAL);
}
void Scene_Record::accept_notify() {
  this->on_rec_start_btn_clicked();
  this->activateWindow();
}

void Scene_Record::on_infoButton_pressed() {
  if (ui->title->text() == "" && ui->context->text() == "" &&
      ui->member->text() == "") {
    return;
  }

  _conf_info.insert("title", ui->title->text());
  _conf_info.insert("content", ui->context->text());
  _conf_info.insert("members", ui->member->text());

  if (_conf_uuid.isEmpty()) {
    ui->infoButton->setText("保存成功");

    if (!infoButton_timer) {
      infoButton_timer = this->startTimer(INFO_INTERVAL);
    } else {
      this->killTimer(infoButton_timer);
      infoButton_timer = this->startTimer(INFO_INTERVAL);
    }
  } else {
    ServiceThread::GetInstance()->GetConferenceMode()->SetConferenceInfo(
        _conf_uuid, _conf_info["title"].toString(),
        _conf_info["content"].toString(), _conf_info["members"].toString());
  }
}

void Scene_Record::on_pause_btn_clicked(bool checked) {
  if (!_conf_uuid.isEmpty()) {
    if (checked) {
      m_pRecordControl->PauseConference(_conf_uuid);
    } else {
      m_pRecordControl->StartConference(_conf_uuid);
    }

    ui->pause_btn->setEnabled(false);
  }
}

void Scene_Record::updata_time_slider() {
  QString format = "hh:mm:ss";

  if (ui->time_Slider->value() < ui->time_Slider->maximum()) {
    ui->time_Slider->setValue(ui->time_Slider->value() + 1);
    _current_time = _current_time.addMSecs(_last_time.elapsed());
  } else {
    _total_time.setHMS(_total_time.hour() + 1, 0, 0, 0);
    ui->time_Slider->setRange(0, QTime(0, 0, 0, 0).secsTo(_total_time));
    ui->time_Slider->setValue(ui->time_Slider->value() + 1);
    _current_time = _current_time.addMSecs(_last_time.elapsed());

    ui->total_time_lable->setText(_total_time.toString(format));
  }

  ui->current_time_lable->setText(_current_time.toString(format));

  _last_time.restart();
}
