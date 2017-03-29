#ifndef NORMALCIMUNIT_H
#define NORMALCIMUNIT_H
#include "cim/cimunitlist.h"
#include <list>
class NormalCimUnit : public CimUnitList
{
public:
    enum
    {
        FLAG_HEADER,//按给定的表头读取
        FLAG_ALL//读取所有表头和对应数据
    };
    NormalCimUnit();
    NormalCimUnit(const vector<string>& header);

    //处理标题
    virtual bool  handleHeader(vector<char *>& vecHead);
    //处理数据
    virtual bool handleData(vector<char *>& vecData);
    //写cim
    virtual void writeCimContent(ofstream& ofCim,string  mark);
    //清数据
    void clear()
    {
        _norData.clear();
    }
    //设标題
    void setHeader(const vector<string>& header);
    //得到数据
    list<vector<string> >&  getNorData()
    {
        return _norData;
    }
    bool IsEmpty()
    {
        return _norData.empty();
    }

    //得到标题对就索引
    int getHeaderIndex(const string& headerName) const;

private:
    //数据
    list<vector<string> > _norData;
    //表头
    vector<string> _header;
    //标识
    int _flag;
    //一行临时数据
    vector<string> _tmpData;

};

#endif // NORMALCIMUNIT_H
