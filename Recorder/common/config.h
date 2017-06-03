#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QSettings>
#include <QDir>
#include <QDebug>

class Config
{
public:
    Config();
    ~Config();

    void Reset();

    Config& operator=(const Config& rhs);

    QStringList    _type_combo;
    QStringList    _clip_combo;

    QString        _clip_prev;
    QString        _clip_next;

    QString        _ffmpeg_path;
    QString        _output_dir;

    QString        _file_extention;

    QString        _test_server;
    QString        _server_address;

    bool           _qa_test;
    bool           _hackers;
    bool           _debug_output;
    bool           _auto_download;

    const QString  DEFAULT_SERVER = "ws://118.191.2.215:12080";
    const QString  TEST_SERVER = "ws://172.24.132.162:9004";
};

inline Config::Config()
{
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope,QDir::currentPath());
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                             "ND", "Configs");

    settings.beginGroup("COMMON");

    _ffmpeg_path = settings.value("ffmpeg_path").toString();
    _output_dir = settings.value("output_dir").toString();

    _clip_prev = settings.value("clip_prev",QString("5s")).toString();
    _clip_next = settings.value("clip_next",QString("5s")).toString();

    _type_combo = settings.value("type_combo",QStringList()<<"会议录音"<<"个人录音").toStringList();
    _clip_combo = settings.value("clip_combo").toStringList();

    _file_extention = settings.value("file_extention",QString(".wav")).toString();

    _qa_test = settings.value("qa_test", (bool)false).toBool();
    _test_server = settings.value("test_server", TEST_SERVER).toString();

    QString server;
    if(_qa_test)
        server =  _test_server;
    else
        server = DEFAULT_SERVER;

    _server_address = server;
    _hackers = settings.value("hackers",(bool)false).toBool();

    _debug_output = settings.value("debug_output",(bool)true).toBool();
    _auto_download = settings.value("auto_download",(bool)false).toBool();

    settings.endGroup();
}

inline Config::~Config()
{
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope,QDir::currentPath());
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                             "ND", "Configs");

    settings.clear();

    settings.beginGroup("COMMON");

    settings.setValue("ffmpeg_path",_ffmpeg_path);
    settings.setValue("output_dir",_output_dir);

    settings.setValue("clip_prev",_clip_prev);
    settings.setValue("clip_next",_clip_next);

    settings.setValue("type_combo",_type_combo);
    settings.setValue("clip_combo",_clip_combo);

    settings.setValue("file_extention",_file_extention);

    settings.setValue("test_server",_test_server);
    settings.setValue("qa_test",_qa_test);

    settings.setValue("debug_output",_debug_output);

    if(_auto_download)
        settings.setValue("auto_download",_auto_download);
    if(_hackers)
        settings.setValue("hackers", _hackers);

    settings.endGroup();
}

inline void Config::Reset()
{
    _ffmpeg_path = "";
    _output_dir = "";
    _clip_prev = "5s";
    _clip_next = "5s";
    _type_combo.clear();
    _clip_combo.clear();
    _file_extention = ".wav";
    _test_server = "ws://172.24.132.162:9004";
    _server_address = "ws://118.191.2.215:12080";
    _debug_output = true;
    _qa_test = false;
    _auto_download = false;
    _hackers = false;
}

inline Config& Config::operator=(const Config& rhs)
{
    _ffmpeg_path = rhs._ffmpeg_path;
    _output_dir = rhs._output_dir;
    _clip_prev = rhs._clip_prev;
    _clip_next = rhs._clip_next;
    _type_combo = rhs._type_combo;
    _clip_combo = rhs._clip_combo;
    _file_extention = rhs._file_extention;
    _server_address = rhs._server_address;
    _debug_output = rhs._debug_output;
    _test_server = rhs._test_server;
    _qa_test = rhs._qa_test;
    _hackers = rhs._hackers;
    _auto_download = rhs._auto_download;

    return *this;
}


#endif // CONFIG_H
