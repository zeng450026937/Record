#include "scene_record_addinfo.h"
#include "ui_scene_record_addinfo.h"

Scene_Record_AddInfo::Scene_Record_AddInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Scene_Record_AddInfo)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::SubWindow);//去掉标题栏
}

Scene_Record_AddInfo::~Scene_Record_AddInfo()
{
    delete ui;
}

void Scene_Record_AddInfo::setTemplateInfo(QVariantMap& info)
{
    _info = info;

    ui->title_lineEdit->setText(_info.value("title").toString());
    ui->member_plainTextEdit->setPlainText(_info.value("members").toString());
}
QVariantMap Scene_Record_AddInfo::getTemplateInfo()
{
    return _info;
}

void Scene_Record_AddInfo::mousePressEvent(QMouseEvent *event)
{
    movePos = event->globalPos() - this->pos();
}

void Scene_Record_AddInfo::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos() - movePos);
}

void Scene_Record_AddInfo::on_confirm_btn_clicked()
{
    _info.insert("title", ui->title_lineEdit->text().trimmed());
    _info.insert("content", "");
    _info.insert("members", ui->member_plainTextEdit->toPlainText().trimmed());

    this->accept();
    emit add_conference_info(_info);

    ui->title_lineEdit->setFocus();

    ui->title_lineEdit->clear();
    ui->member_plainTextEdit->clear();
}

void Scene_Record_AddInfo::on_cancel_btn_clicked()
{
    this->reject();

    ui->title_lineEdit->setFocus();

    ui->title_lineEdit->clear();
    ui->member_plainTextEdit->clear();
}
