#include "normalcimunit.h"

NormalCimUnit::NormalCimUnit()
{
    _flag = FLAG_ALL;
}
NormalCimUnit::NormalCimUnit(const vector<string>& header)
{
    setHeader(header);
}
int NormalCimUnit::getHeaderIndex(const string& headerName) const
{
    for(int i = 0; i < _vecHeader.size(); ++i)
    {
        const ParaRefect& headerPara = _vecHeader[i];
        if(InsensitiveCmp(headerPara._header)(headerName))
        {
            return i;
        }
    }
    return -1;
}

bool NormalCimUnit::handleHeader(vector<char *>& vecHead)
{
    //如果没有表头按默順序，有表头，按表头，如果表头不全報錯
    switch(_flag)
    {
    case FLAG_HEADER:
        CimUnitList::handleHeader(vecHead);
        break;
    case FLAG_ALL:
        _vecHeader.resize(vecHead.size());
        for(uint i = 0; i < vecHead.size(); i++)
        {
            _vecHeader[i]._pos = i;
            _vecHeader[i]._header = vecHead[i];
        }
        break;
    }
    return true;
}

bool NormalCimUnit::handleData(vector<char *>& vecData)
{
    switch(_flag)
    {
    case FLAG_HEADER:
        _tmpData.resize(_header.size());
        for(int i = 0; i < _header.size(); ++i)
        {
            _tmpData[i]  = getData(vecData,i);
        }
        _norData.push_back(_tmpData);
        break;
    case FLAG_ALL:
        _tmpData.resize(vecData.size());
        for(int i = 0; i < vecData.size(); ++i)
        {
            _tmpData[i] = vecData[i];
        }
        if(_tmpData.size() < _vecHeader.size())
        {
            _tmpData.resize(_vecHeader.size());
        }
        _norData.push_back(_tmpData);
        break;
    }
}

void NormalCimUnit::setHeader(const vector<string>& header)
{
    _flag = FLAG_HEADER;
    _header = header;
    initHeader(header.size());
    int i = 0;
    for(vector<string>::const_iterator iterHeader = header.begin();
        iterHeader != header.end(); ++iterHeader,++i)
    {
        CimUnitList::setHeader(i,*iterHeader);
    }
}

void NormalCimUnit::writeCimContent(ofstream& ofCim,string  mark)
{
    for(list<vector<string> >::const_iterator iterNor = _norData.begin();
        iterNor != _norData.end(); ++iterNor)
    {
        const vector<string>& tmpNor = *iterNor;
        if(!tmpNor.empty())
        {
            ofCim << "#";
            vector<string>::const_iterator iterNor = tmpNor.begin();
            ofCim << *iterNor;
            for(;iterNor != tmpNor.end(); ++iterNor)
            {
                ofCim << mark << *iterNor;
            }
            ofCim << "\n";
        }
        else
        {
            ofCim << "#\n";
        }
    }
}
