#include "conf_detail.h"
#include <QDebug>
#include "conf_form.h"
#include "list_form.h"
#include "recorder_shared.h"
#include "service/command/ConferenceMode.h"
#include "service/conf_service_impl.h"
#include "service/service_thread.h"
#include "ui_conf_detail.h"

ConfDetail::ConfDetail(QWidget *parent)
    : QWidget(parent), ui(new Ui::ConfDetail), conf_mode(Q_NULLPTR) {
  ui->setupUi(this);

  QObject::connect(ui->backButton, &QPushButton::clicked, this,
                   &ConfDetail::goBack);

  conf_mode = ServiceThread::GetInstance()->GetConferenceMode();
  QObject::connect(
      conf_mode, &ConferenceMode::getConferenceFiles, this,
      [=](const QVariantList &list) {
        while (ui->listWidget->count()) {
          ui->listWidget->itemWidget(ui->listWidget->item(0))->deleteLater();
          ui->listWidget->takeItem(0);
        }
        foreach (QVariant item, list) {
          QListWidgetItem *listItem = new QListWidgetItem(
              item.toMap().value("uuid").toString(), ui->listWidget);
          ListForm *listItemWidget = new ListForm();
          ui->listWidget->addItem(listItem);
          ui->listWidget->setItemWidget(listItem, listItemWidget);
          listItemWidget->update_display(item.toMap());
        }
      });
}

ConfDetail::~ConfDetail() { delete ui; }

void ConfDetail::setInfo(const QVariantMap &info) {
  _info = info;
  while (ui->listWidget->count()) {
    ui->listWidget->itemWidget(ui->listWidget->item(0))->deleteLater();
    ui->listWidget->takeItem(0);
  }
  ui->titleLabel->setText(_info.value("title").toString());
  ui->dateLabel->setText(_info.value("date").toString());
  ui->timeLabel->setText(_info.value("time").toString());
  if (conf_mode &&
      _info.value("recordType").toInt() != RecorderShared::RT_PERSONAL)
    conf_mode->GetConferenceFiles(_info.value("conferenceUuid").toString());
}
