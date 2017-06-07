#include "conf_service_impl.h"
#include "service_thread_private.h"
#include "messager/message_base.h"
#include "messager/binary_reader.h"
#include "common/config.h"

#include "storage/include/templatedatabase.h"
#include "storage/include/device_database.h"
#include "storage/include/markdatabase.h"
#include "storage/include/conferencedatabase.h"
#include "storage/include/download_database.h"
#include "storage/include/clip_file_database.h"
#include "storage/include/personal_database.h"

#include <QDebug>
#include <QDateTime>
#include <QFile>
#include <QBuffer>
#include <QDir>
#include <QTimerEvent>

ConfServiceImpl::ConfServiceImpl(ServiceThreadPrivate* shared, QObject *parent) :
    _shared(shared),
    ConfService(parent),
    _monitor_timer(0),
    _update_timer(0),
    auto_download(false)
{
    QObject::connect(_shared->Messager(),SIGNAL(notify_command(int,bool,QVariantMap&)),this,SLOT(parseCommand(int,bool,QVariantMap&)),Qt::DirectConnection);
    QObject::connect(_shared->Messager(),SIGNAL(notify_binary(uint,QByteArray&)),this,SLOT(parseBinary(uint,QByteArray&)),Qt::DirectConnection);

    this->setDownloadFolder("./OUT");

    auto_download = Config::GetInstance()->_auto_download;
}

void ConfServiceImpl::timerEvent(QTimerEvent *e)
{
    if(_monitor_timer == e->timerId()){

        if(_monitor_map.count() == 0){

            this->killTimer(_monitor_timer);
            _monitor_timer = 0;
        }else{
            QMap<QString, Monitor*>::iterator iter = _monitor_map.begin();

            while(iter != _monitor_map.end()){
                Monitor* monitor = const_cast<Monitor *>(iter.value());
                if(monitor->alive == _monitor_timer){

                    qDebug()<<"monitering download for:"<<monitor->uuid<<monitor->identity;
                    if(!monitor->completed){

                        if(monitor->idle > 20){
                            qDebug()<<"download is timeout";
                            this->downloadNextFile(monitor->uuid, monitor->identity);

                            QVariantMap conf;

                            if(monitor->type == 1){
                                conf = _shared->ConferenceDB()->ConferenceInfo(monitor->uuid);
                            }
                            else if(monitor->type == 0){
                                conf = _shared->PersonalDB()->ConferenceInfo(monitor->uuid);
                            }

                            conf.insert("completed",0);

                            if(monitor->type == 1){
                                _shared->ConferenceDB()->AddConference(conf);
                            }
                            else if(monitor->type == 0){
                                _shared->PersonalDB()->AddConference(conf);
                            }

                            emit downloadConferenceCompleted(monitor->type, monitor->uuid, monitor->percentage, monitor->speed, false);
                        }
                        else{
                            iter++;
                            monitor->idle++;
                            if(monitor->speed != 0){
                                monitor->speed = 0;
                                emit downloadConferenceCompleted(monitor->type, monitor->uuid, monitor->percentage, monitor->speed, 2);
                            }
                            //qDebug()<<"downloading idle:"<<monitor->idle;
                            continue;
                        }
                    }else{
                        qDebug()<<"download is completed";
                        emit downloadConferenceCompleted(monitor->type, monitor->uuid, monitor->percentage, monitor->speed, monitor->completed);
                    }

                    iter = _monitor_map.erase(iter);
                    delete monitor;
                    monitor = nullptr;
                }else{
                    //qDebug()<<"download for:"<<monitor->uuid<<monitor->identity<<"is alive";
                    monitor->alive = _monitor_timer;
                    emit downloadConferenceCompleted(monitor->type, monitor->uuid, monitor->percentage, monitor->speed, 2);
                    monitor->idle = 0;
                    iter++;
                }
            }
        }
    }
}

void ConfServiceImpl::createConference(QVariantMap info)
{
    this->Execute(MessageBase::CreateConference, info);
}
void ConfServiceImpl::startConference(QString uuid)
{
    QVariantMap param;
    param.insert("uuid",uuid);

    this->Execute(MessageBase::StartConference, param);
}
void ConfServiceImpl::pauseConference(QString uuid)
{
    QVariantMap param;
    param.insert("uuid",uuid);

    this->Execute(MessageBase::PauseConference, param);
}
void ConfServiceImpl::stopConference(QString uuid)
{
    QVariantMap param;
    param.insert("uuid",uuid);

    this->Execute(MessageBase::StopConference, param);
}

