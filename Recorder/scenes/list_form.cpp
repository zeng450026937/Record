#include "list_form.h"
#include <QDateTime>
#include <QDebug>
#include <Recorder/recorder_shared.h>
#include <service/command/RecordDownloadService.h>
#include "ui_list_form.h"
#include "scene_file_dl.h"

ListForm::ListForm() :
    RecordDownloadReceiver(),
    ui(new Ui::ListForm),
    _download_status(DS_UNEXSITS)
{
    ui->setupUi(this);

    QIcon icon = QIcon(":/resource/u620.png");
    ui->downloadButton->setIcon(icon);
    ui->downloadButton->setIconSize(ui->downloadButton->size());
    ui->speedLabel->setVisible(false);
    ui->percentageLabel->setVisible(false);

    ui->dateLabel->clear();
    ui->timeLabel->clear();
    ui->titleLabel->clear();
    ui->keywordLabel->clear();
    ui->locationLabel->clear();

    connect(this, SIGNAL(downloading_tick(int, int)), this, SLOT(onDownloadingTick(int,int)));
}

ListForm::~ListForm()
{
    delete ui;
}

void ListForm::update_display(const QVariantMap& info)
{
    QString text = info.value("date").toString();
    if(text != _info.value("date").toString()){
        ui->dateLabel->setText(QDate::fromString(text, "yyyy-MM-dd")
            .toString("yyyy年MM月dd\346\227\245"));
    }

    text = info.value("time").toString();
    if(text != _info.value("time").toString()){
        ui->timeLabel->setText(text);
    }

    text = info.value("title").toString();
    if(text != _info.value("title").toString())
        ui->titleLabel->setText(text);

    text = info.value("userId").toString();
    if(text.isEmpty()){
        ui->userGroupBox->hide();
    }
    else{
        if(text != _info.value("userId").toString())
        {
            if(!ui->userGroupBox->isVisible())
                ui->userGroupBox->show();

            QString user_name = info.value("userName").toString();
            ui->idLabel->setText(user_name+"("+text+")");
        }

        text = info.value("gpsAddress").toString();
        if(text != _info.value("gpsAddress").toString())
            ui->locationLabel->setText(text);

        text = info.value("tag").toString();
        if(text != _info.value("tag").toString())
            ui->keywordLabel->setText(text);
    }

    if (_info.isEmpty())
    {
        QIcon icon;
        _download_status = GetDownloadStatus(info["fileUuid"].toString(),
            info["conferenceUuid"].toString(),
            info["deviceUuid"].toString());
        switch (_download_status)
        {
        case DS_UNCOMPLETED:
            icon = QIcon(":/resource/u620.png");
            break;
        case DS_COMPELETED:
            icon = QIcon(":/resource/u579.png");
            break;
        case DS_UNEXSITS:
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
    if (_download_status != DS_COMPELETED)
    {
        Scene_File_DL promptDialog;
        if (promptDialog.exec() == QDialog::Rejected)
            return;
        
        switch (_info["recordType"].toInt())
        {
        case RecorderShared::RT_PERSONAL:
            RecordDownloadService::GetInstance()->DownloadRecord(
                this, RecorderShared::RT_PERSONAL,QString(),_info["conferenceUuid"].toString(),
                _info["deviceUuid"].toString(), _info["createTime"].toString(),_info["fileExtension"].toString());
            break;
        case RecorderShared::RT_CONFERENCE:
            RecordDownloadService::GetInstance()->DownloadRecord(
                this, RecorderShared::RT_CONFERENCE, QString(), _info["uuid"].toString(),
                _info["deviceUuid"].toString(), _info["createTime"].toString(), _info["fileExtension"].toString());
            break;
        case RecorderShared::RT_MOBILE:
            RecordDownloadService::GetInstance()->DownloadRecord(
                this, RecorderShared::RT_CONFERENCE, _info["fileUuid"].toString(), _info["conferenceUuid"].toString(),
                _info["deviceUuid"].toString(), _info["createTime"].toString(), _info["fileExtension"].toString());
            break;
        default:
            break;
        }
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

void ListForm::onDownloadingTick(int iPercent, int iDownloadPerSecond)
{
    if (iPercent == 100)
    {
        ui->speedLabel->setVisible(false);
        ui->percentageLabel->setVisible(false);
        ui->downloadButton->setIcon(QIcon(":/resource/u579.png"));
        ui->downloadButton->setIconSize(ui->downloadButton->size());
        return;
    }

#define DS_PB (1024LL*1024LL*1024LL*1024LL*1024LL)
#define DS_TB (1024LL*1024LL*1024LL*1024LL)
#define DS_GB (1024*1024*1024)
#define DS_MB (1024*1024)
#define DS_KB (1024)

    ui->percentageLabel->setText(QString::number(iPercent) + "%");
    char szData[16] = "NaUN/s";
    if (iDownloadPerSecond >= DS_GB)
    {
        sprintf_s(szData, 16, "%.2lfGB/s", (double)iDownloadPerSecond / DS_GB);
    }
    else if (iDownloadPerSecond >= DS_MB)
    {
        sprintf_s(szData, 16, "%.2lfMB/s", (double)iDownloadPerSecond / DS_MB);
    }
    else if (iDownloadPerSecond >= DS_KB)
    {
        sprintf_s(szData, 16, "%.2lfKB/s", (double)iDownloadPerSecond / DS_KB);
    }
    else if (iDownloadPerSecond >= 0)
    {
        sprintf_s(szData, 16, "%dB/s", iDownloadPerSecond);
    }
    ui->speedLabel->setText(QString(szData));

#undef DS_PB
#undef DS_TB
#undef DS_GB
#undef DS_MB
#undef DS_KB
}

#include "scenes/scene_record_warning.h"
#include <QScreen>
void ListForm::onDownloadPrompt(QString qstrInfo)
{
    if (qstrInfo.isEmpty())
    {
        ui->speedLabel->setVisible(true);
        ui->percentageLabel->setVisible(true);
        return;
    }

    ui->speedLabel->setVisible(false);
    ui->percentageLabel->setVisible(false);
    ui->downloadButton->setIcon(QIcon(":/resource/u620.png"));
    ui->downloadButton->setIconSize(ui->downloadButton->size());
    QPoint pos = QGuiApplication::primaryScreen()->geometry().center();
    Scene_Record_Warning::ShowMessage(pos, qstrInfo);
}
