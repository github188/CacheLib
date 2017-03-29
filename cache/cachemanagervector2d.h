#ifndef CACHEMANAGERVECTOR2D_H
#define CACHEMANAGERVECTOR2D_H

#include "cache/cacheserver.h"
#include "DataAccess/jsonstandardcache.h"
#include "cachemanager.h"
#include "comm/vector2d.h"

namespace DataCache
{
#define MAX_EACH  20
//由于矩陣数据太大,所以以MAX_EACH为大小分着存儲

template<typename T>
class CacheManagerVector2d : public CacheManager
{
public:
    CacheManagerVector2d(const string& tableName,vector2d<T>& vector2dCache) :
        CacheManager(tableName),
        _vector2dCache(vector2dCache)
    {
    }
    bool saveCache(const T& objCache);
    bool loadCache(bool bTri = true);
    bool saveCache(bool bTri = true);

    virtual bool delTable();
    virtual bool clearMapData(bool bClearTable=false);
    virtual void saveTableFlag()
    {
        CacheManager::saveTableFlag(TBLFLAG_VECTOR2D);
    }

    virtual const string toJson() const;
    virtual bool fromJson(const string& strJson);

    const vector2d<T>& getCache() const
    {
        return _vector2dCache;
    }
private:
    vector2d<T>& _vector2dCache;
};

template <typename T>
bool CacheManagerVector2d<T>::delTable()
{
    if(!CacheServer::getCacheServer()->delKey(_tableName))
    {
        return false;
    }
    delTableFlag();
    return true;
}

template <typename T>
bool CacheManagerVector2d<T>::clearMapData(bool bClearTable)
{
    if(bClearTable)
    {
        delTable();
    }
    _vector2dCache.resize(0,0);
    return true;
}

/*
template <typename T>
bool CacheManagerVector2d<T>::saveCache(const vector2d<T>& vec2dCache)
{
    DataCache::AutoConnCache conn = CacheServer::getCacheServer();
    if(!conn->saveVector2d(_tableName,vec2dCache))
    {
        return false;
    }
    saveTableFlag();
    if(&_objCache != &objCache)
    {
       _vector2dCache = vec2dCache;
    }
    return true;
}*/

template <typename T>
bool CacheManagerVector2d<T>::loadCache(bool bTri)
{
    DataCache::AutoConnCache conn = CacheServer::getCacheServer();
    if(bTri)
    {
        if(!conn->loadTriVector2d(_tableName,_vector2dCache))
        {
            return false;
        }
    }
    else
    {
        if(!conn->loadVector2d(_tableName,_vector2dCache))
        {
            return false;
        }
    }

    return true;
}

template <typename T>
const string CacheManagerVector2d<T>::toJson() const
{
    rapidjson::Document doc;
    rapidjson::Value valJson(rapidjson::kArrayType);
    string strSize = boost::lexical_cast<string>(_vector2dCache.getRow()) + "," +
            boost::lexical_cast<string>(_vector2dCache.getCol());
    rapidjson::Value valSize;
    valSize.SetString(strSize.c_str(),static_cast<size_t>(strSize.size()), doc.GetAllocator());
    valJson.PushBack(valSize,doc.GetAllocator());
    for(uint x = 0; x < _vector2dCache.getRow(); x++)
    {
        for(uint y = 0; y < _vector2dCache.getCol(); y++)
        {
            rapidjson::Value valItem(rapidjson::kArrayType);
            JsonContext jsonContextItem(doc,valItem);
            typename JsonConvertor<T>::JsonConvertFunc jsonConvertFunc;
            jsonConvertFunc.convertToJson(_vector2dCache(x,y),jsonContextItem);
            valJson.PushBack(valItem,doc.GetAllocator());
        }
    }
    JsonContext jsonContextData(doc,valJson);
    return jsonContextData.getString();
}
template <typename T>
bool CacheManagerVector2d<T>::fromJson(const string& strJson)
{
    rapidjson::Document doc;
    doc.Parse<0>(strJson.c_str());
    if (doc.HasParseError())  //解析错误
    {
        string strError = string("obj table:") + this->getTableName() + "  Parse error!";
        throw NorException(strError);
    }
    if(!doc.IsArray())
    {
        string strError = string("vector2d table:") + this->getTableName() + "  root must array!";
        throw NorException(strError);
    }

    typename JsonConvertor<T>::JsonConvertFunc convertFunc;
    JsonContext jsonContexSize(doc,doc[0]);
    string strSize = jsonContexSize.getString();
    string::size_type szPos = strSize.find(",");
    int x = atoi(strSize.c_str());
    int y = atoi(strSize.substr(szPos+1).c_str());
    _vector2dCache.resize(x,y);
    int size = doc.Size();
    assert(x * y + 1 == size);
    if(x * y + 1 != size)
    {
        string strError = string("vector2d table:") + this->getTableName() +
                "  read json size must match!";
        throw NorException(strError);
    }

    for(int i = 1,x = 0,y = 0; i < size; i++)
    {
        JsonContext jsonContex(doc,doc[i]);
        T obj;
        convertFunc.jsonToObj(obj,jsonContex);
        _vector2dCache(x,y++) = obj;
        int size = _vector2dCache.getCol();
        if(y >= size)
        {
            x ++;
            y = 0;
        }
    }
     return true;
}

template <typename T>
bool CacheManagerVector2d<T>::saveCache(bool bTri)
{
    DataCache::AutoConnCache conn = CacheServer::getCacheServer();
    if(bTri)
    {
        if(!conn->saveTriVector2d(_tableName,_vector2dCache))
        {
            return false;
        }
    }
    else
    {
        if(!conn->saveVector2d(_tableName,_vector2dCache))
        {
            return false;
        }
    }
    saveTableFlag();
    return true;
}
}

#endif // CACHEMANAGERVECTOR2D_H
