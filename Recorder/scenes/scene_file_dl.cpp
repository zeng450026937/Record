#include "scene_file_dl.h"
#include "ui_scene_file_dl.h"

Scene_File_DL::Scene_File_DL(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Scene_File_DL)
{
    ui->setupUi(this);
    setModal(true);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);//去掉标题栏
    connect(ui->confirm_btn, SIGNAL(clicked()),this,SLOT(accept()));
}

Scene_File_DL::~Scene_File_DL()
{
    delete ui;
}

void Scene_File_DL::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
}

// void Scene_File_DL::show_dl(const QString& uuid)
// {
//     _uuid = uuid;
//     this->show();
// }

void Scene_File_DL::on_close()
{
    this->close();
}

void Scene_File_DL::on_confirm_btn_clicked()
{
    this->accept();
}
