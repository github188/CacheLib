#include "datetime.h"
#include <time.h>
#include <boost/algorithm/string.hpp>
#include <vector>


DateTimeSpan::DateTimeSpan()
{
    _day = 0;
    _hour = 0;
    _min = 0;
    _sec = 0;
}
DateTimeSpan::DateTimeSpan(int day,int16 hour,int16 min,int16 sec)
{
    _day = day;
    _hour = hour;
    _min = min;
    _sec = sec;
}

string DateTimeSpan::getStdString() const
{
    char datePath[64];
    sprintf(datePath,"%02d %02d:%02d:%02d",
            _day,_hour,_min,_sec);
    return datePath;
}

bool DateTimeSpan::operator>(const DateTimeSpan& dateTime)const
{
    int cmp = compare(dateTime);
    switch(cmp)
    {
    case -1:
        return true;
    default:
        return false;
    }
}
int DateTimeSpan::compare(const DateTimeSpan& dateTime)const
{
    if(this->_day <  dateTime._day)
    {
        return 1;
    }
    else if(this->_day  > dateTime._day)
    {
        return -1;
    }
    else
    {
        if(this->_hour <  dateTime._hour)
        {
            return 1;
        }
        else if(this->_hour  > dateTime._hour)
        {
            return -1;
        }
        else
        {
            if(this->_min <  dateTime._min)
            {
                return 1;
            }
            else if(this->_min  > dateTime._min)
            {
                return -1;
            }
            else
            {
                if(this->_sec <  dateTime._sec)
                {
                    return 1;
                }
                else if(this->_sec  > dateTime._sec)
                {
                    return -1;
                }
                else
                {
                    return 0;
                }
            }
        }
    }
}
DateTime::DateTime()
{
    time_t tt = time(NULL);//这句返回的只是一个时间cuo
    tm* t= localtime(&tt);
    _year = t->tm_year + 1900;
    _mon = t->tm_mon + 1;
    _day = t->tm_mday;
    _hour = t->tm_hour;
    _min = t->tm_min;
    _sec = t->tm_sec;
}
DateTime::DateTime(int year,int16 mon,int16 day,int16 hour,int16 min,int16 sec)
{
    _year = year;
    _mon = mon;
    _day = day;
    _hour = hour;
    _min = min;
    _sec = sec;
}

DateTime::DateTime(const DateTime& dateTime)
{
    _year = dateTime._year;
    _mon = dateTime._mon;
    _day = dateTime._day;
    _hour = dateTime._hour;
    _min = dateTime._min;
    _sec = dateTime._sec;
}

void DateTime::decDay(int& year,int16& mon,int16& day,int aDay)
{
    int curDay = day + aDay;
    if(curDay <= 0)
    {
        mon -= 1;
        if(0 == mon)
        {
            mon = 12;
            year -= 1;
        }
        day = getMaxDayOfMon(year,mon);
        decDay(year,mon,day,curDay);
    }
    else
    {
        day += aDay;
    }
}

void DateTime::addDay(int& year,int16& mon,int16& day,int aDay)
{
    int dayOfMon = getMaxDayOfMon(year,mon);
    int daySum = day + aDay;
    if(daySum > dayOfMon)
    {
        mon += 1;
        day = 0;
        if(mon > 12)
        {
            mon = 1;
            year += 1;
        }
        addDay(year,mon,day,daySum - dayOfMon);
    }
    else
    {
        day += aDay;
    }
}
const DateTime& DateTime::addDayRep(int aDay)
{
    if(aDay > 0)
    {
        addDay(_year,_mon,_day,aDay);
    }
    else if(aDay < 0)
    {
        decDay(_year,_mon,_day,aDay);
    }
    return *this;
}

