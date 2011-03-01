// toggles debug messages
//#define MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE
//#define MEMORY_BIP_DIRECTORY_DEBUG_DIRECTORY_DATA
//#define MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE_OLD
//#define MEMORY_BIP_DIRECTORY_DEBUG_MSG_COUNT
//#define MEMORY_BIP_DIRECTORY_DEBUG_PENDING_DIRECTORY_SHARED_READS
//#define MEMORY_BIP_DIRECTORY_DEBUG_PENDING_EVICTION
//#define MEMORY_BIP_DIRECTORY_DEBUG_PENDING_LOCAL_READS
//#define MEMORY_BIP_DIRECTORY_DEBUG_PENDING_REMOTE_INVALIDATES
//#define MEMORY_BIP_DIRECTORY_DEBUG_PENDING_REMOTE_READS

#include "BIPDirectory.h"
#include "../Debug.h"
#include "../MSTypes.h"
#include "../Messages/AllMessageTypes.h"
#include "../Configuration.h"
#include "../EventManager.h"
#include "../Connection.h"

// debugging utilities
#include <sstream>
#include <string>
#include "to_string.h"

using std::cerr;
using std::cout;
using std::endl;
using std::string;

namespace Memory
{
   // this is a debug variable
   int debugMemoryBIPDirectoryGlobalInt = 0;

	NodeID BIPDirectory::HashedPageCalculator::CalcNodeID(Address addr) const
	{
		int index = ((int)(addr / pageSize) % elementCount);
		DebugAssert(index >= 0 && index < (int)nodeSet.size());
		return nodeSet[index];
	}

	void BIPDirectory::HashedPageCalculator::Initialize(const RootConfigNode& node)
	{
	   // "PageSize" should be a multiple of block size
	   // usually, PageSize would be the same as block size
		pageSize = Config::GetInt(node,"PageSize");
		elementCount = Config::GetSetSize(node,"NodeIDSet");
		for(int i = 0; i < elementCount; i++)
		{
			nodeSet.push_back(Config::GetInt(node,"NodeIDSet",0,i));
		}
	}

	// debug function
   void BIPDirectory::dump(HashMap<Memory::MessageID, const Memory::BaseMsg*> &m)
   {
      DumpMsgTemplate<Memory::MessageID>(m);
   }

   // performs a directory fetch from main memory of address a
	void BIPDirectory::PerformDirectoryFetch(Address a)
	{
      // check that Address a is in pendingDirectorySharedReads or pendingDirectoryExclusiveReads
		DebugAssert(pendingDirectorySharedReads.find(a) != pendingDirectorySharedReads.end()
		      || pendingDirectoryExclusiveReads.find(a) != pendingDirectoryExclusiveReads.end());
      // check that Address a is not in both pendingDirectorySharedReads and pendingDirectoryExclusiveReads
		DebugAssert(pendingDirectorySharedReads.find(a) == pendingDirectorySharedReads.end()
		      || pendingDirectoryExclusiveReads.find(a) == pendingDirectoryExclusiveReads.end());
		ReadMsg* m = (ReadMsg*)EM().ReplicateMsg(
		      (pendingDirectorySharedReads.find(a) != pendingDirectorySharedReads.end())
		      ?pendingDirectorySharedReads.find(a)->second.msg : pendingDirectoryExclusiveReads[a].msg);
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE
         PrintDebugInfo("PerformDirectoryFetch",*m,"");
#endif
		m->directoryLookup = false;
		//m->isMemory = true;
		m->onCompletedCallback = NULL;
		m->alreadyHasBlock = false;

		BlockData& b = directoryData[m->addr];
		NodeID target;
		if(b.owner != InvalidNodeID)
		{
			target = b.owner;
		}
		else if(b.sharers.begin() != b.sharers.end())
		{
			target = *(b.sharers.begin());
		}
		else
		{
			SendRequestToMemory(m);
			return;
		}
		if(target == nodeID)
		{// if target == nodeID, it could mean that the block was evicted or invalidated
		   // and a response has already been sent, but we still need
			// to do this to get a proper read response, otherwise the program
			// will stall
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE_OLD
         PrintDebugInfo("RemoteRead",*m,"PerformDirectoryFetch:Error",nodeID);
#endif
			OnRemoteReadCache(m, nodeID);//ERROR
		}
		else
		{
			NetworkMsg* nm = EM().CreateNetworkMsg(GetDeviceID(), m->GeneratingPC());
			nm->destinationNode = target;
			nm->sourceNode = nodeID;
			nm->payloadMsg = m;
			SendMsg(remoteConnectionID, nm, lookupTime + remoteSendTime);
		}
	} // PerformDirectoryFetch(Address a)

	/**
	 * erase Node id as a share for Address a. If a is owned by id, check that there
	 * are no other shares
	 */
	void BIPDirectory::EraseDirectoryShare(Address a, NodeID id)
	{
		DebugAssert(directoryData.find(a) != directoryData.end());
		BlockData& b = directoryData[a];
		if(b.owner == id)
		{
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_DIRECTORY_DATA
         PrintEraseOwner("EraseDirectoryShare",a, id,"b.owner=InvalidNodeID");
#endif
			b.owner = InvalidNodeID;
			DebugAssert(b.sharers.find(id) == b.sharers.end());
		}
		else if(b.sharers.find(id) != b.sharers.end())
		{
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_DIRECTORY_DATA
		   PrintDebugInfo("EraseDirectoryShare",a, id,"b.sharers.erase");
#endif
			b.sharers.erase(id);
		}
	}

	void BIPDirectory::AddDirectoryShare(Address a, NodeID id, bool exclusive)
	{
      DebugAssert(directoryData.find(a) != directoryData.end());
		BlockData& b = directoryData[a];
		DebugAssert(!exclusive || (b.sharers.size() == 0 && (b.owner == id || b.owner == InvalidNodeID)));
		if(b.owner == id || b.owner == InvalidNodeID)
		{
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_DIRECTORY_DATA
         PrintDebugInfo("AddDirectoryShare",a, id, "b.owner=");
#endif
			b.owner = id;
		}
		else if(b.sharers.find(id) == b.sharers.end())
		{
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_DIRECTORY_DATA
         PrintDebugInfo("AddDirectoryShare",a, id,
            ("exclusive="+to_string<bool>(exclusive)+" b.sharers.insert").c_str());
#endif
			b.sharers.insert(id);
		}
	}

