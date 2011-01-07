#pragma once
#include "BaseMemDevice.h"
#include "../StoredFunctionCall.h"
#include "../HashContainers.h"
#include "NetworkMsg.h"
#include <vector>

// toggles debug messages
//#define MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
//#define MEMORY_3_STAGE_DIRECTORY_DEBUG_DIRECTORY_DATA
//#define MEMORY_3_STAGE_DIRECTORY_DEBUG_COUNTERS
//#define MEMORY_3_STAGE_DIRECTORY_DEBUG_MSG_COUNT
//#define MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_DIRECTORY_EXCLUSIVE_READS
//#define MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_DIRECTORY_SHARED_READS
//#define MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_EVICTION
//#define MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_LOCAL_READS
//#define MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_REMOTE_INVALIDATES
//#define MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_REMOTE_READS

namespace Memory
{
	class ReadMsg;
	class WriteMsg;
	class InvalidateMsg;
	class EvictionMsg;
	class ReadResponseMsg;
	class WriteResponseMsg;
	class InvalidateResponseMsg;
	class EvictionResponseMsg;

	class OriginDirectory : public BaseMemDevice
	{
		typedef Address AddrTag;
		// used for creating function pointers for OnRemote* methods
      typedef void (OriginDirectory::*OriginDirectoryMemFn)(const BaseMsg* msg, NodeID id);

		class NodeIDCalculator
		{
		public:
			virtual NodeID CalcNodeID(Address addr) const = 0;
			virtual void Initialize(const RootConfigNode& node) = 0;
			virtual ~NodeIDCalculator() {}
		};
		class HashedPageCalculator : public NodeIDCalculator
		{
			Address pageSize;
			int elementCount;
			std::vector<NodeID> nodeSet;
		public:
			virtual NodeID CalcNodeID(Address addr) const;
			virtual void Initialize(const RootConfigNode& node);
		};
		enum CacheState
		{
			cs_Exclusive,
			cs_ExclusiveWaitingForSpeculativeReply,
			cs_Invalid,
			cs_Shared,
			cs_SharedWaitingForSpeculativeReply,
			cs_WaitingForIntervention,
			cs_WaitingForExclusiveReadResponse,
			cs_WaitingForExclusiveResponseAck,
			cs_WaitingForKInvalidatesJInvalidatesReceived,
			cs_WaitingForSharedReadResponse,
			cs_WaitingForSharedResponseAck,
			cs_WaitingForWritebackBusyAck,
			cs_WaitingForWritebackResponse
		};
		enum ReadRequestState
		{
			rrs_NoPendingReads,
			rrs_PendingExclusiveRead,
			rrs_PendingSharedRead,
			rrs_PendingSharedReadExclusiveRead
		};
		enum DirectoryState
		{
			ds_BusyExclusive,
			ds_BusyExclusiveMemoryAccess,
			ds_BusyExclusiveMemoryAccessWritebackRequest,
			ds_BusyShared,
			ds_BusySharedMemoryAccess,
			ds_BusySharedMemoryAccessWritebackRequest,
			ds_Exclusive,
			ds_ExclusiveMemoryAccess,
			ds_Shared,
			ds_SharedMemoryAccess,
			ds_SharedExclusiveMemoryAccess,
			ds_SharedMemoryAccessWritebackRequest,
			ds_Unowned
		};
		class CacheData
		{
		public:
			CacheState cacheState;
			ReadRequestState readRequestState;

			const ReadMsg* firstReply;
			NodeID firstReplySrc;
			int invalidAcksReceived;
			const ReadMsg* pendingExclusiveRead;
			const ReadMsg* pendingSharedRead;

			CacheData() :
				cacheState(cs_Invalid),
				readRequestState(rrs_NoPendingReads),
				firstReply(NULL),
				firstReplySrc(InvalidNodeID),
				invalidAcksReceived(-1),
				pendingExclusiveRead(NULL),
				pendingSharedRead(NULL)
			{}
		};
		class DirectoryData
		{
		public:
			const BaseMsg* firstRequest;
			NodeID owner;
			int pendingInvalidates;
			HashSet<NodeID> sharers;
			DirectoryState state;

