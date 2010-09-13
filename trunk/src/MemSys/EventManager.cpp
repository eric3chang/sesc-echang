#include "EventManager.h"
#include "Messages/AllMessageTypes.h"
#include "Devices/BaseMemDevice.h"
#include "StoredFunctionCall.h"
#include "Debug.h"

namespace Memory
{
	EventManager::EventManager(MessageID firstID, TickTime startTick)
	{
		currentMsgStamp = firstID;
		currentTick = startTick;
	}
	void EventManager::DeliverMsg(BaseMemDevice* destination, const BaseMsg* msg, int connection)
	{
		destination->RecvMsg(msg,connection);
	}
	ReadMsg* EventManager::CreateReadMsg(DeviceID devID, Address generatingPC)
	{
		ReadMsg* m = readPool.Take();
		m->directoryLookup = false;
		m->originalRequestingNode = InvalidNodeID;
		m->isIntervention = false;
      //m->isSpeculative = false;
		m->SetIDInfo(currentMsgStamp++,devID,generatingPC);
		return m;
	}
	WriteMsg* EventManager::CreateWriteMsg(DeviceID devID, Address generatingPC)
	{
		WriteMsg* m = writePool.Take();
		m->SetIDInfo(currentMsgStamp++,devID,generatingPC);
		return m;
	}
	InvalidateMsg* EventManager::CreateInvalidateMsg(DeviceID devID, Address generatingPC)
	{
		InvalidateMsg* m = invalidatePool.Take();
		m->SetIDInfo(currentMsgStamp++,devID,generatingPC);
      m->newOwner = InvalidNodeID;
		return m;
	}
   InvalidateSharerMsg* EventManager::CreateInvalidateSharerMsg(DeviceID devID, Address generatingPC)
   {
      InvalidateSharerMsg* m = invalidateSharerPool.Take();
      m->SetIDInfo(currentMsgStamp++,devID,generatingPC);
      m->addr = 0;
      return m;
   }
	EvictionMsg* EventManager::CreateEvictionMsg(DeviceID devID, Address generatingPC)
	{
		EvictionMsg* m = evictionPool.Take();
		m->SetIDInfo(currentMsgStamp++,devID,generatingPC);
		return m;
	}
	ReadResponseMsg* EventManager::CreateReadResponseMsg(DeviceID devID, Address generatingPC)
	{
		ReadResponseMsg* m = readResponsePool.Take();
		m->directoryLookup = false;
      m->isIntervention = false;
      //m->isSpeculative = false;
      m->pendingInvalidates = INVALID_PENDING_INVALIDATES;
		m->originalRequestingNode = InvalidNodeID;
		m->SetIDInfo(currentMsgStamp++,devID,generatingPC);
		return m;
	}
	WriteResponseMsg* EventManager::CreateWriteResponseMsg(DeviceID devID, Address generatingPC)
	{
		WriteResponseMsg* m = writeResponsePool.Take();
		m->SetIDInfo(currentMsgStamp++,devID,generatingPC);
		return m;
	}
	InvalidateResponseMsg* EventManager::CreateInvalidateResponseMsg(DeviceID devID, Address generatingPC)
	{
		InvalidateResponseMsg* m = invalidateResponsePool.Take();
		m->SetIDInfo(currentMsgStamp++,devID,generatingPC);
		return m;
	}
	EvictionResponseMsg* EventManager::CreateEvictionResponseMsg(DeviceID devID, Address generatingPC)
	{
		EvictionResponseMsg* m = evictionResponsePool.Take();
		m->SetIDInfo(currentMsgStamp++,devID,generatingPC);
		return m;
	}
	NetworkMsg* EventManager::CreateNetworkMsg(DeviceID devID, Address generatingPC)
	{
		NetworkMsg* m = networkPool.Take();
		m->isOverrideSource = false;
		m->SetIDInfo(currentMsgStamp++,devID,generatingPC);
		return m;
	}
	BaseMsg* EventManager::ReplicateMsg(const BaseMsg* msg)
	{
		DebugAssert(msg);
		BaseMsg* ret = NULL;
		switch(msg->Type())
		{
		case(mt_Read):
			{
				ReadMsg* m = CreateReadMsg(msg->GeneratingDeviceID(),msg->GeneratingPC());
				*m = *((ReadMsg*)msg);
				ret = m;
				break;
			}
		case(mt_Write):
			{
				WriteMsg* m = CreateWriteMsg(msg->GeneratingDeviceID(),msg->GeneratingPC());
				*m = *((WriteMsg*)msg);
				ret = m;
				break;
			}
		case(mt_Invalidate):
			{
				InvalidateMsg* m = CreateInvalidateMsg(msg->GeneratingDeviceID(),msg->GeneratingPC());
				*m = *((InvalidateMsg*)msg);
				ret = m;
				break;
			}
      case(mt_InvalidateSharer):
         {
            InvalidateSharerMsg* m = CreateInvalidateSharerMsg(msg->GeneratingDeviceID(),msg->GeneratingPC());
            *m = *((InvalidateSharerMsg*)msg);
            ret = m;
            break;
         }
		case(mt_Eviction):
			{
				EvictionMsg* m = CreateEvictionMsg(msg->GeneratingDeviceID(),msg->GeneratingPC());
				*m = *((EvictionMsg*)msg);
				ret = m;
				break;
			}
		case(mt_ReadResponse):
			{
				ReadResponseMsg* m = CreateReadResponseMsg(msg->GeneratingDeviceID(),msg->GeneratingPC());
				*m = *((ReadResponseMsg*)msg);
				ret = m;
				break;
			}
		case(mt_WriteResponse):
			{
				WriteResponseMsg* m = CreateWriteResponseMsg(msg->GeneratingDeviceID(),msg->GeneratingPC());
				*m = *((WriteResponseMsg*)msg);
				ret = m;
				break;
			}
		case(mt_InvalidateResponse):
			{
				InvalidateResponseMsg* m = CreateInvalidateResponseMsg(msg->GeneratingDeviceID(),msg->GeneratingPC());
				*m = *((InvalidateResponseMsg*)msg);
				ret = m;
				break;
			}
		case(mt_EvictionResponse):
			{
				EvictionResponseMsg* m = CreateEvictionResponseMsg(msg->GeneratingDeviceID(),msg->GeneratingPC());
				*m = *((EvictionResponseMsg*)msg);
				ret = m;
				break;
			}
		case(mt_Network):
			{
				NetworkMsg* m = CreateNetworkMsg(msg->GeneratingDeviceID(),msg->GeneratingPC());
				*m = *((NetworkMsg*)msg);
				ret = m;
				break;
			}
		default:
			DebugFail("Unknown Msg Type");
		}
		DebugAssert(ret);
		return ret;
	}
	void EventManager::DisposeMsg(const BaseMsg* msg)
	{
		DebugAssert(msg);
		delete new ReadResponseMsg();
		switch(msg->Type())
		{
			case(mt_Read): readPool.Return((ReadMsg*)msg); break;
			case(mt_Write): writePool.Return((WriteMsg*)msg); break;
			case(mt_Invalidate): invalidatePool.Return((InvalidateMsg*)msg); break;
			case(mt_InvalidateSharer): invalidateSharerPool.Return((InvalidateSharerMsg*)msg); break;
			case(mt_Eviction): evictionPool.Return((EvictionMsg*)msg); break;
			case(mt_ReadResponse): readResponsePool.Return((ReadResponseMsg*)msg); break;
			case(mt_WriteResponse): writeResponsePool.Return((WriteResponseMsg*)msg); break;
			case(mt_InvalidateResponse): invalidateResponsePool.Return((InvalidateResponseMsg*)msg); break;
			case(mt_EvictionResponse): evictionResponsePool.Return((EvictionResponseMsg*)msg); break;
			case(mt_Network): networkPool.Return((NetworkMsg*)msg); break;
			default: DebugFail("Unknown Msg Type");
		}
	}
	void EventManager::ScheduleEvent(StoredFunctionBase* f, TimeDelta dT)
	{
		DebugAssert(dT >= 0);
		ScheduleEventAbsolute(f, currentTick + dT);
	}
	void EventManager::ScheduleEventAbsolute(StoredFunctionBase* f, TickTime time)
	{
		DebugAssert(f);
		DebugAssert(time >= currentTick);
		if(scheduleSet.find(time) == scheduleSet.end())
		{
			scheduleSet[time] = containerPool.Take();
		}
		(*(scheduleSet[time])).push_back(f);
	}
	void EventManager::ScheduleMessageTransmit(const BaseMsg* msg, BaseMemDevice* from, BaseMemDevice* to, int toConnectionID, TimeDelta dT)
	{//CHANGE THIS FOR PARALLEL VERSION
		CBDeliverMsg::FunctionType* ev = cbDeliverMsg.Create();
		DebugAssert(ev);
		ev->Initialize(this, to, msg, toConnectionID);
		ScheduleEvent(ev, dT);
	}
	void EventManager::ProcessTick()
	{
		if(scheduleSet.find(currentTick) != scheduleSet.end())
		{
			for(size_t i = 0; i < scheduleSet[currentTick]->size(); i++)
			{
				(*scheduleSet[currentTick])[i]->Call();
			}
			scheduleSet[currentTick]->clear();
			containerPool.Return(scheduleSet[currentTick]);
			scheduleSet.erase(currentTick);
		}
		currentTick++;
	}
	TickTime EventManager::CurrentTick()
	{
		return currentTick;
	}
}
