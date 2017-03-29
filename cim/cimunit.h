#ifndef CIMUNIT_H
#define CIMUNIT_H
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <list>

using namespace std;
const string NullChar = "NULL";
class CimUnit
{
public:
    CimUnit();
    virtual ~CimUnit();
    //处理表头
    virtual bool  handleTable(const char* area,const map<char*,char*> mapAttri);
    //处理表标題
    virtual bool handleHeader(vector<char *>& vecHead) = 0;
    //处理每行数据
    virtual bool handleData(vector<char *>& vecData) = 0;
    //处理表尾
    virtual bool handelDataEnd() = 0;
    //得到标题大小
    virtual int getHeaderSize() = 0;
    //设某行数据对应的名字
    virtual void setHeader(unsigned int index,const string& header) = 0;
    //cim写入
    virtual void writeCim(ofstream& ofCim,string mark)  = 0;
    virtual void writeCimContent(ofstream& ofCim,string mark) = 0;
    //\t\n转化
    string makeSureDim(const string& str);
    //空字符串转化
    const string&  makeSureChar(const string& str)
    {
        if(str.empty())
        {
            return NullChar;
        }
        else
        {
            return str;
        }
    }
    string& makeNullChar(string& str)
    {
        if("NULL" == str)
        {
            str = "";
        }
        return str;
    }
    //得到注释
    const string& getNote() const
    {
        return _note;
    }
    void setNote(const string& strNote)
    {
        _note = strNote;
    }
    //得到公司
    void setCompany(const string& company)
    {
        _company = company;
    }
    const string& getCompany()const
    {
        return _company;
    }
private:
    //注释
    string _note;
    //公司
    string _company;
};

#endif // CIMUNIT_H
