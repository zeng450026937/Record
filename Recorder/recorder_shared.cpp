
#include "service/service_thread.h"
#include "service/user_service_impl.h"
#include "service/conf_service_impl.h"
#include "common/config.h"
#include "recorder_shared.h"

#include <QDir>
#include <QDebug>
#include <QDateTime>
#include <QFile>

static QFile* gDebugLogInstance = 0;

void RecorderShared::customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &str)
{
    Q_UNUSED(context);

    QString txt;
    switch (type) {
    //调试信息提示
    case QtDebugMsg:
            txt = QString("Debug: %1").arg(str);
            break;
    //一般的warning提示
    case QtWarningMsg:
            txt = QString("Warning: %1").arg(str);
    break;
    //严重错误提示
    case QtCriticalMsg:
            txt = QString("Critical: %1").arg(str);
    break;
    //致命错误提示
    case QtFatalMsg:
            txt = QString("Fatal: %1").arg(str);
            abort();
    }

    QTextStream ts(gDebugLogInstance);
    ts << txt << endl;
}

RecorderShared::RecorderShared(QObject* parent) :
    QObject(parent),
    _service(ServiceThread::GetInstance()),
    _config(Config::GetInstance()),
    _service_ready(false)
{
    //init debug output
    gDebugLogInstance = new QFile();
    gDebugLogInstance->setFileName("debugLog.txt");
    gDebugLogInstance->open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(gDebugLogInstance);
    ts << endl << QDateTime::currentDateTime().toString() << endl << endl;

    this->initialize();
}

RecorderShared::~RecorderShared()
{
    delete _config;

    if(gDebugLogInstance)
    {
        gDebugLogInstance->flush();
        gDebugLogInstance->close();
        delete gDebugLogInstance;
        gDebugLogInstance = 0;
    }
}

void RecorderShared::aboutToQuit()
{

}

//conference interface
void     RecorderShared::CreateConf(QVariantMap& conf)
{
    if(_service_ready)
        _service->GetConfService()->createConference(conf);
}
void     RecorderShared::StartConf(QString uuid)
{
    if(_service_ready)
        _service->GetConfService()->startConference(uuid);
}
void     RecorderShared::PauseConf(QString uuid)
{
    if(_service_ready)
        _service->GetConfService()->pauseConference(uuid);
}
void     RecorderShared::StopConf(QString uuid)
{
    if(_service_ready)
        _service->GetConfService()->stopConference(uuid);
}
void     RecorderShared::SetConfInfo(QString uuid, QVariantMap& conf)
{
    if(_service_ready)
        _service->GetConfService()->setConferenceInfo(uuid, conf);
}
void     RecorderShared::SetFolder(QString folder)
{
    if(_service_ready){
        _service->GetConfService()->setDownloadFolder(folder);
    }
}
QString  RecorderShared::GetFolder(int type, QString uuid)
{
    if(_service_ready){
        QDir dir;
        dir.setPath(_service->GetConfService()->outputFolder(type, uuid));
        return dir.dirName();
    }

    return QString();
}
void     RecorderShared::AddClipFile(QVariantMap& file)
{
    if(_service_ready)
        _service->GetConfService()->addClipFile(file);
}

QVariantList RecorderShared::GetFileList(QString uuid)
{
    if(_service_ready){
        return _service->GetConfService()->fileList(uuid);
    }

    return QVariantList();
}

void RecorderShared::DownLoadConference(int type, QString uuid)
{
    _service->GetConfService()->downloadConference(type, uuid);
}
void RecorderShared::CheckConferenceFile(QVariantMap &conf)
{
    QString uuid = conf.value("uuid").toString();

    int index = _conf_uuid_list.indexOf( uuid );

    if(index == -1){
        return;
    }
    else{
        _service->GetConfService()->checkConference(conf);
        ModelUpdater::UpdateRow(ModelUpdater::ConferenceModel, index, conf);
    }
}
void     RecorderShared::CheckPersonalFile(QVariantMap& conf)
{
    QString uuid = conf.value("conference_uuid").toString();

    int index = _personal_uuid_list.indexOf( uuid );

    if(index == -1){
        return;
    }
    else{
        _service->GetConfService()->checkPersonal(conf);
        ModelUpdater::UpdateRow(ModelUpdater::PersonalModel, index, conf);
    }
}
//mark interface
QVariantList RecorderShared::GetMark(QString conf_uuid)
{
    if(_service_ready)
        return _service->GetConfService()->markInfo(conf_uuid);

    return QVariantList();
}
void RecorderShared::AddMark(ModelType type, QString conf_uuid,QVariantMap& mark)
{
    if(_service_ready){
        _service->GetConfService()->addMarkInfo(conf_uuid, mark);
        ModelUpdater::AppendRow(type, mark);
    }
}

