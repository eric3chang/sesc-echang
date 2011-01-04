#ifndef COLLISION_DETECTION_NOBLEND_H
#define COLLISION_DETECTION_NOBLEND_H

#include "TMOpCodes.h"
#include <vector>
#include <queue>
#include <stack>
#include "estl.h"

template <class T> // where T : ITMCollisionDetection
class CollisionDetectionNoBlend : public T
{
	class StoredTransOp
	{
	public:
		uint32_t 
			oldTrans,
			newTrans,
			opcode,
			arg1,
			arg2,
			arg3,
			arg4,
			arg5;
	};
	bool stallCommand;
	bool lastWasAccess;
	bool forced;
	int currentPid;
	HASH_SET<uint32_t> currentActiveTrans;
	HASH_MAP<uint32_t,HASH_SET<uint32_t> > associatedGroup;
	HASH_MAP<uint32_t,uint32_t> mapsTo;
	StoredTransOp currentOp;
	std::vector<std::stack<uint32_t> > currentTrans;
	std::vector<std::queue<StoredTransOp> > forcedOps;
public:
	template<class A, class B, class C, class D, class E, class F, class G, class H, class I, class J>
	CollisionDetectionNoBlend(A a, B b, C c, D d, E e, F f, G g, H h, I i, J j) : T(a,b,c,d,e,f,g,h,i,j){}
	template<class A, class B, class C, class D, class E, class F, class G, class H, class I>
	CollisionDetectionNoBlend(A a, B b, C c, D d, E e, F f, G g, H h, I i) : T(a,b,c,d,e,f,g,h,i){}
	template<class A, class B, class C, class D, class E, class F, class G, class H>
	CollisionDetectionNoBlend(A a, B b, C c, D d, E e, F f, G g, H h) : T(a,b,c,d,e,f,g,h){}
	template<class A, class B, class C, class D, class E, class F, class G>
	CollisionDetectionNoBlend(A a, B b, C c, D d, E e, F f, G g) : T(a,b,c,d,e,f,g){}
	template<class A, class B, class C, class D, class E, class F>
	CollisionDetectionNoBlend(A a, B b, C c, D d, E e, F f) : T(a,b,c,d,e,f){}
	template<class A, class B, class C, class D, class E>
	CollisionDetectionNoBlend(A a, B b, C c, D d, E e) : T(a,b,c,d,e){}
	template<class A, class B, class C, class D>
	CollisionDetectionNoBlend(A a, B b, C c, D d) : T(a,b,c,d){}
	template<class A, class B, class C>
	CollisionDetectionNoBlend(A a, B b, C c) : T(a,b,c){}
	template<class A, class B>
	CollisionDetectionNoBlend(A a, B b) : T(a,b){}
	template<class A>
	CollisionDetectionNoBlend(A a) : T(a){}
	CollisionDetectionNoBlend() : T(){}
	virtual ~CollisionDetectionNoBlend(){}

	virtual void Initialize(TMProcessor** procSet, int procCount, TMMemory* memory)
	{
		forcedOps.resize(procCount);
		currentTrans.resize(procCount);
		T::Initialize(procSet,procCount,memory);
	}

	virtual void ObserveAccess(bool isRead, int pid, uint32_t transactionEntry, VAddr address, size_t size)
	{
		I(transactionEntry == 0 || currentActiveTrans.find(transactionEntry) != currentActiveTrans.end());
		currentPid = pid;
		lastWasAccess = true;
		forced = false;
		stallCommand = false;
		T::ObserveAccess(isRead,pid,transactionEntry,address,size);
	}
	virtual void ObserveTransactionInst(int pid, uint32_t oldLevel, uint32_t newLevel, uint32_t opCode,
		uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5)
	{
		lastWasAccess = false;
		forced = false;
		lastWasAccess = false;
		currentPid = pid;
		if(opCode == TMOpCodes::TransBegin)
		{
			if(currentTrans[pid].empty())
			{
				stallCommand = false;
			}
			else if(currentTrans[pid].top() == oldLevel)
			{
				stallCommand = false;
			}
			else
			{
				I(newLevel > currentTrans[pid].top());
				stallCommand = true;
			}
		}
		else
		{
			I(currentActiveTrans.find(oldLevel) != currentActiveTrans.end());
			I(currentTrans[pid].top() == oldLevel);
			stallCommand = false;
		}
		currentPid = pid;
		currentOp.oldTrans = oldLevel;
		currentOp.newTrans = newLevel;
		currentOp.opcode = opCode;
		currentOp.arg1 = arg1;
		currentOp.arg2 = arg2;
		currentOp.arg3 = arg3;
		currentOp.arg4 = arg4;
		currentOp.arg5 = arg5;
		if(!stallCommand)
		{
			T::ObserveTransactionInst(pid,oldLevel,newLevel,opCode,arg1,arg2,arg3,arg4,arg5);
		}
	}
	virtual void FinalizeInstruction()
	{
		if(!stallCommand)
		{
			T::FinalizeInstruction();
			if(!lastWasAccess)
			{
				if(currentOp.opcode != TMOpCodes::TransBegin)
				{
					I(!currentTrans[currentPid].empty());
					I(currentTrans[currentPid].top() == currentOp.oldTrans);
					I(currentActiveTrans.find(currentOp.oldTrans) != currentActiveTrans.end());
					currentActiveTrans.erase(currentOp.oldTrans);
					currentTrans[currentPid].pop();
					if(currentTrans[currentPid].empty())
					{
						while(!forcedOps[currentPid].empty())
						{
							StoredTransOp& st = forcedOps[currentPid].front();
							I(st.opcode == TMOpCodes::TransBegin);
							if(!currentTrans[currentPid].empty() && currentTrans[currentPid].top() != st.oldTrans)
							{
								break;
							}
							currentTrans[currentPid].push(currentOp.newTrans);
							currentActiveTrans.insert(currentOp.newTrans);
							T::ObserveTransactionInst(currentPid,st.oldTrans,st.newTrans,st.opcode,st.arg1,st.arg2,st.arg3,st.arg4,st.arg5);
							T::MayNotDelay();
							T::FinalizeInstruction();
							forcedOps[currentPid].pop();
						}
					}
				}
				else
				{
					if(currentTrans[currentPid].empty())
					{
						I(currentOp.oldTrans == 0);
					}
					else
					{
						I(currentOp.oldTrans == currentTrans[currentPid].top());
					}
					currentTrans[currentPid].push(currentOp.newTrans);
					currentActiveTrans.insert(currentOp.newTrans);
				}
			}
		}
		else
		{
			I(!lastWasAccess);
			forcedOps[currentPid].push(currentOp);
		}
	}
	virtual void RollbackInstruction()
	{
		if(!stallCommand)
		{
			T::RollbackInstruction();
		}
	}

	virtual uint32_t GetDelay()
	{
		if(stallCommand)
		{
			I(forced == false);
			return 10;
		}
		return T::GetDelay();
	}
	virtual void MayNotDelay()
	{
		I(!lastWasAccess);
		forced = true;
		if(!stallCommand)
		{
			T::MayNotDelay();
		}
	}
};

#endif
