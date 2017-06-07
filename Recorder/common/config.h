#pragma once
#include <QString>
#include <QStringList>

class Config
{
	Config();
public:
    ~Config();

	static Config *GetInstance();

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

	QString			_device_uuid;

	const char *const  DEVICE_TYPE = "PC";
	const char *const  DEFAULT_SERVER = "ws://118.191.2.215:12080";
	const char *const  TEST_SERVER = "ws://192.168.85.31:9008";

	static const QString s_DEFAULT_SERVER;

private:
	static Config *s_pSinglton;
};