   /**
   send a memory access complete message
   */
   void BIPDirectory::SendRequestToMemory(const BaseMsg *msg)
   {
   	if (msg->Type()==mt_Read)
   	{
   		DebugAssertWithMessageID(pendingMemoryReadAccesses.find(msg->MsgID())==pendingMemoryReadAccesses.end(),msg->MsgID());
   		const ReadMsg* m = (const ReadMsg*) msg;
   		pendingMemoryReadAccesses[msg->MsgID()] = m;
   		BaseMsg *forward = EM().ReplicateMsg(msg);
   		// using 0 for send time because the time is taken care of in TestMemory
   		SendMsg(localMemoryConnectionID,forward,localSendTime);
   	}
   	else if (msg->Type()==mt_Write)
   	{
   		DebugAssertWithMessageID(pendingMemoryWriteAccesses.find(msg->MsgID())==pendingMemoryWriteAccesses.end(),msg->MsgID());
   		const WriteMsg* m = (const WriteMsg*) msg;
   		pendingMemoryWriteAccesses[msg->MsgID()] = m;
   		BaseMsg *forward = EM().ReplicateMsg(msg);
   		// using 0 for send time because the time is taken care of in TestMemory
   		SendMsg(localMemoryConnectionID,forward,localSendTime);
   	}
		/*
		else if (msg->Type()==mt_Eviction)
   	{
   		DebugAssertWithMessageID(pendingMemoryEvictionAccesses.find(msg->MsgID())==pendingMemoryEvictionAccesses.end(),msg->MsgID());
   		const EvictionMsg* m = (const EvictionMsg*) msg;
   		pendingMemoryEvictionAccesses[msg->MsgID()] = m;
   		BaseMsg *forward = EM().ReplicateMsg(msg);
   		// using 0 for send time because the time is taken care of in TestMemory
   		SendMsg(localMemoryConnectionID,forward,localSendTime);
   	}
		*/
   	else
   	{
   		PrintError("SendMemoryRequest", msg, "can't send current message type to memory");
   	}
   }

	/**
	 * the message came from the local (cpu) side. It is a read msg
	 */
	void BIPDirectory::OnLocalRead(const ReadMsg* m)
	{
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("LocRead",*m,"");
#endif
		DebugAssertWithMessageID(m,m->MsgID())
		NodeID remoteNode = directoryNodeCalc->CalcNodeID(m->addr);
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_PENDING_LOCAL_READS
		PrintDebugInfo("LocalRead", *m,
		      ("pendingLocalReads.insert("+to_string<MessageID>(m->MsgID())+")").c_str());
#endif
		localReadsReceived++;
		
		DebugAssertWithMessageID(pendingLocalReads.find(m->MsgID()) == pendingLocalReads.end(),m->MsgID())
		pendingLocalReads[m->MsgID()] = m;
		ReadMsg* forward = (ReadMsg*)EM().ReplicateMsg(m);
		forward->onCompletedCallback = NULL;
		forward->directoryLookup = true;
		if(remoteNode == nodeID)
		{
			CBOnDirectoryBlockRequest::FunctionType* f = cbOnDirectoryBlockRequest.Create();
			f->Initialize(this,forward,nodeID);
			EM().ScheduleEvent(f, localSendTime);
		}
		else
		{
			NetworkMsg* nm = EM().CreateNetworkMsg(GetDeviceID(), m->GeneratingPC());
			DebugAssertWithMessageID(nm,m->MsgID())
			nm->destinationNode = remoteNode;
			nm->sourceNode = nodeID;
			nm->payloadMsg = forward;
			SendMsg(remoteConnectionID, nm, remoteSendTime);
		}
	}
	void BIPDirectory::OnLocalReadResponse(const ReadResponseMsg* m)
	{
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("LocReadRes",*m,"");
#endif
		localReadResponsesReceived++;

		DebugAssertWithMessageID(m,m->MsgID())
		DebugAssertWithMessageID(pendingRemoteReads.find(m->solicitingMessage) != pendingRemoteReads.end(),m->MsgID())
		LookupData<ReadMsg>& d = pendingRemoteReads[m->solicitingMessage];
		DebugAssertWithMessageID(d.msg->MsgID() == m->solicitingMessage,m->MsgID())
		NetworkMsg* nm = EM().CreateNetworkMsg(GetDeviceID(),d.msg->GeneratingPC());
		ReadResponseMsg* forward = (ReadResponseMsg*)EM().ReplicateMsg(m);
		nm->sourceNode = nodeID;
		nm->destinationNode = d.sourceNode;
		nm->payloadMsg = forward;
		forward->directoryLookup = false;
		EM().DisposeMsg(d.msg);
		EM().DisposeMsg(m);
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_PENDING_REMOTE_READS
		PrintDebugInfo("LocReadRes",*m,
		      ("pendingRemoteReads.erase("+to_string<MessageID>(d.msg->MsgID())+")").c_str());

#endif
		pendingRemoteReads.erase(d.msg->MsgID());
		SendMsg(remoteConnectionID, nm, remoteSendTime);
	}
	void BIPDirectory::OnLocalWrite(const WriteMsg* m)
	{
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE
      PrintDebugInfo("LocWrite",*m,"");
#endif
		localWritesReceived++;
		
		DebugAssertWithMessageID(m,m->MsgID())
		NodeID id = directoryNodeCalc->CalcNodeID(m->addr);
		WriteResponseMsg* wrm = EM().CreateWriteResponseMsg(GetDeviceID(),m->GeneratingPC());
		wrm->addr = m->addr;
		wrm->size = m->size;
		wrm->solicitingMessage = m->MsgID();
		SendMsg(localCacheConnectionID, wrm, localSendTime);
		if(id == nodeID)
		{
			OnRemoteWrite(m,nodeID);
		}
		else
		{
			NetworkMsg* nm = EM().CreateNetworkMsg(GetDeviceID(),m->GeneratingPC());
			DebugAssertWithMessageID(nm,m->MsgID())
			nm->sourceNode = nodeID;
			nm->destinationNode = id;
			nm->payloadMsg = m;
			SendMsg(remoteConnectionID, nm, remoteSendTime);
		}
	}
	void BIPDirectory::OnLocalEviction(const EvictionMsg* m)
	{
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE
               PrintDebugInfo("LocEvic",*m,"");
#endif
		DebugAssertWithMessageID(m,m->MsgID())
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_PENDING_EVICTION
      PrintDebugInfo("LocalEviction", *m,
         ("pendingEviction.insert("+to_string<Address>(m->addr)+")").c_str());
#endif
		localEvictionsReceived++;

		DebugAssert(pendingEviction.find(m->addr) == pendingEviction.end())
		pendingEviction.insert(m->addr);
		EvictionResponseMsg* erm = EM().CreateEvictionResponseMsg(GetDeviceID(),m->GeneratingPC());
		erm->addr = m->addr;
		erm->size = m->size;
		erm->solicitingMessage = m->MsgID();
		SendMsg(localCacheConnectionID, erm, localSendTime);
		NodeID id = directoryNodeCalc->CalcNodeID(m->addr);
		if(id == nodeID)
		{
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE_OLD
               PrintDebugInfo("RemoteEviction",*m,"LocalEviction",nodeID);
#endif
			OnRemoteEviction(m,nodeID);
		}
		else
		{
			NetworkMsg* nm = EM().CreateNetworkMsg(GetDeviceID(),m->GeneratingPC());
			nm->destinationNode = id;
			nm->sourceNode = nodeID;
			nm->payloadMsg = m;
			SendMsg(remoteConnectionID, nm, remoteSendTime);
		}
	}
	void BIPDirectory::OnLocalInvalidateResponse(const InvalidateResponseMsg* m)
	{
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE
      PrintDebugInfo("LocInvRes",*m,"");
#endif
		localInvalidateResponsesReceived++;

		DebugAssertWithMessageID(m,m->MsgID())
		DebugAssertWithMessageID(pendingRemoteInvalidates.find(m->addr) != pendingRemoteInvalidates.end(),m->MsgID())
		LookupData<InvalidateMsg>& d = pendingRemoteInvalidates[m->addr];
		DebugAssertWithMessageID(d.msg->MsgID() == m->solicitingMessage,m->MsgID())
		if(nodeID != d.sourceNode)
		{
			NetworkMsg* nm = EM().CreateNetworkMsg(GetDeviceID(),d.msg->GeneratingPC());
			nm->sourceNode = nodeID;
			nm->destinationNode = d.sourceNode;
			nm->payloadMsg = m;
			SendMsg(remoteConnectionID, nm, remoteSendTime);
		}
		else
		{
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE_OLD
               PrintDebugInfo("RemoteInvalidateResponse",*m,"LocalInvalidateResponse",nodeID);
#endif
			OnRemoteInvalidateResponse(m,nodeID);
		}
		EM().DisposeMsg(d.msg);
		pendingRemoteInvalidates.erase(d.msg->addr);
	}

