//
//  AISmartObject_main.cpp
//  AISmartObject
//
//  Created by liyoudi on 15/6/4.
//  Copyright (c) 2015年 liyoudi. All rights reserved.
//

#include "AISmartObject.h"
#include "ftduration.h"

#include "test_AISmartObject.h"

#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <unistd.h>
#include <list>
#include <map>

std::string _i_aio_get_statistic_graph();
void SaveAIObjectGraphToFile(const char *szFile);

class SomeCls2;
class SomeCls3;

static bool gb_show_log = true;
static bool gb_save_graph_to_file = true;

#define CONDES(cls) \
    cls() { if(gb_show_log) std::cout << __FUNCTION__ << "(" << this << ")" << std::endl; } \
    cls(const cls &c) { if(gb_show_log) std::cout << __FUNCTION__ << "(" << this << ") copy from (" << &c << ")" << std::endl; } \
    ~cls() { if(gb_show_log) std::cout << __FUNCTION__ << "(" << this << ")" << std::endl; }

class base
{
public:
    base()
    {
//        n = 22;
    }
    AISmartObject<int> n;
};

class SomeCls1 : public base
{
public:
    CONDES(SomeCls1);
public:
    AISmartObject<SomeCls1> cls1;
    AISmartObject<SomeCls2> cls2;
    AISmartObject<SomeCls3> cls3;
};

class SomeCls2 : public base
{
public:
    CONDES(SomeCls2);
public:
    AISmartObject<SomeCls1> cls1;
    AISmartObject<SomeCls2> cls2;
    AISmartObject<SomeCls3> cls3;
    SomeCls1 cls;
};

class SomeCls3 : public base
{
public:
    CONDES(SomeCls3);
public:
    AISmartObject<SomeCls1> cls1;
    AISmartObject<SomeCls2> cls2;
    AISmartObject<SomeCls3> cls3;
};

void TrySet()
{
    std::set<int> si;
    int n = 0;
    for (n = 0; n < 10; n++)
    {
        si.insert(n * 3);
    }
    std::set<int>::iterator iter;
    int sep = 9;
    iter = si.lower_bound(sep);
    std::cout << "lower_bound(" << sep << ") is at " << *iter << std::endl;
    iter = si.upper_bound(sep);
    std::cout << "upper_bound(" << sep << ") is at " << *iter << std::endl;
}

void TestPerform(int nTimes)
{
    int nIndex = 0;
    ftduration d;
    AISmartObject<SomeCls1> p;
    for(nIndex = 0;nIndex < nTimes;++nIndex)
    {
        AISmartObject<SomeCls1> psc1;
        AISmartObject<SomeCls2> psc2;
        AISmartObject<SomeCls3> psc3;
        
        //        AISmartObject<SomeCls1> psc1_1(psc1), psc1_2(psc1);
        
        psc1.Init();
        psc2.Init();
        psc3.Init();
        
        psc1->cls1 = psc1;
        psc1->cls2 = psc2;
        psc1->cls3 = psc3;
        
        psc2->cls1 = psc1;
        psc2->cls2 = psc2;
        psc2->cls3 = psc3;
        
        psc3->cls1 = psc1;
        psc3->cls2 = psc2;
        psc3->cls3 = psc3;
    }
    std::cout << __FUNCTION__ << " total time is " << d.durationSecond() << " seconds, speed is " << nTimes / d.durationSecond() << " times/s" << std::endl;
    p.Init();
    collect_garbage();
}

int main_test();

#define ShowSizeOf(t) std::cout << "sizeof(" << #t << ") is " << sizeof(t) << std::endl;

int main(int argc, const char * argv[])
{
    std::cout << "Hello, World!\n";
    std::cout << argv[0] << std::endl;
    int nTimes = 1;
    if(argc > 1)
    {
        nTimes = atoi(argv[1]);
    }
    AISmartObject<int> a(10), b;
    for(int i = 0;i < nTimes;i++)
    {
        std::cout << "=============== " << i << " of " << nTimes << " ======================" << std::endl;
        main_test();
        
        if(i % 10 == 9)
        {
//            sleep(3);
        }
    }
    std::cout << "******** sleep a moment for check memory **********" << std::endl;

    collect_garbage();
    
    SaveAIObjectGraphToFile("before_exit.dot");
    
    sleep((nTimes + 1) * 5);
    ShowSizeOf(std::list<int>);
    ShowSizeOf(std::list<SomeCls2>);
    
    std::cout << "exit" << std::endl;
    return 0;
}
int main_test()
{
    // insert code here...
    {
        AISmartObject<SomeCls1> psc1;
        psc1.Init();
        psc1->cls1 = psc1;
        SaveAIObjectGraphToFile("debug_loop_in.dot");
    }
    SaveAIObjectGraphToFile("debug_loop_out.dot");
    collect_garbage();
    SaveAIObjectGraphToFile("debug_loop_after_garbage.dot");
//    AISmartObject<int> a(1), b(2), c(3);
//    a = *b = 3;
//    c = a - b;
//    c = a * b;
//    c = a / b;
//    c = a + b;
//    c += a;
//    c -= a;
//    c *= a;
//    c /= a;
//    AISmartObject<int> o(a);
//    if(a == b)
//    {
//        std::cout << "equal" << std::endl;
//    }
    {
        AISmartObject<SomeCls1> psc1;
        AISmartObject<SomeCls2> psc2;
        AISmartObject<SomeCls3> psc3;
        
//        AISmartObject<SomeCls1> psc1_1(psc1), psc1_2(psc1);
        
        psc1.Init();
        psc2.Init();
        psc3.Init();
        
        psc1->cls1 = psc1;
        psc1->cls2 = psc2;
        psc1->cls3 = psc3;
        
        psc2->cls1 = psc1;
        psc2->cls2 = psc2;
        psc2->cls3 = psc3;
        
        psc3->cls1 = psc1;
        psc3->cls2 = psc2;
        psc3->cls3 = psc3;
        
        psc2->cls.cls1 = psc1;
        psc2->cls.cls2 = psc2;
        psc2->cls.cls3 = psc3;
//        psc2->cls.cls3.Init();
        psc1->n = psc2->cls.cls3->n = 5;
        psc2->n = 10;
        SaveAIObjectGraphToFile("graph1.dot");
        collect_garbage();
    }
    collect_garbage();
    std::string filename = "graph.dot";
    SaveAIObjectGraphToFile(filename.c_str());
    TrySet();
    gb_show_log = false;
    TestPerform(1000*1);
    
    AISmartObject<SomeCls1> s1;
    std::cout << typeid(TestPerform).name() << std::endl;
    
    test_AISmartObject();
    return 0;
}
void SaveAIObjectGraphToFile(const char *szFile)
{
    if(gb_save_graph_to_file)
    {
        std::ofstream of(szFile);
        of << _i_aio_get_statistic_graph();
    }
}