void ConfServiceImpl::setConferenceInfo(QString uuid, QVariantMap info)
{
    QVariantMap param = info;
    param.insert("uuid",uuid);

    this->Execute(MessageBase::SetConferenceInfo, param);
}
void ConfServiceImpl::delConferenceInfo(QString uuid)
{
    QVariantMap param;
    param.insert("uuid",uuid);

    this->Execute(MessageBase::DelConferenceInfo, param);
}
void ConfServiceImpl::getConferenceInfo(QString uuid)
{
    QVariantMap param;
    param.insert("uuid",uuid);

    this->Execute(MessageBase::GetConferenceInfo, param);
}
QVariantMap ConfServiceImpl::conferenceInfo(QString uuid) const
{
    return _shared->ConferenceDB()->ConferenceInfo(uuid);
}
void ConfServiceImpl::getConferenceList()
{
    QVariantMap param;

    this->Execute(MessageBase::GetConferenceList, param);
}
void ConfServiceImpl::getPersonalList(QString user_id)
{
    QVariantMap param;
    param.insert("user_id", user_id);

    this->Execute(MessageBase::GetPersonalList, param);
}
void ConfServiceImpl::getAllPersonalList()
{
	_shared->Messager()->sendMessage("getAllPersonalList", "personal");
}
QVariantList ConfServiceImpl::conferenceList() const
{
    return _shared->ConferenceDB()->ConfList();
}
QVariantMap ConfServiceImpl::personalConfInfo(QString uuid) const
{
    return _shared->PersonalDB()->ConferenceInfo(uuid);
}
QVariantList ConfServiceImpl::personalConfList() const
{
    return _shared->PersonalDB()->ConfList();
}

void ConfServiceImpl::addTemplateInfo(QVariantMap info)
{
    this->Execute(MessageBase::AddTemplateInfo, info);
}

void ConfServiceImpl::setTemplateInfo(QVariantMap info)
{
    this->Execute(MessageBase::SetTemplateInfo, info);
}
void ConfServiceImpl::getTemplateInfo(QString uuid)
{
    QVariantMap param;
    param.insert("uuid",uuid);

    this->Execute(MessageBase::GetTemplateInfo, param);
}
void ConfServiceImpl::delTemplateInfo(QString uuid)
{
    QVariantMap param;
    param.insert("uuid",uuid);

    this->Execute(MessageBase::DelTemplateInfo, param);
}
QVariantMap ConfServiceImpl::templateInfo(QString uuid) const
{
    return _shared->TemplateDB()->TemplateInfo(uuid);
}
void ConfServiceImpl::getTemplateList()
{
    QVariantMap param;

    this->Execute(MessageBase::GetTemplateList, param);
}
QVariantList ConfServiceImpl::templateList() const
{
    return _shared->TemplateDB()->TemplateList();
}

void ConfServiceImpl::addMarkInfo(QString uuid, QVariantMap mark)
{
    QVariantMap param = mark;
    param.insert("conference_uuid",uuid);
    param.insert("content",mark.value("mark").toString());

    this->Execute(MessageBase::AddMarkInfo, param);
}
void ConfServiceImpl::setMarkInfo(QString uuid, QVariantMap mark)
{
    QVariantMap param = mark;
    param.insert("conference_uuid",uuid);

    this->Execute(MessageBase::SetMarkInfo, param);
}
void ConfServiceImpl::delMarkInfo(QString uuid, QVariantMap mark)
{
    QVariantMap param = mark;
    param.insert("conference_uuid",uuid);

    this->Execute(MessageBase::DelMarkInfo, param);
}
void ConfServiceImpl::getMarkInfo(QString uuid)
{
    QVariantMap param;
    param.insert("conference_uuid",uuid);

    this->Execute(MessageBase::GetMarkinfo, param);
}
QVariantList ConfServiceImpl::markInfo(QString uuid) const
{
    return _shared->MarkDB()->GetMark(uuid);
}

QVariantList ConfServiceImpl::deviceInfoList() const
{
    return _shared->DeviceDB()->DeviceList();
}

