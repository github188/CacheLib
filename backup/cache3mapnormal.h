#ifndef CACHE3MAPNORMAL_H
#define CACHE3MAPNORMAL_H

#include "cache/cachemanager3map.h"

template<typename KEY>
class Cache3MapNormal
{
public:
    Cache3MapNormal(){}

    bool readCache(const string& table,string& strContent)
    {
        strContent = "";
        DataCache::CacheManager3Map<KEY,string> cacheTmp(table);
        if(!cacheTmp.loadCacheAll())
        {
            return false;
        }
        rapidjson::Document doc;
        rapidjson::Value root(rapidjson::kObjectType);
        const map<KEY,string>& tmpMap = cacheTmp.getCache();
        if(tmpMap.empty())
        {
            return false;
        }
        const MapTree<3,KEY,string>& firstKeyIndex = cacheTmp.getMapIndex();
        const typename MapTree<3,KEY,string>::MapType& mapFirIndex =  firstKeyIndex.getChild();
        typename MapTree<3,KEY,string>::ConstMapIterator iterFirIndex = mapFirIndex.begin();
        for(;iterFirIndex != mapFirIndex.end(); iterFirIndex++)
        {
            //第一个key
            const KEY& firstKey = iterFirIndex->first;
            string strFirstKey = boost::lexical_cast<string>(firstKey);
            rapidjson::Value valFistKey;
            JsonContext contextFirstKey(doc,valFistKey);
            contextFirstKey.setVal(strFirstKey);

            if(!root.HasMember(valFistKey))
            {
                rapidjson::Value tmpSecKey(rapidjson::kObjectType);
                root.AddMember(valFistKey,tmpSecKey,doc.GetAllocator());
            }
            rapidjson::Value& firstKeyRef = root[strFirstKey.c_str()];

            const typename MapTree<3,KEY,string>::Type* secIndex = iterFirIndex->second;
            const typename MapTree<2,KEY,string>::MapType& mapSecIndex =  secIndex->getChild();
            typename MapTree<2,KEY,string>::ConstMapIterator iterSecIndex = mapSecIndex.begin();
            for(;iterSecIndex != mapSecIndex.end(); iterSecIndex++)
            {
                //第二个key
                const KEY& secKey = iterSecIndex->first;
                rapidjson::Value keySecVal;
                string strSecKey = boost::lexical_cast<string>(secKey);
                JsonContext contextSecKey(doc,keySecVal);
                contextSecKey.setVal(strSecKey);

                if(!firstKeyRef.HasMember(keySecVal))
                {
                    rapidjson::Value tmpThirKey(rapidjson::kObjectType);
                    firstKeyRef.AddMember(keySecVal,tmpThirKey,doc.GetAllocator());
                }
                rapidjson::Value& secKeyRef = firstKeyRef[strSecKey.c_str()];

                const typename MapTree<2,KEY,string>::Type* thirIndex = iterSecIndex->second;
                const typename MapTree<1,KEY,string>::MapType& mapThirIndex =  thirIndex->getChild();
                typename MapTree<1,KEY,string>::ConstMapIterator iterThirIndex = mapThirIndex.begin();

                for(;iterThirIndex != mapThirIndex.end(); iterThirIndex++)
                {
                    //第三个key
                    const KEY& thirKey = iterThirIndex->first;
                    rapidjson::Value keyThirVal;
                    JsonContext contextThirKey(doc,keyThirVal);
                    contextThirKey.setVal(boost::lexical_cast<string>(thirKey));

                    const string& item = *(iterThirIndex->second);
                    doc.Parse<0>(item.c_str());
                    if (doc.HasParseError())  //解析错误
                    {
                        string strError = string("table:") + table  + " backup Parse item error!";
                        printf(strError.c_str());
                        LOG_ERROR(strError.c_str());
                        throw NorException(strError);
                        return false;
                    }
                    secKeyRef.AddMember(keyThirVal,doc,doc.GetAllocator());
                }
            }
        }
        JsonContext contextRoot(doc,root);
        strContent = contextRoot.getString();
        return true;
    }

    bool cacheToRedis(const string& table,const string& strContent)
    {
        DataCache::CacheManager3Map<KEY,string> cacheTmp(table);
        cacheTmp.fromJson(strContent);
        cacheTmp.saveCacheAll();
        return true;
    }
};

#endif // CACHE3MAPNORMAL_H
