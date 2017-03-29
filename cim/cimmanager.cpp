#include "cimmanager.h"

#include <unistd.h>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <stdio.h>
#include <algorithm>
#include <string>
#include <boost/algorithm/string.hpp>
#include "log4cpp/log4cppwraper.h"
#include "normalcimunit.h"
#include <boost/regex.hpp>


CimManager::CimManager(const string& filePath,const char& markChar,
                       const string& configPath) :
    _filePath(filePath),
    _configPath(configPath)
{
     _markChars=markChar;
     _area = "";
     _entity = "";
}
CimManager::~CimManager()
{
}
/*
bool CimManager::loadConfig()
{
    //通过文件获得不带后綴的文件名
    string::size_type posDot = _filePath.find_last_of()

}*/

CimUnit* CimManager::findCim(const string& strTable)
{
    map<string,CimUnit*>::iterator iterCim = _mapCim.find(strTable);
    if(iterCim != _mapCim.end())
    {
        return iterCim->second;
    }
    else
    {
        return NULL;
    }
}

NormalCimUnit* CimManager::findNorCim(const string& strTable)
{
    CimUnit* tmpCim = findCim(strTable);
    if(tmpCim)
    {
        return dynamic_cast<NormalCimUnit*>(tmpCim);
    }
    return NULL;
}

bool CimManager::loadConf(const string& strConfPath)
{
    FILE *fp = NULL;
    if((fp = fopen(strConfPath.c_str(), "r")) == NULL)
    {
        return false;
    }
    char buff[STRING_BUFF_LENGTH];
    vector<char*> vecSplit;
    while(fgets(buff, STRING_BUFF_LENGTH, fp) != NULL)
    {
        switch(buff[0])
        {
        case '/':
            continue;
        }
        int size = strlen(buff);
        int j;
        for(j = size - 1; j > 0; j --)
        {
            if(' ' != buff[j] && '\n' != buff[j] && '\r' != buff[j])
            {
                buff[j + 1] = 0;
                break;
            }
        }
        if(j < 0)
        {
            continue;
        }

        SplitChar(buff,vecSplit,'=');
        if(vecSplit.size() < 2)
        {
            continue;
        }
        string strTable = vecSplit[0];
        map<string,CimUnit*>::iterator iteCim = _mapCim.find(strTable);
        if(iteCim != _mapCim.end())
        {
            CimUnit* tmpCim = iteCim->second;
            SplitChar(vecSplit[1],vecSplit,',');
            if((int)vecSplit.size() == tmpCim->getHeaderSize())
            {
                for(uint i = 0; i < vecSplit.size(); i++)
                {
                    tmpCim->setHeader(i,vecSplit[i]);
                }
            }
        }
    }
    return true;
}

bool CimManager::writeCim()
{
    ofstream ofCim(_filePath.c_str());
    map<string,CimUnit*>::iterator iterCim =  _mapCim.begin();
    string strMark(1,_markChars);
    if(!_entity.empty())
    {
        ofCim << _entity;
    }
    for(;iterCim != _mapCim.end(); iterCim++)
    {
        CimUnit* tmpCim = iterCim->second;
        const string& strTable = iterCim->first;
        if(_area.empty())
        {
            ofCim << "<" << strTable << ">" <<endl;
            tmpCim->writeCim(ofCim,strMark);
            ofCim << "</" << strTable << ">" <<endl;
        }
        else
        {
            ofCim << "<" << strTable << ":"  << _area <<  ">" <<endl;
            tmpCim->writeCim(ofCim,strMark);
            ofCim << "</" << strTable <<":"  << _area << ">" <<endl;
        }
    }
    return true;
}

string CimManager::getFileName(const string& strFile)
{
    string strName;
    string::size_type pos = _filePath.find_last_of("/");
    if(pos == string::npos)
    {
        pos = 0;
    }
    else
    {
        pos ++;
    }
    string::size_type posDot = _filePath.find(".",pos);
    if(posDot != string::npos)
    {
        strName = _filePath.substr(pos,posDot - pos);
    }
    else
    {
        strName = _filePath.substr(pos);
    }
    return strName;
}

char* zeroReturnAfter(char* data,char deli)
{
    char* dataSec = strchr(data,deli);
    do
    {
        *dataSec = 0;
        ++dataSec;
    }while(deli == *dataSec);
    return 0 == *dataSec? NULL:dataSec;
}

