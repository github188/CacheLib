#include "stringbunch.h"
#include <string.h>

StringBunch::StringBunch(const unsigned int& size):_size(size),_pBuffer(NULL)
{
    if(_size > 0)
    {
        _pBuffer = new char*[_size];
        memset(_pBuffer,0,sizeof(char*) * _size);
        _vecItemSize.resize(_size);
    }
}
StringBunch::~StringBunch()
{
    if(_pBuffer)
    {
        for(unsigned int i = 0; i < _size; i ++)
        {
            if(_pBuffer[i])
            {
                delete []_pBuffer[i];
            }
        }
        delete []_pBuffer;
    }
}

void StringBunch::setValue(const unsigned int& index,const string& strVal)
{
    if(_pBuffer && index < _size)
    {
        if(_pBuffer[index])
        {
            if(strVal.size() + 1 > _vecItemSize[index])
            {
                delete []_pBuffer[index];
                _pBuffer[index] = new char[strVal.size() + 1];
                _vecItemSize[index] = strVal.size() + 1;
            }
        }
        else
        {
            _pBuffer[index] = new char[strVal.size() + 1];
            _vecItemSize[index] = strVal.size() + 1;
        }

        strcpy(_pBuffer[index] ,strVal.c_str());
    }
}
const char** StringBunch::getBunchString() const
{
    return (const char **)_pBuffer;
}