DateTime DateTime::addDay(int aDay)
{
    /*ulong64 tmpTime2;
    ulong64  tmpTime = _curDateTime;
    tmpTime2 =  tmpTime / 100;
    int min = tmpTime - tmpTime2 * 100;
    tmpTime = tmpTime2;
    tmpTime2 =  tmpTime / 100;
    int hour = tmpTime - tmpTime2 * 100;
    tmpTime = tmpTime2;
    tmpTime2 = tmpTime / 100;
    int day = tmpTime - tmpTime2 * 100;
    tmpTime = tmpTime2;
    tmpTime2 = tmpTime / 100;
    int mon = tmpTime - tmpTime2 * 100;
    tmpTime = tmpTime2;
    tmpTime2 = tmpTime / 10000;
    int year = tmpTime - tmpTime2 * 10000;*/
    int year = _year;
    int16 mon = _mon;
    int16 day = _day;
    int16 hour = _hour;
    int16 min = _min;
    int16 sec = _sec;
    if(aDay > 0)
    {
        addDay(year,mon,day,aDay);
    }
    else
    {
        decDay(year,mon,day,aDay);
    }
    return DateTime(year,mon,day,hour,min,sec);
}
//YYYY_MM_DD_HH_mm_SS_KIND_Trigger
void DateTime::setTimeFromCheckID(const string& strCheckID)
{
    string strTime = strCheckID;
    //2016_02_20_11_25_13
    if(strTime.length()>=19)
    {

        _year = atoi(strTime.substr(0, 4).c_str());
        _mon = atoi(strTime.substr(5, 2).c_str());
        _day = atoi(strTime.substr(8, 2).c_str());
        _hour = atoi(strTime.substr(11, 2).c_str());
        _min = atoi(strTime.substr(14, 2).c_str());
        _sec = atoi(strTime.substr(17, 2).c_str());
    }
    //    boost::trim(strTime);
    //    vector<string> vecTime;
    //    boost::split(vecTime,strTime,boost::is_any_of("_"));
    //    if(vecTime.size() >= 6)
    //    {
    //        _year = atoi(vecTime[0].c_str());
    //        _mon = atoi(vecTime[1].c_str());
    //        _day = atoi(vecTime[2].c_str());
    //        _hour = atoi(vecTime[3].c_str());
    //        _min = atoi(vecTime[4].c_str());
    //        _sec = atoi(vecTime[5].c_str());
    //    }
}

//HH:mm:SS
void DateTime::setOnlyTime(const string& sTime)
{
    string strTime = sTime;
    boost::trim(strTime);
    vector<string> vecTime;
    boost::split(vecTime,strTime,boost::is_any_of(":"));
    _year = 0;
    _mon = 0;
    _day = 0;
    _hour = atoi(vecTime[0].c_str());
    _min = atoi(vecTime[1].c_str());
    _sec= atoi(vecTime[2].c_str());
}
string DateTime::getOnlyTimeString()
{
    char datePath[32];
    sprintf(datePath,"%02d:%02d:%02d",
            _hour,_min,_sec);
    return datePath;
}

void DateTime::setStdTime(const string& sTime)
{
    string strTime = sTime;
    boost::trim(strTime);
    vector<string> vecTime;
    boost::split(vecTime,strTime,boost::is_any_of(" -:"));
    _sec = 0;
    switch(vecTime.size())
    {
    case 6:
        _sec = atoi(vecTime[5].c_str());
    case 5:
        _year = atoi(vecTime[0].c_str());
        _mon = atoi(vecTime[1].c_str());
        _day = atoi(vecTime[2].c_str());
        _hour = atoi(vecTime[3].c_str());
        _min = atoi(vecTime[4].c_str());
        break;
    }

}
//YYYYMMDDHHmmSS
void DateTime::setTime(const string& strTime)
{
    _sec = 0;
    switch(strTime.size())
    {
    case 14:
        _sec = atoi(strTime.substr(12,2).c_str());
    case 12:
        _year = atoi(strTime.substr(0,4).c_str());
        _mon = atoi(strTime.substr(4,2).c_str());
        _day = atoi(strTime.substr(6,2).c_str());
        _hour = atoi(strTime.substr(8,2).c_str());
        _min = atoi(strTime.substr(10,2).c_str());
        break;
    }
}
string DateTime::getTimeString() const
{
    char datePath[64];
    sprintf(datePath,"%04d%02d%02d%02d%02d%02d",
            _year,_mon,_day,_hour,_min,_sec);
    return datePath;
}

DateTime DateTime::NowDataTime()
{
    DateTime tmpDateTime;
    return tmpDateTime;
}
string DateTime::getStdStringH() const
{
    char datePath[64];
    sprintf(datePath,"%04d-%02d-%02d %02d:%02d",
            _year,_mon,_day,_hour,_min);
    return datePath;
}
string DateTime::getStdString() const
{
    char datePath[64];
    sprintf(datePath,"%04d-%02d-%02d %02d:%02d:%02d",
            _year,_mon,_day,_hour,_min,_sec);
    return datePath;
}

