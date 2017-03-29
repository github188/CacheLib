#ifndef REFLEXCLASS_H
#define REFLEXCLASS_H

#include <string>
#include <map>

#include <boost/lexical_cast.hpp>



using namespace std;
typedef  unsigned char byte;

class FuncBase
{
public:
    enum
    {
        TYPE_UNKNOW = -1,
        TYPE_INT,
        TYPE_SHORT,
        TYPE_BYTE,
        TYPE_FLOAT,
        TYPE_DOULE,
        TYPE_STRING
    };
    FuncBase(int type)
    {
        _type = type;
    }
    virtual ~FuncBase()
    {
    }
    const int& getType() const
    {
        return _type;
    }
    void setType(int type)
    {
        _type = type;
    }

private:
    int _type;
};

template<typename C,typename T>
class FuncConfig : public FuncBase
{
public:
    FuncConfig(int type) :
        FuncBase(type)
    {
    }

    typedef  const T&  (C::*FuncGet)() const;
    typedef void (C::*FuncSet)(const T&);

    FuncGet  getFuncGet()
    {
        return _funcGet;
    }
    void setFuncGet(FuncGet  funcGet)
    {
        _funcGet = funcGet;
    }

    FuncSet getFuncSet()
    {
        return _funcSet;
    }
    void setFuncSet(FuncSet funcSet)
    {
        _funcSet = funcSet;
    }


private:
    FuncGet  _funcGet;
    FuncSet _funcSet;
};

class Reflex
{
public:
    virtual ~Reflex()
    {
    }
    virtual void setFuncRun(const string& name,const string& val) = 0;
    virtual string getFuncRun(const string& name) = 0;
    virtual int getStringType(const string& str) = 0;
};

template<typename T>
class ReflexClass : public Reflex
{
public:
    ReflexClass()
    {
    }
    ~ReflexClass()
    {
        for(map<string,FuncBase*>::iterator iterFunc = _funcMap.begin();
            iterFunc != _funcMap.end(); ++iterFunc)
        {
            FuncBase* tmpFunc = iterFunc->second;
            delete tmpFunc;
        }
        _funcMap.clear();
    }

    void setSource(T* source)
    {
        _source = source;
    }
    /// 转化字符串（如把整形转换为字符串）
   template<typename K>
   static string toString(const K& myString)
   {
       std::stringstream streamTemp;
       streamTemp << myString;
       return streamTemp.str();
   }

    template<typename F>
    void bind(const string& nameMem,typename FuncConfig<T,F>::FuncSet funcSet,typename FuncConfig<T,F>::FuncGet funcGet,int type)
    {
       map<string,FuncBase*>::iterator iterFunc = _funcMap.find(nameMem);
       if(iterFunc == _funcMap.end())
       {
          FuncConfig<T,F>* func  = new FuncConfig<T,F>(type);
          func->setFuncGet(funcGet);
          func->setFuncSet(funcSet);
           _funcMap[nameMem] = func;
       }
    }

