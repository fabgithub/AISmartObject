//
//  AISmartObject.cpp
//  TrySomething
//
//  Created by liyoudi on 15/6/4.
//  Copyright (c) 2015年 liyoudi. All rights reserved.
//

#include "AISmartObject.h"

#include <list>
#include <set>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <map>

//
void _i_so_save_link_graph();

// 保存导至引用循环的成员集合
typedef std::set<AISmartObject_Base *> RefSobSet;

AISmartObject_Base::AISmartObject_Base()
: mpso(NULL), mpBelongTo(NULL), mpNext(NULL), mpPrev(NULL)
{
}
AISmartObject_Base::AISmartObject_Base(const AISmartObject_Base &)
: mpso(NULL), mpBelongTo(NULL), mpNext(NULL), mpPrev(NULL)
{
}

// 根下的链表 双向链表的头尾永远不变
static AISmartObject_Base * _i_init_root_list()
{
    static AISmartObject_Base gHT[2];
    
    if(gHT[0].mpNext == NULL || gHT[1].mpPrev == NULL)
    {
        gHT[0].mpNext = gHT + 1;
        gHT[1].mpPrev = gHT;
    }
    return gHT;
}
// 属于某个指针所指向对象的指针的链表 双向链表的头尾永远不变
static AISmartObject_Base * _i_init_inner_list()
{
    static AISmartObject_Base gHT[2];
    
    if(gHT[0].mpNext == NULL || gHT[1].mpPrev == NULL)
    {
        gHT[0].mpNext = gHT + 1;
        gHT[1].mpPrev = gHT;
    }
    return gHT;
}
//static AISmartObject_Base * _i_get_head()
//{
//    static AISmartObject_Base *ht = _i_init_list();
//    return ht;
//}
static AISmartObject_Base * _i_get_root_tail()
{
    static AISmartObject_Base *ht = _i_init_root_list();
    return ht + 1;
}
static AISmartObject_Base * _i_get_inner_tail()
{
    static AISmartObject_Base *ht = _i_init_inner_list();
    return ht + 1;
}

static void _i_insert_to_list(AISmartObject_Base *psob, AISmartObject_Base *pWhichTail)
{
    psob->mpPrev = pWhichTail->mpPrev;
    psob->mpNext = pWhichTail;
    pWhichTail->mpPrev->mpNext = psob;
    pWhichTail->mpPrev = psob;
}
void _i_insert_to_root_list(AISmartObject_Base *psob)
{
    static AISmartObject_Base *pT = _i_get_root_tail();
    _i_insert_to_list(psob, pT);
}
void _i_insert_to_inner_list(AISmartObject_Base *psob)
{
    static AISmartObject_Base *pT = _i_get_inner_tail();
    _i_insert_to_list(psob, pT);
}
static void _i_remove_from_list(AISmartObject_Base *psob)
{
    psob->mpPrev->mpNext = psob->mpNext;
    psob->mpNext->mpPrev = psob->mpPrev;
    psob->mpNext = psob->mpPrev = NULL;
}

static int gn_i_AISmartObject_data_Count = 0;

typedef std::list<AISmartObject_Base *> ChildList;

class _i_AISmartObject_data
{
    _i_AISmartObject_data(const _i_AISmartObject_data &);
    _i_AISmartObject_data & operator = (const _i_AISmartObject_data &);
public:
    _i_AISmartObject_data(void (*fnFree) (void *), const char *szTypeName = "") : mptr(NULL), mnMemSize(0), mnRefCount(0)
    , mfnFree(fnFree)
    , mszTypeName(szTypeName)
    , mnCanFree(0)
    {
        ++gn_i_AISmartObject_data_Count;
//        std::cout << __FUNCTION__ << ": " << this << std::endl;
    }
    ~_i_AISmartObject_data()
    {
        --gn_i_AISmartObject_data_Count;
//        std::cout << __FUNCTION__ << ": " << this << std::endl;
    }

public:
    void *mptr;
    unsigned int mnMemSize;
    int mnRefCount;
    void (*mfnFree) (void *);
    
    const char *mszTypeName;
    int mnCanFree; // -1表示未判定，0表示判定为不可释放, 1表示判定为可释放
    
