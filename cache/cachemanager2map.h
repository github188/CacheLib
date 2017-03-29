#ifndef CACHEMANAGER2MAP_H
#define CACHEMANAGER2MAP_H

#include "cachemanager.h"
#include "comm/norexception.h"

#include <map>
#include <list>
#include <string>
#include <set>
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include "maptree.h"

using namespace std;

namespace DataCache
{

template <typename K,typename T>
class CacheManager2Map : public CacheManager
{
public:
    CacheManager2Map(const string& tableName);
    bool  loadCacheAll();
    bool saveCacheAll();

    //T* getCacheSingle(const K& strSec);
    T* saveCacheSingle(const K& strFirst,const K& strSec,const T& data);
    bool delCacheSingle(const K& strFirst,const K& strSec);

    bool delCacheByFirst(const K& strFirst);

    T* setMapData(const K& strFirst,const K& strSec,const T& data);
    T* addMapData(const K& strFirst,const K& strSec);
    T* getMapData(const K& strFirst,const K& strSec);



    bool clearMapData(bool bClearTable = false);
    bool delTable();

    virtual void saveTableFlag()
    {
        CacheManager::saveTableFlag(TBLFLAG_2MAP);
    }

    template<typename FilterFun>
    bool removeDataByFunc(FilterFun filterFunc);

    const string toJson() const;
    bool fromJson(const string& strJson);

