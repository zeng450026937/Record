#ifndef NOTIFY_DIALOG_H
#define NOTIFY_DIALOG_H

#include <QDialog>
#include <QTimer>

namespace Ui {
class NotifyDialog;
}

class NotifyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NotifyDialog(QWidget *parent = 0);
    ~NotifyDialog();

    void NotifyMessage(QString message, bool buttonBox = false, QString accept = "", QString reject = "", int last_time = 6000);

signals:
    void notifyAccepted();
    void notifyRejected();

protected:
    bool event(QEvent *e) override;

private:
    void set_content(QString content);
    void set_title(QString title);
    void set_buttonBox(QString accept, QString reject);

    void set_disappearTime(int msec);

private slots:
    void on_closeButton_clicked();

    void on_acceptButton_clicked();

    void on_rejectButton_clicked();

private:
    Ui::NotifyDialog *ui;
    QTimer  *timer;
    int _last_time;
    QPoint _pos;
};

#endif // NOTIFY_DIALOG_H
