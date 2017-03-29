#ifndef USEDEQUIPMENTCIMUNIT_H
#define USEDEQUIPMENTCIMUNIT_H

#include "cim/cimunitlist.h"
#include <list>
#include "comm/define.h"

#include "comm/norexception.h"

class OnlyOneCimUnit : public CimUnit
{
public:
    OnlyOneCimUnit();

    virtual ~OnlyOneCimUnit(){}


    virtual int getHeaderSize()
    {
        return 1;
    }

    virtual void setHeader(unsigned int index,const string& header)
    {
        UNUSED(index);
        UNUSED(header);
        return;
    }

    virtual bool handleHeader(vector<char *>& vecHead)
    {
        UNUSED(vecHead);
        return false;
    }

    virtual bool handleData(vector<char *>& vecData)
    {
        if(vecData.size() > _index)
        {
            _vecUsedEqui.push_back(vecData[_index]);
        }
        return true;
    }

    virtual bool handelDataEnd()
    {
        return false;
    }

    list<string>& getCimData()
    {
        return _vecUsedEqui;
    }
    void clearCimData()
    {
        _vecUsedEqui.clear();
    }
    void addCimData(const string& strData)
    {
        _vecUsedEqui.push_back(strData);
    }
    //дcim
    virtual void writeCim(ofstream& ofCim,string mark);
    virtual void writeCimContent(ofstream& ofCim,string mark);

    void setIndex(const int& index)
    {
        _index = index;
        assert(_index >= 0);
    }

private:
    list<string>  _vecUsedEqui;
    int _index;
};

#endif // USEDEQUIPMENTCIMUNIT_H
