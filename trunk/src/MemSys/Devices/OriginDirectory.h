#pragma once
#include "BaseMemDevice.h"
#include "../StoredFunctionCall.h"
#include "../HashContainers.h"
#include "NetworkMsg.h"
#include <vector>

using std::string;
using std::stringstream;
using std::vector;

// toggles debug messages
#define MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
//#define MEMORY_ORIGIN_DIRECTORY_DEBUG_DIRECTORY_DATA
//#define MEMORY_ORIGIN_DIRECTORY_DEBUG_COUNTERS
//#define MEMORY_ORIGIN_DIRECTORY_DEBUG_MSG_COUNT
//#define MEMORY_ORIGIN_DIRECTORY_DEBUG_PENDING_DIRECTORY_EXCLUSIVE_READS
//#define MEMORY_ORIGIN_DIRECTORY_DEBUG_PENDING_DIRECTORY_SHARED_READS
//#define MEMORY_ORIGIN_DIRECTORY_DEBUG_PENDING_EVICTION
//#define MEMORY_ORIGIN_DIRECTORY_DEBUG_PENDING_LOCAL_READS
//#define MEMORY_ORIGIN_DIRECTORY_DEBUG_PENDING_REMOTE_INVALIDATES
//#define MEMORY_ORIGIN_DIRECTORY_DEBUG_PENDING_REMOTE_READS

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

		static const int InvalidInvalidAcksReceived;

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

			const BaseMsg* firstReply;
			NodeID firstReplySrc;
			int invalidAcksReceived;
			const ReadMsg* pendingExclusiveRead;
			const ReadMsg* pendingSharedRead;

			CacheData() :
				cacheState(cs_Invalid),
				readRequestState(rrs_NoPendingReads),
				firstReply(NULL),
				firstReplySrc(InvalidNodeID),
				invalidAcksReceived(InvalidInvalidAcksReceived),
				pendingExclusiveRead(NULL),
				pendingSharedRead(NULL)
			{}
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
		class DirectoryData
		{
		public:
			const ReadMsg* firstRequest;
			NodeID firstRequestSrc;
			NodeID owner;
			NodeID previousOwner;
			//int pendingInvalidates;
			HashSet<NodeID> sharers;
			const WritebackRequestMsg* secondRequest;
			NodeID secondRequestSrc;
			vector<LookupData<ReadMsg> >pendingSharedReads;
			DirectoryState state;

         DirectoryData() :
         	firstRequest(NULL),
         	firstRequestSrc(InvalidNodeID),
         	owner(InvalidNodeID),
         	previousOwner(InvalidNodeID),
         	//pendingInvalidates(-1),
         	secondRequest(NULL),
         	secondRequestSrc(InvalidNodeID),
         	state(ds_Unowned)
			{}
         void print(Address myAddress, MessageID myMessageID)
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

		HashMap<MessageID, const ReadMsg* > pendingRemoteReads;
		HashMap<MessageID, const InvalidateMsg* > pendingRemoteInvalidates;
      HashMap<MessageID, const ReadMsg*> pendingMemoryReadAccesses;
      HashMap<MessageID, const WriteMsg*> pendingMemoryWriteAccesses;

		HashMap<Address, const EvictionMsg*> pendingEviction;
		HashMap<Address, DirectoryData> directoryDataMap;
		HashMap<Address, CacheData> cacheDataMap;

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
		
		CacheData& GetCacheData(Address addr);
		CacheData& GetCacheData(const BaseMsg* m);
		DirectoryData& GetDirectoryData(Address addr);
		DirectoryData& GetDirectoryData(const BaseMsg* m);
      void PerformMemoryReadResponseCheck(const ReadResponseMsg *m, NodeID src);
      void PerformMemoryWriteResponseCheck(const WriteResponseMsg *m, NodeID src);

      // these are being used currently
      void SendCacheNak(const ReadMsg* m, NodeID dest);
      void SendDirectoryNak(const ReadMsg* m);
      void SendInvalidateMsg(const ReadMsg* m, NodeID dest, NodeID newOwner);
      void SendMessageToLocalCache(const ReadReplyMsg* msg);
      void SendMessageToRemoteCache(const BaseMsg *msg, NodeID dest);
      void SendMessageToDirectory(BaseMsg *msg);
      void SendMessageToDirectory(BaseMsg *msg, bool isFromMemory);
      void SendMessageToNetwork(const BaseMsg *msg, NodeID dest);
      void SendRequestToMemory(const BaseMsg *msg);

      void ClearTempCacheData(CacheData& cacheData);
      void ClearTempDirectoryData(DirectoryData& directoryData);

      void AutoDetermineDestSendMsg(const BaseMsg* msg, NodeID dest, TimeDelta sendTime,
         OriginDirectoryMemFn func, const char* fromMethod, const char* toMethod);

		void PrintError(const char* fromMethod, const BaseMsg *m);
		void PrintError(const char* fromMethod, const BaseMsg *m, const char* comment);
		void PrintError(const char* fromMethod, const BaseMsg *m, int state);
	   void PutErrorStringStream(stringstream& ss, const char* fromMethod, const BaseMsg*m);

		void PrintDebugInfo(const char* fromMethod, const BaseMsg &myMessage, const char* operation);
		void PrintDebugInfo(const char* fromMethod, const BaseMsg &myMessage, const char* operation,NodeID src);
	   void PrintDebugInfo(const char* fromMethod,Address addr,NodeID id,const char* operation="");
      void PrintDirectoryData(Address myAddress, MessageID myMessageID);
	   void PrintEraseOwner(const char* fromMethod,Address addr,NodeID id,const char* operation);

	   // special treatments
		void OnCacheCacheNak(const CacheNakMsg* m, NodeID src);
		void OnCacheInvalidateAck(const InvalidateAckMsg* m, NodeID src);
	   void OnCacheRead(const ReadMsg* m);
	   void OnCacheReadResponse(const ReadResponseMsg* m, NodeID src);

	   void OnCache(const BaseMsg* msg, NodeID src, CacheData& cacheData);
	   void OnCacheCleanExclusive(const BaseMsg* msg, CacheData& cacheData);
	   void OnCacheDirtyExclusive(const BaseMsg* msg, CacheData& cacheData);
      void OnCacheExclusive(const BaseMsg* msg, NodeID src, CacheData& cacheData);
      void OnCacheExclusiveWaitingForSpeculativeReply(const BaseMsg* msg, NodeID src, CacheData& cacheData);
	   void OnCacheInvalid(const BaseMsg* msg, NodeID src, CacheData& cacheData);
      void OnCacheShared(const BaseMsg* msg, NodeID src, CacheData& cacheData);
      void OnCacheSharedWaitingForSpeculativeReply(const BaseMsg* msg, NodeID src, CacheData& cacheData);
      void OnCacheWaitingForExclusiveReadResponse(const BaseMsg* msg, NodeID src, CacheData& cacheData);
      void OnCacheWaitingForExclusiveResponseAck(const BaseMsg* msg, NodeID src, CacheData& cacheData);
      void OnCacheWaitingForIntervention(const BaseMsg* msg, NodeID src, CacheData& cacheData);
      void OnCacheWaitingForKInvalidatesJInvalidatesReceived(const BaseMsg* msg, NodeID src, CacheData& cacheData);
      void OnCacheWaitingForSharedReadResponse(const BaseMsg* msg, NodeID src, CacheData& cacheData);
      void OnCacheWaitingForSharedResponseAck(const BaseMsg* msg, NodeID src, CacheData& cacheData);
      void OnCacheWaitingForWritebackBusyAck(const BaseMsg* msg, NodeID src, CacheData& cacheData);
      void OnCacheWaitingForWritebackResponse(const BaseMsg* msg, NodeID src, CacheData& cacheData);

	   void OnDirectory(const BaseMsg* msg, NodeID src, bool isFromMemory);
		void OnDirectoryBusyExclusive(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory);
	   void OnDirectoryBusyExclusiveMemoryAccess(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory);
      void OnDirectoryBusyExclusiveMemoryAccessWritebackRequest(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory);
      void OnDirectoryBusyShared(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory);
	   void OnDirectoryBusySharedMemoryAccess(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory);
      void OnDirectoryBusySharedMemoryAccessWritebackRequest(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory);
	   void OnDirectoryExclusive(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory);
		void OnDirectoryExclusiveMemoryAccess(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory);
	   void OnDirectoryShared(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory);
      void OnDirectorySharedExclusiveMemoryAccess(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory);
      void OnDirectorySharedMemoryAccess(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory);
	   void OnDirectoryUnowned(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory);

		void RecvMsgCache(const BaseMsg *msg, NodeID src);
		void RecvMsgDirectory(const BaseMsg *msg, NodeID src, bool isFromMemory);

		void ResendRequestToDirectory(ReadMsg *m);

		typedef PooledFunctionGenerator<StoredClassFunction3<OriginDirectory,const BaseMsg*,NodeID,bool,&OriginDirectory::RecvMsgDirectory> > CBRecvMsgDirectory;
		CBRecvMsgDirectory cbRecvMsgDirectory;
		typedef PooledFunctionGenerator<StoredClassFunction2<OriginDirectory,const BaseMsg*,NodeID,&OriginDirectory::RecvMsgCache> > CBRecvMsgCache;
		CBRecvMsgCache cbRecvMsgCache;
	public:
		virtual void Initialize(EventManager* em, const RootConfigNode& config, const std::vector<Connection*>& connectionSet);
		virtual void DumpRunningState(RootConfigNode& node);
		virtual void DumpStats(std::ostream& out);
		virtual void RecvMsg(const BaseMsg* msg, int connectionID);
	};
}