	void BIPDirectory::OnLocalMemoryReadResponse(const ReadResponseMsg* m)
	{
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("LocMemReadRes",*m,"");
#endif
		DebugAssertWithMessageID(pendingMemoryReadAccesses.find(m->solicitingMessage)!=pendingMemoryReadAccesses.end(), m->MsgID());
		pendingMemoryReadAccesses.erase(m->solicitingMessage);
		OnRemoteReadResponse(m, InvalidNodeID);
	}

	void BIPDirectory::OnLocalMemoryWriteResponse(const WriteResponseMsg* m)
	{
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("LocMemWriteRes",*m,"");
#endif
		DebugAssertWithMessageID(pendingMemoryWriteAccesses.find(m->solicitingMessage)!=pendingMemoryWriteAccesses.end(), m->MsgID());
		pendingMemoryWriteAccesses.erase(m->solicitingMessage);
		OnRemoteWriteResponse(m, InvalidNodeID);
	}
	/*
	void BIPDirectory::OnLocalMemoryEvictionResponse(const EvictionResponseMsg* m)
	{
		DebugAssertWithMessageID(pendingMemoryEvictionAccesses.find(m->solicitingMessage)!=pendingMemoryEvictionAccesses.end(), m->MsgID());
		pendingMemoryEvictionAccesses.erase(m->solicitingMessage);
		OnRemoteEvictionResponse(m, InvalidNodeID);
	}
	*/
	void BIPDirectory::OnRemoteReadCache(const ReadMsg* m, NodeID src)
	{
		remoteReadsReceived++;
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE
      PrintDebugInfo("RemReadCache",*m,"",src);
#endif
		DebugAssertWithMessageID(m,m->MsgID())
		DebugAssertWithMessageID(!m->directoryLookup,m->MsgID())
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_PENDING_REMOTE_READS
		PrintDebugInfo("RemoteRead", *m,
		      ("pendingRemoteReads.insert("+to_string<MessageID>(m->MsgID())+")").c_str());
#endif
		DebugAssertWithMessageID(pendingRemoteReads.find(m->MsgID()) == pendingRemoteReads.end(),m->MsgID())
		pendingRemoteReads[m->MsgID()].msg = m;
		pendingRemoteReads[m->MsgID()].sourceNode = src;
		SendMsg(localCacheConnectionID, EM().ReplicateMsg(m), localSendTime);
	}

	/*
	void BIPDirectory::OnRemoteReadMemory(const ReadMsg* m, NodeID src)
	{
		;//this method should not be necessary
	}
	*/

	void BIPDirectory::OnRemoteReadResponse(const ReadResponseMsg* m, NodeID src)
	{
		remoteReadResponsesReceived++;
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE
      PrintDebugInfo("RemReadRes",*m,"",src);
#endif
		DebugAssertWithMessageID(m,m->MsgID())
		DebugAssertWithMessageID(!m->directoryLookup,m->MsgID())
		DebugAssertWithMessageID(pendingDirectorySharedReads.find(m->addr) != pendingDirectorySharedReads.end() || pendingDirectoryExclusiveReads.find(m->addr) != pendingDirectoryExclusiveReads.end(),m->MsgID())
		DebugAssertWithMessageID(pendingDirectorySharedReads.find(m->addr) == pendingDirectorySharedReads.end() || pendingDirectoryExclusiveReads.find(m->addr) == pendingDirectoryExclusiveReads.end(),m->MsgID())
		DebugAssertWithMessageID(directoryData.find(m->addr) != directoryData.end(),m->MsgID())
		if(m->satisfied)
		{
			if(m->exclusiveOwnership)
			{
				EraseDirectoryShare(m->addr,src);
			}
			// if there is some pending shared read on this address
			if(pendingDirectorySharedReads.find(m->addr) != pendingDirectorySharedReads.end())
			{
			   // for all the elements in pendingDirectorySharedReads where key is in the range of m->addr
				for(HashMultiMap<Address,LookupData<ReadMsg> >::iterator i = pendingDirectorySharedReads.equal_range(m->addr).first; i != pendingDirectorySharedReads.equal_range(m->addr).second; i++)
				{
					ReadResponseMsg* r = EM().CreateReadResponseMsg(GetDeviceID(),i->second.msg->GeneratingPC());
					r->blockAttached = true;
					r->addr = m->addr;
					r->size = m->size;
					r->directoryLookup = true;
					r->exclusiveOwnership = (pendingDirectorySharedReads.equal_range(m->addr).first++) == pendingDirectorySharedReads.equal_range(m->addr).second;// only one reader
					r->satisfied = true;
					r->solicitingMessage = i->second.msg->MsgID();
					AddDirectoryShare(m->addr,i->second.sourceNode,false);
					if(i->second.sourceNode == nodeID)
					{
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE_OLD
               PrintDebugInfo("DirBlkRes",*r,"RemReadRes",nodeID);
#endif
						OnDirectoryBlockResponse(r,nodeID);
					}
					else
					{
						NetworkMsg* nm = EM().CreateNetworkMsg(GetDeviceID(),i->second.msg->GeneratingPC());
						nm->destinationNode = i->second.sourceNode;
						nm->sourceNode = nodeID;
						nm->payloadMsg = r;
						SendMsg(remoteConnectionID, nm, remoteSendTime + satisfyTime);
					}
					EM().DisposeMsg(i->second.msg);
				}
				pendingDirectorySharedReads.erase(pendingDirectorySharedReads.equal_range(m->addr).first,pendingDirectorySharedReads.equal_range(m->addr).second);
			}  //if(pendingDirectorySharedReads.find(m->addr) != pendingDirectorySharedReads.end())
			else // pendingDirectorySharedReads.find(m->addr) == pendingDirectorySharedReads.end())
			{  // there is no pending shared read on this address, then we have an exclusive pending read
				DebugAssertWithMessageID(m->exclusiveOwnership,m->MsgID())
				DebugAssertWithMessageID(m->blockAttached,m->MsgID())
				DebugAssertWithMessageID(directoryData[m->addr].owner == InvalidNodeID || directoryData[m->addr].owner == src,m->MsgID())
				if(directoryData[m->addr].sharers.size() == 0)
				{//send block on now
					ReadResponseMsg* response = (ReadResponseMsg*)EM().ReplicateMsg(m);
					response->directoryLookup = true;
					if(pendingDirectoryExclusiveReads[m->addr].sourceNode != nodeID)
					{
						NetworkMsg* net = EM().CreateNetworkMsg(GetDeviceID(),m->GeneratingPC());
						net->sourceNode = nodeID;
						net->destinationNode = pendingDirectoryExclusiveReads[m->addr].sourceNode;
						net->payloadMsg = response;
						SendMsg(remoteConnectionID, net, lookupTime + remoteSendTime);
					}
					else
					{
						SendMsg(localCacheConnectionID, response, lookupTime + localSendTime);
					}
					AddDirectoryShare(m->addr,pendingDirectoryExclusiveReads[m->addr].sourceNode,true);
					EM().DisposeMsg(pendingDirectoryExclusiveReads[m->addr].msg);
					pendingDirectoryExclusiveReads.erase(m->addr);
				} // if(directoryData[m->addr].sharers.size() == 0)
				else
				{//hold the block, send on once all invalidations are complete
					for(HashSet<NodeID>::iterator i = directoryData[m->addr].sharers.begin(); i != directoryData[m->addr].sharers.end(); i++)
					{
						InvalidateMsg* inv = EM().CreateInvalidateMsg(GetDeviceID(),m->GeneratingPC());
						inv->addr = m->addr;
						inv->size = m->size;
						if(*i != nodeID)
						{
							NetworkMsg* net = EM().CreateNetworkMsg(GetDeviceID(),m->GeneratingPC());
							net->destinationNode = *i;
							net->sourceNode = nodeID;
							net->payloadMsg = inv;
							SendMsg(remoteConnectionID, net, lookupTime + remoteSendTime);
						}
						else
						{
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE_OLD
							PrintDebugInfo("RemoteInvalidate",*inv,"RemReadRes",nodeID);
#endif
							OnRemoteInvalidate(inv, nodeID);
						}
					}
				} // else (directoryData[m->addr].sharers.size() != 0)
			} // else pendingDirectorySharedReads.find(m->addr) == pendingDirectorySharedReads.end())
			EM().DisposeMsg(m);
		}// if(m->satisfied)
		else  // !m->satisfied
		{
			EraseDirectoryShare(m->addr,src);
			// 2011/02/23 Commented out the following line and replaced it with the one below
			DebugAssertWithMessageID(directoryData[m->addr].owner == InvalidNodeID,m->MsgID());
			PerformDirectoryFetch(m->addr);
			/*
			if (directoryData[m->addr].owner==InvalidNodeID)
			{

			}
			*/
			EM().DisposeMsg(m);
		}
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_DIRECTORY_DATA
      PrintDirectoryData(m->addr,m->MsgID());
#endif
	} // OnRemoteReadResponse

