#pragma once
#include "BaseMemDevice.h"
#include "../StoredFunctionCall.h"
#include "../HashContainers.h"

/* added this to remove complaints about forward declaration and
 * possible problems with invocation of delete operator 2010-06-09
 */
#include "ReadMsg.h"
#include "WriteMsg.h"
#include "InvalidateMsg.h"
#include "EvictionMsg.h"
#include "ReadResponseMsg.h"
#include "WriteResponseMsg.h"
#include "InvalidateResponseMsg.h"
#include "EvictionResponseMsg.h"

namespace Memory
{
/*
	class ReadMsg;
	class WriteMsg;
	class InvalidateMsg;
	class EvictionMsg;
	class ReadResponseMsg;
	class WriteResponseMsg;
	class InvalidateResponseMsg;
	class EvictionResponseMsg;
	*/
	class MOESICache : public BaseMemDevice
	{
		static const int InvalidBlock = -1;
		typedef Address AddrTag;
		enum MOESIState
		{
			bs_Modified,
			bs_Owned,
			bs_Exclusive,
			bs_Shared,
			bs_Invalid,
		};
		class BlockState
		{
		public:
			TickTime lastRead;
			TickTime lastWrite;
			bool locked;
			bool valid;
			AddrTag tag;
			MOESIState state;
		};
		class EvictionPolicy
		{
		public:
			virtual int Evict(BlockState* set, int setSize) = 0;
		};
		class RandomEvictionPolicy : public EvictionPolicy
		{
		public:
			virtual int Evict(BlockState* set, int setSize);
		};
		class LRUEvictionPolicy : public EvictionPolicy
		{
		public:
			virtual int Evict(BlockState* set, int setSize);
		};
		TimeDelta hitTime;
		TimeDelta missTime;
		TimeDelta evictionTime;
		TimeDelta invalidateTime;
		TimeDelta checkAndFailTime;
		TimeDelta satisfyRequestTime;//time from recieving a block on miss, and forwarding recieved value
		EvictionPolicy* evictionPolicy;

		Connection* localConnection;
		Connection* remoteConnection;
		int localConnectionID;
		int remoteConnectionID;

		std::vector<BlockState> cacheContents;
		int associativity;
		int setCount;
		Address lineSize;
		bool topCache;

		HashMap<AddrTag, StoredFunctionBase*> waitingOnBlockUnlock;
		std::vector<StoredFunctionBase*> waitingOnSetUnlock;
		HashMap<AddrTag, StoredFunctionBase*> waitingOnRemoteReads;

		HashMap<AddrTag, BlockState> pendingEviction;
		HashMap<AddrTag, InvalidateMsg*> pendingInvalidate;

		BlockState* GetSet(int i);
		void InvalidateBlock(BlockState& block);
		AddrTag CalcTag(Address addr);
		Address CalcAddr(AddrTag tag);
		int CalcSetFromAddr(Address addr);
		int CalcSetFromTag(AddrTag tag);
		BlockState* Lookup(AddrTag tag);

		typedef PooledFunctionGenerator<CompositStoredFunction> CompositPool;
		CompositPool compositPool;

		bool AllocateBlock(AddrTag tag);

		void RetryMsg(const BaseMsg* m, int connectionID);
		typedef	PooledFunctionGenerator<StoredClassFunction2<MOESICache, const BaseMsg*, int, &MOESICache::RetryMsg> > CBRetryMsg;
		CBRetryMsg cbRetryMsg;

		void PrepareFreshBlock(int set, int index, AddrTag tag);
		typedef	PooledFunctionGenerator<StoredClassFunction3<MOESICache, int, int, AddrTag, &MOESICache::PrepareFreshBlock> > CBPrepareFreshBlock;
		CBPrepareFreshBlock cbPrepareFreshBlock;

		void EvictBlock(AddrTag tag);
		typedef	PooledFunctionGenerator<StoredClassFunction1<MOESICache, AddrTag, &MOESICache::EvictBlock> > CBEvictBlock;
		CBEvictBlock cbEvictBlock;

		void PerformRead(const ReadMsg* m);
		typedef PooledFunctionGenerator<StoredClassFunction1<MOESICache, const ReadMsg*, &MOESICache::PerformRead> > CBPerformRead;
		CBPerformRead cbPerformRead;

		void PerformRemoteRead(const ReadMsg* m);
		typedef PooledFunctionGenerator<StoredClassFunction1<MOESICache, const ReadMsg*, &MOESICache::PerformRemoteRead> > CBPerformRemoteRead;
		CBPerformRemoteRead cbPerformRemoteRead;

		void PerformWrite(const WriteMsg* m);
		typedef PooledFunctionGenerator<StoredClassFunction1<MOESICache, const WriteMsg*, &MOESICache::PerformWrite> > CBPerformWrite;
		CBPerformWrite cbPerformWrite;

		void RespondInvalidate(AddrTag tag);

		void LockBlock(AddrTag tag);
		void UnlockBlock(AddrTag tag);
		void WaitOnBlockUnlock(AddrTag tag, StoredFunctionBase* f);
		void WaitOnSetUnlock(int s, StoredFunctionBase* f);
		void WaitOnRemoteReadResponse(AddrTag tag, StoredFunctionBase* f);

		void OnLocalRead(ReadMsg* m);
		void OnRemoteRead(ReadMsg* m);
		void OnLocalWrite(WriteMsg* m);
		void OnRemoteWrite(WriteMsg* m);
		void OnLocalInvalidate(InvalidateMsg* m);
		void OnRemoteInvalidate(InvalidateMsg* m);
		void OnLocalEviction(EvictionMsg* m);
		void OnRemoteEviction(EvictionMsg* m);
		void OnLocalReadResponse(ReadResponseMsg* m);
		void OnRemoteReadResponse(ReadResponseMsg* m);
		void OnLocalWriteResponse(WriteResponseMsg* m);
		void OnRemoteWriteResponse(WriteResponseMsg* m);
		void OnLocalInvalidateResponse(InvalidateResponseMsg* m);
		void OnRemoteInvalidateResponse(InvalidateResponseMsg* m);
		void OnLocalEvictionResponse(EvictionResponseMsg* m);
		void OnRemoteEvictionResponse(EvictionResponseMsg* m);
	public:
		virtual ~MOESICache();
		virtual void Initialize(EventManager* em, const RootConfigNode& config, const std::vector<Connection*>& connectionSet);
		virtual void DumpRunningState(RootConfigNode& node);
		virtual void DumpStats(std::ostream& out);
		virtual void RecvMsg(const BaseMsg* msg, int connectionID);
	};
}
