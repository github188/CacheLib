#include "onlyonecimunit.h"

OnlyOneCimUnit::OnlyOneCimUnit()
{
    _index = 0;
}

void OnlyOneCimUnit::writeCim(ofstream& ofCim,string mark)
{
    ofCim << "@one\n";
    writeCimContent(ofCim,mark);
}
void OnlyOneCimUnit::writeCimContent(ofstream& ofCim,string mark)
{
    for(list<string>::iterator iterData =  _vecUsedEqui.begin();
        iterData != _vecUsedEqui.end(); ++iterData)
    {
        ofCim << "#" << *iterData << "\n";
    }
}
