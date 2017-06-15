#include "service_thread_private.h"
#include "messager/message_base.h"
#include "account/account_center.h"
#include "white_list.h"

#include "storage/include/data_base.h"
#include "storage/include/templatedatabase.h"
#include "storage/include/device_database.h"
#include "storage/include/markdatabase.h"
#include "storage/include/conferencedatabase.h"
#include "storage/include/user_database.h"
#include "storage/include/clip_file_database.h"
#include "storage/include/download_database.h"
#include "storage/include/personal_database.h"


ServiceThreadPrivate::ServiceThreadPrivate():
    _conf_service(nullptr),
    _user_service(nullptr),
    _messager(new MessageBase),
    _account(new AccountCenter),
    _white_list(new WhiteList),
    _info_mode(nullptr),
    _pConferenceMode(nullptr),
    _pPersonalMode(nullptr),
    _record_shared(nullptr)
{
    _db = DataBase::CreateDB();
    _conf_db = ConferenceDatabase::GetInterface(_db);
    _device_db = DeviceDatabase::GetInterface(_db);
    _download_db = DownloadDatabase::GetInterface(_db);
    _clip_db = ClipFileDatabase::GetInterface(_db);
    _template_db = TemplateDatabase::GetInterface(_db);
    _mark_db = MarkDatabase::GetInterface(_db);
    _user_db = UserDatabase::GetInterface(_db);
    _personal_db = PersonalDatabase::GetInterface(_db);
}

ServiceThreadPrivate::~ServiceThreadPrivate()
{
    _messager->stopConnection();
    delete _messager;

    _account->UserLogout();
    delete _account;

    delete _white_list;

    _conf_db->Release();
    _device_db->Release();
    _clip_db->Release();
    _download_db->Release();
    _template_db->Release();
    _mark_db->Release();
    _user_db->Release();
    _personal_db->Release();
    DataBase::DeleteDB(_db);
}
