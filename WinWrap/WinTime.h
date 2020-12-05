#ifndef __WIN_TIME_WRAP_HEADER__

#define __WIN_TIME_WRAP_HEADER__

#include <windows.h>

//////////////////////////////////////////////////////////////////////////

namespace WinWrap{

struct WinTime : public _SYSTEMTIME{

    typedef const char* cstr;
    typedef unsigned short time_t;
    typedef unsigned long long time_ex;

    enum default_time{ SYSTEM, LOCAL, NONE };

    static bool isLeapYear( time_t );

    time_t getCurrentYear() const;
    time_t getCurrentMonth() const;
    time_t getCurrentWeekday() const;
    time_t getCurrentDay() const;
    time_t getCurrentHour() const;
    time_t getCurrentMinute() const;
    time_t getCurrentSecond() const;
    time_t getCurrentMillisecond() const;

    cstr getCurrentMonthName() const;
    cstr getCurrentDayName() const;

    time_ex convertYears() const;
    time_ex convertMonths() const;
    time_ex convertDays() const;
    time_ex convertHours() const;
    time_ex convertMinutes() const;
    time_ex convertSeconds() const;
    time_ex convertMilliseconds() const;

    WinTime( default_time = SYSTEM );
    WinTime( const _FILETIME& );

    void refresh( default_time = SYSTEM );

    _FILETIME toFileTime();

	static WinTime get( default_time = SYSTEM );

private:

    static cstr M_NAMES[];
    static cstr W_NAMES[];
    static const time_t D_M[];
    static const time_t DM_SUM[];
};

};

#ifndef __WIN_TIME_WRAP_IMPLEMENTATION__
#include "WinTime.cpp"
#endif

#endif
