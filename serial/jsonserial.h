#ifndef JSONSERIAL_H
#define JSONSERIAL_H

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "serial/jsonoarchive.h"
#include "serial/jsoniarchive.h"
#include "comm/norexception.h"
#include <boost/type_traits/is_same.hpp>
#include <stdio.h>
#include "comm/vector2d.h"
using namespace std;


namespace JsonSerial
{
    template<typename T>
    bool serialObj(string& strJson,const T& obj)
    {
        rapidjson::Value archive(rapidjson::kArrayType);
        rapidjson::Document doc;
        JsonContext jsonContext(doc,archive);
        //JsonOArchive jsonOArc(jsonContext);
        typename JsonConvertor<T>::JsonConvertFunc jsonConvertFunc;
        jsonConvertFunc.convertToJson(obj,jsonContext);
        /*rapidjson::StringBuffer buffer;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
        archive.Accept(writer);
        strJson = buffer.GetString();*/
        strJson = jsonContext.getStringWrap();
        return true;
    }

    template<typename T>
    bool deSerialObj(T& obj,const string& strJson)
    {
        if(strJson.empty())
        {
            return true;
        }
        bool isJson = false;
        switch(strJson[0])
        {
        case '{':
        case '[':
        case '"':
            isJson = true;
            break;
        default:
            if(!boost::is_same<string,T>::value)
            {
                if(boost::is_same<complex<double>,T>::value)
                {
                    isJson = false;
                }
                else
                {
                    isJson = true;
                }
            }
            break;
        }

        rapidjson::Document doc;
        typename JsonConvertor<T>::JsonConvertFunc jsonConvertFunc;
        if(isJson)
        {
            doc.Parse<0>(strJson.c_str());
            if (doc.HasParseError())  //½âÎö´íÎó
            {
                string strError = string("class:") + typeid(obj).name()  + " Parse error!";
                throw NorException(strError);
            }
            /*if(doc.IsObject())
            {
                string strError = "class:" + string(typeid(obj).name()) + string(" root is not array!");
                throw CacheException(strError);
            }*/
            JsonContext jsonContext(doc,doc);
            //JsonIArchive jsonIArc(jsonContext);
            jsonConvertFunc.jsonToObj(obj,jsonContext);
        }
        else
        {
            rapidjson::Value val;
            val.SetString(strJson.c_str(),doc.GetAllocator());
            JsonContext jsonContext(doc,val);
            //JsonIArchive jsonIArc(jsonContext);
            jsonConvertFunc.jsonToObj(obj,jsonContext);
        }
        return true;
    }

    template <typename T, template<typename T,class Alloc = allocator<T> > class C>
    bool serialObjList(string& strJson,const C<T >& vecObj)
    {
        typename JsonConvertor<T>::JsonConvertFunc convertFunc;
        rapidjson::Document doc;
        rapidjson::Value conVal(rapidjson::kArrayType);
        typename C<T >::const_iterator iterObj = vecObj.begin();
        for(;iterObj != vecObj.end(); iterObj++)
        {
            const T& tmpObj = *iterObj;
            rapidjson::Value objVal(rapidjson::kArrayType);
            JsonContext jsonContext(doc,objVal);
            convertFunc.convertToJson(tmpObj,jsonContext);
            conVal.PushBack(objVal,doc.GetAllocator());
        }
        rapidjson::StringBuffer buffer;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
        conVal.Accept(writer);
        strJson = buffer.GetString();
        return true;
    }

    template <typename T, template<typename T,class Alloc = allocator<T> > class C>
    bool serialObjListToFile(const string& strPath,const C<T >& vecObj)
    {
        typename JsonConvertor<T>::JsonConvertFunc convertFunc;
        rapidjson::Document doc;
        rapidjson::Value conVal(rapidjson::kArrayType);
        typename C<T >::const_iterator iterObj = vecObj.begin();
        for(;iterObj != vecObj.end(); iterObj++)
        {
            const T& tmpObj = *iterObj;
            rapidjson::Value objVal(rapidjson::kArrayType);
            JsonContext jsonContext(doc,objVal);
            convertFunc.convertToJson(tmpObj,jsonContext);
            conVal.PushBack(objVal,doc.GetAllocator());
        }
        FILE* file = fopen(strPath.c_str(),"w");
        if(file)
        {
            char cBuffer[1024];
            rapidjson::FileWriteStream fileStream(file,cBuffer,1024);
            rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(fileStream);
            conVal.Accept(writer);
            fclose(file);
            return true;
        }
        else
        {
            return false;
        }
    }

