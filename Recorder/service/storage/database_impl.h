#ifndef DATABASE_IMPL_H
#define DATABASE_IMPL_H

#include "include/data_base.h"
#include "shareddata.h"
#include "conferencedatabase_impl.h"
#include "filedatabase_impl.h"
#include "markdatabase_impl.h"
#include "device_database_impl.h"
#include "templatedatabase_impl.h"
#include "user_database_impl.h"
#include "download_database_impl.h"
#include "clip_file_database_impl.h"
#include "personal_database_impl.h"

class DataBase_Impl : public SharedData,
                      public DataBase,
                      public ConferenceDatabase_Impl,
                      public FileDatabase_Impl,
                      public MarkDatabase_Impl,
                      public DeviceDatabase_Impl,
                      public TemplateDatabase_Impl,
                      public UserDatabase_Impl,
                      public DownloadDatabaseImpl,
                      public ClipFileDatabaseImpl,
                      public PersonalDatabaseImpl
{
public:
    DataBase_Impl():ConferenceDatabase_Impl(this),
                    FileDatabase_Impl(this),
                    MarkDatabase_Impl(this),
                    DeviceDatabase_Impl(this),
                    TemplateDatabase_Impl(this),
                    UserDatabase_Impl(this),
                    DownloadDatabaseImpl(this),
                    ClipFileDatabaseImpl(this),
                    PersonalDatabaseImpl(this),
                    _ref_count(0){}
    ~DataBase_Impl() override {}

    int AddRef();
    int Release() override;

protected:
    int  _ref_count;
};

#endif // DATABASE_IMPL_H
