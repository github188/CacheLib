#ifndef ODBCDB_H
#define ODBCDB_H

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

#include <sstream>
#include <stdio.h>
#include <string.h>

#include "comm/norexception.h"
#include "DB/batentity.h"
#include "DB/odsingleentity.h"
#include "autostmt.h"
#include <stdarg.h>
#include "sqlparameter.h"

//默諗連接
struct TagConnection
{
  string _dns;
  string _user;
  string _pass;
};

namespace DB
{
class OdbcDB
{
public:

    OdbcDB(SQLHENV& henv,int type);
    ~OdbcDB();
    bool connServer(const TagConnection& connConfig);
public:

    bool batInsert(BatEntity& batEntity);


    //只用于基本类型
    template <typename T, template<typename T,class Alloc = allocator<T> > class C>
    bool executeQuerySingle(const string& sql,C<T>& singleList,const int& datSize)
    {
        ODSingleEntity<T,C> singleEntity(singleList,datSize);
        AutoStmt autoState(_hdbc);
        SQLHSTMT hstmt = autoState.allocateStmt();

        //cout<<"hStmt="<<hStmt<<endl;
         SQLRETURN retCode=SQLExecDirect(hstmt,(SQLCHAR*)sql.c_str(),SQL_NTS);
        if((retCode != SQL_SUCCESS) && (retCode != SQL_SUCCESS_WITH_INFO))
        {
            SQLCHAR msg[128];
            SQLCHAR state[128];
            SQLINTEGER error_id;
            SQLSMALLINT text;
            SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, state, &error_id, msg, 128, &text);
            ostringstream errorFormator;
            errorFormator << "odbc查询操作失敗,state:" << state << ",msg:" <<  msg;
           throw NorException(errorFormator.str().c_str());
           return false;
        }

        singleEntity.dataAccess(hstmt);
        return false;
    }

    template<typename ENTITY>
    bool executeQuery(const string& pszWhere,ENTITY& entity)
    {
        const string strSql = entity.getSql() + pszWhere;
        if(strSql.empty())
        {
           return false;
        }

        AutoStmt autoState(_hdbc);
        SQLHSTMT hstmt = autoState.allocateStmt();

        //cout<<"hStmt="<<hStmt<<endl;
         SQLRETURN retCode=SQLExecDirect(hstmt,(SQLCHAR*)strSql.c_str(),SQL_NTS);
        if((retCode != SQL_SUCCESS) && (retCode != SQL_SUCCESS_WITH_INFO))
        {
            SQLCHAR msg[128];
            SQLCHAR state[128];
            SQLINTEGER error_id;
            SQLSMALLINT text;
            SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, state, &error_id, msg, 128, &text);
            ostringstream errorFormator;
            errorFormator << "odbc查询操作失敗,state:" << state << ",msg:" <<  msg;
           throw NorException(errorFormator.str().c_str());
           return false;
        }

        entity.dataAccess(hstmt);
        /*entity.bindEntity(_hstmt);
        while ( (retCode = SQLFetch(_hstmt) ) != SQL_NO_DATA)
        {
            entity.oneComplete();
        }*/
        return true;
    }

    /*template<typename ENTITY>
    bool executeQueryPrepare(const char* pszSql,SqlParameter& paraSql,ENTITY& entity)
    {
        if(strlen(pszSql) < 1)
        {
           return 0;
        }

        AutoStmt autoState(_hdbc);
        SQLHSTMT hstmt = autoState.allocateStmt();

        paraSql.bind(hstmt);
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

        entity.dataAccess(hstmt);
        return true;
    }*/
    bool executeNonQuery(const string& pszSql);
    bool executeNonQueryPrepare(const char* pszSql,SqlParameter& paraSql);

    const int getType()
    {
        return _type;
    }

private:
    SQLHENV& _henv;  //环境句柄
    SQLHDBC _hdbc;  //连接句柄
    //SQLHSTMT _hstmt;  //语句句柄
    bool _bConn;
    TagConnection _connConfig;
    int _type;
};
}

#endif // ODBCDB_H