//YYYY_MM_DD_HH_mm_SS
string DateTime::getUnderLineString()
{
    char datePath[64];
    sprintf(datePath,"%04d_%02d_%02d_%02d_%02d_%02d",
            _year,_mon,_day,_hour,_min,_sec);
    return datePath;
}
//HH_mm_SS
string DateTime::getUnderLineTimeString()
{
    char datePath[64];
    sprintf(datePath,"%02d_%02d_%02d",
            _hour,_min,_sec);
    return datePath;
}
//YYYY[Y]/MM[M]/DD[H]
string DateTime::getPathDateFlagString()
{
    char datePath[64];
    sprintf(datePath,"%04dY/%02dM/%02dD",
            _year,_mon,_day);
    return datePath;
}
//YYYY[Y]/MM[M]/DD[D]/HH(H)
string DateTime::getPathFlagString()
{
    char datePath[64];
    sprintf(datePath,"%04dY/%02dM/%02dD/%02dH",
            _year,_mon,_day,_hour);
    return datePath;
}

string DateTime::getStringZhbr() const
{
    char datePath[64];
    sprintf(datePath,"%04d%02d%02d_%02d:%02d",
            _year,_mon,_day,_hour,_min);
    return datePath;
}

//重载
bool DateTime::operator<=(const DateTime& dateTime)const
{
    int cmp = compare(dateTime);
    switch(cmp)
    {
    case 1:
    case 0:
        return true;
    default:
        return false;
    }
}
bool DateTime::operator!=(const DateTime& dateTime) const
{
    int cmp = compare(dateTime);
    switch(cmp)
    {
    case 0:
        return false;
    default:
        return true;
    }
}
bool DateTime::operator==(const DateTime& dateTime)const
{
    int cmp = compare(dateTime);
    switch(cmp)
    {
    case 0:
        return true;
    default:
        return false;
    }
}

bool DateTime::operator<(const DateTime& dateTime)const
{
    int cmp = compare(dateTime);
    switch(cmp)
    {
    case 1:
        return true;
    default:
        return false;
    }
}
bool DateTime::operator>=(const DateTime& dateTime)const
{
    int cmp = compare(dateTime);
    switch(cmp)
    {
    case -1:
    case 0:
        return true;
    default:
        return false;
    }
}

bool DateTime::operator>(const DateTime& dateTime)const
{
    int cmp = compare(dateTime);
    switch(cmp)
    {
    case -1:
        return true;
    default:
        return false;
    }
}

int DateTime::compare(const DateTime& dateTime)const
{
    if(this->_year < dateTime._year)
    {
        return 1;
    }
    else if(this->_year > dateTime._year)
    {
        return -1;
    }
    else
    {
        if(this->_mon < dateTime._mon)
        {
            return 1;
        }
        else if(this->_mon > dateTime._mon)
        {
            return -1;
        }
        else
        {
            if(this->_day < dateTime._day)
            {
                return 1;
            }
            else if(this->_day > dateTime._day)
            {
                return -1;
            }
            else
            {
                if(this->_hour < dateTime._hour)
                {
                    return 1;
                }
                else if(this->_hour > dateTime._hour)
                {
                    return -1;
                }
                else
                {
                    if(this->_min < dateTime._min)
                    {
                        return 1;
                    }
                    else if(this->_min > dateTime._min)
                    {
                        return -1;
                    }
                    else
                    {
                        if(this->_sec < dateTime._sec)
                        {
                            return 1;
                        }
                        else if(this->_sec > dateTime._sec)
                        {
                            return -1;
                        }
                        else
                        {
                            return 0;
                        }
                    }
                }
            }
        }
    }
}

