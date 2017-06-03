#ifndef DEVICE_LIST_ITEM_A_H
#define DEVICE_LIST_ITEM_A_H

#include <QWidget>
#include <QVariantMap>

namespace Ui {
class DeviceListItemA;
}

class DeviceListItemA : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceListItemA(QWidget *parent = 0);
    ~DeviceListItemA();

    void update_display(QVariantMap info);

signals:
    void checkBox_stateChanged(bool checked, QVariantMap device);

private slots:
    void on_checkBox_stateChanged(int arg1);

private:
    Ui::DeviceListItemA *ui;
    QVariantMap      _info;
};


#endif // DEVICE_LIST_ITEM_A_H
