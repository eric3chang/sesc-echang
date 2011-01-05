#pragma once
#include "MSTypes.h"
#include "HashContainers.h"
#include "Pool.h"
#include "StoredFunctionCall.h"
#include "NetworkMsg.h"
#include <vector>

namespace Memory
{
	class BaseMsg;
	class ReadMsg;
	class WriteMsg;
	class InvalidateMsg;
	class EvictionMsg;
	class ReadResponseMsg;
	class WriteResponseMsg;
	class InvalidateResponseMsg;
	class EvictionResponseMsg;

	// additional messages added for Origin protocol
	class BaseNakMsg;
   class CacheNakMsg;
   class DirectoryNakMsg;
   class InterventionMsg;
   class InvalidateAckMsg;
   class ReadReplyMsg;
   class SpeculativeReplyMsg;
	class TransferMsg;
	class WritebackMsg;
	class WritebackAckMsg;
	class WritebackRequestMsg;

	class BaseMemDevice;
	class EventManager
	{
		HashMap<TickTime,std::vector<StoredFunctionBase*>*> scheduleSet;
		Pool<std::vector<StoredFunctionBase*> > containerPool;
		TickTime currentTick;
		MessageID currentMsgStamp;
	public:
		EventManager(MessageID firstID, TickTime startTick = 0);

	private:
		void DeliverMsg(BaseMemDevice* destination, const BaseMsg* msg, int connection);
		typedef PooledFunctionGenerator<StoredClassFunction3<EventManager, BaseMemDevice*, const BaseMsg*, int, &EventManager::DeliverMsg> > CBDeliverMsg;
		CBDeliverMsg cbDeliverMsg;

		void InitializeBaseNakMsg(BaseNakMsg* m, DeviceID devID, Address generatingPC);
		void InitializeEvictionMsg(EvictionMsg* m, DeviceID devID, Address generatingPC);
		void InitializeEvictionResponseMsg(EvictionResponseMsg* m, DeviceID devID, Address generatingPC);
		void InitializeInvalidateMsg(InvalidateMsg* m, DeviceID devID, Address generatingPC);
		void InitializeInvalidateResponseMsg(InvalidateResponseMsg* m, DeviceID devID, Address generatingPC);
		void InitializeReadMsg(ReadMsg* m, DeviceID devID, Address generatingPC);
		void InitializeReadResponseMsg(ReadResponseMsg* m, DeviceID devID, Address generatingPC);
	protected:
		Pool<ReadMsg> readPool;
		Pool<WriteMsg> writePool;
		Pool<InvalidateMsg> invalidatePool;
		Pool<EvictionMsg> evictionPool;
		Pool<ReadResponseMsg> readResponsePool;
		Pool<WriteResponseMsg> writeResponsePool;
		Pool<InvalidateResponseMsg> invalidateResponsePool;
		Pool<EvictionResponseMsg> evictionResponsePool;
		Pool<NetworkMsg> networkPool;

		Pool<CacheNakMsg> cacheNakPool;
		Pool<DirectoryNakMsg> directoryNakPool;
		Pool<InterventionMsg> interventionPool;
		Pool<InvalidateAckMsg> invalidateAckPool;
		Pool<ReadReplyMsg> readReplyPool;
		Pool<SpeculativeReplyMsg> speculativeReplyPool;
		Pool<TransferMsg> transferPool;
		Pool<WritebackMsg> writebackPool;
		Pool<WritebackAckMsg> writebackAckPool;
		Pool<WritebackRequestMsg> writebackRequestPool;
	public:
		ReadMsg* CreateReadMsg(DeviceID devID, Address generatingPC = 0);
		WriteMsg* CreateWriteMsg(DeviceID devID, Address generatingPC = 0);
		InvalidateMsg* CreateInvalidateMsg(DeviceID devID, Address generatingPC = 0);
		EvictionMsg* CreateEvictionMsg(DeviceID devID, Address generatingPC = 0);
		ReadResponseMsg* CreateReadResponseMsg(DeviceID devID, Address generatingPC = 0);
		WriteResponseMsg* CreateWriteResponseMsg(DeviceID devID, Address generatingPC = 0);
		InvalidateResponseMsg* CreateInvalidateResponseMsg(DeviceID devID, Address generatingPC = 0);
		EvictionResponseMsg* CreateEvictionResponseMsg(DeviceID devID, Address generatingPC = 0);
      NetworkMsg* CreateNetworkMsg(DeviceID devID, Address generatingPC = 0);

      CacheNakMsg* CreateCacheNakMsg(DeviceID devID, Address generatingPC = 0);
      DirectoryNakMsg* CreateDirectoryNakMsg(DeviceID devID, Address generatingPC = 0);
      InterventionMsg* CreateInterventionMsg(DeviceID devID, Address generatingPC = 0);
      InvalidateAckMsg* CreateInvalidateAckMsg(DeviceID devID, Address generatingPC = 0);
      ReadReplyMsg* CreateReadReplyMsg(DeviceID devID, Address generatingPC = 0);
      SpeculativeReplyMsg* CreateSpeculativeReplyMsg(DeviceID devID, Address generatingPC = 0);
      TransferMsg* CreateTransferMsg(DeviceID devID, Address generatingPC = 0);
      WritebackMsg* CreateWritebackMsg(DeviceID devID, Address generatingPC = 0);
      WritebackAckMsg* CreateWritebackAckMsg(DeviceID devID, Address generatingPC = 0);
      WritebackRequestMsg* CreateWritebackRequestMsg(DeviceID devID, Address generatingPC = 0);

		BaseMsg* ReplicateMsg(const BaseMsg* msg);

		void DisposeMsg(const BaseMsg* msg);

		void ScheduleEvent(StoredFunctionBase* f, TimeDelta dT);
		void ScheduleEventAbsolute(StoredFunctionBase* f, TickTime time);
		void ScheduleMessageTransmit(const BaseMsg* msg, BaseMemDevice* from, BaseMemDevice* to, int toConnectionID, TimeDelta dT);
		void ProcessTick();
		TickTime CurrentTick();
	};
}
