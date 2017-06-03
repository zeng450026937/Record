#ifndef DATA_BASE_H
#define DATA_BASE_H

class DataBase
{
public:
    static DataBase* CreateDB();
    static bool      DeleteDB(DataBase* db);

protected:
    DataBase(){}
    ~DataBase(){}
};

#endif // DATA_BASE_H
