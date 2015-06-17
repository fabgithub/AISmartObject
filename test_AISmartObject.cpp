//
//  test_AISmartObject.cpp
//  TrySomething
//
//  Created by liyoudi on 15/6/5.
//  Copyright (c) 2015年 liyoudi. All rights reserved.
//

#include "AISmartObject.h"

#include "test_AISmartObject.h"
#include "ftduration.h"

#include <unistd.h>

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <set>
#include <list>

void SaveAIObjectGraphToFile(const char *szFile);

std::string _i_aio_get_statistic_graph();
void collect_garbage_with_line(const char *szFile = NULL, int nLine = -1);

static int gnItemCount = 0;
static bool gb_show_log = false;
#define ShowFn() if(gb_show_log) std::cout << __FUNCTION__ << "(" << this << ")" << std::endl
class Item
{
    Item(const Item &);
public:
    Item()
    {
        gnItemCount ++; mpn = new int[1024];
        ShowFn();
    }
    ~Item()
    {
        gnItemCount --; delete [] mpn;
        ShowFn();
    }
    Item & operator = (const Item &it)
    {
        delete []mpn;
        mpn = NULL;
        return *this;
    }
public:
    int *mpn;
    AISmartObject<Item> mpNext;
};

void test_InList(int nTimes, bool bLoop = false);
void test_simple_loop(int nTimes);
void test_simple_Item(int nTimes);
void test_simple_ptr(int nTimes);
void test_simple_ptr_set(int nTimes);
void test_simple_copy(int nTimes);
void test_insert_of_set_perform(int nTimes);
void test_insert_of_dqueue(int nTimes);
void test_iterator_of_set_perform(int nTimes);
void test_push_pop_of_list_perform(int nTimes);

void test_AISmartObject()
{
    collect_garbage();
    test_InList(50, true);
    test_InList(100, false);
//    test_InList(200);
//    test_InList(500);
//    test_InList(800);
//    test_InList(1000);
//    test_InList(1500);
//    test_InList(3000);
//    test_InList(30000);
    test_InList(300 * 1);
    
    test_simple_copy(1000 * 1000);
    
    test_simple_loop(1000 * 100);
    test_simple_Item(1000 * 100);
    test_simple_ptr(1000 * 100);
    test_simple_ptr_set(1000 * 100);
    
    test_insert_of_set_perform(1000 * 100);
    test_insert_of_dqueue(1000 * 1000);
    test_iterator_of_set_perform(1000 * 100);
    test_push_pop_of_list_perform(1000 * 1000);
}
static bool gbSaveListProcessToFile = false;
void test_InList(int nTimes, bool bLoop)
{
    ftduration d;
    {
        AISmartObject<Item> head, tail, tmp;
        int nIndex = 0;
        head.Init();
        tail.Init();
        head->mpNext = tail;
        SaveAIObjectGraphToFile("list_begin.dot");
        
        for (nIndex = 0; nIndex < nTimes;++nIndex)
        {
            AISmartObject<Item> obj;
            obj.Init();
            if(gbSaveListProcessToFile)
            {
                std::stringstream ss;
                ss << "list_" << nIndex << "_pre.dot";
                SaveAIObjectGraphToFile(ss.str().c_str());
            }
            tail->mpNext = obj;
            tail = obj;
            if(gbSaveListProcessToFile)
            {
                std::stringstream ss;
                ss << "list_" << nIndex << ".dot";
                SaveAIObjectGraphToFile(ss.str().c_str());
            }
        }
        collect_garbage();
        tmp = head;
        while(tmp.IsValid())
        {
            tmp = tmp->mpNext;
        }
        if(bLoop)
        {
            tail->mpNext = head;
        }
    }
    std::cout << __FUNCTION__ << " nTimes is " << nTimes << ", total time is " << d.durationSecond() << " seconds, speed is " << nTimes / d.durationSecond() << " times/s" << std::endl;
    std::cout << __FUNCTION__ << " gnItemCount is " << gnItemCount << std::endl;
    collect_garbage();
    SaveAIObjectGraphToFile("list_end.dot");
}