    ChildList mChildList; // 保存此对象中的其它指针对象。
};

class _i_SodLess
{
public:
    bool operator () (const _i_AISmartObject_data *so1, const _i_AISmartObject_data *so2)
    {
        if(so1->mptr < so2->mptr)
        {
            return true;
        }
        else if(so1->mptr > so2->mptr)
        {
            return false;
        }
        return so1->mnMemSize < so2->mnMemSize;
    }
};

_i_AISmartObject_data * _i_so_Init(void (*fnFree) (void *), const char *szTypeName)
{
    return new _i_AISmartObject_data(fnFree, szTypeName);
}
int _i_so_Increase(AISmartObject_Base *psob)
{
    ++psob->mpso->mnRefCount;
    return psob->mpso->mnRefCount;
}

typedef std::set<_i_AISmartObject_data *, _i_SodLess> SodSet;
typedef std::set<void *> SobSet; // AISmartObject_Base指针集合
//static SobSet gSobSet;

int _i_so_Decrease(AISmartObject_Base *psob)
{
    --psob->mpso->mnRefCount;
    
    if(psob->mpso->mnRefCount <= 0)
    {
        psob->mpso->mfnFree( psob->mpso->mptr );
        delete psob->mpso;
        psob->mpso = NULL;
        return 0;
    }
    return  psob->mpso->mnRefCount;
}

void _i_so_SetPtr(AISmartObject_Base *psob, void *ptr, unsigned int nMemSize, bool bMoveToInnerAfterSet)
{
    psob->mpso->mptr = ptr;
    psob->mpso->mnMemSize = nMemSize;
    //
    void *ptrEnd = ((char *) ptr) + nMemSize;
    //
    AISmartObject_Base *pPrev = NULL;
    AISmartObject_Base *pIdx = _i_get_root_tail()->mpPrev;
    while(pIdx != psob)
    {
        pPrev = pIdx->mpPrev;
        if(ptr <= pIdx && pIdx < ptrEnd)
        {
            _i_remove_from_list(pIdx);
            _i_insert_to_inner_list(pIdx);
            pIdx->mpBelongTo = psob->mpso;
            psob->mpso->mChildList.push_front(pIdx);
        }
        pIdx = pPrev;
    }
    if(bMoveToInnerAfterSet)
    {
        _i_remove_from_list(psob);
        _i_insert_to_inner_list(psob);
    }
//    //
//    _i_so_save_link_graph();
}
void * _i_so_GetPtr(_i_AISmartObject_data *pso)
{
    if(!pso->mptr)
    {
        throw "NULL";
    }
    return pso->mptr;
}
void _i_so_Register(AISmartObject_Base *psob)
{
//    gSobSet.insert(psob);
    //
    _i_insert_to_root_list(psob);
}
void _i_so_Unregister(AISmartObject_Base *psob)
{
//    gSobSet.erase(psob);
    _i_remove_from_list(psob);
}
bool _i_so_ShouldMoveForRegister(AISmartObject_Base *psob)
{
    if(psob->mpBelongTo)
    {
        _i_remove_from_list(psob);
        _i_insert_to_root_list(psob);
        return true;
    }
    return false;
}

bool _i_so_IsNull(const AISmartObject_Base *psob)
{
    return psob->mpso == NULL || psob->mpso->mptr == NULL;
}

void _i_so_SetSoValue(AISmartObject_Base *psob, const AISmartObject_Base *pUsingSob)
{
    psob->mpso = pUsingSob->mpso;
}

/***
 可以实现一个函数，用来释放可能有引用循环的内存。
 算法是
 1. 扫描一遍，将所有的内存对象记录下来。
 2. 检查这些内存对象是否有全局智能指针指向它，如果没有，则说明是有引用循环，将其释放。
 */