//device interface
void RecorderShared::RefreshDeviceList()
{
    if(_service_ready)
        _service->GetConfService()->getDeviceInfoList();
}
void RecorderShared::SelectDevice(bool selected, QVariantMap& device)
{
    if(selected)
        _selected_device << device.value("device_mac").toString();
    else
        _selected_device.removeAll( device.value("device_mac").toString() );

    this->receive_deviceInfoUpdate(device);
}
QVariantMap RecorderShared::DeviceInfo(QString mac)
{
    if(_service_ready){
        return _service->GetConfService()->deviceInfo(mac);
    }

    return QVariantMap();
}

//template interface
void     RecorderShared::AddTemplate(QVariantMap& Template)
{
    if(_service_ready)
        _service->GetConfService()->addTemplateInfo(Template);
}
void     RecorderShared::RemoveTemplate(QString& uuid)
{
    if(_service_ready)
        _service->GetConfService()->delTemplateInfo(uuid);
}
void    RecorderShared::SetTemplate(QVariantMap& Template)
{
    if(_service_ready)
        _service->GetConfService()->setTemplateInfo(Template);
}

QVariantList RecorderShared::GetTemplateList()
{
    if(_service_ready)
        return _service->GetConfService()->templateList();
    else
        return QVariantList();
}

void RecorderShared::receive_service_ready()
{ 
    _service_ready = true;
    QObject::connect(_service->GetConfService(),SIGNAL(conferenceCreated(bool,QVariantMap)),
                     this,SLOT(receive_conferenceCreated(bool,QVariantMap)),Qt::QueuedConnection);
    QObject::connect(_service->GetConfService(),SIGNAL(conferenceStarted(bool,QVariantMap)),
                     this,SLOT(receive_conferenceStarted(bool,QVariantMap)),Qt::QueuedConnection);
    QObject::connect(_service->GetConfService(),SIGNAL(conferencePaused(bool,QVariantMap)),
                     this,SLOT(receive_conferencePaused(bool,QVariantMap)),Qt::QueuedConnection);
    QObject::connect(_service->GetConfService(),SIGNAL(conferenceStoped(bool,QVariantMap)),
                     this,SLOT(receive_conferenceStoped(bool,QVariantMap)),Qt::QueuedConnection);
    QObject::connect(_service->GetConfService(),SIGNAL(conferenceInfoSetted(bool,QVariantMap)),
                     this,SLOT(receive_conferenceInfoSetted(bool,QVariantMap)),Qt::QueuedConnection);
    
    QObject::connect(_service->GetConfService(),SIGNAL(personalConfCreated(bool,QVariantMap)),
                     this,SLOT(receive_personalConfCreated(bool,QVariantMap)),Qt::QueuedConnection);
    QObject::connect(_service->GetConfService(),SIGNAL(personalConfSetted(bool,QVariantMap)),
                     this,SLOT(receive_personalConfSetted(bool,QVariantMap)),Qt::QueuedConnection);
    QObject::connect(_service->GetConfService(),SIGNAL(personalConfDeleted(bool,QVariantMap)),
                     this,SLOT(receive_personalConfDeleted(bool,QVariantMap)),Qt::QueuedConnection);
   
    
    QObject::connect(_service->GetConfService(),SIGNAL(downloadConferenceCompleted(int,QString,int,int,int)),
                     this,SLOT(receive_downloadConferenceCompleted(int,QString,int,int,int)),Qt::QueuedConnection);

    QObject::connect(_service->GetConfService(),SIGNAL(deviceInfoListGetted(bool,QVariantList)),
                     this,SLOT(receive_deviceInfoListGetted(bool,QVariantList)),Qt::QueuedConnection);
    QObject::connect(_service->GetConfService(),SIGNAL(deviceInfoUpdate(QVariantMap)),
                     this,SLOT(receive_deviceInfoUpdate(QVariantMap)),Qt::QueuedConnection);
    QObject::connect(_service->GetConfService(),SIGNAL(deviceEnvironmentUpdate(QVariantMap)),
                     this,SLOT(receive_deviceEnvironmentUpdate(QVariantMap)),Qt::QueuedConnection);

    QObject::connect(_service->GetConfService(),SIGNAL(templateInfoAdded(bool,QVariantMap)),
                     this,SLOT(receive_templateInfoAdded(bool,QVariantMap)),Qt::QueuedConnection);
    QObject::connect(_service->GetConfService(),SIGNAL(templateInfoDeleted(bool,QVariantMap)),
                     this,SLOT(receive_templateInfoDeleted(bool,QVariantMap)),Qt::QueuedConnection);
    QObject::connect(_service->GetConfService(),SIGNAL(templateInfoGot(bool,QVariantMap)),
                     this,SLOT(receive_templateInfoGot(bool,QVariantMap)),Qt::QueuedConnection);
    QObject::connect(_service->GetConfService(),SIGNAL(templateInfoSetted(bool,QVariantMap)),
                     this,SLOT(receive_templateInfoSetted(bool,QVariantMap)),Qt::QueuedConnection);
//     QObject::connect(_service->GetConfService(),SIGNAL(templateListGot(bool,QVariantList)),
//                      this,SLOT(receive_templateListGot(bool,QVariantList)),Qt::QueuedConnection);

// 
//     if(_config->_output_dir.isEmpty())
         // _config->_output_dir = _service->GetConfService()->downloadFolder();
// 
//     if(QDir(_config->_output_dir).isReadable())
//         _service->GetConfService()->setDownloadFolder(_config->_output_dir);
//     else
//         _config->_output_dir = _service->GetConfService()->downloadFolder();
}

