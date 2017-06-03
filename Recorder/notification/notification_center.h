#ifndef NOTIFICATION_CENTER_H
#define NOTIFICATION_CENTER_H

#include <QDialog>
#include <QVBoxLayout>

namespace Ui {
class NotificationCenter;
}

class NotificationCenter : public QDialog
{
    Q_OBJECT

public:
    explicit NotificationCenter(QWidget *parent = 0);
    ~NotificationCenter();

    enum NotifyType{
        TextNotify = 0,
        QueryNotify
    };

    QWidget* addNotify(int notify_type, QString content, QString accept = "", QString reject = "");

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void on_closeButton_clicked();

private:
    Ui::NotificationCenter *ui;
    QPoint _position;
    QVBoxLayout* _notify_layout;
};

#endif // NOTIFICATION_CENTER_H
