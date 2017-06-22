#include "scene_setting.h"
#include "ui_scene_setting.h"

#include "config.h"
#include <service/service_thread.h>
#include <service/command/TemplateControl.h>
#include "recorder_shared.h"
#include "scene_record_addinfo.h"
#include "device_list_delegate_a.h"
#include "template_list_delegate.h"

#include <QFileDialog>

Scene_Setting::Scene_Setting(RecorderShared *sharedData, QWidget *parent) :
    QGroupBox(parent),
    _sharedData(sharedData),
    ui(new Ui::Scene_Setting),
    _scene_record_addinfo(new Scene_Record_AddInfo),
    m_pTemplateControl(nullptr)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint);//去掉标题栏

    this->initialize();

    m_pTemplateControl = ServiceThread::GetInstance()->GetTemplateControl();
    m_pTemplateControl->GetTemplateList();
}

Scene_Setting::~Scene_Setting()
{
    if(_scene_record_addinfo){
        delete _scene_record_addinfo;
    }
    delete ui;
}

void Scene_Setting::initialize()
{
    //prepare model
    ui->deviceList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->deviceList->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->deviceList->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->deviceList->setWrapping(false);
    ui->deviceList->setModel(_sharedData->GetModel(ModelUpdater::DeviceModel));

    DeviceListDelegateA* device_delegate = new DeviceListDelegateA(this);
    ui->deviceList->setItemDelegateForColumn(0, device_delegate);
    QObject::connect(device_delegate,SIGNAL(device_select_stateChanged(bool,QVariantMap)),
                     this,SLOT(receive_device_select_stateChanged(bool,QVariantMap)));

    ui->templateList->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->templateList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->templateList->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->templateList->setWrapping(false);
    ui->templateList->setModel(_sharedData->GetModel(ModelUpdater::TemplateModel));

    TemplateListDelegate* template_delegate = new TemplateListDelegate(this);
    ui->templateList->setItemDelegateForColumn(0, template_delegate);
    QObject::connect(template_delegate,SIGNAL(editTemplate(QVariantMap)),
                     this,SLOT(receive_editTemplate(QVariantMap)));
    QObject::connect(template_delegate,SIGNAL(deleteTemplate(QString)),
                     this,SLOT(receive_deleteTemplate(QString)));

    //prepare setting
    Config* config = _sharedData->GetConfig();

    QStringList combo_list = config->_clip_combo;

    ui->mark_comboBox->addItems( combo_list );
    ui->edit_comboBox->addItems( combo_list );

    for(int i = 0;i < combo_list.count();i++){
        if(combo_list.at(i) == config->_clip_prev){
            ui->mark_comboBox->setCurrentIndex(i);
            break;
        }
    }

    for(int i = 0;i < combo_list.count();i++){
        if(combo_list.at(i) == config->_clip_next){
            ui->edit_comboBox->setCurrentIndex(i);
            break;
        }
    }

    ui->output_lineEdit->setText( config->_output_dir );
    ui->cancel_btn->setEnabled(false);
    ui->save_btn->setEnabled(false);

    //connect singal&slot
    connect(ui->mark_comboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(setting_changed(QString)));
    connect(ui->edit_comboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(setting_changed(QString)));
    connect(ui->output_lineEdit,SIGNAL(textChanged(QString)),this,SLOT(setting_changed(QString)));
    connect(_sharedData,SIGNAL(conference_notify(int,bool,QVariantMap)),this,SLOT(receive_conference_notify(int,bool,QVariantMap)));
}

void Scene_Setting::receive_conference_notify(int state, bool result, QVariantMap conf)
{
    Q_UNUSED(conf);
    switch(state)
    {
    case RecorderShared::kConfCreated:
    case RecorderShared::kConfStarted:
    case RecorderShared::kConfPaused:
        if(result){
            ui->deviceList->setEnabled(false);
        }
        else{
            ui->deviceList->setEnabled(true);
        }
        break;
    case RecorderShared::kConfStoped:
        if(result){
            ui->deviceList->setEnabled(true);
        }
        else{
            ui->deviceList->setEnabled(false);
        }
        break;
    case RecorderShared::kConfSetted:
        break;
    }
}

