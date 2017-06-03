#include "notification_center.h"
#include "ui_notification_center.h"
#include <QDesktopWidget>
#include <QDebug>

#include "scenes/notify_dialog.h"

NotificationCenter::NotificationCenter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NotificationCenter),
    _notify_layout(new QVBoxLayout)
{
    ui->setupUi(this);

    ui->title->setText(QObject::tr("通知中心"));

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog | Qt::WindowStaysOnTopHint);//去掉标题栏

    QDesktopWidget* desktopWidget = QApplication::desktop();
     //获取可用桌面大小
    QRect deskRect = desktopWidget->availableGeometry();
    _position = deskRect.bottomRight();
}

NotificationCenter::~NotificationCenter()
{
    delete ui;
}

QWidget* NotificationCenter::addNotify(int notify_type, QString content, QString accept, QString reject)
{
    NotifyDialog* notify = new NotifyDialog();
    _notify_layout->addWidget(notify);
    _notify_layout->addWidget(new NotifyDialog());
    _notify_layout->addWidget(new NotifyDialog());
    _notify_layout->addWidget(new NotifyDialog());

    ui->groupBox->setLayout(_notify_layout);

    ui->groupBox->adjustSize();
    this->adjustSize();
    qDebug()<<this->width()<<this->height();
    //this->move(_position.x() - this->width(), _position.y() - this->height());

    return notify;
}

void NotificationCenter::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);

    this->adjustSize();
    //this->move(_position.x() - this->width(), _position.y() - this->height());
}

void NotificationCenter::on_closeButton_clicked()
{
    this->hide();
}
