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
	//class NetworkMsg;
	class Directory : public BaseMemDevice
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
		HashMultiMap<Address, LookupData<ReadMsg> > pendingDirectorySharedReads;
		HashMap<Address, LookupData<ReadMsg> > pendingDirectoryExclusiveReads;
		HashSet<Address> pendingEviction;
		HashMap<Address, BlockData> directoryData;

		void PerformDirectoryFetch(Address a);
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
		void OnRemoteInvalidateResponse(const InvalidateResponseMsg* m, NodeID src);

		void OnDirectoryBlockRequest(const ReadMsg* m, NodeID src);
		void OnDirectoryBlockResponse(const ReadResponseMsg* m, NodeID src);

	   void printPendingLocalReads(const char* fromMethod, MessageID myMessageID, const char* operation);

		typedef PooledFunctionGenerator<StoredClassFunction2<Directory,const ReadMsg*, NodeID, &Directory::OnDirectoryBlockRequest> > CBOnDirectoryBlockRequest;
		CBOnDirectoryBlockRequest cbOnDirectoryBlockRequest;
		// TODO 2010/08/05 Eric
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