void test_simple_loop(int nTimes)
{
    ftduration d;
    int nIndex = 0;
    for (nIndex = 0; nIndex < nTimes;++nIndex)
    {
        AISmartObject<Item> item, item2;
        item.Init();
        item2.Init();
        item->mpNext.Init();
        item->mpNext->mpNext = item2;
        item2->mpNext = item;

        if(nIndex % 1000 == 20)
        {
            collect_garbage_with_line();
        }
//        SaveAIObjectGraphToFile("simple_loop_begin.dot");
    }
    collect_garbage();
    std::cout << __FUNCTION__ << " nTimes is " << nTimes << ", total time is " << d.durationSecond() << " seconds, speed is " << nTimes / d.durationSecond() << " times/s" << std::endl;
    std::cout << __FUNCTION__ << " gnItemCount is " << gnItemCount << std::endl;
    SaveAIObjectGraphToFile("simple_loop.dot");
}
void test_simple_Item(int nTimes)
{
    ftduration d;
    int nIndex = 0;
    for (nIndex = 0; nIndex < nTimes;++nIndex)
    {
        AISmartObject<Item> item;
        item.Init();
        item.Init();
        item.Init();
    }
    
    std::cout << __FUNCTION__ << " nTimes is " << nTimes << ", total time is " << d.durationSecond() << " seconds, speed is " << nTimes / d.durationSecond() << " times/s" << std::endl;
    std::cout << __FUNCTION__ << " gnItemCount is " << gnItemCount << std::endl;
}
void test_simple_ptr(int nTimes)
{
    ftduration d;
    int nIndex = 0;
    for (nIndex = 0; nIndex < nTimes;++nIndex)
    {
        AISmartObject<int> item;
    }
    
    std::cout << __FUNCTION__ << " nTimes is " << nTimes << ", total time is " << d.durationSecond() << " seconds, speed is " << nTimes / d.durationSecond() << " times/s" << std::endl;
    std::cout << __FUNCTION__ << " gnItemCount is " << gnItemCount << std::endl;
}
void test_simple_ptr_set(int nTimes)
{
    ftduration d;
    int nIndex = 0;
    AISmartObject<int> item;
    item.Init(10);
    AISmartObject<int> i;
    for (nIndex = 0; nIndex < nTimes;++nIndex)
    {
        i = item;
    }
    
    std::cout << __FUNCTION__ << " nTimes is " << nTimes << ", total time is " << d.durationSecond() << " seconds, speed is " << nTimes / d.durationSecond() << " times/s" << std::endl;
    std::cout << __FUNCTION__ << " gnItemCount is " << gnItemCount << std::endl;
}
static void some_func(AISmartObject<Item> )
{
    
}
void test_simple_copy(int nTimes)
{
    int nIndex = 0;
    ftduration d;
    gb_show_log = nTimes < 10;
    std::cout << __FUNCTION__ << " begin gnItemCount is " << gnItemCount << std::endl;
    {
        AISmartObject<Item> si;
        si.Init();
        for (nIndex = 0; nIndex < nTimes; ++nIndex)
        {
//            AISmartObject<Item> tmp;
            some_func(si);
//            Item i;
//            tmp = i;
//            tmp->mpNext = tmp;
//            collect_garbage_with_line();
        }
    }
    collect_garbage_with_line();
    std::cout << __FUNCTION__ << " gnItemCount is " << gnItemCount << std::endl;
    std::cout << __FUNCTION__ << " nTimes is " << nTimes << ", total time is " << d.durationSecond() << " seconds, speed is " << nTimes / d.durationSecond() << " times/s" << std::endl;
    
    gb_show_log = false;
}
void test_insert_of_set_perform(int nTimes)
{
    ftduration d;
    std::set<int> nset;
    int n = 0;
    for (n = -10; n < 0; ++n)
    {
        nset.insert(n);
    }
    for (n = 0; n < nTimes; ++n)
    {
        nset.insert(n);
        nset.erase(n);
    }
    std::cout << __FUNCTION__ << " nTimes is " << nTimes << ", total time is " << d.durationSecond() << " seconds, speed is " << nTimes / d.durationSecond() << " times/s" << std::endl;
}
void test_iterator_of_set_perform(int nTimes)
{
    ftduration d;
    std::set<int> nset;
    int n = 0;
    for (n = -10; n < 0; ++n)
    {
        nset.insert(n);
    }
    std::set<int>::iterator iter;
    for (n = 0; n < nTimes; ++n)
    {
        iter = nset.begin();
        ++iter;
    }
    std::cout << __FUNCTION__ << " nTimes is " << nTimes << ", total time is " << d.durationSecond() << " seconds, speed is " << nTimes / d.durationSecond() << " times/s" << std::endl;
}
void test_push_pop_of_list_perform(int nTimes)
{
    ftduration d;
    std::list<int> nlist;
    int n = 0;
    for (n = -10; n < 0; ++n)
    {
        nlist.push_back(n);
    }
    std::set<int>::iterator iter;
    for (n = 0; n < nTimes; ++n)
    {
        nlist.push_back(n);
        nlist.pop_back();
    }
    std::cout << __FUNCTION__ << " nTimes is " << nTimes << ", total time is " << d.durationSecond() << " seconds, speed is " << nTimes / d.durationSecond() << " times/s" << std::endl;
}
void test_insert_of_dqueue(int nTimes)
{
    ftduration d;
    std::set<int> nset;
    int n = 0;
    class Cls
    {
    public:
        Cls() : mpNext(NULL), mpPre(NULL), n(0) {}
        Cls *mpNext;
        Cls *mpPre;
        int n;
    };
    Cls head, tail;
    head.mpNext = &tail;
    tail.mpPre = &head;
//    Cls *ph = &head;
    Cls *pt = &tail;
    Cls tmp;
    Cls *pTmp = &tmp;
    for (n = 0; n < nTimes; ++n)
    {
        pt->mpNext = pTmp;
        pTmp->mpPre = pt;
        pt = pTmp;
        
        pt = pt->mpPre;
        pt->mpNext->mpPre = NULL;
        pt->mpNext = NULL;
    }
    std::cout << __FUNCTION__ << " nTimes is " << nTimes << ", total time is " << d.durationSecond() << " seconds, speed is " << nTimes / d.durationSecond() << " times/s" << std::endl;
}
void collect_garbage_with_line(const char *szFile, int nLine)
{
    ftduration d;
    unsigned int nFreeCount = AISmartObject_check_garbage();
    if(szFile)
    {
        std::cout << "----------------------" << std::endl;
        std::cout << nLine << ": _i_so_check_garbage free " << nFreeCount << " objects, used " << d.durationSecond() << " seconds." << std::endl;
        std::cout << std::endl;
    }
}
