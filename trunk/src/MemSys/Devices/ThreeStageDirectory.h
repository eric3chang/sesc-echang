#pragma once
#include "BaseMemDevice.h"
#include "../StoredFunctionCall.h"
#include "../HashContainers.h"
#include "NetworkMsg.h"
#include <vector>

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
   class InterventionCompleteMsg;
	//class NetworkMsg;
	class ThreeStageDirectory : public BaseMemDevice
	{
		typedef Address AddrTag;
		// used for creating function pointers for OnRemote* methods
      typedef void (ThreeStageDirectory::*ThreeStageDirectoryMemFn)(const BaseMsg* msg, NodeID id);

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
         void print(Address myAddress, MessageID myMessageID,
            bool isSharedBusy, bool isExclusiveBusy, bool hasPendingMemAccess, bool isWaitingForInvalidationComplete)
         {
            cout << setw(10) << " ";
            cout << " addr=" << myAddress;
            cout << " msgID=" << myMessageID;
            cout << " own=" << convertDirectoryNetworkIDToDeviceNodeID(owner);
            cout << " sh=";
            for (HashSet<NodeID>::iterator i = sharers.begin(); i != sharers.end(); i++)
            {
               cout << convertDirectoryNetworkIDToDeviceNodeID(*i) << " ";
            }
            cout << " isSharedBusy=" << isSharedBusy
               << " isExclusiveBusy=" << isExclusiveBusy
               << " hasPendingMemAccess=" << hasPendingMemAccess
               << " isWaitForInvCom=" << isWaitingForInvalidationComplete
               << endl;
         }
		};
		template <class T>
		class LookupData
		{
		public:
         LookupData() : previousOwner(InvalidNodeID) {}
			const T* msg;
			NodeID sourceNode;
         NodeID previousOwner;
		};
      class InvalidateData
      {
      public:
         const ReadResponseMsg* msg;
         int count;
         std::vector<LookupData<ReadMsg> > pendingReads;
      };
      class InterventionCompleteData
      {
      public:
         const ReadResponseMsg* rrm;
         const InterventionCompleteMsg* icm;
         InterventionCompleteData() : rrm(NULL), icm(NULL) {}
      };

      unsigned int messagesReceived;

		TimeDelta localSendTime;
		TimeDelta remoteSendTime;
		TimeDelta lookupTime;
		TimeDelta satisfyTime;
		TimeDelta lookupRetryTime;

		NodeIDCalculator* directoryNodeCalc;
		NodeIDCalculator* memoryNodeCalc;

		int localConnectionID;
		int remoteConnectionID;
		NodeID nodeID;

		HashMap<MessageID, const ReadMsg*> pendingLocalReads;
		HashMap<MessageID, LookupData<ReadMsg> > pendingRemoteReads;
		HashMap<MessageID, LookupData<InvalidateMsg> > pendingRemoteInvalidates;
		HashMap<Address, LookupData<ReadMsg> > pendingDirectorySharedReads;
		HashMap<Address, LookupData<ReadMsg> > pendingDirectoryExclusiveReads;
      HashMap<Address, std::vector<LookupData<ReadMsg> > > pendingMainMemAccesses;
      HashSet<MessageID> pendingMemoryWrites;
      //HashSet<Address> pendingIgnoreInterventions;
      HashMap<Address, const ReadResponseMsg* >waitingForEvictionBusyAck;
      HashMap<Address, const ReadMsg*>waitingForEvictionResponse;
      HashMap<Address, InvalidateData> waitingForInvalidates;
      HashMap<MessageID, InterventionCompleteData >waitingForInterventionComplete;
      HashMap<Address, LookupData<ReadMsg> >waitingForInvalidationComplete;
      HashMap<Address, const EvictionMsg* >waitingForReadResponse;
		HashMap<Address, const EvictionMsg*> pendingEviction;
		HashMap<Address, BlockData> directoryData;
		//HashMap<Address, BlockData> pendingDirectoryExclusiveReadsDirectoryData;
      //HashMap<MessageID, LookupData<ReadMsg> > pendingSpeculativeReads;
      //HashMap<Address, const ReadResponseMsg* > pendingSpeculativeReadResponses;
      //HashMap<MessageID, int> unsatisfiedRequests;

      void HandleInterventionComplete(const BaseMsg *msgIn, bool isPendingExclusive);
		//void PerformDirectoryFetch(Address a, NodeID src);
		void PerformDirectoryFetch(const ReadMsg *msgIn, NodeID src);
      void PerformDirectoryFetch(const ReadMsg *msgIn,NodeID src,bool isExclusive,NodeID target);
		//void PerformDirectoryFetchOwner(const ReadMsg *msgIn, NodeID src);
      void SendLocalReadResponse(const ReadResponseMsg *msgIn);
      void SendDirectoryBlockRequest(const ReadMsg *msgIn);
      void SendMemAccessComplete(Address addr, NodeID directoryNode);
		void EraseDirectoryShare(Address a, NodeID id);
		void AddDirectoryShare(Address a, NodeID id, bool exclusive);
      void ChangeOwnerToShare(Address a, NodeID id);
      void writeToMainMemory(const EvictionMsg *m);
      void writeToMainMemory(const InvalidateResponseMsg *m);
      void writeToMainMemory(const ReadResponseMsg *m);
      void writeToMainMemory(Address addr, Address generatingPC, size_t size);

		void OnLocalRead(const BaseMsg* msgIn);
		void OnLocalReadResponse(const BaseMsg* msgIn);
		void OnLocalWrite(const BaseMsg* msgIn);
		void OnLocalEviction(const BaseMsg* msgIn);
		void OnLocalInvalidateResponse(const BaseMsg* msgIn);

		void OnRemoteRead(const BaseMsg* msgIn, NodeID src);
		void OnRemoteReadResponse(const BaseMsg* msgIn, NodeID src);
      //void OnRemoteSpeculativeReadResponse(const BaseMsg* msgIn, NodeID src);
		void OnRemoteWrite(const BaseMsg* msgIn, NodeID src);
		void OnRemoteWriteResponse(const BaseMsg* msgIn, NodeID src);
		void OnRemoteEviction(const BaseMsg* msgIn, NodeID src);
		void OnRemoteEvictionResponse(const BaseMsg* msgIn, NodeID src);
		void OnRemoteEvictionBusyAck(const BaseMsg* msgIn, NodeID src);
		void OnRemoteInvalidate(const BaseMsg* msgIn, NodeID src);
		void OnRemoteInvalidateResponse(const BaseMsg* msgIn, NodeID src);
		void OnRemoteMemAccessComplete(const BaseMsg* msgIn, NodeID src);
      void OnRemoteInterventionComplete(const BaseMsg* msgIn, NodeID src);
      void OnRemoteInvalidationComplete(const BaseMsg* msgIn, NodeID src);

		void OnDirectoryBlockRequest(const BaseMsg* msgIn, NodeID src);
      void OnDirectoryBlockRequestSharedRead(const BaseMsg* msgIn, NodeID src);
      void OnDirectoryBlockRequestExclusiveRead(const BaseMsg* msgIn, NodeID src);
		void OnDirectoryBlockResponse(const BaseMsg* msgIn, NodeID src);

      void AutoDetermineDestSendMsg(const BaseMsg* msg, NodeID dest, TimeDelta sendTime,
         ThreeStageDirectoryMemFn func, const char* fromMethod, const char* toMethod);

		void printDebugInfo(const char* fromMethod, const BaseMsg &myMessage, const char* operation);
		void printDebugInfo(const char* fromMethod, const BaseMsg &myMessage, const char* operation,NodeID src);
	   void printDebugInfo(const char* fromMethod,Address addr,NodeID id,const char* operation="");
	   void printEraseOwner(const char* fromMethod,Address addr,NodeID id,const char* operation);

      void printDirectoryData(Address myAddress, MessageID myMessageID);
		void printPendingDirectorySharedReads();
	   void printPendingLocalReads();

		typedef PooledFunctionGenerator<StoredClassFunction2<ThreeStageDirectory,const BaseMsg*, NodeID, &ThreeStageDirectory::OnDirectoryBlockRequest> > CBOnDirectoryBlockRequest;
		CBOnDirectoryBlockRequest cbOnDirectoryBlockRequest;
      typedef PooledFunctionGenerator<StoredClassFunction2<ThreeStageDirectory,const BaseMsg*, NodeID, &ThreeStageDirectory::OnRemoteRead> > CBOnRemoteRead;
		CBOnRemoteRead cbOnRemoteRead;
      typedef PooledFunctionGenerator<StoredClassFunction2<ThreeStageDirectory,Address, NodeID, &ThreeStageDirectory::SendMemAccessComplete> > CBSendMemAccessComplete;
		CBSendMemAccessComplete cbSendMemAccessComplete;

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
