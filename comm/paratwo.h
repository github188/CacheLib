#ifndef PARATWO_H
#define PARATWO_H

#include <boost/lexical_cast.hpp>
#include <string>
using namespace std;

#include "serial/jsonobj.h"

template <typename K,char DELIM='.'>
class ParaTwo : public JsonObj
{
public:
    ParaTwo()
    {
    }
    ParaTwo(const K& first,const K& second)
    {
        _first = first;
        _second = second;
    }
    ParaTwo(const ParaTwo& para) : _first(para._first),_second(para._second)
    {
    }
    ParaTwo(const string& strKey)
    {
        string::size_type pos = strKey.find(DELIM);
        if(string::npos != pos)
        {
            _first = boost::lexical_cast<K>(strKey.substr(0,pos));
            _second = boost::lexical_cast<K>(strKey.substr(pos + 1));
        }
        else
        {
            _first = boost::lexical_cast<K>(strKey);
            _second = boost::lexical_cast<K>("");
        }
    }

    template<typename Archive>
    void jsonArchive(Archive& arc)
    {
        arc.json_val(_first);
        arc.json_val(_second);
    }

    string toString() const
    {
        ostringstream osKey;
        osKey << _first << DELIM << _second;
        return osKey.str();
    }
    const K& getFirst() const
    {
       return _first;
    }
    K& getFirst()
    {
       return _first;
    }
    void setFirst(const K& first)
    {
        _first = first;
    }

    const K& getSecond() const
    {
       return _second;
    }
    K& getSecond()
    {
       return _second;
    }
    void setSecond(const K& sec)
    {
        _second = sec;
    }
    void setTwo(const K& first,const K& sec)
    {
        _first = first;
        _second = sec;
    }

    bool operator==(const ParaTwo& para)
    {
        return _first == para._first && _second == para._second;
    }
    bool operator <(const ParaTwo& para) const
    {
        if(_first > para._first)
        {
            return false;
        }
        else if(_first  == para._first)
        {
            if(_second < para._second)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return true;
        }
    }

    operator string() const
    {
        ostringstream osKey;
        osKey << _first << DELIM  << _second;
        return  osKey.str();
    }

    ParaTwo& operator=(const string& strKey)
    {
        string::size_type pos = strKey.find(DELIM);
        if(string::npos != pos)
        {
            _first = boost::lexical_cast<K>(strKey.substr(0,pos));
            _second = boost::lexical_cast<K>(strKey.substr(pos + 1));
        }
        else
        {
            _first = boost::lexical_cast<K>(strKey);
            _second = boost::lexical_cast<K>("");
        }
        return *this;
    }
    void clear()
    {
        _first = boost::lexical_cast<K>("");
        _second = boost::lexical_cast<K>("");
    }
    string join(char chSplit)
    {
        return boost::lexical_cast<string>(_first) + chSplit + boost::lexical_cast<string>(_second);
    }

public:
    K _first;
    K _second;
};


#endif // PARATWO_H
