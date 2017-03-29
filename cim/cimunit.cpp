#include "cimunit.h"
#include "comm/define.h"
#include <boost/algorithm/string.hpp>

CimUnit::CimUnit()
{
}
CimUnit::~CimUnit()
{
}
bool  CimUnit::handleTable(const char* area,const map<char*,char*> mapAttri)
{
    UNUSED(area);
    UNUSED(mapAttri);
    return true;
}
string CimUnit::makeSureDim(const string& str)
{
    string strTmp = str;
    boost::replace_all(strTmp,"\t","@t@");
    boost::replace_all(strTmp,"\n","@n@");
    if(strTmp.empty())
    {
        return NullChar;
    }
    else
    {
        return strTmp;
    }
}
