#ifndef DEVICE_DATABASE_IMPL_H
#define DEVICE_DATABASE_IMPL_H

#include "include/device_database.h"

class SharedData;

class DeviceDatabase_Impl : public DeviceDatabase
{
public:
    int AddDevice(QVariantMap& device) override;
    QVariantMap DeviceInfo(QString& mac) override;
    QVariantMap DeviceInfo(int row) override;

    QVariantList DeviceList() override;

    int DeviceCount() override;
    int DeviceCount(QString mac) override;

    int DelDevice(QString& mac) override;
    int DelDevice(int row) override;

    int DeviceRow(QString mac) override;

    int SetDeviceSelected(int row, int selected) override;
    int SetDeviceSelected(QString& mac, int selected) override;
    int DeviceSelected(QString& mac) override;

    int SetDeviceUsed(int row, int used) override;
    int SetDeviceUsed(QString& mac, int used) override;
    int DeviceUsed(QString& mac) override;

    int SetDeviceName(int row, QString& name) override;
    int SetDeviceName(QString& mac, QString& name) override;
    QString DeviceName(QString& mac) override;

    int SetDeviceBatteryP(int row, int battery) override;
    int SetDeviceBatteryP(QString& mac, int battery) override;
    int DeviceBatteryP(QString& mac) override;

    int SetDeviceBatteryS(int row, int battery_time) override;
    int SetDeviceBatteryS(QString& mac, int battery_time) override;
    int DeviceBatteryS(QString& mac) override;

    int SetDeviceStatus(int row, QString status) override;
    int SetDeviceStatus(QString mac, QString status) override;
    QString DeviceStatus(QString mac) override;

    int SetDeviceAccount(int row, QString account) override;
    int SetDeviceAccount(QString mac, QString account) override;
    QString DeviceAccount(QString mac) override;

protected:
    DeviceDatabase_Impl(SharedData* shared):_shared(NULL){ _shared = shared; }
    ~DeviceDatabase_Impl() override {}

private:
    SharedData* _shared;
};

#endif // DEVICE_DATABASE_IMPL_H
