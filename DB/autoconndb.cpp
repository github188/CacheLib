#include "autoconndb.h"

#include "odbcpool.h"
namespace DB
{
AutoConnDB::AutoConnDB()
{
    _curConn = NULL;
}
AutoConnDB::AutoConnDB(OdbcDB *p) : _curConn(p)  //构造函数
{
}
OdbcDB& AutoConnDB::operator *()        //重载*操作符
{
    return *_curConn;
}
OdbcDB* AutoConnDB::operator ->()       //重载->操作符
{
    return _curConn;
}
AutoConnDB::~AutoConnDB()        //析构函数
{
    if(_curConn)
    {
        ODBCPool->freeConnServer(_curConn);
    }
}
AutoConnDB& AutoConnDB::operator =(OdbcDB* curConn)
{
    if(curConn != _curConn)
    {
        if(_curConn)
        {
             ODBCPool->freeConnServer(_curConn);
        }
       _curConn = curConn;
    }
    return *this;
}
}
