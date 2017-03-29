#include "cachebase.h"

#include "comm/norexception.h"

#include "log4cpp/log4cppwraper.h"

namespace DataCache
{
CacheBase::CacheBase(const std::string& server,const int port,const string& pass):
    ServerURL(server),
    ServerPort(port),
    _pass(pass)
{
    conn = redisConnect(ServerURL.c_str(), ServerPort);
    if(!_pass.empty())
    {
        redisReply *reply = NULL;
        try {
            reply = (redisReply *)redisCommand(conn, "auth %s",_pass.c_str());
            switch(reply->type )
            {
            case REDIS_REPLY_STATUS:
                if(0 != strcmp("OK",reply->str))
                {
                    throw NorException(reply->str);
                }
                break;
            default:
                throw NorException(reply->str);
                break;
            }
            freeReplyObject(reply);
            reply = NULL;

        } catch(std::exception& ex) {
            LOG_ERROR("deal with redis error:%s",ex.what());
            printf("%s", ex.what());
            if(reply)
            {
               freeReplyObject(reply);
            }
            throw ex;
        }
    }
    else
    {
        //测试连接
        redisReply *reply = NULL;
        try {
            reply = (redisReply *)redisCommand(conn, "PING");

            switch(reply->type )
            {
            case REDIS_REPLY_STATUS:
                if(0 != strcmp("PONG",reply->str))
                {
                    throw NorException(reply->str);
                }
                break;
            default:
                throw NorException(reply->str);
                break;
            }
            freeReplyObject(reply);
            reply = NULL;
        }
        catch(std::exception& ex)
        {
            LOG_ERROR("deal with redis error:%s",ex.what());
            printf("%s", ex.what());
            if(reply)
            {
               freeReplyObject(reply);
            }
            throw ex;
        }
    }
}
CacheBase::~CacheBase()
{
    if(conn)
    {
        redisFree(conn);
    }
}
bool CacheBase::connServer(const std::string& server,const int port)
{
    if(conn)
    {
        redisFree(conn);
    }
    ServerURL = server;
    ServerPort = port;
    conn =  redisConnect(server.c_str(), port);
    if(conn)
    {
        return true;
    }
    return false;
}

bool CacheBase::getAllKeys(std::vector<std::string>& keys)
{
    redisReply *reply = NULL;
    try {
        keys.clear();
        reply = (redisReply *)redisCommand(conn, "KEYS *");

        bool ret = true;
        switch(reply->type )
        {
        case REDIS_REPLY_ARRAY:
            for (size_t i = 0; i < reply->elements; i++)
            {
                keys.push_back(reply->element[i]->str);
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

bool CacheBase::clearAll()
{
    redisReply *reply = NULL;
    try {
        reply = (redisReply *)redisCommand(conn, "flushdb");

        bool ret = false;
        switch(reply->type )
        {
        case REDIS_REPLY_STATUS:
            if(0 == strcmp("OK",reply->str))
            {
                ret = true;
            }
            break;
        case REDIS_REPLY_ARRAY:
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
    return false;
}
bool CacheBase::rename(const std::string & beforname, const std::string & aftername)
{
    redisReply *reply = NULL;
    try {
        reply = (redisReply *)redisCommand(conn, "RENAME %s %s",beforname.c_str(),aftername.c_str());

        bool ret = false;
        switch(reply->type )
        {
        case REDIS_REPLY_STATUS:
            if(0 == strcmp("OK",reply->str))
            {
                ret = true;
            }
            break;
        case REDIS_REPLY_ARRAY:
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
    return false;
}
bool CacheBase::delKeys(const vector<string>& vecKey)
{
    redisReply *reply = NULL;
    try {
        const int maxDelCount = 5;
        StringBunch paraBunch(maxDelCount);
        ostringstream osFormator;
        osFormator << "del";
        int num = 0;
        for(vector<string>::const_iterator iterKey = vecKey.begin();
            iterKey != vecKey.end();iterKey++)
        {
            paraBunch.setValue(num,*iterKey);
            osFormator << " %s";
            num ++;
            if(num >= maxDelCount)
            {
                reply = (redisReply *)redisCommandBunch(conn, osFormator.str().c_str(),paraBunch.getBunchString());
                bool ret = false;
                switch(reply->type )
                {
                case REDIS_REPLY_INTEGER:
                    ret = true;
                    break;
                case REDIS_REPLY_STATUS:
                case REDIS_REPLY_ARRAY:
                    break;
                case REDIS_REPLY_NIL:
                    break;
                default:
                    ret = false;
                    break;
                }
                freeReplyObject(reply);
                reply = NULL;
                if(!ret)
                {
                    LOG_ERROR("delete table error");
                    if(reply)
                    {
                       freeReplyObject(reply);
                    }
                    return false;
                }
                osFormator.str("");
                osFormator << "del";
                num = 0;
            }
        }
        if(num > 0)
        {
            reply = (redisReply *)redisCommandBunch(conn, osFormator.str().c_str(),paraBunch.getBunchString());
            bool ret = false;
            switch(reply->type)
            {
            case REDIS_REPLY_INTEGER:
                ret = true;
                break;
            case REDIS_REPLY_STATUS:
            case REDIS_REPLY_ARRAY:
                break;
            case REDIS_REPLY_NIL:
                break;
            default:
                ret = false;
                break;
            }
            freeReplyObject(reply);
            reply = NULL;
            if(!ret)
            {
                LOG_ERROR("delete table error");
                if(reply)
                {
                   freeReplyObject(reply);
                }
                return false;
            }
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

bool CacheBase::delKey(const string& strKey)
{
    redisReply *reply = NULL;
    try {

        reply = (redisReply *)redisCommand(conn, "del %s",strKey.c_str());
        bool ret = false;
        switch(reply->type )
        {
        case REDIS_REPLY_INTEGER:
            ret = true;
            break;
        case REDIS_REPLY_STATUS:
        case REDIS_REPLY_ARRAY:
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
    return false;
}
 CacheBase::RedisType CacheBase::getKeyType(const std::string& key)
 {
     redisReply *reply = NULL;
     try {
         reply = (redisReply *)redisCommand(conn, "TYPE %s ",key.c_str());

         CacheBase::RedisType ret = REDIS_NONE;
         string strType;
         switch(reply->type )
         {
         case REDIS_REPLY_STATUS:
             strType = reply->str;
             if("string" == strType)
             {
                 ret =  REDIS_STRING;
             }
             else if("list" == strType)
             {
                 ret = REDIS_LIST;
             }
             else if("set" == strType)
             {
                 ret = REDIS_SET;
             }
             else if("zset" == strType)
             {
                 ret = REDIS_ZSET;
             }
             else if("hash" == strType)
             {
                 ret = REDIS_HASH;
             }
             else if("none" == strType)// (key不存在)
             {
                 ret = REDIS_NONE;
             }
             else
             {
                 ret = REDIS_NONE;
             }
             break;
         case REDIS_REPLY_ARRAY:
             ret = REDIS_NONE;
             break;
         case REDIS_REPLY_NIL:
             ret = REDIS_NONE;
             break;
         default:
             ret = REDIS_NONE;
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
         return REDIS_NONE;
     }
     return  REDIS_NONE;
 }

 bool CacheBase::delTable(const string& strTable)
 {
     redisReply *reply = NULL;
     try {
         vector<string> vecKey;
         vecKey.push_back(strTable);
         getKeyFilter(strTable + ".*",vecKey,false);
         getKeyFilter(strTable + ":*",vecKey,false);
         for(uint i = 0; i < vecKey.size(); i++)
         {
             string& strKey = vecKey[i];
             reply = (redisReply *)redisCommand(conn, "del %s",strKey.c_str());
             if(REDIS_REPLY_INTEGER != reply->type )
             {
                 string strError = "delTable:" + strTable + " 删除key:" +  strKey + " 錯誤!";
                 throw NorException(strError);
             }
             freeReplyObject(reply);
             reply = NULL;
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
     return  true;
 }

 bool CacheBase::getKeyFilter(const std::string& strFilter, std::vector<std::string>& vecKey,bool bClear)
 {
     redisReply *reply = NULL;
     try {
         if(bClear)
         {
             vecKey.clear();
         }
         reply = (redisReply *)redisCommand(conn, "keys %s ",strFilter.c_str());

         bool ret = false;
         switch(reply->type )
         {
         case REDIS_REPLY_STRING:
             break;
         case REDIS_REPLY_ARRAY:
             for (size_t i = 0; i < reply->elements; i++)
             {
                 vecKey.push_back(reply->element[i]->str);
             }
             ret = true;
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
         return false;;
     }
     return  false;
 }
 bool CacheBase::removeList(const string& strKey,const int index)
 {
     redisReply *reply = NULL;
     try {

         reply = (redisReply *)redisCommand(conn, "LSET %s  %d \"\"",strKey.c_str(),index);

         bool ret = false;
         switch(reply->type )
         {
         case REDIS_REPLY_STATUS:
             if(0 == strcmp("OK",reply->str))
             {
                 ret = true;
             }
         }
         freeReplyObject(reply);
         if(ret)
         {
             reply = (redisReply *)redisCommand(conn, "LREM %s %d \"\"",strKey.c_str(),index);


             bool ret = false;
             switch(reply->type )
             {
             case REDIS_REPLY_INTEGER:
                 ret = true;
                 break;
             case REDIS_REPLY_STATUS:
                 break;
             case REDIS_REPLY_ARRAY:
                 break;
             case REDIS_REPLY_NIL:
                 break;
             default:
                 ret = false;
                 break;
             }
             freeReplyObject(reply);
             return ret;
         }
         else
         {
             return false;
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
//錯誤返回負一
 int CacheBase::getListLen(const string& strKey)
 {
     redisReply *reply = NULL;
     try {
         int len = -1;
         reply = (redisReply *)redisCommand(conn, "LLEN  %s",strKey.c_str());
         switch(reply->type )
         {
         case REDIS_REPLY_INTEGER:
             len =   reply->integer;
             break;
         default:
             break;
         }
         freeReplyObject(reply);
         return len;
     } catch(std::exception& ex) {
         LOG_ERROR("deal with redis error:%s",ex.what());
         printf("%s", ex.what());
         if(reply)
         {
            freeReplyObject(reply);
         }
         return -1;
     }
     return -1;
 }

 bool CacheBase::removeMapField(const string& strKey,const string field)
 {
     redisReply *reply = NULL;
     try {
         reply = (redisReply *)redisCommand(conn, "HDEL  %s %s",strKey.c_str(),field.c_str());

         bool ret = false;
         switch(reply->type )
         {
         case REDIS_REPLY_INTEGER:
             ret = true;
             break;
         case REDIS_REPLY_ARRAY:
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
bool CacheBase::removeUniq(const string& strKey,const string& field)
{
    redisReply *reply = NULL;
    try {
        reply = (redisReply *)redisCommand(conn, "SREM  %s %s",strKey.c_str(),field.c_str());

        bool ret = true;
        switch(reply->type )
        {
        case REDIS_REPLY_INTEGER:
            ret = true;
            break;
        case REDIS_REPLY_STATUS:
            if(0 == strcmp("OK",reply->str))
            {
                ret = true;
            }
            break;
        case REDIS_REPLY_ARRAY:
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
 bool CacheBase::getMapField(const string& key,vector<string>& vecField)
 {
     redisReply *reply = NULL;
     try {
         vecField.clear();
         reply = (redisReply *)redisCommand(conn, "HKEYS %s",key.c_str());

         bool ret = false;
         switch(reply->type )
         {
         case REDIS_REPLY_ARRAY:
             for (size_t i = 0; i < reply->elements; i++)
             {
                 vecField.push_back(reply->element[i]->str);
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
}
