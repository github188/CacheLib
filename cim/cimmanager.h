#ifndef CIMMANAGER_H
#define CIMMANAGER_H

#include <string>
#include <list>
#include <map>
#include <vector>
using namespace std;

#define STRING_BUFF_LENGTH  1024*20

#include "cim/cimunit.h"
#include "comm/cachequeue.h"
#include "cim/cimunitlist.h"
#include "cim/normalcimunit.h"

const string TABLE_CIM[] = {
    "BasePower",
    "ControlArea",
    "BaseVoltage",
    "Substation",
    "BusbarSection",
    "Breaker",
    "Disconnector",
    "SynchronousMachine",
    "ACLineDot",
    "ACLineSegment",
    "Load",
    "GroundDisconnector",
    "TransformerWinding",
    "PowerTransformer",
    "SeriesCompensator",
    "DCLineDot",
    "DCLineSegment",
    "UsedEquipment",
    "Correspondence",
    "CacheEquiValueSource",
    "CacheEquivalentLine",
    "MutualInductance",
    "RunWay",
    "TopologicalPhysics",
    "CacheBreaker",
    "CachePowerTransformer",
    //diffencecheck 用到的
    "LineProtectionRuleCheck",
    "BusProtectionRuleCheck",
    "TranProtectionRuleCheck",
    "LineProtectionVariableCheck",
    "BusProtectionVariableCheck",
    "TranProtectionVariableCheck",

    "BreakerModeInfo",

    "BusProtectionCheck",
    "BusItemCheck",
    "BusRuleCheck",
    "BusVariableCheck",

    "LineProtectionCheck",
    "LineItemCheck",
    "LineRuleCheck",
    "LineVariableCheck",

    "TranProtectionCheck",
    "TranItemCheck",
    "TranRuleCheck",
    "TranVariableCheck",

    "GeneratorProtectionCheck",
    "GeneratorItemCheck",
    "GeneratorRuleCheck",
    "GeneratorVariableCheck",

    "TranProtectionShWCheck",
    "TranItemShWCheck",
    "TranRuleShWCheck",
    "TranVariableShWCheck",


    "ProtectionCheckVersion",
    "CimAddBus",
    "CimAddLine",
    "CimAddPowerTransformer",
    "CimAddSynchronousMachine",

    "装置定值",
    "LINEEQUIPMENTSETTINGITEMS",
    "BUSEQUIPMENTSETTINGITEMS",
    "TRANEQUIPMENTSETTINGITEMS",
    "BorderData"
};

class CimManager
{
public:
    enum CIMTBL
    {
        CTBL_BASEPOWER = 0,
        CTBL_CONTROLAREA,
        CTBL_BASEVOLTAGE,
        CTBL_SUBSTATION,
        CTBL_BUSBARSECTION,
        CTBL_BREAKER,
        CTBL_DISCONNECTOR,
        CTBL_SYNCHRONOUSMACHINE,
        CTBL_ACLINEDOT,
        CTBL_ACLINESEGMENT,
        CTBL_LOAD,
        CTBL_GROUNDDISCONNECTOR,
        CTBL_TRANSFORMERWINDING,
        CTBL_POWERTRANSFORMER,
        CTBL_SERIESCOMPENSATOR,
        CTBL_DCLINEDOT,
        CTBL_DCLINESEGMENT,
        CTBL_USEDEQUIPMENT,
        CTBL_CORRESPONDENCE,
        CTBL_CACHEEQUIVALENTSOURCE,
        CTBL_CACHEEQUIVALENTLINE,
        CTBL_MUTUALINDUCTANCE,
        CTBL_RUNWAY,
        CTBL_TOPOLOGICALPHYSICS,
        CTBL_CACHEBREAKER,
        CTBL_CACHEPOWERTRANSFORMER,
        //diffencecheck 用到的
        CTBL_DIFFCK_BEGIN,
        CTBL_LineProtectionRuleCheck=CTBL_DIFFCK_BEGIN,
        CTBL_BusProtectionRuleCheck,
        CTBL_TranProtectionRuleCheck,
        CTBL_LineProtectionVariableCheck,
        CTBL_BusProtectionVariableCheck,
        CTBL_TranProtectionVariableCheck,
        CTBL_DIFFCK_END,

