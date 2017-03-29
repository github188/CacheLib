#ifndef CIMUNIT_H
#define CIMUNIT_H
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <list>

using namespace std;
const string NullChar = "NULL";
class CimUnit
{
public:
    CimUnit();
    virtual ~CimUnit();
    //�����ͷ
    virtual bool  handleTable(const char* area,const map<char*,char*> mapAttri);
    //�������}
    virtual bool handleHeader(vector<char *>& vecHead) = 0;
    //����ÿ������
    virtual bool handleData(vector<char *>& vecData) = 0;
    //�����β
    virtual bool handelDataEnd() = 0;
    //�õ������С
    virtual int getHeaderSize() = 0;
    //��ĳ�����ݶ�Ӧ������
    virtual void setHeader(unsigned int index,const string& header) = 0;
    //cimд��
    virtual void writeCim(ofstream& ofCim,string mark)  = 0;
    virtual void writeCimContent(ofstream& ofCim,string mark) = 0;
    //\t\nת��
    string makeSureDim(const string& str);
    //���ַ���ת��
    const string&  makeSureChar(const string& str)
    {
        if(str.empty())
        {
            return NullChar;
        }
        else
        {
            return str;
        }
    }
    string& makeNullChar(string& str)
    {
        if("NULL" == str)
        {
            str = "";
        }
        return str;
    }
    //�õ�ע��
    const string& getNote() const
    {
        return _note;
    }
    void setNote(const string& strNote)
    {
        _note = strNote;
    }
    //�õ���˾
    void setCompany(const string& company)
    {
        _company = company;
    }
    const string& getCompany()const
    {
        return _company;
    }
private:
    //ע��
    string _note;
    //��˾
    string _company;
};

#endif // CIMUNIT_H
