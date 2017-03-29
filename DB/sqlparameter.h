#ifndef SQLPARAMETER_H
#define SQLPARAMETER_H

#include <vector>
using namespace std;

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <string>
#include <sys/types.h>

struct ParaEntity
{
    int _type;
    int _len;
    union
    {
        long integer;
        double dReal;
    };
    string strData;
    long resLen;
};

/*
 *#define SQL_C_CHAR    SQL_CHAR            // CHAR, VARCHAR, DECIMAL, NUMERIC
 *#define SQL_C_LONG    SQL_INTEGER         // INTEGER
 *#define SQL_C_SHORT   SQL_SMALLINT    // SMALLINT
 *#define SQL_C_FLOAT   SQL_REAL             // REAL
 *#define SQL_C_DOUBLE  SQL_DOUBLE       // FLOAT, DOUBLE
*/
class SqlParameter
{
public:
    SqlParameter(int size);

    void setParaInt(int pos,int len,const long& data)
    {
        _vecPara[pos]._type = SQL_INTEGER;
        _vecPara[pos]._len = len;
        _vecPara[pos].integer = data;
    }
    void setParaReal(int pos,int len,const double& data)
    {
        _vecPara[pos]._type = SQL_DOUBLE;
        _vecPara[pos]._len = len;
        _vecPara[pos].dReal = data;
    }

    void setParaChar(int pos,int len,const char* data)
    {
        _vecPara[pos].strData = data;
        _vecPara[pos]._type = SQL_CHAR;
        _vecPara[pos]._len = len;
    }

    void bind(SQLHSTMT&  hstmt)
    {
        for(uint i = 0; i < _vecPara.size(); i++)
        {
            switch(_vecPara[i]._type)
            {
            case SQL_C_CHAR:
                SQLBindParameter(hstmt, i + 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR,
                                 _vecPara[i]._len, 0, (void*)_vecPara[i].strData.c_str(),
                        0, &_vecPara[i].resLen);
                break;
            case SQL_C_LONG:
                SQLBindParameter(hstmt,  i + 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,
                                 _vecPara[i]._len, 0, &_vecPara[i].integer,
                       0, &_vecPara[i].resLen);
                break;
            case SQL_C_DOUBLE:
                SQLBindParameter(hstmt,  i + 1 , SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE,
                                 _vecPara[i]._len, 0, &_vecPara[i].dReal,
                        0, &_vecPara[i].resLen);
                break;
            }

        }

    }

private:
    vector<ParaEntity> _vecPara;
};

#endif // SQLPARAMETER_H
