#include "list_form.h"
#include <Recorder/recorder_shared.h>
#include <service/command/RecordDownloadReceiver.h>
#include <service/command/RecordDownloadService.h>
#include <QDateTime>
#include <QDebug>
#include <QMouseEvent>
#include <QScreen>
#include "scene_file_dl.h"
#include "scenes/scene_record_warning.h"
#include "ui_list_form.h"

ListForm::ListForm(bool isDetail)
    : ui(new Ui::ListForm),
      _isDetail(isDetail),
      _download_status(RecordDownloadReceiver::DS_UNEXSITS) {
  ui->setupUi(this);

  QIcon icon = QIcon(":/resource/u620.png");
  ui->downloadButton->setIcon(icon);
  ui->downloadButton->setIconSize(ui->downloadButton->size());
  ui->speedLabel->setVisible(false);
  ui->percentageLabel->setVisible(false);

  ui->dateLabel->clear();
  ui->timeLabel->clear();
  ui->titleLabel->clear();
  ui->keywordLabel->clear();
  ui->locationLabel->clear();
}

ListForm::~ListForm() { delete ui; }

void ListForm::mousePressEvent(QMouseEvent *event) {
  Q_EMIT itemClicked(_info);
  event->ignore();
}

void ListForm::update_display(const QVariantMap &info) {
  QDateTime dtCreateTime = QDateTime::fromString(info["createTime"].toString(),
                                                 "yyyy-MM-dd HH:mm:ss");

  ui->dateLabel->setText(dtCreateTime.toString("yyyy年MM月dd\346\227\245"));
  ui->timeLabel->setText(dtCreateTime.toString("HH:mm:ss"));

  QString text;
  switch (info.value("recordType").toInt()) {
    case RecorderShared::RT_PERSONAL: {
      text = info.value("title").toString();
      if (text.isEmpty()) text = tr("个人录音");

      ui->titleLabel->setText(text);
      if (!ui->userGroupBox->isVisible()) ui->userGroupBox->show();

      QString user_id = info.value("userId").toString();
      QString user_name = info.value("userName").toString();
      ui->idLabel->setText(user_name + "(" + user_id + ")");

      text = info.value("gpsAddress").toString();
      ui->locationLabel->setText(text);

      text = info.value("tag").toString();
      ui->keywordLabel->setText(text);
    } break;
    case RecorderShared::RT_CONFERENCE: {
      text = info.value("title").toString();

      if (!_isDetail) {
        ui->downloadButton->hide();
        ui->userGroupBox->hide();
        if (text.isEmpty()) text = tr("会议录音");
      } else {
        ui->downloadButton->show();
        ui->userGroupBox->show();
        QString user_id = info.value("userId").toString();
        QString user_name = info.value("userName").toString();
        ui->idLabel->setText(user_name + "(" + user_id + ")");

        text = info.value("gpsAddress").toString();
        ui->locationLabel->setText(text);

        text = info.value("tag").toString();
        ui->keywordLabel->setText(text);
      }

      ui->titleLabel->setText(text);

    } break;
    case RecorderShared::RT_MOBILE: {
      text = info.value("title").toString();
      if (!_isDetail) {
        ui->downloadButton->hide();
        ui->userGroupBox->hide();
        if (text.isEmpty()) text = tr("移动会议");
      } else {
        ui->downloadButton->show();
        ui->userGroupBox->show();
        QString user_id = info.value("userId").toString();
        QString user_name = info.value("userName").toString();
        ui->idLabel->setText(user_name + "(" + user_id + ")");

        text = info.value("gpsAddress").toString();
        ui->locationLabel->setText(text);

        text = info.value("tag").toString();
        ui->keywordLabel->setText(text);
      }

      ui->titleLabel->setText(text);

    } break;
    default:
      break;
  }

  if (_info.isEmpty()) {
    QIcon icon;
    _download_status = RecordDownloadReceiver::GetDownloadStatus(
        info["fileUuid"].toString(), info["conferenceUuid"].toString(),
        info["deviceUuid"].toString());
    switch (_download_status) {
      case RecordDownloadReceiver::DS_UNCOMPLETED:
      case RecordDownloadReceiver::DS_UNEXSITS:
        icon = QIcon(":/resource/u620.png");
        break;
      case RecordDownloadReceiver::DS_COMPELETED:
        icon = QIcon(":/resource/u579.png");
        break;
      case RecordDownloadReceiver::DS_DOWNLOADING:
        icon = QIcon(":/resource/u1610.png");
        break;
    }
    ui->downloadButton->setIcon(icon);
    ui->downloadButton->setIconSize(ui->downloadButton->size());
  }
  _info = info;
}

