#pragma once
#include "BaseMemDevice.h"
#ifdef SYSTEM_SESC
#include "../HashContainers.h"
#include "../MSTypes.h"
#include "MemObj.h"
#endif

namespace Memory
{
#ifdef SYSTEM_SESC
	class SESCProcessorInterface : public BaseMemDevice, public MemObj
#else
	class SESCProcessorInterface : public BaseMemDevice
#endif
	{
	   unsigned int readCount;
	   unsigned int writeCount;

#ifdef SYSTEM_SESC
		HashMap<MessageID,MemRequest*> pendingRequests;
		Connection* toDevice;
		size_t maxTransferCount;
#endif
		SESCProcessorInterface();//restricts creation by factory
	public:
#ifdef SYSTEM_SESC
		SESCProcessorInterface(const char *section, const char *sName);
		virtual Time_t getNextFreeCycle() const;
		virtual void access(MemRequest *mreq);
		virtual void returnAccess(MemRequest *mreq);
		virtual void invalidate(PAddr addr, ushort size, MemObj *oc);
		virtual void doInvalidate(PAddr addr, ushort size);
		virtual bool canAcceptStore(PAddr addr);
		virtual bool canAcceptLoad(PAddr addr);
		virtual void dump() const;
	private:
		typedef CallbackMember1<SESCProcessorInterface, MemRequest*,&SESCProcessorInterface::access> accessCB;
	public:
#endif
		unsigned int GetReadCount();
		unsigned int GetWriteCount();
		unsigned int GetTotalOperations();

		virtual void Initialize(EventManager* em, const RootConfigNode& config, const std::vector<Connection*>& connectionSet);
		virtual void DumpRunningState(RootConfigNode& node);
		virtual void DumpStats(std::ostream& out);
		virtual void RecvMsg(const BaseMsg* msg, int connectionID);
	};
}
