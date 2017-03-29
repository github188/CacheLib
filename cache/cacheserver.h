#ifndef _CACHESERVER_H_
#define _CACHESERVER_H_

#include <string>

#include <set>
using namespace std;
#include <pthread.h>
#include "DataAccess/jsonstandardcache.h"
#include "cache/autoconncache.h"

#include <boost/thread/mutex.hpp>
#include <boost/ptr_container/ptr_list.hpp>

#define MAX_CONN  2

namespace DataCache
{

    class CacheServer
    {
    public:
        CacheServer();
        ~CacheServer();

        static AutoConnCache getCacheServer();
        static void freeConnServer(JsonStandardCache* usedConn);

    private:
        static boost::ptr_list<JsonStandardCache> _freeConn;
        static boost::ptr_list<JsonStandardCache> _usedConn;
        static boost::mutex _connLock;
    public:
        static string strSerIP;
        static int intSerPort;
        static string _pass;
    };
}
#endif // _CACHE_SERVER_H_
