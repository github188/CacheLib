#ifndef JSONOARCHIVE_H
#define JSONOARCHIVE_H

#include "rapidjson/document.h"
#include "rapidjson/filestream.h"   // wrapper of C stream for prettywriter as output
#include "rapidjson/prettywriter.h" // for stringify JSON
#include <string>
#include <map>
#include <boost/lexical_cast.hpp>
using namespace std;

#include "typeforjsonconvertor.h"

class JsonOArchive
{
public:
    JsonOArchive(rapidjson::Document& doc,rapidjson::Value& arc) :
        _contex(doc,arc)
    {}
    JsonOArchive(JsonContext& jsonContext) :
        _contex(jsonContext)
    {}
    template<typename T>
    void json_val(T& val)
    {
        typename JsonConvertor<T>::JsonConvertFunc convertFunc;
        rapidjson::Value jsonItem(rapidjson::kArrayType);
        JsonContext jsonContex(_contex.getDoc(),jsonItem);
        convertFunc.convertToJson(val,jsonContex);
        _contex.getVal().PushBack(jsonItem,_contex.getDoc().GetAllocator());
    }
    template <typename T, template<typename T,class Alloc = allocator<T> > class C>
    void json_list(C<T >& vecObj)
    {
        typename JsonConvertor<T>::JsonConvertFunc convertFunc;
        rapidjson::Value conVal(rapidjson::kArrayType);
        typename C<T >::iterator iterObj = vecObj.begin();
        for(;iterObj != vecObj.end(); iterObj++)
        {
            T& tmpObj = *iterObj;
            rapidjson::Value objVal(rapidjson::kArrayType);
            JsonContext jsonContext(_contex.getDoc(),objVal);
            convertFunc.convertToJson(tmpObj,jsonContext);
            conVal.PushBack(objVal,_contex.getDoc().GetAllocator());
        }
        _contex.getVal().PushBack(conVal,_contex.getDoc().GetAllocator());
    }

    template<typename KEY,typename T>
    void json_mapkey(map<KEY,T>& objMap)
    {
        rapidjson::Value conVal(rapidjson::kArrayType);
        typename map<KEY,T>::iterator iterObj = objMap.begin();
        for(;iterObj != objMap.end(); iterObj++)
        {
            const KEY& key = iterObj->first;
            string strKey = boost::lexical_cast<string>(key);
            rapidjson::Value nameKey;
            JsonContext contextKey(_contex.getDoc(),nameKey);
            contextKey.setVal(strKey);
            conVal.PushBack(nameKey,_contex.getDoc().GetAllocator());
        }
        _contex.getVal().PushBack(conVal,_contex.getDoc().GetAllocator());
    }

    template<typename KEY,typename T>
    void json_map(map<KEY,T>& objMap)
    {
        typename JsonConvertor<T>::JsonConvertFunc convertFunc;
        rapidjson::Value conVal(rapidjson::kObjectType);
        typename map<KEY,T>::iterator iterObj = objMap.begin();
        for(;iterObj != objMap.end(); iterObj++)
        {
            const KEY& key = iterObj->first;

            T& tmpObj = iterObj->second;
            rapidjson::Value objVal(rapidjson::kArrayType);
            JsonContext jsonContext(_contex.getDoc(),objVal);
            convertFunc.convertToJson(tmpObj,jsonContext);
            string strKey = boost::lexical_cast<string>(key);
            rapidjson::Value nameKey;
            JsonContext contextKey(_contex.getDoc(),nameKey);
            contextKey.setVal(strKey);
            conVal.AddMember(nameKey,objVal,_contex.getDoc().GetAllocator());
        }
        _contex.getVal().PushBack(conVal,_contex.getDoc().GetAllocator());
    }

    template<typename KEY,typename T>
    void json_mapmap(map<KEY,T>& objMap)
    {
        rapidjson::Value conVal(rapidjson::kObjectType);
        typename map<KEY,T>::iterator iterObj = objMap.begin();
        for(;iterObj != objMap.end(); iterObj++)
        {
            const KEY& key = iterObj->first;

            //Ùxvalue
            T& tmpObj = iterObj->second;
            rapidjson::Value objVal(rapidjson::kObjectType);
            JsonOArchive jsonOItemAr(_contex.getDoc(),objVal);
            jsonOItemAr.json_addmap(tmpObj);

            //Ùxkey
            string strKey = boost::lexical_cast<string>(key);
            rapidjson::Value nameKey;
            JsonContext contextKey(_contex.getDoc(),nameKey);
            contextKey.setVal(strKey);
            conVal.AddMember(nameKey,objVal,_contex.getDoc().GetAllocator());
        }
        _contex.getVal().PushBack(conVal,_contex.getDoc().GetAllocator());
    }

    template<typename KEY,typename T>
    void json_addmap(map<KEY,T>& objMap)
    {
        typename JsonConvertor<T>::JsonConvertFunc convertFunc;
        typename map<KEY,T>::iterator iterObj = objMap.begin();
        for(;iterObj != objMap.end(); iterObj++)
        {
            const KEY& key = iterObj->first;

            T& tmpObj = iterObj->second;
            rapidjson::Value objVal(rapidjson::kArrayType);
            JsonContext jsonContext(_contex.getDoc(),objVal);
            convertFunc.convertToJson(tmpObj,jsonContext);
            string strKey = boost::lexical_cast<string>(key);
            rapidjson::Value nameKey;
            JsonContext contextKey(_contex.getDoc(),nameKey);
            contextKey.setVal(strKey);
            _contex.getVal().AddMember(nameKey,objVal,_contex.getDoc().GetAllocator());
        }
    }

private:
    JsonContext _contex;
};

#endif // JSONOARCHIVE_H
