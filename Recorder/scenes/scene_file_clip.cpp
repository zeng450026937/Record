#include "scene_file_clip.h"
#include "ui_scene_file_clip.h"

#include "scene_record_warning.h"
#include "ffmpeg_wrapper.h"
#include "recorder_shared.h"
#include "config.h"
#include <QFile>
#include <QMessageBox>

Scene_File_Clip::Scene_File_Clip(RecorderShared *sharedData, QWidget *parent) :
    QDialog(parent),
	_sharedData(sharedData),
    ui(new Ui::Scene_File_Clip),
    _ffmpeg(new Ffmpeg_Wrapper(this))
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::SubWindow);//去掉标题栏

    _file_extention = _sharedData->GetConfig()->_file_extention;
}

Scene_File_Clip::~Scene_File_Clip()
{
    delete ui;
}

void Scene_File_Clip::set_clip(QVariantMap clip_info)
{
    _input_file = clip_info.value("input_file").toString();

    _output_name = clip_info.value("output_name").toString();

    _start_time = clip_info.value("start_time").toInt();
    _duration = clip_info.value("duration").toInt();

    ui->file_name_lineEdit->setText(_output_name);

    _file_extention = _input_file.right(4);

    if(!_output_name.endsWith( _file_extention )){
        _output_name.append( _file_extention );
    }
}

void Scene_File_Clip::on_cancel_btn_clicked()
{
    this->reject();
    ui->file_name_lineEdit->clear();
}

void Scene_File_Clip::on_confirm_btn_clicked()
{
    _output_name = ui->file_name_lineEdit->text();
    if(!_output_name.endsWith( _file_extention )){
        _output_name.append( _file_extention );
    }

    QString _output_dir = _sharedData->GetConfig()->_output_dir;

    if(QFile(_output_dir + "/" + _output_name).exists()){

        QMessageBox msgBox;
        msgBox.setText("已存在同名剪辑文件！");
        msgBox.setInformativeText("点击Yes覆盖原剪辑文件，点击No修改剪辑命名，点击Cancel取消剪辑操作");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();

        switch (ret) {
            case QMessageBox::Yes:
                // Save was clicked
                this->setCursor(QCursor(Qt::WaitCursor));


                _ffmpeg->ClipFile(_sharedData->GetConfig()->_ffmpeg_path,
                                  _input_file,
                                  _output_dir + "/" + _output_name,
                                  _start_time,
                                  _duration);

                this->unsetCursor();

                emit output_file( _output_dir + "/" + _output_name, _start_time );

                this->accept();
                ui->file_name_lineEdit->clear();
                break;
            case QMessageBox::No:
                // Don't Save was clicked
                ui->file_name_lineEdit->setFocus();
                break;
            case QMessageBox::Cancel:
                // Cancel was clicked
                this->reject();
                ui->file_name_lineEdit->clear();
                break;
            default:
                // should never be reached
                break;
          }
    }
    else{
        this->setCursor(QCursor(Qt::WaitCursor));


        _ffmpeg->ClipFile(_sharedData->GetConfig()->_ffmpeg_path,
                          _input_file,
                          _output_dir + "/" + _output_name,
                          _start_time,
                          _duration);

        this->unsetCursor();

        emit output_file( _output_dir + "/" + _output_name, _start_time );

        this->accept();
        ui->file_name_lineEdit->clear();
    }

}

