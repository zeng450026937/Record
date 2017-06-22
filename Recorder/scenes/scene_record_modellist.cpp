#include "scene_record_modellist.h"
#include "ui_scene_record_modellist.h"

#include "recorder_shared.h"
#include "scene_record_addinfo.h"
#include "template_list_delegate_a.h"

Scene_Record_ModelList::Scene_Record_ModelList(RecorderShared *sharedData, QWidget *parent) :
    QDialog(parent),
	_sharedData(sharedData),
    ui(new Ui::Scene_Record_ModelList),
    _scene_record_addinfo(new Scene_Record_AddInfo)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint  | Qt::SubWindow);//去掉标题栏

    this->init_file_model();

    connect(_scene_record_addinfo,SIGNAL(add_conference_info(QVariant)),this,SLOT(set_conference_info(QVariant)));
}

Scene_Record_ModelList::~Scene_Record_ModelList()
{
    if(_scene_record_addinfo){
        delete _scene_record_addinfo;
    }

    delete ui;
}

void Scene_Record_ModelList::mousePressEvent(QMouseEvent *event)
{
    _movePos = event->globalPos() - this->pos();

    return QDialog::mousePressEvent(event);
}

void Scene_Record_ModelList::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos() - _movePos);

    return QDialog::mouseMoveEvent(event);
}

void Scene_Record_ModelList::init_file_model()
{
    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->listView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->listView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->listView->setWrapping(false);
    ui->listView->setModel(_sharedData->GetModel(ModelUpdater::TemplateModel));

    TemplateListDelegateA* list_delegate = new TemplateListDelegateA(this);
    ui->listView->setItemDelegateForColumn(0, list_delegate);
}


void Scene_Record_ModelList::set_conference_info(QVariant info)
{
    QVariantMap conference_info = info.toMap();

    //_sharedData->AddTemplate(conference_info);

    emit modelList_selected(conference_info);
}

void Scene_Record_ModelList::on_confirm_btn_clicked()
{
    if (_scene_record_addinfo == nullptr){
        _scene_record_addinfo = new Scene_Record_AddInfo();
    }

    QPoint pos = this->geometry().center();
    _scene_record_addinfo->move(pos.x() - _scene_record_addinfo->width() / 2, pos.y() - _scene_record_addinfo->height() / 2);

    this->setEnabled(false);

    if ( _scene_record_addinfo->exec() == QDialog::Accepted ) {
        ui->listView->clearSelection();
        this->close();
    }

    this->setEnabled(true);
}

void Scene_Record_ModelList::on_cancel_btn_clicked()
{
    ui->listView->clearSelection();
    this->close();
}

void Scene_Record_ModelList::on_listView_clicked(const QModelIndex &index)
{
    QVariantMap info_map = index.data(Qt::UserRole).toMap();
    emit modelList_selected(info_map);
    ui->listView->clearSelection();
    this->close();
}
