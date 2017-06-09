#ifndef LOGIN_FORM_H
#define LOGIN_FORM_H

#include <QDialog>

class ServiceThread;
class RecorderShared;
class UserServiceImpl;

namespace Ui {
class LoginForm;
}

class LoginForm : public QDialog
{
    Q_OBJECT

public:
    explicit LoginForm(RecorderShared* shared, QWidget *parent = 0);
    ~LoginForm();

protected:
    virtual void showEvent(QShowEvent *event) override;

private slots:
	void on_login_result(QString qstrResultInfo);

    void on_loginPushButton_clicked();

    void on_exitPushButton_clicked();

	void on_service_is_readied();

private:
    Ui::LoginForm  *ui;
	ServiceThread  *_service;
    RecorderShared *_record_shared;

	UserServiceImpl	*_user_service;
};

#endif // LOGIN_FORM_H
