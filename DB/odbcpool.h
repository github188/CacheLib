#ifndef ODBCPOOL_H
#define ODBCPOOL_H

#include <boost/noncopyable.hpp>
#include <set>


#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

#include <pthread.h>
#include <boost/thread/mutex.hpp>
using namespace std;

#include "odbcdb.h"
#include "autoconndb.h"



#define ODBCPool  DB::OdbcPool::Instance()

namespace DB
{
class OdbcPool : boost::noncopyable
{
public:

    enum DBType
    {
        DB_SET,
        DB_DATA,
        DB_MAX
    };
    static OdbcPool* Instance()
    {
        if(!_instance)
        {
            _instance = new OdbcPool();
        }
        return _instance;
    }

    AutoConnDB getCacheServer(DBType dbType = DB_DATA);
    void freeConnServer(OdbcDB* usedConn);
private:
    OdbcPool();
    ~OdbcPool();
    bool loadConf();
    static OdbcPool* _instance;
private:

    map<int,set<OdbcDB*> > _freeConn;
    map<int,set<OdbcDB*> > _usedConn;
    //set<OdbcDB*> _freeConn;

    boost::mutex _connLock;
    SQLHENV _henv;  //»·¾³¾ä±ú

    TagConnection _connConfig[DB_MAX];
};
}

#endif // ODBCPOOL_H
