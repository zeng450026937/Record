#pragma once

#include <QVariantList>
#include <service/messager/CommandModeBase.h>


class Config;
class ConfServiceImpl;
class MessageBase;
class RecorderShared;
class PersonalMode : public CommandModeBase
{
    friend class ServiceThread;

    Q_OBJECT

    PersonalMode(MessageBase *pMessage);
public:
    ~PersonalMode();


    void GetPersonalList();
    void GetAllPersoanlList();

private:
    void GetPersonalListReply(bool bResult, QJsonObject jsData); 

private:

    RecorderShared      *m_pRecordShared;
    Config              *m_pConfig;
    ConfServiceImpl     *m_pConfService;

};