	void BIPDirectory::OnRemoteWrite(const WriteMsg* m, NodeID src)
	{
		remoteWritesReceived++;
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE
      PrintDebugInfo("RemWrite",*m,"",src);
#endif
		DebugAssertWithMessageID(m,m->MsgID())
		DebugAssertWithMessageID(nodeID == directoryNodeCalc->CalcNodeID(m->addr),m->MsgID())

		SendRequestToMemory(m);

		/*
		NodeID memoryNode = memoryNodeCalc->CalcNodeID(m->addr);
		if(src != nodeID)
		{
			NetworkMsg* nm = EM().CreateNetworkMsg(GetDeviceID(), m->GeneratingPC());
			nm->sourceNode = nodeID;
			nm->destinationNode = memoryNode;
			nm->payloadMsg = m;

			WriteResponseMsg* wrm = EM().CreateWriteResponseMsg(GetDeviceID(), m->GeneratingPC());
			wrm->addr = m->addr;
			wrm->size = m->size;
			wrm->solicitingMessage = m->MsgID();
			SendMsg(remoteConnectionID, nm, remoteSendTime);
			nm = EM().CreateNetworkMsg(GetDeviceID(), m->GeneratingPC());
			nm->sourceNode = nodeID;
			nm->destinationNode = src;
			nm->payloadMsg = wrm;
			SendMsg(remoteConnectionID, nm, remoteSendTime);
		}
		*/
	}

	void BIPDirectory::OnRemoteWriteResponse(const WriteResponseMsg* m, NodeID src)
	{
		remoteWriteResponsesReceived++;
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE
      PrintDebugInfo("RemWriteRes",*m,"",src);
#endif
		DebugAssertWithMessageID(m,m->MsgID())
		EM().DisposeMsg(m);
	}

	void BIPDirectory::OnRemoteEviction(const EvictionMsg* m, NodeID src)
	{
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("RemEvic",*m,"",src);
#endif
		remoteEvictionsReceived++;
		
		DebugAssertWithMessageID(m,m->MsgID())
		DebugAssertWithMessageID(directoryData.find(m->addr) != directoryData.end(),m->MsgID())
		BlockData& b = directoryData[m->addr];
		if(b.owner == src)
		{
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_DIRECTORY_DATA
         PrintDebugInfo("RemoteEviction",*m,"b.owner=InvalidNodeID",src);
#endif
			b.owner = InvalidNodeID;
		}
		else if(b.sharers.find(src) != b.sharers.end())
		{
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_DIRECTORY_DATA
         PrintDebugInfo("RemoteEviction",*m,
            ("b.sharers.erase("+to_string<NodeID>(src)+")").c_str(),src);
#endif
			b.sharers.erase(src);
		}
		if(src == nodeID)
		{
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_PENDING_EVICTION
      PrintDebugInfo("RemoteEviction", *m,
         ("pendingEviction.erase("+to_string<Address>(m->addr)+")").c_str(),src);
#endif
			DebugAssertWithMessageID(pendingEviction.find(m->addr) != pendingEviction.end(),m->MsgID())
			pendingEviction.erase(m->addr);
		}
		else
		{
			EvictionResponseMsg* rm = EM().CreateEvictionResponseMsg(GetDeviceID(),m->GeneratingPC());
			rm->addr = m->addr;
			rm->size = m->size;
			rm->solicitingMessage = m->MsgID();
			NetworkMsg* nm = EM().CreateNetworkMsg(GetDeviceID(),m->GeneratingPC());
			nm->destinationNode = src;
			nm->sourceNode = nodeID;
			nm->payloadMsg = rm;
			SendMsg(remoteConnectionID, nm, remoteSendTime);
		}

		if(m->blockAttached)
		{
			WriteMsg* wm = EM().CreateWriteMsg(GetDeviceID(), m->GeneratingPC());
			wm->addr = m->addr;
			wm->size = m->size;
			wm->onCompletedCallback = NULL;

			SendRequestToMemory(wm);

			/*
			NetworkMsg* nm = EM().CreateNetworkMsg(GetDeviceID(), m->GeneratingPC());
			nm->sourceNode = nodeID;
			nm->destinationNode = memoryNodeCalc->CalcNodeID(m->addr);
			nm->payloadMsg = wm;
			SendMsg(remoteConnectionID, nm, remoteSendTime);
			*/
		}
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_DIRECTORY_DATA
      PrintDirectoryData(m->addr,m->MsgID());
#endif
	}

