#include "jsonstandardcache.h"

#include "../redis/redis.h"

namespace DataCache
{
JsonStandardCache::JsonStandardCache(const std::string& server,const int port,const string& pass):
    CacheBase(server,port,pass)
{
}
}
