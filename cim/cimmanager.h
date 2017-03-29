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
    //diffencecheck �õ���
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

    "װ�ö�ֵ",
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
        //diffencecheck �õ���
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
    //����cim·����
    void setFilePath(const string& filePath)
    {
        _filePath = filePath;
    }
    //��ȡ�����ļ��ű�
    bool loadCim()
    {
        return loadCimDetail(false);
    }
    //��ȡ���еı�
    bool loadAllCim()
    {
        return loadCimDetail(true);
    }
    //��ȡ��ͷ���ã����ڻ�û��ô��
    bool loadConf(const string& strConfPath);
    //cimд���ļ�
    bool writeCim();
    //����cim��Ԫ
    void setCimUnit(const CIMTBL& tbl,CimUnit* curCim);
    void setCimUnit(const string& tableName,CimUnit* curCim);

    //�ɱ��������õ�����
    const string& getCimTblName(const CIMTBL& tbl);
    //cim�ļ�ͷ�Ķ�ȡ
    void setEntity(const string& entity)
    {
        _entity = entity;
    }
    void setArea(const string& area)
    {
        _area = area;
    }
    //���ݱ����õ�cim��Ԫ
    CimUnit* findCim(const string& strTable);
    NormalCimUnit* findNorCim(const string& strTable);
    //�õ�����cim��Ԫ
    const map<string,CimUnit*> & getCimMap() const
    {
        return _mapCim;
    }
private:
    //��д����
    char buff[STRING_BUFF_LENGTH];
private:
    //cim��ȡ����
    bool loadCimDetail(bool isAll);
    //cim�ļ������߳�
    void cimReadThread(const string& filePath,CacheQueue<char>* cimQueue,const char cSplit='\t');
    //��һ��cim���зָ�
    bool SplitChar(char *strchar,vector<char*>& vecChar,const char& mark);
    //�õ��ļ���
    string getFileName(const string& strFile);

    //bool loadConfig();
    //����cim��Ԫ
    map<string,CimUnit*> _mapCim;
    //��ȡ����
    CacheQueue<char>  _cimQueue;
    //�ָ��ַ�
    char _markChars;
    //cim�ļ���
    string _filePath;
    //cim�����ļ���
    string _configPath;
    //��ͷ
    //cim�ļ�ͷʵ��
    string _entity;
    //����
    string _area;
};

#endif // CIMMANAGER_H
