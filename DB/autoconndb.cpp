#include "autoconndb.h"

#include "odbcpool.h"
namespace DB
{
AutoConnDB::AutoConnDB()
{
    _curConn = NULL;
}
AutoConnDB::AutoConnDB(OdbcDB *p) : _curConn(p)  //���캯��
{
}
OdbcDB& AutoConnDB::operator *()        //����*������
{
    return *_curConn;
}
OdbcDB* AutoConnDB::operator ->()       //����->������
{
    return _curConn;
}
AutoConnDB::~AutoConnDB()        //��������
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
