#ifndef TYPEFORJSONCONVERTOR_H
#define TYPEFORJSONCONVERTOR_H

#include <string>
using namespace std;
#include<boost/lexical_cast.hpp>
#include <boost/type_traits.hpp>
#include <boost/type_traits/is_pointer.hpp>
#include <boost/function.hpp>
#include <boost/mpl/if.hpp>
#include <boost/type_traits/remove_pointer.hpp>
#include "Json/value.h"
#include "Json/reader.h"
#include <model/jsonobj.h>

namespace TypeJson
{

template<typename T>
 struct basicToString
{
    const string operator()(const T& aa)
    {
        return  boost::lexical_cast<string>(aa);
    }
};

template<typename T>
struct objToString
{
    const string operator()(const T& aa)
    {
        return  aa.toJson();
    }
};

template<typename T>
struct pointerToString
{
    const string operator()(const T& aa)
    {
        return  aa->toJson();
    }
};

//指針需要以后释放
template<typename T>
struct pointerFromString
{
    bool operator()(const string& str,T& objPtr)
    {
        objPtr = NULL;
        try {
            if (_reader.parse(str, _root))
            {
                objPtr = new typename boost::remove_pointer<T>::type();
                return  objPtr->fromJson(_root);
            }
            else
            {
                if(objPtr)
                {
                    delete objPtr;
                }
                return false;
            }
        } catch(std::exception& ex) {
            if(objPtr)
            {
                delete objPtr;
            }
            throw ex;
            return false;
        }
    }
private:
    Json::Reader _reader;
    Json::Value _root;
};
template<typename T>
struct objFromString
{
    bool operator()(const string& str,T& obj)
    {
        try {
            if (_reader.parse(str, _root))
            {
                return  obj.fromJson(_root);
            }
            else
            {
                return false;
            }
        } catch(std::exception& ex) {
            throw ex;
            return false;
        }
    }
private:
    Json::Reader _reader;
    Json::Value _root;
};

template<typename T>
struct basicFromString
{
    bool operator()(const string& str,T& basic)
    {
        basic = boost::lexical_cast<T>(str);
        return  true;
    }
};

template <typename T>
struct JsonConvertor
{
    typedef typename boost::mpl::eval_if<boost::is_pointer<T>,
    boost::mpl::identity<pointerToString<T> >,
    boost::mpl::if_<boost::is_base_of<DataCache::JsonObj,T>,
    objToString<T>,basicToString<T> > >::type ToStringFunc;

    typedef  typename boost::mpl::eval_if<boost::is_pointer<T>,
            boost::mpl::identity<pointerFromString<T> >,
            boost::mpl::if_<boost::is_base_of<DataCache::JsonObj,T>,
            objFromString<T>,basicFromString<T> > >::type FromStringFunc;
};

template<typename T>
string toString(const T& val)
{
    return typename JsonConvertor<T>::ToStringFunc()(val);
}

template<typename T>
bool fromString(const string& strout,T& val)
{
    return typename JsonConvertor<T>::FromStringFunc()(strout,val);
}
}

#endif // TYPEFORJSONCONVERTOR_H
