#pragma once

#include <QJsonObject>
#include <QByteArray>
#include <QObject>

#define MB_INFO_MODE "info"
#define MB_PERSONAL_MODE "personal"
#define MB_CONFERENCE_MODE "conference"

class MessageBase;
class CommandBase : public QObject 
{
    friend class MessageBase;

  Q_OBJECT
 public:
  CommandBase(MessageBase *pMessage);
  virtual ~CommandBase() = 0;

 signals:
  void action_trigger(QByteArray, bool, QJsonObject);
  void binary_received(QByteArray binary);

 protected:
  typedef void (CommandBase::*HandleActionProc)(bool, const QJsonObject &);

#define AddActionProc(mode,ac, proc) _AddActionProc((mode),(ac), (HandleActionProc)(proc))
  void _AddActionProc(const char *pModeName, const char *pAction, HandleActionProc fnHandleProc);

 private slots:
  void on_action_trigger(QByteArray qstrTriggerId, bool bResult, QJsonObject jsData);
  virtual void on_binary_received(QByteArray binary);

 protected:

  MessageBase *m_pMessage;
 private:
  typedef std::map<QByteArray, HandleActionProc> ACTION_PROC_MAP;
  ACTION_PROC_MAP m_mapHandleAction;
};
