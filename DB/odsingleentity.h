#ifndef ODSINGLEENTITY_H
#define ODSINGLEENTITY_H

#include "dbentity.h"

template <typename T, template<typename T,class Alloc = allocator<T> > class C>
class ODSingleEntity : public DBEntity
{
public:
    ODSingleEntity(C<T>& singleList,int size):_singleList(singleList),_dateSize(size){}
    virtual bool bindEntity(SQLHSTMT& hstmt)
    {
        T myTmp;
        bind(hstmt,myTmp,1,_dateSize);
        _singleList.push_back(myTmp);
        return true;
    }

    void setDataSize(const int& size)
    {
        _dateSize = size;
    }

private:
    C<T>& _singleList;
    int _dateSize;
};

#endif // ODSINGLEENTITY_H
