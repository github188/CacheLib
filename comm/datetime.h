#ifndef DATETIME_H
#define DATETIME_H

#include "comm/define.h"
const int SpanCarry[3] = {24,60,60};
class DateTimeSpan
{
public:
    DateTimeSpan();
    DateTimeSpan(int day,int16 hour,int16 min,int16 sec);
    bool operator>(const DateTimeSpan& dateTime)const;
    int compare(const DateTimeSpan& dateTime)const;
    DateTimeSpan operator -()    //重载负号
    {
         return DateTimeSpan(-_day,-_hour,-_min,-_sec);
    }
    string getStdString() const;

    void setDay(int day)
    {
        _day = day;
    }
    int getDay() const
    {
        return _day;
    }
    void setHour(int16 hour)
    {
        _hour = hour;
    }
    int16 getHour() const
    {
        return _hour;
    }
    void setMin(int16 min)
    {
        _min = min;
    }
    int16 getMin() const
    {
        return _min;
    }
    void setSec(int16 sec)
    {
        _sec = sec;
    }
    int16 getSec() const
    {
        return _sec;
    }
private:
    int _day;
    int16 _hour;
    int16 _min;
    int16 _sec;
};

class DateTime
{
public:
    DateTime();
    DateTime(int year,int16 mon,int16 day,int16 hour,int16 min,int16 sec);
    DateTime(const DateTime& dateTime);

    DateTime addDay(int day);
    const DateTime& addDayRep(int aDay);
    static DateTime NowDataTime();

    //YYYYMMDD_HH:%MM
   string getStringZhbr() const;

   //HH:mm:SS
   void setOnlyTime(const string& strTime);
   string getOnlyTimeString();

    //YYYY-MM-DD HH:mm:SS
    void setStdTime(const string& strTime);
    string getStdString() const;
    string getStdStringH() const;
    //YYYY_MM_DD_HH_mm_SS_KIND_Trigger
    void setTimeFromCheckID(const string& strCheckID);

    //YYYY_MM_DD_HH_mm_SS
    string getUnderLineString();
    string getUnderLineTimeString();
    //YYYY[Y]/MM[M]/DD[D]/HH(H)
    string getPathFlagString();
    //YYYY[Y]/MM[M]/DD[H]
    string getPathDateFlagString();

    //YYYYMMDDHHmmSS
    void setTime(const string& strTime);
    string getTimeString() const;


    //重载
    int compare(const DateTime& dateTime)const;
    bool operator<=(const DateTime& dateTime)const;
    bool operator<(const DateTime& dateTime) const;
    bool operator==(const DateTime& dateTime) const;
    bool operator!=(const DateTime& dateTime) const;
    bool operator>=(const DateTime& dateTime)const;
    bool operator>(const DateTime& dateTime)const;

    DateTimeSpan operator-(const DateTime& dateTime) const;
    DateTime operator-(const DateTimeSpan& dateSpan) const;
    DateTime operator+(const DateTimeSpan& dateSpan) const;

    void setYear(int year)
    {
        _year = year;
    }
    int getYear() const
    {
        return _year;
    }

    void setMon(int16 mon)
    {
        _mon = mon;
    }
    int16 getMon() const
    {
        return _mon;
    }

    void setDay(int16 day)
    {
        _day = day;
    }
    int16 getDay() const
    {
        return _day;
    }

    void setHour(int16 hour)
    {
        _hour = hour;
    }
    int16 getHour() const
    {
        return _hour;
    }
    void setMin(int16 min)
    {
        _min = min;
    }
    int16 getMin() const
    {
        return _min;
    }
    void setSec(int16 sec)
    {
        _sec = sec;
    }
    int16 getSec() const
    {
        return _sec;
    }

private:
    int getAbsDay(const DateTime& date) const;
    //大的时间减小的时间
    void substractPos(const DateTime& first,const DateTime& sec,DateTimeSpan& outSpan) const;
    void decDay(int& year,int16& mon,int16& day,int aDay);
    void addDay(int& year,int16& mon,int16& day,int aDay);
    int getMaxDayOfMon(int year,int16 mon);
    int getMaxDayOfYear(int year);
private:
    int _year;
    int16 _mon;
    int16 _day;
    int16 _hour;
    int16 _min;
    int16 _sec;
    //int _usec;
};

#endif // DATETIME_H
