#ifndef BATENTITY_H
#define BATENTITY_H

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#define ROW_ARRAY_SIZE 1000

namespace DB
{
class BatEntity
{
public:
    BatEntity();
    virtual bool batBindEntity(SQLHSTMT& hstmt) = 0;
    virtual int getEntitySize() = 0;

};
}

#endif // BATENTITY_H