    template <typename T, template<typename T,class Alloc = allocator<T> > class C>
    bool deSerialObjListFromFile(C<T >& vecObj,const string& strPath)
    {
        if(strPath.empty())
        {
            return false;
        }
        std::FILE* readFile = fopen(strPath.c_str(),"r");
        if(!readFile)
        {
            return false;
        }
        char Buffer[1024];
        rapidjson::FileReadStream readStream(readFile,Buffer,1024);
        rapidjson::Document doc;
        doc.ParseStream<0>(readStream);
        if (doc.HasParseError())  //½âÎö´íÎó
        {
            fclose(readFile);
            string strError = string("vector|list<class>:") + typeid(T).name()  + " Parse error!";
            throw NorException(strError);
        }
        fclose(readFile);

        if(!doc.IsArray())
        {
            string strError = "vector|list<class>:" + string(typeid(T).name()) + string(" root is not array!");
            throw NorException(strError);
        }
        typename JsonConvertor<T>::JsonConvertFunc convertFunc;
        for(int i = 0; i < doc.Size(); i++)
        {
            JsonContext jsonContex(doc,doc[i]);
            T obj;
            convertFunc.jsonToObj(obj,jsonContex);
            vecObj.push_back(obj);
        }
        return true;
    }
    template <typename T, template<typename T,class Alloc = allocator<T> > class C>
    bool deSerialObjList(C<T >& vecObj,const string& strJson)
    {
        if(strJson.empty())
        {
            return true;
        }
        rapidjson::Document doc;
        doc.Parse<0>(strJson.c_str());
        if (doc.HasParseError())  //½âÎö´íÎó
        {
            string strError = string("vector|list<class>:") + typeid(T).name()  + " Parse error!";
            throw NorException(strError);
        }
        if(!doc.IsArray())
        {
            string strError = "vector|list<class>:" + string(typeid(T).name()) + string(" root is not array!");
            throw NorException(strError);
        }
        typename JsonConvertor<T>::JsonConvertFunc convertFunc;
        for(uint i = 0; i < doc.Size(); i++)
        {
            JsonContext jsonContex(doc,doc[i]);
            T obj;
            convertFunc.jsonToObj(obj,jsonContex);
            vecObj.push_back(obj);
        }
        return true;
    }

    template <typename T>
    bool serialVector2d(string& strJson)
    {
        vector2d<T>& _vector2dCache;
        rapidjson::Document doc;
        rapidjson::Value valJson(rapidjson::kArrayType);
        string strSize = boost::lexical_cast<string>(_vector2dCache.getRow()) + "," +
                boost::lexical_cast<string>(_vector2dCache.getCol());
        rapidjson::Value valSize;
        valSize.SetString(strSize.c_str(),static_cast<size_t>(strSize.size()), doc.GetAllocator());
        valJson.PushBack(valSize,doc.GetAllocator());
        for(int x = 0; x < _vector2dCache.getRow(); x++)
        {
            for(int y = 0; y < _vector2dCache.getCol(); y++)
            {
                rapidjson::Value valItem(rapidjson::kArrayType);
                JsonContext jsonContextItem(doc,valItem);
                typename JsonConvertor<T>::JsonConvertFunc jsonConvertFunc;
                jsonConvertFunc.convertToJson(_vector2dCache(x,y),jsonContextItem);
                valJson.PushBack(valItem,doc.GetAllocator());
            }
        }
        JsonContext jsonContextData(doc,valJson);
        strJson =  jsonContextData.getString();
        return true;
    }
    template <typename T>
    bool deSerialVector2d(const string& strJson,vector2d<T>& _vector2dCache)
    {
        rapidjson::Document doc;
        doc.Parse<0>(strJson.c_str());
        if (doc.HasParseError())  //½âÎö´íÎó
        {
            string strError = string("obj table:") + "  Parse error!";
            throw NorException(strError);
        }
        if(!doc.IsArray())
        {
            string strError = string("vector2d table:")  + "  root must array!";
            throw NorException(strError);
        }

        typename JsonConvertor<T>::JsonConvertFunc convertFunc;
        JsonContext jsonContexSize(doc,doc[0]);
        string strSize = jsonContexSize.getString();
        string::size_type szPos = strSize.find(",");
        int x = atoi(strSize.c_str());
        int y = atoi(strSize.substr(szPos+1).c_str());
        _vector2dCache.resize(x,y);
        assert(x * y + 1 == doc.Size());
        if(x * y + 1 != doc.Size())
        {
            string strError = string("vector2d table:")  +
                    "  read json size must match!";
            throw NorException(strError);
        }

        for(int i = 1,x = 0,y = 0; i < doc.Size(); i++)
        {
            JsonContext jsonContex(doc,doc[i]);
            T obj;
            convertFunc.jsonToObj(obj,jsonContex);
            _vector2dCache(x,y++) = obj;
            if(y >= _vector2dCache.getCol())
            {
                x ++;
                y = 0;
            }
        }
         return true;
    }

