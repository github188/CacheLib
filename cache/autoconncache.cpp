#include "autoconncache.h"

#include "cacheserver.h"
namespace DataCache
{
AutoConnCache::AutoConnCache()
{
    _curConn = NULL;
}
AutoConnCache::AutoConnCache(DataCache::JsonStandardCache *p) : _curConn(p)  //构造函数
{
}
DataCache::JsonStandardCache& AutoConnCache::operator *()        //重载*操作符
{
    return *_curConn;
}
DataCache::JsonStandardCache* AutoConnCache::operator ->()       //重载->操作符
{
    return _curConn;
}
AutoConnCache::~AutoConnCache()        //析构函数
{
    if(_curConn)
    {
        CacheServer::freeConnServer(_curConn);
    }
}
DataCache::AutoConnCache& AutoConnCache::operator =(DataCache::JsonStandardCache* curConn)
{
    if(curConn != _curConn)
    {
        if(_curConn)
        {
             CacheServer::freeConnServer(_curConn);
        }
       _curConn = curConn;
    }
    return *this;
}
}
