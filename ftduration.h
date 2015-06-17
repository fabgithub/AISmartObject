//
//  ftduration.h
//  TestAllUseful
//
//  Created by liyoudi on 15/2/28.
//  Copyright (c) 2015Äê liyoudi. All rights reserved.
//

#ifndef __TestAllUseful__ftduration__
#define __TestAllUseful__ftduration__

#include <string>
#include <stdio.h>
#ifdef WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#endif

class ftduration
{
public:
    ftduration();
    
    void start();
    
    double whenStart() const;
    double durationSecond() const;
    
    std::string StringOfStartTime() const;
private:
#ifdef WIN32
    FILETIME mftStart;
#else
    struct timeval mtStart;
#endif
};

#endif /* defined(__TestAllUseful__ftduration__) */