    template<typename KEY,typename T>
    bool serialObjMap(string& strJson,const map<KEY,T>& objMap)
    {
        typename JsonConvertor<T>::JsonConvertFunc convertFunc;
        rapidjson::Document doc;
        rapidjson::Value conVal(rapidjson::kObjectType);
        typename map<KEY,T>::const_iterator iterObj = objMap.begin();
        for(;iterObj != objMap.end(); iterObj++)
        {
            const KEY& key = iterObj->first;

            const T& tmpObj = iterObj->second;
            rapidjson::Value objVal(rapidjson::kArrayType);
            JsonContext jsonContext(doc,objVal);
            convertFunc.convertToJson(tmpObj,jsonContext);
            string strKey = boost::lexical_cast<string>(key);
            rapidjson::Value nameKey;
            JsonContext contextKey(doc,nameKey);
            contextKey.setVal(strKey);
            conVal.AddMember(nameKey,objVal,doc.GetAllocator());
        }

        rapidjson::StringBuffer buffer;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
        conVal.Accept(writer);
        strJson = buffer.GetString();
        return true;
    }
    template<typename KEY,typename T>
    bool serialObjMapToFile(const string& strPath,const map<KEY,T>& objMap)
    {
        if(strPath.empty())
        {
            return false;
        }
        typename JsonConvertor<T>::JsonConvertFunc convertFunc;
        rapidjson::Document doc;
        rapidjson::Value conVal(rapidjson::kObjectType);
        typename map<KEY,T>::const_iterator iterObj = objMap.begin();
        for(;iterObj != objMap.end(); iterObj++)
        {
            const KEY& key = iterObj->first;

            const T& tmpObj = iterObj->second;
            rapidjson::Value objVal(rapidjson::kArrayType);
            JsonContext jsonContext(doc,objVal);
            convertFunc.convertToJson(tmpObj,jsonContext);
            string strKey = boost::lexical_cast<string>(key);
            rapidjson::Value nameKey;
            JsonContext contextKey(doc,nameKey);
            contextKey.setVal(strKey);
            conVal.AddMember(nameKey,objVal,doc.GetAllocator());
        }


        std::FILE* writeFile = fopen(strPath.c_str(),"w");
        if(!writeFile)
        {
            return false;
        }
        char Buffer[1024];
        rapidjson::FileWriteStream stream(writeFile,Buffer,1024);
        rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(stream);
        conVal.Accept(writer);
        fclose(writeFile);
        //strJson = buffer.GetString();
        return true;
    }
    template<typename KEY,typename T>
    bool deSerialObjMap(map<KEY,T>& objMap,const string& strJson)
    {
        if(strJson.empty())
        {
            return true;
        }
        rapidjson::Document doc;
        doc.Parse<0>(strJson.c_str());
        if (doc.HasParseError())  //½âÎö´íÎó
        {
            string strError = string("map<class>:") + typeid(T).name()  + " Parse error!";
            throw NorException(strError);
        }
        if(!doc.IsObject())
        {
            string strError = "map<class>:" + string(typeid(T).name()) + string(" root is not obj!");
            throw NorException(strError);
        }

        typename JsonConvertor<T>::JsonConvertFunc convertFunc;
        rapidjson::Value::MemberIterator ite = doc.MemberBegin();
        for(; ite != doc.MemberEnd(); ++ite)
        {
            KEY myKey;
            string strKey;
            JsonContext jsonKeyContex(doc,ite->name);
            jsonKeyContex.toVal(strKey);
            myKey = boost::lexical_cast<KEY>(strKey);
            rapidjson::Value& itemVal  = ite->value;
            T obj;
            JsonContext jsonContex(doc,itemVal);
            convertFunc.jsonToObj(obj,jsonContex);
            objMap.insert(pair<KEY,T>(myKey,obj));
        }
        return true;
    }

    template<typename KEY,typename T>
    bool deSerialObjMapFromFile(map<KEY,T>& objMap,const string& strPath)
    {
        if(strPath.empty())
        {
            return false;
        }
        std::FILE* readFile = fopen(strPath.c_str(),"r");
        if(!readFile)
        {
            return false;
        }
        char Buffer[1024];
        rapidjson::FileReadStream readStream(readFile,Buffer,1024);
        rapidjson::Document doc;
        doc.ParseStream<0>(readStream);
        if (doc.HasParseError())  //½âÎö´íÎó
        {
            fclose(readFile);
            string strError = string("map<class>:") + typeid(T).name()  + " Parse error!";
            throw NorException(strError);
        }
        if(!doc.IsObject())
        {
            fclose(readFile);
            string strError = "map<class>:" + string(typeid(T).name()) + string(" root is not obj!");
            throw NorException(strError);
        }

        typename JsonConvertor<T>::JsonConvertFunc convertFunc;
        rapidjson::Value::MemberIterator ite = doc.MemberBegin();
        for(; ite != doc.MemberEnd(); ++ite)
        {
            KEY myKey;
            string strKey;
            JsonContext jsonKeyContex(doc,ite->name);
            jsonKeyContex.toVal(strKey);
            myKey = boost::lexical_cast<KEY>(strKey);
            rapidjson::Value& itemVal  = ite->value;
            T obj;
            JsonContext jsonContex(doc,itemVal);
            convertFunc.jsonToObj(obj,jsonContex);
            objMap.insert(pair<KEY,T>(myKey,obj));
        }
        fclose(readFile);
        return true;
    }
};

#endif // JSONSERIAL_H
