#ifndef CACHEMANAGER3MAP_H
#define CACHEMANAGER3MAP_H

#include "cachemanager.h"
#include "comm/norexception.h"

#include <map>
#include <list>
#include <string>
#include <set>
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <sstream>
#include "maptree.h"
#include "comm/paratwo.h"


namespace DataCache
{
template <typename K,typename T>
class CacheManager3Map : public CacheManager
{
public:

    //typedef bool (*FilterFun)(const T& item);

    CacheManager3Map(const string& tableName);

    bool  loadCacheAll();
    bool saveCacheAll();

    T* saveCacheSingle(const K& strFirst,const K& strSec,
                       const K& strThird,const T& data);
    bool delCacheSingle(const K& strFirst,const K& strSec,
                        const K& strThird);

    bool delCacheByFirst(const K& strFirst);
    bool delCacheByTwo(const K& strFirst,const K& strSec);

    void setMapData(const K& strFirst,const K& strSec,const K& strThird,const T& data);
    T* addMapData(const K& strFirst,const K& strSec,const K& strThird);
    T* getMapData(const K& strFirst,const K& strSec,const K& strThird);
    bool clearMapData(bool bClearTable=false);
    void removeMapData(const K& strFirst,const K& strSec,const K& strThird);


    bool delTable();

    virtual void saveTableFlag()
    {
        CacheManager::saveTableFlag(TBLFLAG_3MAP);
    }

    const string toJson() const;
    bool fromJson(const string& strJson);

    template<typename FilterFun>
    bool removeDataByFunc(FilterFun filterFunc);


    /*list<string>& getTopCache()
    {
        return _topCache;
    }
    const list<string>& getTopCache() const
    {
        return _topCache;
    }*/

    MapTree<3,K,T>& getMapIndex()
    {
        return _mapTree;
    }
    const MapTree<3,K,T>& getMapIndex() const
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
    void getCacheBy2Prefix(const K& first,const K& sec,map<K,T*>& retMap);
    void getCacheBy1Prefix(const K& first,map<K,T*>& retMap);

private:
    //void second2Json(const K& strKey1,const K& strKey2,JsonContext& firstJson) const;
private:
    //set<string> _topCache;
    map<K,T> _mapCache;
    map<K,int> _mapRef;
    MapTree<3,K,T> _mapTree;

