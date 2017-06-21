#pragma once

#include <QString>
#include "../messager/CommandBase.h"

class RecorderShared;
class MessageBase;
class RecordControl : public CommandBase
{
public:
	RecordControl(MessageBase *pMessager);
	~RecordControl();


    void GetDeviceList();
    void GetTemplateList();
protected:
    
private:
    void GetDeviceListReply(bool bResult, const QJsonObject &jsData);
    void GetTemplateListReply(bool bResult, const QJsonObject &jsData);


    RecorderShared      *m_pRecrodShared;
};

