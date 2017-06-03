#include "template_list_item_a.h"
#include "ui_template_list_item_a.h"

TemplateListItemA::TemplateListItemA(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TemplateListItemA)
{
    ui->setupUi(this);
    _prefix = QObject::tr("参会人员 : ");
}

TemplateListItemA::~TemplateListItemA()
{
    delete ui;
}

void TemplateListItemA::update_display(QVariantMap info)
{
    if(_info != info){
        _info = info;

        ui->titleLabel->setText(_info.value("title").toString());
        ui->memberLabel->setText(_prefix + _info.value("members").toString());
    }
}
