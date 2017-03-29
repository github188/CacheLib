#ifndef AUTOSTMT_H
#define AUTOSTMT_H

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include "comm/norexception.h"

class AutoStmt
{
public:
    AutoStmt(SQLHDBC& hdbc);
    ~AutoStmt();

    SQLHSTMT& allocateStmt()
    {
        if(!_hstmt)
        {
            SQLRETURN retCode = SQLAllocHandle(SQL_HANDLE_STMT,_hdbc,&_hstmt);
             if(retCode != SQL_SUCCESS && retCode != SQL_SUCCESS_WITH_INFO)
            {
                 SQLFreeHandle(SQL_HANDLE_STMT,_hstmt);
                 _hstmt = NULL;
                throw NorException("odbc����ʵ������ʧ��");
            }
        }
         return _hstmt;
    }

private:
    AutoStmt&  operator =(SQLHSTMT& curConn);
    AutoStmt&  operator =(AutoStmt& curConn);

    SQLHDBC& _hdbc;  //���Ӿ��
    SQLHSTMT  _hstmt;
};

#endif // AUTOSTMT_H