void Scene_Setting::receive_device_select_stateChanged(bool checked, QVariantMap device)
{
    _sharedData->SelectDevice(checked, device);
}

void Scene_Setting::receive_editTemplate(QVariantMap info)
{
    if (_scene_record_addinfo == nullptr){
        _scene_record_addinfo = new Scene_Record_AddInfo();
    }

    QPoint pos = this->parentWidget()->parentWidget()->parentWidget()->geometry().topLeft() + ui->template_box->geometry().center();
    _scene_record_addinfo->move(pos.x() - _scene_record_addinfo->width() / 2, pos.y() - _scene_record_addinfo->height() / 2);

    ui->template_box->setEnabled(false);

    _scene_record_addinfo->setTemplateInfo(info);

    QVariantMap modified;

    if ( _scene_record_addinfo->exec() == QDialog::Accepted ) {
        //refresh data;
        modified = _scene_record_addinfo->getTemplateInfo();
        // _sharedData->SetTemplate(modified);
        m_pTemplateControl->SetTemplateinfo(modified["templateUuid"].toString(),
            modified["title"].toString(),
            modified["content"].toString(), modified["members"].toString());
    }

    ui->template_box->setEnabled(true);
    ui->templateList->setFocus();
}
void Scene_Setting::receive_deleteTemplate(QString uuid)
{
    m_pTemplateControl->DeleteTemplateInfo(uuid);
}

void Scene_Setting::on_fresh_btn_clicked()
{
    // _sharedData->refresh_deviceInfoList();
}

void Scene_Setting::on_add_btn_clicked()
{
    if (_scene_record_addinfo == nullptr){
        _scene_record_addinfo = new Scene_Record_AddInfo();
    }

    QPoint pos = this->parentWidget()->parentWidget()->parentWidget()->geometry().topLeft() + ui->template_box->geometry().center();
    _scene_record_addinfo->move(pos.x() - _scene_record_addinfo->width() / 2, pos.y() - _scene_record_addinfo->height() / 2);

    ui->template_box->setEnabled(false);

    QVariantMap modified;

    if ( _scene_record_addinfo->exec() == QDialog::Accepted ) {
        modified = _scene_record_addinfo->getTemplateInfo();
        m_pTemplateControl->AddTemplateInfo(modified["title"].toString(), 
            modified["content"].toString(), modified["members"].toString());
    }

    ui->template_box->setEnabled(true);
}

void Scene_Setting::on_select_folder_btn_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"), QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if(!dir.isEmpty()){
        ui->output_lineEdit->setText(dir);
    }
}

void Scene_Setting::setting_changed(QString text)
{
    ui->cancel_btn->setEnabled(true);
    ui->save_btn->setEnabled(true);
}

void Scene_Setting::on_save_btn_clicked()
{
    _sharedData->GetConfig()->_clip_prev = ui->mark_comboBox->currentText();
    _sharedData->GetConfig()->_clip_next = ui->edit_comboBox->currentText();
    _sharedData->GetConfig()->_output_dir = ui->output_lineEdit->text();

    _sharedData->SetFolder( _sharedData->GetConfig()->_output_dir );

    ui->cancel_btn->setEnabled(false);
    ui->save_btn->setEnabled(false);
}

void Scene_Setting::on_cancel_btn_clicked()
{
    ui->mark_comboBox->setCurrentText(_sharedData->GetConfig()->_clip_prev);
    ui->edit_comboBox->setCurrentText(_sharedData->GetConfig()->_clip_next);
    ui->output_lineEdit->setText(_sharedData->GetConfig()->_output_dir);

    ui->cancel_btn->setEnabled(false);
    ui->save_btn->setEnabled(false);
}
