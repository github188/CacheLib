#ifndef CACHEMANAGEROBJ_H
#define CACHEMANAGEROBJ_H


#include "cache/cacheserver.h"
#include "DataAccess/jsonstandardcache.h"
#include "cachemanager.h"


namespace DataCache
{

template <typename T>
class CacheManagerObj : public CacheManager
{
public:
    CacheManagerObj(const string& tableName) : CacheManager(tableName)
    {
    }
    bool saveCache(const T& objCache);
    T* loadCache();
    bool saveCache();

    virtual bool delTable();
    virtual bool clearMapData(bool bClearTable=false);
    virtual void saveTableFlag()
    {
        CacheManager::saveTableFlag(TBLFLAG_OBJ);
    }

    virtual const string toJson() const;
    virtual bool fromJson(const string& strJson);

    const T& getCache() const
    {
        return _objCache;
    }
private:
    T _objCache;
};

template <typename T>
bool CacheManagerObj<T>::delTable()
{
    if(!CacheServer::getCacheServer()->delKey(_tableName))
    {
        return false;
    }
    delTableFlag();
    return true;
}

template <typename T>
bool CacheManagerObj<T>::clearMapData(bool bClearTable)
{
    if(bClearTable)
    {
        delTable();
    }
    _objCache.clear();
    return true;
}

template <typename T>
bool CacheManagerObj<T>::saveCache(const T& objCache)
{
    DataCache::AutoConnCache conn = CacheServer::getCacheServer();
    if(!conn->saveObj(_tableName,objCache))
    {
        return false;
    }
    saveTableFlag();
    if(&_objCache != &objCache)
    {
       _objCache = objCache;
    }
    return true;
}

template <typename T>
T* CacheManagerObj<T>::loadCache()
{
    DataCache::AutoConnCache conn = CacheServer::getCacheServer();
    if(!conn->loadObj(_tableName,_objCache))
    {
        return NULL;
    }

    return &_objCache;
}

template <typename T>
const string CacheManagerObj<T>::toJson() const
{
    rapidjson::Document doc;
    rapidjson::Value valJson(rapidjson::kArrayType);
    JsonContext jsonContextData(doc,valJson);
    typename JsonConvertor<T>::JsonConvertFunc jsonConvertFunc;
    jsonConvertFunc.convertToJson(_objCache,jsonContextData);
    return jsonContextData.getString();
}
template <typename T>
bool CacheManagerObj<T>::fromJson(const string& strJson)
{
    rapidjson::Document doc;
    doc.Parse<0>(strJson.c_str());
    if (doc.HasParseError())  //½âÎö´íÎó
    {
        string strError = string("obj table:") + this->getTableName() + "  Parse error!";
        throw NorException(strError);
    }
    if(doc.IsObject())
    {
        string strError = string("obj table:") + this->getTableName() + "  root must not obj!";
        throw NorException(strError);
    }
    typename JsonConvertor<T>::JsonConvertFunc jsonConvertFunc;

    JsonContext jsonContext(doc,doc);
    return jsonConvertFunc.jsonToObj(_objCache,jsonContext);
}

template <typename T>
bool CacheManagerObj<T>::saveCache()
{
    DataCache::AutoConnCache conn = CacheServer::getCacheServer();
    if(!conn->saveObj(_tableName,_objCache))
    {
        return false;
    }
    saveTableFlag();
    return true;
}
}

#endif // CACHEMANAGEROBJ_H