//大的时间减小的时间
void DateTime::substractPos(const DateTime& first,const DateTime& sec,DateTimeSpan& outSpan) const
{
    int dayFirst = getAbsDay(first);
    int daySec = getAbsDay(sec);
    uint spanFirst[4] = {dayFirst,first.getHour(),first.getMin(),first.getSec()};
    uint spanSec[4] = {daySec,sec.getHour(),sec.getMin(),sec.getSec()};
    for(int i = 3; i >= 0; --i)
    {
        if(spanFirst[i] < spanSec[i])
        {
            //进行借位
            for(int j = i - 1; j >= 0; --j)
            {
                if(spanFirst[j] < 1)
                {
                    spanFirst[j] = SpanCarry[j - 1] - 1;
                }
                else
                {
                    spanFirst[j] --;
                    break;
                }
            }
            spanFirst[i]  = spanFirst[i] + SpanCarry[i - 1] - spanSec[i];
        }
        else
        {
            spanFirst[i] -= spanSec[i];
        }
    }
    outSpan = DateTimeSpan(spanFirst[0],spanFirst[1],spanFirst[2],spanFirst[3]);
}
int DateTime::getAbsDay(const DateTime& date) const
{
    int i;
    int month_day[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    int year = date._year - 1;  // 因为欲求距离1年1月1日的距离
    int days = year * 365 + year/4 - year/100 + year/400;  //求得之前闰年的数量并在天数上进行想加
    if(date._year %4==0 && date._year %100!=0 || date._year %400==0)
    {
        month_day[1]++; //当前年为闰年，二月加 1
    }
    for(i=0; i<date._mon-1; i++)
    {
        days += month_day[i];
    }
    days += date._day - 1;  //今天应该是不算如天数计数
    return days;
}

DateTimeSpan DateTime::operator-(const DateTime& dateTime) const
{
    int cmp = compare(dateTime);
    DateTimeSpan timeSpan;
    switch(cmp)
    {
    case -1:
        substractPos(*this,dateTime,timeSpan);
        break;
    case 1:
        substractPos(dateTime,*this,timeSpan);
        timeSpan = -timeSpan;
        break;
    default:
        return DateTimeSpan(0,0,0,0);
    }
    return timeSpan;
}
DateTime DateTime::operator-(const DateTimeSpan& dateSpan) const
{
    int dateArray[4] = {0,this->getHour(),this->getMin(),this->getSec()};
    int dataASub[4] = {dateSpan.getDay(),dateSpan.getHour(),dateSpan.getMin(),dateSpan.getSec()};
    for(int i = 3;i > 0; --i)
    {
        if(dateArray[i] > dataASub[i])
        {
            dateArray[i] -= dataASub[i];
        }
        else
        {
            dataASub[i - 1] += 1;
            dateArray[i] += 60 - dataASub[i];
        }
    }
    dateArray[0] -= dataASub[0];
    DateTime tmpDate(this->getYear(),this->getMon(),this->getDay(),dateArray[1],dateArray[2],dateArray[3]);
    tmpDate.addDayRep(dateArray[0]);
    return tmpDate;
}
DateTime DateTime::operator+(const DateTimeSpan& dateSpan) const
{
    int dateArray[4] = {0,this->getHour(),this->getMin(),this->getSec()};
    int dataASub[4] = {dateSpan.getDay(),dateSpan.getHour(),dateSpan.getMin(),dateSpan.getSec()};
    int sub;
    for(int i = 3;i > 0; --i)
    {
        sub = dateArray[i] + dataASub[i];
        if(sub < 60)
        {
            dateArray[i] = sub;
        }
        else
        {
            dataASub[i - 1] += 1;
            dateArray[i] = sub - 60;
        }
    }
    dateArray[0] += dataASub[0];
    DateTime tmpDate(this->getYear(),this->getMon(),this->getDay(),dateArray[1],dateArray[2],dateArray[3]);
    tmpDate.addDayRep(dateArray[0]);
    return tmpDate;
}

int DateTime::getMaxDayOfMon(int year,int16 mon)
{
    switch(mon)
    {
    case 4:
    case 6:
    case 9:
    case 11:
        return 30;
    case 2:
        if((year % 4 == 0 && year % 100 != 0) || year % 400 == 0 )
        {
            return 29;
        }
        else
        {
            return 28;
        }
    default:
        return 31;
    }
}

int DateTime::getMaxDayOfYear(int year)
{
    if(year % 100)
    {
        if(0 == (year % 4))
        {
            return 366;
        }
        else
        {
            return 365;
        }
    }
    else
    {
        if(0 == (year % 400))
        {
            return 366;
        }
        else
        {
            return 365;
        }
    }
}
