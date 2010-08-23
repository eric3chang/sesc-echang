#include "MOESICache.h"
#include "../Messages/AllMessageTypes.h"
#include "../EventManager.h"
#include "../Connection.h"
#include "../Debug.h"
#include "../Configuration.h"
#include <cstdlib>
#include <algorithm>

// toggles debug output
#define MEMORY_MOESI_CACHE_DEBUG

namespace Memory
{
	int MOESICache::RandomEvictionPolicy::Evict(Memory::MOESICache::BlockState *set, int setSize)
	{
		DebugAssert(setSize > 0);
		int validCount = 0;
		for(int i = 0; i < setSize; i++)
		{
			if(set[i].valid && set[i].state != bs_Invalid && !set[i].locked)
			{
				validCount++;
			}
		}
		if(validCount == 0)
		{
			return InvalidBlock;
		}
		int index = rand() % setSize;
		for(int i = 0; i < setSize; i++)
		{
			if(set[i].valid && set[i].state != bs_Invalid && !set[i].locked)
			{
				if(index == 0)
				{
					return i;
				}
				index--;
			}
		}
		DebugFail("Strange indexing problem in Random Eviction [MOESI]");
		return InvalidBlock;
	}
	int MOESICache::LRUEvictionPolicy::Evict(Memory::MOESICache::BlockState *set, int setSize)
	{
		DebugAssert(setSize > 0);
		DebugAssert(set);
		TickTime useTime = (TickTime)-1;
		int index = InvalidBlock;
		int validCount = 0;
		for(int i = 0; i < setSize; i++)
		{
			if(set[i].valid && set[i].state != bs_Invalid && !set[i].locked)
			{
				TickTime t = std::max(set[i].lastWrite,set[i].lastRead);
				if(useTime > t)
				{
					index = i;
					useTime = t;
				}
				validCount++;
			}
		}
		if(validCount == 0)
		{
			return InvalidBlock;
		}
		DebugAssert(index != InvalidBlock);
		return index;
	}
	MOESICache::BlockState* MOESICache::GetSet(int i)
	{
		DebugAssert(i >= 0 && i < setCount);
		return &(cacheContents[i * associativity]);
	}
	void MOESICache::InvalidateBlock(MOESICache::BlockState& block)
	{
		DebugAssert(waitingOnBlockUnlock.find(block.tag) == waitingOnBlockUnlock.end());
		block.tag = 0;
		block.lastRead = 0;
		block.lastWrite = 0;
		block.state = bs_Invalid;
		block.valid = false;
		block.locked = false;
	}
	MOESICache::AddrTag MOESICache::CalcTag(Address addr)
	{
		return (AddrTag)(addr / lineSize);
	}
	Address MOESICache::CalcAddr(MOESICache::AddrTag tag)
	{
		return (Address)(tag * lineSize);
	}
	int MOESICache::CalcSetFromAddr(Address addr)
	{
		return CalcSetFromTag(CalcTag(addr));
	}
	int MOESICache::CalcSetFromTag(MOESICache::AddrTag tag)
	{
		return (int)(tag % setCount);
	}
	MOESICache::BlockState* MOESICache::Lookup(MOESICache::AddrTag tag)
	{
		BlockState* set = GetSet(CalcSetFromTag(tag));
		DebugAssert(set);
		for(int i = 0; i < associativity; i++)
		{
			if(set[i].valid)
			{
				if(set[i].tag == tag)
				{
					return &(set[i]);
				}
			}
		}
		return NULL;
	}
	bool MOESICache::AllocateBlock(MOESICache::AddrTag tag)
	{
		BlockState* set = GetSet(CalcSetFromTag(tag));
		DebugAssert(set);
		for(int i = 0; i < associativity; i++)
		{
			if(set[i].valid == false)
			{
				PrepareFreshBlock(CalcSetFromTag(tag),i,tag);
				return true;
			}
		}
		int eviction = evictionPolicy->Evict(set,associativity);
		if(eviction == InvalidBlock)
		{
			return false;
		}
		if(topCache)
		{
			EvictBlock(set[eviction].tag);
		}
		else
		{
			DebugAssert(pendingEviction.find(set[eviction].tag) == pendingEviction.end());
			pendingEviction[set[eviction].tag] = set[eviction];
			if(pendingInvalidate.find(tag) == pendingInvalidate.end())
			{
				InvalidateMsg* im = EM().CreateInvalidateMsg(ID(),0);
				im->addr = CalcAddr(set[eviction].tag);
				im->size = lineSize;
				localConnection->SendMsg(im,invalidateTime);
			}
		}
		PrepareFreshBlock(CalcSetFromTag(tag),eviction,tag);
		return true;
	}
	void MOESICache::RetryMsg(const BaseMsg* m, int connectionID)
	{
		RecvMsg(m,connectionID);
	}
	void MOESICache::PrepareFreshBlock(int set, int index, AddrTag tag)
	{
		BlockState* s = GetSet(set);
		DebugAssert(s[index].locked == false);
		if(pendingEviction.find(tag) == pendingEviction.end())
		{
			s[index].valid = true;
			s[index].tag = tag;
			s[index].lastWrite = 0;
			s[index].lastRead = 0;
			s[index].state = bs_Invalid;
		}
		else
		{
			s[index] = pendingEviction[tag];
			pendingEviction.erase(tag);
			DebugAssert(s[index].state != bs_Invalid);
		}
		DebugAssert(s[index].locked == false);
	}
	void MOESICache::EvictBlock(MOESICache::AddrTag tag)
	{
		BlockState* b = Lookup(tag);
		DebugAssert(b != NULL);
		DebugAssert(pendingEviction.find(tag) == pendingEviction.end());
		DebugAssert(b->valid);
		DebugAssert(!b->locked);
		DebugAssert(b->state != bs_Invalid);
		EvictionMsg* m = EM().CreateEvictionMsg(ID(),0);
		DebugAssert(m);
		m->addr = CalcAddr(b->tag);
		m->size = lineSize;
		m->blockAttached = (b->lastWrite != 0);
		DebugAssert(remoteConnection);
		remoteConnection->SendMsg(m,evictionTime);
	}
	void MOESICache::PerformRead(const ReadMsg* m)
	{		
		DebugAssert(m);
		AddrTag tag = CalcTag(m->addr);
		BlockState* b = Lookup(tag);
		DebugAssert(b);
		if(b->state == bs_Invalid || (m->requestingExclusive && (b->state == bs_Shared || b->state == bs_Owned)))
		{//miss
			if(!b->locked)
			{
				LockBlock(tag);
				ReadMsg* forward = EM().CreateReadMsg(ID(),m->GeneratingPC());
				forward->addr = CalcAddr(tag);
				forward->size = lineSize;
				forward->alreadyHasBlock = (b->state != bs_Invalid);
				forward->onCompletedCallback = NULL;
				forward->requestingExclusive = m->requestingExclusive;
				remoteConnection->SendMsg(forward, missTime);
			}
			CBPerformRead::FunctionType* f = cbPerformRead.Create();
			f->Initialize(this,m);
			WaitOnBlockUnlock(tag,f);
		}
		else
		{//hit 
			ReadResponseMsg* res = EM().CreateReadResponseMsg(ID(),m->GeneratingPC());
			m->SignalComplete();
			res->addr = m->addr;
			res->size = m->size;
			res->blockAttached = !m->alreadyHasBlock;
			res->exclusiveOwnership = (b->state == bs_Exclusive || b->state == bs_Modified);
			res->satisfied = true;
			res->solicitingMessage = m->MsgID();
			localConnection->SendMsg(res,hitTime);
			EM().DisposeMsg(m);
		}
		b->lastRead = EM().CurrentTick();
	}
	void MOESICache::PerformRemoteRead(const ReadMsg* m)
	{
		DebugAssert(m);
		AddrTag tag = CalcTag(m->addr);
		BlockState* b = Lookup(tag);
		ReadResponseMsg* res = EM().CreateReadResponseMsg(ID(),m->GeneratingPC());
		res->addr = m->addr;
		res->size = m->size;
		m->SignalComplete();
		res->solicitingMessage = m->MsgID();
		if(b == NULL || b->state == bs_Invalid)
		{
			res->exclusiveOwnership = false;
			res->satisfied = false;
			res->blockAttached = false;
		}
		else if(m->requestingExclusive)
		{
			res->blockAttached = !m->alreadyHasBlock;
			res->exclusiveOwnership = true;
			res->satisfied = true;
			if(waitingOnBlockUnlock.find(tag) != waitingOnBlockUnlock.end())
			{
				DebugAssert(b->locked);
				b->state = bs_Invalid;
			}
			else
			{
				DebugAssert(!b->locked);
				InvalidateBlock(*b);
			}
		}
		else
		{
			if(b->state == bs_Modified)
			{
				b->state = bs_Owned;
			}
			else if(b->state == bs_Exclusive)
			{
				b->state = bs_Shared;
			}
			res->exclusiveOwnership = false;
			res->blockAttached = true;
			res->satisfied = true;
		}
		remoteConnection->SendMsg(res,hitTime);
		EM().DisposeMsg(m);
	}
	void MOESICache::PerformWrite(const WriteMsg* m)
	{
		DebugAssert(m);
		AddrTag tag = CalcTag(m->addr);
		BlockState* b = Lookup(tag);
		DebugAssert(b);
		if(b->state == bs_Invalid || b->state == bs_Shared || b->state == bs_Owned)
		{
			if(!b->locked)
			{
				LockBlock(tag);
				ReadMsg* forward = EM().CreateReadMsg(ID(),m->GeneratingPC());
				forward->addr = CalcAddr(tag);
				forward->size = lineSize;
				forward->alreadyHasBlock = (b->state != bs_Invalid);
				forward->onCompletedCallback = NULL;
				forward->requestingExclusive = true;
				remoteConnection->SendMsg(forward, missTime);
			}
			CBPerformWrite::FunctionType* f = cbPerformWrite.Create();
			f->Initialize(this,m);
			WaitOnBlockUnlock(tag,f);
		}
		else
		{
			b->state = bs_Modified;
			WriteResponseMsg* res = EM().CreateWriteResponseMsg(ID(),m->GeneratingPC());
			res->addr = m->addr;
			res->size = m->size;
			res->solicitingMessage = m->MsgID();
			m->SignalComplete();
			localConnection->SendMsg(res,hitTime);
			EM().DisposeMsg(m);
		}
		b->lastWrite = EM().CurrentTick();
	}
	void MOESICache::RespondInvalidate(MOESICache::AddrTag tag)
	{
		DebugAssert(pendingInvalidate.find(tag) != pendingInvalidate.end())
		BlockState* b = Lookup(tag);
		InvalidateResponseMsg* res = EM().CreateInvalidateResponseMsg(ID(),0);
		res->addr = CalcAddr(tag);
		res->size = lineSize;
		res->solicitingMessage = pendingInvalidate[tag]->MsgID();
		if(b && (b->state == bs_Modified || b->state == bs_Owned))
		{
			res->blockAttached = true;
		}
		else if(b == NULL && pendingEviction.find(tag) != pendingEviction.end())
		{
			res->blockAttached = pendingEviction[tag].state == bs_Modified || pendingEviction[tag].state == bs_Owned;
			pendingEviction.erase(tag);
		}
		else
		{
			res->blockAttached = false;
		}
		remoteConnection->SendMsg(res,invalidateTime);
		if(waitingOnBlockUnlock.find(tag) != waitingOnBlockUnlock.end())
		{
			DebugAssert(b);
			DebugAssert(b->locked);
			b->state = bs_Invalid;
			ReadMsg* forward = EM().CreateReadMsg(ID(),pendingInvalidate[tag]->GeneratingPC());
			forward->addr = pendingInvalidate[tag]->addr;
			forward->size = lineSize;
			forward->alreadyHasBlock = false;
			forward->onCompletedCallback = NULL;
			forward->requestingExclusive = false;
			remoteConnection->SendMsg(forward,invalidateTime);
		}
		else if (b)
		{
			InvalidateBlock(*b);
		}
		EM().DisposeMsg(pendingInvalidate[tag]);
		pendingInvalidate.erase(tag);
	}
	void MOESICache::LockBlock(MOESICache::AddrTag tag)
	{
		BlockState* b = Lookup(tag);
		DebugAssert(b);
		DebugAssert(b->valid);
		DebugAssert(!b->locked);
		DebugAssert(waitingOnBlockUnlock.find(tag) == waitingOnBlockUnlock.end());
		b->locked = true;
	}
	void MOESICache::UnlockBlock(MOESICache::AddrTag tag)
	{
		BlockState* b = Lookup(tag);
		DebugAssert(b);
		DebugAssert(b->valid);
		DebugAssert(b->locked);
		DebugAssert(waitingOnBlockUnlock.find(tag) != waitingOnBlockUnlock.end());
		b->locked = false;
		StoredFunctionBase* f = waitingOnBlockUnlock[tag];
		waitingOnBlockUnlock.erase(tag);
		DebugAssert(f);
		f->Call();
		if(!b->locked)
		{
			int setIndex = CalcSetFromTag(tag);
			f = waitingOnSetUnlock[setIndex];
			if(f)
			{
				waitingOnSetUnlock[setIndex] = NULL;
				f->Call();
			}
		}
	}
	void MOESICache::WaitOnBlockUnlock(MOESICache::AddrTag tag, StoredFunctionBase* f)
	{
		DebugAssert(f);
		if(waitingOnBlockUnlock.find(tag) != waitingOnBlockUnlock.end())
		{
			CompositPool::FunctionType* c = compositPool.Create();
			c->Initialize(waitingOnBlockUnlock[tag],f);
			f = c;
		}
		waitingOnBlockUnlock[tag] = f;
	}
	void MOESICache::WaitOnSetUnlock(int s, StoredFunctionBase* f)
	{
		DebugAssert(f);
		if(waitingOnSetUnlock[s])
		{
			CompositPool::FunctionType* c = compositPool.Create();
			c->Initialize(waitingOnSetUnlock[s],f);
			f = c;
		}
		waitingOnSetUnlock[s] = f;
	}
	void MOESICache::WaitOnRemoteReadResponse(MOESICache::AddrTag tag, StoredFunctionBase* f)
	{
		DebugAssert(f);
		if(waitingOnRemoteReads.find(tag) != waitingOnRemoteReads.end())
		{
			CompositPool::FunctionType* c = compositPool.Create();
			c->Initialize(waitingOnRemoteReads[tag],f);
			f = c;
		}
		waitingOnRemoteReads[tag] = f;
	}
	void MOESICache::OnLocalRead(const ReadMsg* m)
	{
		DebugAssert(m);
		AddrTag tag = CalcTag(m->addr);
		BlockState* b = Lookup(tag);
		if(b)
		{//block found
			PerformRead(m);
		}
		else
		{//block not found at all
			if(!AllocateBlock(tag))
			{
				CBRetryMsg::FunctionType* retry = cbRetryMsg.Create();
				retry->Initialize(this,m,localConnectionID);
				WaitOnSetUnlock(CalcSetFromTag(tag),retry);
			}
			else
			{
				PerformRead(m);
			}
		}
	}
	void MOESICache::OnRemoteRead(const ReadMsg* m)
	{
		DebugAssert(m);
		AddrTag tag = CalcTag(m->addr);
		BlockState* b = Lookup(tag);
		if(b)
		{//block found
			if(topCache || b->state == bs_Invalid || (!m->requestingExclusive && (b->state == bs_Shared || b->state == bs_Owned)))
			{//satisfy here
				PerformRemoteRead(m);
			}
			else
			{
				ReadMsg* forward = EM().CreateReadMsg(ID(),m->GeneratingPC());
				forward->addr = m->addr;
				forward->alreadyHasBlock = true;
				forward->onCompletedCallback = NULL;
				forward->requestingExclusive = m->requestingExclusive;
				forward->size = m->size;
				localConnection->SendMsg(forward, hitTime);
				CBPerformRemoteRead::FunctionType* f = cbPerformRemoteRead.Create();
				f->Initialize(this,m);
				WaitOnRemoteReadResponse(tag,f);
			}
		}
		else
		{//cache is inclusive, and thus cannot be satisfied
			PerformRemoteRead(m);
		}
	}
	void MOESICache::OnLocalWrite(const WriteMsg* m)
	{
		DebugAssert(m);
		AddrTag tag = CalcTag(m->addr);
		BlockState* b = Lookup(tag);
		if(b)
		{//block found
			PerformWrite(m);
		}
		else
		{//block not found at all
			if(!AllocateBlock(tag))
			{
				CBRetryMsg::FunctionType* retry = cbRetryMsg.Create();
				retry->Initialize(this,m,localConnectionID);
				WaitOnSetUnlock(CalcSetFromTag(tag),retry);
			}
			else
			{
				PerformWrite(m);
			}
		}
	}
	void MOESICache::OnRemoteWrite(const WriteMsg* m)
	{
		WriteResponseMsg* res = EM().CreateWriteResponseMsg(ID(),m->GeneratingPC());
		res->addr = m->addr;
		res->size = m->size;
		res->solicitingMessage = m->MsgID();
		m->SignalComplete();
		remoteConnection->SendMsg(res,satisfyRequestTime);
		EM().DisposeMsg(m);
	}
	void MOESICache::OnLocalInvalidate(const InvalidateMsg* m)
	{
		EM().DisposeMsg(m);
		DebugFail("Local Invalidates illegal in this cache");
	}
	void MOESICache::OnRemoteInvalidate(const InvalidateMsg* m)
	{
		DebugAssert(m);
		AddrTag tag = CalcTag(m->addr);
		if(pendingInvalidate.find(tag) != pendingInvalidate.end())
		{
			EM().DisposeMsg(m);
			return;
		}
		else
		{
		   pendingInvalidate[tag] = m;
		}
		if(topCache)
		{
			RespondInvalidate(tag);
			return;
		}
		BlockState* b = Lookup(tag);
		if(b)
		{
			localConnection->SendMsg(EM().ReplicateMsg(m),0);
		}
		else
		{
			RespondInvalidate(tag);
		}
	}
	void MOESICache::OnLocalEviction(const EvictionMsg* m)
	{
		DebugAssert(m);
		AddrTag tag = CalcTag(m->addr);
		BlockState* b = Lookup(tag);
		if(b && m->blockAttached)
		{
			if(b->state == bs_Exclusive)
			{
				b->state = bs_Modified;
			}
			else if(b->state == bs_Shared)
			{
				b->state = bs_Owned;
			}
		}
		EvictionResponseMsg* res = EM().CreateEvictionResponseMsg(ID(),m->GeneratingPC());
		res->addr = m->addr;
		res->size = m->size;
		res->solicitingMessage = m->MsgID();
		localConnection->SendMsg(res,hitTime);
		EM().DisposeMsg(m);
	}
	void MOESICache::OnRemoteEviction(const EvictionMsg* m)
	{
		DebugAssert(m);
		EvictionResponseMsg* res = EM().CreateEvictionResponseMsg(ID(),m->GeneratingPC());
		res->addr = m->addr;
		res->size = m->size;
		res->solicitingMessage = m->MsgID();
		remoteConnection->SendMsg(res,hitTime);
		EM().DisposeMsg(m);
	}
	void MOESICache::OnLocalReadResponse(const ReadResponseMsg* m)
	{
		DebugAssert(m);
		AddrTag tag = CalcTag(m->addr);
		if(waitingOnRemoteReads.find(tag) != waitingOnRemoteReads.end())
		{
			StoredFunctionBase* f = waitingOnRemoteReads[tag];
			waitingOnRemoteReads.erase(tag);
			f->Call();
		}
		EM().DisposeMsg(m);
	}
	void MOESICache::OnRemoteReadResponse(const ReadResponseMsg* m)
	{
		DebugAssert(m);
		AddrTag tag = CalcTag(m->addr);
		BlockState* b = Lookup(tag);
		if(b == NULL || !b->locked)
		{
			EM().DisposeMsg(m);
			return;
		}
		if(m->exclusiveOwnership)
		{
			b->state = bs_Exclusive;
		}
		else
		{
			b->state = bs_Shared;
		}
		UnlockBlock(tag);
		EM().DisposeMsg(m);
	}
	void MOESICache::OnLocalWriteResponse(const WriteResponseMsg* m)
	{
		EM().DisposeMsg(m);
		DebugFail("A Write should never have been emitted");
	}
	void MOESICache::OnRemoteWriteResponse(const WriteResponseMsg* m)
	{
		EM().DisposeMsg(m);
	}
	void MOESICache::OnLocalInvalidateResponse(const InvalidateResponseMsg* m)
	{
#ifdef WIN32
      waitingOnBlockUnlock;
      waitingOnSetUnlock;
      waitingOnRemoteReads;
      pendingEviction;
      pendingInvalidate;
#elif defined linux
      #define MEMORY_MOESI_CACHE_ARRAY_SIZE 10
      StoredFunctionBase* waitingOnBlockUnlockArray[MEMORY_MOESI_CACHE_ARRAY_SIZE];
      StoredFunctionBase* waitingOnSetUnlockArray[MEMORY_MOESI_CACHE_ARRAY_SIZE];
      StoredFunctionBase* waitingOnRemoteReadsArray[MEMORY_MOESI_CACHE_ARRAY_SIZE];
      BlockState pendingEvictionArray[MEMORY_MOESI_CACHE_ARRAY_SIZE];
      const InvalidateMsg* pendingInvalidateArray[MEMORY_MOESI_CACHE_ARRAY_SIZE];

      waitingOnBlockUnlock.convertToArray(waitingOnBlockUnlockArray,MEMORY_MOESI_CACHE_ARRAY_SIZE);
      convertVectorToArray<StoredFunctionBase*>(waitingOnSetUnlock,waitingOnSetUnlockArray,MEMORY_MOESI_CACHE_ARRAY_SIZE);
      waitingOnRemoteReads.convertToArray(waitingOnRemoteReadsArray,MEMORY_MOESI_CACHE_ARRAY_SIZE);
      pendingEviction.convertToArray(pendingEvictionArray,MEMORY_MOESI_CACHE_ARRAY_SIZE);
      pendingInvalidate.convertToArray(pendingInvalidateArray,MEMORY_MOESI_CACHE_ARRAY_SIZE);
#endif
		DebugAssert(m);
		AddrTag tag = CalcTag(m->addr);
		DebugAssert(pendingEviction.find(tag) != pendingEviction.end() || pendingInvalidate.find(tag) != pendingInvalidate.end());
		if(m->blockAttached)
		{
			BlockState* b;
			if(pendingEviction.find(tag) != pendingEviction.end())
			{
				b = &(pendingEviction[tag]);
			}
			else if(pendingInvalidate.find(tag) != pendingInvalidate.end())
			{
				b = Lookup(tag);
			}
			if(b->state == bs_Exclusive)
			{
				b->state = bs_Modified;
			}
			else if(b->state == bs_Shared)
			{
				b->state = bs_Owned;
			}
		}
		if(pendingInvalidate.find(tag) != pendingInvalidate.end())
		{
			RespondInvalidate(tag);
		}
		if(pendingEviction.find(tag) != pendingEviction.end())
		{
			EvictionMsg* forward = EM().CreateEvictionMsg(ID(),m->GeneratingPC());
			forward->addr = CalcAddr(CalcTag(m->addr));
			forward->size = lineSize;
			forward->blockAttached = pendingEviction[tag].state == bs_Modified || pendingEviction[tag].state == bs_Owned;
			pendingEviction.erase(tag);
			remoteConnection->SendMsg(forward,evictionTime);
		}
		EM().DisposeMsg(m);
	}
	void MOESICache::OnRemoteInvalidateResponse(const InvalidateResponseMsg* m)
	{
		EM().DisposeMsg(m);
	}
	void MOESICache::OnLocalEvictionResponse(const EvictionResponseMsg* m)
	{
		EM().DisposeMsg(m);
		DebugFail("An Eviction should never have been emitted");
	}
	void MOESICache::OnRemoteEvictionResponse(const EvictionResponseMsg* m)
	{
		EM().DisposeMsg(m);
	}
	MOESICache::~MOESICache()
	{
		if(evictionPolicy)
		{
			delete evictionPolicy;
		}
	}
	void MOESICache::Initialize(EventManager* em, const RootConfigNode& config, const std::vector<Connection*>& connectionSet)
	{
		BaseMemDevice::Initialize(em,config,connectionSet);
		localConnection = NULL;
		remoteConnection = NULL;
		for(size_t i = 0; i < connectionSet.size(); i++)
		{
			if(connectionSet[i]->Name() == "LocalConnection")
			{
				DebugAssert(localConnection == NULL);
				localConnection = connectionSet[i];
				localConnectionID = (int)i;
			}
			else if(connectionSet[i]->Name() == "RemoteConnection")
			{
				DebugAssert(remoteConnection == NULL);
				remoteConnection = connectionSet[i];
				remoteConnectionID = (int)i;
			}
		}
		DebugAssert(localConnection);
		DebugAssert(remoteConnection);
		if(config.ContainsSubNode("HitTime") && config.SubNode("HitTime").IsA<IntConfigNode>())
		{
			hitTime = (TimeDelta)((const IntConfigNode&)config.SubNode("HitTime")).Value();
		}
		else
		{
			hitTime = 1;
		}
		if(config.ContainsSubNode("MissTime") && config.SubNode("MissTime").IsA<IntConfigNode>())
		{
			missTime = (TimeDelta)((const IntConfigNode&)config.SubNode("MissTime")).Value();
		}
		else
		{
			missTime = 1;
		}
		if(config.ContainsSubNode("EvictionTime") && config.SubNode("EvictionTime").IsA<IntConfigNode>())
		{
			evictionTime = (TimeDelta)((const IntConfigNode&)config.SubNode("EvictionTime")).Value();
		}
		else
		{
			evictionTime = 1;
		}
		if(config.ContainsSubNode("InvalidateTime") && config.SubNode("InvalidateTime").IsA<IntConfigNode>())
		{
			invalidateTime = (TimeDelta)((const IntConfigNode&)config.SubNode("InvalidateTime")).Value();
		}
		else
		{
			invalidateTime = 1;
		}
		if(config.ContainsSubNode("CheckAndFailTime") && config.SubNode("CheckAndFailTime").IsA<IntConfigNode>())
		{
			checkAndFailTime = (TimeDelta)((const IntConfigNode&)config.SubNode("CheckAndFailTime")).Value();
		}
		else
		{
			checkAndFailTime = 1;
		}
		if(config.ContainsSubNode("SatisfyRequestTime") && config.SubNode("SatisfyRequestTime").IsA<IntConfigNode>())
		{
			satisfyRequestTime = (TimeDelta)((const IntConfigNode&)config.SubNode("SatisfyRequestTime")).Value();
		}
		else
		{
			satisfyRequestTime = 1;
		}
		DebugAssert(config.ContainsSubNode("Associativity") && config.SubNode("Associativity").IsA<IntConfigNode>());
		DebugAssert(config.ContainsSubNode("SetCount") && config.SubNode("SetCount").IsA<IntConfigNode>());
		DebugAssert(config.ContainsSubNode("TopCache") && config.SubNode("TopCache").IsA<IntConfigNode>());
		DebugAssert(config.ContainsSubNode("LineSize") && config.SubNode("LineSize").IsA<IntConfigNode>());
		associativity = (int)((const IntConfigNode&)config.SubNode("Associativity")).Value();
		setCount = (int)((const IntConfigNode&)config.SubNode("SetCount")).Value();
		topCache = ((const IntConfigNode&)config.SubNode("TopCache")).Value() != 0;
		lineSize = (Address)((const IntConfigNode&)config.SubNode("LineSize")).Value();
		cacheContents.resize(setCount * associativity);
		waitingOnSetUnlock.resize(setCount);
		for(size_t i = 0; i < cacheContents.size(); i++)
		{
			cacheContents[i].valid = false;
			cacheContents[i].tag = 0;
			cacheContents[i].lastRead = cacheContents[i].lastWrite = false;
			cacheContents[i].locked = false;
		}
		for(size_t i = 0; i < waitingOnSetUnlock.size(); i++)
		{
			waitingOnSetUnlock[i] = NULL;
		}
		DebugAssert(config.ContainsSubNode("EvictionPolicy") && config.SubNode("EvictionPolicy").IsA<StringConfigNode>());
		const std::string& evictName= ((const StringConfigNode&)config.SubNode("EvictionPolicy")).Value();
		evictionPolicy = NULL;
		if(evictName == "Random")
		{
			evictionPolicy = new RandomEvictionPolicy();
		}
		else if(evictName == "LRU")
		{
			evictionPolicy = new LRUEvictionPolicy();
		}
		else
		{
			DebugFail("Bad eviction policy specified");
		}
	}
	void MOESICache::DumpRunningState(RootConfigNode& node){}
	void MOESICache::DumpStats(std::ostream& out){}
	void MOESICache::RecvMsg(const BaseMsg* msg, int connectionID)
	{
		if(connectionID == localConnectionID)
		{
			switch(msg->Type())
			{
			case(mt_Read):
         #ifdef MEMORY_MOESI_CACHE_DEBUG
            printDebugInfo("OnLocalRead", *msg, "read");
         #endif
				OnLocalRead((const ReadMsg*)msg);
				break;
			case(mt_Write):
         #ifdef MEMORY_MOESI_CACHE_DEBUG
            printDebugInfo("OnLocalWrite", *msg, "read");
         #endif
				OnLocalWrite((const WriteMsg*)msg);
				break;
			case(mt_Invalidate):
         #ifdef MEMORY_MOESI_CACHE_DEBUG
            printDebugInfo("OnLocalInvalidate", *msg, "read");
         #endif
				OnLocalInvalidate((const InvalidateMsg*)msg);
				break;
			case(mt_Eviction):
         #ifdef MEMORY_MOESI_CACHE_DEBUG
            printDebugInfo("OnLocalEviction", *msg, "read");
         #endif
				OnLocalEviction((const EvictionMsg*)msg);
				break;
			case(mt_ReadResponse):
         #ifdef MEMORY_MOESI_CACHE_DEBUG
            printDebugInfo("OnLocalReadResponse", *msg, "read");
         #endif
				OnLocalReadResponse((const ReadResponseMsg*)msg);
				break;
			case(mt_WriteResponse):
         #ifdef MEMORY_MOESI_CACHE_DEBUG
            printDebugInfo("OnLocalWriteResponse", *msg, "read");
         #endif
				OnLocalWriteResponse((const WriteResponseMsg*)msg);
				break;
			case(mt_InvalidateResponse):
         #ifdef MEMORY_MOESI_CACHE_DEBUG
            printDebugInfo("OnLocalInvalidateResponse", *msg, "read");
         #endif
				OnLocalInvalidateResponse((const InvalidateResponseMsg*)msg);
				break;
			case(mt_EvictionResponse):
         #ifdef MEMORY_MOESI_CACHE_DEBUG
            printDebugInfo("OnLocalEvictionResponse", *msg, "read");
         #endif
				OnLocalEvictionResponse((const EvictionResponseMsg*)msg);
				break;
			default:
				DebugFail("MOESICache::Bad msg Type");
			}
		}
		else if(connectionID == remoteConnectionID)
		{
			switch(msg->Type())
			{
			case(mt_Read):
         #ifdef MEMORY_MOESI_CACHE_DEBUG
            printDebugInfo("OnRemoteRead", *msg, "read");
         #endif
				OnRemoteRead((const ReadMsg*)msg);
				break;
			case(mt_Write):
         #ifdef MEMORY_MOESI_CACHE_DEBUG
            printDebugInfo("OnRemoteWrite", *msg, "read");
         #endif
				OnRemoteWrite((const WriteMsg*)msg);
				break;
			case(mt_Invalidate):
         #ifdef MEMORY_MOESI_CACHE_DEBUG
            printDebugInfo("OnRemoteInvalidate", *msg, "read");
         #endif
				OnRemoteInvalidate((const InvalidateMsg*)msg);
				break;
			case(mt_Eviction):
         #ifdef MEMORY_MOESI_CACHE_DEBUG
            printDebugInfo("OnRemoteEviction", *msg, "read");
         #endif
				OnRemoteEviction((const EvictionMsg*)msg);
				break;
			case(mt_ReadResponse):
         #ifdef MEMORY_MOESI_CACHE_DEBUG
            printDebugInfo("OnRemoteReadResponse", *msg, "read");
         #endif
				OnRemoteReadResponse((const ReadResponseMsg*)msg);
				break;
			case(mt_WriteResponse):
         #ifdef MEMORY_MOESI_CACHE_DEBUG
            printDebugInfo("OnRemoteWriteResponse", *msg, "read");
         #endif
				OnRemoteWriteResponse((const WriteResponseMsg*)msg);
				break;
			case(mt_InvalidateResponse):
         #ifdef MEMORY_MOESI_CACHE_DEBUG
            printDebugInfo("OnRemoteInvalidateResponse", *msg, "read");
         #endif
				OnRemoteInvalidateResponse((const InvalidateResponseMsg*)msg);
				break;
			case(mt_EvictionResponse):
         #ifdef MEMORY_MOESI_CACHE_DEBUG
            printDebugInfo("OnRemoteEvictionResponse", *msg, "read");
         #endif
				OnRemoteEvictionResponse((const EvictionResponseMsg*)msg);
				break;
			default:
				DebugFail("Bad msg Type");
			}
		}
		else
		{
			DebugFail("Connection not a valid ID");
		}
	} // MOESICache::RecvMsg

   void MOESICache::printDebugInfo(const char* fromMethod, const BaseMsg &myMessage, const char* operation)
   {
      printBaseMemDeviceDebugInfo("MOESICache", fromMethod, myMessage, operation);
   }
}
