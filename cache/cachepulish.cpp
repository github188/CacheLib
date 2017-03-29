#include "cachepulish.h"
#include "cache/cacheserver.h"
namespace DataCache
{
CachePulish::CachePulish()
{
}
bool CachePulish::publish(const string& channel,const string& msg)
{
    return DataCache::CacheServer::getCacheServer()->publish(channel,msg);
}
}
