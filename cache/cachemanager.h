#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H

#include <string>
#include <map>
#include <list>
#include <vector>
#include <sstream>
#include <string.h>
using namespace std;

#include "cacheserver.h"
#include "DataAccess/jsonstandardcache.h"

#define TBL_TABLEFLAG "tableflag"
namespace DataCache
{

class CacheManager
{
public:
    enum FlagTable
    {
        TBLFLAG_LIST = 1,
        TBLFLAG_OBJ,
        TBLFLAG_MAP,
        TBLFLAG_2MAP,
        TBLFLAG_3MAP,
        TBLFLAG_VECTOR2D
    };
    CacheManager(const string tableName) : _tableName(tableName)
    {
    }
    virtual ~CacheManager()
    {
    }

    virtual bool delTable() = 0;
    virtual bool clearMapData(bool bClearTable=false) = 0;
    virtual const string toJson() const = 0;
    virtual bool fromJson(const string& strJson) = 0;
    virtual void saveTableFlag() = 0;

    const string& getTableName() const
    {
        return _tableName;
    }
    void setTableName(const string& tableName,bool bClear = true)
    {
        if (_tableName == tableName) {
            return;
        }
        _tableName = tableName;
        if(bClear)
        {
            clearMapData(false);
        }
    }



    void delTableFlag()
    {
        DataCache::CacheServer::getCacheServer()->removeMapField(TBL_TABLEFLAG,_tableName);
    }

    int loadTableFlag()
    {
        int flag;
        DataCache::CacheServer::getCacheServer()->loadMapObjSingle(TBL_TABLEFLAG,_tableName,flag);
        return flag;
    }

private:


protected:
    void saveTableFlag(FlagTable flag)
    {
        DataCache::CacheServer::getCacheServer()->saveMapObjSingle(TBL_TABLEFLAG,_tableName,flag);
    }

    string _tableName;
};


template <typename T, template<typename T,class Alloc = allocator<T> > class C>
class CacheManagerList : public CacheManager
{
public:
    CacheManagerList(const string& tableName);


    T* addCache(const T& cache);
    bool saveCacheSingle(int index,const T& cache);
    bool delCacheSingle(const int& index);

    bool saveCache(const C<T>& vecCache,bool bAdd = false);
    C<T>* loadCache();
    bool saveCache(bool bAdd = false);

    bool delTable();
    bool clearMapData(bool bClearTable=false);

    const string toJson() const;
    bool fromJson(const std::string &strJson);

    virtual void saveTableFlag()
    {
        CacheManager::CacheManager::saveTableFlag(TBLFLAG_LIST);
    }

    const C<T>& getCache() const
    {
        return _vecCache;
    }
    C<T>& getCache()
    {
        return _vecCache;
    }
private:
    C<T> _vecCache;

    void delData(vector<T>& vecT,int index)
    {
        vecT.erase(vecT.begin() + index);
    }
    void delData(list<T>& vecT,int index)
    {
        typename list<T>::iterator iter = vecT.begin();
        for(int i = 0;i < index; i++)
        {
            iter++;
        }
        vecT.erase(iter);
    }

    T& getData(vector<T>& vecT,int index)
    {
        return vecT[index];
    }

