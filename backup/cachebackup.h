#ifndef CACHEBACKUP_H
#define CACHEBACKUP_H

#include <string>
using namespace std;

/*
class BackForm
{
public:
    enum FlagTable
    {
        TBLFLAG_LIST,
        TBLFLAG_OBJ,
        TBLFLAG_MAP,
        TBLFLAG_2MAP,
        TBLFLAG_3MAP
    };
    enum FlagKey
    {
        KETFLAG_NUMBER,
        KETFLAG_STRING
    };

    string _table;//表名
    int _type;//表类型分为list,obj,map,3map,2map
    int _typeKey;//当为map,3map,2map时key的类型,可为int,long,string
};*/

class CacheBackup
{
public:
    CacheBackup(const string& dir = "../backup/");

    bool backupTable(const string& backTable);
    bool restoreTable(const string& backTable);
    bool deleteTable(const string& backTable);

    //将表数据保存到backup文件夹中
    bool saveTable(const string& strTable,int tableFlag,const string& strContent);
    //从backup文件夹中读表数据
    bool readTable(const string& strTable,int& tableFlag,string& strContent);

    string ModifyJson(const string &strJSon);
private:
    string _dir;//保存目 录
};

#endif // CACHEBACKUP_H
