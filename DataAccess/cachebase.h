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
        REDIS_STRING = 1,// (�ַ���)
        REDIS_LIST,// (�б�)
        REDIS_SET,//(����)
        REDIS_ZSET,// (����)
        REDIS_HASH,// (��ϣ��)
        REDIS_NONE// (key������)
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

    //ʧ������ؓһ
    int getListLen(const string& key);
    //vector ��list ���

    //template <typename T, template<typename T,class Alloc = allocator<T> > class C>
    //bool loadListBasic(const string& key,C<T>& vecBasic,const int& begin=0,const int& end=-1,bool bClear=true);
    /* ����ֵΪ��ǰ���б��е����ݸ�����-1��ʾʧ��*/
    //template <typename T, template<typename T,class Alloc = allocator<T> > class C>
    //int saveListBasic(const string& key,const C<T>& vecBasic,bool bAdd=false);
    //template<typename T>
   // bool saveListBasicSingle(const string& key,const int index,const T& obj );


    //map����
    bool getMapField(const string& key,vector<string>& vecField);
    bool removeMapField(const string& key,const string field);

    //set����
    bool removeUniq(const string& key,const string& field);

    //һ��map�����
    //template<typename KEY,typename T>
    //bool loadMapBasic(const string& key,map<KEY,T>& vecBasic);
    //template<typename KEY,typename T>
    //bool saveMapBasic(const string& key,const map<KEY,T>& vecBasic,bool bAdd=false);

    //template<typename KEY,typename T>
    //bool loadMapBasicSingle(const string& key,const KEY& field,T& mapBasic);
    //template<typename KEY,typename T>
    //bool saveMapBasicSingle(const string& key,const KEY& field,const T& mapBasic);


    //set����
    //template<typename T>
    //bool loadUniqBasic(const string& key,set<T>& uniqBasic);
    //template<typename T>
    //bool saveUniqBasic(const string& key,const set<T>& uniqBasic,bool bAdd=false);
    //

    //��һ����
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
