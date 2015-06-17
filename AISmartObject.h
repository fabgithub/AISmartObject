//
//  AISmartObject.h
//  TrySomething
//
//  Created by liyoudi on 15/6/4.
//  Copyright (c) 2015年 liyoudi. All rights reserved.
//

#ifndef __TrySomething__AISmartObject__
#define __TrySomething__AISmartObject__

#include <stdio.h>
#include <typeinfo>

class _i_AISmartObject_data;
class AISmartObject_Base
{
public:
    AISmartObject_Base();
    AISmartObject_Base(const AISmartObject_Base &);
public:
    _i_AISmartObject_data *mpso;
    
    _i_AISmartObject_data *mpBelongTo; // 本对象的内存属于哪个实际对象，即是哪个对象的成员。
    
    AISmartObject_Base *mpNext;
    AISmartObject_Base *mpPrev;
};

_i_AISmartObject_data * _i_so_Init(void (*fnFree) (void *), const char *szTypeName);
int _i_so_Increase(AISmartObject_Base *psob);
int _i_so_Decrease(AISmartObject_Base *psob);
void _i_so_SetPtr(AISmartObject_Base *psob, void *ptr, unsigned int nMemSize, bool bMoveToInnerAfterSet);
void * _i_so_GetPtr(_i_AISmartObject_data *pso);

void _i_so_Register(AISmartObject_Base *psob);
void _i_so_Unregister(AISmartObject_Base *psob);
bool _i_so_ShouldMoveForRegister(AISmartObject_Base *psob);

bool _i_so_IsNull(const AISmartObject_Base *psob);
void _i_so_SetSoValue(AISmartObject_Base *psob, const AISmartObject_Base *pUsingSob);

template <class T>
class AISmartObject : public AISmartObject_Base
{
public:
    AISmartObject()
    {
        _i_so_Register(this);
        
        mpso = _i_so_Init(free_so_ptr, typeid(T).name());
        _i_so_Increase(this);
    }
    explicit AISmartObject(const T &v)
    {
        _i_so_Register(this);
        
        mpso = _i_so_Init(free_so_ptr, typeid(T).name());
        void *ptr = new T(v);
        _i_so_SetPtr(this, ptr, (unsigned int) sizeof(T), false);
        _i_so_Increase(this);
    }
    AISmartObject(const AISmartObject &so)
    {
        _i_so_Register(this);

        _i_so_SetSoValue(this, &so);
        _i_so_Increase(this);
    }
    ~AISmartObject()
    {
        if(mpso)
        {
            _i_so_Decrease(this);
        }
        _i_so_Unregister(this);
    }
    AISmartObject & operator = (const AISmartObject &so)
    {
        if(this != &so)
        {
            _i_so_Decrease(this);
            _i_so_SetSoValue(this, &so);
            _i_so_Increase(this);
        }
        return *this;
    }
    AISmartObject & operator = (const T &v)
    {
        if(_i_so_IsNull(this))
        {
            bool bIsInner = _i_so_ShouldMoveForRegister(this);
            T *ptr = new T();
            *ptr = v;
            _i_so_SetPtr(this, ptr, (unsigned int) sizeof(T), bIsInner);
        }
        else
        {
            *get() = v;
        }
        return *this;
    }
    
    
    bool IsValid() const { return !_i_so_IsNull(this); }
    
    bool operator ! () const { return !_i_so_IsNull(this); }
    bool operator < (const AISmartObject &so) const { return (*get() < (*so.get())); }
    bool operator > (const AISmartObject &so) const { return (*get() > (*so.get())); }
    bool operator <= (const AISmartObject &so) const { return (*get() <= (*so.get())); }
    bool operator >= (const AISmartObject &so) const { return (*get() >= (*so.get())); }
    bool operator == (const AISmartObject &so) const { return (*get() == (*so.get())); }
    bool operator != (const AISmartObject &so) const { return (*get() != (*so.get())); }

    AISmartObject & operator += (const AISmartObject &so) { (*get() += (*so.get())); return *this; }
    AISmartObject & operator -= (const AISmartObject &so) { (*get() -= (*so.get())); return *this; }
    AISmartObject & operator *= (const AISmartObject &so) { (*get() *= (*so.get())); return *this; }
    AISmartObject & operator /= (const AISmartObject &so) { (*get() /= (*so.get())); return *this; }

    AISmartObject operator + (const AISmartObject &so) const { return AISmartObject(*(*this) + *so); }
    AISmartObject operator - (const AISmartObject &so) const { return AISmartObject(*(*this) - *so); }
    AISmartObject operator * (const AISmartObject &so) const { return AISmartObject(*(*this) * *so); }
    AISmartObject operator / (const AISmartObject &so) const { return AISmartObject(*(*this) / *so); }

    T * operator -> () { return get(); }
    const T * operator -> () const { return get(); }
    T & operator * () { return *get(); }
    const T & operator * () const { return *get(); }
public:
    void Init()
    {
        if(_i_so_IsNull(this))
        {
            bool bIsInner = _i_so_ShouldMoveForRegister(this);
            void *ptr = new T();
            _i_so_SetPtr(this, ptr, (unsigned int) sizeof(T), bIsInner);
        }
        else
        {
            T t;
            *get() = t;
        }
    }
    template <class T1>
    void Init(const T1 &p1)
    {
        if(_i_so_IsNull(this))
        {
            bool bIsInner = _i_so_ShouldMoveForRegister(this);
            void *ptr = new T(p1);
            _i_so_SetPtr(this, ptr, (unsigned int) sizeof(T), bIsInner);
        }
        else
        {
            T t(p1);
            *get() = t;
        }
    }
    template <class T1, class T2>
    void Init(const T1 &p1, const T2 &p2)
    {
        if(_i_so_IsNull(this))
        {
            bool bIsInner = _i_so_ShouldMoveForRegister(this);
            void *ptr = new T(p1, p2);
            _i_so_SetPtr(this, ptr, (unsigned int) sizeof(T), bIsInner);
        }
        else
        {
            T t(p1, p2);
            *get() = t;
        }
    }
private:
    T * get() { return (T *) _i_so_GetPtr(mpso); }
    const T * get() const { return (T *) _i_so_GetPtr(mpso); }
    static void free_so_ptr(void *ptr)
    {
        delete (T *) ptr;
    }
private:
    
};

unsigned int AISmartObject_check_garbage();

#endif /* defined(__TrySomething__AISmartObject__) */
