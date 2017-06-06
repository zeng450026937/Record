#include "login_form.h"
#include "ui_login_form.h"
#include "Recorder/recorder_shared.h"
#include "scenes/scene_record_warning.h"

#include <QShowEvent>
#include <QTimerEvent>

LoginForm::LoginForm(RecorderShared* shared, QWidget *parent) :
    _shared(shared),
    QDialog(parent),
    ui(new Ui::LoginForm)
{
    ui->setupUi(this);
    //this->setWindowModality(Qt::ApplicationModal);

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);//去掉标题栏
    //this->setModal(true);

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

void LoginForm::receive_connection_notify(int state, QString text)
{
    switch(state){
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
}

void LoginForm::userLogin(QString user, QString password)
{
	user = "301255";
	password = "20170224";
    _shared->UserLogin(user, password);
    ui->loginPushButton->setEnabled(false);
}

void LoginForm::on_loginPushButton_clicked()
{
    _user = ui->userLineEdit->text();
    _password = ui->passwordLineEdit->text();

    this->userLogin(_user, _password);
}

void LoginForm::on_exitPushButton_clicked()
{
    emit aboutToExitApp();
    this->hide();

    this->reject();
}
