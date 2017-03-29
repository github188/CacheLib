#include "cachebackup.h"
#include "rapidjson/document.h"
#include "cachemapnormal.h"
#include "cache2mapnormal.h"
#include "cache3mapnormal.h"
#include "../cache/cachemanagerobj.h"
#include <fstream>
using namespace std;
CacheBackup::CacheBackup(const string& dir) : _dir(dir)
{
}

bool CacheBackup::backupTable(const string& backTable)
{
    int tableFlag = 0;
    DataCache::CacheServer::getCacheServer()->loadMapObjSingle(TBL_TABLEFLAG,backTable,tableFlag);
    switch(tableFlag)
    {
    case DataCache::CacheManager::TBLFLAG_LIST:
    case DataCache::CacheManager::TBLFLAG_VECTOR2D:
    {
        DataCache::CacheManagerList<string,list> cacheTmp(backTable);
        if(!cacheTmp.loadCache())
        {
            return false;
        }
        const list<string>& itemList = cacheTmp.getCache();
        rapidjson::Document doc;
        rapidjson::Value root(rapidjson::kArrayType);
        list<string>::const_iterator iterItem = itemList.begin();
        for(;iterItem != itemList.end(); iterItem++)
        {
            const string& tmpItem = *iterItem;
            doc.Parse<0>(tmpItem.c_str());
            if (doc.HasParseError())  //解析错误
            {
                string strError = string("table:") + backTable  + " backup Parse item error!";
                printf(strError.c_str());
                LOG_ERROR(strError.c_str());
                throw NorException(strError);
                return false;
                //throw CacheException(strError);
            }
            root.PushBack(doc,doc.GetAllocator());
        }
        JsonContext contextRoot(doc,root);
        saveTable(backTable,tableFlag,
                  contextRoot.getString());
    }
        break;
    case DataCache::CacheManager::TBLFLAG_OBJ:
    {
        DataCache::CacheManagerObj<string> cacheTmp(backTable);
        if(!cacheTmp.loadCache())
        {
            return false;
        }
        const string& strJson = cacheTmp.getCache();
        if(strJson.empty())
        {
            return false;
        }
        saveTable(backTable,DataCache::CacheManager::TBLFLAG_OBJ,
                  strJson);
    }
        break;
    case DataCache::CacheManager::TBLFLAG_MAP:
    {

        string strContent;
        CacheMapNormal<string> cacheNormal;
        if(!cacheNormal.readCache(backTable,strContent))
        {
            return false;
        }
        saveTable(backTable,DataCache::CacheManager::TBLFLAG_MAP,
                  strContent);
    }
        break;
    case DataCache::CacheManager::TBLFLAG_2MAP:
    {
        Cache2MapNormal<string> cacheMapNormal;
        string strContent;
        if(!cacheMapNormal.readCache(backTable,strContent))
        {
            return false;
        }
        saveTable(backTable,DataCache::CacheManager::TBLFLAG_2MAP,
                  strContent);
    }
        break;
    case DataCache::CacheManager::TBLFLAG_3MAP:
    {
        Cache3MapNormal<string> cacheMapNormal;
        string strContent;
        if(!cacheMapNormal.readCache(backTable,strContent))
        {
            return false;
        }
        saveTable(backTable,DataCache::CacheManager::TBLFLAG_3MAP,
                  strContent);

    }
        break;
    default:
        return false;
    }
    return true;
}
bool CacheBackup::deleteTable(const string& backTable)
{
    return DataCache::CacheServer::getCacheServer()->delTable(backTable);
}

bool CacheBackup::restoreTable(const string& backTable)
{
    string strContent;
    int tableFlag = 0;
    if(!readTable(backTable,tableFlag,strContent))
    {
        return false;
    }
    switch(tableFlag)
    {
    case DataCache::CacheManager::TBLFLAG_LIST:
    case DataCache::CacheManager::TBLFLAG_VECTOR2D:
    {
        DataCache::CacheManagerList<string,list> cacheTmp(backTable);
        if(!cacheTmp.fromJson(strContent))
        {
            return false;
        }
        cacheTmp.saveCache();
    }
        break;
    case DataCache::CacheManager::TBLFLAG_OBJ:
    {
        DataCache::CacheManagerObj<string> cacheTmp(backTable);
        if(!cacheTmp.fromJson(strContent))
        {
            return false;
        }
        cacheTmp.saveCache();
    }
        break;
    case DataCache::CacheManager::TBLFLAG_MAP:
    {
        CacheMapNormal<string> cacheTmp;
        if(!cacheTmp.cacheToRedis(backTable,strContent))
        {
            return false;
        }
    }
        break;
    case DataCache::CacheManager::TBLFLAG_2MAP:
    {
        Cache2MapNormal<string> cacheTmp;
        if(!cacheTmp.cacheToRedis(backTable,strContent))
        {
            return false;
        }
    }
        break;
    case DataCache::CacheManager::TBLFLAG_3MAP:
    {
        Cache3MapNormal<string> cacheTmp;
        if(!cacheTmp.cacheToRedis(backTable,strContent))
        {
            return false;
        }
    }
        break;
    default:
        return false;
    }
    return true;
}
bool CacheBackup::saveTable(const string& strTable,int tableFlag,const string& strContent)
{
    string strPath = _dir + strTable + ".bak";
    ofstream ofTable(strPath.c_str());
    ofTable << tableFlag;
    ofTable << ModifyJson(strContent);
    ofTable.close();
    return true;
}
string CacheBackup::ModifyJson(const string &strJSon)
{
    string strRet;
    if (strJSon.size() > 0)
    {
        strRet.push_back(strJSon[0]);
    }

    bool bIsLineHead = false;//是否是行首的标记，如果是行首，则不消除制表符
    for(uint i = 1; i < strJSon.size(); i++)
    {
        switch (strJSon[i])
        {
        case '\n':
        {
            if('{' == strJSon[i - 1]
                    || '}' == strJSon[i - 1]
                    || '[' == strJSon[i - 1]
                    || ']' == strJSon[i - 1])
            {
                strRet.push_back(strJSon[i]);
                bIsLineHead = true;
            }
            else if(',' == strJSon[i - 1])
            {
                if('{' == strJSon[i - 2]
                        || '}' == strJSon[i - 2]
                        || '[' == strJSon[i - 2]
                        || ']' == strJSon[i - 2])
                {
                    strRet.push_back(strJSon[i]);
                    bIsLineHead = true;
                }
            }
            else
            {
                bIsLineHead = false;
            }
        }
            break;
        case ' ':
        {
            //非连续制表符或者是行首的制表符则保留
            if(' ' != strJSon[i - 1] || bIsLineHead)
            {
                strRet.push_back(strJSon[i]);
            }
        }
            break;
        default:
        {
            strRet.push_back(strJSon[i]);
            bIsLineHead = false;
        }
            break;
        }
    }
    return strRet;
}
bool CacheBackup::readTable(const string& strTable,int& tableFlag,string& strContent)
{
    string strPath = _dir + strTable + ".bak";
    ifstream inTable(strPath.c_str());
    strContent = "";
    inTable >> tableFlag;
    if(inTable.is_open())
    {
        strContent = string((std::istreambuf_iterator<char>(inTable)),
                         std::istreambuf_iterator<char>());
        if(strContent.empty())
        {
            return false;
        }
    }
    else
    {
        return false;
    }
    return true;
}
