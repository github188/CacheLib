#ifndef NORMALCIMUNIT_H
#define NORMALCIMUNIT_H
#include "cim/cimunitlist.h"
#include <list>
class NormalCimUnit : public CimUnitList
{
public:
    enum
    {
        FLAG_HEADER,//�������ı�ͷ��ȡ
        FLAG_ALL//��ȡ���б�ͷ�Ͷ�Ӧ����
    };
    NormalCimUnit();
    NormalCimUnit(const vector<string>& header);

    //�������
    virtual bool  handleHeader(vector<char *>& vecHead);
    //��������
    virtual bool handleData(vector<char *>& vecData);
    //дcim
    virtual void writeCimContent(ofstream& ofCim,string  mark);
    //������
    void clear()
    {
        _norData.clear();
    }
    //����}
    void setHeader(const vector<string>& header);
    //�õ�����
    list<vector<string> >&  getNorData()
    {
        return _norData;
    }
    bool IsEmpty()
    {
        return _norData.empty();
    }

    //�õ�����Ծ�����
    int getHeaderIndex(const string& headerName) const;

private:
    //����
    list<vector<string> > _norData;
    //��ͷ
    vector<string> _header;
    //��ʶ
    int _flag;
    //һ����ʱ����
    vector<string> _tmpData;

};

#endif // NORMALCIMUNIT_H
