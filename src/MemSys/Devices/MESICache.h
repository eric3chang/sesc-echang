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
	class MESICache : public BaseMemDevice
	{
	private:
		static const int InvalidBlock = -1;
		typedef Address AddrTag;
		enum MESIState
		{
			bs_Modified,
			//bs_Owned,
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
			MESIState state;
		};
		class EvictionPolicy
		{
		public:
		   // added virtual destructor to eliminate constructor warnings
		   // Eric Chang 2010/07/30
		   virtual ~EvictionPolicy(){}
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
		unsigned int messagesReceived;
		unsigned int readHits;
		unsigned int readMisses;
		unsigned int writeHits;
		unsigned int writeMisses;

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
		HashMap<AddrTag, const InvalidateMsg*> pendingInvalidate;
		HashSet<AddrTag> canceledBlockEviction;

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
		typedef	PooledFunctionGenerator<StoredClassFunction2<MESICache, const BaseMsg*, int, &MESICache::RetryMsg> > CBRetryMsg;
		CBRetryMsg cbRetryMsg;

		void PrepareFreshBlock(int set, int index, AddrTag tag);
		typedef	PooledFunctionGenerator<StoredClassFunction3<MESICache, int, int, AddrTag, &MESICache::PrepareFreshBlock> > CBPrepareFreshBlock;
		CBPrepareFreshBlock cbPrepareFreshBlock;

		void EvictBlock(AddrTag tag);
		typedef	PooledFunctionGenerator<StoredClassFunction1<MESICache, AddrTag, &MESICache::EvictBlock> > CBEvictBlock;
		CBEvictBlock cbEvictBlock;

		void PerformRead(const ReadMsg* m);
		typedef PooledFunctionGenerator<StoredClassFunction1<MESICache, const ReadMsg*, &MESICache::PerformRead> > CBPerformRead;
		CBPerformRead cbPerformRead;

		void PerformRemoteRead(const ReadMsg* m);
		typedef PooledFunctionGenerator<StoredClassFunction1<MESICache, const ReadMsg*, &MESICache::PerformRemoteRead> > CBPerformRemoteRead;
		CBPerformRemoteRead cbPerformRemoteRead;

		void PerformWrite(const WriteMsg* m);
		typedef PooledFunctionGenerator<StoredClassFunction1<MESICache, const WriteMsg*, &MESICache::PerformWrite> > CBPerformWrite;
		CBPerformWrite cbPerformWrite;

		void RespondInvalidate(AddrTag tag);

		void LockBlock(AddrTag tag);
		void UnlockBlock(AddrTag tag);
		void WaitOnBlockUnlock(AddrTag tag, StoredFunctionBase* f);
		void WaitOnSetUnlock(int s, StoredFunctionBase* f);
		void WaitOnRemoteReadResponse(AddrTag tag, StoredFunctionBase* f);

		void OnLocalRead(const ReadMsg* m);
		void OnRemoteRead(const ReadMsg* m);
		void OnLocalWrite(const WriteMsg* m);
		void OnRemoteWrite(const WriteMsg* m);
		void OnLocalInvalidate(const InvalidateMsg* m);
		void OnRemoteInvalidate(const InvalidateMsg* m);
		void OnLocalEviction(const EvictionMsg* m);
		void OnRemoteEviction(const EvictionMsg* m);
		void OnLocalReadResponse(const ReadResponseMsg* m);
		void OnRemoteReadResponse(const ReadResponseMsg* m);
		void OnLocalWriteResponse(const WriteResponseMsg* m);
		void OnRemoteWriteResponse(const WriteResponseMsg* m);
		void OnLocalInvalidateResponse(const InvalidateResponseMsg* m);
		void OnRemoteInvalidateResponse(const InvalidateResponseMsg* m);
		void OnLocalEvictionResponse(const EvictionResponseMsg* m);
		void OnRemoteEvictionResponse(const EvictionResponseMsg* m);

      void printDebugInfo(const char* fromMethod, const BaseMsg &myMessage, const char* operation);
      void printDebugInfo(const char* fromMethod,const AddrTag tag,const char* operation);
	public:
		virtual ~MESICache();
		virtual void Initialize(EventManager* em, const RootConfigNode& config, const std::vector<Connection*>& connectionSet);
		virtual void DumpRunningState(RootConfigNode& node);
		virtual void DumpStats(std::ostream& out);
		virtual void RecvMsg(const BaseMsg* msg, int connectionID);
	};
}
