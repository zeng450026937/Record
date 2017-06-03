#ifndef LOGIN_FORM_H
#define LOGIN_FORM_H

#include <QDialog>

class RecorderShared;

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
    virtual void timerEvent(QTimerEvent *e) override;

signals:
    void aboutToExitApp();

private slots:
    void receive_connection_notify(int state, QString text);

    void userLogin(QString user, QString password);

    void on_loginPushButton_clicked();

    void on_exitPushButton_clicked();

private:
    Ui::LoginForm *ui;
    RecorderShared* _shared;
    QString _user;
    QString _password;
};

#endif // LOGIN_FORM_H
