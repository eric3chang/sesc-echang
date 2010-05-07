#pragma once
#include "Pool.h"

class StoredFunctionBase
{
protected:
	StoredFunctionBase(){}
	virtual void Execute() = 0;
public:
	void Call()
	{
		Execute();
	}
};
class CompositStoredFunction : public StoredFunctionBase
{
	StoredFunctionBase* f1, * f2;
protected:
	virtual void Execute()
	{
		f1->Call();
		f2->Call();
	}
public:
	void Initialize(StoredFunctionBase* f1, StoredFunctionBase* f2)
	{
		this->f1 = f1;
		this->f2 = f2;
	}
};

template <class Host, void (Host::*Func)()>
class StoredClassFunction0 : public StoredFunctionBase
{
protected:
	Host* host;
	virtual void Execute()
	{
		(host->*Func)();
	}
public:
	void Initialize(Host* h)
	{
		host = h;
	}
};
template <class Host, class Arg1, void (Host::*Func)(Arg1)>
class StoredClassFunction1 : public StoredFunctionBase
{
protected:
	Host* host;
	Arg1 arg1;
	virtual void Execute()
	{
		(host->*Func)(arg1);
	}
public:
	void Initialize(Host* h, Arg1 a1)
	{
		host = h;
		arg1 = a1;
	}
};
template <class Host, class Arg1, class Arg2, void (Host::*Func)(Arg1,Arg2)>
class StoredClassFunction2 : public StoredFunctionBase
{
protected:
	Host* host;
	Arg1 arg1;
	Arg2 arg2;
	virtual void Execute()
	{
		(host->*Func)(arg1,arg2);
	}
public:
	void Initialize(Host* h, Arg1 a1, Arg2 a2)
	{
		host = h;
		arg1 = a1;
		arg2 = a2;
	}
};
template <class Host, class Arg1, class Arg2, class Arg3, void (Host::*Func)(Arg1,Arg2,Arg3)>
class StoredClassFunction3 : public StoredFunctionBase
{
protected:
	Host* host;
	Arg1 arg1;
	Arg2 arg2;
	Arg3 arg3;
	virtual void Execute()
	{
		(host->*Func)(arg1,arg2,arg3);
	}
public:
	void Initialize(Host* h, Arg1 a1, Arg2 a2, Arg3 a3)
	{
		host = h;
		arg1 = a1;
		arg2 = a2;
		arg3 = a3;
	}
};

template <class T>
class PooledFunctionGenerator
{
	class PooledFunction : public T
	{
	protected:
		Pool<PooledFunction>* disposePool;
		virtual void Execute()
		{
			T::Execute();
			disposePool->Return(this);
		}
	public:
		void SetPool(Pool<PooledFunction>* disposePool)
		{
			this->disposePool = disposePool;
		}
	};

	Pool<PooledFunction> pool;
public:
	typedef T FunctionType;
	PooledFunctionGenerator(int reserve = 0, int maxReserve = 10000)
		: pool(reserve,maxReserve)
	{}
	T* Create()
	{
		PooledFunction* ret = pool.Take();
		ret->SetPool(&pool);
		return ret;
	}
};