        CTBL_BREAKERMODE = CTBL_DIFFCK_END,

        CTBL_BusCheck,
        CTBL_BusItemCheck,
        CTBL_BusRuleCheck,
        CTBL_BusVariableCheck,

        CTBL_LineCheck,
        CTBL_LineItemCheck,
        CTBL_LineRuleCheck,
        CTBL_LineVariableCheck,

        CTBL_TranCheck,
        CTBL_TranItemCheck,
        CTBL_TranRuleCheck,
        CTBL_TranVariableCheck,


        CTBL_GenerCheck,
        CTBL_GenerItemCheck,
        CTBL_GenerRuleCheck,
        CTBL_GenerVariableCheck,

        CTBL_TranShwCheck,
        CTBL_TranShwItemCheck,
        CTBL_TranShwRuleCheck,
        CTBL_TranShwVariableCheck,


        CTBL_ProtectionCheckVersion,
        CTBL_CimAddBus, //
        CTBL_CimAddLine,
        CTBL_CimAddPowerTransformer,
        CTBL_CimAddSynchronousMachine,

        CTBL_EquSetting,

        CTBL_LINEEQUIPMENTSETTINGITEMS,
        CTBL_BUSEQUIPMENTSETTINGITEMS,
        CTBL_TRANEQUIPMENTSETTINGITEMS,
        CTBL_BORDERDATA
    };
    enum LineType {
        HeaderBegin,
        ColumName,
        Row,
        HeaderEnd,
        BadRow
    };
    CimManager(const string& filePath,const char& markChar = '\t',
               const string& configPath = "../hisdata/cimconf");
    virtual ~CimManager();
    //设置cim路径名
    void setFilePath(const string& filePath)
    {
        _filePath = filePath;
    }
    //读取给定的几张表
    bool loadCim()
    {
        return loadCimDetail(false);
    }
    //读取所有的表
    bool loadAllCim()
    {
        return loadCimDetail(true);
    }
    //读取表头配置，现在还没怎么用
    bool loadConf(const string& strConfPath);
    //cim写入文件
    bool writeCim();
    //设置cim单元
    void setCimUnit(const CIMTBL& tbl,CimUnit* curCim);
    void setCimUnit(const string& tableName,CimUnit* curCim);

    //由表名索引得到表名
    const string& getCimTblName(const CIMTBL& tbl);
    //cim文件头的读取
    void setEntity(const string& entity)
    {
        _entity = entity;
    }
    void setArea(const string& area)
    {
        _area = area;
    }
    //更据表名得到cim单元
    CimUnit* findCim(const string& strTable);
    NormalCimUnit* findNorCim(const string& strTable);
    //得到所有cim单元
    const map<string,CimUnit*> & getCimMap() const
    {
        return _mapCim;
    }
private:
    //读写缓存
    char buff[STRING_BUFF_LENGTH];
private:
    //cim读取主体
    bool loadCimDetail(bool isAll);
    //cim文件解析线程
    void cimReadThread(const string& filePath,CacheQueue<char>* cimQueue,const char cSplit='\t');
    //对一行cim进行分割
    bool SplitChar(char *strchar,vector<char*>& vecChar,const char& mark);
    //得到文件名
    string getFileName(const string& strFile);

    //bool loadConfig();
    //所有cim单元
    map<string,CimUnit*> _mapCim;
    //读取队列
    CacheQueue<char>  _cimQueue;
    //分隔字符
    char _markChars;
    //cim文件名
    string _filePath;
    //cim配置文件名
    string _configPath;
    //表头
    //cim文件头实体
    string _entity;
    //区域
    string _area;
};

#endif // CIMMANAGER_H
