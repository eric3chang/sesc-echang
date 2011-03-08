#pragma once
#include "BaseMemDevice.h"
#include "../StoredFunctionCall.h"
#include "../HashContainers.h"
#include "NetworkMsg.h"
#include <vector>

using std::pair;
using std::stringstream;
using std::vector;

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
	//class NetworkMsg;
	class BIPDirectory : public BaseMemDevice
	{
		typedef Address AddrTag;
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
		class BlockData
		{
		public:
			HashSet<NodeID> sharers;
			NodeID owner;

         BlockData() : owner(InvalidNodeID) {}
         void print(Address myAddress, MessageID myMessageID,bool isSharedBusy, bool isExclusiveBusy)
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
               << endl;
         }
		};
		template <class T>
		class LookupData
		{
		public:
			const T* msg;
			NodeID sourceNode;
			int invalidateResponsesExpected;
			int invalidateResponsesReceived;
			LookupData() :
				invalidateResponsesExpected(0),
				invalidateResponsesReceived(0)
			{}
		};

		unsigned long long messagesReceived;
		unsigned long long directoryRequestsReceived;
		unsigned long long directoryResponsesReceived;
		unsigned long long localEvictionsReceived;
		unsigned long long localInvalidateResponsesReceived;
		unsigned long long localReadsReceived;
		unsigned long long localReadResponsesReceived;
		unsigned long long localWritesReceived;
		unsigned long long remoteEvictionsReceived;
		unsigned long long remoteEvictionResponsesReceived;
		unsigned long long remoteInvalidatesReceived;
		unsigned long long remoteInvalidateResponsesReceived;
		unsigned long long remoteReadsReceived;
		unsigned long long remoteReadResponsesReceived;
		unsigned long long remoteWritesReceived;
		unsigned long long remoteWriteResponsesReceived;

		TimeDelta localSendTime;
		TimeDelta remoteSendTime;
		TimeDelta lookupTime;
		TimeDelta satisfyTime;
		TimeDelta lookupRetryTime;

		NodeIDCalculator* directoryNodeCalc;
		//NodeIDCalculator* memoryNodeCalc;

		int localCacheConnectionID;
		int localMemoryConnectionID;
		int remoteConnectionID;
		NodeID nodeID;

		typedef HashMultiMap<Address, const ReadMsg*> AddrReadMultimap;
		typedef HashMap<Address, const ReadMsg*> AddrReadMap;
		typedef pair<Address,const ReadMsg*> AddrReadPair;
		typedef pair<AddrReadMultimap::iterator,AddrReadMultimap::iterator> AddrReadMultimapPairii;
		typedef HashMultiMap<Address,LookupData<ReadMsg> > AddrLDReadMultimap;
		typedef pair<Address,LookupData<ReadMsg> > AddrLDReadPair;
		typedef pair<AddrLDReadMultimap::iterator,AddrLDReadMultimap::iterator> AddrLDReadMultimapPairii;
		typedef HashMap<Address,LookupData<ReadMsg> > AddrLDReadMap;
		typedef HashMap<MessageID,const ReadMsg*> MessageReadMap;
		typedef pair<MessageID,const ReadMsg*> MessageReadPair;

		HashMap<MessageID, const ReadMsg*> pendingLocalReads;
		HashMap<MessageID, LookupData<ReadMsg> > pendingRemoteReads;
		HashMap<MessageID, LookupData<InvalidateMsg> > pendingRemoteInvalidates;
		HashMultiMap<Address, LookupData<ReadMsg> > pendingDirectorySharedReads;
		HashMap<Address, LookupData<ReadMsg> > pendingDirectoryExclusiveReads;
		HashSet<Address> pendingEviction;
		HashMap<MessageID, const ReadMsg*> pendingMemoryReadAccesses;
      HashMap<MessageID, const EvictionMsg*> pendingMemoryEvictionAccesses;
      HashMap<MessageID, const WriteMsg*> pendingMemoryWriteAccesses;
		AddrReadMultimap reversePendingLocalReads;
		HashMap<Address, BlockData> directoryData;

		// debug functions
      void dump(HashMap<MessageID, const BaseMsg*> &m);
      void dump(MessageReadMap &m);
      void dump(AddrLDReadMultimap &m);
      void dump(AddrLDReadMap &m);
      void dump(AddrReadMultimap &m);
      void dumpPendingLocalReads();
      void dumpPendingDirectorySharedReads();
      void dumpPendingDirectoryExclusiveReads();
      void dumpReversePendingLocalReads();

		void PerformDirectoryFetch(Address a);
		void EraseDirectoryShare(Address a, NodeID id);
		void AddDirectoryShare(Address a, NodeID id, bool exclusive);

		// local cache methods
		void OnLocalRead(const ReadMsg* m);
		void OnLocalReadResponse(const ReadResponseMsg* m);
		void OnLocalWrite(const WriteMsg* m);
		void OnLocalEviction(const EvictionMsg* m);
		void OnLocalInvalidateResponse(const InvalidateResponseMsg* m);

		// local memory methods
		void OnLocalMemoryReadResponse(const ReadResponseMsg* m);
		void OnLocalMemoryWriteResponse(const WriteResponseMsg* m);
		//void OnLocalMemoryEvictionResponse(const EvictionResponseMsg* m);

		void OnRemoteDirectoryNak(const DirectoryNakMsg* m, NodeID src);
		void OnRemoteReadCache(const ReadMsg* m, NodeID src);
		//void OnRemoteReadMemory(const ReadMsg* m, NodeID src);
		void OnRemoteReadResponse(const ReadResponseMsg* m, NodeID src);
		void OnRemoteWrite(const WriteMsg* m, NodeID src);
		void OnRemoteWriteResponse(const WriteResponseMsg* m, NodeID src);
		void OnRemoteEviction(const EvictionMsg* m, NodeID src);
		void OnRemoteEvictionResponse(const EvictionResponseMsg* m, NodeID src);
		void OnRemoteInvalidate(const InvalidateMsg* m, NodeID src);
		void OnRemoteInvalidateResponse(const InvalidateResponseMsg* m, NodeID src);

		void OnDirectoryBlockRequest(const ReadMsg* m, NodeID src);
		void OnDirectoryBlockResponse(const ReadResponseMsg* m, NodeID src);

		void SendDirectoryNak(const DirectoryNakMsg *m);
		void SendDirectoryNak(const InvalidateMsg *m);
		void SendDirectoryNak(const ReadMsg *m);
		void SendDirectoryNak(const ReadResponseMsg *m);
      void SendMessageToNetwork(const BaseMsg *msg, NodeID dest);
      void SendReadRequestToMemory(const ReadMsg *msg);
		void SendWriteRequestToMemory(const WriteMsg *msg);

		void PrintDebugInfo(const char* fromMethod, const BaseMsg &myMessage, const char* operation);
		void PrintDebugInfo(const char* fromMethod, const BaseMsg &myMessage, const char* operation,NodeID src);
	   void PrintDebugInfo(const char* fromMethod,Address addr,NodeID id,const char* operation);
      void PrintDirectoryData(Address myAddress, MessageID myMessageID);
	   void PrintEraseOwner(const char* fromMethod,Address addr,NodeID id,const char* operation);
		void PrintPendingDirectorySharedReads();
	   void PrintPendingLocalReads();

		void PrintError(const char* fromMethod, const BaseMsg *m);
		void PrintError(const char* fromMethod, const BaseMsg *m, const char* comment);
	   void PutErrorStringStream(stringstream& ss, const char* fromMethod, const BaseMsg*m);

		typedef PooledFunctionGenerator<StoredClassFunction2<BIPDirectory,const ReadMsg*, NodeID, &BIPDirectory::OnDirectoryBlockRequest> > CBOnDirectoryBlockRequest;
		CBOnDirectoryBlockRequest cbOnDirectoryBlockRequest;
		typedef PooledFunctionGenerator<StoredClassFunction2<BIPDirectory,const DirectoryNakMsg*, NodeID, &BIPDirectory::OnRemoteDirectoryNak> > CBOnRemoteDirectoryNak;
		CBOnRemoteDirectoryNak cbOnRemoteDirectoryNak;
		// 2010/08/05 Eric: seems to be unused
		/*
		typedef PooledFunctionGenerator<StoredClassFunction2<BIPDirectory,const ReadResponseMsg*, NodeID, &BIPDirectory::OnDirectoryBlockResponse> > CBOnDirectoryBlockResponse;
		CBOnDirectoryBlockResponse cbOnDirectoryBlockResponse;
		typedef PooledFunctionGenerator<StoredClassFunction2<BIPDirectory,const ReadResponseMsg*, NodeID, &BIPDirectory::OnRemoteReadResponse> > CBOnRemoteReadResponse;
		CBOnRemoteReadResponse cbOnRemoteReadResponse;
		*/
	public:
		virtual void Initialize(EventManager* em, const RootConfigNode& config, const std::vector<Connection*>& connectionSet);
		virtual void DumpRunningState(RootConfigNode& node);
		virtual void DumpStats(std::ostream& out);
		virtual void RecvMsg(const BaseMsg* msg, int connectionID);
	};
}
