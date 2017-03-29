#ifndef CACHE2MAPNORMAL_H
#define CACHE2MAPNORMAL_H

#include "cache/cachemanager2map.h"

template<typename KEY>
class Cache2MapNormal
{
public:
    Cache2MapNormal(){}

    bool readCache(const string& table,string& strContent)
    {
        strContent = "";
        DataCache::CacheManager2Map<KEY,string> cacheTmp(table);
        if(!cacheTmp.loadCacheAll())
        {
            return false;
        }

        rapidjson::Document doc;
        rapidjson::Value root(rapidjson::kObjectType);
        const map<KEY,string>& tmpMap = cacheTmp.getCache();
        const MapTree<2,KEY,string>& firstKeyIndex = cacheTmp.getMapIndex();
        if(tmpMap.empty())
        {
            return false;
        }
        const typename MapTree<2,KEY,string>::MapType mapFirstIndex = firstKeyIndex.getChild();
        typename MapTree<2,KEY,string>::ConstMapIterator iterIndex = mapFirstIndex.begin();
        for(;iterIndex != mapFirstIndex.end(); iterIndex++)
        {
            //第一个key
            const KEY& firstKey = iterIndex->first;
            string strFirKey = boost::lexical_cast<string>(firstKey);
            rapidjson::Value valFistKey;
            JsonContext contextFirstKey(doc,valFistKey);
            contextFirstKey.setVal(strFirKey);

            if(!root.HasMember(valFistKey))
            {
                rapidjson::Value tmpSecKey(rapidjson::kObjectType);
                root.AddMember(valFistKey,tmpSecKey,doc.GetAllocator());
            }
            rapidjson::Value& firstKeyRef = root[strFirKey.c_str()];

            const typename MapTree<2,KEY,string>::Type* tmpSecIndex = iterIndex->second;
            const typename MapTree<1,KEY,string>::MapType& mapSecIndex = tmpSecIndex->getChild();
            typename MapTree<1,KEY,string>::ConstMapIterator iterSecKey = mapSecIndex.begin();
            for(;iterSecKey != mapSecIndex.end(); iterSecKey++)
            {
                //第二个key
                const KEY& secKey = iterSecKey->first;
                rapidjson::Value keySecVal;
                JsonContext contextSecKey(doc,keySecVal);
                contextSecKey.setVal(boost::lexical_cast<string>(secKey));

                const string* data = iterSecKey->second;


                const string& item = *data;
                doc.Parse<0>(item.c_str());
                if (doc.HasParseError())  //解析错误
                {
                    string strError = string("table:") + table  + " backup Parse item error!";
                    printf(strError.c_str());
                    LOG_ERROR(strError.c_str());
                    throw NorException(strError);
                    return false;
                }
                firstKeyRef.AddMember(keySecVal,doc,doc.GetAllocator());
            }
        }
        JsonContext contextRoot(doc,root);
        strContent = contextRoot.getString();
        return true;
    }

    bool cacheToRedis(const string& table,const string& strContent)
    {
        DataCache::CacheManager2Map<KEY,string> cacheTmp(table);
        cacheTmp.fromJson(strContent);
        cacheTmp.saveCacheAll();
        return true;
    }
};

#endif // CACHE2MAPNORMAL_H
