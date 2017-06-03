#include "message_base_private.h"

#include "websocketclient.h"
#include "network_info.h"

MessageBasePrivate::MessageBasePrivate():
    _info(new NetworkInfo),
    _client(new WebSocketClient)
{
    _command_map.insert("CreateConference",MessageBase::CreateConference);
    _command_map.insert("StartConference",MessageBase::StartConference);
    _command_map.insert("PauseConference",MessageBase::PauseConference);
    _command_map.insert("StopConference",MessageBase::StopConference);
    _command_map.insert("SetConferenceInfo",MessageBase::SetConferenceInfo);
    _command_map.insert("DeleteConferenceInfo",MessageBase::DelConferenceInfo);
    _command_map.insert("GetConferenceInfo",MessageBase::GetConferenceInfo);
    _command_map.insert("GetConferenceList",MessageBase::GetConferenceList);

    _command_map.insert("NotifyPersonRecordAdd",MessageBase::NotifyPersonRecordAdd);
    _command_map.insert("NotifyPersonRecordUpdate",MessageBase::NotifyPersonRecordUpdate);
    _command_map.insert("NotifyPersonRecordDelete",MessageBase::NotifyPersonRecordDelete);
    _command_map.insert("GetPersonalList",MessageBase::GetPersonalList); 
    _command_map.insert("GetAllPersonalList",MessageBase::GetAllPersonalList);

    _command_map.insert("AddDevice",MessageBase::AddDevice);
    _command_map.insert("RemoveDevice",MessageBase::RemoveDevice);
    _command_map.insert("GetDeviceList",MessageBase::GetDeviceList);

    _command_map.insert("AddTemplateInfo",MessageBase::AddTemplateInfo);
    _command_map.insert("SetTemplateInfo",MessageBase::SetTemplateInfo);
    _command_map.insert("DeleteTemplateInfo",MessageBase::DelTemplateInfo);
    _command_map.insert("GetTemplateInfo",MessageBase::GetTemplateInfo);
    _command_map.insert("GetTemplateList",MessageBase::GetTemplateList);

    _command_map.insert("AddMarkInfo",MessageBase::AddMarkInfo);
    _command_map.insert("SetMarkInfo",MessageBase::SetMarkInfo);
    _command_map.insert("DelMarkInfo",MessageBase::DelMarkInfo);
    _command_map.insert("GetMarkinfo",MessageBase::GetMarkinfo);
    _command_map.insert("GetMarkList",MessageBase::GetMarkList);

    _command_map.insert("DownloadFile",MessageBase::DownloadFile);
    _command_map.insert("DownloadFileAck",MessageBase::DownloadFileAck);
    _command_map.insert("NotifyNewDataRecv",MessageBase::NotifyNewDataRecv);
    _command_map.insert("LoginDevice",MessageBase::LoginDevice);
    _command_map.insert("HeartBeat",MessageBase::HeartBeat);
    _command_map.insert("NotifyDeviceInfoChange",MessageBase::NotifyDeviceInfoChange);
    _command_map.insert("DeviceEnvironmentException",MessageBase::DeviceEnvironmentException);
}

MessageBasePrivate::~MessageBasePrivate(){
    delete _info;
    delete _client;
}
