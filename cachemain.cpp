#include <iostream>

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
using namespace std;
#include "cache/cachemanager.h"
#include "cache/cachemanager3map.h"
#include "cache/cachemanager2map.h"
#include <boost/ptr_container/ptr_vector.hpp>


#include "DataAccess/jsonstandardcache.h"
#include "comm/norexception.h"
#include "serial/jsonobj.h"

#include "log4cpp/log4cppwraper.h"
#include "cache/maptree.h"
#include "comm/datetime.h"

class BB : public JsonObj
{
public:
    int k;
    string m;
    void clear()
    {
        k = 0;
        m = "";
    }

    template<typename Archive>
    void jsonArchive(Archive& arc)
    {
        arc.json_val(k);
        arc.json_val(m);
    }
};

class AA : public JsonObj
{
public:
    int aa;
    string bb;
    BB m;
    vector<BB> vecM;

    void clear()
    {
        aa = 0;
        bb = "";
    }

    template<typename Archive>
    void jsonArchive(Archive& arc)
    {
        arc.json_val(aa);
        arc.json_val(bb);
        arc.json_val(m);
        arc.json_list(vecM);
    }
};

int main(int argc, char *argv[])
{
    {
        string strdir = argv[0];
        string::size_type dirPos = strdir.find_last_of("/");
        strdir = strdir.substr(0,dirPos);
        chdir(strdir.c_str());
    }
    //log4cpp::Log4CppIni::initLog(argc,argv);

    DateTime firstDate(2006,1,1,4,40,23);
    DateTime SecDate(2005,12,2,5,2,21);
    DateTimeSpan span(6,1,5,30);
    DateTime sub = SecDate - span;
    DateTimeSpan span1(27,56,5,30);
    DateTime sub1 = SecDate + span1;

    map<long,BB> mapData;
    map<long,int> mapRef;
    MapTree<3,long,BB> bbMapTree(mapData,mapRef);
    MapTree<3,long,BB>::Type* child1 = bbMapTree.addChild(12);
    MapTree<2,long,BB>::Type* child11  = child1->addChild(11);
    BB baseBB;
    baseBB.k = 12;
    baseBB.m = "122";
    child11->addChild(13,baseBB);

    log4cpp::Log4CppIni::initLog(argc,argv);
    AA test;
    test.aa = 1;
    test.bb = "太阳";
    test.m.k = 9;
    test.m.m = "大";
    test.vecM.resize(2);
    test.vecM[0].k = 91;
    test.vecM[0].m = "91";
    test.vecM[1].k = 92;
    test.vecM[1].m = "92";
    vector<AA> vecAA;
    vecAA.push_back(test);
    test.aa = 2;
    test.bb = "月亮";
    test.m.k = 10;
    test.m.m = "小";
    test.vecM[0].k = 93;
    test.vecM[0].m = "亚";
    test.vecM[1].k = 94;
    test.vecM[1].m = "东";
    vecAA.push_back(test);
    test.aa = 3;
    test.bb = "星星";
    test.m.k = 11;
    test.m.m = "中";
    test.vecM[0].k = 931;
    test.vecM[0].m = "亚1";
    test.vecM[1].k = 941;
    test.vecM[1].m = "东1";
    vecAA.push_back(test);

    DataCache::CacheManager2Map<long,AA>  bbCache("");
    bbCache.setMapData(12,21,test);
    bbCache.setMapData(12,22,test);
    string strAAJson = bbCache.toJson();
    ofstream objAA("objAA.txt");
    objAA << strAAJson;
    objAA.close();

    string strAA2;
    string strTmp;
    ifstream inObjAA("objAA.txt");
    while(!inObjAA.eof())
    {
        inObjAA >> strTmp;
        strAA2 += strTmp;
    }
    inObjAA.close();

    DataCache::CacheManager2Map<long,AA>  bbCache22("");
    bbCache22.fromJson(strAA2);

    ofstream objAA1("objAA1.txt");
    objAA1 << bbCache22.toJson();
    objAA1.close();

    DataCache::CacheManager2Map<long,string>  bbCacheString("");
    bbCacheString.fromJson(strAA2);
    string strSSAA2 = bbCacheString.toJson();
    ofstream objAA2("objAA2.txt");
    objAA2 << strSSAA2;
    objAA2.close();


   // log4cpp::Log4CppIni::initLog(argc,argv);
    cout << "Hello World!" << endl;

    vector<string> aa;
    aa.push_back("1");
    aa.push_back("2");

    DataCache::JsonStandardCache bb("127.0.0.1",6379,"");
    bb.saveListObj(string("11"),aa);

    vector<int> kk;
    bb.loadListObj("11",kk);


    bb.saveListObjSingle("11",1,"123343");
    bb.removeList("11",1);




    cout << "object begin" << endl;
    bb.delKey("11");
    bb.saveListObj(string("11"),vecAA);
    vector<AA*> vecBB;
    bb.loadListObj(string("11"),vecBB);

    for(uint i = 0; i < vecBB.size(); i++)
    {
        cout << vecBB[i]->aa << "," << vecBB[i]->bb << ",";
        cout << "[" << vecBB[i]->m.k << "," << vecBB[i]->m.m << "]" << ",[";
        for(uint j = 0 ; j < vecBB[i]->vecM.size(); j ++)
        {
            cout << "[" << vecBB[i]->vecM[j].k << "," << vecBB[i]->vecM[j].m  << "]" << ",";
        }
        cout << "]]" << endl;
    }

    cout << "object end" << endl;

    cout << "map object begin" << endl;
    map<string,AA> mapAA;
    test.aa = 1;
    test.bb = "小明";
    mapAA.insert(pair<string,AA>("1",test));
    test.aa = 2;
    test.bb = "中明";
    mapAA.insert(pair<string,AA>("2",test));
    test.aa = 3;
    test.bb = "大明";
    mapAA.insert(pair<string,AA>("3",test));


    bb.delKey("11");
    bb.saveMapObj(string("12"),mapAA);
    map<string,AA> mapBB;
    bb.loadMapObj(string("12"),mapBB);

    AA myaa;
    bb.loadMapObjSingle("12","1",myaa);


   map<string,AA>::iterator iterBB = mapBB.begin();
    for(; iterBB != mapBB.end(); iterBB++)
    {
       cout <<iterBB->first << ":" <<  iterBB->second.aa << "," << iterBB->second.bb;
       cout << "[" << iterBB->second.m.k << "," << iterBB->second.m.m << "]" << ",[";
       for(uint j = 0 ; j < iterBB->second.vecM.size(); j ++)
       {
           cout << "[" << iterBB->second.vecM[j].k << "," << iterBB->second.vecM[j].m  << "]" << ",";
       }
       cout << "]]" << endl;
    }

    cout << "map object end" << endl;

    vector<string> vecKey;
    bb.getMapField("12",vecKey);
    bb.getKeyType("12");
    bb.rename("12","55");

    bb.getKeyFilter("Line*",vecKey);

    int asd;
    bb.loadObj("234",asd);

    //DataCache::ProtectionCheck  proCheck;
//    DataCache::CacheManagerMap<string,DataCache::ProtectionCheck> cacheMan("LineProtectionCheck");
//    map<string,DataCache::ProtectionCheck>* myProCheck = cacheMan.loadCache();
//    map<string,DataCache::ProtectionCheck>::iterator iterProC = myProCheck->begin();

//    cacheMan.saveCache(*myProCheck);

    /*DataCache::CacheManager3Map<DataCache::Breaker> cacheBreaker("Breaker");

    map<string,map<string,map<string,DataCache::Breaker> > >*  mapBreaker = cacheBreaker.loadCache();

    cacheBreaker.saveCache(*mapBreaker);

    map<string,map<string,DataCache::Breaker> > map2Breaker;
    bb.load3MapObjSingle("Breaker","113715890591105026",map2Breaker);

    bb.save3MapObjSingle("Breaker","113715890591105026",map2Breaker);

    bb.save2MapObj("breaker2",map2Breaker);

    map<string,map<string,DataCache::Breaker> > cccBreaker;
    bb.load2MapObj("breaker2",cccBreaker);
    bb.save2MapObj("breaker2",cccBreaker);*/


    map<string,map<string,AA> > mapAA1;

    map<string,AA> map1AA;

    map1AA["001"] = test;
    mapAA1["1000"] = map1AA;

    test.aa = 2;
    test.bb = "月亮";
    test.m.k = 10;
    test.m.m = "小";
    test.vecM[0].k = 93;
    test.vecM[0].m = "亚";
    test.vecM[1].k = 94;
    test.vecM[1].m = "东";
    map1AA["002"] = test;
    mapAA1["1001"] = map1AA;

    test.aa = 3;
    test.bb = "星星";
    test.m.k = 11;
    test.m.m = "中";
    test.vecM[0].k = 931;
    test.vecM[0].m = "亚1";
    test.vecM[1].k = 941;
    test.vecM[1].m = "东1";
    map1AA["003"] = test;
    mapAA1["1002"] = map1AA;

    DataCache::CacheManager2Map<string,AA> cacheManAA("my2map");
    test.aa = 1;
    test.bb = "太阳";
    test.m.k = 9;
    test.m.m = "大";
    test.vecM.resize(2);
    test.vecM[0].k = 91;
    test.vecM[0].m = "91";
    test.vecM[1].k = 92;
    test.vecM[1].m = "92";
    cacheManAA.setMapData("1000","001",test);
    test.aa = 1;
    test.bb = "太阳";
    test.m.k = 9;
    test.m.m = "大";
    test.vecM.resize(2);
    test.vecM[0].k = 91;
    test.vecM[0].m = "91";
    test.vecM[1].k = 92;
    test.vecM[1].m = "92";
    cacheManAA.setMapData("1000","002",test);
    cacheManAA.saveCacheAll();

    map<string,map<string,AA> > mapbb;
    DataCache::CacheManager2Map<string,AA> cacheManbb("my2map");
    cacheManbb.loadCacheAll();


    DataCache::CacheManager3Map<string,AA> cache3Map("myymap3");

    test.aa = 1;
    test.bb = "太阳";
    test.m.k = 9;
    test.m.m = "大";
    test.vecM.resize(2);
    test.vecM[0].k = 91;
    test.vecM[0].m = "91";
    test.vecM[1].k = 92;
    test.vecM[1].m = "92";
    cache3Map.setMapData("1","11","112",test);
    cache3Map.setMapData("1","12","113",test);
    cache3Map.setMapData("1","13","114",test);

    test.aa = 1;
    test.bb = "太阳";
    test.m.k = 9;
    test.m.m = "大";
    test.vecM.resize(2);
    test.vecM[0].k = 91;
    test.vecM[0].m = "91";
    test.vecM[1].k = 92;
    test.vecM[1].m = "92";
    cache3Map.setMapData("2","14","111",test);
    cache3Map.setMapData("2","14","112",test);
    cache3Map.setMapData("2","14","113",test);

    cache3Map.saveCacheAll();


    DataCache::CacheManager3Map<string,AA> cache3Map1("myymap3");
    cache3Map1.loadCacheAll();

    std::vector<std::string> vecBreaker;
    bb.getKeyFilter("B*",vecBreaker);
    return 0;
}

