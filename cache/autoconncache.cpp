#include "autoconncache.h"

#include "cacheserver.h"
namespace DataCache
{
AutoConnCache::AutoConnCache()
{
    _curConn = NULL;
}
AutoConnCache::AutoConnCache(DataCache::JsonStandardCache *p) : _curConn(p)  //���캯��
{
}
DataCache::JsonStandardCache& AutoConnCache::operator *()        //����*������
{
    return *_curConn;
}
DataCache::JsonStandardCache* AutoConnCache::operator ->()       //����->������
{
    return _curConn;
}
AutoConnCache::~AutoConnCache()        //��������
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
