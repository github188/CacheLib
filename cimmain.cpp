#include <iostream>

#include <stdlib.h>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

#include "log4cpp/log4cppwraper.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
using namespace std;


#include "cim/cimmanager.h"
struct MySt
{
    vector<char *> _data;
    bool fromCim(const vector<char *>& cim)
    {
        _data = cim;
        return true;
    }
};

class BasePowerCim : public CimUnit
{
public:
    BasePowerCim(vector<MySt>& vecOut) : _data(vecOut)
    {
    }

    virtual ~BasePowerCim()
    {
    }

    virtual bool handleHeader(vector<char *>& vecHead)
    {
        MySt tmpSt;
        tmpSt._data = vecHead;
        _data.push_back(tmpSt);
        return true;
    }

    virtual bool handleData( vector<char *>& vecData)
    {
        MySt tmpSt;
        tmpSt._data = vecData;
        _data.push_back(tmpSt);
        return true;
    }
    virtual bool handelDataEnd()
    {
          return false;
    }

private:
    string _area;
    vector<MySt>& _data;

};

int main(int argc, char *argv[])
{
    {
        string strdir = argv[0];
        string::size_type dirPos = strdir.find_last_of("/");
        strdir = strdir.substr(0,dirPos);
        chdir(strdir.c_str());
    }

    log4cpp::Log4CppIni::initLog(argc,argv);


    string s = ",Hello,, the beautiful world!";
    vector<string> rs;
    boost::split( rs, s, boost::is_any_of( " ,!" ), boost::token_compress_on );


    return 0;

    cout << "Hello World!" << endl;
//    CimManager cimMan("ZDOneTimeEquipement.CIME");
//    map<long,DataCache::BaseVoltage> curData;
//    BaseVoltageCimUnit basePowerCim(curData);
//    cimMan.setCimUnit(CimManager::CTBL_BASEVOLTAGE,&basePowerCim);

    //vector<MySt> curData1;
    /*BasePowerCim basePowerCim1(curData1);*/
    //cimMan.setLoadData(CimManager::CTBL_SUBSTATION,curData1);
    //cimMan.loadCim();
    return 0;
}

