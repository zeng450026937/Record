
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
    _shared(shared),
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
    this->connect(_shared,SIGNAL(connection_notify(int,QString)),this,SLOT(receive_connection_notify(int,QString)));
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
void LoginForm::timerEvent(QTimerEvent *e)
{
    Q_UNUSED(e);
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
	}
}

void LoginForm::receive_connection_notify(int state, QString text)
{
    switch(state)
	{
    case RecorderShared::kConnectOpened:
        break;
    case RecorderShared::kConnectFailed:
        ui->loginPushButton->setEnabled(false);
        break;
    case RecorderShared::kConnectClosed:
        ui->loginPushButton->setEnabled(false);
        break;
    case RecorderShared::kLoginOk:
        ui->loginPushButton->setEnabled(false);
        this->hide();
        this->accept();
        break;
    case RecorderShared::kLoginFailed:
        ui->loginPushButton->setEnabled(true);
        break;
    case RecorderShared::kLogoutOk:
        ui->loginPushButton->setEnabled(true);
        break;
    case RecorderShared::kLogoutFailed:
        ui->loginPushButton->setEnabled(false);
        break;
    }

	ui->loginPushButton->setEnabled(true);
	ui->passwordLineEdit->setEnabled(true);
	ui->loginPushButton->setEnabled(true);
}

void LoginForm::on_loginPushButton_clicked()
{
	QString qstrUserId = ui->userLineEdit->text().trimmed();
	ui->userLineEdit->setText(qstrUserId);

	QStringList lsUserId = _service->GetLoginWhiteList()->whiteList();

	Config *pConfig = _shared->GetConfig();
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
	emit aboutToExitApp();
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