void ListForm::CheckAndAliveData() {
  RecordDownloadService *pService = RecordDownloadService::GetInstance();
  RecordDownloadReceiver *pReceiver = pService->GetDownloadReciver(
      _info["fileUuid"].toString(), _info["conferenceUuid"].toString(),
      _info["deviceUuid"].toString());

  if (pReceiver != nullptr) {
    connect(pReceiver, SIGNAL(downloading_tick(int, int, int)), this,
            SLOT(onDownloadingTick(int, int, int)));
    connect(pReceiver, SIGNAL(download_prompt(QString)), this,
            SLOT(onDownloadPrompt(QString)));
  }
}

void ListForm::on_downloadButton_clicked() {
  if (_download_status != RecordDownloadReceiver::DS_COMPELETED) {
    Scene_File_DL promptDialog;
    if (promptDialog.exec() == QDialog::Rejected) return;

    RecordDownloadReceiver *pDownloadReciver = new RecordDownloadReceiver();
    connect(pDownloadReciver, SIGNAL(downloading_tick(int, int, int)), this,
            SLOT(onDownloadingTick(int, int, int)));
    connect(pDownloadReciver, SIGNAL(download_prompt(QString)), this,
            SLOT(onDownloadPrompt(QString)));
    RecordDownloadService::GetInstance()->DownloadRecord(
        pDownloadReciver, _info["recordType"].toInt(),
        _info["title"].toString(), _info["userId"].toString(),
        _info["fileUuid"].toString(), _info["conferenceUuid"].toString(),
        _info["deviceUuid"].toString(), _info["createTime"].toString(),
        _info["fileExtension"].toString());
  }
}

void ListForm::on_downloadButton_pressed() {
  QSize size = ui->downloadButton->iconSize();
  size.setWidth(size.width() - 5);
  size.setHeight(size.height() - 5);

  ui->downloadButton->setIconSize(size);
}

void ListForm::on_downloadButton_released() {
  QSize size = ui->downloadButton->iconSize();
  size.setWidth(size.width() + 5);
  size.setHeight(size.height() + 5);

  ui->downloadButton->setIconSize(size);
}

void ListForm::onDownloadingTick(int iPercent, int iDownloadPerSecond,
                                 int status) {
  if (iPercent == 100 && status == RecordDownloadReceiver::DS_COMPELETED) {
    ui->speedLabel->setVisible(false);
    ui->percentageLabel->setVisible(false);
    ui->downloadButton->setIcon(QIcon(":/resource/u579.png"));
    ui->downloadButton->setIconSize(ui->downloadButton->size());
    return;
  }

  if (status == RecordDownloadReceiver::DS_UNCOMPLETED) {
    ui->speedLabel->setVisible(false);
    ui->percentageLabel->setVisible(false);
    ui->downloadButton->setIcon(QIcon(":/resource/u620.png"));
  }

  if (status == RecordDownloadReceiver::DS_DOWNLOADING) {
    ui->downloadButton->setIcon(QIcon(":/resource/u1610.png"));
    if (!ui->speedLabel->isVisible()) ui->speedLabel->setVisible(true);
    if (!ui->percentageLabel->isVisible())
      ui->percentageLabel->setVisible(true);

#define DS_PB (1024LL * 1024LL * 1024LL * 1024LL * 1024LL)
#define DS_TB (1024LL * 1024LL * 1024LL * 1024LL)
#define DS_GB (1024 * 1024 * 1024)
#define DS_MB (1024 * 1024)
#define DS_KB (1024)

    ui->percentageLabel->setText(QString::number(iPercent) + "%");
    char szData[16] = "NaUN/s";
    if (iDownloadPerSecond >= DS_GB) {
      sprintf_s(szData, 16, "%.2lfGB/s", (double)iDownloadPerSecond / DS_GB);
    } else if (iDownloadPerSecond >= DS_MB) {
      sprintf_s(szData, 16, "%.2lfMB/s", (double)iDownloadPerSecond / DS_MB);
    } else if (iDownloadPerSecond >= DS_KB) {
      sprintf_s(szData, 16, "%.2lfKB/s", (double)iDownloadPerSecond / DS_KB);
    } else if (iDownloadPerSecond >= 0) {
      sprintf_s(szData, 16, "%dB/s", iDownloadPerSecond);
    }
    ui->speedLabel->setText(QString(szData));
  }

#undef DS_PB
#undef DS_TB
#undef DS_GB
#undef DS_MB
#undef DS_KB
}

void ListForm::onDownloadPrompt(QString qstrInfo) {
  if (qstrInfo.isEmpty()) {  // 开始下载。
    ui->downloadButton->setIcon(QIcon(":/resource/u1610.png"));
    ui->speedLabel->setVisible(true);
    ui->percentageLabel->setVisible(true);
    return;
  }

  ui->speedLabel->setVisible(false);
  ui->percentageLabel->setVisible(false);
  ui->downloadButton->setIcon(QIcon(":/resource/u620.png"));
  ui->downloadButton->setIconSize(ui->downloadButton->size());
  QPoint pos = QGuiApplication::primaryScreen()->geometry().center();
  Scene_Record_Warning::ShowMessage(pos, qstrInfo);
}