    T& getData(list<T>& vecT,int index)
    {
        typename list<T>::iterator iter = vecT.begin();
        for(int i = 0;i < index; i++)
        {
            iter++;
        }
        return *iter;
    }
};

template <typename T, template<typename T,class Alloc = allocator<T> > class C>
CacheManagerList<T,C>::CacheManagerList(const string& tableName):CacheManager(tableName)
{
}

template <typename T, template<typename T,class Alloc = allocator<T> > class C>
bool CacheManagerList<T,C>::saveCache(const C<T>& vecCache,bool bAdd)
{
    if(!CacheServer::getCacheServer()->saveListObj(_tableName,vecCache,bAdd))
    {
        return false;
    }
    saveTableFlag();
    if(&vecCache != &_vecCache)
    {
       _vecCache = vecCache;
    }
    return true;
}

template <typename T, template<typename T,class Alloc = allocator<T> > class C>
bool CacheManagerList<T,C>::clearMapData(bool bClearTable)
{
    if(bClearTable)
    {
        delTable();
    }
    _vecCache.clear();
    return true;
}

template <typename T, template<typename T,class Alloc = allocator<T> > class C>
bool CacheManagerList<T,C>::delTable()
{
    if(!CacheServer::getCacheServer()->delKey(_tableName))
    {
        return false;
    }
    delTableFlag();
    return true;
}

template <typename T, template<typename T,class Alloc = allocator<T> > class C>
bool CacheManagerList<T,C>::fromJson(const std::string &strJson)
{
    return JsonSerial::deSerialObjList(_vecCache,strJson);
}

template <typename T, template<typename T,class Alloc = allocator<T> > class C>
const string CacheManagerList<T,C>::toJson() const
{
    string strJson;
    JsonSerial::serialObjList(strJson,_vecCache);
    return strJson;
}

template <typename T, template<typename T,class Alloc = allocator<T> > class C>
bool CacheManagerList<T,C>::delCacheSingle(const int& index)
{
    if(index >= _vecCache.size())
    {
        return false;
    }
    if(!CacheServer::getCacheServer()->removeList(_tableName,index))
    {
        return false;
    }
    delData(_vecCache,index);
    /*typename C<T>::iterator iter = _vecCache.begin();
    for(int i = 0;i < index; i++)
    {
        iter++;
    }
    _vecCache.ease(iter);*/
    return true;
}

template <typename T, template<typename T,class Alloc = allocator<T> > class C>
bool CacheManagerList<T,C>::saveCacheSingle(int index,const T& cache)
{
    if(index >= _vecCache.size())
    {
        return false;
    }
    if(!CacheServer::getCacheServer()->saveListObjSingle(_tableName,index,cache))
    {
        return false;
    }
    T& tmp = getData(_vecCache,index);
    if(&cache != &tmp)
    {
        tmp = cache;
    }
    return true;
}

template <typename T, template<typename T,class Alloc = allocator<T> > class C>
T* CacheManagerList<T,C>::addCache(const T& cache)
{
    if(!CacheServer::getCacheServer()->addListObjSingle(_tableName,cache))
    {
        return NULL;
    }
    _vecCache.push_back(cache);
    typename C<T>::reverse_iterator iterCache = _vecCache.rbegin();
    T& tmpCache = *iterCache;
    return &tmpCache;
}

template <typename T, template<typename T,class Alloc = allocator<T> > class C>
bool CacheManagerList<T,C>::saveCache(bool bAdd)
{
    if(!CacheServer::getCacheServer()->saveListObj(_tableName,_vecCache,bAdd))
    {
        return false;
    }
    saveTableFlag();
    return true;
}

template <typename T, template<typename T,class Alloc = allocator<T> > class C>
C<T>* CacheManagerList<T,C>::loadCache()
{
    if(_vecCache.size() < 1)
    {
        if(!CacheServer::getCacheServer()->loadListObj(_tableName,_vecCache))
        {
            return NULL;
        }
    }
    return &_vecCache;
}

template <typename K, typename T>
class CacheManagerMap : public CacheManager
{
public:
    CacheManagerMap(const string& tableName);

    //保存mapCache到緩存
    bool saveCache(const map<K,T>& mapCache);
    map<K,T>* loadCache();
    //保存当前数据到緩存
    bool saveCache();
    bool clearMapData(bool bClearTable=false);
    bool delTable();

    virtual void saveTableFlag()
    {
        CacheManager::saveTableFlag(TBLFLAG_MAP);
    }

    //保存某个域的值
    T* setCacheSingle(const K&strField,const T& caData);
    T* getCacheSingle(const K&strField);
    bool delCacheSingle(const K& field);


    void setMapData(const K& key,const T& data);
    T* getMapData(const K& key);

    const string toJson() const;
    bool fromJson(const std::string &strJson);

