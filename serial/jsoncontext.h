#ifndef JSONCONTEXT_H
#define JSONCONTEXT_H

#include <string>
using namespace std;
#include <boost/lexical_cast.hpp>
#include "comm/norexception.h"
#include "comm/define.h"
#include "comm/datetime.h"

#include "rapidjson/document.h"
#include "rapidjson/filestream.h"   // wrapper of C stream for prettywriter as output
#include "rapidjson/prettywriter.h" // for stringify JSON
#include <complex>
using namespace std;


class JsonContext
{
public:
    JsonContext(rapidjson::Document& doc,rapidjson::Value& val) :
        _doc(doc),
        _val(val),
        _index(0)
    {
    }
    JsonContext(JsonContext& jsonContext) :
        _doc(jsonContext._doc),
        _val(jsonContext._val),
        _index(0)
    {
    }

    //转化成json
    void setVal(const double& val)
    {
        _val.SetDouble(val);
    }
    void setVal(const int& val)
    {
        _val.SetInt(val);
    }
    void setVal(const short& val)
    {
        _val.SetInt(val);
    }
    void setVal(const string& val)
    {
        if(!val.empty())
        {
            switch (val[0])
            {
            case '{':
            case '[':
                _doc.Parse<0>(val.c_str());
                if (_doc.HasParseError())  //解析错误
                {
                    _val.SetString(val.c_str(),static_cast<size_t>(val.size()), _doc.GetAllocator());
                }
                else
                {
                    _val.CopyFrom(_doc.Move(),_doc.GetAllocator());
                }
                break;
            default:
                _val.SetString(val.c_str(),static_cast<size_t>(val.size()), _doc.GetAllocator());
                break;
            }
        }
        else
        {
            _val.SetString(val.c_str(),static_cast<size_t>(val.size()), _doc.GetAllocator());
        }
    }
    void setVal(const char* val)
    {
        if(strlen(val)  > 0)
        {
            switch (val[0])
            {
            case '{':
            case '[':
                _doc.Parse<0>(val);
                if (_doc.HasParseError())  //解析错误
                {
                    _val.SetString(val,static_cast<size_t>(strlen(val)), _doc.GetAllocator());
                }
                else
                {
                    _val.CopyFrom(_doc.Move(),_doc.GetAllocator());
                }
                break;
            default:
                _val.SetString(val,static_cast<size_t>(strlen(val)), _doc.GetAllocator());
                break;
            }
        }
        else
        {
            _val.SetString(val,static_cast<size_t>(strlen(val)), _doc.GetAllocator());
        }
    }
    void setVal(const ulong64& val)
    {
        _val.SetInt64(val);
    }
    void setVal(const int64_t& val)
    {
        _val.SetInt64(val);
    }
    void setVal(const unsigned char & val)
    {
        _val.SetInt(val);
    }
    void setVal(const char & val)
    {
        _val.SetInt(val);
    }
    void setVal(const bool& val)
    {
        _val.SetBool(val);
    }
    bool setVal(const complex<double>& outVal)
    {
        ostringstream osFormator;
        osFormator << outVal;
        _val.SetString(osFormator.str().c_str(),static_cast<size_t>(osFormator.str().size()), _doc.GetAllocator());
        return true;
    }
    bool setVal(const DateTime& outVal)
    {
        string strTm = outVal.getStdString();
        _val.SetString(strTm.c_str(),static_cast<size_t>(strTm.size()), _doc.GetAllocator());
        return true;
    }


