#include "autostmt.h"
#include <assert.h>

AutoStmt::AutoStmt(SQLHDBC& hdbc) : _hdbc(hdbc)  //���캯��
{
    _hstmt = NULL;
}

AutoStmt::~AutoStmt()        //��������
{
    if(_hstmt)
    {
        SQLFreeHandle(SQL_HANDLE_STMT,_hstmt);
    }
}
AutoStmt& AutoStmt::operator =(SQLHSTMT& curConn)
{
    assert(false);
}
AutoStmt& AutoStmt::operator =(AutoStmt& curConn)
{
    assert(false);
}
