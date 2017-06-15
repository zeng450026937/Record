#include "shareddata.h"

#include <QMutex>
#include <QDebug>

SharedData::SharedData() :
    _apiLock(new QMutex(QMutex::Recursive))
{
    InitDb();
    _db.transaction();
}
SharedData::~SharedData()
{
    _db.commit();
    _db.close();

    delete _apiLock;
}

bool SharedData::InitDb()
{
    //init db
    _db = QSqlDatabase::addDatabase("QSQLITE");

    _db.setDatabaseName("local.db");

    if (!_db.open()) {
        //"Cannot open database"
    }

    //create table
    QSqlQuery query(_db);
    bool ret(false);

//     ret = query.exec("CREATE TABLE IF NOT EXISTS tb_conference ("
//                      "id INTEGER PRIMARY KEY NOT NULL, "
//                      "uuid varchar(255) UNIQUE NOT NULL, "
//                      "title varchar(255) DEFAULT '', "
//                      "content varchar(255) DEFAULT '', "
//                      "members varchar(255) DEFAULT '', "
//                      "devices varchar NOT NULL,"
//                      "create_time varchar(255) NOT NULL DEFAULT '', "
//                      "update_time varchar(255) NOT NULL DEFAULT '', "
//                      "date varchar(255) DEFAULT '', "
//                      "time varchar(255) DEFAULT '', "
//                      "completed int DEFAULT '0' "
//                       ")");

    ret = query.exec("CREATE TABLE IF NOT EXISTS tb_download ("
                     "type int DEFAULT '0', "
                     "identity varchar(255) NOT NULL, "
                     "uuid varchar(255) NOT NULL, "
                     "meeting_time int DEFAULT '0', "
                     "startpos int DEFAULT '0', "
                     "status int DEFAULT '0', "
                     "data_size int DEFAULT '0', "
                     "data blob DEFAULT '' "
                     ")");

    ret = query.exec("CREATE TABLE IF NOT EXISTS tb_clip_file ("
                     "id INTEGER PRIMARY KEY NOT NULL, "
                     "uuid varchar(255) NOT NULL, "
                     "identity varchar(255) NOT NULL, "
                     "start_time int DEFAULT '0', "
                     "path varchar(255) UNIQUE NOT NULL "
                     ")");

    ret = query.exec("CREATE TABLE IF NOT EXISTS tb_record_info ("
                     "id INTEGER PRIMARY KEY NOT NULL, "
                     "record_type TEXT,"
                     "conference_uuid varchar(255) UNIQUE NOT NULL, "
                     "title varchar(255) NOT NULL, "
                     "user_id varchar(255) NOT NULL, "
                     "user_name varchar(255) NOT NULL, "
                     // "type varchar(255) DEFAULT '', "
                     "gps varchar(255) DEFAULT '', "
                     "gps_address varchar(255) DEFAULT '', "
                     "tag varchar(255) DEFAULT '', "
                     "device_mac varchar(255) DEFAULT '', "
                     "device_name varchar(255) DEFAULT '', "
                     "state varchar(255) DEFAULT '', "
                     "create_time varchar(255) NOT NULL DEFAULT '', "
                     "update_time varchar(255) NOT NULL DEFAULT '', "
                     "date varchar(255) DEFAULT '', "
                     "time varchar(255) DEFAULT '', "
                     "completed int DEFAULT '0' "
                     ")");

    ret = query.exec("CREATE TABLE IF NOT EXISTS MARK ("
                          //"id INTEGER PRIMARY KEY autoincrement, "
                          "uuid varchar(255) UNIQUE NOT NULL, "
                          "conference_uuid varchar(255) DEFAULT '', "
                          "time int DEFAULT '0', "
                          "mark varchar(255) DEFAULT ''"
                          ")");

    ret = query.exec("CREATE TABLE IF NOT EXISTS DEVICE ("
                         "id INTEGER PRIMARY KEY NOT NULL, "
                         "mac varchar(255) UNIQUE NOT NULL, "
                         "name varchar(255) DEFAULT '', "
                         "account varchar(255) DEFAULT '', "
                         "selected int DEFAULT '0', "
                         "used int DEFAULT '0', "
                         "battery int DEFAULT '0', "
                         "battery_time int DEFAULT '0', "
                         "status varchar(255) DEFAULT 'offline' "
                         ")");

    ret = query.exec("CREATE TABLE IF NOT EXISTS TEMPLATE ("
                         "id INTEGER PRIMARY KEY NOT NULL, "
                         "uuid varchar(255) UNIQUE NOT NULL, "
                         "title varchar(255) DEFAULT '', "
                         "content varchar(255) DEFAULT '', "
                         "members varchar(255) DEFAULT '' "
                         ")");

    ret = query.exec("CREATE TABLE IF NOT EXISTS USER ("
                         //"id INTEGER PRIMARY KEY, "
                         "account varchar(255) UNIQUE NOT NULL, "
                         "display_name varchar(255) DEFAULT '' "
                         ")");
    return ret;
}
