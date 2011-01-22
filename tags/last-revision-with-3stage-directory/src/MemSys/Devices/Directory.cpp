#include "Directory.h"
#include "../Debug.h"
#include "../MSTypes.h"
#include "../Messages/AllMessageTypes.h"
#include "../Configuration.h"
#include "../EventManager.h"
#include "../Connection.h"

// debugging utilities
#include "to_string.h"

using std::cerr;
using std::cout;
using std::endl;

namespace Memory
{
   // this is a debug variable
   int debugMemoryDirectoryGlobalInt = 0;

	NodeID Directory::HashedPageCalculator::CalcNodeID(Address addr) const
	{
		int index = ((int)(addr / pageSize) % elementCount);
		DebugAssert(index >= 0 && index < (int)nodeSet.size());
		return nodeSet[index];
	}

	void Directory::HashedPageCalculator::Initialize(const RootConfigNode& node)
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
   void Directory::dump(HashMap<Memory::MessageID, const Memory::BaseMsg*> &m)
   {
      DumpMsgTemplate<Memory::MessageID>(m);
   }

   // performs a directory fetch from main memory of address a
	void Directory::PerformDirectoryFetch(Address a)
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
#ifdef MEMORY_DIRECTORY_DEBUG_VERBOSE
         printDebugInfo("PerformDirectoryFetch",*m,"");
#endif
		m->directoryLookup = false;
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
			target = memoryNodeCalc->CalcNodeID(a);
		}
		if(target == nodeID)
		{// if target == nodeID, it could mean that the block was evicted or invalidated
		   // and a response has already been sent, but we still need
			// to do this to get a proper read response, otherwise the program
			// will stall
#ifdef MEMORY_DIRECTORY_DEBUG_VERBOSE_OLD
         printDebugInfo("RemoteRead",*m,"PerformDirectoryFetch:Error",nodeID);
#endif
			OnRemoteRead(m, nodeID);//ERROR
		}
		else
		{
			NetworkMsg* nm = EM().CreateNetworkMsg(getDeviceID(), m->GeneratingPC());
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
	void Directory::EraseDirectoryShare(Address a, NodeID id)
	{
		DebugAssert(directoryData.find(a) != directoryData.end());
		BlockData& b = directoryData[a];
		if(b.owner == id)
		{
#ifdef MEMORY_DIRECTORY_DEBUG_DIRECTORY_DATA
         printEraseOwner("EraseDirectoryShare",a, id,"b.owner=InvalidNodeID");
#endif
			b.owner = InvalidNodeID;
			DebugAssert(b.sharers.find(id) == b.sharers.end());
		}
		else if(b.sharers.find(id) != b.sharers.end())
		{
#ifdef MEMORY_DIRECTORY_DEBUG_DIRECTORY_DATA
		   printDebugInfo("EraseDirectoryShare",a, id,"b.sharers.erase");
#endif
			b.sharers.erase(id);
		}
	}

	void Directory::AddDirectoryShare(Address a, NodeID id, bool exclusive)
	{
      DebugAssert(directoryData.find(a) != directoryData.end());
		BlockData& b = directoryData[a];
		DebugAssert(!exclusive || (b.sharers.size() == 0 && (b.owner == id || b.owner == InvalidNodeID)));
		if(b.owner == id || b.owner == InvalidNodeID)
		{
#ifdef MEMORY_DIRECTORY_DEBUG_DIRECTORY_DATA
         printDebugInfo("AddDirectoryShare",a, id, "b.owner=");
#endif
			b.owner = id;
		}
		else if(b.sharers.find(id) == b.sharers.end())
		{
#ifdef MEMORY_DIRECTORY_DEBUG_DIRECTORY_DATA
         printDebugInfo("AddDirectoryShare",a, id,
            ("exclusive="+to_string<bool>(exclusive)+" b.sharers.insert").c_str());
#endif
			b.sharers.insert(id);
		}
	}

	/**
	 * the message came from the local (cpu) side. It is a read msg
	 */
	void Directory::OnLocalRead(const ReadMsg* m)
	{
#ifdef MEMORY_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("LocRead",*m,"");
#endif
		DebugAssertWithMessageID(m,m->MsgID())
		NodeID remoteNode = directoryNodeCalc->CalcNodeID(m->addr);
#ifdef MEMORY_DIRECTORY_DEBUG_PENDING_LOCAL_READS
		printDebugInfo("LocalRead", *m,
		      ("pendingLocalReads.insert("+to_string<MessageID>(m->MsgID())+")").c_str());
#endif
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
			NetworkMsg* nm = EM().CreateNetworkMsg(getDeviceID(), m->GeneratingPC());
			DebugAssertWithMessageID(nm,m->MsgID())
			nm->destinationNode = remoteNode;
			nm->sourceNode = nodeID;
			nm->payloadMsg = forward;
			SendMsg(remoteConnectionID, nm, remoteSendTime);
		}
	}
	void Directory::OnLocalReadResponse(const ReadResponseMsg* m)
	{
#ifdef MEMORY_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("LocReadRes",*m,"");
#endif
		DebugAssertWithMessageID(m,m->MsgID())
		DebugAssertWithMessageID(pendingRemoteReads.find(m->solicitingMessage) != pendingRemoteReads.end(),m->MsgID())
		LookupData<ReadMsg>& d = pendingRemoteReads[m->solicitingMessage];
		DebugAssertWithMessageID(d.msg->MsgID() == m->solicitingMessage,m->MsgID())
		NetworkMsg* nm = EM().CreateNetworkMsg(getDeviceID(),d.msg->GeneratingPC());
		ReadResponseMsg* forward = (ReadResponseMsg*)EM().ReplicateMsg(m);
		nm->sourceNode = nodeID;
		nm->destinationNode = d.sourceNode;
		nm->payloadMsg = forward;
		forward->directoryLookup = false;
		EM().DisposeMsg(d.msg);
		EM().DisposeMsg(m);
#ifdef MEMORY_DIRECTORY_DEBUG_PENDING_REMOTE_READS
		printDebugInfo("LocReadRes",*m,
		      ("pendingRemoteReads.erase("+to_string<MessageID>(d.msg->MsgID())+")").c_str());

#endif
		pendingRemoteReads.erase(d.msg->MsgID());
		SendMsg(remoteConnectionID, nm, remoteSendTime);
	}
	void Directory::OnLocalWrite(const WriteMsg* m)
	{
#ifdef MEMORY_DIRECTORY_DEBUG_VERBOSE
      printDebugInfo("LocWrite",*m,"");
#endif
		DebugAssertWithMessageID(m,m->MsgID())
		NodeID id = directoryNodeCalc->CalcNodeID(m->addr);
		WriteResponseMsg* wrm = EM().CreateWriteResponseMsg(getDeviceID(),m->GeneratingPC());
		wrm->addr = m->addr;
		wrm->size = m->size;
		wrm->solicitingMessage = m->MsgID();
		SendMsg(localConnectionID, wrm, localSendTime);
		if(id == nodeID)
		{
			OnRemoteWrite(m,nodeID);
		}
		else
		{
			NetworkMsg* nm = EM().CreateNetworkMsg(getDeviceID(),m->GeneratingPC());
			DebugAssertWithMessageID(nm,m->MsgID())
			nm->sourceNode = nodeID;
			nm->destinationNode = id;
			nm->payloadMsg = m;
			SendMsg(remoteConnectionID, nm, remoteSendTime);
		}
	}
	void Directory::OnLocalEviction(const EvictionMsg* m)
	{
#ifdef MEMORY_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("LocEvic",*m,"");
#endif
		DebugAssertWithMessageID(m,m->MsgID())
#ifdef MEMORY_DIRECTORY_DEBUG_PENDING_EVICTION
      printDebugInfo("LocalEviction", *m,
         ("pendingEviction.insert("+to_string<Address>(m->addr)+")").c_str());
#endif
		DebugAssert(pendingEviction.find(m->addr) == pendingEviction.end())
		pendingEviction.insert(m->addr);
		EvictionResponseMsg* erm = EM().CreateEvictionResponseMsg(getDeviceID(),m->GeneratingPC());
		erm->addr = m->addr;
		erm->size = m->size;
		erm->solicitingMessage = m->MsgID();
		SendMsg(localConnectionID, erm, localSendTime);
		NodeID id = directoryNodeCalc->CalcNodeID(m->addr);
		if(id == nodeID)
		{
#ifdef MEMORY_DIRECTORY_DEBUG_VERBOSE_OLD
               printDebugInfo("RemoteEviction",*m,"LocalEviction",nodeID);
#endif
			OnRemoteEviction(m,nodeID);
		}
		else
		{
			NetworkMsg* nm = EM().CreateNetworkMsg(getDeviceID(),m->GeneratingPC());
			nm->destinationNode = id;
			nm->sourceNode = nodeID;
			nm->payloadMsg = m;
			SendMsg(remoteConnectionID, nm, remoteSendTime);
		}
	}
	void Directory::OnLocalInvalidateResponse(const InvalidateResponseMsg* m)
	{
#ifdef MEMORY_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("LocInvRes",*m,"");
#endif
		DebugAssertWithMessageID(m,m->MsgID())
		DebugAssertWithMessageID(pendingRemoteInvalidates.find(m->addr) != pendingRemoteInvalidates.end(),m->MsgID())
		LookupData<InvalidateMsg>& d = pendingRemoteInvalidates[m->addr];
		DebugAssertWithMessageID(d.msg->MsgID() == m->solicitingMessage,m->MsgID())
		if(nodeID != d.sourceNode)
		{
			NetworkMsg* nm = EM().CreateNetworkMsg(getDeviceID(),d.msg->GeneratingPC());
			nm->sourceNode = nodeID;
			nm->destinationNode = d.sourceNode;
			nm->payloadMsg = m;
			SendMsg(remoteConnectionID, nm, remoteSendTime);
		}
		else
		{
#ifdef MEMORY_DIRECTORY_DEBUG_VERBOSE_OLD
               printDebugInfo("RemoteInvalidateResponse",*m,"LocalInvalidateResponse",nodeID);
#endif
			OnRemoteInvalidateResponse(m,nodeID);
		}
		EM().DisposeMsg(d.msg);
		pendingRemoteInvalidates.erase(d.msg->addr);
	}
	void Directory::OnRemoteRead(const ReadMsg* m, NodeID src)
	{
#ifdef MEMORY_DIRECTORY_DEBUG_VERBOSE
      printDebugInfo("RemRead",*m,"",src);
#endif
		DebugAssertWithMessageID(m,m->MsgID())
		DebugAssertWithMessageID(!m->directoryLookup,m->MsgID())
#ifdef MEMORY_DIRECTORY_DEBUG_PENDING_REMOTE_READS
		printDebugInfo("RemoteRead", *m,
		      ("pendingRemoteReads.insert("+to_string<MessageID>(m->MsgID())+")").c_str());
#endif
		DebugAssertWithMessageID(pendingRemoteReads.find(m->MsgID()) == pendingRemoteReads.end(),m->MsgID())
		pendingRemoteReads[m->MsgID()].msg = m;
		pendingRemoteReads[m->MsgID()].sourceNode = src;
		SendMsg(localConnectionID, EM().ReplicateMsg(m), localSendTime);
	}
	void Directory::OnRemoteReadResponse(const ReadResponseMsg* m, NodeID src)
	{
#ifdef MEMORY_DIRECTORY_DEBUG_VERBOSE
      printDebugInfo("RemReadRes",*m,"",src);
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
					ReadResponseMsg* r = EM().CreateReadResponseMsg(getDeviceID(),i->second.msg->GeneratingPC());
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
#ifdef MEMORY_DIRECTORY_DEBUG_VERBOSE_OLD
               printDebugInfo("DirBlkRes",*r,"RemReadRes",nodeID);
#endif
						OnDirectoryBlockResponse(r,nodeID);
					}
					else
					{
						NetworkMsg* nm = EM().CreateNetworkMsg(getDeviceID(),i->second.msg->GeneratingPC());
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
			{  // there is no pending shared read on this address
				DebugAssertWithMessageID(m->exclusiveOwnership,m->MsgID())
				DebugAssertWithMessageID(m->blockAttached,m->MsgID())
				DebugAssertWithMessageID(directoryData[m->addr].owner == InvalidNodeID || directoryData[m->addr].owner == src,m->MsgID())
				if(directoryData[m->addr].sharers.size() == 0)
				{//send block on now
					ReadResponseMsg* response = (ReadResponseMsg*)EM().ReplicateMsg(m);
					response->directoryLookup = true;
					if(pendingDirectoryExclusiveReads[m->addr].sourceNode != nodeID)
					{
						NetworkMsg* net = EM().CreateNetworkMsg(getDeviceID(),m->GeneratingPC());
						net->sourceNode = nodeID;
						net->destinationNode = pendingDirectoryExclusiveReads[m->addr].sourceNode;
						net->payloadMsg = response;
						SendMsg(remoteConnectionID, net, lookupTime + remoteSendTime);
					}
					else
					{
						SendMsg(localConnectionID, response, lookupTime + localSendTime);
					}
					AddDirectoryShare(m->addr,pendingDirectoryExclusiveReads[m->addr].sourceNode,true);
					EM().DisposeMsg(pendingDirectoryExclusiveReads[m->addr].msg);
					pendingDirectoryExclusiveReads.erase(m->addr);
				} // if(directoryData[m->addr].sharers.size() == 0)
				else
				{//hold the block, send on once all invalidations are complete
					for(HashSet<NodeID>::iterator i = directoryData[m->addr].sharers.begin(); i != directoryData[m->addr].sharers.end(); i++)
					{
						InvalidateMsg* inv = EM().CreateInvalidateMsg(getDeviceID(),m->GeneratingPC());
						inv->addr = m->addr;
						inv->size = m->size;
						if(*i != nodeID)
						{
							NetworkMsg* net = EM().CreateNetworkMsg(getDeviceID(),m->GeneratingPC());
							net->destinationNode = *i;
							net->sourceNode = nodeID;
							net->payloadMsg = inv;
							SendMsg(remoteConnectionID, net, lookupTime + remoteSendTime);
						}
						else
						{
#ifdef MEMORY_DIRECTORY_DEBUG_VERBOSE_OLD
							printDebugInfo("RemoteInvalidate",*inv,"RemReadRes",nodeID);
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
			DebugAssertWithMessageID(directoryData[m->addr].owner == InvalidNodeID,m->MsgID());
			PerformDirectoryFetch(m->addr);
			EM().DisposeMsg(m);
		}
#ifdef MEMORY_DIRECTORY_DEBUG_DIRECTORY_DATA
      printDirectoryData(m->addr,m->MsgID());
#endif
	} // OnRemoteReadResponse

	void Directory::OnRemoteWrite(const WriteMsg* m, NodeID src)
	{
#ifdef MEMORY_DIRECTORY_DEBUG_VERBOSE
      printDebugInfo("RemWrite",*m,"",src);
#endif
		DebugAssertWithMessageID(m,m->MsgID())
		DebugAssertWithMessageID(nodeID == directoryNodeCalc->CalcNodeID(m->addr),m->MsgID())
		NodeID memoryNode = memoryNodeCalc->CalcNodeID(m->addr);
		if(src != nodeID)
		{
			WriteResponseMsg* wrm = EM().CreateWriteResponseMsg(getDeviceID(), m->GeneratingPC());
			wrm->addr = m->addr;
			wrm->size = m->size;
			wrm->solicitingMessage = m->MsgID();
			NetworkMsg* nm = EM().CreateNetworkMsg(getDeviceID(), m->GeneratingPC());
			nm->sourceNode = nodeID;
			nm->destinationNode = memoryNode;
			nm->payloadMsg = m;
			SendMsg(remoteConnectionID, nm, remoteSendTime);
			nm = EM().CreateNetworkMsg(getDeviceID(), m->GeneratingPC());
			nm->sourceNode = nodeID;
			nm->destinationNode = src;
			nm->payloadMsg = wrm;
			SendMsg(remoteConnectionID, nm, remoteSendTime);
		}
	}
	void Directory::OnRemoteWriteResponse(const WriteResponseMsg* m, NodeID src)
	{
#ifdef MEMORY_DIRECTORY_DEBUG_VERBOSE
      printDebugInfo("RemWriteRes",*m,"",src);
#endif
		DebugAssertWithMessageID(m,m->MsgID())
		EM().DisposeMsg(m);
	}
	void Directory::OnRemoteEviction(const EvictionMsg* m, NodeID src)
	{
#ifdef MEMORY_DIRECTORY_DEBUG_VERBOSE
      printDebugInfo("RemEvic",*m,"",src);
#endif
		DebugAssertWithMessageID(m,m->MsgID())
		DebugAssertWithMessageID(directoryData.find(m->addr) != directoryData.end(),m->MsgID())
		BlockData& b = directoryData[m->addr];
		if(b.owner == src)
		{
#ifdef MEMORY_DIRECTORY_DEBUG_DIRECTORY_DATA
         printDebugInfo("RemoteEviction",*m,"b.owner=InvalidNodeID",src);
#endif
			b.owner = InvalidNodeID;
		}
		else if(b.sharers.find(src) != b.sharers.end())
		{
#ifdef MEMORY_DIRECTORY_DEBUG_DIRECTORY_DATA
         printDebugInfo("RemoteEviction",*m,
            ("b.sharers.erase("+to_string<NodeID>(src)+")").c_str(),src);
#endif
			b.sharers.erase(src);
		}
		if(src == nodeID)
		{
#ifdef MEMORY_DIRECTORY_DEBUG_PENDING_EVICTION
      printDebugInfo("RemoteEviction", *m,
         ("pendingEviction.erase("+to_string<Address>(m->addr)+")").c_str(),src);
#endif
			DebugAssertWithMessageID(pendingEviction.find(m->addr) != pendingEviction.end(),m->MsgID())
			pendingEviction.erase(m->addr);
		}
		else
		{
			EvictionResponseMsg* rm = EM().CreateEvictionResponseMsg(getDeviceID(),m->GeneratingPC());
			rm->addr = m->addr;
			rm->size = m->size;
			rm->solicitingMessage = m->MsgID();
			NetworkMsg* nm = EM().CreateNetworkMsg(getDeviceID(),m->GeneratingPC());
			nm->destinationNode = src;
			nm->sourceNode = nodeID;
			nm->payloadMsg = rm;
			SendMsg(remoteConnectionID, nm, remoteSendTime);
		}
		if(m->blockAttached)
		{
			WriteMsg* wm = EM().CreateWriteMsg(getDeviceID(), m->GeneratingPC());
			wm->addr = m->addr;
			wm->size = m->size;
			wm->onCompletedCallback = NULL;
			NetworkMsg* nm = EM().CreateNetworkMsg(getDeviceID(), m->GeneratingPC());
			nm->sourceNode = nodeID;
			nm->destinationNode = memoryNodeCalc->CalcNodeID(m->addr);
			nm->payloadMsg = wm;
			SendMsg(remoteConnectionID, nm, remoteSendTime);
		}
#ifdef MEMORY_DIRECTORY_DEBUG_DIRECTORY_DATA
      printDirectoryData(m->addr,m->MsgID());
#endif
	}

	void Directory::OnRemoteEvictionResponse(const EvictionResponseMsg* m, NodeID src)
	{
#ifdef MEMORY_DIRECTORY_DEBUG_VERBOSE
      printDebugInfo("RemEvicRes",*m,"",src);
#endif
		DebugAssertWithMessageID(m,m->MsgID())
#ifdef MEMORY_DIRECTORY_DEBUG_PENDING_EVICTION
      printDebugInfo("RemoteEvictionResponse", *m,
         ("pendingEviction.erase("+to_string<Address>(m->addr)+")").c_str(),src);
#endif
		DebugAssertWithMessageID(pendingEviction.find(m->addr) != pendingEviction.end(),m->MsgID())
		pendingEviction.erase(m->addr);
		SendMsg(localConnectionID, m, localSendTime);
	}

	void Directory::OnRemoteInvalidate(const InvalidateMsg* m, NodeID src)
	{
#ifdef MEMORY_DIRECTORY_DEBUG_VERBOSE
      printDebugInfo("RemInv",*m,"",src);
#endif
		DebugAssertWithMessageID(m,m->MsgID())
#ifdef MEMORY_DIRECTORY_DEBUG_PENDING_REMOTE_INVALIDATES
      printDebugInfo("RemoteInvalidate", *m,
            ("pendingRemoteInvalidates.insert("+to_string<Address>(m->addr)+")").c_str());
#endif
		DebugAssertWithMessageID(pendingRemoteInvalidates.find(m->addr) == pendingRemoteInvalidates.end(),m->MsgID())
		pendingRemoteInvalidates[m->addr].sourceNode = src;
		pendingRemoteInvalidates[m->addr].msg = m;
		SendMsg(localConnectionID, EM().ReplicateMsg(m), localSendTime);
	}

	void Directory::OnRemoteInvalidateResponse(const InvalidateResponseMsg* m, NodeID src)
	{
#ifdef MEMORY_DIRECTORY_DEBUG_VERBOSE
      printDebugInfo("RemInvRes",*m,"",src);
#endif
		DebugAssertWithMessageID(m,m->MsgID())
		DebugAssertWithMessageID(directoryData.find(m->addr) != directoryData.end(),m->MsgID())
		BlockData& b = directoryData[m->addr];
		DebugAssertWithMessageID(!m->blockAttached || (b.owner == src) || (b.owner==InvalidNodeID),m->MsgID())
      DebugAssertWithMessageID(m->blockAttached || (b.sharers.find(src)!=b.sharers.end()) || b.owner==src || b.owner==InvalidNodeID,m->MsgID())
		if(b.owner == src)
		{
#ifdef MEMORY_DIRECTORY_DEBUG_DIRECTORY_DATA
         printDebugInfo("RemoteInvalidateResponse",*m,"b.owner=InvalidNodeID",src);
#endif
			b.owner = InvalidNodeID;
		}
      else if (b.sharers.find(src) != b.sharers.end())
		{
#ifdef MEMORY_DIRECTORY_DEBUG_DIRECTORY_DATA
			printDebugInfo("RemoteInvalidateResponse",*m,
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
			WriteMsg* wm = EM().CreateWriteMsg(getDeviceID(), m->GeneratingPC());
			wm->addr = m->addr;
			wm->size = m->size;
			wm->onCompletedCallback = NULL;
			NetworkMsg* nm = EM().CreateNetworkMsg(getDeviceID(), m->GeneratingPC());
			nm->sourceNode = nodeID;
			nm->destinationNode = memoryNodeCalc->CalcNodeID(m->addr);
			nm->payloadMsg = wm;
			SendMsg(remoteConnectionID, nm, remoteSendTime);
		}
		if(pendingDirectoryExclusiveReads.find(m->addr) != pendingDirectoryExclusiveReads.end())
		{
			if( (b.owner == InvalidNodeID && b.sharers.size() == 0) ||
				(b.owner == pendingDirectoryExclusiveReads[m->addr].sourceNode && b.sharers.size() == 0) ||
				(b.owner == InvalidNodeID && b.sharers.size() == 1 && b.sharers.find(pendingDirectoryExclusiveReads[m->addr].sourceNode) != b.sharers.end()))
			{
#ifdef MEMORY_DIRECTORY_DEBUG_DIRECTORY_DATA
			   printDebugInfo("RemoteInvalidateResponse",*m,"b.sharers.clear()",src);
            printDebugInfo("RemoteInvalidateResponse",*m,
               ("b.owner="+to_string<NodeID>(pendingDirectoryExclusiveReads[m->addr].sourceNode)).c_str(),
               src);
#endif
				b.sharers.clear();
				b.owner = pendingDirectoryExclusiveReads[m->addr].sourceNode;
				ReadResponseMsg* rm = EM().CreateReadResponseMsg(getDeviceID(),pendingDirectoryExclusiveReads[m->addr].msg->GeneratingPC());
				rm->addr = m->addr;
				rm->size = m->size;
				rm->blockAttached = true;
				rm->solicitingMessage = pendingDirectoryExclusiveReads[m->addr].msg->MsgID();
				rm->directoryLookup = true;
				rm->exclusiveOwnership = true;
				rm->satisfied = true;
				if(pendingDirectoryExclusiveReads[m->addr].sourceNode == nodeID)
				{
					SendMsg(localConnectionID, rm, localSendTime + satisfyTime);
				}
				else
				{
					NetworkMsg* n = EM().CreateNetworkMsg(getDeviceID(), pendingDirectoryExclusiveReads[m->addr].msg->GeneratingPC());
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
#ifdef MEMORY_DIRECTORY_DEBUG_DIRECTORY_DATA
      printDirectoryData(m->addr,m->MsgID());
#endif
		EM().DisposeMsg(m);
	}

	void Directory::OnDirectoryBlockRequest(const ReadMsg* m, NodeID src)
	{
#ifdef MEMORY_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("DirBlkReq",*m,"",src);
#endif
		DebugAssertWithMessageID(m,m->MsgID())
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
				NetworkMsg* n = EM().CreateNetworkMsg(getDeviceID(),m->GeneratingPC());
				ReadResponseMsg* rm = EM().CreateReadResponseMsg(getDeviceID(),m->GeneratingPC());
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
#ifdef MEMORY_DIRECTORY_DEBUG_DIRECTORY_DATA
         printDirectoryData(m->addr,m->MsgID());
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
#ifdef MEMORY_DIRECTORY_DEBUG_DIRECTORY_DATA
      printDirectoryData(m->addr,m->MsgID());
#endif
	}
	void Directory::OnDirectoryBlockResponse(const ReadResponseMsg* m, NodeID src)
	{
#ifdef MEMORY_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("DirBlkRes",*m,"",src);
#endif
		DebugAssertWithMessageID(m,m->MsgID())

#ifdef MEMORY_DIRECTORY_DEBUG_PENDING_DIRECTORY_SHARED_READS
		printPendingDirectorySharedReads();
#endif
#ifdef MEMORY_DIRECTORY_DEBUG_PENDING_LOCAL_READS
		printPendingLocalReads();
#endif
		// check that m->solicitingMessage is in pendingLocalReads before accessing it
		// error here means that we expect there to be a message in pendingLocalReads,
		// but the message is not there
      DebugAssertWithMessageID(pendingLocalReads.find(m->solicitingMessage) != pendingLocalReads.end(),m->MsgID())
		const ReadMsg* ref = pendingLocalReads[m->solicitingMessage];

		if(!m->satisfied)
		{
#ifdef MEMORY_DIRECTORY_DEBUG_PENDING_LOCAL_READS
		   printDebugInfo("DirBlkRes",*m,
		         ("pendingLocalReads.erase("+to_string<MessageID>(m->solicitingMessage)+")").c_str());
#endif
			pendingLocalReads.erase(m->solicitingMessage);
#ifdef MEMORY_DIRECTORY_DEBUG_VERBOSE_OLD
               printDebugInfo("LocalRead",*m,"DirBlkRes");
#endif
			OnLocalRead(ref);
			return;
		}
		ReadResponseMsg* r = EM().CreateReadResponseMsg(getDeviceID(),ref->GeneratingPC());
		r->addr = ref->addr;
		r->blockAttached = m->blockAttached;
		r->directoryLookup = false;
		r->exclusiveOwnership = m->exclusiveOwnership;
		r->satisfied = true;
		r->size = ref->size;
		r->solicitingMessage = ref->MsgID();
		EM().DisposeMsg(ref);
#ifdef MEMORY_DIRECTORY_DEBUG_PENDING_LOCAL_READS
		printDebugInfo("DirBlkRes", *m,
		   ("pendingLocalReads.erase("+m->solicitingMessage+")").c_str());
#endif
		pendingLocalReads.erase(m->solicitingMessage);

		EM().DisposeMsg(m);
		SendMsg(localConnectionID, r, satisfyTime + localSendTime);
	}

	void Directory::Initialize(EventManager* em, const RootConfigNode& config, const std::vector<Connection*>& connectionSet)
	{
		BaseMemDevice::Initialize(em,config,connectionSet);
		localConnectionID = remoteConnectionID = -1;
		for(int i = 0; i < ConnectionCount(); i++)
		{
			if(GetConnection(i).Name() == "LocalConnection")
			{
				localConnectionID = i;
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
		DebugAssert(localConnectionID != -1 && remoteConnectionID != -1);
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
		const RootConfigNode& memCalc = Config::GetSubRoot(config,"MemoryNodeCalculator");
		if(Config::GetString(dirCalc, "Type") == "HashedPageCalculator")
		{
			directoryNodeCalc = new HashedPageCalculator();
		}
		else
		{
			DebugFail("Unknown directory node calculator type");
		}
		if(Config::GetString(memCalc, "Type") == "HashedPageCalculator")
		{
			memoryNodeCalc = new HashedPageCalculator();
		}
		else
		{
			DebugFail("Unknown memory node calculator type");
		}
		directoryNodeCalc->Initialize(dirCalc);
		memoryNodeCalc->Initialize(memCalc);

      messagesReceived = 0;
	}  // Directory::Initialize()
	/**
	 * this is used for checkpoint purposes
	 */
	void Directory::DumpRunningState(RootConfigNode& node)
	{}
	/**
	 * put anything here that you might want to output to the report file
	 */
	void Directory::DumpStats(std::ostream& out)
	{
	   out << "messagesReceived:" << messagesReceived << std::endl;
	}
	/**
	 * Handles all the incoming messages from outside of the directory.
	 * The message can come from the cache side or from the network.
	 */
	void Directory::RecvMsg(const BaseMsg* msg, int connectionID)
	{
	   messagesReceived++;
#ifdef MEMORY_DIRECTORY_DEBUG_MSG_COUNT
	   cout << "Directory::RecvMsg: " << memoryDirectoryGlobalInt++ << ' ' << endl;
#endif
		DebugAssert(msg);
		if(connectionID == localConnectionID)
		{
			switch(msg->Type())
			{
			case(mt_Read):
				{
#ifdef MEMORY_DIRECTORY_DEBUG_VERBOSE_OLD
			      printDebugInfo("LocalRead",*msg,"RecvMsg");
#endif
					OnLocalRead((ReadMsg*)msg);
					break;
				}
			case(mt_ReadResponse):
				{
#ifdef MEMORY_DIRECTORY_DEBUG_VERBOSE_OLD
               printDebugInfo("LocReadRes",*msg,"RecvMsg");
#endif
					OnLocalReadResponse((ReadResponseMsg*)msg);
					break;
				}
			case(mt_Write):
				{
#ifdef MEMORY_DIRECTORY_DEBUG_VERBOSE_OLD
               printDebugInfo("LocalWrite",*msg,"RecvMsg");
#endif
					OnLocalWrite((WriteMsg*)msg);
					break;
				}
			case(mt_Eviction):
				{
#ifdef MEMORY_DIRECTORY_DEBUG_VERBOSE_OLD
               printDebugInfo("LocalEviction",*msg,"RecvMsg");
#endif
					OnLocalEviction((EvictionMsg*)msg);
					break;
				}
			case(mt_InvalidateResponse):
				{
#ifdef MEMORY_DIRECTORY_DEBUG_VERBOSE_OLD
               printDebugInfo("LocalInvalidateResponse",*msg,"RecvMsg");
#endif
					OnLocalInvalidateResponse((InvalidateResponseMsg*)msg);
					break;
				}
			default:
				DebugFail("Msg Type not handled");
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
#ifdef MEMORY_DIRECTORY_DEBUG_VERBOSE_OLD
               printDebugInfo("DirBlkReq",*m,"RecvMsg",src);
#endif
						OnDirectoryBlockRequest(m,src);
					}
					else
					{
#ifdef MEMORY_DIRECTORY_DEBUG_VERBOSE_OLD
               printDebugInfo("RemoteRead",*m,"RecvMsg",src);
#endif
						OnRemoteRead(m,src);
					}
					break;
				}
			case(mt_ReadResponse):
				{
					ReadResponseMsg* m = (ReadResponseMsg*)payload;
					if(m->directoryLookup)
					{
#ifdef MEMORY_DIRECTORY_DEBUG_VERBOSE_OLD
               printDebugInfo("DirBlkRes",*m,"RecvMsg",src);
#endif
						OnDirectoryBlockResponse(m,src);
					}
					else
					{
#ifdef MEMORY_DIRECTORY_DEBUG_VERBOSE_OLD
               printDebugInfo("RemReadRes",*m,"RecvMsg",src);
#endif
						OnRemoteReadResponse(m,src);
					}
					break;
				}
			case(mt_WriteResponse):
				{
               WriteResponseMsg* m = (WriteResponseMsg*)payload;
#ifdef MEMORY_DIRECTORY_DEBUG_VERBOSE_OLD
               printDebugInfo("RemoteWriteResponse",*m,"RecvMsg",src);
#endif
					OnRemoteWriteResponse(m,src);
					break;
				}
			case(mt_Eviction):
				{
               EvictionMsg* m = (EvictionMsg*)payload;
#ifdef MEMORY_DIRECTORY_DEBUG_VERBOSE_OLD
               printDebugInfo("RemoteEviction",*m,"RecvMsg",src);
#endif
					OnRemoteEviction(m,src);
					break;
				}
			case(mt_EvictionResponse):
				{
               EvictionResponseMsg* m = (EvictionResponseMsg*)payload;
#ifdef MEMORY_DIRECTORY_DEBUG_VERBOSE_OLD
               printDebugInfo("RemoteEvictionResponse",*m,"RecvMsg",src);
#endif
					OnRemoteEvictionResponse(m,src);
					break;
				}
			case(mt_Invalidate):
            {
               InvalidateMsg* m = (InvalidateMsg*)payload;
#ifdef MEMORY_DIRECTORY_DEBUG_VERBOSE_OLD
               printDebugInfo("RemoteInvalidate",*m,"RecvMsg",src);
#endif
					OnRemoteInvalidate(m,src);
					break;
				}
			case(mt_InvalidateResponse):
				{
			      InvalidateResponseMsg* m = (InvalidateResponseMsg*)payload;
#ifdef MEMORY_DIRECTORY_DEBUG_VERBOSE_OLD
               printDebugInfo("RemoteInvalidateResponse",*m,"RecvMsg",src);
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
	void Directory::printPendingDirectorySharedReads()
	{
	   HashMultiMap<Address, LookupData<ReadMsg> >::const_iterator myIterator;


	   myIterator = pendingDirectorySharedReads.begin();
      cout << "Directory::printPendingDirectorySharedReads: ";

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
	void Directory::printPendingLocalReads()
	{
      HashMap<MessageID, const ReadMsg*>::const_iterator myIterator;

      myIterator = pendingLocalReads.begin();

      cout << "Directory::printPendingLocalReads: ";

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

   void Directory::printDebugInfo(const char* fromMethod, const BaseMsg &myMessage,
         const char* operation)
   {
      printBaseMemDeviceDebugInfo("4", fromMethod, myMessage, operation);
   }

   void Directory::printDebugInfo(const char* fromMethod, const BaseMsg &myMessage,
         const char* operation, NodeID src)
   {
      printBaseMemDeviceDebugInfo("4", fromMethod, myMessage, operation, src);
   }

   void Directory::printDebugInfo(const char* fromMethod,Address addr,NodeID id,const char* operation)
   {
      cout << setw(17) << " " // account for spacing from src and msgSrc
            << " dst=" << setw(3) << getDeviceID()
            << setw(11) << " "   // account for spacing from msgID
            << " addr=" << addr
            << " " << fromMethod
            << " " << operation << "(" << id << ")"
            << endl;
   }

   void Directory::printDirectoryData(Address myAddress, MessageID myMessageID)
   {
      DebugAssert(directoryData.find(myAddress)!=directoryData.end());

      bool isSharedBusy = (pendingDirectorySharedReads.find(myAddress)!=pendingDirectorySharedReads.end());
      bool isExclusiveBusy = (pendingDirectoryExclusiveReads.find(myAddress)!=pendingDirectoryExclusiveReads.end());
      
      directoryData[myAddress].print(myAddress, myMessageID,isSharedBusy, isExclusiveBusy);
   }

   void Directory::printEraseOwner(const char* fromMethod,Address addr,NodeID id,const char* operation)
   {
      cout << setw(17) << " " // account for spacing from src and msgSrc
            << " dst=" << setw(3) << getDeviceID()
            << setw(10) << " "   // account for spacing from msgID
            << " addr=" << addr
            << " " << fromMethod
            << " nodeID=" << id
            << " " << operation
            << endl;
   }
}