#include "notify_dialog.h"
#include "ui_notify_dialog.h"
#include <QDesktopWidget>
#include <QEvent>

NotifyDialog::NotifyDialog(QWidget *parent) :
    QDialog(parent),
    timer(new QTimer(this)),
    _last_time(6000),
    ui(new Ui::NotifyDialog)
{
    ui->setupUi(this);

    this->set_title(QObject::tr("异常提醒"));

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);//去掉标题栏

    ui->groupBox->hide();

    timer->setSingleShot(true);
    this->connect(timer,SIGNAL(timeout()),this,SLOT(on_closeButton_clicked()));

    QDesktopWidget* desktopWidget = QApplication::desktop();
     //获取可用桌面大小
    QRect deskRect = desktopWidget->availableGeometry();
    _pos = deskRect.bottomRight();
}

NotifyDialog::~NotifyDialog()
{
    delete ui;
}

void NotifyDialog::NotifyMessage(QString message, bool buttonBox, QString accept, QString reject, int last_time)
{
    this->set_title(QObject::tr("异常提醒"));
    this->set_content(message);

    if(buttonBox)
        this->set_buttonBox(accept, reject);
    else
        ui->groupBox->hide();

    this->set_disappearTime(last_time);

    this->adjustSize();

    this->move(_pos.x() - this->width(), _pos.y() - this->height());

    this->show();

    if(timer->isActive())
        timer->stop();
    timer->start(_last_time);
}

void NotifyDialog::set_content(QString content)
{
    ui->content->setText(content);
}
void NotifyDialog::set_title(QString title)
{
    ui->title->setText(title);
}
void NotifyDialog::set_buttonBox(QString accept, QString reject)
{
    ui->groupBox->show();
    ui->acceptButton->setText(accept);
    ui->rejectButton->setText(reject);
}

void NotifyDialog::set_disappearTime(int msec)
{
    _last_time = msec;
}

bool NotifyDialog::event(QEvent *e)
{
    switch(e->type()){
    case QEvent::HoverEnter:
        if(timer->isActive())
            timer->stop();
        break;
    case QEvent::HoverLeave:
        timer->start(2000);
        break;
    }

    return QDialog::event(e);
}

void NotifyDialog::on_closeButton_clicked()
{
    if(timer->isActive())
        timer->stop();
    emit notifyRejected();
    this->hide();
}

void NotifyDialog::on_acceptButton_clicked()
{
    emit notifyAccepted();
    this->hide();
}

void NotifyDialog::on_rejectButton_clicked()
{
    emit notifyRejected();
    this->hide();
}
