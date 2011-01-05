#include "SESCProcessorInterface.h"
#include "../Messages/AllMessageTypes.h"
#include "../EventManager.h"
#include "../Connection.h"
#include "../MSTypes.h"
#ifdef SYSTEM_SESC
#include "MemRequest.h"
#endif
#undef ID

// enable to show debug outputs
//#define MEMORY_SESC_PROCESSOR_INTERFACE_DEBUG_COUNTER

namespace Memory
{
#ifdef MEMORY_SESC_PROCESSOR_INTERFACE_DEBUG_COUNTER
   unsigned int memorySescProcessorInterfaceDebugCounter = 0;
#endif

	SESCProcessorInterface::SESCProcessorInterface()
#ifdef SYSTEM_SESC
		: MemObj("","")//required for compilation, because SESC didn't implement MemObj(void), but stupidly defined it
#endif
	{
#ifndef SYSTEM_SESC
		DebugFail("SESC processor interface created outside of SESC system environment");
#endif
	}
#ifdef SYSTEM_SESC
	SESCProcessorInterface::SESCProcessorInterface(const char *section, const char *sName)
		: MemObj(section,sName)
	{}
	Time_t SESCProcessorInterface::getNextFreeCycle() const
	{
		return globalClock;
	}
	void SESCProcessorInterface::access(MemRequest *mreq)
	{
		DebugAssert(mreq);
		if(mreq->dinst)
		{
			TMInterface::ObserveAccess(mreq->dinst->instructionStamp);
			if(TMInterface::IsDeadInstruction())
			{
				mreq->goUp(0);
				return;
			}
			TimeDelta_t retryDelay = (TimeDelta_t)TMInterface::RetryTime();
			if(retryDelay != 0)
			{
				accessCB::schedule(retryDelay,this,mreq);
				return;
			}
		}
		else
		{
			mreq->goUp(10);
			return;
		}
		mreq->setClockStamp((Time_t) - 1);
		if(mreq->getPAddr() <= 1024)
		{
			mreq->goUp(10); 
			return;
		}
		BaseMsg* msg = NULL;
		switch(mreq->getMemOperation())
		{
		case MemRead:
			{
			   readCount++;
				ReadMsg* r = EM().CreateReadMsg(this->GetDeviceID(),(Address)(mreq->dinst->getInst()->addr));
				r->addr = (Address)mreq->getPAddr();
				r->size = 1;
				r->alreadyHasBlock = false;
				r->onCompletedCallback = NULL;
				r->requestingExclusive = false;
				msg = r;
				break;
			}
		case MemWrite:
			{
			   writeCount++;
				WriteMsg* w = EM().CreateWriteMsg(this->GetDeviceID(),(Address)(mreq->dinst->getInst()->addr));
				msg = w;
				w->addr = (Address)mreq->getPAddr();
				w->size = 1;
				w->onCompletedCallback = NULL;
				break;
			}
		default:
			DebugFail("Unexpected MemRequest type");
			break;
		}
		DebugAssert(msg);
		DebugAssert(pendingRequests.find(msg->MsgID()) == pendingRequests.end());
		pendingRequests[msg->MsgID()] = mreq;
		toDevice->SendMsg(msg);
	}
	void SESCProcessorInterface::returnAccess(MemRequest *mreq)
	{
		DebugAssert(0);
	}
	void SESCProcessorInterface::invalidate(PAddr addr, ushort size, MemObj *oc)
	{
		DebugAssert(0);
	}
	void SESCProcessorInterface::doInvalidate(PAddr addr, ushort size)
	{
		DebugAssert(0);
	}
	bool SESCProcessorInterface::canAcceptStore(PAddr addr)
	{
		return pendingRequests.size() < maxTransferCount;
	}
	bool SESCProcessorInterface::canAcceptLoad(PAddr addr)
	{
		return pendingRequests.size() < maxTransferCount;
	}
	void SESCProcessorInterface::dump() const{}
#endif
	void SESCProcessorInterface::Initialize(EventManager* em, const RootConfigNode& config, const std::vector<Connection*>& connectionSet)
	{
#ifdef SYSTEM_SESC
		BaseMemDevice::Initialize(em,config,connectionSet);
		DebugAssert(connectionSet.size() == 1);
		toDevice = connectionSet[0];
		maxTransferCount = 32;
#endif
		readCount = 0;
		writeCount = 0;
	}
	void SESCProcessorInterface::DumpRunningState(RootConfigNode& node){}
	void SESCProcessorInterface::DumpStats(std::ostream& out)
	{
	   out << "readCount:" << readCount << std::endl;
	   out << "writeCount:" << writeCount << std::endl;
	   out << "totalOperations:" << readCount+writeCount << std::endl;
	}
	void SESCProcessorInterface::RecvMsg(const BaseMsg* msg, int connectionID)
	{
#ifdef MEMORY_SESC_PROCESSOR_INTERFACE_DEBUG_COUNTER
      std::cout << "SESCProcessorInterface::RecvMsg: memorySescProcessorInterfaceDebugCounter="
         << memorySescProcessorInterfaceDebugCounter++ << std::endl;
#endif
#ifdef SYSTEM_SESC
		DebugAssert(msg);
		MessageID id;
		switch(msg->Type())
		{
		case(mt_ReadResponse):
			id = ((ReadResponseMsg*)msg)->solicitingMessage;
			break;
		case(mt_WriteResponse):
			id = ((WriteResponseMsg*)msg)->solicitingMessage;
			break;
		default:
			DebugFail("Unexpected msg type arrived at SESC processor interface");
		}
		DebugAssert(pendingRequests.find(id) != pendingRequests.end());
		pendingRequests[id]->goUp(0);
		pendingRequests.erase(id);
		EM().DisposeMsg(msg);
#endif
	}
}
