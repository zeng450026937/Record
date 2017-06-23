#include "user_service_impl.h"
#include "account/account_center.h"
#include "common/config.h"
#include "messager/message_base.h"
#include "service_thread.h"
#include "service_thread_private.h"
#include "white_list.h"

#include "../command/LoginControl.h"
#include "storage/include/user_database.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimerEvent>

UserServiceImpl::UserServiceImpl(ServiceThread* pService, QObject* parent)
    : QObject(parent),
      _service(pService),
      _private(pService->_private),
      _warning(true),
      _connect_monitor(0) {
  _message = _private->Messager();
  _account = _private->Account();

  QObject::connect(_message, SIGNAL(connection_status(int)), this,
                   SLOT(on_connection_status(int)));
  QObject::connect(_account, SIGNAL(loginResult(bool, const QString&)), this,
                   SLOT(logined(bool, const QString&)), Qt::DirectConnection);
}

void UserServiceImpl::stopConnection() {
  _message->stopConnection();
  _uri.clear();
}

void UserServiceImpl::userLogin(QString account, QString password) {
  _account->UserLogin(account, password);
}

void UserServiceImpl::on_connection_status(int iStatus) {
  switch (iStatus) {
    // common command
    case MessageBase::CS_OPENED:
      emit loginResult(QString());
      this->killTimer(_connect_monitor);
      _connect_monitor = 0;
      _warning = true;
      break;
    case MessageBase::CS_FAILED:
      if (_warning) {
        emit loginResult(QObject::tr("连接服务失败"));
        this->killTimer(_connect_monitor);
        _connect_monitor = 0;
        _warning = false;
      }
      _message->stopConnection();
      break;
    case MessageBase::CS_CLOSED:
      if (_warning) {
        emit loginResult(QObject::tr("服务器断开连接,程序进行自动重连"));
        _warning = false;
      }
      _message->stopConnection();
      DoWebsocketConnect();
      break;
  }
}

void UserServiceImpl::logined(bool ok, const QString& reason) {
  if (ok) {
    DoWebsocketConnect();
    if (_connect_monitor) {
      this->killTimer(_connect_monitor);
      _connect_monitor = 0;
    }
    _connect_monitor = this->startTimer(10000);
  } else {
    emit loginResult(reason);
  }
}

void UserServiceImpl::timerEvent(QTimerEvent* e) {
  if (_connect_monitor = e->timerId()) {
    emit loginResult(QObject::tr("服务器无响应"));

    this->killTimer(_connect_monitor);
    _connect_monitor = 0;
  }
}

void UserServiceImpl::DoWebsocketConnect() {
  _service->GetLoginControl()->ConnectToServer();
}
