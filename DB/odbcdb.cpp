#include "odbcdb.h"
#include "comm/zini.h"

#include <string.h>
#include <log4cpp/log4cppwraper.h>

namespace DB
{


OdbcDB::OdbcDB(SQLHENV& henv,int type):
    _henv(henv),
    _bConn(false)
{
    _type = type;
}
 OdbcDB::~OdbcDB()
{
     if(_bConn)
     {
         SQLDisconnect(_hdbc);
         _bConn = false;
     }
}

bool OdbcDB::connServer(const TagConnection& connConfig)
{
    //加载配置
    _connConfig = connConfig;
    if(!_bConn)
    {
        /*分配连接句柄*/
         SQLRETURN retcode = SQLAllocHandle(SQL_HANDLE_DBC,_henv,&_hdbc);
         if(retcode != SQL_SUCCESS &&  retcode != SQL_SUCCESS_WITH_INFO)
         {
             _bConn =  false;
             LOG_ERROR("odbc初始化连接句柄失敗");
             throw NorException("odbc初始化连接句柄失敗");
         }
        /*连接到数据源*/
        retcode = SQLConnect (_hdbc, (SQLCHAR*)_connConfig._dns.c_str(),SQL_NTS,
                              (SQLCHAR*)_connConfig._user.c_str(),SQL_NTS,  (SQLCHAR*)_connConfig._pass.c_str(),SQL_NTS);
        if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
        {
            _bConn = false;
            SQLDisconnect(_hdbc);
            LOG_ERROR("odbc连接失敗");
            throw NorException("odbc连接失敗");
            return false;
        }

        _bConn = true;
    }
    return true;
}

bool OdbcDB::batInsert(BatEntity& batEntity)
{
    SQLRETURN retcode;

    AutoStmt autoState(_hdbc);
    SQLHSTMT hstmt = autoState.allocateStmt();



    //设定SQL_ATTR_ROW_ARRAY_SIZE属性，bulk的长度
    //SQLSetStmtAttr(hstmt, SQL_ATTR_ROW_ARRAY_SIZE, (SQLPOINTER)ROW_ARRAY_SIZE, 0);

    retcode = SQLSetStmtAttr(hstmt, SQL_ATTR_ROW_BIND_TYPE, (SQLPOINTER)batEntity.getEntitySize(), 0);

    retcode = SQLSetStmtAttr(hstmt, SQL_ATTR_PARAM_BIND_TYPE, (SQLPOINTER)batEntity.getEntitySize(), SQL_IS_INTEGER);


    //设定每次参数的数量
    retcode = SQLSetStmtAttr(hstmt, SQL_ATTR_PARAMSET_SIZE, (SQLPOINTER)(long)ROW_ARRAY_SIZE, SQL_IS_INTEGER);

    SQLUSMALLINT ParamStatusArray[ROW_ARRAY_SIZE] = { 0 };
    //设定状态数组
    retcode = SQLSetStmtAttr(hstmt, SQL_ATTR_ROW_STATUS_PTR, ParamStatusArray, 0);
    SQLINTEGER nBindOffset = 0;
    retcode = SQLSetStmtAttr(hstmt, SQL_ATTR_ROW_BIND_OFFSET_PTR, (SQLPOINTER)&nBindOffset, 0);

    //retcode = SQLSetStmtAttr(hstmt, SQL_ATTR_CONCURRENCY, (SQLPOINTER)SQL_CONCUR_ROWVER, 0);

    //retcode = SQLSetStmtAttr(hstmt, SQL_ATTR_CURSOR_TYPE, (SQLPOINTER)SQL_CURSOR_DYNAMIC, 0);

    retcode = SQLSetConnectAttr(_hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, 0);
    if(batEntity.batBindEntity(hstmt))
    {
        SQLEndTran(SQL_HANDLE_DBC, _hdbc, SQL_COMMIT);
    }
    else
    {
        SQLEndTran(SQL_HANDLE_DBC, _hdbc, SQL_ROLLBACK);
    }

   retcode = SQLSetConnectAttr(_hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, 0);
    return true;
}

/*
bool OdbcDB::executeNonQueryPrepare(const char* pszSql,...)
{

    va_list ap;
    va_start(ap, pszSql);//将ap指向fmt后的第一个参数
    int value = va_arg(ap,int);//前提是我们知道第一个参数是int型；指针指向下一个参数
    //printf(“value[%d]\n”,value);
    va_end(ap);//将ap置为NULL
}*/

bool OdbcDB::executeNonQueryPrepare(const char* pszSql,SqlParameter& paraSql)
{
    if(strlen(pszSql) < 1)
    {
       return 0;
    }

    AutoStmt autoState(_hdbc);
    SQLHSTMT hstmt = autoState.allocateStmt();
    SQLRETURN retCode;
    retCode = SQLPrepare(hstmt, (SQLCHAR*)pszSql, SQL_NTS);
    if(retCode < 0)
    {
        SQLCHAR msg[128];
        SQLCHAR state[128];
        SQLINTEGER error_id;
        SQLSMALLINT text;
        SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, state, &error_id, msg, 128, &text);
        ostringstream errorFormator;
        errorFormator << "SQLPrepare failed,state:" << state << ",msg:" <<  msg;
        //throw CalcException(errorFormator.str());
        printf("%s\n",errorFormator.str().c_str());
        return false;
    }

    paraSql.bind(hstmt);

    retCode = SQLExecute(hstmt);
    if (retCode < 0)
    {
        SQLCHAR msg[128];
        SQLCHAR state[128];
        SQLINTEGER error_id;
        SQLSMALLINT text;
        SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, state, &error_id, msg, 128, &text);
        printf("db Insert fail, sqlstate=%s, errormsg=%s\n", state, msg);

        return false;
    }
    else if (retCode == SQL_SUCCESS_WITH_INFO)
    {
        SQLCHAR msg[128];
        SQLCHAR state[128];
        SQLINTEGER error_id;
        SQLSMALLINT text;
        SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, state, &error_id, msg, 128, &text);
        printf("warning msg=%s\n", msg);
    }
    return true;
}

bool OdbcDB::executeNonQuery(const string& pszSql)
{
    if(pszSql.empty())
    {
       return 0;
    }

    AutoStmt autoState(_hdbc);
    SQLHSTMT hstmt = autoState.allocateStmt();

    SQLRETURN retCode;

    //cout<<"hStmt="<<hStmt<<endl;
    retCode=SQLExecDirect(hstmt,(SQLCHAR*)pszSql.c_str(),SQL_NTS);
    if((retCode != SQL_SUCCESS) && (retCode != SQL_SUCCESS_WITH_INFO))
    {
       //throw CalcException("odbc非查询操作失敗");
       return false;
    }
    return true;
}
}
