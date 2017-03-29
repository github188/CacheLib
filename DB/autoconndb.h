#ifndef AUTOCONNDB_H
#define AUTOCONNDB_H

#include "DB/odbcdb.h"
namespace DB
{
class AutoConnDB
{
private:
    OdbcDB * _curConn;
public:
    AutoConnDB();
    AutoConnDB(OdbcDB *p);

    AutoConnDB& operator =(OdbcDB* curConn);
    OdbcDB& operator *();
    OdbcDB* operator ->();
    ~AutoConnDB();
};
}

#endif // AUTOCONNCACHE_H
