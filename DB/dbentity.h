#ifndef DBENTITY_H
#define DBENTITY_H


#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

#include <vector>
#include <string>
#include <list>
#include <boost/scoped_array.hpp>
using namespace std;

#include "comm/define.h"
#define ENTITY_BUF_SIZE 1024*4

class EntityString
{
public:
    EntityString(string& value,long lenInit):
        _value(value),
        _initLen(lenInit)
    {
        value.resize(lenInit);
    }

    void shrink()
    {
        _value.resize(_len);
    }
    void reset()
    {
        _value.resize(_initLen);
    }
    long& getLen()
    {
       return _len;
    }

private:
    long _len;
    string& _value;
    long _initLen;
};


class DBEntity
{
public:
    DBEntity()
    {
    }
    ~DBEntity()
    {
    }

    bool bind(SQLHSTMT& hstmt,string& data,int pos,int len)
    {
        SQLLEN resLen;
        SQLGetData(hstmt, pos, SQL_C_CHAR,_Buf,len , &resLen);
        data = _Buf;
        return true;
    }
    bool bind(SQLHSTMT& hstmt,char* data,int pos,int len)
    {
        SQLLEN resLen;
        SQLGetData(hstmt, pos, SQL_C_CHAR,data,len , &resLen);
        return true;
    }
    bool bind(SQLHSTMT& hstmt,int& data,int pos,int len)
    {
        SQLLEN resLen;
        SQLGetData(hstmt, pos, SQL_C_LONG,&data,len , &resLen);
        return true;
    }
    bool bind(SQLHSTMT& hstmt,short& data,int pos,int len)
    {
        SQLLEN resLen;
        SQLGetData(hstmt, pos, SQL_C_SHORT,&data,len , &resLen);
        return true;
    }
    bool bind(SQLHSTMT& hstmt,long& data,int pos,int len)
    {
        SQLLEN resLen;
        SQLGetData(hstmt, pos, SQL_C_LONG,&data,len , &resLen);
        return true;
    }
    bool bind(SQLHSTMT& hstmt,float& data,int pos,int len)
    {
        SQLLEN resLen;
        SQLGetData(hstmt, pos, SQL_C_FLOAT,&data,len , &resLen);
        return true;
    }
    bool bind(SQLHSTMT& hstmt,double& data,int pos,int len)
    {
        SQLLEN resLen;
        SQLGetData(hstmt, pos, SQL_C_DOUBLE,&data,len , &resLen);
        return true;
    }
    bool bind(SQLHSTMT& hstmt,byte& data,int pos,int len)
    {
        SQLLEN resLen;
        SQLGetData(hstmt, pos, SQL_C_TINYINT,&data,len , &resLen);
        return true;
    }
    #  if __WORDSIZE == 64
    #  elif __GLIBC_HAVE_LONG_LONG
        bool bind(SQLHSTMT& hstmt,int64& data,int pos,int len)
        {
            SQLLEN resLen;
            SQLGetData(hstmt, pos, SQL_C_CHAR,&data,len , &resLen);
            return true;
        }
    #  endif

    virtual  bool bindEntity(SQLHSTMT& hstmt) = 0;

    bool dataAccess(SQLHSTMT& hstmt)
    {
       SQLNumResultCols (hstmt,&_colLen);
        while(true)
        {
            if(SQL_NO_DATA == SQLFetch(hstmt))
            {
                break;
            }
            bindEntity(hstmt);
        }
        return true;
    }
    SQLSMALLINT GetColNum()
    {
        return _colLen;
    }
    const string& getSql() const
    {
        return _sql;
    }
    void setSql(const string& sql)
    {
        _sql = sql;
    }

private:
    SQLSMALLINT _colLen;
    string _sql;
    char _Buf[ENTITY_BUF_SIZE];
};

#endif // DBENTITY_H
