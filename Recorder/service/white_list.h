#ifndef WHITE_LIST_H
#define WHITE_LIST_H

#include <QStringList>

class WhiteList{
public:
    WhiteList(){
        _user_list.clear();
        _user_list<<"198800";
        _user_list<<"697111";
        _user_list<<"770321";
        _user_list<<"777";
        //_user_list<<"450026";
    }
    ~WhiteList(){
        _user_list.clear();
    }

    QStringList whiteList(){ return _user_list; }
    void addUser(QString user){ _user_list<<user;}
    void removeUser(QString user){ _user_list.removeAll(user);}

private:
    QStringList _user_list;
};


#endif // WHITE_LIST_H
