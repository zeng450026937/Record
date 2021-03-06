#include "device_list_item.h"
#include <QDebug>
#include <QHelpEvent>
#include <QTime>
#include <QToolTip>
#include "ui_device_list_item.h"

DeviceListItem::DeviceListItem(QWidget *parent)
    : QWidget(parent), ui(new Ui::DeviceListItem) {
  ui->setupUi(this);
  ui->checkBox->setTristate(false);
  ui->slashLabel->setText("/");
}

DeviceListItem::~DeviceListItem() { delete ui; }

void DeviceListItem::update_display(QVariantMap info) {
  if (_info != info) {
    _info = info;

    ui->nameLabel->setText(_info.value("userName").toString());
    ui->statusLable->setText(_info.value("status").toString());

    if (this->parentWidget()->isEnabled()) {
      if (_info.value("status").toString() == "offline") {
        ui->checkBox->setChecked(false);
        this->setEnabled(false);
      } else if (_info.value("status").toString() == "online") {
        this->setEnabled(true);
      }
    }

    int percent = _info.value("batteryPercent").toString().toInt();
    if (percent <= 0) {
      ui->batteryLabel->setText(
          QObject::tr("\350\216\267\345\217\226\344\270\255\342\200\246"));
      ui->timeLable->setText(
          QObject::tr("\350\216\267\345\217\226\344\270\255\342\200\246"));
    } else {
      ui->batteryLabel->setText(_info.value("batteryPercent").toString() + "%" +
                                QObject::tr("电量"));

      int seconds = _info.value("batteryTime").toString().toInt() * 60;
      QScopedPointer<QTime> time(new QTime((seconds / 3600) % 60,
                                           (seconds / 60) % 60, seconds % 60,
                                           (seconds * 1000) % 1000));

      ui->timeLable->setText(time->toString("h小时mm分钟"));
    }

    this->setToolTip(QObject::tr("DEVICE : ") +
                     _info.value("deviceName").toString());
  }
}

void DeviceListItem::on_checkBox_stateChanged(int arg1) {
  bool checked(false);

  switch (arg1) {
    case Qt::Unchecked:
      checked = false;
      break;
    case Qt::PartiallyChecked:  // should not go to here
      checked = true;
      break;
    case Qt::Checked:
      checked = true;
      break;
  }

  if (_info.value("status").toString() == "online" ||
      _info.value("status").toString() == "offline")
    emit checkBox_stateChanged(checked, _info);
  else
    ui->checkBox->setCheckState(Qt::Unchecked);
}
