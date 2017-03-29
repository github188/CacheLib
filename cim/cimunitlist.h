#ifndef CIMUNITLIST_H
#define CIMUNITLIST_H
#include "cim/cimunit.h"
#include <algorithm>
#include <assert.h>

//mrid名字对照表
struct Correspondence
{
    long mRID;
    string ID;
    string name;
};
//增加onetime中mrid字符串转换全局变量
//TG 2015-11-20
class MRIDConvert
{
 public:
    static int g_iVersionFlag;
    static map<string, long> g_Mridmap;
    static long g_iMRIDIndex;
    static long g_ConvertMrid(const char* ID);
    static map<string, Correspondence> g_mapCorrespondence;
};
//表列名字对应
struct ParaRefect
{
    enum
    {
        PARA_DATA,
        PARA_DEFAULT
    };
    ParaRefect() :
        _flag(PARA_DATA)
    {

    }

    string _header;
    int _pos;
    int _flag;
    string _default;
};
//大小写转化
class InsensitiveCmp
{
public:
    InsensitiveCmp(string other);
    bool operator()(const char* first);
    bool operator()(const string& first);
private:
    string _cmp;
};
//空串
const char  myEmpty[] = "";
class CimUnitList : public CimUnit
{
public:
    CimUnitList()
    {}
    virtual ~CimUnitList(){}
    //处理表标題
    virtual bool handleHeader(vector<char *>& vecHead);
    //处理每行数据
    virtual bool handleData(vector<char *>& vecData);
    //处理表结尾
    virtual bool handelDataEnd();
    //得到标题大小
    virtual int getHeaderSize()
    {
        return _vecHeader.size();
    }
    //得到标題数据对照表
    const vector<ParaRefect>& getHeader() const
    {
        return _vecHeader;
    }
    //初始化对照表大小
    void initHeader(int size)
    {
        _vecHeader.resize(size);
        for(int i = 0; i < size; i ++)
        {
            _vecHeader[i]._pos = i;
        }
    }
    //设置数据对应的表名,及位置
    void setHeader(int index,const string& header,int pos);
    virtual void setHeader(unsigned int index,const string& header);

    //更据标题索引得到数据
    const char* getData(const vector<char*>& vecData,int index);
    //查看这条数据是所存在
    bool isHasCol(int index);
//dcb 2015-12-18
    //空串转数字
    long nullToNegOne(const char* xNode);
    //写cim
    virtual void writeCim(ofstream& ofCim,string mark);
    virtual void writeCimContent(ofstream& ofCim,string mark);
protected:
    //标题对照
    vector<ParaRefect> _vecHeader;
};
#endif // CIMUNITLIST_H
