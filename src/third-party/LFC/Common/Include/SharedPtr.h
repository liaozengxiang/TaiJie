/*
 * 文件: SharedPtr.h
 * 功能: 引用计数型智能指针
 * 作者: 廖增祥
 * 日期: 2013-05-05
 */

#ifndef ___SHARED__PTR__H__20130505___
#define ___SHARED__PTR__H__20130505___

#include <iostream>
#include "OSType.h"


template <typename T> class CSharedPtr;
template <typename T> class CWeakPtr;
template <typename T> class CEnableSharedFromThis;

template <typename T> class dynamic_pointer_cast;
template <typename T> class static_pointer_cast;
template <typename T> class const_pointer_cast;

int atomic_conditional_increment(int *pw);
template <typename K, typename Y> void EnableSharedFromThis(CSharedPtr<K> *sp, CEnableSharedFromThis<Y> *p);
void EnableSharedFromThis(...);


/*
 |--------------------------------------------------------------------------
 | CSharedCount 模板类
 |--------------------------------------------------------------------------
 |
 | 计数器管理类，用于管理强引用计数器和弱引用计数器，被用于 CSharedPtr/CWeakPtr 中
 */
class CSharedCount
{
public:
    CSharedCount(Int32 nSharedCount, Int32 nWeakCount)
    {
        m_nSharedCount = nSharedCount;
        m_nWeakCount = nWeakCount;
    }

public:
    Int32 AddSharedCount() { return __sync_add_and_fetch(&m_nSharedCount, 1); }
    Int32 ReleaseSharedCount() { return __sync_sub_and_fetch(&m_nSharedCount, 1); }
    Int32 GetSharedCount() const { return m_nSharedCount; }

    Int32 AddWeakCount() { return __sync_add_and_fetch(&m_nWeakCount, 1); }
    Int32 ReleaseWeakCount() { return __sync_sub_and_fetch(&m_nWeakCount, 1); }
    Int32 GetWeakCount() const { return m_nWeakCount; }

    Bool ShouldBeDeleted() const { return (m_nSharedCount == 0 && m_nWeakCount == 0); }

    Bool LockSharedCount()
    {
        // 若共享次数为 0，则返回失败；若共享次数大于 0，则将其值加 1 并返回成功
        // 所有操作必须为原子操作
        return (atomic_conditional_increment(&m_nSharedCount) != 0);
    }

private:
    Int32 m_nSharedCount;
    Int32 m_nWeakCount;
};


/*
 |--------------------------------------------------------------------------
 | CSharedPtr 模板类
 |--------------------------------------------------------------------------
 |
 | CSharedPtr 类为对目标对象的强引用，用于管理目标对象的生命周期
 |
 */
template <typename T>
class CSharedPtr
{
public:
	// 用一个堆指针构造智能指针对象
	explicit CSharedPtr(T *ptr = NULL)
	{
		m_pInstance = ptr;
        m_pRefCount = new CSharedCount(1, 0);

        EnableSharedFromThis(this, ptr);
	}
    
	// 用一个智能指针构造对象
	CSharedPtr(const CSharedPtr &rhs)
	{
		m_pRefCount = rhs.m_pRefCount;
		m_pInstance = rhs.m_pInstance;
        m_pRefCount->AddSharedCount();
	}

private:
    // 提供给 CWeakPtr<T>::Lock 函数使用，用户禁止调用此构造函数，引用计数已经加 1
    CSharedPtr(const CWeakPtr<T> &rhs)
    {
        m_pInstance = rhs.m_pInstance;
        m_pRefCount = rhs.m_pRefCount;
    }

    // 在实现 dynamic_pointer_cast/const_pointer_cast/static_pointer_cast 时构造
    CSharedPtr(T *ptr, CSharedCount *pRefCount)
    {
        m_pInstance = ptr;
        m_pRefCount = pRefCount;
        m_pRefCount->AddSharedCount();
    }

public:	
	// 智能指针的赋值，会先删除原来拥有的指针，再接管新的指针引用计数加1
	CSharedPtr& operator =(const CSharedPtr &rhs)
	{
		__Free();
		m_pRefCount = rhs.m_pRefCount;
		m_pInstance = rhs.m_pInstance;
        m_pRefCount->AddSharedCount();

		return *this;
	}

