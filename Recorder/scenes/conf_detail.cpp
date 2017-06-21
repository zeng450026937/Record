#include "conf_detail.h"
#include "ui_conf_detail.h"
#include "conf_form.h"
#include "service/service_thread.h"
#include "service/conf_service_impl.h"

ConfDetail::ConfDetail(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfDetail)
{
    ui->setupUi(this);

    QObject::connect(ui->backButton, &QPushButton::clicked, this, &ConfDetail::goBack);
}

ConfDetail::~ConfDetail()
{
    delete ui;
}

void ConfDetail::setUuid(const QString &uuid)
{
    QVariantMap info =  ServiceThread::GetInstance()->GetConfService()->conferenceInfo(uuid);

    ui->listWidget->clear();

    foreach (QString conf, info.value("devices").toStringList()) {
        ConfForm *detail = new ConfForm(this);
        detail->update_display(info);
        ui->listWidget->setItemWidget(new QListWidgetItem(ui->listWidget),detail);
    }
}