    void setFuncRun(const string& name,const string& val)
    {
        map<string,FuncBase*>::iterator iterFunc = _funcMap.find(name);
        if(iterFunc != _funcMap.end())
        {
            FuncBase* funcBase = iterFunc->second;
            switch(funcBase->getType())
            {
            case FuncBase::TYPE_INT:
            {
                FuncConfig<T,int>* funcReal =  dynamic_cast< FuncConfig<T,int>* >(funcBase);
                typename FuncConfig<T,int>::FuncSet funcSet = funcReal->getFuncSet();
                (_source->*funcSet)(boost::lexical_cast<int>(val));
            }
                break;
            case FuncBase::TYPE_SHORT:
            {
                FuncConfig<T,short>* funcReal =  dynamic_cast< FuncConfig<T,short>* >(funcBase);
                typename FuncConfig<T,short>::FuncSet funcSet = funcReal->getFuncSet();
                (_source->*funcSet)(boost::lexical_cast<short>(val));
            }
                break;
            case FuncBase::TYPE_BYTE:
            {
                FuncConfig<T,byte>* funcReal =  dynamic_cast< FuncConfig<T,byte>* >(funcBase);
                typename FuncConfig<T,byte>::FuncSet funcSet = funcReal->getFuncSet();
                (_source->*funcSet)(boost::lexical_cast<int>(val));
            }
                break;
            case FuncBase::TYPE_FLOAT:
            {
                FuncConfig<T,float>* funcReal =  dynamic_cast< FuncConfig<T,float>* >(funcBase);
                typename FuncConfig<T,float>::FuncSet funcSet = funcReal->getFuncSet();
                (_source->*funcSet)(boost::lexical_cast<float>(val));
            }
                break;
            case FuncBase::TYPE_DOULE:
            {
                FuncConfig<T,double>* funcReal =  dynamic_cast< FuncConfig<T,double>* >(funcBase);
                typename FuncConfig<T,double>::FuncSet funcSet = funcReal->getFuncSet();
                (_source->*funcSet)(boost::lexical_cast<double>(val));
            }
                break;
            case FuncBase::TYPE_STRING:
            {
                FuncConfig<T,string>* funcReal =  dynamic_cast< FuncConfig<T,string>* >(funcBase);
                typename FuncConfig<T,string>::FuncSet funcSet = funcReal->getFuncSet();
                (_source->*funcSet)(val);
            }
                break;
            default:
                assert(false);
                break;
            }
        }
    }
    string getFuncRun(const string& name)
    {
        map<string,FuncBase*>::iterator iterFunc = _funcMap.find(name);
        if(iterFunc != _funcMap.end())
        {
             FuncBase* funcBase = iterFunc->second;

             switch(funcBase->getType())
             {
             case FuncBase::TYPE_INT:
             {
                 FuncConfig<T,int>* funcReal =  dynamic_cast< FuncConfig<T,int>*>(funcBase);
                 typename FuncConfig<T,int>::FuncGet funcGet = funcReal->getFuncGet();
                 return toString((_source->*funcGet)());
             }
                 break;
             case FuncBase::TYPE_SHORT:
             {
                 FuncConfig<T,short>* funcReal =  dynamic_cast< FuncConfig<T,short>*>(funcBase);
                 typename FuncConfig<T,short>::FuncGet funcGet = funcReal->getFuncGet();
                 return toString((_source->*funcGet)());
             }
                 break;
             case FuncBase::TYPE_BYTE:
             {
                 FuncConfig<T,byte>* funcReal =  dynamic_cast< FuncConfig<T,byte>*>(funcBase);
                 typename FuncConfig<T,byte>::FuncGet funcGet = funcReal->getFuncGet();
                 return toString((int)(_source->*funcGet)());
             }
                 break;
             case FuncBase::TYPE_FLOAT:
             {
                 FuncConfig<T,float>* funcReal =  dynamic_cast< FuncConfig<T,float>*>(funcBase);
                 typename FuncConfig<T,float>::FuncGet funcGet = funcReal->getFuncGet();
                 return toString((_source->*funcGet)());
             }
                 break;
             case FuncBase::TYPE_DOULE:
             {
                 FuncConfig<T,double>* funcReal =  dynamic_cast< FuncConfig<T,double>*>(funcBase);
                 typename FuncConfig<T,double>::FuncGet funcGet = funcReal->getFuncGet();
                 return toString((_source->*funcGet)());
             }
                 break;
             case FuncBase::TYPE_STRING:
             {
                 FuncConfig<T,string>* funcReal =  dynamic_cast< FuncConfig<T,string>*>(funcBase);
                 typename FuncConfig<T,string>::FuncGet funcGet = funcReal->getFuncGet();
                 return (_source->*funcGet)();
             }
                 break;
             default:
                 assert(false);
                 break;
             }
        }
        throw exception();
    }

    int getStringType(const string& str)
    {
        map<string,FuncBase*>::iterator iterFunc = _funcMap.find(str);
        if(iterFunc != _funcMap.end())
        {
            FuncBase* funcBase = iterFunc->second;
            return funcBase->getType();
        }
        return FuncBase::TYPE_UNKNOW;
    }

private:
    T* _source;
    map<string,FuncBase*> _funcMap;
};

#endif // REFLEXCLASS_H