	void BIPDirectory::OnRemoteEvictionResponse(const EvictionResponseMsg* m, NodeID src)
	{
		remoteEvictionResponsesReceived++;
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE
      PrintDebugInfo("RemEvicRes",*m,"",src);
#endif
		DebugAssertWithMessageID(m,m->MsgID())
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_PENDING_EVICTION
      PrintDebugInfo("RemoteEvictionResponse", *m,
         ("pendingEviction.erase("+to_string<Address>(m->addr)+")").c_str(),src);
#endif
		DebugAssertWithMessageID(pendingEviction.find(m->addr) != pendingEviction.end(),m->MsgID())
		pendingEviction.erase(m->addr);
		SendMsg(localCacheConnectionID, m, localSendTime);
	}

	void BIPDirectory::OnRemoteInvalidate(const InvalidateMsg* m, NodeID src)
	{
		remoteInvalidatesReceived++;
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE
      PrintDebugInfo("RemInv",*m,"",src);
#endif
		DebugAssertWithMessageID(m,m->MsgID())
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_PENDING_REMOTE_INVALIDATES
      PrintDebugInfo("RemoteInvalidate", *m,
            ("pendingRemoteInvalidates.insert("+to_string<Address>(m->addr)+")").c_str());
#endif
		DebugAssertWithMessageID(pendingRemoteInvalidates.find(m->addr) == pendingRemoteInvalidates.end(),m->MsgID())
		pendingRemoteInvalidates[m->addr].sourceNode = src;
		pendingRemoteInvalidates[m->addr].msg = m;
		SendMsg(localCacheConnectionID, EM().ReplicateMsg(m), localSendTime);
	}

	void BIPDirectory::OnRemoteInvalidateResponse(const InvalidateResponseMsg* m, NodeID src)
	{
		remoteInvalidateResponsesReceived++;
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE
      PrintDebugInfo("RemInvRes",*m,"",src);
#endif
		DebugAssertWithMessageID(m,m->MsgID())
		DebugAssertWithMessageID(directoryData.find(m->addr) != directoryData.end(),m->MsgID())
		BlockData& b = directoryData[m->addr];
		DebugAssertWithMessageID(!m->blockAttached || (b.owner == src) || (b.owner==InvalidNodeID),m->MsgID())
      DebugAssertWithMessageID(m->blockAttached || (b.sharers.find(src)!=b.sharers.end()) || b.owner==src || b.owner==InvalidNodeID,m->MsgID())
		if(b.owner == src)
		{
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_DIRECTORY_DATA
         PrintDebugInfo("RemoteInvalidateResponse",*m,"b.owner=InvalidNodeID",src);
#endif
			b.owner = InvalidNodeID;
		}
      else if (b.sharers.find(src) != b.sharers.end())
		{
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_DIRECTORY_DATA
			PrintDebugInfo("RemoteInvalidateResponse",*m,
			   ("b.sharers.erase("+to_string<NodeID>(src)+")").c_str(),src);
#endif
			b.sharers.erase(src);
		}
      // it is possible that both the owner and sharer don't contain src if an RemoteEviction arrives before this
      else
      {// a convenient place to set a breakpoint
         int breakpoint = 0;
      }

		if(m->blockAttached)
		{
			WriteMsg* wm = EM().CreateWriteMsg(GetDeviceID(), m->GeneratingPC());
			wm->addr = m->addr;
			wm->size = m->size;
			wm->onCompletedCallback = NULL;

			SendRequestToMemory(wm);
			/*
			NetworkMsg* nm = EM().CreateNetworkMsg(GetDeviceID(), m->GeneratingPC());
			nm->sourceNode = nodeID;
			nm->destinationNode = memoryNodeCalc->CalcNodeID(m->addr);
			nm->payloadMsg = wm;
			SendMsg(remoteConnectionID, nm, remoteSendTime);
			*/
		}
		if(pendingDirectoryExclusiveReads.find(m->addr) != pendingDirectoryExclusiveReads.end())
		{
			if( (b.owner == InvalidNodeID && b.sharers.size() == 0) ||
				(b.owner == pendingDirectoryExclusiveReads[m->addr].sourceNode && b.sharers.size() == 0) ||
				(b.owner == InvalidNodeID && b.sharers.size() == 1 && b.sharers.find(pendingDirectoryExclusiveReads[m->addr].sourceNode) != b.sharers.end()))
			{
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_DIRECTORY_DATA
			   PrintDebugInfo("RemoteInvalidateResponse",*m,"b.sharers.clear()",src);
            PrintDebugInfo("RemoteInvalidateResponse",*m,
               ("b.owner="+to_string<NodeID>(pendingDirectoryExclusiveReads[m->addr].sourceNode)).c_str(),
               src);
#endif
				b.sharers.clear();
				b.owner = pendingDirectoryExclusiveReads[m->addr].sourceNode;
				ReadResponseMsg* rm = EM().CreateReadResponseMsg(GetDeviceID(),pendingDirectoryExclusiveReads[m->addr].msg->GeneratingPC());
				rm->addr = m->addr;
				rm->size = m->size;
				rm->blockAttached = true;
				rm->solicitingMessage = pendingDirectoryExclusiveReads[m->addr].msg->MsgID();
				rm->directoryLookup = true;
				rm->exclusiveOwnership = true;
				rm->satisfied = true;
				if(pendingDirectoryExclusiveReads[m->addr].sourceNode == nodeID)
				{
					SendMsg(localCacheConnectionID, rm, localSendTime + satisfyTime);
				}
				else
				{
					NetworkMsg* n = EM().CreateNetworkMsg(GetDeviceID(), pendingDirectoryExclusiveReads[m->addr].msg->GeneratingPC());
					n->sourceNode = nodeID;
					n->destinationNode = pendingDirectoryExclusiveReads[m->addr].sourceNode;
					n->payloadMsg = rm;
					SendMsg(remoteConnectionID, n, remoteSendTime + satisfyTime);
				}
				AddDirectoryShare(m->addr,pendingDirectoryExclusiveReads[m->addr].sourceNode,true);
				EM().DisposeMsg(pendingDirectoryExclusiveReads[m->addr].msg);
				pendingDirectoryExclusiveReads.erase(m->addr);
			} //        if( (b.owner == InvalidNodeID && b.sharers.size() == 0) ||
		} // if(pendingDirectoryExclusiveReads.find(m->addr) != pendingDirectoryExclusiveReads.end())
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_DIRECTORY_DATA
      PrintDirectoryData(m->addr,m->MsgID());
#endif
		EM().DisposeMsg(m);
	}