void RecorderShared::receive_conferenceCreated(bool result, QVariantMap info)
{
    QString uuid = info.value("uuid").toString();
    info = _service->GetConfService()->conferenceInfo(uuid);

    if(result){
        int index = _conf_uuid_list.indexOf( uuid );

        if(index == -1){
            _conf_uuid_list << uuid;
            ModelUpdater::AppendRow(ModelUpdater::ConferenceModel, info);
        }
        else{
            ModelUpdater::UpdateRow(ModelUpdater::ConferenceModel, index, info);
        }
    }
    else{
        qDebug()<<"create conference failed";
    }
    emit conference_notify(kConfCreated, result, info);
}
void RecorderShared::receive_conferenceStarted(bool result, QVariantMap info)
{
    if(!result)
        qDebug()<<"start conference failed";
    emit conference_notify(kConfStarted, result, info);
}

void RecorderShared::receive_conferencePaused(bool result, QVariantMap info)
{
    if(!result)
        qDebug()<<"pause conference failed";
    emit conference_notify(kConfPaused, result, info);
}
void RecorderShared::receive_conferenceStoped(bool result, QVariantMap info)
{
    if(!result)
        qDebug()<<"stop conference failed";
    emit conference_notify(kConfStoped, result, info);
}

void RecorderShared::receive_conferenceInfoSetted(bool result, QVariantMap info)
{
    QString uuid = info.value("uuid").toString();
    info = _service->GetConfService()->conferenceInfo(uuid);

    if(result){
        int index = _conf_uuid_list.indexOf( uuid );

        if(index == -1){
            _conf_uuid_list << uuid;
            ModelUpdater::AppendRow(ModelUpdater::ConferenceModel, info);
        }
        else{
            ModelUpdater::UpdateRow(ModelUpdater::ConferenceModel, index, info);
        }
    }
    else{
        qDebug()<<"set conference info failed";
    }
    emit conference_notify(kConfCreated, result, info);
}
void RecorderShared::on_conference_list_got_trigger(bool result)
{
    QVariantList list = _service->GetConfService()->conferenceList();

    if(result){
        _conf_uuid_list.clear();
        foreach (const QVariant &conf, list) {
            _conf_uuid_list << conf.toMap().value("uuid").toString();
            _service->GetConfService()->getMarkInfo( conf.toMap().value("uuid").toString() );
        }
        ModelUpdater::ListToModel(ModelUpdater::ConferenceModel, list);
    }
    else{
        qDebug()<<"get conference list failed";
    }
}