QVariantMap ConfServiceImpl::deviceInfo(QString mac) const
{
    return _shared->DeviceDB()->DeviceInfo(mac);
}
QVariantList ConfServiceImpl::fileList(QString uuid) const
{
    return _shared->ClipDB()->GetFileList(uuid);
}
int ConfServiceImpl::addClipFile(QVariantMap& file)
{
    QString uuid;
    QString identity;
    QString path;
    int start_time(0);

    uuid = file.value("uuid").toString();
    identity = file.value("identity").toString();
    path = file.value("path").toString();
    start_time = file.value("start_time").toInt();

    return _shared->ClipDB()->AddFile(uuid,identity,start_time,path);
}

void ConfServiceImpl::getMarkList()
{
    QVariantMap param;

    this->Execute(MessageBase::GetMarkList, param);
}

void ConfServiceImpl::getDeviceInfoList()
{
    QVariantMap param;

    this->Execute(MessageBase::GetDeviceList, param);
}

void ConfServiceImpl::downloadConference(int type, QString uuid)
{
    QStringList exists;
    QStringList missing;
    QStringList needed;

    //needed total file
    if(type == 1){
        needed = _shared->ConferenceDB()->ConferenceInfo(uuid).value("devices").toString().split(",");
    }
    else if(type == 0){
        needed << _shared->PersonalDB()->ConferenceInfo(uuid).value("user_id").toString();
    }

    needed.removeDuplicates();

    //check exists from file database
    this->checkConferenceFile(uuid, exists, missing);
    //check exists from download buffer
    exists += _shared->DownloadDB()->GetCompletedIdentity(type, uuid);

    exists.removeDuplicates();

    qDebug()<<"download conference for:"<<uuid;
    qDebug()<<"needed list:"<<needed;
    qDebug()<<"exist file:"<<exists;

    foreach (QString item, needed) {
        if(!exists.contains(item)){
            //this->downloadFile(type, uuid, item);
            this->queryBinary(type, uuid, item);
        }
    }
}

void ConfServiceImpl::setDownloadFolder(QString folder)
{
    QDir dir;
    dir.setPath(folder);
    dir.mkpath(dir.absolutePath());

    _folder = dir.absolutePath();
}

void ConfServiceImpl::checkConference(QVariantMap& conf)
{
    QString uuid = conf.value("uuid").toString();
    QStringList needed_list = conf.value("devices").toString().split(",");
    needed_list.removeDuplicates();

    bool completed = checkConferenceFile(uuid, needed_list);

    conf.insert("completed",completed);
    _shared->ConferenceDB()->AddConference(conf);

    if(!completed && auto_download){
        this->downloadConference(1, uuid);
    }
}
void ConfServiceImpl::checkPersonal(QVariantMap& conf)
{
    QString uuid = conf.value("conference_uuid").toString();
    QStringList needed_list;
    needed_list << conf.value("user_id").toString();
    needed_list.removeDuplicates();

    bool completed = checkConferenceFile(uuid, needed_list);

    conf.insert("completed",completed);
    _shared->PersonalDB()->AddConference(conf);

    if(!completed && auto_download){
        this->downloadConference(0, uuid);
    }
}

QString ConfServiceImpl::downloadFolder() const
{
    return _folder;
}

QString ConfServiceImpl::outputFolder(int type,QString uuid) const
{
    QVariantMap conf;
    if(type == 1){
        conf = _shared->ConferenceDB()->ConferenceInfo(uuid);
    }
    else if(type == 0){
        conf = _shared->PersonalDB()->ConferenceInfo(uuid);
    }

    QString folderName;

    folderName = conf.value("create_time").toString();
    folderName.replace(":","");
    folderName.replace("-","");
    folderName.replace(" ","_");
    folderName += "_";
    if(type == 0)
        folderName += conf.value("user_name").toString();
    folderName += conf.value("title").toString();

    return _folder+"/"+folderName;
}

