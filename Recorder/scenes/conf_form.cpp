#include "conf_form.h"
#include "ui_conf_form.h"

ConfForm::ConfForm(QWidget* parent)
    : QPushButton(parent)
    , ui(new Ui::ConfForm)
{
    ui->setupUi(this);
}

ConfForm::~ConfForm()
{
    delete ui;
}

void ConfForm::update_display(const QVariantMap& info)
{
}
