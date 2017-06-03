#include "scene_record_finish.h"
#include "ui_scene_record_finish.h"

Scene_Record_Finish::Scene_Record_Finish(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Scene_Record_Finish)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::SubWindow);//去掉标题栏
}

Scene_Record_Finish::~Scene_Record_Finish()
{
    delete ui;
}

void Scene_Record_Finish::set_file_name(QString file_name)
{
    ui->file_name_label->setText(file_name);
}

void Scene_Record_Finish::on_cancel_btn_clicked()
{
    this->reject();
}

void Scene_Record_Finish::on_confirm_btn_clicked()
{
    this->accept();
}
