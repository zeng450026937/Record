#pragma once

#include <QJsonObject>
#include <QObject>

class MessageBase;
class CommandModeBase : public QObject {
  Q_OBJECT
 public:
  CommandModeBase(MessageBase *pMessage, const char *pModeName);
  virtual ~CommandModeBase() = 0;

 signals:
  void action_trigger(QString, bool, QJsonObject);
  void binary_received(QByteArray binary);

 protected:
  typedef void (CommandModeBase::*HandleActionProc)(bool, const QJsonObject &);

#define AddActionProc(ac, proc) _AddActionProc((ac), (HandleActionProc)(proc))
  void _AddActionProc(const char *pAction, HandleActionProc fnHandleProc);

 private slots:
  void on_action_trigger(QString qstrAction, bool bResult, QJsonObject jsData);
  virtual void on_binary_received(QByteArray binary);

 protected:
  virtual void SendAction(const char *pAction, const QJsonObject &jsData) = 0;

  MessageBase *m_pMessage;

 private:
  typedef std::map<QString, HandleActionProc> ACTION_PROC_MAP;
  ACTION_PROC_MAP m_mapHandleAction;
};
