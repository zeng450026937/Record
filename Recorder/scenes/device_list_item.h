#ifndef DEVICE_LIST_ITEM_H
#define DEVICE_LIST_ITEM_H

#include <QWidget>
#include <QVariantMap>

namespace Ui {
class DeviceListItem;
}

class DeviceListItem : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceListItem(QWidget *parent = 0);
    ~DeviceListItem();
    
    void update_display(QVariantMap info);

signals:
    void checkBox_stateChanged(bool checked, QVariantMap device);

private slots:
    void on_checkBox_stateChanged(int arg1);

private:
    Ui::DeviceListItem *ui;
    QVariantMap      _info;
};

#endif // DEVICE_LIST_ITEM_H