    const MapTree<2,K,T>& getMapIndex() const
    {
        return _mapTree;
    }

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
    map<K,int> _mapRef;
    MapTree<2,K,T> _mapTree;
};

template <typename K,typename T>
CacheManager2Map<K,T>::CacheManager2Map(const string& tableName) :
    CacheManager(tableName),
    _mapTree(_mapCache,_mapRef)
{
}

template <typename K,typename T>
T* CacheManager2Map<K,T>::setMapData(const K& strFirst,
                                       const K& strSec,const T& data)
{
    //_topCache.insert(strFirst);
    typename MapTree<2,K,T>::Type* firstIndex = _mapTree.addChild(strFirst);
    return firstIndex->addChild(strSec,data);
}

template <typename K,typename T>
T* CacheManager2Map<K,T>::addMapData(const K& strFirst,
                                       const K& strSec)
{
    //_topCache.insert(strFirst);
    typename MapTree<2,K,T>::Type* firstIndex = _mapTree.addChild(strFirst);
    return firstIndex->addChild(strSec);
}


template <typename K,typename T>
T* CacheManager2Map<K,T>::getMapData(const K& strFirst,const K& strSec)
{
    typename MapTree<2,K,T>::Type* firstIndex = _mapTree.getChildSingle(strFirst);
    if(firstIndex)
    {
        return firstIndex->getChildSingle(strSec);
    }
    else
    {
        return NULL;
    }
}

template <typename K,typename T>
bool CacheManager2Map<K,T>::delTable()
{
    vector<string> vecKeys;
    vecKeys.push_back(_tableName);
    vecKeys.push_back(_tableName + ":data");
    set<K> vecTop;
    DataCache::AutoConnCache conn = CacheServer::getCacheServer();
    conn->loadUniqObj(_tableName,vecTop);
    //加第二层
    if(!vecTop.empty())
    {
        typename set<K>::iterator iterTop = vecTop.begin();
        for(;iterTop != vecTop.end(); iterTop++)
        {
            K tmpKey = *iterTop;
            const string& topKey = boost::lexical_cast<string>(tmpKey);
            vecKeys.push_back(_tableName + "." + topKey);
        }
    }
    conn->delKeys(vecKeys);
    delTableFlag();
    return true;
}

template <typename K,typename T>
bool CacheManager2Map<K,T>::clearMapData(bool bClearTable)
{
   //_topCache.clear();
    if(bClearTable)
    {
        delTable();
    }

    _mapTree.clearChild();
    _mapRef.clear();
    _mapCache.clear();
   return true;
}

template <typename K,typename T>
bool  CacheManager2Map<K,T>::loadCacheAll()
{
    //加载数据
    DataCache::AutoConnCache conn = CacheServer::getCacheServer();
    if(_mapCache.size() < 1)
    {
        if(!conn->loadMapObj(_tableName + ":data",_mapCache))
        {
            return false;
        }
    }
    //加第一层
    typename MapTree<2,K,T>::MapType& topMap = _mapTree.getChild();
    if(topMap.empty())
    {

        if(!conn->loadUniqMapKey(_tableName,topMap,
                                 (typename MapTree<2,K,T>::Type*)NULL))
        {
            return false;
        }
        //加第二层
        typename MapTree<2,K,T>::MapIterator iterFir = topMap.begin();
        for(;iterFir != topMap.end(); iterFir++)
        {
            const K& topKey = iterFir->first;
            typename MapTree<2,K,T>::Type* secIndexData = _mapTree.allocChild(topKey);
            typename MapTree<1,K,T>::MapType& mapSecIndex = secIndexData->getChild();
            ostringstream osSubTable;
            osSubTable << _tableName << "." << topKey;
            if(!conn->loadUniqMapKey(osSubTable.str(),mapSecIndex,
                                     (typename MapTree<1,K,T>::Type*)NULL))
            {
                return false;
            }
            typename MapTree<1,K,T>::MapIterator iterSecIndex = mapSecIndex.begin();
            for(;iterSecIndex != mapSecIndex.end(); iterSecIndex++)
            {
                const K& secKey = iterSecIndex->first;
                T* tmpData = secIndexData->allocChild(secKey,true);
                if(!tmpData)
                {

                    ostringstream osSubTable;
                    osSubTable << "table:" << _tableName <<
                               ",key1:" << topKey <<
                               ",key2:" << secKey <<
                               " ,data find error!";
                    LOG_ERROR(osSubTable.str().c_str());
                    assert(false);
                }
            }
        }
    }
    for(typename map<K,int>::iterator iterRef = _mapRef.begin(); iterRef != _mapRef.end(); )
    {
        if(iterRef->second < 2)
        {
            _mapRef.erase(iterRef++);
        }
        else
            {
            iterRef++;
             }
    }
    return true;
}

template <typename K,typename T>
template<typename FilterFun>
bool CacheManager2Map<K,T>::removeDataByFunc(FilterFun filterFunc)
{
    DataCache::AutoConnCache conn = CacheServer::getCacheServer();

    typename MapTree<2,K,T>::MapType& mapFirstChild =  _mapTree.getChild();
    typename MapTree<2,K,T>::MapIterator iterFirIndex = mapFirstChild.begin();
    while(iterFirIndex != mapFirstChild.end())
    {
        const K  strFirst = iterFirIndex->first;
        typename MapTree<2,K,T>::Type* myIndex = iterFirIndex->second;

        typename MapTree<1,K,T>::MapType& mapSecChild =  myIndex->getChild();
        typename MapTree<1,K,T>::MapIterator iterSecIndex = mapSecChild.begin();

        for(;iterSecIndex != mapSecChild.end(); )
        {
            const K  strSec = iterSecIndex->first;
            T* secData = iterSecIndex->second;
            iterSecIndex++;
            if(filterFunc(*secData))
            {
                int ret = myIndex->delChild(strSec);
                switch(ret)
                {
                case MapTree<1,K,T>::RET_NONE:
                    break;
                case MapTree<1,K,T>::RET_DELDATA:
                    conn->removeMapField(_tableName + ":data",
                                         boost::lexical_cast<string>(strSec));
                case MapTree<1,K,T>::RET_DELINDEX:
                    conn->removeUniq(_tableName + "." + boost::lexical_cast<string>(strFirst),
                                     boost::lexical_cast<string>(strSec));
                    break;
                }
            }
        }
        iterFirIndex++;
        if(myIndex->getChild().empty())
        {
            ostringstream osSubKey;
            osSubKey << _tableName << "." << strFirst;
            conn->delKey(osSubKey.str());
            conn->removeUniq(_tableName,boost::lexical_cast<string>(strFirst));
            _mapTree.delChild(strFirst);
            continue;
        }
    }
    return true;
}

template <typename K,typename T>
 T* CacheManager2Map<K,T>::saveCacheSingle(const K& strFirst,
                                            const K& strSec,
                                            const T& data)
{
    DataCache::AutoConnCache conn =  CacheServer::getCacheServer();
    //保存基本数据
    if(!conn->saveMapObjSingle(_tableName + ":data",strSec,data))
    {
        return NULL;
    }
    //更新索引
    if(!conn->saveUniqObjSingle(_tableName,strFirst))
    {
        ostringstream osFormator;
        osFormator << "table:" << _tableName << " key1:" << strFirst << " key2:"
                   << strSec << "save first key error!";
        LOG_ERROR(osFormator.str().c_str());
    }
    if(!conn->saveUniqObjSingle(_tableName + "." +
                                boost::lexical_cast<string>(strFirst),
                                strSec))
    {
        ostringstream osFormator;
        osFormator << "table:" << _tableName << " key1:" << strFirst << " key2:"
                   << strSec << "save second key error!";
        LOG_ERROR(osFormator.str().c_str());
    }
    typename MapTree<2,K,T>::Type* secIndex = _mapTree.addChild(strFirst);
    typename MapTree<1,K,T>::Type* cacheOut = secIndex->addChild(strSec,data);
    return cacheOut;
}

