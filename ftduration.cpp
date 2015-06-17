//
//  ftduration.cpp
//  TestAllUseful
//
//  Created by liyoudi on 15/2/28.
//  Copyright (c) 2015Äê liyoudi. All rights reserved.
//

#include "ftduration.h"

/***
 AsUInt64 AsUtility::GetCurrentTimeMicroSec()
 {
	assert(sizeof(AsUInt64) == 64/8);
 
 #if defined(WIN32)
	FILETIME ft;
 
	::GetSystemTimeAsFileTime(&ft);
 
	ULARGE_INTEGER li;
	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;
	return li.QuadPart/10;
 
 #else //#if defined(WIN32)
 
	AsUInt64 time=0;
	struct timeval now;
	gettimeofday( &now , NULL );
	//assert( now );
	time = now.tv_sec;
	time = time*1000000;
	time += now.tv_usec;
	return time;
 #endif
 }
 */

ftduration::ftduration()
{
    start();
}
void ftduration::start()
{
#ifdef WIN32
    ::GetSystemTimeAsFileTime(&mftStart);
#else
    gettimeofday(&mtStart, NULL);
#endif
}
double ftduration::whenStart() const
{
#ifdef WIN32
    ULARGE_INTEGER li;
    li.LowPart = mftStart.dwLowDateTime;
    li.HighPart = mftStart.dwHighDateTime;
    return li.QuadPart/(10 * 1000000.0);
#else
    return mtStart.tv_sec + mtStart.tv_usec / 1000000.0;
#endif
}
double ftduration::durationSecond() const
{
#ifdef WIN32
    FILETIME ftEnd;
    ::GetSystemTimeAsFileTime(&ftEnd);
    ULARGE_INTEGER li;
    li.LowPart = ftEnd.dwLowDateTime;
    li.HighPart = ftEnd.dwHighDateTime;
    double dDuration = li.QuadPart/(10 * 1000000.0) - whenStart();
#else
    struct timeval tEnd;
    gettimeofday(&tEnd, NULL);
    double dDuration = (tEnd.tv_sec - mtStart.tv_sec) + (tEnd.tv_usec - mtStart.tv_usec) / 1000000.0;
#endif
    return dDuration;
}
std::string ftduration::StringOfStartTime() const
{
    char szTime[120] = "";
//#ifdef WIN32
//    SYSTEMTIME sysTime;
//    GetLocalTime(&sysTime);
//    sprintf(szTime, "%4d-%02d-%02d %02d:%02d:%02d.%03d", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond , sysTime.wMilliseconds);
//#else
//    struct timeval tv;
//    struct timezone tz;
//    gettimeofday(&tv, &tz);
//    struct tm *pTm = NULL;
//    time_t tLast = tv.tv_sec - tz.tz_minuteswest * 60;
//    pTm = gmtime(&tLast);
//    sprintf(szTime, "%4d-%02d-%02d %02d:%02d:%02d.%03d", pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday, pTm->tm_hour, pTm->tm_min, pTm->tm_sec, tv.tv_usec / 1000);
//#endif // WIN32
    return szTime;
}
