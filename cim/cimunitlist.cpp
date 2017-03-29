#include "cimunitlist.h"
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>

//增加onetime中mrid字符串转换全局变量
//TG 2015-11-20
int MRIDConvert::g_iVersionFlag = 0;
long MRIDConvert::g_iMRIDIndex = 0;

map<string, long> MRIDConvert::g_Mridmap;
map<string, Correspondence> MRIDConvert::g_mapCorrespondence;

long MRIDConvert::g_ConvertMrid(const char* ID)
{

    if (g_iVersionFlag == 0)
    {
        return atol(ID);
    }

    if (g_mapCorrespondence.end() != g_mapCorrespondence.find(ID))
    {
        return g_mapCorrespondence[ID].mRID;
    }
    else
    {
        ++g_iMRIDIndex;
        struct timeval tv;
        gettimeofday(&tv,NULL);
        long number = tv.tv_sec * 1000 + tv.tv_usec;
        Correspondence tmp;
        tmp.ID = ID;
        tmp.mRID = number * 10000 + g_iMRIDIndex;
        tmp.name = "NULL";
        g_mapCorrespondence[ID] = tmp;
        return (number * 10000 + g_iMRIDIndex);
    }
}


InsensitiveCmp::InsensitiveCmp(string other) : _cmp(other)
{
    for(uint i = 0; i < _cmp.size();i++)
    {
        if(_cmp[i] >= 'A' && _cmp[i] <= 'Z')
        {
            _cmp[i] = _cmp[i] + 'a' - 'A';
        }
    }
}

bool InsensitiveCmp::operator()(const char* first)
{
    char tmpFirst;
    const char* other = _cmp.c_str();
    while(*first && *other)
    {
        if(*first >= 'A' && *first <= 'Z')
        {
            tmpFirst= *first + 'a' - 'A';
        }
        else
        {
            tmpFirst = *first;
        }
        if(tmpFirst == *other)
        {
            first++;
            other++;
        }
        else
        {
            return false;
        }
    }
    return (0 == *first) && (0 == *other);
}

bool InsensitiveCmp::operator()(const string& first)
{
    return operator()(first.c_str());
}

void CimUnitList::setHeader(int index,const string& header,int pos)
{
    if(index >= (int)_vecHeader.size())
    {
        return;
    }
    switch(header[0])
    {
    case '$':
    {
        string::size_type pos = header.find(":");
        if(pos != string::npos)
        {
            _vecHeader[index]._header = header.substr(1,pos - 1);
            _vecHeader[index]._default = header.substr(pos + 1);
        }
        else
        {
            _vecHeader[index]._header = header.substr(1);
            _vecHeader[index]._default = "";
        }
        _vecHeader[index]._flag = ParaRefect::PARA_DEFAULT;
    }
        break;
    default:
        _vecHeader[index]._header = header;
        _vecHeader[index]._flag = ParaRefect::PARA_DATA;
        _vecHeader[index]._pos = pos;
        break;
    }
}
void CimUnitList::setHeader(unsigned int index,const string& header)
{
    if(index >= _vecHeader.size())
    {
        return;
    }

    switch(header[0])
    {
    case '$':
    {
        string::size_type pos = header.find(":");
        if(pos != string::npos)
        {
            _vecHeader[index]._header = header.substr(1,pos - 1);
            _vecHeader[index]._default = header.substr(pos + 1);
        }
        else
        {
            _vecHeader[index]._header = header.substr(1);
            _vecHeader[index]._default = "";
        }
        _vecHeader[index]._flag = ParaRefect::PARA_DEFAULT;
    }
        break;
    default:
        _vecHeader[index]._header = header;
        _vecHeader[index]._flag = ParaRefect::PARA_DATA;
        break;
    }
}

const char* CimUnitList::getData(const vector<char*>& vecData,int index)
{
    if(_vecHeader[index]._pos >= 0 && _vecHeader[index]._pos < vecData.size())
    {
        switch(_vecHeader[index]._flag)
        {
        case ParaRefect::PARA_DATA:
            if(vecData[_vecHeader[index]._pos])
            {
                return vecData[_vecHeader[index]._pos];
            }
            else
            {
                return myEmpty;
            }
            break;
        case ParaRefect::PARA_DEFAULT:
            return _vecHeader[index]._default.c_str();
            break;
        default:
            return myEmpty;
        }
    }
    else
    {
        return myEmpty;
    }
}

bool CimUnitList::isHasCol(int index)
{
    if(_vecHeader[index]._pos >= 0)
    {
        switch(_vecHeader[index]._flag)
        {
        case ParaRefect::PARA_DATA:
            return true;
            break;
        default:
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool CimUnitList::handleHeader(vector<char *>& vecHead)
{
    //如果没有表头按默順序，有表头，按表头，如果表头不全報錯
    for(uint i = 0; i < _vecHeader.size(); i++)
    {
        vector<char *>::iterator iterOut = find_if(vecHead.begin(),vecHead.end(),InsensitiveCmp(_vecHeader[i]._header));
        if(iterOut != vecHead.end())
        {
            _vecHeader[i]._pos = iterOut - vecHead.begin();
        }
        else
        {
            _vecHeader[i]._pos = -1;
        }
    }
    return true;
}


bool CimUnitList::handleData(vector<char*>& vecData)
{
    /*vector<string>::iterator iterData = vecData.begin();
    for(;iterData != vecData.end(); iterData++)
    {
        string& tmpData = *iterData;
        if("NULL" == tmpData)
        {
            tmpData = "";
        }
    }*/
    if(vecData.size() < _vecHeader.size())
    {
        vecData.resize(_vecHeader.size());
    }
    return true;
}
void CimUnitList::writeCim(ofstream& ofCim,string mark)
{
    vector<ParaRefect>::iterator iterHeader =  _vecHeader.begin();
    bool bHeader = false;
    if(iterHeader != _vecHeader.end())
    {
        ParaRefect& tmpPara = *iterHeader;
        ofCim << "@" << tmpPara._header;
        iterHeader++;
        bHeader = true;
    }
    for(;iterHeader != _vecHeader.end(); iterHeader++)
    {
        ParaRefect& tmpPara = *iterHeader;
        ofCim << mark << tmpPara._header;
    }
    if(bHeader)
    {
        ofCim << endl;
    }
    writeCimContent(ofCim,mark);
}
void CimUnitList::writeCimContent(ofstream& ofCim,string  mark)
{

}

bool CimUnitList::handelDataEnd()
{
    return true;
}
//dcb 2015-12-18
long CimUnitList::nullToNegOne(const char* xNode){
        if(0 == strcmp("NULL",xNode))
        {
            return  -1;
        }else{
            return atol(xNode);
        }
}
