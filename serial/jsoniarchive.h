#ifndef JSONIARCHIVE_H
#define JSONIARCHIVE_H

#include "rapidjson/document.h"
#include "typeforjsonconvertor.h"
#include <string>
#include <map>
using namespace std;

#include "typeforjsonconvertor.h"
#include "jsoncontext.h"
#include "typeforjsonconvertor.h"
#include "comm/norexception.h"

class JsonIArchive
{
public:
    JsonIArchive(JsonContext& context) :
        _contex(context)
    {
    }

    template<typename T>
    void json_val(T& val)
    {
        typename JsonConvertor<T>::JsonConvertFunc convertFunc;
        JsonContext jsonContex(_contex.getDoc(),_contex.getValChildAndNext());
        convertFunc.jsonToObj(val,jsonContex);
    }

    template <typename T, template<typename T,class Alloc = allocator<T> > class C>
    void json_list(C<T >& vecObj)
    {
        typename JsonConvertor<T>::JsonConvertFunc convertFunc;
        rapidjson::Value& conVal = _contex.getValChildAndNext();
        if(conVal.IsArray())
        {
            for(uint i = 0; i < conVal.Size(); i++)
            {
                JsonContext jsonContex(_contex.getDoc(),conVal[i]);
                T obj;
                convertFunc.jsonToObj(obj,jsonContex);
                vecObj.push_back(obj);
            }
        }
        else
        {
            string strError = "class:" + string(typeid(T).name()) + string(" json_list  is not array!");
            throw NorException(strError);
        }
    }
    template<typename KEY,typename T>
    void json_mapkey(map<KEY,T>& objMap)
    {
        typename JsonConvertor<string>::JsonConvertFunc convertFunc;
        rapidjson::Value& conVal = _contex.getValChildAndNext();

        if(conVal.IsArray())
        {
            for(uint i = 0; i < conVal.Size(); i++)
            {
                JsonContext jsonContex(_contex.getDoc(),conVal[i]);
                string strKey;
                convertFunc.jsonToObj(strKey,jsonContex);
                KEY key = boost::lexical_cast<KEY>(strKey);
                objMap[key];
            }
        }
        else
        {
            string strError = "class:" + string(typeid(T).name()) + string(" json_list  is not array!");
            throw NorException(strError);
        }
    }

    template<typename KEY,typename T>
    void json_map(map<KEY,T>& objMap)
    {
        typename JsonConvertor<T>::JsonConvertFunc convertFunc;
        rapidjson::Value& conVal = _contex.getValChildAndNext();
        rapidjson::Value::MemberIterator ite = conVal.MemberBegin();
        for(; ite != conVal.MemberEnd(); ++ite)
        {
            KEY myKey;
            string strKey;
            JsonContext jsonKeyContex(_contex.getDoc(),ite->name);
            jsonKeyContex.toVal(strKey);
            myKey = boost::lexical_cast<KEY>(strKey);
            rapidjson::Value& itemVal  = ite->value;
            T obj;
            JsonContext jsonContex(_contex.getDoc(),itemVal);
            convertFunc.jsonToObj(obj,jsonContex);
            objMap.insert(pair<KEY,T>(myKey,obj));
        }
    }

    template<typename KEY,typename T>
    void json_mapmap(map<KEY,T>& objMap)
    {
        rapidjson::Value& conVal = _contex.getValChildAndNext();
        rapidjson::Value::MemberIterator ite = conVal.MemberBegin();
        for(; ite != conVal.MemberEnd(); ++ite)
        {
            KEY myKey;

            //得到key
            string strKey;
            JsonContext jsonKeyContex(_contex.getDoc(),ite->name);
            jsonKeyContex.toVal(strKey);
            myKey = boost::lexical_cast<KEY>(strKey);
            //得到obj
            T obj;
            rapidjson::Value& itemVal  = ite->value;
            JsonContext jsonValContex(_contex.getDoc(),itemVal);
            JsonIArchive jsonIAr(jsonValContex);
            jsonIAr.json_addmap(obj);
            objMap.insert(pair<KEY,T>(myKey,obj));
        }
    }

    template<typename KEY,typename T>
    void json_addmap(map<KEY,T>& objMap)
    {
        typename JsonConvertor<T>::JsonConvertFunc convertFunc;
        rapidjson::Value& conVal = _contex.getVal();
        rapidjson::Value::MemberIterator ite = conVal.MemberBegin();
        for(; ite != conVal.MemberEnd(); ++ite)
        {
            KEY myKey;
            string strKey;
            JsonContext jsonKeyContex(_contex.getDoc(),ite->name);
            jsonKeyContex.toVal(strKey);
            myKey = boost::lexical_cast<KEY>(strKey);
            rapidjson::Value& itemVal  = ite->value;
            T obj;
            JsonContext jsonContex(_contex.getDoc(),itemVal);
            convertFunc.jsonToObj(obj,jsonContex);
            objMap.insert(pair<KEY,T>(myKey,obj));
        }
    }


private:
    JsonContext _contex;
};

#endif // JSONIARCHIVE_H