    void Swap(CSharedPtr &rhs)
    {
        std::swap(m_pInstance, rhs.m_pInstance);
        std::swap(m_pRefCount, rhs.m_pRefCount);
    }

	// 析构函数，解绑指针
	~CSharedPtr() { __Free(); }

	// 返回绑定的对象指针
	T* operator ->() const { return m_pInstance; }

	// 返回绑定的对象引用
	T& operator *() const { return *m_pInstance; }

	// 返回绑定的资源指针
	T* Get() const { return m_pInstance; }

	// 判断智能指针和其他类型是否相等，实际上比较的是它所绑定的指针的值
	Bool operator ==(const CSharedPtr &rhs) const { return m_pRefCount == rhs.m_pRefCount; }
	Bool operator ==(const void *ptr) const { return m_pInstance == (const T *)ptr; }

	Bool operator !=(const CSharedPtr &rhs) const { return m_pRefCount != rhs.m_pRefCount; }
	Bool operator !=(const void *ptr) const { return m_pInstance != (const T *)ptr; }

    Bool operator <(const CSharedPtr &rhs) const { return m_pRefCount < rhs.m_pRefCount; }

    Int32 GetSharedCount() const { return m_pRefCount->GetSharedCount(); }
    Int32 GetWeakCount() const { return m_pRefCount->GetWeakCount(); }

private:
	void __Free()
	{
		// 计数器减 1
        if (m_pRefCount->ReleaseSharedCount() == 0)
        {
            // 删除计数器的操作必须在删除对象前，防止对象继承自 CEnableSharedFromThis 时成员中包含 CWeakPtr 对象
            if (m_pRefCount->ShouldBeDeleted())
            {
                delete m_pRefCount;
                m_pRefCount = NULL;
            }

			delete m_pInstance;
			m_pInstance = NULL;
		}
	}

private:
	CSharedCount*  m_pRefCount;				// 引用计数器
	T*             m_pInstance;				// 被绑定的实例指针

    template <typename K> friend class dynamic_pointer_cast;
    template <typename K> friend class static_pointer_cast;
    template <typename K> friend class const_pointer_cast;
    template <typename K> friend class CWeakPtr;
};


/*
 |--------------------------------------------------------------------------
 | CWeakPtr 模板类
 |--------------------------------------------------------------------------
 |
 | CWeakPtr 类为对目标对象的弱引用，在使用时，需要通过 CWeakPtr::Lock 方法尝试获取 CSharedPtr
 | 成功后方可调用目标对象的成员方法
 |
 */
template <typename T>
class CWeakPtr
{
public:
    CWeakPtr()
    {
        m_pRefCount = new CSharedCount(0, 1);
        m_pInstance = NULL;
    }

    CWeakPtr(const CSharedPtr<T> &spObject)
    {
        m_pRefCount = spObject.m_pRefCount;
        m_pInstance   = spObject.Get();

        m_pRefCount->AddWeakCount();
    }

    CWeakPtr(const CWeakPtr &rhs)
    {
        m_pRefCount = rhs.m_pRefCount;
        m_pInstance = rhs.m_pInstance;

        m_pRefCount->AddWeakCount();
    }

    CWeakPtr& operator =(const CWeakPtr &rhs)
    {
        Free();
        m_pRefCount = rhs.m_pRefCount;
        m_pInstance = rhs.m_pInstance;

        m_pRefCount->AddWeakCount();
        return *this;
    }

	template <typename K>
	CWeakPtr& operator =(const CSharedPtr<K> &rhs)
	{
		Free();

		m_pRefCount = rhs.m_pRefCount;
		m_pInstance = dynamic_cast<T *>(rhs.m_pInstance);

		m_pRefCount->AddWeakCount();
		return *this;
	}

    CWeakPtr& operator =(const CSharedPtr<T> &rhs)
    {
        Free();

        m_pRefCount = rhs.m_pRefCount;
        m_pInstance = rhs.Get();

        m_pRefCount->AddWeakCount();
        return *this;
    }

    Bool Expired() const { return m_pRefCount->GetSharedCount() == 0; }

    Bool operator ==(const CWeakPtr &rhs) const { return m_pRefCount == rhs.m_pRefCount; }
    Bool operator !=(const CWeakPtr &rhs) const { return m_pRefCount != rhs.m_pRefCount; }
    Bool operator <(const CWeakPtr &rhs) const { return m_pRefCount < rhs.m_pRefCount; }