bool CimManager::loadCimDetail(bool isAll)
{
    //判文件是否存在
    if(::access(_filePath.c_str(),R_OK))
    {
        return false;
    }
    //荻得配置文件路径

    string strConf = _configPath + "/" + getFileName(_filePath) + ".conf";
    loadConf(strConf);
    boost::thread thrd(boost::bind(&CimManager::cimReadThread,this,_filePath,&_cimQueue,_markChars));

    int flag = HeaderBegin;
    CimUnit* curCim = NULL;
    string strTable;

    string strArea;
    string strEnd;
    vector<char *> vecSegTag;
    map<char*,char*> mapAttri;
    bool bFindHead;
    int len = 0;
    int iIndex = 0;
    _entity = "";
    while(true)
    {
        char* tmpBuf = _cimQueue.getBuf();
        if(!tmpBuf)
        {
            LOG_DEBUG("cimman get buf null maybe not run and queue empty!");
            break;
        }
        //int size = strlen(tmpBuf);
        switch(tmpBuf[0])
        {
        case '/':
            if(ColumName == flag && curCim)
            {
                curCim->setNote(tmpBuf);
            }
            continue;
        case '<':
            len = strlen(tmpBuf);
            if(len > 1 && '!' == tmpBuf[1] || len > 2 && 0 == strncmp("！", tmpBuf+1,2))
            {
                if(0 == iIndex)
                {
                    _entity = tmpBuf;
                }
                continue;
            }
            break;
        }
        iIndex++;

        if(tmpBuf == NULL)
        {
            _cimQueue.freePool(tmpBuf);
            continue;
        }
        switch(flag)
        {
        case HeaderBegin:
            bFindHead = true;

            if(strlen(tmpBuf) < 3)
            {
                bFindHead = false;
            }
            else if('<' != tmpBuf[0] || '/' == tmpBuf[1] || '>' != tmpBuf[strlen(tmpBuf) - 1])
            {
                bFindHead = false;
            }
            else
            {
                //char* pos = strchr(tmpBuf+1,':');
                //解析表名
                mapAttri.clear();
                tmpBuf[strlen(tmpBuf) - 1] = 0;
                char* dataBegin = tmpBuf+1;
                char* dataContinue = strchr(dataBegin,' ');
                if(dataContinue)
                {
                    char* dataContinue = zeroReturnAfter(dataBegin,' ');
                    char* dataSec= strchr(dataBegin,':');
                    if(dataSec)
                    {
                        char* dataSec = zeroReturnAfter(dataBegin,':');
                        strTable = dataBegin;
                        strArea = dataSec;
                    }
                    else
                    {
                        strTable = dataBegin;
                        strArea = "";
                    }
                    //处理属性，先不做
                    if(dataContinue)
                    {
                        do
                        {
                            char* proterty = dataContinue;
                            dataContinue = strchr(proterty,' ');
                            if(dataContinue)
                            {
                                dataContinue = zeroReturnAfter(dataContinue,' ');
                            }
                            //提出属性
                            char* valData = strchr(proterty,'=');
                            if(valData)
                            {
                                *valData++ =  0;
                                mapAttri[proterty] = valData;
                            }
                        }
                        while(dataContinue);
                    }
                }
                else
                {
                    char* dataContinue = strchr(dataBegin,':');
                    if(dataContinue)
                    {
                        char* dataContinue = zeroReturnAfter(dataBegin,':');
                        strTable = dataBegin;
                        strArea = dataContinue;
                    }
                    else
                    {
                        strTable = dataBegin;
                        strArea = "";
                    }
                }
            }
            if(bFindHead)
            {
                if(isAll)
                {
                    map<string,CimUnit*>::iterator iterCimUnit =  _mapCim.find(strTable);
                    if(iterCimUnit != _mapCim.end())
                    {
                        curCim = iterCimUnit->second;
                        curCim->handleTable(strArea.c_str(),mapAttri);

                    }
                    else
                    {
                        curCim = new NormalCimUnit;
                        _mapCim[strTable] = curCim;
                        curCim->handleTable(strArea.c_str(),mapAttri);
                    }
                }
                else
                {
                    map<string,CimUnit*>::iterator iterCimUnit =  _mapCim.find(strTable);
                    if(iterCimUnit != _mapCim.end())
                    {
                        curCim = iterCimUnit->second;
                        curCim->handleTable(strArea.c_str(),mapAttri);

                    }
                    else
                    {
                        LOG_DEBUG("can not find table:%s cimunit",strTable.c_str());
                    }
                }
                flag = ColumName;
            }
            else
            {
                LOG_DEBUG("find table:%s error",strTable.c_str());
                _cimQueue.signalReturn(CacheQueue<string>::CIM_FAILED);
            }
            break;
        case ColumName:
            if('@' == tmpBuf[0])
            {
                if(curCim)
                {
                      SplitChar(tmpBuf + 1,vecSegTag,_markChars);
                      curCim->handleHeader(vecSegTag);
                }
                flag = Row;
            }
            else
            {
                LOG_DEBUG("cim table:%s can not find header",strTable.c_str());
                _cimQueue.signalReturn(CacheQueue<string>::CIM_FAILED);
            }
            break;
        case Row:
            switch (tmpBuf[0]) {
            case '#':
                if(curCim)
                {
                    SplitChar(tmpBuf + 1,vecSegTag,_markChars);
                    curCim->handleData(vecSegTag);
                }
                //flag = Row;
                break;
            case '<':
                if(strArea.empty())
                {
                    strEnd = "</" +  strTable + " *>";
                }
                else
                {
                    if('?' != strArea[0])
                    {
                        strEnd = "</" +  strTable + "(::" + strArea + ")?( .*| *)>";
                    }
                    else
                    {
                        strEnd = "</" +  strTable + "::.*>";
                    }
                }
                if(boost::regex_match(tmpBuf,boost::regex(strEnd)))
                {
                    if(curCim)
                    {
                        curCim->handelDataEnd();
                        if(!isAll)
                        {
                            _mapCim.erase(strTable);
                            if(_mapCim.empty())
                            {
                                LOG_DEBUG("cim all table read",strTable.c_str());
                                _cimQueue.signalReturn(CacheQueue<string>::CIM_SUCCESS);
                            }
                        }
                    }
                    flag = HeaderBegin;
                    curCim = NULL;
                }
                else
                {
                    LOG_DEBUG("cim table:%s row for < or #,but:%s,index:%d",strTable.c_str(),tmpBuf,iIndex);
                    _cimQueue.signalReturn(CacheQueue<string>::CIM_FAILED);
                }
                break;
            default:
                LOG_DEBUG("cim table:%s row for < or #,but:%s,index:%d",strTable.c_str(),tmpBuf,iIndex);
                _cimQueue.signalReturn(CacheQueue<string>::CIM_FAILED);
                break;
            }
            break;
        case HeaderEnd:
        case BadRow:
        default:
            LOG_DEBUG("cim table:%s bad line",strTable.c_str());
            _cimQueue.signalReturn(CacheQueue<string>::CIM_FAILED);
            break;
        }
        _cimQueue.freePoolArray(tmpBuf);
        if(!_cimQueue.isSuccess())
        {
            break;
        }
    }
    thrd.join();
    return _cimQueue.isSuccess();
}
void CimManager::setCimUnit(const CIMTBL& tbl,CimUnit* curCim)
{
    setCimUnit(TABLE_CIM[tbl],curCim);
}
const string& CimManager::getCimTblName(const CIMTBL& tbl)
{
    return TABLE_CIM[tbl];
}

