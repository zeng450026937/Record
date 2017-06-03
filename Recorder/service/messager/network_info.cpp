#include "network_info.h"

#include <QtNetwork/QNetworkInterface>
#include <QtNetwork/QHostInfo>

NetworkInfo::NetworkInfo()
{
    this->updateInfo();
}

void NetworkInfo::updateInfo()
{
    QList<QNetworkInterface> list = QNetworkInterface::allInterfaces();
    QHostAddress address2;

    for(int i = 0;i<list.count();i++)
    {
        QString deviceName = list.at(i).name();
        if(!deviceName.startsWith("ethernet"))
            continue;
        qDebug() <<"Device:"<<list.at(i).humanReadableName();

        QList<QNetworkAddressEntry> entryList = list.at(i).addressEntries();

        for(int j = 0;j<entryList.count();j++)
        {
            address2=entryList.at(j).ip();

            if(address2.isLoopback()){
                continue;
            }
            if(address2.protocol() == QAbstractSocket::IPv4Protocol)
            {

               qDebug()<<"MAC:"<<list.at(i).hardwareAddress();
               qDebug()<<"IPV4 Address:"<<entryList.at(j).ip().toString();
               qDebug()<<"Netmask: "  <<entryList.at(j).netmask().toString();
               qDebug()<<"Broadcast:" <<entryList.at(j).broadcast().toString();

               _ip_address = entryList.at(j).ip().toString();
               _mac_address = list.at(i).hardwareAddress();
            }
        }
        break;
    }

    _local_host = QHostInfo::localHostName();
}
