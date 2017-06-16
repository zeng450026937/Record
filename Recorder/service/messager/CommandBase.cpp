#include "CommandBase.h"
#include <assert.h>
#include "message_base.h"

CommandBase::CommandBase(MessageBase *pMessage)
    : m_pMessage(pMessage) {

  //  使用 Qt::QueuedConnection是因�?action_trigger(...)
  //  信号会被消息接收线程发起，而希望在其他的工作线程中执行
  //  on_action_trigger(..)
  connect(this, SIGNAL(action_trigger(QByteArray, bool, QJsonObject)), 
      this, SLOT(on_action_trigger(QByteArray, bool, QJsonObject)),
      Qt::QueuedConnection);

  connect(this, SIGNAL(binary_received(QByteArray)), 
      this, SLOT(on_binary_received(QByteArray)),
      Qt::QueuedConnection);
}

CommandBase::~CommandBase() {}

void CommandBase::on_action_trigger(QByteArray qstrTriggerId, bool bResult, QJsonObject jsData) 
{

  ACTION_PROC_MAP::iterator itrFond = m_mapHandleAction.find(qstrTriggerId);
  if (itrFond != m_mapHandleAction.end())
  {
    (this->*itrFond->second)(bResult, jsData);
  }
}

void CommandBase::on_binary_received(QByteArray binary)
{
  // on_binary_received() is not impled
}

void CommandBase::_AddActionProc(const char *pModeName,
                                    const char *pAction, 
                                    HandleActionProc fnHandleProc) 
{
    QByteArray qbaLocate(pModeName);
    qbaLocate += pAction;
    m_pMessage->AddActionHandleObject(qbaLocate, *this);

    assert(m_mapHandleAction.find(qbaLocate) == m_mapHandleAction.end());
    m_mapHandleAction.insert(ACTION_PROC_MAP::value_type(qbaLocate, fnHandleProc));
}