    ~CWeakPtr()
    {
        Free();
    }

    CSharedPtr<T> Lock()
    {
        if (m_pRefCount->LockSharedCount())
        {
            return CSharedPtr<T>(*this);
        }

        return CSharedPtr<T>(NULL);
    }

private:
    void Free()
    {
        if (m_pRefCount->ReleaseWeakCount() == 0 && m_pRefCount->ShouldBeDeleted())
        {
            delete m_pRefCount;
            m_pRefCount = NULL;
        }
    }

private:
    CSharedCount*   m_pRefCount;
    T*              m_pInstance;

    // 声明友元类，
    template <typename K> friend class CSharedPtr;
};


/*
 |--------------------------------------------------------------------------
 | CEnableSharedFromThis 模板类
 |--------------------------------------------------------------------------
 |
 | 在需要从客户类成员函数中向其他函数传递自身类的 CSharedPtr/CWeakPtr 时
 | 需要让客户类继承自该类
 |
 | CEnableSharedFromThis::m_wpInstance 的初始化在 CSharedPtr 的构造函数中进行
 | 所以一定不要在客户类的构造函数中调用 SharedFromThis 方法
 |
 */
template <typename T>
class CEnableSharedFromThis
{
public:
    virtual ~CEnableSharedFromThis() {}

protected:
    CSharedPtr<T> SharedFromThis() { return m_wpInstance.Lock(); }

private:
    CWeakPtr<T>     m_wpInstance;

    // 声明友元函数，该函数需要初始化 m_wpInstance 成员
    template <typename K, typename Y> friend void EnableSharedFromThis(CSharedPtr<K> *sp, CEnableSharedFromThis<Y> *p);
};


/*
 |--------------------------------------------------------------------------
 | dynamic_pointer_cast
 |--------------------------------------------------------------------------
 |
 */
template <typename T>
class dynamic_pointer_cast
{
public:
    template <typename Y>
    dynamic_pointer_cast(const CSharedPtr<Y> &ptr)
        : m_spInstance(dynamic_cast<T *>(ptr.m_pInstance), ptr.m_pRefCount)
    {
    }

    operator CSharedPtr<T> () const { return m_spInstance; }

private:
    CSharedPtr<T>   m_spInstance;
};


/*
 |--------------------------------------------------------------------------
 | static_pointer_cast
 |--------------------------------------------------------------------------
 |
 */
template <typename T>
class static_pointer_cast
{
public:
    template <typename Y>
    static_pointer_cast(const CSharedPtr<Y> &ptr)
        : m_spInstance(static_cast<T *>(ptr.m_pInstance), ptr.m_pRefCount)
    {
    }

    operator CSharedPtr<T>() const { return m_spInstance; }

private:
    CSharedPtr<T>   m_spInstance;
};


/*
 |--------------------------------------------------------------------------
 | const_pointer_cast
 |--------------------------------------------------------------------------
 |
 */
template <typename T>
class const_pointer_cast
{
public:
    template <typename Y>
    const_pointer_cast(const CSharedPtr<Y> &ptr)
        : m_spInstance(const_cast<T *>(ptr.m_pInstance), ptr.m_pRefCount)
    {
    }

    operator CSharedPtr<T>& () const { return m_spInstance; }

private:
    CSharedPtr<T>   m_spInstance;
};


/*
 |--------------------------------------------------------------------------
 | EnableSharedFromThis
 |--------------------------------------------------------------------------
 |
 | 由 CSharedPtr 的第一个使用原始指针版本的构造函数调用
 | 若客户端继承自 CEnableSharedFromThis, 表明客户端类需要在内部获取 CSharedPtr 指针
 | 该函数用于将 CEnableSharedFromThis::m_wpInstance 进行初始化
 |
 | inline void EnableShareFromThis(...);
 | 该函数为重载版本，服务于没有继承 CEnableSharedFromThis 类的客户类
 |
 */
template <typename K, typename Y>
void EnableSharedFromThis(CSharedPtr<K> *sp, CEnableSharedFromThis<Y> *p)
{
    if (p != NULL)
    {
        p->m_wpInstance = *sp;
    }
}

#endif
