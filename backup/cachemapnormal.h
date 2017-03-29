#ifndef CACHEMAPNORMAL_H
#define CACHEMAPNORMAL_H

#include "cache/cachemanager.h"
#include "rapidjson/document.h"

template<typename KEY>
class CacheMapNormal
{
public:
    CacheMapNormal(){}

    bool readCache(const string& table,string& strContent);
    bool cacheToRedis(const string& table,const string& strContent);
};

template<typename KEY>
bool CacheMapNormal<KEY>::readCache(const string& table,string& strContent)
{
    strContent = "";
    DataCache::CacheManagerMap<KEY,string> cacheTmp(table);
    if(!cacheTmp.loadCache())
    {
        return false;
    }

    rapidjson::Document doc;
    rapidjson::Value root(rapidjson::kObjectType);
    const map<KEY,string>& tmpMap = cacheTmp.getCache();
    typename map<KEY,string>::const_iterator iterTmp = tmpMap.begin();
    for(;iterTmp != tmpMap.end(); iterTmp++)
    {
        const KEY& tmpKey = iterTmp->first;
        const string& item = iterTmp->second;
        doc.Parse<0>(item.c_str());
        if (doc.HasParseError())  //½âÎö´íÎó
        {
            string strError = string("table:") + table  + " backup Parse item error!";
            printf(strError.c_str());
            LOG_ERROR(strError.c_str());
            throw NorException(strError);
            return false;
        }

        rapidjson::Value keyVal;
        JsonContext contextKey(doc,keyVal);
        contextKey.setVal(boost::lexical_cast<string>(tmpKey));
        root.AddMember(keyVal,doc,doc.GetAllocator());
    }
    JsonContext contextRoot(doc,root);
    strContent = contextRoot.getString();
    return true;
}

template<typename KEY>
bool CacheMapNormal<KEY>::cacheToRedis(const string& table,const string& strContent)
{
    DataCache::CacheManagerMap<KEY,string> cacheTmp(table);
    cacheTmp.fromJson(strContent);
    cacheTmp.saveCache();
    return true;
}

#endif // CACHEMAPNORMAL_H