	void BIPDirectory::OnDirectoryBlockRequest(const ReadMsg* m, NodeID src)
	{
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE
      PrintDebugInfo("DirBlkReq",*m,"",src);
#endif
		DebugAssertWithMessageID(m,m->MsgID());
		directoryRequestsReceived++;
      // if the address is in pendingDirectoryExclusiveReads or
		// we are requesting for exclusive access and the address is in pendingDirectorySharedReads
		if(pendingDirectoryExclusiveReads.find(m->addr) != pendingDirectoryExclusiveReads.end() ||
		      (m->requestingExclusive && pendingDirectorySharedReads.find(m->addr) != pendingDirectorySharedReads.end()))
		{//cannot complete the request at this time
			if(src == nodeID)
			{
				CBOnDirectoryBlockRequest::FunctionType* f = cbOnDirectoryBlockRequest.Create();
				f->Initialize(this,m,src);
				EM().ScheduleEvent(f,lookupRetryTime);
			}
			else
			{
				NetworkMsg* n = EM().CreateNetworkMsg(GetDeviceID(),m->GeneratingPC());
				ReadResponseMsg* rm = EM().CreateReadResponseMsg(GetDeviceID(),m->GeneratingPC());
				n->sourceNode = nodeID;
				n->destinationNode = src;
				n->payloadMsg = rm;
				rm->addr = m->addr;
				rm->satisfied = false;
				rm->size = m->size;
				rm->solicitingMessage = m->MsgID();
				rm->blockAttached = false;
				rm->directoryLookup = true;
				SendMsg(remoteConnectionID, n, lookupTime + remoteSendTime);
				EM().DisposeMsg(m);
			}
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_DIRECTORY_DATA
         PrintDirectoryData(m->addr,m->MsgID());
#endif
			return;
		} // endif cannot satisfy request at this time
		LookupData<ReadMsg> ld;
		ld.msg = m;
		ld.sourceNode = src;
		if(m->requestingExclusive)
		{
			DebugAssertWithMessageID(pendingDirectoryExclusiveReads.find(m->addr) == pendingDirectoryExclusiveReads.end(),m->MsgID())
			pendingDirectoryExclusiveReads[m->addr] = ld;
		}
		else
		{
			bool existingRequest = false;
			if(pendingDirectorySharedReads.find(m->addr) != pendingDirectorySharedReads.end())
			{
				existingRequest = true;
			}
			pendingDirectorySharedReads.insert(std::pair<Address,LookupData<ReadMsg> >(m->addr,ld));
         // if it is an existing request, return so that we don't
         // perform directory fetch again
			if(existingRequest)
			{
				return;
			}
		}
		PerformDirectoryFetch(m->addr);
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_DIRECTORY_DATA
      PrintDirectoryData(m->addr,m->MsgID());
#endif
	}
	void BIPDirectory::OnDirectoryBlockResponse(const ReadResponseMsg* m, NodeID src)
	{
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE
      PrintDebugInfo("DirBlkRes",*m,"",src);
#endif
		DebugAssertWithMessageID(m,m->MsgID());
		directoryResponsesReceived++;

#ifdef MEMORY_BIP_DIRECTORY_DEBUG_PENDING_DIRECTORY_SHARED_READS
		PrintPendingDirectorySharedReads();
#endif
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_PENDING_LOCAL_READS
		PrintPendingLocalReads();
#endif
		// check that m->solicitingMessage is in pendingLocalReads before accessing it
		// error here means that we expect there to be a message in pendingLocalReads,
		// but the message is not there
      DebugAssertWithMessageID(pendingLocalReads.find(m->solicitingMessage) != pendingLocalReads.end(),m->MsgID())
		const ReadMsg* ref = pendingLocalReads[m->solicitingMessage];

		if(!m->satisfied)
		{
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_PENDING_LOCAL_READS
		   PrintDebugInfo("DirBlkRes",*m,
		         ("pendingLocalReads.erase("+to_string<MessageID>(m->solicitingMessage)+")").c_str());
#endif
			pendingLocalReads.erase(m->solicitingMessage);
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE_OLD
               PrintDebugInfo("LocalRead",*m,"DirBlkRes");
#endif
			OnLocalRead(ref);
			return;
		}
		ReadResponseMsg* r = EM().CreateReadResponseMsg(GetDeviceID(),ref->GeneratingPC());
		r->addr = ref->addr;
		r->blockAttached = m->blockAttached;
		r->directoryLookup = false;
		r->exclusiveOwnership = m->exclusiveOwnership;
		r->satisfied = true;
		r->size = ref->size;
		r->solicitingMessage = ref->MsgID();
		EM().DisposeMsg(ref);
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_PENDING_LOCAL_READS
		PrintDebugInfo("DirBlkRes", *m,
		   ("pendingLocalReads.erase("+m->solicitingMessage+")").c_str());
#endif
		pendingLocalReads.erase(m->solicitingMessage);

		EM().DisposeMsg(m);
		SendMsg(localCacheConnectionID, r, satisfyTime + localSendTime);
	}