    const map<K,T>& getCache() const
    {
        return _mapCache;
    }
    map<K,T>& getCache()
    {
        return _mapCache;
    }

private:
    map<K,T> _mapCache;
};

template <typename K,typename T>
CacheManagerMap<K,T>::CacheManagerMap(const string& tableName):CacheManager(tableName)
{
}

template <typename K,typename T>
bool CacheManagerMap<K,T>::saveCache(const map<K,T>& mapCache)
{
    DataCache::AutoConnCache conn = CacheServer::getCacheServer();
    if(!conn->saveMapObj(_tableName,mapCache))
    {
        return false;
    }
    saveTableFlag();
    if(&_mapCache != &mapCache)
    {
        _mapCache = mapCache;
    }
    return true;
}

template <typename K,typename T>
map<K,T>* CacheManagerMap<K,T>::loadCache()
{  
    if(_mapCache.size() < 1)
    {
        if(!CacheServer::getCacheServer()->loadMapObj(_tableName,_mapCache))
        {
            return NULL;
        }
    }
    return &_mapCache;
}

template <typename K,typename T>
T* CacheManagerMap<K,T>::setCacheSingle(const K&strField,const T& caData)
{
    if(!CacheServer::getCacheServer()->saveMapObjSingle(_tableName,strField,caData))
    {
        return NULL;
    }
    T& tmpCache = _mapCache[strField];
    tmpCache = caData;
    return &tmpCache;
}

template <typename K,typename T>
 bool CacheManagerMap<K,T>::delCacheSingle(const K& field)
 {
     if(!CacheServer::getCacheServer()->removeMapField(_tableName,
                                                       boost::lexical_cast<string>(field)))
     {
         return false;
     }
     _mapCache.erase(field);
     return true;
 }

template <typename K,typename T>
T* CacheManagerMap<K,T>::getCacheSingle(const K&strField)
{
    T tmpData;
    if(!CacheServer::getCacheServer()->loadMapObjSingle(_tableName,strField,tmpData))
    {
        return NULL;
    }
    T& sinCache = _mapCache[strField];
    sinCache= tmpData;
    return &sinCache;
}

template <typename K,typename T>
bool CacheManagerMap<K,T>::clearMapData(bool bClearTable)
{
    if(bClearTable)
    {
        delTable();
    }
    _mapCache.clear();
    return true;
}

template <typename K,typename T>
bool  CacheManagerMap<K,T>::delTable()
{
    if(!CacheServer::getCacheServer()->delKey(_tableName))
    {
        return false;
    }
    delTableFlag();
    return true;
}

template <typename K,typename T>
void  CacheManagerMap<K,T>::setMapData(const K& key,const T& data)
{
    _mapCache[key] = data;
}

template <typename K,typename T>
T* CacheManagerMap<K,T>::getMapData(const K& key)
{
    typename map<K,T>::iterator iter = _mapCache.find(key);
    if(iter != _mapCache.end())
    {
        T& tmpData = iter->second;
        return &tmpData;
    }
    else
    {
        return NULL;
    }
}

template <typename K,typename T>
bool CacheManagerMap<K,T>::fromJson(const std::string &strJson)
{
    JsonSerial::deSerialObjMap(_mapCache,strJson);
    return true;
    /*
    Json::Reader reader;
    Json::Value root;
    if (reader.parse(strJson, root)) {

        if(root.isObject()) { //json顶层是{}
            std::vector<std::string> Members = root.getMemberNames();
            for(std::vector<std::string>::iterator it = Members.begin(); it != Members.end(); ++it)
            {
                std::string member = *it;
                _mapCache[member].fromJson(root[member]);
            }
        }
        else
        {
            return false;
        }
    } else {
        return false;
    }*/
}

template <typename K,typename T>
const string CacheManagerMap<K,T>::toJson() const
{
    string strJson;
    JsonSerial::serialObjMap(strJson,_mapCache);
    return strJson;
}

template <typename K,typename T>
bool CacheManagerMap<K,T>::saveCache()
{
    if(!CacheServer::getCacheServer()->saveMapObj(_tableName,_mapCache))
    {
        return false;
    }
    saveTableFlag();
    return true;
}

}
#endif // CACHEMANAGER_H
