#ifndef MESSAGE_BASE_PRIVATE_H
#define MESSAGE_BASE_PRIVATE_H

#include "message_base.h"

#include <QMap>
#include <map>
#include <QList>

class CommandModeBase;
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

	std::map<QString, CommandModeBase *> mapMode;
};

#endif // MESSAGE_BASE_PRIVATE_H
