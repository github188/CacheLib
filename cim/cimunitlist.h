#ifndef CIMUNITLIST_H
#define CIMUNITLIST_H
#include "cim/cimunit.h"
#include <algorithm>
#include <assert.h>

//mrid���ֶ��ձ�
struct Correspondence
{
    long mRID;
    string ID;
    string name;
};
//����onetime��mrid�ַ���ת��ȫ�ֱ���
//TG 2015-11-20
class MRIDConvert
{
 public:
    static int g_iVersionFlag;
    static map<string, long> g_Mridmap;
    static long g_iMRIDIndex;
    static long g_ConvertMrid(const char* ID);
    static map<string, Correspondence> g_mapCorrespondence;
};
//�������ֶ�Ӧ
struct ParaRefect
{
    enum
    {
        PARA_DATA,
        PARA_DEFAULT
    };
    ParaRefect() :
        _flag(PARA_DATA)
    {

    }

    string _header;
    int _pos;
    int _flag;
    string _default;
};
//��Сдת��
class InsensitiveCmp
{
public:
    InsensitiveCmp(string other);
    bool operator()(const char* first);
    bool operator()(const string& first);
private:
    string _cmp;
};
//�մ�
const char  myEmpty[] = "";
class CimUnitList : public CimUnit
{
public:
    CimUnitList()
    {}
    virtual ~CimUnitList(){}
    //�������}
    virtual bool handleHeader(vector<char *>& vecHead);
    //����ÿ������
    virtual bool handleData(vector<char *>& vecData);
    //������β
    virtual bool handelDataEnd();
    //�õ������С
    virtual int getHeaderSize()
    {
        return _vecHeader.size();
    }
    //�õ����}���ݶ��ձ�
    const vector<ParaRefect>& getHeader() const
    {
        return _vecHeader;
    }
    //��ʼ�����ձ��С
    void initHeader(int size)
    {
        _vecHeader.resize(size);
        for(int i = 0; i < size; i ++)
        {
            _vecHeader[i]._pos = i;
        }
    }
    //�������ݶ�Ӧ�ı���,��λ��
    void setHeader(int index,const string& header,int pos);
    virtual void setHeader(unsigned int index,const string& header);

    //���ݱ��������õ�����
    const char* getData(const vector<char*>& vecData,int index);
    //�鿴����������������
    bool isHasCol(int index);
//dcb 2015-12-18
    //�մ�ת����
    long nullToNegOne(const char* xNode);
    //дcim
    virtual void writeCim(ofstream& ofCim,string mark);
    virtual void writeCimContent(ofstream& ofCim,string mark);
protected:
    //�������
    vector<ParaRefect> _vecHeader;
};
#endif // CIMUNITLIST_H
