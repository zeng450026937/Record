#include "database_impl.h"

static int32_t gDataBaseInstanceCounter = 0;

DataBase* GetDataBase(){
    DataBase_Impl* self(NULL);
    self = new DataBase_Impl();
    if (self != NULL) {
        self->AddRef();  // First reference.  Released in DataBase::Delete.
        gDataBaseInstanceCounter++;
    }
    return self;
}


DataBase* DataBase::CreateDB()
{
    return GetDataBase();
}
bool      DataBase::DeleteDB(DataBase* db)
{
    if(db == NULL)
        return false;

    DataBase_Impl* s = static_cast<DataBase_Impl*>(db);
    int ref = s->Release();
    db = NULL;

    if(ref != 0) {
        //  Delete did not release the very last reference.
    }

    return true;
}

int DataBase_Impl::AddRef() {
  return ++_ref_count;
}

int DataBase_Impl::Release() {
  int new_ref = --_ref_count;

  if (new_ref == 0) {
    delete this;
  }

  return new_ref;
}

