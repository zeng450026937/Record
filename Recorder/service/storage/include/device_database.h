#ifndef DEVICE_DATABASE_H
#define DEVICE_DATABASE_H

#include <QVariant>

class DataBase;

class DeviceDatabase {
public:
    static DeviceDatabase* GetInterface(DataBase* db);
    virtual int Release() = 0;

    virtual int AddDevice(QVariantMap& device) = 0;
    virtual QVariantMap DeviceInfo(QString& mac) = 0;
    virtual QVariantMap DeviceInfo(int row) = 0;

    virtual QVariantList DeviceList() = 0;

    virtual int DeviceCount() = 0;
    virtual int DeviceCount(QString mac) = 0;

    virtual int DelDevice(QString& mac) = 0;
    virtual int DelDevice(int row) = 0;

    virtual int DeviceRow(QString mac) = 0;

    virtual int SetDeviceSelected(int row, int selected) = 0;
    virtual int SetDeviceSelected(QString& mac, int selected) = 0;
    virtual int DeviceSelected(QString& mac) = 0;

    virtual int SetDeviceUsed(int row, int used) = 0;
    virtual int SetDeviceUsed(QString& mac, int used) = 0;
    virtual int DeviceUsed(QString& mac) = 0;

    virtual int SetDeviceName(int row, QString& name) = 0;
    virtual int SetDeviceName(QString& mac, QString& name) = 0;
    virtual QString DeviceName(QString& mac) = 0;

    virtual int SetDeviceBatteryP(int row, int battery) = 0;
    virtual int SetDeviceBatteryP(QString& mac, int battery) = 0;
    virtual int DeviceBatteryP(QString& mac) = 0;

    virtual int SetDeviceBatteryS(int row, int battery_time) = 0;
    virtual int SetDeviceBatteryS(QString& mac, int battery_time) = 0;
    virtual int DeviceBatteryS(QString& mac) = 0;

    virtual int SetDeviceStatus(int row, QString status) = 0;
    virtual int SetDeviceStatus(QString mac, QString status) = 0;
    virtual QString DeviceStatus(QString mac) = 0;

    virtual int SetDeviceAccount(int row, QString account) = 0;
    virtual int SetDeviceAccount(QString mac, QString account) = 0;
    virtual QString DeviceAccount(QString mac) = 0;

protected:
    DeviceDatabase(){}
    virtual ~DeviceDatabase(){}
};

#endif // DEVICE_DATABASE_H
