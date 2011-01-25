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

		void FillBaseNakMsg(BaseNakMsg* m, DeviceID devID, Address generatingPC);
		void FillEvictionMsg(EvictionMsg* m, DeviceID devID, Address generatingPC);
		void FillEvictionResponseMsg(EvictionResponseMsg* m, DeviceID devID, Address generatingPC);
		void FillInvalidateMsg(InvalidateMsg* m, DeviceID devID, Address generatingPC);
		void FillInvalidateResponseMsg(InvalidateResponseMsg* m, DeviceID devID, Address generatingPC);
		void FillReadMsg(ReadMsg* m, DeviceID devID, Address generatingPC);
		void FillReadResponseMsg(ReadResponseMsg* m, DeviceID devID, Address generatingPC);
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

      void InitializeBaseNakMsg(BaseNakMsg* baseNak, const ReadMsg* read);
      void InitializeEvictionMsg(EvictionMsg* eviction, const ReadMsg* read);
      void InitializeEvictionMsg(EvictionMsg* copy, const EvictionMsg* original);
      void InitializeEvictionResponseMsg(EvictionResponseMsg* evictionResponse, const EvictionMsg* eviction);
      void InitializeEvictionResponseMsg(EvictionResponseMsg* copy, const EvictionResponseMsg* original);
      void InitializeEvictionResponseMsg(EvictionResponseMsg* evictionResponse, const ReadMsg* read);
      void InitializeInvalidateMsg(InvalidateMsg* invalidate, const ReadMsg* read);
      void InitializeInvalidateResponseMsg(InvalidateResponseMsg* copy, const InvalidateResponseMsg* original);
      void InitializeReadMsg(ReadMsg* copy, const ReadMsg* original);
      void InitializeReadMsg(ReadMsg* read, const ReadResponseMsg* readResponse);
		void InitializeReadResponseMsg(ReadResponseMsg* readResponse, const EvictionMsg* eviction);
		void InitializeReadResponseMsg(ReadResponseMsg* readResponse, const InterventionMsg* intervention);
		void InitializeReadResponseMsg(ReadResponseMsg* readResponse, const ReadMsg* read);
		void InitializeReadResponseMsg(ReadResponseMsg* copy, const ReadResponseMsg* original);
		void InitializeWriteMsg(WriteMsg* write, const EvictionMsg* eviction);
		void InitializeWriteMsg(WriteMsg* write, const ReadResponseMsg* readResponse);

		BaseMsg* ReplicateMsg(const BaseMsg* msg);

		void DisposeMsg(const BaseMsg* msg);

		void ScheduleEvent(StoredFunctionBase* f, TimeDelta dT);
		void ScheduleEventAbsolute(StoredFunctionBase* f, TickTime time);
		void ScheduleMessageTransmit(const BaseMsg* msg, BaseMemDevice* from, BaseMemDevice* to, int toConnectionID, TimeDelta dT);
		void ProcessTick();
		TickTime CurrentTick();
	};
}