  template <typename K,typename T>
 bool CacheManager2Map<K,T>::delCacheByFirst(const K& strFirst)
 {
     DataCache::AutoConnCache conn =  CacheServer::getCacheServer();
     //删索引
     typename MapTree<2,K,T>::Type* secIndex = _mapTree.getChildSingle(strFirst);
     if(!secIndex)
     {
         return true;
     }
     typename MapTree<1,K,T>::MapType& mapSec = secIndex->getChild();
     typename MapTree<1,K,T>::MapIterator iterSec = mapSec.begin();
     for(;iterSec != mapSec.end();)
     {
           const K  secKey = iterSec->first;
           //删除
           int ret = secIndex->delChildByIter(iterSec++);
           switch(ret)
           {
           case  MapTree<1,K,T>::RET_NONE:
               continue;
           case  MapTree<1,K,T>::RET_DELDATA:
               if(!conn->removeMapField(_tableName + ":data",boost::lexical_cast<string>(secKey)))
               {
                   assert(false);
                   return false;
               }
                _mapCache.erase(secKey);
           case  MapTree<1,K,T>::RET_DELINDEX:
               if(!conn->removeUniq(_tableName + "." + boost::lexical_cast<string>(strFirst),
                                    boost::lexical_cast<string>(secKey)))
               {
                   assert(false);
                   return false;
               }
               break;
           }
     }
     _mapTree.delChild(strFirst);
     if(!conn->removeUniq(_tableName,
                          boost::lexical_cast<string>(strFirst)))
     {
         assert(false);
         return false;
     }
     return true;
 }

