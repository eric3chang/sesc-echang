#pragma once
#include <stack>
#include "Debug.h"

template <class Type>
class Pool
{
private:
	std::stack<Type*> bank;
	size_t upperBound;
public:
	Pool(int reserve = 0, int upperBound = 1000000)
	{
		DebugAssert(reserve >= 0);
		DebugAssert(upperBound > reserve);
		this->upperBound = upperBound;
		for(int i = 0; i < reserve; i++)
		{
			bank.push(new Type());
		}
	}
	~Pool()
	{
		while(!bank.empty())
		{
			delete bank.top();
			bank.pop();
		}
	}
	Type* Take()
	{
		if(bank.empty())
		{
			return new Type();
		}
		Type* ret = bank.top();
		bank.pop();
		return ret;
	}
	void Return(Type* element)
	{
#ifdef _DEBUG
		std::stack<Type*> compareStack;
		while(!bank.empty())
		{
			compareStack.push(bank.top());
			bank.pop();
		}
		while(!compareStack.empty())
		{
			DebugAssert(element != compareStack.top());
			bank.push(compareStack.top());
			compareStack.pop();
		}
#endif
		if((int)bank.size() < upperBound)
		{
			bank.push(element);
		}
		else
		{
			delete element;
		}
	}		
};