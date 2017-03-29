#include "cacheserver.h"



#include "DataAccess/jsonstandardcache.h"
#include <algorithm>

namespace DataCache
{
string CacheServer::strSerIP = "127.0.0.1";

int CacheServer::intSerPort = 6379;
string CacheServer::_pass = "";

boost::ptr_list<JsonStandardCache> CacheServer::_freeConn;
boost::ptr_list<JsonStandardCache> CacheServer::_usedConn;
boost::mutex CacheServer::_connLock;

AutoConnCache CacheServer::getCacheServer()
{

    JsonStandardCache* curConn;
    _connLock.lock();
    if(_freeConn.size() > 0)
    {
        boost::ptr_list<JsonStandardCache>::iterator iterConn = _freeConn.begin();
        boost::ptr_list<JsonStandardCache>::auto_type tmpAutoConn =  _freeConn.release(iterConn);
        curConn = tmpAutoConn.get();
        _usedConn.push_back(tmpAutoConn.release());
    }
    else
    {
        curConn = new JsonStandardCache(CacheServer::strSerIP,
                                                    CacheServer::intSerPort,CacheServer::_pass);
        _usedConn.push_back(curConn);
    }
    _connLock.unlock();

    return curConn;
}
void CacheServer::freeConnServer(JsonStandardCache* useConn)
{
    _connLock.lock();
    boost::ptr_list<JsonStandardCache>::iterator iterConn = _usedConn.begin();
    for(;iterConn != _usedConn.end(); )
    {
        JsonStandardCache& tmpConn = *iterConn;
        if(&tmpConn == useConn)
        {
            boost::ptr_list<JsonStandardCache>::auto_type tmpAutoConn =  _usedConn.release(iterConn++);
            if(_freeConn.size() < MAX_CONN)
            {
                _freeConn.push_back(tmpAutoConn.release());
            }
        }
        else
        {
            ++iterConn;
        }
    }
    _connLock.unlock();

}

CacheServer::CacheServer()
{
}
CacheServer::~CacheServer()
{
}
}