unsigned int AISmartObject_check_garbage()
{
    static int gnThisMagicForNoFree = 100; // 每次循环使用此标志判断如果不是此值，则需要释放。
    ++gnThisMagicForNoFree;
    if(gnThisMagicForNoFree <= 0)
    {
        ++gnThisMagicForNoFree;
    }
    //
    std::map<void *, void (*) (void *)> canFreeSet;
    AISmartObject_Base *pTmp = NULL;
    pTmp = _i_get_root_tail()->mpPrev;
    for (; pTmp->mpPrev; pTmp = pTmp->mpPrev)
    {
        pTmp->mpso->mnCanFree = gnThisMagicForNoFree;
        // 所有其子对象也是不可释放的。
        if(!pTmp->mpso->mChildList.empty())
        {
            ChildList stack = pTmp->mpso->mChildList;
            while(!stack.empty())
            {
                AISmartObject_Base *psob = stack.front();
                stack.pop_front();
                if(psob->mpso->mnCanFree != gnThisMagicForNoFree)
                {
                    psob->mpso->mnCanFree = gnThisMagicForNoFree;
                    stack.insert(stack.end(), psob->mpso->mChildList.begin(), psob->mpso->mChildList.end());
                }
            }
        }
    }
    //

    pTmp = _i_get_inner_tail()->mpPrev;
    for (; pTmp->mpPrev; pTmp = pTmp->mpPrev)
    {
        if(pTmp->mpso->mnCanFree != gnThisMagicForNoFree)
        {
            canFreeSet[pTmp->mpso->mptr] = pTmp->mpso->mfnFree;
            pTmp->mpso->mptr = NULL;
        }
    }
    //
    std::map<void *, void (*) (void *)>::iterator iterFree;
    for (iterFree = canFreeSet.begin(); iterFree != canFreeSet.end(); ++iterFree)
    {
        iterFree->second(iterFree->first);
    }
//    std::cout << "gn_i_AISmartObject_data_Count is " << gn_i_AISmartObject_data_Count << ", gSobSet.size() is " << gSobSet.size() << std::endl;
    return (unsigned int) canFreeSet.size();
}

// 获取当前对象的引用图
std::string _i_aio_get_statistic_graph()
{
    std::stringstream ss;
    ss << "digraph G {" << std::endl;
    ss << "    compound=true;" << std::endl;
    ss << "    //rankdir=LR;" << std::endl;
    //
    AISmartObject_Base *pTmp = NULL;
    SobSet::iterator iter, iterInner;
    SodSet sodSet;
    int nn = 0;
    pTmp = _i_get_root_tail()->mpPrev;
    for (nn = 0; nn < 2; ++nn)
    {
        for(;pTmp->mpPrev;pTmp = pTmp->mpPrev)
        {
            AISmartObject_Base *psob = pTmp;
            if(psob->mpso->mptr)
            {
                sodSet.insert(psob->mpso);
            }
        }
        pTmp = _i_get_inner_tail()->mpPrev;
    }
    SodSet::iterator iterSod;
    ChildList::iterator iterChild;
    for (iterSod = sodSet.begin(); iterSod != sodSet.end(); ++iterSod)
    {
        _i_AISmartObject_data *psod = (*iterSod);
        if(!psod->mChildList.empty())
        {
            ss << "    subgraph cluster_" << *iterSod << " {" << std::endl;
            ss << "        color=lightgray;" << std::endl;
            ss << "        style=filled;" << std::endl;
            ss << "        node [shape=record];" << std::endl;
            ss << "        label=\"obj_" << psod->mszTypeName << "(" << psod->mptr << ") size(" << psod->mnMemSize << ")\";" << std::endl;
            ss << "        obj_" << (*iterSod)->mptr;
            ss << "[label=\"{";
            bool bFirst = true;
            for(iterChild = psod->mChildList.begin();iterChild != psod->mChildList.end();++iterChild)
            {
                AISmartObject_Base *psob = (*iterChild);
                if(!bFirst)
                {
                    ss << "|";
                }
                bFirst = false;
                ss << "<ptr" << psob << "> ptr_" << psob->mpso->mszTypeName << "(" << psob << ")";
                if (psob->mpso == psod)
                {
                    ss << " self";
                }
                if(!psob->mpso->mptr)
                {
                    ss << " null";
                }
            }
            ss << "}\"];" << std::endl;
            ss << "    }" << std::endl;
        }
    }
    //
    pTmp = _i_get_root_tail()->mpPrev;

    for(nn = 0; nn < 2;++nn)
    {
        for(;pTmp->mpPrev;pTmp = pTmp->mpPrev)
        {
            AISmartObject_Base *psob = pTmp;
            if(psob->mpso->mptr && psob->mpso != psob->mpBelongTo)
            {
                ss << "    ";
                if(psob->mpBelongTo)
                {
                    ss << "obj_" << psob->mpBelongTo->mptr << ":";
                    ss << "ptr" << psob;
                }
                else
                {
                    ss << "ptr_" << psob->mpso->mszTypeName << "_" << psob;
                }
                ss << " -> ";
                if(!psob->mpso->mChildList.empty())
                {
                    AISmartObject_Base *psobTmp = (*psob->mpso->mChildList.begin());
                    if(psobTmp->mpBelongTo)
                    {
                        ss << "obj_" << psobTmp->mpBelongTo->mptr << ":";
                    }
                    ss << "ptr" << psobTmp;
                    if(!psobTmp->mpso->mptr)
                    {
                        ss << "_null";
                    }
                    if(psobTmp != psob)
                    {
                        ss << "[color=green, lhead=cluster_" << psob->mpso << "];" << std::endl;
                    }
                    else
                    {
                        ss << "[color=green];" << std::endl;
                    }
                }
                else //if(!psob->mpBelongTo)
                {
                    ss << "obj_" << psob->mpso->mszTypeName << "_" << psob->mpso->mptr << ";" << std::endl;
                }
            }
            else if(!psob->mpBelongTo && !psob->mpso->mptr)
            {
                ss << "ptr_" << psob << "_null;" << std::endl;
            }
        }
        pTmp = _i_get_inner_tail()->mpPrev;
    }
    ss << "}" << std::endl;
    return ss.str().substr(0, 100 * 1024);
}

