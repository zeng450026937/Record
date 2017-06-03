#include "scene_record_warning.h"
#include "ui_scene_record_warning.h"

void Scene_Record_Warning::ShowMessage(QPoint pos, QString message, int last_time)
{
    Scene_Record_Warning* w = new Scene_Record_Warning();

    w->set_content(message);
    w->set_last_time(last_time);

    w->adjustSize();

    w->move(pos.x() - w->width() / 2, pos.y() - w->height() / 2);

    w->show();
}

Scene_Record_Warning::Scene_Record_Warning(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Scene_Record_Warning),
    timer(new QTimer(this)),
    _last_time(1500)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup | Qt::WindowStaysOnTopHint);//去掉标题栏
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->connect(timer,SIGNAL(timeout()),this,SLOT(on_close()));
}

Scene_Record_Warning::~Scene_Record_Warning()
{
    delete ui;
}

void Scene_Record_Warning::set_content(QString &content)
{
    ui->content->setText(content);
}
void Scene_Record_Warning::set_last_time(int last_time)
{
    _last_time = last_time;
}

void Scene_Record_Warning::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    timer->setSingleShot(true);
    timer->start(_last_time);
}

void Scene_Record_Warning::on_close()
{
    this->close();
}
