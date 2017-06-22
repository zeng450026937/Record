#pragma once

#include <QString>
#include <service/messager/CommandBase.h>

class MarkDatabase;
class MessageBase;
class RecorderShared;
class MarkControl : public CommandBase {
    friend class ServiceThread;

    MarkControl(MarkDatabase *pDB, MessageBase *pMessager);
public:
    ~MarkControl();

    void AddMarkInfo(const QString &qstrConferenceUuid,
        const QString &qstrContent,
        int iTime);

    void SetMarkInfo(const QString &qstrMarkUuid,
                    const QString &qstrContent,
                    int iTime);

    void DeleteConferenceMark(const QString &qstrConferenceUuid,
                        int iIndex);


    void GetConferenceMark(const QString &qstrConferenceUuid);

    void GetAllConferenceMarkList();
protected:
    void AddMarkInfoReplay(bool bResult, const QJsonObject &jsData);



    void SetMarkInfoReplay(bool bResult, const QJsonObject &jsData);

    void DeleteConferenceMarkReplay(bool bResult, const QJsonObject &jsData);


    void GetConferenceMarkReplay(bool bResult, const QJsonObject &jsData);

    void GetAllConferenceMarkListReplay(bool bResult, const QJsonObject &jsData);

private:
    MarkDatabase *m_pMarkDB;
    RecorderShared *m_pRecordShared;
};
