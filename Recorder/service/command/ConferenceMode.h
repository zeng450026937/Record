#pragma once

#include <QMap>
#include <service/messager/CommandBase.h>

class Config;
class MessageBase;
class RecorderShared;
class ConferenceMode : public CommandBase
{
    friend class ServiceThread;

    Q_OBJECT

    ConferenceMode(MessageBase *pMessage);
public:
    ~ConferenceMode();


public:
    void GetConferenceList();

private:
//     struct DownLoadInfo {
//         int type;
//         QString uuid;
//         QString identity;
//         int sample_rate;
//         int channal;
// 
//         bool DownLoadInfo::operator==(const DownLoadInfo &rhs) {            
//             return (rhs.uuid == uuid && rhs.identity == identity);
//         }
//     };

private:
    void GetConferenceListReply(bool bResult, QJsonObject jsData);


    RecorderShared      *m_pRecrodShared;
    Config              *m_pConfig;
};

