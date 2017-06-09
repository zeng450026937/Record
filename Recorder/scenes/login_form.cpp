
#include <QShowEvent>
#include <QTimerEvent>

#include "login_form.h"
#include "ui_login_form.h"
#include "Recorder/recorder_shared.h"
#include "scenes/scene_record_warning.h"
#include "service/white_list.h"
#include "service/service_thread.h"
#include "service/account/account_center.h"
#include "service/user_service_impl.h"
#include "common/config.h"
#include "service/white_list.h"

LoginForm::LoginForm(RecorderShared* shared, QWidget *parent) :
    _record_shared(shared),
    QDialog(parent),
    ui(new Ui::LoginForm),
	_service(ServiceThread::GetInstance())
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);//去掉标题栏

	ui->loginPushButton->setEnabled(false);
	ui->passwordLineEdit->setEnabled(false);
	ui->userLineEdit->setEnabled(false);

	_service->start();
	this->connect(_service, SIGNAL(service_ready()), this, SLOT(on_service_is_readied()), Qt::QueuedConnection);
}

LoginForm::~LoginForm()
{
    delete ui;
}

void LoginForm::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    if(this->isVisible()){
        ui->userLineEdit->setFocus();
        ui->loginPushButton->setEnabled(true);
        this->activateWindow();
    }
}

void LoginForm::on_login_result(QString qstrResultInfo)
{
	if (qstrResultInfo.isEmpty())
	{        
		accept();
	}
	else
	{
		QPoint pos = this->geometry().center();
        Scene_Record_Warning::ShowMessage(pos, qstrResultInfo);
        ui->loginPushButton->setEnabled(true);
        ui->passwordLineEdit->setEnabled(true);
        ui->userLineEdit->setEnabled(true);
	}
}

void LoginForm::on_loginPushButton_clicked()
{
    ui->userLineEdit->setText("301255");
    ui->passwordLineEdit->setText("20170224");

	QString qstrUserId = ui->userLineEdit->text().trimmed();

	QStringList lsUserId = _service->GetLoginWhiteList()->whiteList();

	Config *pConfig = _record_shared->GetConfig();
	if (pConfig->_hackers)
		lsUserId << ui->userLineEdit->text();

	if (!lsUserId.contains(qstrUserId))
	{
		QPoint pos = this->geometry().center();
		Scene_Record_Warning::ShowMessage(pos, "您暂时无权限使用");
		return;
	}

	ui->userLineEdit->setEnabled(false);
	ui->passwordLineEdit->setEnabled(false);
	ui->loginPushButton->setEnabled(false);

	_user_service->userLogin(ui->userLineEdit->text(), ui->passwordLineEdit->text());
}

void LoginForm::on_exitPushButton_clicked()
{
	this->hide();
    this->reject();
}

void LoginForm::on_service_is_readied()
{
	ui->loginPushButton->setEnabled(true);
	ui->passwordLineEdit->setEnabled(true);
	ui->userLineEdit->setEnabled(true);
	_user_service = _service->GetUserService();
	connect(_user_service, SIGNAL(loginResult(QString)), this, SLOT(on_login_result(QString)));
}
