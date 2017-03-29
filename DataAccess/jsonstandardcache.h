#ifndef JSONSTANDARDCACHE_H
#define JSONSTANDARDCACHE_H


#include "../serial/jsonserial.h"

#include "stringbunch.h"
#include "comm/norexception.h"
#include <set>
#include <boost/lexical_cast.hpp>

#include "comm/paratwo.h"
#include "cachebase.h"
#include "log4cpp/log4cppwraper.h"
#include "../comm/vector2d.h"
#include <boost/ptr_container/ptr_list.hpp>
#define REDIS_TRANSSIZE 100 //此值最小也要大于二
#define REDIS_LOADSIZE 100
#define REDIS_UP 0

class redisContext;
namespace DataCache
{
class JsonStandardCache : public CacheBase
{
public:


    JsonStandardCache(const std::string& server,const int port,const string& pass);


    template <typename T>
    bool loadListPtrObj(const string& key,boost::ptr_list<T>& vecObj,const int& begin=0,const int& end=-1);
    template <typename T>
    bool saveListPtrObj(const string& key,const boost::ptr_list<T>& vecObj,bool bAdd=false);

    template <typename T, template<typename T ,class Alloc = allocator<T> > class C>
    bool loadListObj(const string& key,C<T>& vecObj,const int& begin=0,const int& end=-1);
    template <typename T, template<typename T,class Alloc = allocator<T> > class C>
    bool saveListObj(const string& key,const C<T >& vecObj,bool bAdd=false);
    template <typename T>
    bool saveListObjSingle(const string& key,const int index,const T& obj );
    template <typename T>
    bool addListObjSingle(const string& key,const T& obj );

    //对二維数組的处理
    template<typename T>
    bool loadVector2d(const string& key,vector2d<T>& vecBasic);
    template<typename T>
    bool saveVector2d(const string& key,const vector2d<T>& vecBasic);

    template<typename T>
    bool loadTriVector2d(const string& key,vector2d<T>& vecBasic);
    template<typename T>
    bool saveTriVector2d(const string& key,const vector2d<T>& vecBasic);

    //map操作
    template<typename KEY,typename T>
    bool loadMapObj(const string& key,map<KEY,T>& vecBasic);
    template<typename KEY,typename T>
    bool saveMapObj(const string& key,const map<KEY,T>& vecBasic,bool bAdd=false);
    template<typename KEY,typename T>
    bool loadMapObjSingle(const string& key,const KEY& field,T& mapBasic);
    template<typename KEY,typename T>
    bool saveMapObjSingle(const string& key,const KEY& field,const T& mapBasic);

    //set操作
    template<typename T>
    bool loadUniqObj(const string& key,set<T>& uniqBasic);
    //加载map的key,值賦出默认值
    template<typename KEY,typename T,typename D>
    bool loadUniqMapKey(const string& key,map<KEY,T>& mapBasic,
                        const D& tmpDefault);

    //保存map的key值为uniq
    template<typename KEY,typename T>
    bool saveUniqMapKey(const string& key,const map<KEY,T>& mapBasic,bool bAdd=false);

    template<typename T>
    bool saveUniqObj(const string& key,const set<T>& uniqBasic,bool bAdd=false);
    template<typename T>
    bool saveUniqObjSingle(const string& key,const T& uniqBasic);
    template<typename T>
    bool delUniqObjSingle(const string& key,const T& uniqBasic);