void ConfServiceImpl::parseCommand(int command, bool result, QVariantMap& info)
{
    switch(command){
    //common command
    case MessageBase::CreateConference:
        if(result){
            _shared->ConferenceDB()->AddConference(info);
        }
        emit conferenceCreated(result, info);
        break;
    case MessageBase::StartConference:
        emit conferenceStarted(result, info);
        break;
    case MessageBase::PauseConference:
        emit conferencePaused(result, info);
        break;
    case MessageBase::StopConference:
        emit conferenceStoped(result, info);
        break;
    case MessageBase::SetConferenceInfo:
        if(result)
            _shared->ConferenceDB()->AddConference(info);
        emit conferenceInfoSetted(result, info);
        break;
    case MessageBase::DelConferenceInfo:
        if(result)
            _shared->ConferenceDB()->DelConference(info.value("uuid").toString());
        emit conferenceInfoDeleted(result, info);
        break;
    case MessageBase::GetConferenceInfo:
        if(result)
            _shared->ConferenceDB()->AddConference(info);
        emit conferenceInfoGot(result, info);
        break;
    case MessageBase::GetConferenceList:
        if(result){
            QVariantMap conf_map;
            foreach (QVariant conf, info.value("list").toList()) {
                conf_map = conf.toMap();
                this->checkConference(conf_map);
            }
        }
        emit conferenceListGot(result, info.value("list").toList());
        break;
    case MessageBase::GetPersonalList:
        if(result){
            QVariantMap conf_map;
            foreach (QVariant conf, info.value("list").toList()) {
                conf_map = conf.toMap();
                this->checkPersonal(conf_map);
            }
        }
        emit personalListGot(result, info.value("list").toList());
        break;
    case MessageBase::GetAllPersonalList:
        if(result){
            QVariantMap conf_map;
            foreach (QVariant conf, info.value("list").toList()) {
                conf_map = conf.toMap();
                this->checkPersonal(conf_map);
            }
        }
        emit allPersonalListGot(result, info.value("list").toList());
        break;

    case MessageBase::SetTemplateInfo:
        if(result)
            _shared->TemplateDB()->AddTemplate(info);
        emit templateInfoSetted(result, info);
        break;
    case MessageBase::AddTemplateInfo:
        if(result)
            _shared->TemplateDB()->AddTemplate(info);
        emit templateInfoAdded(result, info);
        break;
    case MessageBase::DelTemplateInfo:
        if(result)
            _shared->TemplateDB()->DelTemplate(info.value("uuid").toString());
        emit templateInfoDeleted(result, info);
        break;
    case MessageBase::GetTemplateInfo:
        if(result)
            _shared->TemplateDB()->AddTemplate(info);
        emit templateInfoGot(result, info);
        break;
    case MessageBase::GetTemplateList:
        if(result){
            foreach (QVariant Template, info.value("list").toList()) {
                _shared->TemplateDB()->AddTemplate(Template.toMap());
            }
        }
        emit templateListGot(result, info.value("list").toList());
        break;

    case MessageBase::SetMarkInfo:
        if(result)
            _shared->MarkDB()->AddMark(info);
        emit markInfoSetted(result, info);
        break;
    case MessageBase::AddMarkInfo:
        if(result)
            _shared->MarkDB()->AddMark(info);
        emit markInfoAdded(result, info);
        break;
    case MessageBase::DelMarkInfo:
        if(result)
            _shared->MarkDB()->DelMark(info.value("uuid").toString());
        emit markInfoDeleted(result, info);
        break;
    case MessageBase::GetMarkinfo:
        if(result){
            foreach (QVariant mark, info.value("list").toList()) {
                _shared->MarkDB()->AddMark(mark.toMap());
            }
        }
        emit markInfoGot(result, info.value("mark").toList());
        break;
    case MessageBase::GetMarkList:
        if(result){
            foreach (QVariant mark, info.value("list").toList()) {
                _shared->MarkDB()->AddMark(mark.toMap());
            }
        }
        emit markListGot(result, info.value("list").toList());
        break;

    case MessageBase::NotifyDeviceInfoChange:
        _shared->DeviceDB()->AddDevice(info);
        emit deviceInfoUpdate( info );
        break;
    case MessageBase::DeviceEnvironmentException:
        emit deviceEnvironmentUpdate(info);
        break;
    case MessageBase::GetDeviceList:
        if(result){
            foreach (QVariant device, info.value("list").toList()) {
                _shared->DeviceDB()->AddDevice(device.toMap());
            }
        }
        emit deviceInfoListGetted(result, info.value("list").toList());
        break;
    case MessageBase::AddDevice:
        break;
    case MessageBase::RemoveDevice:
        break;

    case MessageBase::DownloadFile:
        if(info.value("result").toInt() != 0){
            QString uuid = info.value("conference_uuid").toString();
            QString identity = info.value("device_mac").toString();
            identity += info.value("user_id").toString();
            int sample_rate = info.value("sample_rate").toInt();
            int channal = info.value("channal").toInt();

            qDebug()<<"downlad file error response:"<<info;

            this->downloadNextFile(uuid, identity);
        }
        else{
            QString uuid = info.value("conference_uuid").toString();
            QString identity = info.value("device_mac").toString();
            identity += info.value("user_id").toString();
            int sample_rate = info.value("sample_rate").toInt();
            int channal = info.value("channal").toInt();

            int total_size = _conf_total_size_map.value(uuid);
            total_size += (info.value("total_size").toInt() - info.value("startpos").toInt());

            _conf_total_size_map.insert(info.value("conference_uuid").toString(),
                                        total_size);

            QList<DownLoadInfo> info_list = _download_info_map.value("uuid");

            DownLoadInfo download_info;
            download_info.uuid = uuid;
            download_info.identity = identity;
            download_info.sample_rate = sample_rate;
            download_info.channal = channal;

            info_list.append(download_info);

            _download_info_map.insert(uuid, info_list);
        }
        break;
    case MessageBase::NotifyNewDataRecv://conference record data
        {
        int int_type(-1);
        QString type = info.value("type").toString();
        QString uuid = info.value("conference_uuid").toString();
        QString identity;
        if(type == "conference"){
            int_type = 1;
            identity = info.value("device_mac").toString();
        }else if(type == "person"){
            int_type = 0;
            identity = info.value("user_id").toString();
        }

        this->queryBinary(int_type, uuid, identity);
        }
        break;

    case MessageBase::NotifyPersonRecordAdd:
        _shared->PersonalDB()->AddConference(info);
        {
        this->queryBinary(0, info.value("conference_uuid").toString(),
                          info.value("user_id").toString());
        }
        emit personalConfCreated(true, info);
        break;
    case MessageBase::NotifyPersonRecordUpdate:
        {
        QString uuid = info.value("conference_uuid").toString();
        int completed = _shared->PersonalDB()->ConferenceInfo(uuid).value("completed").toInt();
        info.insert("completed",completed);
        _shared->PersonalDB()->AddConference(info);
        }
        emit personalConfSetted(true, info);
        break;
    case MessageBase::NotifyPersonRecordDelete:
        _shared->PersonalDB()->DelConference(info.value("conference_uuid").toString());
        //remove download buffer
        _shared->DownloadDB()->RemoveFile(0, info.value("conference_uuid").toString(),
                                          info.value("user_id").toString());
        emit personalConfDeleted(true, info);
        break;
    }
}
void ConfServiceImpl::parseBinary(unsigned int size, QByteArray& content)
{
    Q_UNUSED(size);
    QScopedPointer<BinaryReader> reader;
    reader.reset(new BinaryReader);

    reader->Read(content);

    bool alive = this->checkConferenceAlive(reader->type, reader->file_uuid);
    if(alive){

        _shared->DownloadDB()->AddSegment(reader->type, reader->sender, reader->file_uuid,
                                          reader->meeting_time, reader->file_startpos,
                                          reader->file_status, reader->data_size,
                                          reader->data);

        QVariantMap param;
        if(reader->type == 1){
            param.insert("type","conference");
            param.insert("device_mac",reader->sender);
        }
        else if(reader->type == 0){
            param.insert("type","person");
            param.insert("user_id",reader->sender);
        }
        param.insert("conference_uuid",reader->file_uuid);
        param.insert("startpos",reader->file_startpos);

        this->Execute(MessageBase::DownloadFileAck, param);

        this->monitorDownload(reader->type, reader->file_uuid, reader->sender, reader->data_size);
    }
    else{
        //remove download buffer
        _shared->DownloadDB()->RemoveConf(reader->type, reader->file_uuid);
    }
}

