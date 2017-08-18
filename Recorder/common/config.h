#pragma once
#include <QString>
#include <QStringList>

class Config {
  Config();

 public:
  struct USER {
    QString user_id;
    QString user_name;
    QString access_token;
    QString user_group;
    QString mac_key;
  };

 public:
  ~Config();

  static Config *GetInstance();

  USER &GetUser();
  void Save();

  void Reset();

  Config &operator=(const Config &rhs);

  QStringList _type_combo;
  QStringList _clip_combo;

  QString _clip_prev;
  QString _clip_next;

  QString _ffmpeg_path;
  QString _output_dir;

  QString _file_extention;

  QString _test_server;
  QString _server_address;

  bool _qa_test;
  bool _hackers;
  bool _debug_output;
  bool _auto_download;

  QString _device_uuid;
  QString _faker;

  const char *const DEVICE_TYPE = "PC";
  const char *const DEFAULT_SERVER = "ws://118.191.2.215:12080";
  const char *const TEST_SERVER = "ws://192.168.85.31:9008";

  static const QString s_DEFAULT_SERVER;

 private:
  struct USER _user;

 private:
  static Config *s_pSinglton;
};
