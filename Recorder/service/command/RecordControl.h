#pragma once

#include <QString>
#include <QVariantMap>
#include "../messager/CommandBase.h"

class RecorderShared;
class MessageBase;
class RecordControl : public CommandBase
{
public:
	RecordControl(RecorderShared *pRecorderShare,MessageBase *pMessager);
	~RecordControl();


    void GetDeviceList();
    void GetTemplateList();

    void CreateConference(const QVariantMap &vmCreateInfo);
    void StartConference(const QString &qstrConferenceUuid);
    void PauseConference(const QString &qstrConferenceUuid);
    void StopConference(const QString &qstrConferenceUuid);
protected:
    
private:
    void GetDeviceListReply(bool bResult, const QJsonObject &jsData);
    void GetTemplateListReply(bool bResult, const QJsonObject &jsData);

    void UpdateDeviceInfo(bool bResult, const QJsonObject &jsData);

    void CreateConferenceReply(bool bResult, const QJsonObject &jsData);
    void StartConferenceReply(bool bResult, const QJsonObject &jsData);
    void PauseConferenceReply(bool bResult, const QJsonObject &jsData);
    void StopConferenceReply(bool bResult, const QJsonObject &jsData);


    RecorderShared      *m_pRecordShared;
};