void CimManager::setCimUnit(const string& tableName,CimUnit* curCim)
{
    _mapCim[tableName] = curCim;
}

void CimManager::cimReadThread(const string& filePath,CacheQueue<char>* cimQueue,const char cSplit)
{
    FILE *fp = fopen(filePath.c_str(), "r");
    if(!fp)
    {
        LOG_DEBUG("file:%s open failed",filePath.c_str());
        cimQueue->signalReturn(CacheQueue<string>::CIM_FAILED);
        return;
    }
    LOG_DEBUG("file:%s open begin",filePath.c_str());
    while(fgets(buff, STRING_BUFF_LENGTH, fp) != NULL) {
        int size = strlen(buff);

        int i = 0;
        for(; i < size; i++)
        {
            if(' '  != buff[i])
            {
                break;
            }
        }
        //去掉空格和换行符和分割符
        int j;
        for(j = size - 1; j >= i; j --)
        {
            if(' ' != buff[j] && '\n' != buff[j] && '\r' != buff[j] && cSplit != buff[j])
            {
                buff[j + 1] = 0;
                break;
            }
        }
        if(i > j)
        {
            continue;
        }

        switch(buff[i])
        {
        case '/':
//            continue;
//            break;
        case '<':
//            if(strlen(buff) > 1 && '!' == buff[1] || strlen(buff) > 2 && 0 == strncmp(buff + 1,"！",2))
//            {
//                continue;
//            }
            break;
        case '@':
        case '#':
            break;
        default:
            continue;
            break;
        }


        char* node = cimQueue->getPoolArray(size - i  + 1);
        if(NULL == node) {
            LOG_DEBUG("thread poll error");
            cimQueue->signalReturn(CacheQueue<string>::CIM_FAILED);
            break;
        }
        strcpy(node,buff + i);
        cimQueue->pushBuf(node);

        if(!cimQueue->isRun())
        {
            break;
        }
    }
    LOG_DEBUG("file:%s open end",filePath.c_str());
    cimQueue->signalReturn(CacheQueue<string>::CIM_SUCCESS);
}
bool CimManager::SplitChar(char *strchar, vector<char *> &vecChar,const char& mark)
{
    //char *tmpchar;
    //strcpy(tmpchar,strchar.c_str());
    vecChar.clear();
    char * tmp = strchar;
    char *before = NULL;
    for(;*tmp != 0;  tmp++)
    {
        if(before)
        {
            if(mark == *tmp)
            {
                *tmp = 0;
                before = NULL;
            }
        }
        else
        {
            if(mark != *tmp)
            {
                before = tmp;
                vecChar.push_back(tmp);
            }
        }
    }
    return true;
}
