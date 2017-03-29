// Hello World example
// This example shows basic usage of DOM-style API.

#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "rapidjson/filestream.h"   // wrapper of C stream for prettywriter as output
#include <cstdio>
#include <iostream>
#include <time.h>
#include <list>
#include "log4cpp/log4cppwraper.h"
#include "serial/jsonserial.h"

using namespace rapidjson;
using namespace std;

#include "jsonserial.h"


//#include "user.h"
class BB : public JsonObj
{
public:
    BB()
    {
        c = "aaa";
        k = 0;
    }
    template<typename Archive>
    void jsonArchive(Archive& arc)
    {
        arc.json_val(c);
        arc.json_val(k);
    }
private:
    string c;
    int k;
};
class AAA : public JsonObj
{
public:
    AAA()
    {
    }

    void clear()
    {
        index = 0;
        mRID = 0;
        my = "";
    }

    /*
    string toJson() const;
    bool fromJson(const Json::Value& valJson);
    */
    template<typename Archive>
    void jsonArchive(Archive& arc)
    {
        arc.json_val(index);
        arc.json_val(mRID);
        arc.json_val(my);
        arc.json_val(_bb);
    }

 public:
    long index;//0,«¯”Ú±‡∫≈
    long mRID;//1,«¯”Ú±Í ∂£¨Œ®“ª
    string my;
    BB _bb;
};
int main(int argc, char *argv[]) {

    {
        string strdir = argv[0];
        string::size_type dirPos = strdir.find_last_of("/");
        strdir = strdir.substr(0,dirPos);
        chdir(strdir.c_str());
    }

    log4cpp::Log4CppIni::initLog(argc,argv);
    map<long,AAA> mapAA;
    AAA aa;
    aa.index = 1;
    aa.mRID = 10;
    aa.my = " •œÕ";
    mapAA.insert(make_pair(aa.index,aa));
    aa.index = 2;
    aa.mRID = 11;
    aa.my = "ºÈ‘Ù";
    mapAA.insert(make_pair(aa.index,aa));
    JsonSerial::serialObjMapToFile("aaa.json",mapAA);

    map<long,AAA> mapBB;
    JsonSerial::deSerialObjMapFromFile(mapBB,"aaa.json");

    list<AAA> listAA;
    aa.index = 1;
    aa.mRID = 10;
    aa.my = " •œÕ";
    listAA.push_back(aa);
    aa.index = 2;
    aa.mRID = 11;
    aa.my = "ºÈ‘Ù";
    listAA.push_back(aa);
    JsonSerial::serialObjListToFile("bbb.json",listAA);

    list<AAA> listBB;
    JsonSerial::deSerialObjListFromFile(listBB,"bbb.json");

    return 0;
}