    //json转化成基本类型
    bool toVal(double& outVal)
    {
        outVal = _val.GetDouble();
        return true;
    }
    bool toVal(short& outVal)
    {
        outVal = _val.GetInt();
        return true;
    }
    bool toVal(int& outVal)
    {
        outVal = _val.GetInt();
        return true;
    }
    bool toVal(string& outVal)
    {
        if(_val.IsString())
        {
            outVal = _val.GetString();
        }
        else
        {
            outVal = getStringWrap();
        }
        return true;
    }
    bool toVal(int64_t& outVal)
    {
        outVal = _val.GetInt64();
        return true;
    }
    bool toVal(ulong64& outVal)
    {
        outVal = _val.GetInt64();
        return true;
    }
    bool toVal(unsigned char& outVal)
    {
        outVal = _val.GetInt();
        return true;
    }
    bool toVal(char& outVal)
    {
        outVal = _val.GetInt();
        return true;
    }
    bool toVal(bool& outVal)
    {
        outVal = _val.GetBool();
        return true;
    }
    bool toVal(complex<double>& outVal)
    {
        string strComp = _val.GetString();

        istringstream isFormator(strComp);
        isFormator >> outVal;

        /*string::size_type pos = strComp.find('+');
        if(pos == string::npos)
        {
            outVal.real() = atof(strComp.c_str());
        }
        else
        {
            outVal.real() = atof(strComp.c_str());
            outVal.imag() = atof(strComp.substr(pos + 1).c_str());
        }*/
        return true;
    }
    bool toVal(DateTime& outVal)
    {
        string strComp = _val.GetString();
        outVal.setStdTime(strComp);
        return true;
    }

    string getStringWrap()
    {
        string strJson = "";
        if(_val.IsArray() || _val.IsObject())
        {
            rapidjson::StringBuffer buffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            _val.Accept(writer);
            strJson = buffer.GetString();
        }
        else
        {
            if(_val.IsString())
            {
                strJson = boost::lexical_cast<string>(_val.GetString());
            }
            else if(_val.IsDouble())
            {
                strJson = boost::lexical_cast<string>(_val.GetDouble());
            }
            else if(_val.IsInt64())
            {
                strJson = boost::lexical_cast<string>(_val.GetInt64());
            }
            else if(_val.IsInt())
            {
                strJson = boost::lexical_cast<string>(_val.GetInt());
            }
            else if(_val.IsBool())
            {
                strJson = boost::lexical_cast<string>(_val.GetBool());
            }
            else if(_val.IsUint())
            {
                strJson = boost::lexical_cast<string>(_val.GetUint());
            }
            else if(_val.IsUint64())
            {
                strJson = boost::lexical_cast<string>(_val.GetUint64());
            }
        }
        return strJson;
    }

    string getString()
    {
        string strJson = "";
        if(_val.IsArray() || _val.IsObject())
        {
            rapidjson::StringBuffer buffer;
            rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
            _val.Accept(writer);
            strJson = buffer.GetString();
        }
        else
        {
            if(_val.IsString())
            {
                strJson = boost::lexical_cast<string>(_val.GetString());
            }
            else if(_val.IsDouble())
            {
                strJson = boost::lexical_cast<string>(_val.GetDouble());
            }
            else if(_val.IsInt64())
            {
                strJson = boost::lexical_cast<string>(_val.GetInt64());
            }
            else if(_val.IsInt())
            {
                strJson = boost::lexical_cast<string>(_val.GetInt());
            }
            else if(_val.IsBool())
            {
                strJson = boost::lexical_cast<string>(_val.GetBool());
            }
            else if(_val.IsUint())
            {
                strJson = boost::lexical_cast<string>(_val.GetUint());
            }
            else if(_val.IsUint64())
            {
                strJson = boost::lexical_cast<string>(_val.GetUint64());
            }
        }
        return strJson;
    }

    rapidjson::Document& getDoc()
    {
        return _doc;
    }
    rapidjson::Value& getVal()
    {
        return _val;
    }
    rapidjson::Value& getValChildAndNext()
    {
        return _val[_index++];
    }

    int getIndex()
    {
        return _index;
    }
private:
    rapidjson::Document& _doc;
    rapidjson::Value& _val;
    int _index;
};

#endif // JSONCONTEXT_H
