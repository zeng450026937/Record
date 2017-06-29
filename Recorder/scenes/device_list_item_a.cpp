#include "device_list_item_a.h"
#include "ui_device_list_item_a.h"

#include <QHelpEvent>
#include <QTime>
#include <QToolTip>

DeviceListItemA::DeviceListItemA(QWidget *parent)
    : QWidget(parent), ui(new Ui::DeviceListItemA) {
  ui->setupUi(this);
  ui->checkBox->setTristate(false);
  ui->prefixLabel->setText(QObject::tr("当前剩余时间 : "));
}

DeviceListItemA::~DeviceListItemA() { delete ui; }

void DeviceListItemA::update_display(QVariantMap info) {
  if (_info != info) {
    _info = info;

    ui->nameLabel->setText(_info.value("userName").toString());
    ui->statusLable->setText(_info.value("status").toString());

    int iper = _info.value("batteryPercent").toString().toInt();

    if (iper > 60) {
      ui->batteryLabe->setStyleSheet(
          "border-image: url(:/resource/u1405.png);");
    } else if (iper > 20) {
      ui->batteryLabe->setStyleSheet(
          "border-image: url(:/resource/u1412.png);");
    } else {
      ui->batteryLabe->setStyleSheet(
          "border-image: url(:/resource/u1418.png);");
    }

    int percent = _info.value("batteryPercent").toString().toInt();
    if (percent <= 0) {
      ui->batteryLabe->setText(
          QObject::tr("\350\216\267\345\217\226\344\270\255\342\200\246"));
      ui->timeLable->setText(
          QObject::tr("\350\216\267\345\217\226\344\270\255\342\200\246"));
    } else {
      ui->batteryLabe->setText(_info.value("batteryPercent").toString() + "%");

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

void DeviceListItemA::on_checkBox_stateChanged(int arg1) {
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

  emit checkBox_stateChanged(checked, _info);
}