void ConfServiceImpl::Execute(int command, QVariantMap param)
{
    _shared->Messager()->sendCommand((MessageBase::CommandList)command, "", param);
}

void ConfServiceImpl::downloadFile(int type, QString uuid, QString identity)
{
    DownLoadInfo info;
    info.identity = identity;
    info.type = type;
    info.uuid = uuid;

    _download_list.append(info);

    if(_download_list.count() == 1){
        info = _download_list.first();
        this->queryBinary(info.type, info.uuid, info.identity);
    }
}
void ConfServiceImpl::downloadNextFile(QString uuid, QString identity)
{
    DownLoadInfo info;
    info.identity = identity;
    info.uuid = uuid;

    if(!_download_list.isEmpty()){

        if(_download_list.first() == info){

            _download_list.removeFirst();

            if(!_download_list.isEmpty()){
                info = _download_list.first();
                this->queryBinary(info.type, info.uuid, info.identity);
            }
        }
    }
}

void ConfServiceImpl::queryBinary(int type, QString uuid, QString identity)
{
    //qDebug()<<"download file"<<uuid<<identity;

    int binary_size(0);
    binary_size = _shared->DownloadDB()->GetFileSize(type, uuid, identity);

    QVariantMap param;

    if(type == 1){
        param.insert("type","conference");
        param.insert("device_mac",identity);
    }
    else if(type == 0){
        param.insert("type","person");
        param.insert("user_id",identity);
    }
    param.insert("conference_uuid",uuid);
    param.insert("startpos",binary_size);

    this->Execute(MessageBase::DownloadFile, param);
}

