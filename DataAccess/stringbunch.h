#ifndef STRINGBUNCH_H
#define STRINGBUNCH_H

#include <string>
#include <vector>
using namespace std;
class StringBunch
{
public:
    StringBunch(const unsigned int& size);
    ~StringBunch();
    void setValue(const unsigned int& index,const string& strVal);

    const char** getBunchString() const;
    unsigned int _size;
    vector<unsigned int> _vecItemSize;
    char** _pBuffer;
};

#endif // STRINGBUNCH_H