	void BIPDirectory::Initialize(EventManager* em, const RootConfigNode& config, const std::vector<Connection*>& connectionSet)
	{
		BaseMemDevice::Initialize(em,config,connectionSet);
		localCacheConnectionID = localMemoryConnectionID = remoteConnectionID = -1;
		for(int i = 0; i < ConnectionCount(); i++)
		{
			if(GetConnection(i).Name() == "LocalCacheConnection")
			{
				localCacheConnectionID = i;
			}
			else if(GetConnection(i).Name() == "LocalMemoryConnection")
			{
				localMemoryConnectionID = i;
			}
			else if(GetConnection(i).Name() == "RemoteConnection")
			{
				remoteConnectionID = i;
			}
			else
			{
				DebugFail("unexpected connection name");
			}
		}
		DebugAssert(localCacheConnectionID != -1);
		DebugAssert(localMemoryConnectionID != -1);
		DebugAssert(remoteConnectionID != -1);
		localSendTime = Config::GetIntOrElse(1,config,"LocalSendTime");
		remoteSendTime = Config::GetIntOrElse(1,config,"RemoteSendTime");
		lookupRetryTime = Config::GetIntOrElse(1,config,"LookupRetryTime");
		// the time for the requesting node to ask for the data,
		// until the home node has received the data
		lookupTime = Config::GetIntOrElse(1,config,"LookupTime");
		// the time after home node has received the requested data,
		// until the requesting node received the requested data
		satisfyTime = Config::GetIntOrElse(1,config,"SatisfyTime");
		nodeID = (NodeID)Config::GetIntOrElse(1,config,"NodeID");
		const RootConfigNode& dirCalc = Config::GetSubRoot(config,"DirectoryNodeCalculator");
		//const RootConfigNode& memCalc = Config::GetSubRoot(config,"MemoryNodeCalculator");
		if(Config::GetString(dirCalc, "Type") == "HashedPageCalculator")
		{
			directoryNodeCalc = new HashedPageCalculator();
		}
		else
		{
			DebugFail("Unknown directory node calculator type");
		}
		/*
		if(Config::GetString(memCalc, "Type") == "HashedPageCalculator")
		{
			memoryNodeCalc = new HashedPageCalculator();
		}
		else
		{
			DebugFail("Unknown memory node calculator type");
		}
		*/
		directoryNodeCalc->Initialize(dirCalc);
		//memoryNodeCalc->Initialize(memCalc);

      messagesReceived = 0;
		directoryRequestsReceived = 0;
		directoryResponsesReceived = 0;
		localEvictionsReceived = 0;
		localInvalidateResponsesReceived = 0;
		localReadsReceived = 0;
		localReadResponsesReceived = 0;
		localWritesReceived = 0;
		remoteEvictionsReceived = 0;
		remoteEvictionResponsesReceived = 0;
		remoteInvalidatesReceived = 0;
		remoteInvalidateResponsesReceived = 0;
		remoteReadsReceived = 0;
		remoteReadResponsesReceived = 0;
		remoteWritesReceived = 0;
		remoteWriteResponsesReceived = 0;
	}  // BIPDirectory::Initialize()
	/**
	 * this is used for checkpoint purposes
	 */
	void BIPDirectory::DumpRunningState(RootConfigNode& node)
	{}
	/**
	 * put anything here that you might want to output to the report file
	 */
	void BIPDirectory::DumpStats(std::ostream& out)
	{
		out << "localSendTime:" << localSendTime << std::endl;
		out << "remoteSendTime:" << remoteSendTime << std::endl;
		out << "lookupRetryTime:" << lookupRetryTime << std::endl;
		out << "lookupTime:" << lookupTime << std::endl;
		out << "satisfyTime:" << satisfyTime << std::endl;
		out << "nodeID:" << nodeID << std::endl;
	   out << "TotalMessagesReceived:" << messagesReceived << std::endl;
		out << "directoryRequestsReceived:" << directoryRequestsReceived << std::endl;
		out << "directoryResponsesReceived:" << directoryResponsesReceived << std::endl;
		out << "localEvictionsReceived:" << localEvictionsReceived << std::endl;
		out << "localInvalidateResponsesReceived:" << localInvalidateResponsesReceived << std::endl;
		out << "localReadsReceived:" << localReadsReceived << std::endl;
		out << "localReadResponsesReceived:" << localReadResponsesReceived << std::endl;
		out << "localWritesReceived:" << localWritesReceived << std::endl;
		out << "remoteEvictionsReceived:" << remoteEvictionsReceived << std::endl;
		out << "remoteEvictionResponsesReceived:" << remoteEvictionResponsesReceived << std::endl;
		out << "remoteInvalidatesReceived:" << remoteInvalidatesReceived << std::endl;
		out << "remoteInvalidateResponsesReceived:" << remoteInvalidateResponsesReceived << std::endl;
		out << "remoteReadsReceived:" << remoteReadsReceived << std::endl;
		out << "remoteReadResponsesReceived:" << remoteReadResponsesReceived << std::endl;
		out << "remoteWritesReceived:" << remoteWritesReceived << std::endl;
		out << "remoteWriteResponsesReceived:" << remoteWriteResponsesReceived << std::endl;
	}
	/**
	 * Handles all the incoming messages from outside of the directory.
	 * The message can come from the cache side or from the network.
	 */
	void BIPDirectory::RecvMsg(const BaseMsg* msg, int connectionID)
	{
	   messagesReceived++;
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_MSG_COUNT
	   cout << "BIPDirectory::RecvMsg: " << memoryDirectoryGlobalInt++ << ' ' << endl;
#endif
		DebugAssert(msg);
		if(connectionID == localCacheConnectionID)
		{
			switch(msg->Type())
			{
			case(mt_Read):
				{
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE_OLD
			      PrintDebugInfo("LocalRead",*msg,"RecvMsg");
#endif
					OnLocalRead((ReadMsg*)msg);
					break;
				}
			case(mt_ReadResponse):
				{
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE_OLD
               PrintDebugInfo("LocReadRes",*msg,"RecvMsg");
#endif
					OnLocalReadResponse((ReadResponseMsg*)msg);
					break;
				}
			case(mt_Write):
				{
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE_OLD
               PrintDebugInfo("LocalWrite",*msg,"RecvMsg");
#endif
					OnLocalWrite((WriteMsg*)msg);
					break;
				}
			case(mt_Eviction):
				{
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE_OLD
               PrintDebugInfo("LocalEviction",*msg,"RecvMsg");
#endif
					OnLocalEviction((EvictionMsg*)msg);
					break;
				}
			case(mt_InvalidateResponse):
				{
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE_OLD
               PrintDebugInfo("LocalInvalidateResponse",*msg,"RecvMsg");
#endif
					OnLocalInvalidateResponse((InvalidateResponseMsg*)msg);
					break;
				}
			default:
				DebugFail("Msg Type not handled");
			}
		}
		else if (connectionID == localMemoryConnectionID)
		{
			if (msg->Type()==mt_ReadResponse)
			{
				const ReadResponseMsg* m = (const ReadResponseMsg*)msg;
				OnLocalMemoryReadResponse(m);
			}
			else if (msg->Type()==mt_WriteResponse)
			{
				const WriteResponseMsg* m = (const WriteResponseMsg*)msg;
				OnLocalMemoryWriteResponse(m);
			}
			/*
			else if (msg->Type()==mt_EvictionResponse)
			{
				const EvictionResponseMsg* m = (const EvictionResponseMsg*)msg;
				OnLocalMemoryEvictionResponse(m);
			}
			*/
			else
			{
				PrintError("RecvMsg", msg, "local memory should only emit ReadResponse or WriteResponse");
			}
		}
		else if(connectionID == remoteConnectionID)
		{
			DebugAssert(msg->Type() == mt_Network);
			const NetworkMsg* m = (const NetworkMsg*)msg;
			const BaseMsg* payload = m->payloadMsg;
			NodeID src = m->sourceNode;
			DebugAssert(m->destinationNode == nodeID);
			EM().DisposeMsg(m);
			switch(payload->Type())
			{
			case(mt_Read):
				{
					ReadMsg* m = (ReadMsg*)payload;
					if(m->directoryLookup)
					{
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE_OLD
               PrintDebugInfo("DirBlkReq",*m,"RecvMsg",src);
#endif
						OnDirectoryBlockRequest(m,src);
					}
					else
					{
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE_OLD
               PrintDebugInfo("RemoteRead",*m,"RecvMsg",src);
#endif
						OnRemoteReadCache(m,src);
					}
					/*
					 * should never happen because memory is always on directory
					else if (m->isMemory)
					{
						OnRemoteReadMemory(m,src);
					}
					*/
					break;
				}
			case(mt_ReadResponse):
				{
					ReadResponseMsg* m = (ReadResponseMsg*)payload;
					if(m->directoryLookup)
					{
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE_OLD
               PrintDebugInfo("DirBlkRes",*m,"RecvMsg",src);
#endif
						OnDirectoryBlockResponse(m,src);
					}
					else
					{
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE_OLD
               PrintDebugInfo("RemReadRes",*m,"RecvMsg",src);
#endif
						OnRemoteReadResponse(m,src);
					}
					break;
				}
			case(mt_WriteResponse):
				{
               WriteResponseMsg* m = (WriteResponseMsg*)payload;
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE_OLD
               PrintDebugInfo("RemoteWriteResponse",*m,"RecvMsg",src);
#endif
					OnRemoteWriteResponse(m,src);
					break;
				}
			case(mt_Eviction):
				{
               EvictionMsg* m = (EvictionMsg*)payload;
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE_OLD
               PrintDebugInfo("RemoteEviction",*m,"RecvMsg",src);
#endif
					OnRemoteEviction(m,src);
					break;
				}
			case(mt_EvictionResponse):
				{
               EvictionResponseMsg* m = (EvictionResponseMsg*)payload;
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE_OLD
               PrintDebugInfo("RemoteEvictionResponse",*m,"RecvMsg",src);
#endif
					OnRemoteEvictionResponse(m,src);
					break;
				}
			case(mt_Invalidate):
            {
               InvalidateMsg* m = (InvalidateMsg*)payload;
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE_OLD
               PrintDebugInfo("RemoteInvalidate",*m,"RecvMsg",src);
#endif
					OnRemoteInvalidate(m,src);
					break;
				}
			case(mt_InvalidateResponse):
				{
			      InvalidateResponseMsg* m = (InvalidateResponseMsg*)payload;
#ifdef MEMORY_BIP_DIRECTORY_DEBUG_VERBOSE_OLD
               PrintDebugInfo("RemoteInvalidateResponse",*m,"RecvMsg",src);
#endif
					OnRemoteInvalidateResponse(m,src);
					break;
				}
			default:
				DebugFail("Payload type unrecognized");
			}
		}
		else
		{
			DebugFail("Connection not a valid ID");
		}
	}

