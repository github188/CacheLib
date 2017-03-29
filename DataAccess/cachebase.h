#ifndef CACHEBASE_H
#define CACHEBASE_H

#include <string>
#include <map>
#include <list>
#include <vector>
#include <set>
#include <sstream>
#include <string.h>
using namespace std;
#include "../redis/redis.h"
#include "stringbunch.h"

namespace DataCache
{
class CacheBase
{
public:
    enum RedisType
    {
        REDIS_STRING = 1,// (字符串)
        REDIS_LIST,// (列表)
        REDIS_SET,//(集合)
        REDIS_ZSET,// (有序集)
        REDIS_HASH,// (哈希表)
        REDIS_NONE// (key不存在)
    };

    CacheBase(const std::string& server,const int port,const string& pass);
    virtual ~CacheBase();

    bool connServer(const std::string& server,const int port);

    bool delTable(const string& strKey);
    bool delKey(const string& strKey);
    bool delKeys(const vector<string>& vecKey);
    bool getAllKeys(std::vector<std::string>& keys);
    bool clearAll();
    bool rename(const std::string & beforname, const std::string & aftername);
    CacheBase::RedisType getKeyType(const std::string& key);
    bool getKeyFilter(const std::string& strFilter, std::vector<std::string>& vecKey,bool bClear = true);
    bool removeList(const string& key,const int index);

    //失敗返回負一
    int getListLen(const string& key);
    //vector 或list 情况

    //template <typename T, template<typename T,class Alloc = allocator<T> > class C>
    //bool loadListBasic(const string& key,C<T>& vecBasic,const int& begin=0,const int& end=-1,bool bClear=true);
    /* 返回值为当前，列表中的数据个数，-1表示失败*/
    //template <typename T, template<typename T,class Alloc = allocator<T> > class C>
    //int saveListBasic(const string& key,const C<T>& vecBasic,bool bAdd=false);
    //template<typename T>
   // bool saveListBasicSingle(const string& key,const int index,const T& obj );


    //map操作
    bool getMapField(const string& key,vector<string>& vecField);
    bool removeMapField(const string& key,const string field);

    //set操作
    bool removeUniq(const string& key,const string& field);

    //一个map的情况
    //template<typename KEY,typename T>
    //bool loadMapBasic(const string& key,map<KEY,T>& vecBasic);
    //template<typename KEY,typename T>
    //bool saveMapBasic(const string& key,const map<KEY,T>& vecBasic,bool bAdd=false);

    //template<typename KEY,typename T>
    //bool loadMapBasicSingle(const string& key,const KEY& field,T& mapBasic);
    //template<typename KEY,typename T>
    //bool saveMapBasicSingle(const string& key,const KEY& field,const T& mapBasic);


    //set操作
    //template<typename T>
    //bool loadUniqBasic(const string& key,set<T>& uniqBasic);
    //template<typename T>
    //bool saveUniqBasic(const string& key,const set<T>& uniqBasic,bool bAdd=false);
    //

    //单一类型
    //template<typename T>
    //bool loadBasic(const string& key,T& basic);
    //template<typename T>
    //bool saveBasic(const string& key,T& basic);

protected:
    redisContext *conn;
    std::string ServerURL;
    string _pass;
    int ServerPort;
};
}

#endif // CACHEBASE_H