         DirectoryData() :
         	firstRequest(NULL),
         	owner(InvalidNodeID),
         	pendingInvalidates(-1),
         	state(ds_Unowned)
			{}
         void print(Address myAddress, MessageID myMessageID,bool isSharedBusy, bool isExclusiveBusy,
            bool hasPendingMemAccess,bool isWaitingForReadResponse)
         {
            cout << setw(10) << " ";
            cout << " addr=" << myAddress;
            cout << " msgID=" << myMessageID;
            cout << " own=" << BaseMsg::convertNodeIDToDeviceID(owner);
            cout << " sh=";
            for (HashSet<NodeID>::iterator i = sharers.begin(); i != sharers.end(); i++)
            {
               cout << BaseMsg::convertNodeIDToDeviceID(*i) << " ";
            }
            cout << " isShBusy=" << isSharedBusy
               << " isExBusy=" << isExclusiveBusy
               << " pendMemAccess=" << hasPendingMemAccess
               << " waitForReadRes=" << isWaitingForReadResponse
               << endl;
         }
		};
		template <class T>
		class LookupData
		{
		public:
         LookupData() : msg(NULL), sourceNode(InvalidNodeID), previousOwner(InvalidNodeID) {}
			const T* msg;
			NodeID sourceNode;
         NodeID previousOwner;
         void print() const
         {
         	std::cout << " srcNode=" << sourceNode
         			<< " prevOwn=" << previousOwner;
         }
		};
      class InvalidateData
      {
      public:
         const ReadResponseMsg* msg;
         int invalidatesReceived;
         int invalidatesWaitingFor;
         std::vector<LookupData<ReadMsg> > pendingReads;
         InvalidateData() : msg(NULL), invalidatesReceived(0), invalidatesWaitingFor(0) {}
      };
      class ReversePendingLocalReadData
      {
      public:
         HashMap<MessageID,const ReadMsg*>exclusiveRead;
         HashMap<MessageID,const ReadMsg*>sharedRead;
         bool isExclusiveReadSatisfiedByEviction;
         bool isSharedReadSatisfiedByEviction;
         EvictionMsg *myEvictionMsg;
         ReversePendingLocalReadData() : isExclusiveReadSatisfiedByEviction(false),isSharedReadSatisfiedByEviction(false)
            ,myEvictionMsg(NULL) {}
      };

      typedef HashMultiMap<Address,LookupData<ReadMsg> > AddrLookupHashMultiMap;
      typedef std::pair<Address,LookupData<ReadMsg> > AddrLookupPair;
      typedef std::pair<AddrLookupHashMultiMap::iterator,AddrLookupHashMultiMap::iterator> AddrLookupIteratorPair;

      unsigned int messagesReceived;

		TimeDelta localSendTime;
		TimeDelta remoteSendTime;
		TimeDelta lookupTime;
		TimeDelta satisfyTime;
		TimeDelta lookupRetryTime;

		NodeIDCalculator* directoryNodeCalc;

		int localCacheConnectionID;
		int localMemoryConnectionID;
		int remoteConnectionID;
		NodeID nodeID;

		HashMap<MessageID, const ReadMsg*> pendingLocalReads;
      HashMap<Address, ReversePendingLocalReadData>reversePendingLocalReads;
		HashMap<MessageID, LookupData<ReadMsg> > pendingRemoteReads;
		HashMap<MessageID, LookupData<InvalidateMsg> > pendingRemoteInvalidates;
		HashMap<Address, LookupData<ReadMsg> > pendingDirectoryBusySharedReads;
		HashMultiMap<Address, LookupData<ReadMsg> > pendingDirectoryNormalSharedReads;
		HashMap<Address, LookupData<ReadMsg> > pendingDirectoryBusyExclusiveReads;
      HashMap<Address, std::vector<LookupData<ReadMsg> > > pendingMainMemAccesses;
      HashSet<MessageID> pendingMemoryWrites;
      //HashSet<Address> pendingIgnoreInterventions;
      HashMap<Address, const ReadResponseMsg* >waitingForEvictionBusyAck;
      HashMap<Address, const ReadMsg*>waitingForEvictionResponse;
      HashMap<Address, InvalidateData> waitingForInvalidates;
      HashMap<Address, const EvictionMsg* >waitingForReadResponse;
		HashMap<Address, const EvictionMsg*> pendingEviction;
      HashMap<Address, std::vector<ReadMsg> >invalidateLock;
		HashMap<Address, DirectoryData> directoryDataMap;
		HashMap<Address, CacheData> cacheDataMap;
		//HashMap<Address, BlockData> pendingDirectoryBusyExclusiveReadsDirectoryData;
      //HashMap<MessageID, LookupData<ReadMsg> > pendingSpeculativeReads;
      //HashMap<Address, const ReadResponseMsg* > pendingSpeculativeReadResponses;
      //HashMap<MessageID, int> unsatisfiedRequests;

		void dump(HashMap<Memory::MessageID, const Memory::BaseMsg*> &m);
		void dump(HashMap<Address,LookupData<BaseMsg> > &m);
		// debugging functions
		template <class Key>
		static void DumpLookupDataTemplate(HashMap<Key,LookupData<Memory::BaseMsg> > &m)
		{
			typename HashMap<Key,LookupData<Memory::BaseMsg> >::const_iterator i,j;
			i = m.begin();
			j = m.end();

			std::cout << "[" << "dump" << "]" << std::endl;
			for(; i != j; ++i)
			{
				std::cout << '[' << i->first << "]";
				const LookupData<Memory::BaseMsg> &ld = i->second;
				ld.print();
				std::cout << std::endl;
				const Memory::BaseMsg* m = ld.msg;
				m->print(0);
				std::cout << std::endl;
			}
		}