 template <typename K,typename T>
 bool CacheManager2Map<K,T>::delCacheSingle(const K& strFirst,const K& strSec)
 {
     DataCache::AutoConnCache conn =  CacheServer::getCacheServer();

     //删索引
     typename MapTree<2,K,T>::Type* secIndex = _mapTree.getChildSingle(strFirst);
     if(secIndex)
     {
           //删除
           int ret = secIndex->delChild(strSec);
           switch(ret)
           {
           case  MapTree<1,K,T>::RET_NONE:
               return true;
           case  MapTree<1,K,T>::RET_DELDATA:
               if(!conn->removeMapField(_tableName + ":data",boost::lexical_cast<string>(strSec)))
               {
                   assert(false);
                   return false;
               }
                _mapCache.erase(strSec);
           case  MapTree<1,K,T>::RET_DELINDEX:
               if(!conn->removeUniq(_tableName + "." + boost::lexical_cast<string>(strFirst),
                                    boost::lexical_cast<string>(strSec)))
               {
                   assert(false);
                   return false;
               }
               break;
           }
           if(secIndex->getChild().empty())
           {
               _mapTree.delChild(strFirst);
               if(!conn->removeUniq(_tableName,
                                    boost::lexical_cast<string>(strFirst)))
               {
                   assert(false);
                   return false;
               }
           }
     }
     else
     {
         return true;
     }
 }

template <typename K,typename T>
bool CacheManager2Map<K,T>::saveCacheAll()
{
    //保存数据
    delTable();
    DataCache::AutoConnCache conn =  CacheServer::getCacheServer();
    if(!conn->saveMapObj(_tableName + ":data",_mapCache))
    {
        return false;
    }
    saveTableFlag();
    typename MapTree<2,K,T>::MapType& mapTreeChild =  _mapTree.getChild();
    typename MapTree<2,K,T>::MapIterator iterTop = mapTreeChild.begin();
    for(;iterTop != mapTreeChild.end(); iterTop++)
    {
        //保存二級
        typename MapTree<2,K,T>::Type* tmpSec = iterTop->second;
        typename MapTree<1,K,T>::MapType& mapSec =  tmpSec->getChild();
        typename MapTree<1,K,T>::MapIterator iterSec = mapSec.begin();
        //保存二級
        ostringstream osSubKey;
        osSubKey << _tableName << "." << iterTop->first;
        if(!conn->saveUniqMapKey(osSubKey.str(),mapSec))
        {
            return false;
        }
    }

    //保存一級
    if(!conn->saveUniqMapKey(_tableName ,mapTreeChild))
    {
        return false;
    }
/*
    typename map<K,set<K> >::iterator iterFirst = _mapIndexFirst.begin();
    for(;iterFirst != _mapIndexFirst.end(); iterFirst++)
    {
        ostringstream osSubKey;
        osSubKey << _tableName << "." << iterFirst->first;
        if(!conn->saveUniqObj(osSubKey.str(),iterFirst->second))
        {
            return false;
        }
    }
    */
    return true;
}

template <typename K,typename T>
bool CacheManager2Map<K,T>::fromJson(const string& strJson)
{
    rapidjson::Document doc;
    doc.Parse<0>(strJson.c_str());
    if (doc.HasParseError())  //解析错误
    {
        string strError = string("2map table:") + this->getTableName() + "  Parse error!";
        throw NorException(strError);
    }
    if(!doc.IsObject())
    {
        string strError = string("2map table:") + this->getTableName() + "  root must obj!";
        throw NorException(strError);
    }

    _mapTree.clearChild();
    _mapRef.clear();
    _mapCache.clear();

    string tmpKey;
    typename JsonConvertor<T>::JsonConvertFunc jsonConvertFunc;
    rapidjson::Value::MemberIterator iterTop = doc.MemberBegin();
    for(;iterTop != doc.MemberEnd(); iterTop++)
    {

        JsonContext jsonKeyContex(doc,iterTop->name);
        jsonKeyContex.toVal(tmpKey);
        K keyFirst = boost::lexical_cast<K>(tmpKey);
        typename MapTree<2,K,T>::Type* firstIndex = _mapTree.addChild(keyFirst);

        rapidjson::Value& firstVal = iterTop->value;
        rapidjson::Value::MemberIterator iterSec = firstVal.MemberBegin();
        for(;iterSec != firstVal.MemberEnd(); iterSec++)
        {
            JsonContext jsonKeyContex(doc,iterSec->name);
            jsonKeyContex.toVal(tmpKey);
            K keySec = boost::lexical_cast<K>(tmpKey);
            rapidjson::Value& secVal = iterSec->value;
            JsonContext jsonContext(doc,secVal);
             T objData;
             jsonConvertFunc.jsonToObj(objData,jsonContext);

             firstIndex->addChild(keySec,objData);
        }
    }
    return true;
}

template <typename K,typename T>
const string CacheManager2Map<K,T>::toJson() const
{
    const typename MapTree<2,K,T>::MapType&  topTree = _mapTree.getChild();
    typename MapTree<2,K,T>::ConstMapIterator iterTop = topTree.begin();
    rapidjson::Value jsonRoot(rapidjson::kObjectType);
     rapidjson::Document doc;
     for(; iterTop != topTree.end(); iterTop++)
     {
         const K& topKey = iterTop->first;
         rapidjson::Value firstJson(rapidjson::kObjectType);

         typename MapTree<2,K,T>::Type* secData = iterTop->second;
         typename MapTree<1,K,T>::MapType& mapSecIndex = secData->getChild();
         typename MapTree<1,K,T>::MapIterator iterListFirst = mapSecIndex.begin();
         for(;iterListFirst !=  mapSecIndex.end(); iterListFirst++)
         {
             const K& secKey = iterListFirst->first;
             const T* data = iterListFirst->second;
             if(!data)
             {
                 ostringstream osError;
                 osError << "表：” + " << _tableName <<"key1:" << topKey
                         << " key2:" << secKey << " 无数据";
                 throw  NorException(osError.str());
             }
             const T& secData = *data;
             rapidjson::Value valSec(rapidjson::kArrayType);
             JsonContext jsonContext(doc,valSec);
             typename JsonConvertor<T>::JsonConvertFunc jsonConvertFunc;
             jsonConvertFunc.convertToJson(secData,jsonContext);

             string secKeyVal = boost::lexical_cast<string>(secKey);

             rapidjson::Value nameSecKey;
             JsonContext contextSecKey(doc,nameSecKey);
             contextSecKey.setVal(secKeyVal);

             firstJson.AddMember(nameSecKey,valSec,doc.GetAllocator());
         }
         string strTopName = boost::lexical_cast<string>(topKey) ;

         rapidjson::Value nameTopKey;
         JsonContext contextTopKey(doc,nameTopKey);
         contextTopKey.setVal(strTopName);

         jsonRoot.AddMember(nameTopKey,firstJson,doc.GetAllocator());
     }
     JsonContext JsonContextRoot(doc,jsonRoot);
     return JsonContextRoot.getString();
}
}
#endif // CACHEMANAGER2MAP_H
