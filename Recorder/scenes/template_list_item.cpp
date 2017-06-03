#include "template_list_item.h"
#include "ui_template_list_item.h"

TemplateListItem::TemplateListItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TemplateListItem)
{
    ui->setupUi(this);

    ui->editButton->setText(QObject::tr("编辑"));
    ui->deleteButton->setText(QObject::tr("删除"));
    ui->prefixLabel->setText(QObject::tr("参会人员 : "));

}

TemplateListItem::~TemplateListItem()
{
    delete ui;
}

void TemplateListItem::update_display(QVariantMap info)
{
    if(_info != info){
        _info = info;

        ui->titleLabel->setText(_info.value("title").toString());
        ui->memberLabel->setText(_info.value("members").toString());
    }
}

void TemplateListItem::on_editButton_clicked()
{
    emit editButton( _info );
}

void TemplateListItem::on_deleteButton_clicked()
{
    emit deleteButton( _info.value("uuid").toString() );
}