void ConfServiceImpl::monitorDownload(int type, QString uuid, QString identity, int data_size)
{
    QStringList completed_list = _shared->DownloadDB()->GetCompletedIdentity(type, uuid);

    //once a file download is compeleted, save it
    if(completed_list.count() > 0){

            QDir dir;
            dir.setPath(this->outputFolder(type, uuid));
            dir.mkpath(dir.absolutePath());

            QString fileName;
            QScopedPointer<QFile> file(new QFile);
            foreach (QString user, completed_list) {

                fileName.clear();
                fileName = _shared->DeviceDB()->DeviceInfo(user).value("name").toString();
                if(fileName.isEmpty())
                    fileName += _shared->ConferenceDB()->ConferenceInfo(uuid).value("title").toString();
                if(fileName.isEmpty())
                    fileName += user;
                fileName.replace(":","");
                fileName.replace("-","");
                fileName.replace(" ","_");

                QByteArray record_data = _shared->DownloadDB()->GetFile(type, uuid, user);

                QBuffer buffer(&record_data);
                buffer.open(QIODevice::ReadWrite);

                buffer.reset();

                QString format = QString::fromLatin1(buffer.read(4));
                if(format=="RIFF"){
                    fileName += ".wav";

                    quint32 totalLen = record_data.size() - 8;

                    buffer.putChar(totalLen);
                    buffer.putChar(totalLen>>8);
                    buffer.putChar(totalLen>>16);
                    buffer.putChar(totalLen>>24);

                    quint32 datalen = record_data.size() - 44;

                    buffer.reset();
                    buffer.seek(40);

                    buffer.putChar(datalen);
                    buffer.putChar(datalen>>8);
                    buffer.putChar(datalen>>16);
                    buffer.putChar(datalen>>24);
                }
                else{
                    fileName += ".amr";
                }

                file->setFileName(dir.absolutePath() + "/" + fileName);
                file->open(QIODevice::WriteOnly | QIODevice::Append);
                if(file->isOpen()){
                    //write into harddisk(storage)
                    file->write(record_data);
                    file->flush();
                    file->close();

                    //save into file database
                    _shared->ClipDB()->AddFile(uuid, user, -1, file->fileName());
                    //remove download buffer
                    _shared->DownloadDB()->RemoveFile(type, uuid, user);

                    this->downloadNextFile(uuid, user);
                }
                else{
                    qDebug()<<"create file failed."<<file->fileName();
                }
            }
            file.reset();
    }

    QStringList exist_list;
    QStringList needed_list;

    QVariantMap conf;

    if(type == 1){
        conf = _shared->ConferenceDB()->ConferenceInfo(uuid);
        needed_list = conf.value("devices").toString().split(",");
    }
    else if(type == 0){
        conf = _shared->PersonalDB()->ConferenceInfo(uuid);
        needed_list << conf.value("user_id").toString();
    }

    needed_list.removeDuplicates();

    this->checkConferenceFile(uuid, exist_list, QStringList());

    bool completed(false);
    int conf_size(0);
    float_t percentage(0.0);

    conf_size = _conf_size_map.value(uuid,0);
    conf_size += data_size;
    _conf_size_map.insert(uuid, conf_size);

    percentage = conf_size * 100.0 / _conf_total_size_map.value(uuid,1);
    if(percentage > 100.0){
        percentage = 100.0;
        qDebug()<<"download size over! conference total size:"<<_conf_total_size_map.value(uuid,1)<<"current size:"<<conf_size;
    }

    foreach (QString each, exist_list) {

        if(needed_list.contains(each)){
            needed_list.removeAll(each);
        }
    }

    if(needed_list.count() == 0){
        completed = true;
        qDebug()<<"download size:"<<_conf_size_map.value(uuid,1)<<"conference total size:"<<_conf_total_size_map.value(uuid,1);

        _conf_size_map.remove(uuid);
        _conf_total_size_map.remove(uuid);
        _download_info_map.remove(uuid);

    }

    conf.insert("completed",completed?completed:2);

    if(type == 1){
        _shared->ConferenceDB()->AddConference(conf);
    }
    else if(type == 0){
        _shared->PersonalDB()->AddConference(conf);
    }

    this->monitorDisconnect(type, uuid, identity, percentage, data_size, completed);
}

