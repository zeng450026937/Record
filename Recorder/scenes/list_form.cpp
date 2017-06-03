#include "list_form.h"
#include "ui_list_form.h"
#include <QDateTime>
#include <QDebug>

ListForm::ListForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ListForm)
{
    ui->setupUi(this);

    QIcon icon = QIcon(":/resource/u620.png");
    ui->downloadButton->setIcon(icon);
    ui->downloadButton->setIconSize(ui->downloadButton->size());

    ui->dateLabel->clear();
    ui->timeLabel->clear();
    ui->titleLabel->clear();
    ui->keywordLabel->clear();
    ui->locationLabel->clear();
}

ListForm::~ListForm()
{
    delete ui;
}

void ListForm::update_display(const QVariantMap& info)
{
    QString text;
    QDate date;
    QTime time;

    text = info.value("date").toString();
    if(text != _info.value("date").toString()){
        date = QDate::fromString(text, "yyyyMMdd");
        ui->dateLabel->setText(date.toString("yyyy年MM月dd\346\227\245"));
    }

    text = info.value("time").toString();
    if(text != _info.value("time").toString()){
        time = QTime::fromString(text, "hhmmss");
        ui->timeLabel->setText(time.toString("hh:mm:ss"));
    }

    text = info.value("title").toString();
    if(text != _info.value("title").toString())
        ui->titleLabel->setText(text);

    text = info.value("user_id").toString();
    if(text.isEmpty()){
        ui->userGroupBox->hide();
    }
    else{
        if(text != _info.value("user_id").toString()){

            if(!ui->userGroupBox->isVisible())
                ui->userGroupBox->show();

            QString user_name = info.value("user_name").toString();
            ui->idLabel->setText(user_name+"("+text+")");
        }

        text = info.value("gps_address").toString();
        if(text != _info.value("gps_address").toString())
            ui->locationLabel->setText(text);

        text = info.value("tag").toString();
        if(text != _info.value("tag").toString())
            ui->keywordLabel->setText(text);
    }

    int completed = info.value("completed").toInt();

    if(completed == 2){
        ui->speedLabel->setVisible(true);
        ui->percentageLabel->setVisible(true);
    }
    else{
        ui->speedLabel->setVisible(false);
        ui->percentageLabel->setVisible(false);
    }

    if(ui->speedLabel->isVisible()){

        QString subfix = "KB/S";

        int speed = info.value("speed").toInt();

        if(speed >= 1024){
            subfix = "M/S";
            speed /= 1024;
        }
        ui->speedLabel->setText(QString::number(speed)+subfix);
    }
    if(ui->percentageLabel->isVisible()){

        QString subfix = "%";

        int percentage = info.value("percentage").toInt();

        ui->percentageLabel->setText(QString::number(percentage)+subfix);
    }

    if(completed != _info.value("completed").toInt()){

        QIcon icon;

        switch(completed){
        case 0:
            icon = QIcon(":/resource/u620.png");
            break;
        case 1:
            icon = QIcon(":/resource/u579.png");
            break;
        case 2:
            icon = QIcon(":/resource/u1610.png");
            break;
        }

        ui->downloadButton->setIcon(icon);
        ui->downloadButton->setIconSize(ui->downloadButton->size());
    }

    _info = info;
}

void ListForm::on_downloadButton_clicked()
{
    int completed = _info.value("completed").toInt();

    if(completed == 0){
        QString uuid = _info.value("uuid").toString();
        if(_info.contains("conference_uuid"))
            uuid = _info.value("conference_uuid").toString();

        emit button_clicked(uuid);
    }
}

void ListForm::on_downloadButton_pressed()
{
    QSize size = ui->downloadButton->iconSize();
    size.setWidth( size.width()-5 );
    size.setHeight( size.height()-5 );

    ui->downloadButton->setIconSize( size);
}

void ListForm::on_downloadButton_released()
{
    QSize size = ui->downloadButton->iconSize();
    size.setWidth( size.width()+5 );
    size.setHeight( size.height()+5 );

    ui->downloadButton->setIconSize( size);
}