	/**
	 * readMsgArray in this method could be coupled with Eclipse's debugger to
	 * see what elements are in pendingDirectorySharedReads. The reason for using
	 * a regular array instead of a vector is because Eclipse can view
	 * Array elements directly, but not elements in STL containers.
	 */
	void BIPDirectory::PrintPendingDirectorySharedReads()
	{
	   HashMultiMap<Address, LookupData<ReadMsg> >::const_iterator myIterator;


	   myIterator = pendingDirectorySharedReads.begin();
      cout << "BIPDirectory::printPendingDirectorySharedReads: ";

      int size = pendingDirectorySharedReads.size();
      cout << "size=" << size << endl;
      ReadMsg const** readMsgArray = new ReadMsg const*[size];
      //readMsgVector[0] = new ReadMsg const;

      int i = 0;
	   for (myIterator = pendingDirectorySharedReads.begin();
	         myIterator != pendingDirectorySharedReads.end(); ++myIterator)
	   {
	      cout << "\t\tmyIterator->first=" << myIterator->first << " ";
	      readMsgArray[i] = myIterator->second.msg;
	      i++;
	   }
      delete [] readMsgArray;
      readMsgArray = NULL;
	}

   /**
    * readMsgArray in this method could be coupled with Eclipse's debugger to
    * see what elements are in pendingLocalReads. The reason for using
    * a regular array instead of a vector is because Eclipse can view
    * Array elements directly, but not elements in STL containers.
    */
	void BIPDirectory::PrintPendingLocalReads()
	{
      HashMap<MessageID, const ReadMsg*>::const_iterator myIterator;

      myIterator = pendingLocalReads.begin();

      cout << "BIPDirectory::printPendingLocalReads: ";

      int size = pendingLocalReads.size();
      cout << "size=" << size;

      ReadMsg const ** readMsgArray = new ReadMsg const *[size];

      int i = 0;
      for (myIterator = pendingLocalReads.begin();
            myIterator != pendingLocalReads.end(); ++myIterator)
      {
         cout << "myIterator->first=" << myIterator->first << " ";
         //const ReadMsg *myReadMsg = myIterator->second.msg;
         readMsgArray[i] = myIterator->second;
         i++;
      }
      
      delete [] readMsgArray;
      readMsgArray = NULL;
	}

   void BIPDirectory::PrintDebugInfo(const char* fromMethod, const BaseMsg &myMessage,
         const char* operation)
   {
      printBaseMemDeviceDebugInfo("BIP", fromMethod, myMessage, operation);
   }

   void BIPDirectory::PrintDebugInfo(const char* fromMethod, const BaseMsg &myMessage,
         const char* operation, NodeID src)
   {
      printBaseMemDeviceDebugInfo("BIP", fromMethod, myMessage, operation, src);
   }

   void BIPDirectory::PrintDebugInfo(const char* fromMethod,Address addr,NodeID id,const char* operation)
   {
      cout << setw(17) << " " // account for spacing from src and msgSrc
            << " dst=" << setw(3) << GetDeviceID()
            << setw(11) << " "   // account for spacing from msgID
            << " addr=" << addr
            << " " << fromMethod
            << " " << operation << "(" << id << ")"
            << endl;
   }

   void BIPDirectory::PrintDirectoryData(Address myAddress, MessageID myMessageID)
   {
      DebugAssert(directoryData.find(myAddress)!=directoryData.end());

      bool isSharedBusy = (pendingDirectorySharedReads.find(myAddress)!=pendingDirectorySharedReads.end());
      bool isExclusiveBusy = (pendingDirectoryExclusiveReads.find(myAddress)!=pendingDirectoryExclusiveReads.end());
      
      directoryData[myAddress].print(myAddress, myMessageID,isSharedBusy, isExclusiveBusy);
   }

   void BIPDirectory::PrintEraseOwner(const char* fromMethod,Address addr,NodeID id,const char* operation)
   {
      cout << setw(17) << " " // account for spacing from src and msgSrc
            << " dst=" << setw(3) << GetDeviceID()
            << setw(10) << " "   // account for spacing from msgID
            << " addr=" << addr
            << " " << fromMethod
            << " nodeID=" << id
            << " " << operation
            << endl;
   }

   void BIPDirectory::PrintError(const char* fromMethod, const BaseMsg *m)
   {
   	stringstream ss;
   	PutErrorStringStream(ss, fromMethod, m);
   	string output = ss.str();
   	cerr << output << endl;
		DebugFail("");
   }

   void BIPDirectory::PrintError(const char* fromMethod, const BaseMsg *m, const char* comment)
   {
   	stringstream ss;
   	PutErrorStringStream(ss, fromMethod, m);
   	ss << ": " << comment;
   	string output = ss.str();
   	cerr << output << endl;
		DebugFail("");
   }

	void BIPDirectory::PutErrorStringStream(stringstream& ss, const char* fromMethod, const BaseMsg*m)
	{
		ss << "BIPDir::"
			<< fromMethod
			<< ": msg received with id "
			<< m->MsgID()
			<< " type "
			<< m->Type()
			;
	}
}