void  ConfServiceImpl::monitorDisconnect(int type, QString uuid, QString identity, int percentage, int data_size, bool completed)
{
    if(_monitor_map.count() == 0){

        _monitor_timer = this->startTimer(DISCONNECT_INTERVAL);
    }

    if(_monitor_map.contains(uuid)){

        Monitor* monitor = _monitor_map.value(uuid,nullptr);
        if(monitor){
            if(monitor->last_time.msecsTo(QTime::currentTime())){
                monitor->speed = monitor->saved_data + data_size / monitor->last_time.msecsTo(QTime::currentTime());
                monitor->last_time = QTime::currentTime();
                monitor->saved_data = 0;
            }
            else{
                monitor->saved_data = data_size;
            }
            monitor->alive = _monitor_timer + 1;
            monitor->percentage = percentage;
            monitor->completed = completed;
            monitor->identity = identity;
            monitor->idle = 0;
        }
    }
    else{
        Monitor* monitor = new Monitor;
        monitor->type = type;
        monitor->uuid = uuid;
        monitor->percentage = percentage;
        monitor->completed = completed;
        monitor->alive = _monitor_timer;
        monitor->identity = identity;
        monitor->speed = 0;
        monitor->idle = 0;
        monitor->saved_data = 0;
        monitor->last_time = QTime::currentTime();

        _monitor_map.insert(uuid, monitor);

        emit downloadConferenceCompleted(monitor->type, monitor->uuid, monitor->percentage, monitor->speed, 2);
    }
}

bool ConfServiceImpl::checkConferenceAlive(int type, QString uuid)
{
    QVariantMap conf;

    if(type == 1){
        conf = _shared->ConferenceDB()->ConferenceInfo(uuid);
    }
    else if(type == 0){
        conf = _shared->PersonalDB()->ConferenceInfo(uuid);
    }

    return !conf.isEmpty();
}

bool ConfServiceImpl::checkConferenceFile(QString uuid, QStringList needed)
{
    bool completed(false);

    if(needed.count() == 0){
        completed = true;
        return completed;
    }

    QStringList exist_list;
    QStringList missing_list;

    this->checkConferenceFile(uuid, exist_list, missing_list);

    if(missing_list.count() > 0){
        completed = false;
        return completed;
    }

    foreach (QString each, exist_list) {

        if(needed.contains(each)){
            needed.removeAll(each);
        }
    }

    if(needed.count() == 0){
        completed = true;
        return completed;
    }

    return completed;
}
int ConfServiceImpl::checkConferenceFile(QString uuid, QStringList &exists, QStringList &missing)
{
    QVariantList list;
    list = _shared->ClipDB()->GetConferenceFile(uuid);

    if(list.count() > 0){
        QString path;
        foreach (QVariant file, list) {
            path = file.toMap().value("path").toString();
            if(QFile(path).exists() && QFile(path).size() > 0){
                exists << file.toMap().value("identity").toString();
            }else{
                missing << file.toMap().value("identity").toString();
            }
        }
    }

    return list.count();
}
