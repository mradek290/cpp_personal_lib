#ifndef __WIN_TIME_WRAP_IMPLEMENTATION__
#define __WIN_TIME_WRAP_IMPLEMENTATION__

#ifndef __WIN_TIME_WRAP_HEADER__
#include "WinTime.h"
#endif

namespace WinWrap{

void WinTime::refresh( WinTime::default_time dt ){

    switch(dt){
        case SYSTEM :
            GetSystemTime(this);
            return;
        case LOCAL :
            GetLocalTime(this);
    }
}

WinTime WinTime::get( WinTime::default_time dt ){

	return WinTime(dt);
}

WinTime::WinTime( WinTime::default_time dt ){

    switch(dt){
        case SYSTEM :
            GetSystemTime(this);
            return;
        case LOCAL :
            GetLocalTime(this);
    }
}

WinTime::WinTime( const _FILETIME& ft ){

    FileTimeToSystemTime(&ft,this);
}

_FILETIME WinTime::toFileTime(){

    _FILETIME retval;
    SystemTimeToFileTime(this,&retval);
    return retval;
}

WinTime::time_ex WinTime::convertMilliseconds() const {

    return convertSeconds()*1000 + wMilliseconds;
}

WinTime::time_ex WinTime::convertSeconds() const {

    return convertMinutes()*60 + wSecond;
}

WinTime::time_ex WinTime::convertMinutes() const {

    return convertHours()*60 + wMinute;
}

WinTime::time_ex WinTime::convertHours() const {

    return 24*convertDays() + wHour;
}

WinTime::time_ex WinTime::convertDays() const {

    time_ex retval = time_ex( 365.25*wYear ) + DM_SUM[wMonth] + wDay;
    return ( isLeapYear(wYear) && wMonth > 2 ) ? retval+1 : retval;
}

WinTime::time_ex WinTime::convertMonths() const {

    return 12*wYear + wMonth;
}

WinTime::time_ex WinTime::convertYears() const {

    return wYear;
}

WinTime::time_t WinTime::getCurrentYear()        const { return wYear; }
WinTime::time_t WinTime::getCurrentMonth()       const { return wMonth; }
WinTime::time_t WinTime::getCurrentWeekday()     const { return wDayOfWeek; }
WinTime::time_t WinTime::getCurrentDay()         const { return wDay; }
WinTime::time_t WinTime::getCurrentHour()        const { return wHour; }
WinTime::time_t WinTime::getCurrentMinute()      const { return wMinute; }
WinTime::time_t WinTime::getCurrentSecond()      const { return wSecond; }
WinTime::time_t WinTime::getCurrentMillisecond() const { return wMilliseconds; }

const char* WinTime::getCurrentDayName()   const { return W_NAMES[wDayOfWeek]; }
const char* WinTime::getCurrentMonthName() const { return M_NAMES[wMonth]; }

bool WinTime::isLeapYear( WinTime::time_t t ){

    if( t % 4 != 0 ) return false;
    else if( t % 100 != 0 ) return true;
    else if( t % 400 != 0 ) return false;
    else return true;

    return false;
}

const WinTime::time_t WinTime::D_M[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

const WinTime::time_t WinTime::DM_SUM[] = { 0, D_M[1],
                                            static_cast<WinTime::time_t>(DM_SUM[1]+D_M[2]),
                                            static_cast<WinTime::time_t>(DM_SUM[2]+D_M[3]),
                                            static_cast<WinTime::time_t>(DM_SUM[3]+D_M[4]),
                                            static_cast<WinTime::time_t>(DM_SUM[4]+D_M[5]),
                                            static_cast<WinTime::time_t>(DM_SUM[5]+D_M[6]),
                                            static_cast<WinTime::time_t>(DM_SUM[6]+D_M[7]),
                                            static_cast<WinTime::time_t>(DM_SUM[7]+D_M[8]),
                                            static_cast<WinTime::time_t>(DM_SUM[8]+D_M[9]),
                                            static_cast<WinTime::time_t>(DM_SUM[9]+D_M[10]),
                                            static_cast<WinTime::time_t>(DM_SUM[10]+D_M[11]),
                                            static_cast<WinTime::time_t>(DM_SUM[11]+D_M[12]) };

const char* WinTime::M_NAMES[] = {
                                    "INVALID",
                                    "JAN",
                                    "FEB",
                                    "MAR",
                                    "APR",
                                    "MAY",
                                    "JUN",
                                    "JUL",
                                    "AUG",
                                    "SEP",
                                    "OCT",
                                    "NOV",
                                    "DEC" };

const char* WinTime::W_NAMES[] = {
                                    "SUN",
                                    "MON",
                                    "TUE",
                                    "WED",
                                    "THR",
                                    "FRI",
                                    "SAT" };

};

#endif
