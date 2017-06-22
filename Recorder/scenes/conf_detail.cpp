#include "conf_detail.h"
#include <QDebug>
#include "conf_form.h"
#include "service/conf_service_impl.h"
#include "service/service_thread.h"
#include "ui_conf_detail.h"

ConfDetail::ConfDetail(QWidget *parent)
    : QWidget(parent), ui(new Ui::ConfDetail) {
  ui->setupUi(this);

  QObject::connect(ui->backButton, &QPushButton::clicked, this,
                   &ConfDetail::goBack);
}

ConfDetail::~ConfDetail() { delete ui; }

void ConfDetail::setInfo(const QVariantMap &info) {
  _info = info;
  ui->listWidget->clear();
  ui->titleLabel->setText(_info.value("title").toString());
  ui->dateLabel->setText(_info.value("date").toString());
  ui->timeLabel->setText(_info.value("time").toString());
}