void RecorderShared::receive_downloadConferenceCompleted(int type, QString uuid, int percentage, int speed, int completed)
{
    int index(-1);
    QVariantMap info;

    if(type == 1){
        info = _service->GetConfService()->conferenceInfo(uuid);
        info.insert("speed",speed);
        info.insert("percentage",percentage);

        index = _conf_uuid_list.indexOf( uuid );

        if(index != -1){
            ModelUpdater::UpdateRow(ModelUpdater::ConferenceModel, index, info);
        }
    }
    else if(type == 0){
        info = _service->GetConfService()->personalConfInfo(uuid);
        info.insert("speed",speed);
        info.insert("percentage",percentage);

        index = _personal_uuid_list.indexOf( uuid );

        if(index != -1){
            ModelUpdater::UpdateRow(ModelUpdater::PersonalModel, index, info);
        }
    }

    emit download_notify(type, uuid, percentage, completed);
}

void RecorderShared::receive_personalConfCreated(bool result, QVariantMap info)
{
    QString uuid = info.value("conference_uuid").toString();
    info = _service->GetConfService()->personalConfInfo(uuid);

    if(result){
        int index = _personal_uuid_list.indexOf( uuid );

        if(index == -1)
        {
            _personal_uuid_list << uuid;
            ModelUpdater::AppendRow(ModelUpdater::PersonalModel, info);
        }
        else
        {
            ModelUpdater::UpdateRow(ModelUpdater::PersonalModel, index, info);
        }
    }
    else{
        qDebug()<<"create personal conference failed";
    }
}

void RecorderShared::receive_personalConfSetted(bool result, QVariantMap info)
{
    QString uuid = info.value("conference_uuid").toString();
    info = _service->GetConfService()->personalConfInfo(uuid);

    if(result){
        int index = _personal_uuid_list.indexOf( uuid );

        if(index == -1)
        {
            _personal_uuid_list << uuid;
            ModelUpdater::AppendRow(ModelUpdater::PersonalModel, info);
        }
        else{
            ModelUpdater::UpdateRow(ModelUpdater::PersonalModel, index, info);
        }
    }
    else{
        qDebug()<<"set personal conference failed";
    }
}

void RecorderShared::receive_personalConfDeleted(bool result, QVariantMap info)
{
    QString uuid = info.value("conference_uuid").toString();

    if(result){
        int index = _personal_uuid_list.indexOf( uuid );

        if(index == -1){
        }
        else{
            _personal_uuid_list.removeAll(uuid);
            ModelUpdater::RemoveRow(ModelUpdater::PersonalModel, index);
        }
    }
    else{
        qDebug()<<"delete personal conference failed";
    }
}

void RecorderShared::on_personal_list_got_trigger(bool result)
{
    QVariantList list = _service->GetConfService()->personalConfList();

    if(result)
    {
        _personal_uuid_list.clear();
        foreach (const QVariant &conf, list) 
        {
            _personal_uuid_list << conf.toMap().value("conference_uuid").toString();
        }
        ModelUpdater::ListToModel(ModelUpdater::PersonalModel, list);
    }
    else
    {
        qDebug()<<"get personal list failed";
    }
}

void RecorderShared::on_all_personal_list_got_trigger(bool result)
{
    QVariantList list = _service->GetConfService()->personalConfList();

    if(result){
        _personal_uuid_list.clear();
        foreach (QVariant conf, list) {
            _personal_uuid_list << conf.toMap().value("conference_uuid").toString();
        }
        ModelUpdater::ListToModel(ModelUpdater::PersonalModel, list);
        //ModelUpdater::AppendList(ModelUpdater::AllConferenceModel, list);
    }
    else{
        qDebug()<<"get all personal list failed";
    }
}

