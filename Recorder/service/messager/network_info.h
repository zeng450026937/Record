#ifndef NETWORKINFO_H
#define NETWORKINFO_H

#include <QString>

class NetworkInfo
{
public:
    NetworkInfo();
    ~NetworkInfo() {}

    void updateInfo();

    QString IP() {return _ip_address;}
    QString MAC() {return _mac_address;}
    QString HOSTNAME() {return _local_host;}

protected:
    QString     _ip_address;
    QString     _mac_address;
    QString     _local_host;
};

#endif // NETWORKINFO_H
