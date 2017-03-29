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

    string _table;//����
    int _type;//�����ͷ�Ϊlist,obj,map,3map,2map
    int _typeKey;//��Ϊmap,3map,2mapʱkey������,��Ϊint,long,string
};*/

class CacheBackup
{
public:
    CacheBackup(const string& dir = "../backup/");

    bool backupTable(const string& backTable);
    bool restoreTable(const string& backTable);
    bool deleteTable(const string& backTable);

    //�������ݱ��浽backup�ļ�����
    bool saveTable(const string& strTable,int tableFlag,const string& strContent);
    //��backup�ļ����ж�������
    bool readTable(const string& strTable,int& tableFlag,string& strContent);

    string ModifyJson(const string &strJSon);
private:
    string _dir;//����Ŀ ¼
};

#endif // CACHEBACKUP_H
