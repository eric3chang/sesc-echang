#ifndef COLLISION_DETECTION_FLATTEN_H
#define COLLISION_DETECTION_FLATTEN_H

#include "estl.h"
#include "TMOpCodes.h"

template <class T> // where T : ITMCollisionDetection
class CollisionDetectionFlatten : public T
{
	std::vector<uint32_t> parentSet;
	std::vector<uint32_t> childSet;
	HASH_MAP<uint32_t,uint32_t> parentRelation;
	bool suppressActions;
	uint32_t parent;
	uint32_t child;
	uint32_t opCode;
	bool isTransInst;
public:
	template<class A, class B, class C, class D, class E, class F, class G, class H, class I, class J>
	CollisionDetectionFlatten(A a, B b, C c, D d, E e, F f, G g, H h, I i, J j) : T(a,b,c,d,e,f,g,h,i,j){}
	template<class A, class B, class C, class D, class E, class F, class G, class H, class I>
	CollisionDetectionFlatten(A a, B b, C c, D d, E e, F f, G g, H h, I i) : T(a,b,c,d,e,f,g,h,i){}
	template<class A, class B, class C, class D, class E, class F, class G, class H>
	CollisionDetectionFlatten(A a, B b, C c, D d, E e, F f, G g, H h) : T(a,b,c,d,e,f,g,h){}
	template<class A, class B, class C, class D, class E, class F, class G>
	CollisionDetectionFlatten(A a, B b, C c, D d, E e, F f, G g) : T(a,b,c,d,e,f,g){}
	template<class A, class B, class C, class D, class E, class F>
	CollisionDetectionFlatten(A a, B b, C c, D d, E e, F f) : T(a,b,c,d,e,f){}
	template<class A, class B, class C, class D, class E>
	CollisionDetectionFlatten(A a, B b, C c, D d, E e) : T(a,b,c,d,e){}
	template<class A, class B, class C, class D>
	CollisionDetectionFlatten(A a, B b, C c, D d) : T(a,b,c,d){}
	template<class A, class B, class C>
	CollisionDetectionFlatten(A a, B b, C c) : T(a,b,c){}
	template<class A, class B>
	CollisionDetectionFlatten(A a, B b) : T(a,b){}
	template<class A>
	CollisionDetectionFlatten(A a) : T(a){}
	CollisionDetectionFlatten() : T(){}
	virtual ~CollisionDetectionFlatten(){}

	virtual void ObserveAccess(bool isRead, int pid, uint32_t transactionEntry, VAddr address, size_t size)
	{
		suppressActions = false;
		isTransInst = false;
		while(transactionEntry)
		{
			I(parentRelation.find(transactionEntry) != parentRelation.end());
			if(parentRelation[transactionEntry] == 0)
			{
				T::ObserveAccess(isRead,pid,transactionEntry,address,size);
				return;
			}
			else
			{
				transactionEntry = parentRelation[transactionEntry];
			}
		}
		T::ObserveAccess(isRead,pid,transactionEntry,address,size);
	}
		
	virtual void ObserveTransactionInst(int pid, uint32_t oldLevel, uint32_t newLevel, uint32_t opCode,
		uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5)
	{
		isTransInst = true;
		this->opCode = opCode;
		if(opCode == TMOpCodes::TransBegin)
		{
			parent = oldLevel;
			child = newLevel;
			I(parentRelation.find(oldLevel) == parentRelation.end());
			if(oldLevel)
			{
				suppressActions = true;
			}
			else
			{
				suppressActions = false;
			}
		}
		else
		{
			parent = newLevel;
			child = oldLevel;
			I(parentRelation.find(oldLevel) != parentRelation.end());
			I(parentRelation[oldLevel] == newLevel);
			if(newLevel)
			{
				suppressActions = true;
			}
			else
			{
				suppressActions = false;
			}
		}
		if(!suppressActions)
		{
			T::ObserveTransactionInst(pid,oldLevel,newLevel,opCode,arg1,arg2,arg3,arg4,arg5);
		}
	}

	virtual void FinalizeInstruction()
	{
		if(isTransInst)
		{
			if(opCode == TMOpCodes::TransBegin)
			{
				I(parentRelation.find(parent) == parentRelation.end());
				parentRelation[child] = parent;
				parentSet.push_back(parent);
				childSet.push_back(child);
			}
			else
			{
				I(parentRelation.find(child) != parentRelation.end());
				I(parentRelation[child] == parent);
				parentRelation.erase(child);
			}
		}
		if(!suppressActions)
		{
			T::FinalizeInstruction();
		}
	}
	virtual void RollbackInstruction()
	{
		if(!suppressActions)
		{
			T::RollbackInstruction();
		}
	}

	virtual uint32_t GetDelay()
	{
		if(suppressActions)
		{
			return 0;
		}
		return T::GetDelay();
	}

	virtual void MayNotDelay()
	{
		if(!suppressActions)
		{
			T::MayNotDelay();
		}
	}

	virtual int CollisionCount()
	{
		if(!suppressActions)
		{
			return T::CollisionCount();
		}
		return 0;
	}

	virtual void DumpStats(std::ostream& out)
	{
		out << "Group string Aspect Flatten string Module CollisionDetection #" << std::endl;
		for(size_t i = 0; i < parentSet.size(); i++)
		{
			out << "int Parent " << parentSet[i] << " int Child " << childSet[i] << " #" << std::endl;
		}
		out << "EndGroup #" << std::endl;
		T::DumpStats(out);
	}
};

#endif