#ifndef SERVICETHREADPRIVATE_H
#define SERVICETHREADPRIVATE_H

class ConfService;
class UserService;
class MessageBase;
class AccountCenter;
class WhiteList;

class DataBase;
class ConferenceDatabase;
class DeviceDatabase;
class TemplateDatabase;
class MarkDatabase;
class UserDatabase;
class ClipFileDatabase;
class DownloadDatabase;
class PersonalDatabase;

class ServiceThreadPrivate
{
public:
    ServiceThreadPrivate();
    ~ServiceThreadPrivate();

    MessageBase*   Messager() { return _messager; }
    AccountCenter* Account()  { return _account; }
    WhiteList*     AppUser()     { return _white_list; }

    ConfService* GetConfService() { return _conf_service; }
    UserService* GetUserService() { return _user_service; }

    ConferenceDatabase* ConferenceDB() { return _conf_db; }
    DeviceDatabase*     DeviceDB() { return _device_db; }
    ClipFileDatabase*   ClipDB() { return _clip_db; }
    DownloadDatabase*   DownloadDB() { return _download_db; }
    TemplateDatabase*   TemplateDB() { return _template_db; }
    MarkDatabase*       MarkDB() { return _mark_db; }
    UserDatabase*       UserDB() { return _user_db; }
    PersonalDatabase*   PersonalDB() { return _personal_db; }

private:
    ConfService*    _conf_service;
    UserService*    _user_service;
    MessageBase*    _messager;
    AccountCenter*  _account;
    WhiteList*      _white_list;

    DataBase*           _db;

    ConferenceDatabase* _conf_db;
    DeviceDatabase*     _device_db;
    ClipFileDatabase*   _clip_db;
    DownloadDatabase*   _download_db;
    TemplateDatabase*   _template_db;
    MarkDatabase*       _mark_db;
    UserDatabase*       _user_db;
    PersonalDatabase*   _personal_db;
};

#endif // SERVICETHREADPRIVATE_H