		CacheData& GetCacheData(const BaseMsg* m);
		DirectoryData& GetDirectoryData(const BaseMsg* m);
      void HandleReceivedAllInvalidates(Address myAddress);
      bool IsInPendingDirectoryNormalSharedRead(const ReadMsg *m);
		//void PerformDirectoryFetch(Address a, NodeID src);
		void PerformDirectoryFetch(const ReadMsg *msgIn, NodeID src);
      void PerformDirectoryFetch(const ReadMsg *msgIn,NodeID src,bool isExclusive,NodeID target);
		//void PerformDirectoryFetchOwner(const ReadMsg *msgIn, NodeID src);
      void SendLocalReadResponse(const ReadResponseMsg *msgIn);
      void SendDirectoryBlockRequest(const ReadMsg *msgIn);
      void SendMemoryRequest(const BaseMsg *msg);
      void SendNetworkMessage(const BaseMsg *msg, NodeID dest);
      void SendRemoteEviction(const EvictionMsg *m,NodeID dest,const char *fromMethod);
      void SendRemoteRead(const ReadMsg *m,NodeID dest,const char *fromMethod);
      void EraseDirectoryShare(Address a, NodeID id);
		void AddDirectoryShare(Address a, NodeID id, bool exclusive);
      void AddReversePendingLocalRead(const ReadMsg *m);
      void EraseReversePendingLocalRead(const ReadResponseMsg *m,const ReadMsg *ref);
      void AddPendingDirectoryNormalSharedRead(const ReadMsg *m, NodeID src);
      void ErasePendingDirectoryNormalSharedRead(const ReadResponseMsg *m);
      void ErasePendingLocalRead(const ReadResponseMsg *m);
      void ChangeOwnerToShare(Address a, NodeID id);

      void AutoDetermineDestSendMsg(const BaseMsg* msg, NodeID dest, TimeDelta sendTime,
         OriginDirectoryMemFn func, const char* fromMethod, const char* toMethod);

		void PrintError(const char* fromMethod, const BaseMsg *m);
		void PrintDebugInfo(const char* fromMethod, const BaseMsg &myMessage, const char* operation);
		void PrintDebugInfo(const char* fromMethod, const BaseMsg &myMessage, const char* operation,NodeID src);
	   void PrintDebugInfo(const char* fromMethod,Address addr,NodeID id,const char* operation="");
	   void PrintEraseOwner(const char* fromMethod,Address addr,NodeID id,const char* operation);

      void PrintDirectoryData(Address myAddress, MessageID myMessageID);
		void PrintPendingDirectoryBusySharedReads();
	   void PrintPendingLocalReads();

	   void OnDirectoryShared(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory);
	   void OnDirectoryUnowned(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory);
	   /*
		typedef PooledFunctionGenerator<StoredClassFunction2<OriginDirectory,const BaseMsg*, NodeID, &OriginDirectory::OnDirectoryBlockRequest> > CBOnDirectoryBlockRequest;
		CBOnDirectoryBlockRequest cbOnDirectoryBlockRequest;
      typedef PooledFunctionGenerator<StoredClassFunction2<OriginDirectory,const BaseMsg*, NodeID, &OriginDirectory::OnRemoteEviction> > CBOnRemoteEviction;
		CBOnRemoteEviction cbOnRemoteEviction;
      typedef PooledFunctionGenerator<StoredClassFunction2<OriginDirectory,const BaseMsg*, NodeID, &OriginDirectory::OnRemoteRead> > CBOnRemoteRead;
		CBOnRemoteRead cbOnRemoteRead;
		*/
		// 2010/08/05 Eric: seems to be unused
		/*
		typedef PooledFunctionGenerator<StoredClassFunction2<Directory,const ReadResponseMsg*, NodeID, &Directory::OnDirectoryBlockResponse> > CBOnDirectoryBlockResponse;
		CBOnDirectoryBlockResponse cbOnDirectoryBlockResponse;
		typedef PooledFunctionGenerator<StoredClassFunction2<Directory,const ReadResponseMsg*, NodeID, &Directory::OnRemoteReadResponse> > CBOnRemoteReadResponse;
		CBOnRemoteReadResponse cbOnRemoteReadResponse;
		*/
	public:
		virtual void Initialize(EventManager* em, const RootConfigNode& config, const std::vector<Connection*>& connectionSet);
		virtual void DumpRunningState(RootConfigNode& node);
		virtual void DumpStats(std::ostream& out);
		virtual void RecvMsg(const BaseMsg* msg, int connectionID);
	};
}
