#include "odbcpool.h"

#include "comm/zini.h"

#define CONFIGPATH "../config/server.ini"

namespace DB
{
#define MAX_CONN  2

OdbcPool* OdbcPool::_instance = NULL;

OdbcPool::OdbcPool()
{
    SQLRETURN retcode; //����ֵ
   /*���价�����*/
   retcode = SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&_henv);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
       throw NorException("odbc���ӻ�����ʼ��ʧ��");
   }
   retcode = SQLSetEnvAttr(_henv,SQL_ATTR_ODBC_VERSION,(void*)SQL_OV_ODBC3,0);
   if(retcode != SQL_SUCCESS &&  retcode != SQL_SUCCESS_WITH_INFO)
   {
       throw NorException("odbc���ӻ�����ʼ���汾ʧ��");
   }
   loadConf();
}

OdbcPool::~OdbcPool()
{
    SQLFreeHandle(SQL_HANDLE_ENV,_henv);
}

bool OdbcPool::loadConf()
{
    for(int i = 0; i < DB_MAX; i++)
    {
        string strSection = "ServerConnection" + boost::lexical_cast<string>(i + 1);
        _connConfig[i]._dns = ZIni::readString(strSection.c_str(), "dns", "", CONFIGPATH);
        _connConfig[i]._user = ZIni::readString(strSection.c_str(), "usr", "sa", CONFIGPATH);
        _connConfig[i]._pass = ZIni::readString(strSection.c_str(), "passwd",  "sa", CONFIGPATH);
    }
     return true;
}

AutoConnDB OdbcPool::getCacheServer(DBType dbType)
{
    if(dbType >= DB_MAX)
    {
        throw NorException("û������B�ӣ�");
    }
    OdbcDB* curConn = NULL;
    _connLock.lock();

    if(_freeConn[dbType].size() > 0)
    {
        curConn = *_freeConn[dbType].begin();
        _freeConn[dbType].erase(_freeConn[dbType].begin());
        _usedConn[dbType].insert(curConn);
    }
    else
    {
        try{
            curConn = new OdbcDB(_henv,dbType);
            curConn->connServer(_connConfig[dbType]);
        }
        catch(std::exception& except)
        {
            if(curConn)
            {
                delete curConn;
            }
            throw except;
        }
        _usedConn[dbType].insert(curConn);
    }
    _connLock.unlock();

    return curConn;
}
void OdbcPool::freeConnServer(OdbcDB* useConn)
{
    _connLock.lock();
    set<OdbcDB*>& useConnOne = _usedConn[useConn->getType()];
    set<OdbcDB*>::iterator iterConn = useConnOne.find(useConn);
    if(iterConn != useConnOne.end())
    {
        _usedConn[useConn->getType()].erase(iterConn);
        if(_freeConn[useConn->getType()].size() < MAX_CONN)
        {
            _freeConn[useConn->getType()].insert(useConn);
        }
        else
        {
            delete useConn;
        }
    }
    _connLock.unlock();

}
}