void RecorderShared::receive_deviceInfoUpdate(QVariantMap info)
{
    int index = _device_mac_list.indexOf( info.value("device_mac").toString() );

    if(index == -1){
        _device_mac_list << info.value("device_mac").toString();
        ModelUpdater::AppendRow(ModelUpdater::DeviceModel, info);
    }
    else{
        if( _selected_device.contains(info.value("device_mac").toString()) )
            info.insert("selected",true);
        else
            info.insert("selected",false);

        ModelUpdater::UpdateRow(ModelUpdater::DeviceModel, index, info);
    }
}
void RecorderShared::receive_deviceInfoListGetted(bool result, QVariantList list)
{
    if(result){
        _device_mac_list.clear();
        foreach (QVariant device, list) {
            _device_mac_list << device.toMap().value("device_mac").toString();
        }
        ModelUpdater::ListToModel(ModelUpdater::DeviceModel, list);
    }
    else{
        qDebug()<<"get all device list failed";
    }
}

void RecorderShared::receive_deviceEnvironmentUpdate(QVariantMap info)
{
    emit record_notify(info.value("device_mac").toString(),
                       info.value("device_name").toString(),
                       info.value("status").toString());
}

void RecorderShared::receive_addMarkInfo(bool result, QVariantMap info)
{
    if(result){

    }
    else{
        qDebug()<<"add mark info failed";
    }
}

void RecorderShared::receive_templateInfoAdded(bool result, QVariantMap info)
{
    if(result){
        _template_uuid_list << info.value("uuid").toString();
        ModelUpdater::AppendRow(ModelUpdater::TemplateModel, info);
    }
    else{
        qDebug()<<"add template info failed";
    }
}
void RecorderShared::receive_templateInfoSetted(bool result, QVariantMap info)
{
    if(result){
        int index = _template_uuid_list.indexOf( info.value("uuid").toString() );
        if(index != -1){
            ModelUpdater::UpdateRow(ModelUpdater::TemplateModel, index, info);
        }
    }
    else{
        qDebug()<<"set template info failed";
    }
}
void RecorderShared::receive_templateInfoDeleted(bool result, QVariantMap info)
{
    if(result){
        int index = _template_uuid_list.indexOf( info.value("uuid").toString() );
        if(index != -1){
            _template_uuid_list.removeAt(index);
            ModelUpdater::RemoveRow(ModelUpdater::TemplateModel, index);
        }
    }
    else{
        qDebug()<<"delete template info failed";
    }
}
void RecorderShared::receive_templateInfoGot(bool result, QVariantMap info)
{
    Q_UNUSED(result);
    Q_UNUSED(info);
}
void RecorderShared::on_template_list_got_trigger(bool result)
{
    QVariantList list = _service->GetConfService()->templateList();

    if(result){
        _template_uuid_list.clear();
        foreach (QVariant Template, list) {
            _template_uuid_list << Template.toMap().value("uuid").toString();
        }
        ModelUpdater::ListToModel(ModelUpdater::TemplateModel, list);
    }
    else{
        qDebug()<<"get template list failed";
    }
}

void    RecorderShared::initialize()
{
    //init config
    if(_config->_ffmpeg_path.isEmpty())
        _config->_ffmpeg_path = "./ffmpeg.exe";

    if(_config->_clip_combo.isEmpty())
        _config->_clip_combo << "0s" << "2s" << "5s" << "10s" ;

    if(_config->_type_combo.isEmpty())
        _config->_type_combo<<"全部录音"<< "会议录音列表" << "个人录音列表"<< "移动会议" ;

    if(_config->_file_extention.isEmpty())
        _config->_file_extention = ".wav";

    if(_config->_output_dir.isEmpty() || !QDir(_config->_output_dir).isReadable()){
        QDir dir("./OUT");
        _config->_output_dir = dir.absolutePath();
    }

    if(_config->_debug_output){
        qInstallMessageHandler(&RecorderShared::customMessageHandler);
    }

    QObject::connect(_service,SIGNAL(service_ready()),
                     this,SLOT(receive_service_ready()),Qt::QueuedConnection);
}

void RecorderShared::request_data()
{
    if(_server_available)
    {
        _service->GetConfService()->getDeviceInfoList();
        _service->GetConfService()->getTemplateList();
        _service->GetConfService()->getConferenceList();
        _service->GetConfService()->getAllPersonalList();
    }
    else{
        this->on_conference_list_got_trigger(true);
        this->on_template_list_got_trigger(true);
        this->on_personal_list_got_trigger(true);
    }
}


