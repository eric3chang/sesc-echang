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
	class InvalidateSharerMsg;
	class EvictionMsg;
	class ReadResponseMsg;
	class WriteResponseMsg;
	class InvalidateResponseMsg;
	class EvictionResponseMsg;
	//class NetworkMsg;
	class ThreeStageDirectory : public BaseMemDevice
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

			BlockData()
			{
				owner = InvalidNodeID;
			}
		};
		template <class T>
		class LookupData
		{
		public:
			const T* msg;
			NodeID sourceNode;
		};

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
		HashSet<Address> pendingEviction;
		HashMap<Address, BlockData> directoryData;
		HashMap<Address, BlockData> pendingDirectoryExclusiveReadsDirectoryData;
      HashMap<MessageID, int> unsatisfiedRequests;

		void PerformDirectoryFetch(Address a, NodeID src);
		void PerformDirectoryFetch(const ReadMsg *msgIn, NodeID src);
      void PerformDirectoryFetch(const ReadMsg *msgIn,NodeID src,bool isExclusive,NodeID target);
		void PerformDirectoryFetchOwner(const ReadMsg *msgIn, NodeID src);
		void EraseDirectoryShare(Address a, NodeID id);
		void AddDirectoryShare(Address a, NodeID id, bool exclusive);

		void OnLocalRead(const ReadMsg* m);
		void OnLocalReadResponse(const ReadResponseMsg* m);
		void OnLocalWrite(const WriteMsg* m);
		void OnLocalEviction(const EvictionMsg* m);
		void OnLocalInvalidateResponse(const InvalidateResponseMsg* m);

		void OnRemoteRead(const ReadMsg* m, NodeID src);
		void OnRemoteReadResponse(const ReadResponseMsg* m, NodeID src);
		void OnRemoteWrite(const WriteMsg* m, NodeID src);
		void OnRemoteWriteResponse(const WriteResponseMsg* m, NodeID src);
		void OnRemoteEviction(const EvictionMsg* m, NodeID src);
		void OnRemoteEvictionResponse(const EvictionResponseMsg* m, NodeID src);
		void OnRemoteInvalidate(const InvalidateMsg* m, NodeID src);
      void OnRemoteInvalidateSharer(const InvalidateSharerMsg* m, NodeID src);
		void OnRemoteInvalidateResponse(const InvalidateResponseMsg* m, NodeID src);

		void OnDirectoryBlockRequest(const ReadMsg* m, NodeID src);
      void OnDirectoryBlockRequestSharedRead(const ReadMsg *m, NodeID src);
		void OnDirectoryBlockResponse(const ReadResponseMsg* m, NodeID src);

		void printDebugInfo(const char* fromMethod, const BaseMsg &myMessage, const char* operation);
		void printDebugInfo(const char* fromMethod, const BaseMsg &myMessage, const char* operation,NodeID src);
	   void printDebugInfo(const char* fromMethod,Address addr,NodeID id,const char* operation);
	   void printEraseOwner(const char* fromMethod,Address addr,NodeID id,const char* operation);

		void printPendingDirectorySharedReads();
	   void printPendingLocalReads();

		typedef PooledFunctionGenerator<StoredClassFunction2<ThreeStageDirectory,const ReadMsg*, NodeID, &ThreeStageDirectory::OnDirectoryBlockRequest> > CBOnDirectoryBlockRequest;
		CBOnDirectoryBlockRequest cbOnDirectoryBlockRequest;
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
