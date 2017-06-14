#include "CommandModeBase.h"
#include <assert.h>
#include "message_base.h"

CommandModeBase::CommandModeBase(MessageBase *pMessage, const char *pModeName)
    : m_pMessage(pMessage) {
  m_pMessage->AddMode(pModeName, *this);

  //  使用 Qt::QueuedConnection是因为 action_trigger(...)
  //  信号会被消息接收线程发起，而希望在其他的工作线程中执行
  //  on_action_trigger(..)
  connect(this, SIGNAL(action_trigger(QString, bool, QJsonObject)), this,
          SLOT(on_action_trigger(QString, bool, QJsonObject)),
          Qt::QueuedConnection);
}

CommandModeBase::~CommandModeBase() {}

void CommandModeBase::on_action_trigger(QString qstrAction, bool bResult,
                                        QJsonObject jsData) {
  ACTION_PROC_MAP::iterator itrFond = m_mapHandleAction.find(qstrAction);
  if (itrFond != m_mapHandleAction.end()) {
    (this->*itrFond->second)(bResult, jsData);
  }
}

void CommandModeBase::_AddActionProc(const char *pAction,
                                     HandleActionProc fnHandleProc) {
  assert(m_mapHandleAction.find(pAction) == m_mapHandleAction.end());
  m_mapHandleAction.insert(ACTION_PROC_MAP::value_type(pAction, fnHandleProc));
}
