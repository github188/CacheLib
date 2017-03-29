#ifndef AUTOCONNCACHE_H
#define AUTOCONNCACHE_H

#include "DataAccess/jsonstandardcache.h"
namespace DataCache
{
class AutoConnCache
{
private:
    DataCache::JsonStandardCache * _curConn;
public:
    AutoConnCache();
    AutoConnCache(DataCache::JsonStandardCache *p);

    DataCache::AutoConnCache& operator =(DataCache::JsonStandardCache* curConn);
    DataCache::JsonStandardCache& operator *();
    DataCache::JsonStandardCache* operator ->();
    ~AutoConnCache();
};
}

#endif // AUTOCONNCACHE_H
