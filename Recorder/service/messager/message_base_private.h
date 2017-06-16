#ifndef MESSAGE_BASE_PRIVATE_H
#define MESSAGE_BASE_PRIVATE_H

#include "message_base.h"

#include <QMap>
#include <map>
#include <QList>

class CommandBase;
class NetworkInfo;
class WebSocketClient;

class MessageBasePrivate
{
public:
    MessageBasePrivate();
    ~MessageBasePrivate();

    NetworkInfo*        _info;
    WebSocketClient*    _client;

    QMap<QString, MessageBase::CommandList> _command_map;

	std::map<QByteArray, CommandBase *> mapMode;

    QByteArray  baActionTriger;
};

#endif // MESSAGE_BASE_PRIVATE_H
