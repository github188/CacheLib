#ifndef CACHEPULISH_H
#define CACHEPULISH_H

#include <string>
using namespace std;
namespace DataCache
{
class CachePulish
{
public:
    CachePulish();
    bool publish(const string& channel,const string& msg);
};
}

#endif // CACHEPULISH_H
