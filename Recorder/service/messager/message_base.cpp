
#include "message_base.h"
#include <assert.h>
#include <QByteArray>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include "CommandBase.h"
#include "common/config.h"
#include "message_base_private.h"
#include "network_info.h"
#include "websocketclient.h"

MessageBase::MessageBase(QObject *parent)
    : QObject(parent), d(new MessageBasePrivate) {
  qRegisterMetaType<QByteArray>();
  connect(d->_client, SIGNAL(binary_message(QByteArray)), this,
          SIGNAL(notify_binary(QByteArray)), Qt::QueuedConnection);
  connect(d->_client, SIGNAL(text_message(QString)), this,
          SLOT(on_message_reply(QString)), Qt::QueuedConnection);
  bool bRes = connect(d->_client, SIGNAL(connection_status(int)), this,
                      SIGNAL(connection_status(int)));
}

MessageBase::~MessageBase() { delete d; }

bool MessageBase::sendMessage(const QString &qstrMode,
                              const QString &qstrAction,
                              const QJsonObject &jsData) {
  QJsonObject jsRoot;
  jsRoot.insert("version", MB_MESSAGE_VERSION);
  jsRoot.insert("authorization", "");
  jsRoot.insert("from", Config::GetInstance()->GetUser().user_id);
  jsRoot.insert("to", "");

  QJsonObject jsCommand;
  jsCommand.insert("mode", qstrMode);
  jsCommand.insert("action", qstrAction);

  jsRoot.insert("command", jsCommand);
  jsRoot.insert("data", jsData);

  QJsonDocument jsonDocument(jsRoot);
  jsonDocument.setObject(jsRoot);
  d->_client->sendText(
      jsonDocument.toJson(QJsonDocument::Compact).toStdString());

  return true;
}

void MessageBase::on_message_reply(QString qstrMessage) {
  QJsonParseError error;
  QJsonDocument jsonDocument =
      QJsonDocument::fromJson(qstrMessage.toUtf8(), &error);
  if (error.error == QJsonParseError::NoError) {
    QJsonObject jsRoot = jsonDocument.object();
    if (jsRoot["version"].toInt() == MB_MESSAGE_VERSION) {
      QJsonObject jsCommand = jsRoot["command"].toObject();
      QByteArray baActionLocator;
      baActionLocator.append(jsCommand["mode"].toString());
      baActionLocator.append(jsCommand["action"].toString());

      auto itrFound = d->mapMode.find(baActionLocator);
      if (itrFound == d->mapMode.end()) {
        qDebug() << jsRoot << " command mode is not found."
                 << error.errorString();
        return;
      }

      if (jsCommand["action"].toString() != "heartBeat") qDebug() << jsCommand;
      emit itrFound->second->on_action_trigger(baActionLocator,
                                            jsRoot["result"].toBool(),
                                            jsRoot["data"].toObject());
    } else {
      // TODO
    }
  } else {
    qDebug() << "json command parse failed" << error.errorString();
  }
}

void MessageBase::connectTo(const QString &qstrHeader, const QString &qstrUri) {
  d->_client->connect_to(qstrHeader.toStdString(), qstrUri.toStdString());
}

void MessageBase::stopConnection() { d->_client->stop_connect(); }

void MessageBase::sendCommand(CommandList command, QString receiver,
                              const QVariantMap &data) {
  QVariantMap json_command;

  json_command.insert("SENDER", d->_info->MAC());
  json_command.insert("RECEIVER", receiver);
  json_command.insert("COMMAND", d->_command_map.key(command));
  json_command.insert("DATA", data);

  QJsonDocument jsonDocument = QJsonDocument::fromVariant(json_command);

  d->_client->sendText(jsonDocument.toJson().toStdString());
}

void MessageBase::AddActionHandleObject(const QByteArray &qbaLocate,
                                        CommandBase &commandMode) {
  assert(d->mapMode.find(qbaLocate) == d->mapMode.end());
  d->mapMode.insert(
      std::map<QByteArray, CommandBase *>::value_type(qbaLocate, &commandMode));
}
