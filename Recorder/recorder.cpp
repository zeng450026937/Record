#include "recorder.h"
#include "ui_recorder.h"
#include <QGraphicsEffect>
#include <QDesktopWidget>

#include "recorder_shared.h"
#include "scenes/scene_record_warning.h"
#include "scenes/scene_setting.h"
#include "scenes/scene_record.h"
#include "scenes/scene_file.h"
#include "service/service_thread.h"

Recorder::Recorder(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Recorder),
    _scene_file(nullptr),
    _scene_record(nullptr),
    _scene_setting(nullptr)
{
    ui->setupUi(this);

    _sharedData = ServiceThread::GetInstance()->GetRecordShared();

    QDesktopWidget* desktopWidget = QApplication::desktop();

     //获取可用桌面大小
    QRect deskRect = desktopWidget->availableGeometry();

     //获取设备屏幕大小
    QRect screenRect = desktopWidget->screenGeometry();

    if(deskRect.width() < 1280 || deskRect.height() < 800)
        this->setGeometry(deskRect);

    _scene_file = new Scene_File(_sharedData,this);
    _scene_record = new Scene_Record(_sharedData,this);
    _scene_setting = new Scene_Setting(_sharedData,this);

    _scene_record->setEnabled(false);

    ui->stackedWidget->addWidget(_scene_file);
    ui->stackedWidget->addWidget(_scene_record);
    ui->stackedWidget->addWidget(_scene_setting);

    ui->stackedWidget->setCurrentWidget(_scene_record);
    ui->record_btn->setChecked(true);
    ui->user_lable->hide();
    ui->hello_lable->hide();

    ui->titlebar->installEventFilter(this);
    ui->stackedWidget->installEventFilter(this);

    this->setWindowFlags(Qt::FramelessWindowHint);//去掉标题栏
    
/*
    QGraphicsDropShadowEffect *shadow_effect = new QGraphicsDropShadowEffect(this);
    shadow_effect->setOffset(0);
    shadow_effect->setColor(Qt::darkGray);
    shadow_effect->setBlurRadius(25);
    ui->groupBox->setGraphicsEffect(shadow_effect);
*/
}

Recorder::~Recorder()
{
    delete ui;
}

bool Recorder::event(QEvent *event)
{
    if ( event->type() == QEvent::WindowActivate ) {
        //this->setStyleSheet("border:1px solid rgb(52, 139, 204);");
    }
    else if ( event->type() == QEvent::WindowDeactivate ) {
        //this->setStyleSheet("border:1px solid gray;");
    }
    return QWidget::event( event );
}

bool Recorder::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick ||
        event->type() == QEvent::MouseMove    ){

        if (obj == ui->titlebar){
            return false;//handle further
        }else if(obj == ui->stackedWidget){
            return true;//stop handle further
        }else{
            return true;//stop handle further
        }
    }
    else{
        return QWidget::eventFilter(obj, event);
    }
}

void Recorder::mousePressEvent(QMouseEvent *event)
{
    _movePos = event->globalPos() - this->pos();
}

void Recorder::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos() - _movePos);
}

void Recorder::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    emit ui->max_btn->click();
}

void Recorder::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
}


void Recorder::on_close_btn_clicked()
{
    if(_scene_record->aboutToStop() == 0)
        this->close();
}

void Recorder::on_max_btn_clicked()
{
    if (!this->isFullScreen()){
        this->showFullScreen();
    }else{
        this->showNormal();
    }
}

void Recorder::on_min_btn_clicked()
{
    this->showMinimized();
}

void Recorder::on_record_btn_clicked(bool checked)
{
    if(checked){
        ui->record_btn->setChecked(true);
        ui->file_btn->setChecked(false);
        ui->setting_btn->setChecked(false);

        ui->stackedWidget->setCurrentWidget(_scene_record);
    }else{
        ui->record_btn->setChecked(true);
        ui->file_btn->setChecked(false);
        ui->setting_btn->setChecked(false);
    }
}

void Recorder::on_file_btn_clicked(bool checked)
{
    if(checked){
        ui->record_btn->setChecked(false);
        ui->file_btn->setChecked(true);
        ui->setting_btn->setChecked(false);

        ui->stackedWidget->setCurrentWidget(_scene_file);
    }else{
        ui->record_btn->setChecked(false);
        ui->file_btn->setChecked(true);
        ui->setting_btn->setChecked(false);
    }
}

void Recorder::on_setting_btn_clicked(bool checked)
{
    if(checked)
    {
        ui->record_btn->setChecked(false);
        ui->file_btn->setChecked(false);
        ui->setting_btn->setChecked(true);

        ui->stackedWidget->setCurrentWidget(_scene_setting);
    }
    else
    {
        ui->record_btn->setChecked(false);
        ui->file_btn->setChecked(false);
        ui->setting_btn->setChecked(true);
    }
}
