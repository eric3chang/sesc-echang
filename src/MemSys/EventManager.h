#pragma once
#include "MSTypes.h"
#include "HashContainers.h"
#include "Pool.h"
#include "StoredFunctionCall.h"
#include "NetworkMsg.h"
#include <vector>
#include "EvictionBusyAckMsg.h"

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
	protected:
		Pool<ReadMsg> readPool;
		Pool<WriteMsg> writePool;
		Pool<InvalidateMsg> invalidatePool;
		Pool<EvictionMsg> evictionPool;
		Pool<ReadResponseMsg> readResponsePool;
		Pool<WriteResponseMsg> writeResponsePool;
		Pool<InvalidateResponseMsg> invalidateResponsePool;
		Pool<EvictionResponseMsg> evictionResponsePool;
		Pool<EvictionBusyAckMsg> evictionBusyAckPool;
		Pool<NetworkMsg> networkPool;
	public:
		ReadMsg* CreateReadMsg(DeviceID devID, Address generatingPC = 0);
		WriteMsg* CreateWriteMsg(DeviceID devID, Address generatingPC = 0);
		InvalidateMsg* CreateInvalidateMsg(DeviceID devID, Address generatingPC = 0);
		EvictionMsg* CreateEvictionMsg(DeviceID devID, Address generatingPC = 0);
		ReadResponseMsg* CreateReadResponseMsg(DeviceID devID, Address generatingPC = 0);
		WriteResponseMsg* CreateWriteResponseMsg(DeviceID devID, Address generatingPC = 0);
		InvalidateResponseMsg* CreateInvalidateResponseMsg(DeviceID devID, Address generatingPC = 0);
		EvictionResponseMsg* CreateEvictionResponseMsg(DeviceID devID, Address generatingPC = 0);
		EvictionBusyAckMsg* CreateEvictionBusyAckMsg(DeviceID devID, Address generatingPC = 0);
		NetworkMsg* CreateNetworkMsg(DeviceID devID, Address generatingPC = 0);

		BaseMsg* ReplicateMsg(const BaseMsg* msg);

		void DisposeMsg(const BaseMsg* msg);

		void ScheduleEvent(StoredFunctionBase* f, TimeDelta dT);
		void ScheduleEventAbsolute(StoredFunctionBase* f, TickTime time);
		void ScheduleMessageTransmit(const BaseMsg* msg, BaseMemDevice* from, BaseMemDevice* to, int toConnectionID, TimeDelta dT);
		void ProcessTick();
		TickTime CurrentTick();
	};
}