    /*map<K,set<K> > _mapIndexFirst;
    map<ParaTwo<K>,set<K> > _mapIndexSecond;
    map<K,T> _mapCache;*/
};

template <typename K,typename T>
CacheManager3Map<K,T>::CacheManager3Map(const string& tableName) :
    CacheManager(tableName),
    _mapTree(_mapCache,_mapRef)
{
}


template <typename K,typename T>
template<typename FilterFun>
bool CacheManager3Map<K,T>::removeDataByFunc(FilterFun filterFunc)
{
    DataCache::AutoConnCache conn = CacheServer::getCacheServer();
    //删索引
    typename MapTree<3,K,T>::MapType& firstIndex =  _mapTree.getChild();
    typename MapTree<3,K,T>::MapIterator firstIter = firstIndex.begin();
    for(;firstIter != firstIndex.end();)
    {
        const K paraKeyFirst = firstIter->first;
        typename MapTree<3,K,T>::Type* secIndex = firstIter->second;

        typename MapTree<2,K,T>::MapType& mapSecIndex = secIndex->getChild();
        typename MapTree<2,K,T>::MapIterator iterSecIndex = mapSecIndex.begin();
        bool bRmFirst = false;
        for(;iterSecIndex != mapSecIndex.end();)
        {
            const K paraKeySec = iterSecIndex->first;
            typename MapTree<2,K,T>::Type* thirIndex = iterSecIndex->second;

            typename MapTree<1,K,T>::MapType& mapThirIndex = thirIndex->getChild();
            typename MapTree<1,K,T>::MapIterator iterThirIndex = mapThirIndex.begin();
            for(;iterThirIndex != mapThirIndex.end(); )
            {
                const K  thirKey = iterThirIndex->first;
                T* data = iterThirIndex->second;
                iterThirIndex++;
                if(filterFunc(*data))
                {
                    int ret = thirIndex->delChild(thirKey);
                    switch(ret)
                    {
                    case MapTree<1,K,T>::RET_NONE:
                        break;
                    case MapTree<1,K,T>::RET_DELDATA:
                        conn->removeMapField(_tableName + ":data",
                                             boost::lexical_cast<string>(thirKey));
                    case MapTree<1,K,T>::RET_DELINDEX:
                        conn->removeUniq(_tableName + "." + ParaTwo<K>(paraKeyFirst,paraKeySec).toString(),
                                         boost::lexical_cast<string>(thirKey));
                        break;
                    }
                }
            }
            if(mapThirIndex.empty())
            {
                conn->delKey(_tableName + "." + ParaTwo<K>(paraKeyFirst,paraKeySec).toString());
                iterSecIndex++;
                secIndex->delChild(paraKeySec);
                if(mapSecIndex.empty())
                {
                    conn->delKey(_tableName + "." + boost::lexical_cast<string>(paraKeyFirst));
                    firstIter++;
                    conn->removeUniq(_tableName ,
                                     boost::lexical_cast<string>(paraKeyFirst));
                    _mapTree.delChild(paraKeyFirst);
                    bRmFirst = true;
                }
                else
                {
                    conn->removeUniq(_tableName + "." + boost::lexical_cast<string>(paraKeyFirst),
                                     boost::lexical_cast<string>(paraKeySec));
                }
            }
            else
            {
                iterSecIndex++;
            }
        }
        if(!bRmFirst)
        {
            firstIter++;
        }
    }
    return true;
}

template <typename K,typename T>
void CacheManager3Map<K,T>::removeMapData(const K& strFirst,
                                          const K& strSec,const K& strThird)
{
    DataCache::AutoConnCache conn = CacheServer::getCacheServer();
    //删索引
    typename MapTree<3,K,T>::MapType& firstIndex =  _mapTree.getChild();
    typename MapTree<3,K,T>::MapIterator firstIter = firstIndex.find(strFirst);
    if(firstIter != firstIndex.end())
    {
        typename MapTree<3,K,T>::Type* secIndex = firstIter->second;
        typename MapTree<2,K,T>::MapType& secMap = secIndex->getChild();
        typename MapTree<2,K,T>::MapIterator secIter = secMap.find(strSec);
        if(secIter != secMap.end())
        {
            typename MapTree<2,K,T>::Type* thirIndex = secIter->second;
            typename MapTree<1,K,T>::MapType& thirMap = thirIndex->getChild();
            typename MapTree<1,K,T>::MapIterator thirIter = thirMap.find(strThird);
            if(thirIter != thirMap.end())
            {
                int ret = thirIndex->delChild(strThird);
                switch(ret)
                {
                case MapTree<1,K,T>::RET_NONE:
                    break;
                case MapTree<1,K,T>::RET_DELDATA:
                    conn->removeMapField(_tableName + ":data",
                                         boost::lexical_cast<string>(strThird));
                case MapTree<1,K,T>::RET_DELINDEX:
                    conn->removeUniq(_tableName + "." + ParaTwo<K>(strFirst,strSec).toString(),
                                     boost::lexical_cast<string>(strThird));
                    break;
                }
                if(thirMap.empty())
                {
                    conn->delKey(_tableName + "." + ParaTwo<K>(strFirst,strSec).toString());

                    secIndex->delChild(strSec);
                    if(secMap.empty())
                    {
                        conn->delKey(_tableName + "." + boost::lexical_cast<string>(strFirst));
                        _mapTree.delChild(strFirst);
                        conn->removeUniq(_tableName ,
                                         boost::lexical_cast<string>(strFirst));
                    }
                    else
                    {
                        conn->removeUniq(_tableName + "." + boost::lexical_cast<string>(strFirst),
                                         boost::lexical_cast<string>(strSec));
                    }
                }
            }
        }
    }
}

template <typename K,typename T>
T* CacheManager3Map<K,T>::addMapData(const K& strFirst,
                                       const K& strSec,const K& strThird)
{
    //_topCache.insert(strFirst);
    typename MapTree<3,K,T>::Type* firstIndex = _mapTree.addChild(strFirst);
    typename MapTree<2,K,T>::Type* secIndex = firstIndex->addChild(strSec);
    return secIndex->addChild(strThird);
}

template <typename K,typename T>
void CacheManager3Map<K,T>::getCacheBy2Prefix(const K& first,const K& sec,map<K,T*>& retMap)
{
    retMap.clear();
    typename MapTree<3,K,T>::Type* firstIndex = _mapTree.getChildSingle(first);
    if(firstIndex)
    {
        typename MapTree<2,K,T>::Type* secIndex = firstIndex->getChildSingle(sec);
        if(secIndex)
        {
            typename  MapTree<1,K,T>::MapType& mapThirIndex = secIndex->getChild();
            typename MapTree<1,K,T>::ConstMapIterator iterThirIndex = mapThirIndex.begin();
            for(;iterThirIndex != mapThirIndex.end(); iterThirIndex++)
            {
                const K& thirKey = iterThirIndex->first;
                T* tmpData = iterThirIndex->second;
                retMap.insert(pair<K,T*>(thirKey,tmpData));
            }
        }
    }
}

template <typename K,typename T>
void CacheManager3Map<K,T>::getCacheBy1Prefix(const K& first,map<K,T*>& retMap)
{
    retMap.clear();
    typename MapTree<3,K,T>::Type* firstIndex = _mapTree.getChildSingle(first);
    if(firstIndex)
    {
        typename MapTree<2,K,T>::MapType& mapSec = firstIndex->getChild();
        typename MapTree<2,K,T>::MapIterator iterSec = mapSec.begin();
        for(;iterSec != mapSec.end();iterSec++)
        {
            typename MapTree<2,K,T>::Type& secIndex = iterSec->second;
            typename  MapTree<1,K,T>::MapType& mapThirIndex = secIndex->getChild();
            typename MapTree<1,K,T>::ConstMapIterator iterThirIndex = mapThirIndex.begin();
            for(;iterThirIndex != mapThirIndex.end(); iterThirIndex++)
            {
                const K& thirKey = iterThirIndex->first;
                T* tmpData = iterThirIndex->second;
                retMap.insert(pair<K,T*>(thirKey,tmpData));
            }
        }
    }
}

template <typename K,typename T>
T* CacheManager3Map<K,T>::getMapData(const K& strFirst,const K& strSec,const K& strThird)
{
    typename MapTree<3,K,T>::Type* firstIndex = _mapTree.getChildSingle(strFirst);
    if(firstIndex)
    {
        typename MapTree<2,K,T>::Type* secIndex = firstIndex->getChildSingle(strSec);
        if(secIndex)
        {
            return secIndex->getChildSingle(strThird);
        }
    }
    return NULL;
}

template <typename K,typename T>
void CacheManager3Map<K,T>::setMapData(const K& strFirst,const K& strSec,
                                       const K& strThird,const T& data)
{
    //_topCache.insert(strFirst);
    typename MapTree<3,K,T>::Type* firstIndex = _mapTree.addChild(strFirst);
    typename MapTree<2,K,T>::Type* secIndex = firstIndex->addChild(strSec);
    secIndex->addChild(strThird,data);
}

template <typename K,typename T>
bool CacheManager3Map<K,T>::delTable()
{
    vector<string> vecKeys;
    vecKeys.push_back(_tableName + ":data");
    vecKeys.push_back(_tableName);

    DataCache::AutoConnCache conn = CacheServer::getCacheServer();
    //加第一层
    set<K> vecTop;
    conn->loadUniqObj(_tableName,vecTop);
    //加第二层
    if(!vecTop.empty())
    {
        typename set<K>::iterator iterTop = vecTop.begin();
        for(;iterTop != vecTop.end(); iterTop++)
        {
            const K& topKey = *iterTop;
            string strTopKey = boost::lexical_cast<string>(topKey);
            vecKeys.push_back(_tableName + "." +strTopKey);
            set<K> vecSec;
            if(!conn->loadUniqObj(_tableName + "." + strTopKey,vecSec))
            {
                return false;
            }
            for(typename set<K>::iterator iterSec = vecSec.begin();iterSec != vecSec.end(); iterSec++)
            {
                 string strSecKey = boost::lexical_cast<string>(*iterSec);
                vecKeys.push_back(_tableName + "." +strTopKey + "." + strSecKey);
            }
        }
    }

    if(!conn->delKeys(vecKeys))
    {
        return false;
    }
    delTableFlag();
    return true;
}

template <typename K,typename T>
bool CacheManager3Map<K,T>::clearMapData(bool bClearTable)
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
bool  CacheManager3Map<K,T>::loadCacheAll()
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
    typename MapTree<3,K,T>::MapType& mapFirst = _mapTree.getChild();
    if(mapFirst.empty())
    {
        if(!conn->loadUniqMapKey(_tableName,mapFirst,
                                 (typename MapTree<3,K,T>::Type*)NULL))
        {
            return false;
        }
        typename MapTree<3,K,T>::MapIterator iterFirst = mapFirst.begin();
        for(;iterFirst != mapFirst.end(); iterFirst++)
        {
            const K& topKey = iterFirst->first;
            typename MapTree<3,K,T>::Type* secIndex = _mapTree.allocChild(topKey);
            typename MapTree<2,K,T>::MapType& mapSec = secIndex->getChild();
            ostringstream osKey;
            osKey << _tableName << "." << topKey;
            if(!conn->loadUniqMapKey(osKey.str(),mapSec,
                                   (typename MapTree<2,K,T>::Type*)NULL))
            {
                assert(false);
                return false;
            }
            typename MapTree<2,K,T>::MapIterator iterSec = mapSec.begin();
            for(;iterSec !=  mapSec.end();iterSec++)
            {
                const K& secKey = iterSec->first;
                typename MapTree<2,K,T>::Type* thirIndex = secIndex->allocChild(secKey);
                typename MapTree<1,K,T>::MapType& mapThir = thirIndex->getChild();
                if(!conn->loadUniqMapKey(_tableName + "." +
                                         ParaTwo<K>(topKey,secKey).toString(),
                                         mapThir,
                                       (typename MapTree<1,K,T>::Type*)NULL))
                {
                    return false;
                }
                typename MapTree<1,K,T>::MapIterator iterThir = mapThir.begin();
                for(;iterThir !=  mapThir.end();iterThir++)
                {
                    const K& thirKey = iterThir->first;
                    typename MapTree<1,K,T>::Type* dataBom = thirIndex->allocChild(thirKey,true);
                    if(!dataBom)
                    {
                        ostringstream errorFor;
                        errorFor << "table:" << _tableName << ","
                                    << "key1:" << topKey << ","
                                    << "key2:" << secKey << ","
                                   << "key3:" << thirKey << ","
                                   << ", data not exist!";
                        LOG_ERROR(errorFor.str().c_str());
                        assert(false);
                    }
                }
            }
        }
    }
    //
    for(typename map<K,int>::iterator iterRef = _mapRef.begin(); iterRef != _mapRef.end();)
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
T* CacheManager3Map<K,T>::saveCacheSingle(const K& strFirst,const K& strSec,
                   const K& strThird,const T& data)
{
    DataCache::AutoConnCache conn =  CacheServer::getCacheServer();
    //保存基本数据
    if(!conn->saveMapObjSingle(_tableName + ":data",strThird,data))
    {
        return NULL;
    }
    //更新索引
    if(!conn->saveUniqObjSingle(_tableName,strFirst))
    {
        ostringstream osFormator;
        osFormator << "table:" << _tableName << " key1:" << strFirst << " key2:"
                   << strSec << " key3:" << strThird << "save first key error!";
        LOG_ERROR(osFormator.str().c_str());
    }
    if(!conn->saveUniqObjSingle(_tableName + "." +
                                boost::lexical_cast<string>(strFirst),
                                strSec))
    {
        ostringstream osFormator;
        osFormator << "table:" << _tableName << " key1:" << strFirst << " key2:"
                   << strSec << " key3:" << strThird << "save second key error!";
        LOG_ERROR(osFormator.str().c_str());
    }
    if(!conn->saveUniqObjSingle(_tableName + "." +
                                boost::lexical_cast<string>(strFirst) +
                                "." + boost::lexical_cast<string>(strSec),
                                strThird))
    {
        ostringstream osFormator;
        osFormator << "table:" << _tableName << " key1:" << strFirst << " key2:"
                   << strSec << " key3:" << strThird << "save third key error!";
        LOG_ERROR(osFormator.str().c_str());
    }
    typename MapTree<3,K,T>::Type* firstIndex = _mapTree.addChild(strFirst);
    typename MapTree<2,K,T>::Type* secIndex = firstIndex->addChild(strSec);
    return secIndex->addChild(strThird,data);
}
template <typename K,typename T>
bool CacheManager3Map<K,T>::delCacheByFirst(const K& strFirst)
{
    DataCache::AutoConnCache conn =  CacheServer::getCacheServer();
    //删索引
    typename MapTree<3,K,T>::Type* secIndex = _mapTree.getChildSingle(strFirst);
    if(!secIndex)
    {
        return true;
    }
    typename MapTree<2,K,T>::MapType& mapSec = secIndex->getChild();
    typename MapTree<2,K,T>::MapIterator iterSec = mapSec.begin();
    for(;iterSec != mapSec.end();)
    {
        const K  secKey = iterSec->first;
        typename MapTree<2,K,T>::Type* thirData = iterSec->second;
        typename MapTree<1,K,T>::MapType& mapThir = thirData->getChild();
        typename MapTree<1,K,T>::MapIterator iterThir = mapThir.begin();
        for(;iterThir != mapThir.end();)
        {
            const K  key = iterThir->first;
            int ret = thirData->delChildByIter(iterThir++);
            switch(ret)
            {
            case  MapTree<1,K,T>::RET_NONE:
                continue;
            case  MapTree<1,K,T>::RET_DELDATA:
                if(!conn->removeMapField(_tableName + ":data",boost::lexical_cast<string>(key)))
                {
                    assert(false);
                    return false;
                }
                 _mapCache.erase(secKey);
            case  MapTree<1,K,T>::RET_DELINDEX:
                if(!conn->removeUniq(_tableName + "." + ParaTwo<K>(strFirst,secKey).toString(),
                                     boost::lexical_cast<string>(key)))
                {
                    assert(false);
                    return false;
                }
                break;
            }
        }
        secIndex->delChildByIter(iterSec++);
        if(!conn->removeUniq(_tableName + "." + boost::lexical_cast<string>(strFirst),
                             boost::lexical_cast<string>(secKey)))
        {
            assert(false);
            return false;
        }
    }
    conn->delKey(_tableName + "." + boost::lexical_cast<string>(strFirst));
    _mapTree.delChild(strFirst);
    conn->removeUniq(_tableName ,boost::lexical_cast<string>(strFirst));
    return true;
}
template <typename K,typename T>
bool CacheManager3Map<K,T>::delCacheByTwo(const K& strFirst,const K& strSec)
{
    DataCache::AutoConnCache conn =  CacheServer::getCacheServer();
    //删索引
    typename MapTree<3,K,T>::Type* secIndex = _mapTree.getChildSingle(strFirst);
    if(secIndex)
    {
        typename MapTree<2,K,T>::Type* thirIndex = secIndex->getChildSingle(strSec);
        if(thirIndex)
        {
            typename MapTree<1,K,T>::MapType& mapThir = thirIndex->getChild();
            typename MapTree<1,K,T>::MapIterator iterThir = mapThir.begin();
            for(;iterThir != mapThir.end();)
            {
                const K key = iterThir->first;
                int ret = thirIndex->delChildByIter(iterThir++);
                switch(ret)
                {
                case  MapTree<1,K,T>::RET_NONE:
                    continue;
                case  MapTree<1,K,T>::RET_DELDATA:
                    if(!conn->removeMapField(_tableName + ":data",boost::lexical_cast<string>(key)))
                    {
                        assert(false);
                        return false;
                    }
                     _mapCache.erase(strSec);
                case  MapTree<1,K,T>::RET_DELINDEX:
                    if(!conn->removeUniq(_tableName + "." + ParaTwo<K>(strFirst,strSec).toString(),
                                         boost::lexical_cast<string>(key)))
                    {
                        assert(false);
                        return false;
                    }
                    break;
                }
            }
            secIndex->delChild(strSec);
            if(secIndex->getChild().empty())
            {
                conn->delKey(_tableName + "." + boost::lexical_cast<string>(strFirst));
                _mapTree.delChild(strFirst);
                conn->removeUniq(_tableName ,
                                 boost::lexical_cast<string>(strFirst));
            }
            else
            {
                if(!conn->removeUniq(_tableName + "." + boost::lexical_cast<string>(strFirst),
                                     boost::lexical_cast<string>(strSec)))
                {
                    assert(false);
                    return false;
                }
            }
        }
    }
    return true;
}

template <typename K,typename T>
bool CacheManager3Map<K,T>::delCacheSingle(const K& strFirst,const K& strSec,
                    const K& strThird)
{
    DataCache::AutoConnCache conn =  CacheServer::getCacheServer();
    //删索引
    typename MapTree<3,K,T>::Type* secIndex = _mapTree.getChildSingle(strFirst);
    if(secIndex)
    {
        typename MapTree<2,K,T>::Type* thirIndex = secIndex->getChildSingle(strSec);
        if(thirIndex)
        {
              //删除
              int ret = thirIndex->delChild(strThird);
              switch(ret)
              {
              case  MapTree<1,K,T>::RET_NONE:
                  return true;
              case  MapTree<1,K,T>::RET_DELDATA:
                  if(!conn->removeMapField(_tableName + ":data",boost::lexical_cast<string>(strThird)))
                  {
                      assert(false);
                      return false;
                  }
                   _mapCache.erase(strSec);
              case  MapTree<1,K,T>::RET_DELINDEX:
                  if(!conn->removeUniq(_tableName + "." + ParaTwo<K>(strFirst,strSec).toString(),
                                       boost::lexical_cast<string>(strThird)))
                  {
                      assert(false);
                      return false;
                  }
                  break;
              }
              if(thirIndex->getChild().empty())
              {
                  secIndex->delChild(strSec);
                  if(secIndex->getChild().empty())
                  {
                      conn->delKey(_tableName + "." + boost::lexical_cast<string>(strFirst));
                      _mapTree.delChild(strFirst);
                      conn->removeUniq(_tableName ,boost::lexical_cast<string>(strFirst));
                  }
                  else
                  {
                      if(!conn->removeUniq(_tableName + "." + boost::lexical_cast<string>(strFirst),
                                           boost::lexical_cast<string>(strSec)))
                      {
                          assert(false);
                          return false;
                      }
                  }
              }
        }
    }
    else
    {
        return true;
    }
    return true;
}

template <typename K,typename T>
bool CacheManager3Map<K,T>::saveCacheAll()
{

    delTable();
    //保存数据
    DataCache::AutoConnCache conn = CacheServer::getCacheServer();
    if(!conn->saveMapObj(_tableName + ":data",_mapCache))
    {
        return false;
    }
    saveTableFlag();
    typename MapTree<3,K,T>::MapType& mapTop = _mapTree.getChild();
    typename MapTree<3,K,T>::MapIterator  iterTop = mapTop.begin();
    for(;iterTop != mapTop.end(); iterTop++)
    {
        const K& firstKey = iterTop->first;
        typename MapTree<3,K,T>::Type* secIndex = iterTop->second;
        typename MapTree<2,K,T>::MapType& mapSecIndex = secIndex->getChild();
        typename MapTree<2,K,T>::MapIterator iterSecIndex = mapSecIndex.begin();
        for(;iterSecIndex != mapSecIndex.end(); iterSecIndex++)
        {
            const K& secKey = iterSecIndex->first;
            typename MapTree<2,K,T>::Type* thirIndex = iterSecIndex->second;
            typename MapTree<1,K,T>::MapType& mapThirIndex = thirIndex->getChild();
            if(!conn->saveUniqMapKey(_tableName + "." + ParaTwo<K>(firstKey,secKey).toString(),
                                  mapThirIndex))
            {
                return false;
            }
        }
        ostringstream osKey;
        osKey << _tableName << "." << firstKey;
        if(!conn->saveUniqMapKey(osKey.str(),mapSecIndex))
        {
            return false;
        }
    }
    if(!conn->saveUniqMapKey(_tableName ,mapTop))
    {
        return false;
    }
    return true;
}

/*
template <typename K,typename T>
void CacheManager3Map<K,T>::second2Json(const K& strKey1,const K& strKey2,JsonContext& firstJson) const
{
    typename map<ParaTwo<K>,set<K> >::const_iterator iterSec =  _mapIndexSecond.find(ParaTwo<K>(strKey1 ,strKey2));
    if(iterSec != _mapIndexSecond.end())
    {
        typename set<K>::const_iterator iterSetSec = iterSec->second.begin();
        for(;iterSetSec !=  iterSec->second.end(); iterSetSec++)
        {
            const K& secKey = *iterSetSec;
            typename map<K,T>::const_iterator iterData = _mapCache.find(secKey);
            if(iterData == _mapCache.end())
            {
                ostringstream osError;
                osError <<  "表：” + " << _tableName << "key1:" << strKey1 <<
                            " key2:" << strKey2 << " key3:" << secKey << " 无数据";
                throw  CacheException(osError.str());
            }
            const T& secData = iterData->second;
            rapidjson::Value valSecData(rapidjson::kArrayType);
            JsonContext jsonContextData(firstJson.getDoc(),valSecData);
            typename JsonConvertor<T>::JsonConvertFunc jsonConvertFunc;
            jsonConvertFunc.convertToJson(secData,jsonContextData);

            string strSecKey = boost::lexical_cast<string>(secKey);

            rapidjson::Value nameSecKey;
            JsonContext contextSecKey(firstJson.getDoc(),nameSecKey);
            contextSecKey.setVal(strSecKey);

            firstJson.getVal().AddMember(nameSecKey,valSecData,firstJson.getDoc().GetAllocator());
        }
    }
}*/

template <typename K,typename T>
bool CacheManager3Map<K,T>::fromJson(const string& strJson)
{
    rapidjson::Document doc;
    doc.Parse<0>(strJson.c_str());
    if (doc.HasParseError())  //解析错误
    {
        string strError = string("3map table:") + this->getTableName() + "  Parse error!";
        throw NorException(strError);
    }
    if(!doc.IsObject())
    {
        string strError = string("3map table:") + this->getTableName() + "  root must obj!";
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
        typename MapTree<3,K,T>::Type* firstChild = _mapTree.addChild(keyFirst);
        rapidjson::Value& firstVal = iterTop->value;
        rapidjson::Value::MemberIterator iterSec = firstVal.MemberBegin();
        for(;iterSec != firstVal.MemberEnd(); iterSec++)
        {
            JsonContext jsonKeyContex(doc,iterSec->name);
            jsonKeyContex.toVal(tmpKey);

            K keySec = boost::lexical_cast<K>(tmpKey);
            typename MapTree<2,K,T>::Type* secChild = firstChild->addChild(keySec);
            rapidjson::Value& secVal = iterSec->value;
            rapidjson::Value::MemberIterator iterThird = secVal.MemberBegin();
            for(;iterThird != secVal.MemberEnd(); iterThird++)
            {
                JsonContext jsonKeyContex(doc,iterThird->name);
                jsonKeyContex.toVal(tmpKey);

                K keyThird = boost::lexical_cast<K>(tmpKey);
                rapidjson::Value& thirdVal = iterThird->value;
                JsonContext jsonContext(doc,thirdVal);
                 T objData;
                 jsonConvertFunc.jsonToObj(objData,jsonContext);
                 secChild->addChild(keyThird,objData);
            }
        }
    }
    return true;
}

template <typename K,typename T>
const string CacheManager3Map<K,T>::toJson() const
{
    const typename MapTree<3,K,T>::MapType& mapFirst = _mapTree.getChild();
    typename MapTree<3,K,T>::ConstMapIterator iterTop = mapFirst.begin();
    rapidjson::Value jsonRoot(rapidjson::kObjectType);
     rapidjson::Document doc;
     for(; iterTop != mapFirst.end(); iterTop++)
     {
         const K& topKey = iterTop->first;
         rapidjson::Value firstJson(rapidjson::kObjectType);

         const typename MapTree<3,K,T>::Type* secIndex = iterTop->second;
         const typename MapTree<2,K,T>::MapType& mapSec = secIndex->getChild();
         typename MapTree<2,K,T>::ConstMapIterator iterSec = mapSec.begin();
         for(;iterSec !=  mapSec.end(); iterSec++)
         {
             const K& secKey = iterSec->first;
             rapidjson::Value secVal(rapidjson::kObjectType);
             JsonContext contextSecVal(doc,secVal);

             const typename MapTree<2,K,T>::Type* thirIndex = iterSec->second;
             const typename MapTree<1,K,T>::MapType& mapThir = thirIndex->getChild();
             typename MapTree<1,K,T>::ConstMapIterator iterThir = mapThir.begin();
             for(;iterThir != mapThir.end(); iterThir++)
             {
                 const K& thirKey = iterThir->first;

                 const T* secData = iterThir->second;
                 rapidjson::Value valSecData(rapidjson::kArrayType);
                 JsonContext jsonContextData(doc,valSecData);
                 typename JsonConvertor<T>::JsonConvertFunc jsonConvertFunc;
                 jsonConvertFunc.convertToJson(*secData,jsonContextData);

                 string strThirKey = boost::lexical_cast<string>(thirKey);

                 rapidjson::Value nameThirKey;
                 JsonContext contextSecKey(doc,nameThirKey);
                 contextSecKey.setVal(strThirKey);

                 secVal.AddMember(nameThirKey,valSecData,doc.GetAllocator());
             }
             string strSecKey = boost::lexical_cast<string>(secKey);
             rapidjson::Value nameSecKey;
             JsonContext contextSecKey(doc,nameSecKey);
             contextSecKey.setVal(strSecKey);

             firstJson.AddMember(nameSecKey,secVal,doc.GetAllocator());
         }
         string strTopKey = boost::lexical_cast<string>(topKey);
         rapidjson::Value nameTopKey;
         JsonContext contextTopKey(doc,nameTopKey);
         contextTopKey.setVal(strTopKey);

         jsonRoot.AddMember(nameTopKey,firstJson,doc.GetAllocator());
     }
     JsonContext contextRoot(doc,jsonRoot);
     return contextRoot.getString();
}

}

#endif // CACHEMANAGER3MAP_H