    //单一类型
    template<typename T>
    bool loadObj(const string& key,T& obj);
    template<typename T>
    bool saveObj(const string& key, T& obj);
    //訂阅和发布
    template<typename T>
    bool publish(const string& channel,const T& msg);
};



template <typename T, template<typename T ,class Alloc = allocator<T> > class C>
bool JsonStandardCache::loadListObj(const string& key,C<T>& vecObj,const int& begin,const int& end)
{
    redisReply *reply = NULL;
    try {
        vecObj.clear();

        int lenEnd = getListLen(key);
        if(lenEnd < 0)
        {
            string osError =   " 解析表:" +  key + ",得到表大小为空!";
            LOG_INFO(osError.c_str());
            return true;
        }
        if(end > 0)
        {
            if(lenEnd > end)
            {
                lenEnd = end;
            }
        }

        int start = begin;
        int over = begin + REDIS_LOADSIZE - 1;
        if(over > lenEnd)
        {
            over = lenEnd;
        }

        for(;start < lenEnd;)
        {
            reply = (redisReply *)redisCommand(conn, "lrange %s %d  %d",key.c_str(),start,over);
            switch(reply->type )
            {
            case REDIS_REPLY_ARRAY:
                for (size_t i = 0; i < reply->elements; ++i)
                {
                    T myData;
                    if(JsonSerial::deSerialObj(myData,reply->element[i]->str))
                    {
                        vecObj.push_back(myData);
                    }
                    else
                    {
                        ostringstream osError;
                        osError <<  "ERROR 解析表:" <<  key + ",第" << i << "项错误!";
                        throw NorException(osError.str().c_str());
                    }
                }
                break;
            case REDIS_REPLY_NIL:
            default:
            {
                ostringstream osError;
                osError <<  "ERROR 解析表:" <<  key + ",项错误!";
                throw NorException(osError.str().c_str());
            }
                break;
            }
            freeReplyObject(reply);
            start += REDIS_LOADSIZE;
            over += REDIS_LOADSIZE;
            if(over > lenEnd)
            {
                over = lenEnd;
            }
        }

    } catch(std::exception& ex) {

        LOG_ERROR("deal with redis error:%s",ex.what());
        printf("%s", ex.what());
        if(reply)
        {
           freeReplyObject(reply);
        }
        return false;
    }
    return true;
}
template <typename T, template<typename T,class Alloc = allocator<T> > class C>
bool JsonStandardCache::saveListObj(const string& key,const C<T >& vecObj,bool bAdd)
{
    redisReply *reply = NULL;
    try {

        if(!bAdd)
        {
            if(!delKey(key))
            {
                return false;
            }
        }
        if(vecObj.empty())
        {
#ifdef _DEBUG
            printf("WARN table: %s,save is empty\n", key.c_str());
#endif
            return true;
        }
        ostringstream strFormator;

        StringBunch bunchString(REDIS_TRANSSIZE);
        //StringBunch bunchString(vecObj.size());
        strFormator << "rpush " << key;
        typename C<T>::const_iterator iterObj= vecObj.begin();
        int batSize= 0;
        string strVal;
        for(;iterObj != vecObj.end(); iterObj++)
        {
            const T& item = *iterObj;
            strFormator << " %s";
            JsonSerial::serialObj(strVal,item);
            bunchString.setValue(batSize++,strVal);
            if(batSize>=REDIS_TRANSSIZE)
            {
                reply = (redisReply *)redisCommandBunch(conn, strFormator.str().c_str(),bunchString.getBunchString());
                strFormator.str("");
                strFormator << "rpush  " << key;
                batSize = 0;
                int ret = false;
                switch(reply->type )
                {
                case REDIS_REPLY_INTEGER:
                    ret = reply->integer;
                    break;
                case REDIS_REPLY_STRING:
                    break;
                case REDIS_REPLY_ARRAY:
                    break;
                case REDIS_REPLY_NIL:
                    break;
                default:
                    break;
                }
                freeReplyObject(reply);
                if(!ret)
                {
                       LOG_ERROR("save listobj:%s error:%s",key.c_str(),reply->str);
                       return false;
                }
            }
        }

        if(batSize>0)
        {
            reply = (redisReply *)redisCommandBunch(conn, strFormator.str().c_str(),bunchString.getBunchString());
            int ret = false;
            switch(reply->type )
            {
            case REDIS_REPLY_INTEGER:
                ret = reply->integer;
                break;
            case REDIS_REPLY_STRING:
                break;
            case REDIS_REPLY_ARRAY:
                break;
            case REDIS_REPLY_NIL:
                break;
            default:
                break;
            }
            freeReplyObject(reply);
            return ret;
        }
    } catch(std::exception& ex) {
        LOG_ERROR("deal with redis error:%s",ex.what());
        printf("%s", ex.what());
        if(reply)
        {
           freeReplyObject(reply);
        }
        return false;
    }
    return true;
}

template <typename T>
bool JsonStandardCache::loadListPtrObj(const string& key,boost::ptr_list<T>& vecObj,const int& begin,const int& end)
{
    redisReply *reply = NULL;
    try {
        vecObj.clear();
        int lenEnd = getListLen(key);
        if(lenEnd < 0)
        {
            string osError =   " 解析表:" +  key + ",得到表大小为空!";
            LOG_INFO(osError.c_str());
            return true;
        }
        if(end > 0)
        {
            if(lenEnd > end)
            {
                lenEnd = end;
            }
        }


        int start = begin;
        int over = begin + REDIS_LOADSIZE - 1;
        if(over > lenEnd)
        {
            over = lenEnd;
        }

        for(;start < lenEnd;)
        {
            reply = (redisReply *)redisCommand(conn, "lrange %s %d  %d",key.c_str(),start,over);
            switch(reply->type )
            {
            case REDIS_REPLY_ARRAY:
                for (size_t i = 0; i < reply->elements; ++i)
                {
                    T* myData;
                    if(JsonSerial::deSerialObj(myData,reply->element[i]->str))
                    {
                        vecObj.push_back(myData);
                    }
                    else
                    {
                        ostringstream osError;
                        osError <<  "ERROR 解析表:" <<  key + ",第" << i << "项错误!";
                        throw NorException(osError.str().c_str());
                    }
                }
                break;
            case REDIS_REPLY_NIL:
            default:
            {
                ostringstream osError;
                osError <<  "ERROR 解析表:" <<  key + ",项错误!";
                throw NorException(osError.str().c_str());
            }
                break;
            }
            freeReplyObject(reply);
            start += REDIS_LOADSIZE;
            over += REDIS_LOADSIZE;
            if(over > lenEnd)
            {
                over = lenEnd;
            }
        }

    } catch(std::exception& ex) {

        LOG_ERROR("deal with redis error:%s",ex.what());
        printf("%s", ex.what());
        if(reply)
        {
           freeReplyObject(reply);
        }
        return false;
    }
    return true;
}
template <typename T>
bool JsonStandardCache::saveListPtrObj(const string& key,const boost::ptr_list<T>& vecObj,bool bAdd)
{
    redisReply *reply = NULL;
    try {

        if(!bAdd)
        {
            if(!delKey(key))
            {
                return false;
            }
        }
        if(vecObj.empty())
        {
#ifdef _DEBUG
            printf("WARN table: %s,save is empty\n", key.c_str());
#endif
            return true;
        }
        ostringstream strFormator;

        StringBunch bunchString(REDIS_TRANSSIZE);
        //StringBunch bunchString(vecObj.size());
        strFormator << "rpush " << key;
        typename boost::ptr_list<T>::const_iterator iterObj = vecObj.begin();
        int batSize= 0;
        string strVal;
        for(;iterObj != vecObj.end(); iterObj++)
        {
            const T& item = *iterObj;
            strFormator << " %s";
            JsonSerial::serialObj(strVal,item);
            bunchString.setValue(batSize++,strVal);
            if(batSize>=REDIS_TRANSSIZE)
            {
                reply = (redisReply *)redisCommandBunch(conn, strFormator.str().c_str(),bunchString.getBunchString());
                strFormator.str("");
                strFormator << "rpush  " << key;
                batSize = 0;
                int ret = false;
                switch(reply->type )
                {
                case REDIS_REPLY_INTEGER:
                    ret = reply->integer;
                    break;
                case REDIS_REPLY_STRING:
                    break;
                case REDIS_REPLY_ARRAY:
                    break;
                case REDIS_REPLY_NIL:
                    break;
                default:
                    break;
                }
                freeReplyObject(reply);
                if(!ret)
                {
                       LOG_ERROR("save listobj:%s error:%s",key.c_str(),reply->str);
                       return false;
                }
            }
        }

        if(batSize>0)
        {
            reply = (redisReply *)redisCommandBunch(conn, strFormator.str().c_str(),bunchString.getBunchString());
            int ret = false;
            switch(reply->type )
            {
            case REDIS_REPLY_INTEGER:
                ret = reply->integer;
                break;
            case REDIS_REPLY_STRING:
                break;
            case REDIS_REPLY_ARRAY:
                break;
            case REDIS_REPLY_NIL:
                break;
            default:
                break;
            }
            freeReplyObject(reply);
            return ret;
        }
    } catch(std::exception& ex) {
        LOG_ERROR("deal with redis error:%s",ex.what());
        printf("%s", ex.what());
        if(reply)
        {
           freeReplyObject(reply);
        }
        return false;
    }
    return true;
}

//对二維数組的处理
template<typename T>
bool JsonStandardCache::loadVector2d(const string& key,
                                     vector2d<T>& vecBasic)
{
    redisReply *reply = NULL;
    try {

        uint lenEnd = getListLen(key);
        if(lenEnd < 0)
        {
            vecBasic.resize(0,0);
            return 0;
        }

        uint start = 0;
        uint over = start + REDIS_LOADSIZE - 1;
        if(over > lenEnd)
        {
            over = lenEnd;
        }
        if(start < lenEnd)
        {
            reply = (redisReply *)redisCommand(conn, "lrange %s %d  %d",key.c_str(),start,over);
            bool ret = true;
            ParaTwo<int> paraSize;
            uint x = 0;
            uint y = 0;
            switch(reply->type )
            {
            case REDIS_REPLY_ARRAY:
                if(reply->elements < 1)
                {
                    //assert(0);
                    ostringstream osError;
                    osError <<  "ERROR 解析表:" <<  key + ",二維数組是空！";
                    return false;
                }
                //读取数組大小
                if(JsonSerial::deSerialObj(paraSize,reply->element[0]->str))
                {
                    vecBasic.resize(paraSize.getFirst(),paraSize.getSecond());
                }
                else
                {
                    assert(0);
                    ostringstream osError;
                    osError <<  "ERROR pase table:" <<  key + ",get array size error！";
                    throw NorException(osError.str().c_str());
                }
                if(lenEnd !=  (size_t)vecBasic.getCol() * (size_t)vecBasic.getRow() + 1)
                {
                    assert(0);
                    ostringstream osError;
                    osError <<  "ERROR pase table:" <<  key + ",data and array size not match！";
                    throw NorException(osError.str().c_str());
                }

                for (size_t i = 1; i < reply->elements; ++i)
                {
                    T myData;
                    if(JsonSerial::deSerialObj(myData,reply->element[i]->str))
                    {
                        vecBasic(x,y++) = myData;
                        if(y >= (uint)vecBasic.getCol())
                        {
                            y = 0;
                            x++;
                        }
                    }
                    else
                    {
                        assert(0);
                        ostringstream osError;
                        osError <<  "ERROR  pase table:" <<  key + ",count:" << i - 1 << " item error!";
                        throw NorException(osError.str().c_str());
                    }
                }
                break;
            case REDIS_REPLY_NIL:
                break;
            default:
                ret = false;
                break;
            }
            freeReplyObject(reply);
            start += REDIS_LOADSIZE;
            over += REDIS_LOADSIZE;
            if(over > lenEnd)
            {
                over = lenEnd;
            }
            while(start < lenEnd)
            {
                 reply = (redisReply *)redisCommand(conn, "lrange %s %d  %d",key.c_str(),start,over);
                 switch(reply->type )
                 {
                 case REDIS_REPLY_ARRAY:
                     for (size_t i = 0; i < reply->elements; ++i)
                     {
                         T myData;
                         if(JsonSerial::deSerialObj(myData,reply->element[i]->str))
                         {
                             vecBasic(x,y++) = myData;
                             if(y >= (uint)vecBasic.getCol())
                             {
                                 y = 0;
                                 x++;
                             }
                         }
                         else
                         {
                             assert(0);
                             ostringstream osError;
                             osError <<  "ERROR  pase table:" <<  key + ",count:" << i - 1 << " item error!";
                             throw NorException(osError.str().c_str());
                         }
                     }
                     break;
                 case REDIS_REPLY_NIL:
                     break;
                 default:
                     ret = false;
                     break;
                 }
                 freeReplyObject(reply);
                 start += REDIS_LOADSIZE;
                 over += REDIS_LOADSIZE;
                 if(over > lenEnd)
                 {
                     over = lenEnd;
                 }
            }
        }

    } catch(std::exception& ex) {

        LOG_ERROR("deal with redis error:%s",ex.what());
        printf("%s\n", ex.what());
        if(reply)
        {
           freeReplyObject(reply);
        }
        return false;
    }
    return true;
}

template<typename T>
bool JsonStandardCache::saveVector2d(const string& key,
                                     const vector2d<T>& vecBasic)
{
    redisReply *reply = NULL;
    try {
        if(!delKey(key))
        {
            return false;
        }

        if(vecBasic.getCol() <= 0 || vecBasic.getRow() <= 0)
        {
#ifdef _DEBUG
            printf("WARN table: %s,save is empty\n", key.c_str());
#endif
            return true;
        }
        ostringstream strFormator;
        StringBunch bunchString(REDIS_TRANSSIZE);
        strFormator << "rpush " << key;
        strFormator << " %s";

        ParaTwo<int> paraSize(vecBasic.getRow(),vecBasic.getCol());
        string strSize;
        JsonSerial::serialObj(strSize,paraSize);
        bunchString.setValue(0,strSize);
        int sizeBunch = 1;
        string strVal;
        for(uint x = 0; x < vecBasic.getRow(); x++)
        {
            for(uint y = 0; y < vecBasic.getCol(); y++)
            {
                const T& item = vecBasic(x,y);
                strFormator << " %s";
                JsonSerial::serialObj(strVal,item);
                bunchString.setValue(sizeBunch++,strVal);
                if(sizeBunch >= REDIS_TRANSSIZE)
                {
                    reply = (redisReply *)redisCommandBunch(conn, strFormator.str().c_str(),
                                                            bunchString.getBunchString());

                    strFormator.str("");
                    strFormator << "rpush " << key;
                    sizeBunch  = 0;
                    int ret = -1;
                    switch(reply->type )
                    {
                    case REDIS_REPLY_INTEGER:
                        ret = reply->integer;
                        break;
                    case REDIS_REPLY_STRING:
                        break;
                    case REDIS_REPLY_ARRAY:
                        break;
                    case REDIS_REPLY_NIL:
                        break;
                    default:
                        break;
                    }
                    freeReplyObject(reply);
                    if(ret < 0)
                    {
                        LOG_ERROR("save vector2d:%s error:%s,x:%d,y:%d",key.c_str(),reply->str,x,y);
                        if(reply)
                        {
                           freeReplyObject(reply);
                        }
                        return false;
                    }
                }
            }
        }

        //保存最后几項
         int ret = -1;
        if(sizeBunch > 0)
        {
            reply = (redisReply *)redisCommandBunch(conn, strFormator.str().c_str(),
                                                    bunchString.getBunchString());
            switch(reply->type )
            {
            case REDIS_REPLY_INTEGER:
                ret = reply->integer;
                break;
            case REDIS_REPLY_STRING:
                break;
            case REDIS_REPLY_ARRAY:
                break;
            case REDIS_REPLY_NIL:
                break;
            default:
                break;
            }
            freeReplyObject(reply);
            if(ret < 0)
            {
                 LOG_ERROR("save vector2d :%s,last item",reply->str);
                if(reply)
                {
                   freeReplyObject(reply);
                }
                return false;
            }
        }
        return ret >= 0;

    } catch(std::exception& ex) {
        LOG_ERROR("deal with redis error:%s",ex.what());
        printf("%s", ex.what());
        if(reply)
        {
           freeReplyObject(reply);
        }
        return false;
    }
    return true;
}

//对二維数組的处理,存半，三角对称
template<typename T>
bool JsonStandardCache::loadTriVector2d(const string& key,
                                     vector2d<T>& vecBasic)
{
    redisReply *reply = NULL;
    try {

        uint lenEnd = getListLen(key);
        if(lenEnd < 0)
        {
            vecBasic.resize(0,0);
            return 0;
        }

        uint start = 0;
        uint over = start + REDIS_LOADSIZE - 1;
        if(over > lenEnd)
        {
            over = lenEnd;
        }
        if(start < lenEnd)
        {
            reply = (redisReply *)redisCommand(conn, "lrange %s %d  %d",key.c_str(),start,over);
            bool ret = true;
            ParaTwo<int> paraSize;
            uint x = 0;
            uint y = 0;
            switch(reply->type )
            {
            case REDIS_REPLY_ARRAY:
                if(reply->elements < 1)
                {
                    //assert(0);
                    ostringstream osError;
                    osError <<  "ERROR 解析表:" <<  key + ",二維数組是空！";
                    return false;
                }
                //读取数組大小
                if(JsonSerial::deSerialObj(paraSize,reply->element[0]->str))
                {
                    if(paraSize.getFirst() != paraSize.getSecond())
                    {
                        ostringstream osError;
                        osError <<  "ERROR pase table:" <<  key + ",get array size must N*N！";
                        throw NorException(osError.str().c_str());
                    }
                    vecBasic.resize(paraSize.getFirst(),paraSize.getSecond());
                }
                else
                {
                    ostringstream osError;
                    osError <<  "ERROR pase table:" <<  key + ",get array size error！";
                    throw NorException(osError.str().c_str());
                }
                if(lenEnd !=  (size_t)(vecBasic.getCol() + 1) * vecBasic.getCol() / 2)
                {
                    ostringstream osError;
                    osError <<  "ERROR pase table:" <<  key + ",data and array size not match！";
                    throw NorException(osError.str().c_str());
                }

                for (size_t i = 1; i < reply->elements; ++i)
                {
                    T myData;
                    if(JsonSerial::deSerialObj(myData,reply->element[i]->str))
                    {
                        vecBasic(x,y) = myData;
                        if(x != y)
                        {
                            vecBasic(y,x) = myData;
                        }
                        ++y;
                        if(y >  x)
                        {
                            y = 0;
                            x++;
                            if( x >= vecBasic.getCol())
                            {
                                ostringstream osError;
                                osError <<  "ERROR pase table:" <<  key + ",size too big!";
                                throw NorException(osError.str().c_str());
                            }
                        }
                    }
                    else
                    {
                        assert(0);
                        ostringstream osError;
                        osError <<  "ERROR  pase table:" <<  key + ",count:" << i - 1 << " item error!";
                        throw NorException(osError.str().c_str());
                    }
                }
                break;
            case REDIS_REPLY_NIL:
                break;
            default:
                ret = false;
                break;
            }
            freeReplyObject(reply);
            start += REDIS_LOADSIZE;
            over += REDIS_LOADSIZE;
            if(over > lenEnd)
            {
                over = lenEnd;
            }
            while(start < lenEnd)
            {
                 reply = (redisReply *)redisCommand(conn, "lrange %s %d  %d",key.c_str(),start,over);
                 switch(reply->type )
                 {
                 case REDIS_REPLY_ARRAY:
                     for (size_t i = 0; i < reply->elements; ++i)
                     {
                         T myData;
                         if(JsonSerial::deSerialObj(myData,reply->element[i]->str))
                         {
                             vecBasic(x,y) = myData;
                             if(x != y)
                             {
                                 vecBasic(y,x) = myData;
                             }
                             ++y;
                             if(y >  x)
                             {
                                 y = 0;
                                 x++;
                                 if( x >= vecBasic.getCol())
                                 {
                                     ostringstream osError;
                                     osError <<  "ERROR pase table:" <<  key + ",size too big!";
                                     throw NorException(osError.str().c_str());
                                 }
                             }
                         }
                         else
                         {
                             assert(0);
                             ostringstream osError;
                             osError <<  "ERROR  pase table:" <<  key + ",count:" << i - 1 << " item error!";
                             throw NorException(osError.str().c_str());
                         }
                     }
                     break;
                 case REDIS_REPLY_NIL:
                     break;
                 default:
                     ret = false;
                     break;
                 }
                 freeReplyObject(reply);
                 start += REDIS_LOADSIZE;
                 over += REDIS_LOADSIZE;
                 if(over > lenEnd)
                 {
                     over = lenEnd;
                 }
            }
        }

    } catch(std::exception& ex) {

        LOG_ERROR("deal with redis error:%s",ex.what());
        printf("%s\n", ex.what());
        if(reply)
        {
           freeReplyObject(reply);
        }
        return false;
    }
    return true;
}

template<typename T>
bool JsonStandardCache::saveTriVector2d(const string& key,
                                     const vector2d<T>& vecBasic)
{
    redisReply *reply = NULL;
    try {
        if(vecBasic.getCol() != vecBasic.getRow() || !delKey(key))
        {
            return false;
        }

        if(vecBasic.getCol() <= 0 || vecBasic.getRow() <= 0)
        {
#ifdef _DEBUG
            printf("WARN table: %s,save is empty\n", key.c_str());
#endif
            return true;
        }
        ostringstream strFormator;
        StringBunch bunchString(REDIS_TRANSSIZE);
        strFormator << "rpush " << key;
        strFormator << " %s";

        ParaTwo<int> paraSize(vecBasic.getRow(),vecBasic.getCol());
        string strSize;
        JsonSerial::serialObj(strSize,paraSize);
        bunchString.setValue(0,strSize);
        int sizeBunch = 1;
        string strVal;
        for(uint x = 0; x < vecBasic.getCol(); x++)
        {
            for(uint y = 0; y <= x; y++)
            {
                const T& item = vecBasic(x,y);
                strFormator << " %s";
                JsonSerial::serialObj(strVal,item);
                bunchString.setValue(sizeBunch++,strVal);
                if(sizeBunch >= REDIS_TRANSSIZE)
                {
                    reply = (redisReply *)redisCommandBunch(conn, strFormator.str().c_str(),
                                                            bunchString.getBunchString());

                    strFormator.str("");
                    strFormator << "rpush " << key;
                    sizeBunch  = 0;
                    int ret = -1;
                    switch(reply->type )
                    {
                    case REDIS_REPLY_INTEGER:
                        ret = reply->integer;
                        break;
                    case REDIS_REPLY_STRING:
                        break;
                    case REDIS_REPLY_ARRAY:
                        break;
                    case REDIS_REPLY_NIL:
                        break;
                    default:
                        break;
                    }
                    freeReplyObject(reply);
                    if(ret < 0)
                    {
                        LOG_ERROR("save vector2d:%s error:%s,x:%d,y:%d",key.c_str(),reply->str,x,y);
                        if(reply)
                        {
                           freeReplyObject(reply);
                        }
                        return false;
                    }
                }
            }
        }

        //保存最后几項
         int ret = -1;
        if(sizeBunch > 0)
        {
            reply = (redisReply *)redisCommandBunch(conn, strFormator.str().c_str(),
                                                    bunchString.getBunchString());
            switch(reply->type )
            {
            case REDIS_REPLY_INTEGER:
                ret = reply->integer;
                break;
            case REDIS_REPLY_STRING:
                break;
            case REDIS_REPLY_ARRAY:
                break;
            case REDIS_REPLY_NIL:
                break;
            default:
                break;
            }
            freeReplyObject(reply);
            if(ret < 0)
            {
                 LOG_ERROR("save vector2d :%s,last item",reply->str);
                if(reply)
                {
                   freeReplyObject(reply);
                }
                return false;
            }
        }
        return ret >= 0;

    } catch(std::exception& ex) {
        LOG_ERROR("deal with redis error:%s",ex.what());
        printf("%s", ex.what());
        if(reply)
        {
           freeReplyObject(reply);
        }
        return false;
    }
    return true;
}

template <typename T>
bool JsonStandardCache::addListObjSingle(const string& key,const T& obj )
{
    redisReply *reply = NULL;
    try {
        string strVal = "";
        JsonSerial::serialObj(strVal,obj);

        reply = (redisReply *)redisCommand(conn, "rpush %s %s",key.c_str(),strVal.c_str());
        //printf("type: %d", reply->type);
        //1-REDIS_REPLY_STRING
        //2-REDIS_REPLY_ARRAY
        //3-REDIS_REPLY_INTEGER
        //4-REDIS_REPLY_NIL
        //5-REDIS_REPLY_STATUS
        //6-REDIS_REPLY_ERROR

        bool ret = false;
        switch(reply->type )
        {
        case REDIS_REPLY_INTEGER:
            //ret = reply->integer;
            ret = true;
            break;
        case REDIS_REPLY_STRING:
            break;
        case REDIS_REPLY_ARRAY:
            break;
        case REDIS_REPLY_NIL:
            break;
        default:
            break;
        }
        freeReplyObject(reply);
        return ret;

    } catch(std::exception& ex) {
        LOG_ERROR("deal with redis error:%s",ex.what());
        printf("%s", ex.what());
        if(reply)
        {
           freeReplyObject(reply);
        }
        return false;
    }
    return true;
}

template <typename T>
bool JsonStandardCache::saveListObjSingle(const string& key,const int index,const T& obj )
{
    redisReply *reply = NULL;
    try {
        string strVal;
        string straa = typeid(T).name();
        JsonSerial::serialObj(strVal,obj);
        reply = (redisReply *)redisCommand(conn, "LSET %s %d %s",key.c_str(),index,strVal.c_str());

        bool ret = 0;
        switch(reply->type )
        {
        case REDIS_REPLY_STATUS:
            if(0 == strcmp("OK",reply->str))
            {
                ret = true;
            }
            break;
        case REDIS_REPLY_INTEGER:
            break;
        case REDIS_REPLY_STRING:
            break;
        case REDIS_REPLY_ARRAY:
            break;
        case REDIS_REPLY_NIL:
            break;
        default:
            break;
        }
        freeReplyObject(reply);
        return ret;

    } catch(std::exception& ex) {
        printf("%s", ex.what());
        if(reply)
        {
           freeReplyObject(reply);
        }
        return false;
    }
    return true;
}


template<typename KEY,typename T>
bool JsonStandardCache::loadMapObj(const string& key,map<KEY,T>& mapObj)
{
    redisReply *reply = NULL;
    try {
        mapObj.clear();
#if REDIS_UP
        int iCursor = 0;
        do
        {
            reply = (redisReply *)redisCommand(conn, "hscan %s %d",key.c_str(),iCursor);
            if(REDIS_REPLY_ARRAY != reply->type || 2 != reply->elements)
            {
                ostringstream osError;
                osError <<  "ERROR 解析表:" <<  key + ",打描" << iCursor << "项错误!";
                throw  CacheException(osError.str().c_str());
            }
            iCursor = atoi(reply->element[0]->str);
            redisReply* dataReply = reply->element[1];
            bool ret = false;
            switch(dataReply->type)
            {
            case REDIS_REPLY_ARRAY:
                for (size_t i = 0; i < dataReply->elements; i += 2)
                {
                    T myData;
                    if(JsonSerial::deSerialObj(myData,dataReply->element[i + 1]->str))
                    {
                        mapObj.insert(pair<KEY,T>(boost::lexical_cast<KEY>(dataReply->element[i]->str),myData));
                    }
                    else
                    {
                        ostringstream osError;
                        osError <<  "ERROR 解析表:" <<  key + ",第" << i << "项错误!";
                        throw  CacheException(osError.str().c_str());
                    }

                }
                ret = true;
                break;
            case REDIS_REPLY_NIL:
            default:
            {
                ostringstream osError;
                osError <<  "ERROR 解析表:" <<  key + ",错误!";
                throw  CacheException(osError.str().c_str());
            }
                break;
            }
            freeReplyObject(reply);
        }
        while(iCursor > 0);

 #else
        reply = (redisReply *)redisCommand(conn, "hgetall %s",key.c_str());
        bool ret = false;
//        if(!reply)//redis内存数据交换太快
//        {
//             sleep(1);
//             reply = (redisReply *)redisCommand(conn, "hgetall %s",key.c_str());
//             sleep(1);
//        }
        switch(reply->type )
        {
        case REDIS_REPLY_ARRAY:
            for (size_t i = 0; i < reply->elements; i += 2)
            {
                T myData;
                if(JsonSerial::deSerialObj(myData,reply->element[i + 1]->str))
                {
                    mapObj.insert(pair<KEY,T>(boost::lexical_cast<KEY>(reply->element[i]->str),myData));
                }
                else
                {
                    ostringstream osError;
                    osError <<  "ERROR 解析表:" <<  key + ",第" << i << "项错误!";
                    throw  NorException(osError.str().c_str());
                }

            }
            ret = true;
            break;
        case REDIS_REPLY_NIL:
            break;
        default:
            ret = false;
            break;
        }
        freeReplyObject(reply);
        return ret;
#endif
    } catch(std::exception& ex) {
        LOG_ERROR("deal with redis error:%s",ex.what());
        printf("%s", ex.what());
        if(reply)
        {
           freeReplyObject(reply);
        }
        return false;
    }
    return true;
}
template<typename KEY,typename T>
bool JsonStandardCache::saveMapObj(const string& key,const map<KEY,T>& mapObj,bool bAdd)
{
    redisReply *reply = NULL;
    try {
        if(!bAdd)
        {
            if(!delKey(key))
            {
                return false;
            }
        }
        if(mapObj.empty())
        {
#ifdef _DEBUG
            printf("WARN table: %s,save is empty\n", key.c_str());
#endif
            return true;
        }
        ostringstream strFormator;
        StringBunch bunchString(REDIS_TRANSSIZE);
        strFormator << "HMSET " << key;
        typename map<KEY,T>::const_iterator iterObj= mapObj.begin();
        int batSize = 0;
        string strObj;
        for(;iterObj != mapObj.end(); iterObj++)
        {
            strFormator << " %s %s";
            bunchString.setValue(batSize++,boost::lexical_cast<string>(iterObj->first));
            JsonSerial::serialObj(strObj,iterObj->second);
            bunchString.setValue(batSize++,strObj);
            if(batSize >= REDIS_TRANSSIZE)
            {
                reply = (redisReply *)redisCommandBunch(conn, strFormator.str().c_str(),bunchString.getBunchString());
                bool ret = false;
                switch(reply->type )
                {
                case REDIS_REPLY_STATUS:
                    if(0 == strcmp("OK",reply->str))
                    {
                        ret = true;
                    }
                    break;
                case REDIS_REPLY_INTEGER:
                    break;
                case REDIS_REPLY_STRING:
                    break;
                case REDIS_REPLY_ARRAY:
                    break;
                case REDIS_REPLY_NIL:
                    break;
                default:
                    break;
                }
                freeReplyObject(reply);
                strFormator.str("");
                strFormator << "HMSET " << key;
                batSize = 0;
                if(!ret)
                {
                     LOG_ERROR("save mapobj:%s error:%s",key.c_str(),reply->str);
                    return false;
                }
            }
        }

        if(batSize > 0)
        {
            reply = (redisReply *)redisCommandBunch(conn, strFormator.str().c_str(),bunchString.getBunchString());
            bool ret = false;
            switch(reply->type )
            {
            case REDIS_REPLY_STATUS:
                if(0 == strcmp("OK",reply->str))
                {
                    ret = true;
                }
                break;
            case REDIS_REPLY_INTEGER:
                break;
            case REDIS_REPLY_STRING:
                break;
            case REDIS_REPLY_ARRAY:
                break;
            case REDIS_REPLY_NIL:
                break;
            default:
                break;
            }
            freeReplyObject(reply);
            return ret;
        }
        return true;
    } catch(std::exception& ex) {
        LOG_ERROR("deal with redis error:%s",ex.what());
        printf("%s", ex.what());
        if(reply)
        {
           freeReplyObject(reply);
        }
        return false;
    }
    return false;
}


template<typename KEY,typename T>
bool JsonStandardCache::loadMapObjSingle(const string& key,const KEY& field,T& mapObj)
{
    redisReply *reply = NULL;
    try {
        ostringstream strFormator;
        strFormator << "HGET  " << key << " " << field;
        reply = (redisReply *)redisCommand(conn, strFormator.str().c_str());

        bool ret = false;
        switch(reply->type )
        {
        case REDIS_REPLY_INTEGER:
            break;
        case REDIS_REPLY_STRING:
        {
            if(!JsonSerial::deSerialObj(mapObj,reply->str))
            {
                ostringstream osError;
                osError <<  "ERROR 解析表:" <<  key + ",错误!";
                throw NorException(osError.str().c_str());
            }
            ret = true;
        }
            break;
        case REDIS_REPLY_ARRAY:
            break;
        case REDIS_REPLY_NIL:
            ret = true;
            break;
        default:
            break;
        }
        if(!ret)
        {
            throw NorException(reply->str);
        }
        freeReplyObject(reply);
        return ret;

    } catch(std::exception& ex) {
        LOG_ERROR("deal with redis error:%s",ex.what());
        printf("%s", ex.what());
        if(reply)
        {
           freeReplyObject(reply);
        }
        return false;
    }
    return true;
}

template<typename KEY,typename T>
bool JsonStandardCache::saveMapObjSingle(const string& key,const KEY& field,
                                         const T& basic)
{
    redisReply *reply = NULL;
    try {
        //typename JsonConvertor<T>::ToStringFunc jsonConvertor;
        string strField = boost::lexical_cast<string>(field);

        string strObj;
        JsonSerial::serialObj(strObj,basic);
        reply = (redisReply *)redisCommand(conn, "HSET %s %s %s",
                                           key.c_str(),strField.c_str(),
                                           strObj.c_str());

        bool bRet = false;
        switch(reply->type )
        {
        case REDIS_REPLY_INTEGER:
            //ret = reply->integer;
            bRet = true;
            break;
        case REDIS_REPLY_STRING:
            break;
        case REDIS_REPLY_ARRAY:
            break;
        case REDIS_REPLY_NIL:
            break;
        default:
            break;
        }
        freeReplyObject(reply);
        return bRet;

    } catch(std::exception& ex) {
        LOG_ERROR("deal with redis error:%s",ex.what());
        printf("%s", ex.what());
        if(reply)
        {
           freeReplyObject(reply);
        }
        return false;
    }
    return true;
}

//set操作
template<typename T>
bool JsonStandardCache::loadUniqObj(const string& key,set<T>& uniqBasic)
{
    redisReply *reply = NULL;
    try {
        uniqBasic.clear();
        reply = (redisReply *)redisCommand(conn, "SMEMBERS  %s",key.c_str());
        //printf("type: %d", reply->type);
        //1-REDIS_REPLY_STRING
        //2-REDIS_REPLY_ARRAY
        //3-REDIS_REPLY_INTEGER
        //4-REDIS_REPLY_NIL
        //5-REDIS_REPLY_STATUS
        //6-REDIS_REPLY_ERROR

        bool ret = true;
        switch(reply->type )
        {
        case REDIS_REPLY_ARRAY:
            for (size_t i = 0; i < reply->elements; ++i)
            {
                T myData;
                if(JsonSerial::deSerialObj(myData,reply->element[i]->str))
                {
                    uniqBasic.insert(myData);
                }
                else
                {
                    ostringstream osError;
                    osError <<  "ERROR 解析表:" <<  key + ",第" << i << "项错误!";
                    throw NorException(osError.str().c_str());
                }
            }
            break;
        case REDIS_REPLY_NIL:
            break;
        default:
            ret = false;
            break;
        }
        freeReplyObject(reply);
        return ret;

    } catch(std::exception& ex) {
        LOG_ERROR("deal with redis error:%s",ex.what());
        printf("%s", ex.what());
        if(reply)
        {
           freeReplyObject(reply);
        }
        return false;
    }
    return true;
}

template<typename KEY,typename T,typename D>
bool JsonStandardCache::loadUniqMapKey(const string& key,
                                       map<KEY,T>& mapBasic,
                                       const D& tmpDefault)
{
    redisReply *reply = NULL;
    try {
        mapBasic.clear();
        reply = (redisReply *)redisCommand(conn, "SMEMBERS  %s",key.c_str());
        //printf("type: %d", reply->type);
        //1-REDIS_REPLY_STRING
        //2-REDIS_REPLY_ARRAY
        //3-REDIS_REPLY_INTEGER
        //4-REDIS_REPLY_NIL
        //5-REDIS_REPLY_STATUS
        //6-REDIS_REPLY_ERROR

        bool ret = true;
        T tmpData = tmpDefault;
        switch(reply->type )
        {
        case REDIS_REPLY_ARRAY:
            for (size_t i = 0; i < reply->elements; ++i)
            {
                KEY myKey;
                if(JsonSerial::deSerialObj(myKey,reply->element[i]->str))
                {
                    mapBasic.insert(pair<KEY,T>(myKey,tmpData));
                }
                else
                {
                    ostringstream osError;
                    osError <<  "ERROR 解析表:" <<  key + ",第" << i << "项错误!";
                    throw NorException(osError.str().c_str());
                }
            }
            break;
        case REDIS_REPLY_NIL:
            break;
        default:
            ret = false;
            break;
        }
        freeReplyObject(reply);
        return ret;

    } catch(std::exception& ex) {
        LOG_ERROR("deal with redis error:%s",ex.what());
        printf("%s", ex.what());
        if(reply)
        {
           freeReplyObject(reply);
        }
        return false;
    }
    return true;
}

template<typename KEY,typename T>
bool JsonStandardCache::saveUniqMapKey(const string& key,
                                       const map<KEY,T>& mapBasic,bool bAdd)
{
    redisReply *reply = NULL;
    try {

        if(!bAdd)
        {
            if(!delKey(key))
            {
                return false;
            }
        }
        if(mapBasic.empty())
        {
#ifdef _DEBUG
            printf("WARN table: %s,save is empty\n", key.c_str());
#endif
            return true;
        }
        ostringstream strFormator;
        StringBunch bunchString(mapBasic.size());
        strFormator << "SADD  " << key;
        typename map<KEY,T>::const_iterator iterObj= mapBasic.begin();
        int i = 0;
        string strObj;
        for(;iterObj != mapBasic.end(); iterObj++)
        {
            const KEY& itemKey = iterObj->first;
            strFormator << " %s";
            JsonSerial::serialObj(strObj,itemKey);
            bunchString.setValue(i++,strObj);
        }

        reply = (redisReply *)redisCommandBunch(conn, strFormator.str().c_str(),
                                                bunchString.getBunchString());
        //printf("type: %d", reply->type);
        //1-REDIS_REPLY_STRING
        //2-REDIS_REPLY_ARRAY
        //3-REDIS_REPLY_INTEGER
        //4-REDIS_REPLY_NIL
        //5-REDIS_REPLY_STATUS
        //6-REDIS_REPLY_ERROR

        int ret = false;
        switch(reply->type )
        {
        case REDIS_REPLY_INTEGER:
            ret = reply->integer;
            break;
        case REDIS_REPLY_STRING:
            break;
        case REDIS_REPLY_ARRAY:
            break;
        case REDIS_REPLY_NIL:
            break;
        default:
            break;
        }
        freeReplyObject(reply);
        return ret;

    } catch(std::exception& ex) {
        LOG_ERROR("deal with redis error:%s",ex.what());
        printf("%s", ex.what());
        if(reply)
        {
           freeReplyObject(reply);
        }
        return false;
    }
    return true;
}

template<typename T>
bool JsonStandardCache::saveUniqObj(const string& key,const set<T>& uniqBasic,bool bAdd)
{
    redisReply *reply = NULL;
    try {

        if(!bAdd)
        {
            if(!delKey(key))
            {
                return false;
            }
        }
        if(uniqBasic.empty())
        {
#ifdef _DEBUG
            printf("WARN table: %s,save is empty\n", key.c_str());
#endif
            return true;
        }
        ostringstream strFormator;
        StringBunch bunchString(REDIS_TRANSSIZE);
        strFormator << "SADD  " << key;
        typename set<T>::const_iterator iterObj= uniqBasic.begin();
        int batSize = 0;
        string strObj;
        for(;iterObj != uniqBasic.end(); iterObj++)
        {
            const T& item = *iterObj;
            strFormator << " %s";
            JsonSerial::serialObj(strObj,item);
            bunchString.setValue(batSize++,strObj);

            if(batSize >= REDIS_TRANSSIZE)
            {
                reply = (redisReply *)redisCommandBunch(conn, strFormator.str().c_str(),bunchString.getBunchString());

                strFormator.str("");
                strFormator << "SADD  " << key;
                batSize = 0;

                int ret = false;
                switch(reply->type )
                {
                case REDIS_REPLY_INTEGER:
                    ret = reply->integer;
                    break;
                case REDIS_REPLY_STRING:
                    break;
                case REDIS_REPLY_ARRAY:
                    break;
                case REDIS_REPLY_NIL:
                    break;
                default:
                    break;
                }
                freeReplyObject(reply);
                if(!ret)
                {
                    LOG_ERROR("save uniqobj:%s error:%s",key.c_str(),reply->str);
                   return false;
                }
            }
        }

        if(batSize > 0)
        {
            reply = (redisReply *)redisCommandBunch(conn, strFormator.str().c_str(),bunchString.getBunchString());
            //printf("type: %d", reply->type);
            //1-REDIS_REPLY_STRING
            //2-REDIS_REPLY_ARRAY
            //3-REDIS_REPLY_INTEGER
            //4-REDIS_REPLY_NIL
            //5-REDIS_REPLY_STATUS
            //6-REDIS_REPLY_ERROR

            int ret = false;
            switch(reply->type )
            {
            case REDIS_REPLY_INTEGER:
                ret = reply->integer;
                break;
            case REDIS_REPLY_STRING:
                break;
            case REDIS_REPLY_ARRAY:
                break;
            case REDIS_REPLY_NIL:
                break;
            default:
                break;
            }
            freeReplyObject(reply);
           return ret;
        }
        return true;

    } catch(std::exception& ex) {
        LOG_ERROR("deal with redis error:%s",ex.what());
        printf("%s", ex.what());
        if(reply)
        {
           freeReplyObject(reply);
        }
        return false;
    }
    return true;
}

template<typename T>
bool JsonStandardCache::saveUniqObjSingle(const string& key,const T& uniqBasic)
{
    redisReply *reply = NULL;
    try {
        string strVal = boost::lexical_cast<string>(uniqBasic);

        reply = (redisReply *)redisCommand(conn, "SADD %s %s",key.c_str(),strVal.c_str());

        bool bRet = false;
        switch(reply->type )
        {
        case REDIS_REPLY_INTEGER:
            //ret = reply->integer;
            bRet = true;
            break;
        case REDIS_REPLY_STRING:
            break;
        case REDIS_REPLY_ARRAY:
            break;
        case REDIS_REPLY_NIL:
            break;
        default:
            break;
        }
        freeReplyObject(reply);
        return bRet;

    } catch(std::exception& ex) {
        LOG_ERROR("deal with redis error:%s",ex.what());
        printf("%s", ex.what());
        if(reply)
        {
           freeReplyObject(reply);
        }
        return false;
    }
    return true;
}

template<typename T>
bool JsonStandardCache::delUniqObjSingle(const string& key,const T& uniqBasic)
{
    redisReply *reply = NULL;
    try {
        string strVal = boost::lexical_cast<string>(uniqBasic);

        reply = (redisReply *)redisCommand(conn, "SREM  %s %s",key.c_str(),strVal.c_str());

        bool bRet = false;
        switch(reply->type )
        {
        case REDIS_REPLY_INTEGER:
            //ret = reply->integer;
            bRet = true;
            break;
        case REDIS_REPLY_STRING:
            break;
        case REDIS_REPLY_ARRAY:
            break;
        case REDIS_REPLY_NIL:
            break;
        default:
            break;
        }
        freeReplyObject(reply);
        return bRet;

    } catch(std::exception& ex) {
        LOG_ERROR("deal with redis error:%s",ex.what());
        printf("%s", ex.what());
        if(reply)
        {
           freeReplyObject(reply);
        }
        return false;
    }
    return true;
}

template<typename T>
bool JsonStandardCache::loadObj(const string& key,T& obj)
{
    redisReply *reply = NULL;
    try {
        ostringstream strFormator;
        strFormator << "GET " << key;
        reply = (redisReply *)redisCommand(conn, strFormator.str().c_str());

        bool ret = false;
        switch(reply->type )
        {
        case REDIS_REPLY_INTEGER:
            break;
        case REDIS_REPLY_STRING:
            {
                if(!JsonSerial::deSerialObj(obj,reply->str))
                {
                    ostringstream osError;
                    osError <<  "ERROR 解析表:" <<  key + ",错误!";
                    throw NorException(osError.str().c_str());
                }
                 ret = true;
            }
            break;
        case REDIS_REPLY_ARRAY:
            break;
        case REDIS_REPLY_NIL:
            break;
        default:
            break;
        }
        freeReplyObject(reply);
        return ret;

    } catch(std::exception& ex) {
        LOG_ERROR("deal with redis error:%s",ex.what());
        printf("%s", ex.what());
        if(reply)
        {
           freeReplyObject(reply);
        }
        return false;
    }
    return true;
}
template<typename T>
 bool JsonStandardCache::saveObj(const string& key,T& obj)
 {
     redisReply *reply = NULL;
     try {

         string strObj;
         JsonSerial::serialObj(strObj,obj);
         reply = (redisReply *)redisCommand(conn, "SET %s %s",
                                            key.c_str(),strObj.c_str());

         bool ret = false;
         switch(reply->type )
         {
         case REDIS_REPLY_STATUS:
             if(0 == strcmp("OK",reply->str))
             {
                 ret = true;
             }
             break;
         case REDIS_REPLY_INTEGER:
             break;
         case REDIS_REPLY_STRING:
             break;
         case REDIS_REPLY_ARRAY:
             break;
         case REDIS_REPLY_NIL:
             break;
         default:
             break;
         }
         freeReplyObject(reply);
         return ret;

     } catch(std::exception& ex) {
         LOG_ERROR("deal with redis error:%s",ex.what());
         printf("%s", ex.what());
         if(reply)
         {
            freeReplyObject(reply);
         }
         return false;
     }
     return true;
 }
 template<typename T>
 bool JsonStandardCache::publish(const string& channel,const T& msg)
 {
     redisReply *reply = NULL;
     try {

         string strMsg = boost::lexical_cast<string>(msg);
         reply = (redisReply *)redisCommand(conn, "PUBLISH %s %s",
                                            channel.c_str(),strMsg.c_str());

         bool ret = false;
         switch(reply->type )
         {
         case REDIS_REPLY_STATUS:
             break;
         case REDIS_REPLY_INTEGER:
             if(reply->integer > 0)
             {
                 ret = true;
             }
             break;
         case REDIS_REPLY_STRING:
             break;
         case REDIS_REPLY_ARRAY:
             break;
         case REDIS_REPLY_NIL:
             break;
         default:
             break;
         }
         freeReplyObject(reply);
         return ret;

     } catch(std::exception& ex) {
         LOG_ERROR("deal with redis error:%s",ex.what());
         printf("%s", ex.what());
         if(reply)
         {
            freeReplyObject(reply);
         }
         return false;
     }
     return true;
 }

}

#endif // JSONSTANDARDCACHE_H