/***
 生成链表图
 */
static void _i_so_gen_link_graph_for(AISmartObject_Base *psobTail, const char *szName, std::ostream &os)
{
    AISmartObject_Base *pTmp = psobTail;
    if(gn_i_AISmartObject_data_Count > 20)
    {
        return ;
    }
    os << "    subgraph cluster_" << psobTail << "{" << std::endl;
    os << "    label=\"" << szName << "\"" << std::endl;
    while(pTmp)
    {
        os << "    n_" << pTmp << " [label=\"<n_" << pTmp->mpPrev << ">||<n_" << pTmp->mpNext << ">\"]" << std::endl;
        pTmp = pTmp->mpPrev;
    }
    os << "}" << std::endl;
    pTmp = psobTail;
    while(pTmp)
    {
        if(pTmp->mpPrev)
        {
            os << "    n_" << pTmp << ":n_" << pTmp->mpPrev << " -> n_" << pTmp->mpPrev << ":n_" << pTmp->mpPrev->mpNext << std::endl;
        }
        if(pTmp->mpNext)
        {
            os << "    n_" << pTmp << ":n_" << pTmp->mpNext << " -> n_" << pTmp->mpNext << ":n_" << pTmp->mpNext->mpPrev << std::endl;
        }
        pTmp = pTmp->mpPrev;
    }
}
std::string _i_so_gen_link_graph()
{
    std::stringstream ss;
    ss << "digraph G {" << std::endl;
    ss << "    node[shape=record]" << std::endl;
    _i_so_gen_link_graph_for(_i_get_root_tail(), "root", ss);
    _i_so_gen_link_graph_for(_i_get_inner_tail(), "inner", ss);
    ss << "}" << std::endl;
    return ss.str();
}

void _i_so_save_link_graph(const char *szFile)
{
    std::ofstream of(szFile);
    if(!of)
    {
        
    }
    else
    {
        of << _i_so_gen_link_graph();
    }
}
void _i_so_save_link_graph()
{
    static int gnTimes = 0;
    ++gnTimes;
    if(gnTimes > 10)
    {
        return ;
    }
    std::stringstream ssFile;
    ssFile << "link_graph_" << gn_i_AISmartObject_data_Count << ".dot";
    _i_so_save_link_graph(ssFile.str().c_str());
}
/***
 TODO: 垃圾回收算法
 分两个集合来保存指针对象，一个是指向明确不需要释放的，另一个是可能会存在引用循环的（即该指针存在于另一个指针指向的对象中）。
 两个集合都用双向链表来保存。
*/
