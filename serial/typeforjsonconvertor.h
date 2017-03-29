#ifndef TYPEFORJSONCONVERTOR_H
#define TYPEFORJSONCONVERTOR_H

#include <string>
using namespace std;
#include<boost/lexical_cast.hpp>
#include <boost/type_traits.hpp>
#include <boost/type_traits/is_pointer.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/mpl/if.hpp>
#include <boost/type_traits/remove_pointer.hpp>
#include "serial/jsonobj.h"

/*#include "jsoniarchive.h"
#include "jsonoarchive.h"*/
#include "jsoncontext.h"

class JsonOArchive;
class JsonIArchive;
template<typename T>
struct JsonForBasicConvertor
{
    //ת����json
    void convertToJson(const T& val,JsonContext& outJsonVal)
    {
        outJsonVal.setVal(val);
    }

    //jsonת���ɻ�������
    bool jsonToObj(T& outVal,JsonContext& outJsonVal)
    {
        outJsonVal.toVal(outVal);
        return true;
    }
};

template<typename T>
struct JsonForObjConvertor
{

    void convertToJson(const T& obj,JsonContext& outJsonVal)
    {
        JsonOArchive jsonIArc(outJsonVal);
        const_cast<T&>(obj).jsonArchive(jsonIArc);
    }
    bool jsonToObj(T& obj,JsonContext& outJsonVal)
    {
        JsonIArchive jsonIArc(outJsonVal);
        obj.jsonArchive(jsonIArc);
        return  true;
    }
};

template<typename T>
struct JsonForObjPtrConvertor
{

    void convertToJson(const T& obj,JsonContext& outJsonVal)
    {
        JsonOArchive jsonIArc(outJsonVal);
        const_cast<T>(obj)->jsonArchive(jsonIArc);
    }
    bool jsonToObj( T& obj,JsonContext& outJsonVal)
    {
        JsonIArchive jsonIArc(outJsonVal);
        /*if(!obj)//���ѷ����ڴ�
        {*/
            try{
                obj = new typename boost::remove_pointer<T>::type();
                obj->jsonArchive(jsonIArc);
            }
            catch(exception ex)
            {
                throw ex;
            }
        /*}
        else//�ѷ�����ڴ�
        {
            obj->jsonArchive(jsonIArc);
        }*/
        return  true;
    }
};

template <typename T>
struct JsonConvertor
{
    typedef typename boost::mpl::eval_if<boost::is_pointer<T>,
    boost::mpl::identity<JsonForObjPtrConvertor<T> >,
    boost::mpl::if_<boost::is_base_of<JsonObj,T>,
    JsonForObjConvertor<T>,JsonForBasicConvertor<T> > >::type JsonConvertFunc;
};


#endif // TYPEFORJSONCONVERTOR_H
