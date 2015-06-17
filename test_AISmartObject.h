//
//  test_AISmartObject.h
//  TrySomething
//
//  Created by liyoudi on 15/6/5.
//  Copyright (c) 2015年 liyoudi. All rights reserved.
//

#ifndef __TrySomething__test_AISmartObject__
#define __TrySomething__test_AISmartObject__

#include <stdio.h>

void test_AISmartObject();
void collect_garbage_with_line(const char *szFile, int nLine);

#define collect_garbage() collect_garbage_with_line(__FILE__, __LINE__)

#endif /* defined(__TrySomething__test_AISmartObject__) */
