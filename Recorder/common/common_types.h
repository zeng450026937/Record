#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <stdio.h>
#include <string>
#include <list>

enum DEVICESTATUS{
    kOnline = 0,
    kOffline = 1,
    kRecording = 2,
    kUnknown = 3
};

enum CONFSTATUS{
    kConfNoDevice = -1,
    kConfReady = 0,
    kConfStarted = 1,
    kConfPaused = 2
};

enum FILESTATUS{
    kNormal =  0,
    kDownloading = 1,
    kDownloaded = 2,
    kDownLoadFail = 3,
    kMissing = 4
};

enum TASKSTATUS{
    kTaskReady = 0,
    kTasking = 1,
    kTaskFail = 2,
    kTaskRemoved= 0,
    kTaskSuccess = 3
};

struct DeviceInst{
    DeviceInst(){
        Reset();
    }
    void Reset(){
        mac = "";
        name = "unknown";
        batteryP = 0;
        batteryS = 0;
        status = kUnknown;
    }
    bool Valid(){
        if(mac.size() > 0 ){
            return true;
        }
        return false;
    }
    void Update(std::string mac_ = "",
                std::string name_ = "",
                int batteryP_ = 0,
                int batteryS_ = 0,
                int status_ = kUnknown){

        if(mac_.size() > 0)
            mac = mac_;
        if(name_.size() > 0)
            name = name_;
        if(batteryP_)
            batteryP = batteryP_;
        if(batteryS_)
            batteryS = batteryS_;
        if(status_ != kUnknown)
            status = status_;
    }
    void CopyFrom(DeviceInst &rhys){
        if(mac != rhys.mac && rhys.mac.size() > 0)
            mac = rhys.mac;
        if(name != rhys.name && rhys.name.size() > 0)
            name = rhys.name;
        if(batteryP != rhys.batteryP && rhys.batteryP != 0)
            batteryP = rhys.batteryP;
        if(batteryS != rhys.batteryS && rhys.batteryS != 0)
            batteryS = rhys.batteryS;
        if(status != rhys.status && rhys.status != kUnknown)
            status = rhys.status;
    }

    std::string   mac;
    std::string   name;
    int           batteryP;
    int           batteryS;
    int           status;//DEVICESTATUS
};

struct MarkInst{
    std::string     fullpath;//file path
    int             seconds;
    std::string     text;
};

struct FileInst{
    std::string     uuid;//conference uuid
    std::string     device;
    std::string     fullpath;
    int             status;
};

struct ConfInst{
    ConfInst(){
        Reset();
    }
    void Reset(){
        uuid = "";
        title = "";
        content = "";
        members = "";
        date = "";
        time = "";
        status = kConfReady;
    }
    bool Valid(){
        if(uuid.size() > 0 &&
             date.size() > 0 &&
                time.size() > 0){
            return true;
        }
        return false;
    }
    void CopyFrom(ConfInst& rhys){
        if(uuid != rhys.uuid && rhys.uuid.size() >0)
            uuid = rhys.uuid;
        if(title != rhys.title && rhys.title.size() >0)
            title = rhys.title;
        if(content != rhys.content && rhys.content.size() >0)
            content = rhys.content;
        if(members != rhys.members && rhys.members.size() >0)
            members = rhys.members;
        if(date != rhys.date && rhys.date.size() >0)
            date = rhys.date;
        if(time != rhys.time && rhys.time.size() >0)
            time = rhys.time;
        if(status != rhys.status)
            status = rhys.status;
    }
    std::string GetFileName(){
        return date+" "+time+" "+(title.size()>0?title:"Default");
    }

    std::string     uuid;//conference uuid
    std::string     title;
    std::string     content;
    std::string     members;
    std::string     date;
    std::string     time;
    int             status;//CONFSTATUS
};

struct TaskInst{
    TaskInst(){
        Reset();
    }

    void Reset(){
        uuid = "";
        sender = "";
        folder = "";
        filename = "";
        seconds = 0;
        status = kTaskReady;
    }

    bool Valid(){
        if( uuid.size() != 0 &&
            sender.size() != 0 &&
            folder.size() != 0 &&
            filename.size() != 0
          )
            return true;
        else
            return false;
    }

    bool operator==(TaskInst& rhys){
        if(uuid == rhys.uuid &&
            sender == rhys.sender &&
            folder == rhys.folder &&
            filename == rhys.filename
          )
            return true;
        else
            return false;
    }

    std::string GetFileName(){
        return folder+"/"+filename+".amr";
    }

    std::string     uuid;
    std::string     sender;
    std::string     folder;
    std::string     filename;
    int             seconds;
    int             status;
};

typedef std::lsRecordInfoes<DeviceInst> DeviceList;
typedef std::lsRecordInfoes<ConfInst> ConferenceList;
typedef std::lsRecordInfoes<FileInst> FileList;
typedef std::lsRecordInfoes<MarkInst> MarkList;
typedef std::lsRecordInfoes<TaskInst> TaskList;

#endif // COMMON_TYPES_H
