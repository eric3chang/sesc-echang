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
	void EventManager::FillBaseNakMsg(BaseNakMsg* m, DeviceID devID, Address generatingPC)
	{
		m->SetIDInfo(currentMsgStamp++,devID,generatingPC);
      m->addr = 0;
      m->solicitingMsg = 0;
	}
	void EventManager::InitializeBaseNakMsg(BaseNakMsg* bnm, const ReadMsg* rm)
	{
		bnm->addr = rm->addr;
		bnm->solicitingMsg = rm->MsgID();
	}

	void EventManager::FillEvictionMsg(EvictionMsg* m, DeviceID devID, Address generatingPC)
	{
		m->SetIDInfo(currentMsgStamp++,devID,generatingPC);
      m->addr = 0;
      m->blockAttached = false;
      m->size = 0;
	}

	void EventManager::InitializeEvictionMsg(EvictionMsg* copy, const EvictionMsg* original)
	{
		copy->addr = original->addr;
		copy->blockAttached = original->blockAttached;
		copy->size = original->blockAttached;
	}

	void EventManager::InitializeEvictionMsg(EvictionMsg* em, const ReadMsg* rm)
	{
		em->addr = rm->addr;
		em->size = rm->size;
	}

	void EventManager::FillEvictionResponseMsg(EvictionResponseMsg* m, DeviceID devID, Address generatingPC)
	{
		m->SetIDInfo(currentMsgStamp++,devID,generatingPC);
		m->addr = 0;
		m->size = 0;
		m->solicitingMessage = 0;
	}

	void EventManager::InitializeEvictionResponseMsg(EvictionResponseMsg* erm, const EvictionMsg* em)
	{
		erm->addr = em->addr;
		erm->size = em->size;
		erm->solicitingMessage = em->MsgID();
	}

	void EventManager::InitializeEvictionResponseMsg(EvictionResponseMsg* copy, const EvictionResponseMsg* original)
	{
		copy->addr = original->addr;
		copy->size = original->size;
		copy->solicitingMessage = original->solicitingMessage;
	}

	void EventManager::InitializeEvictionResponseMsg(EvictionResponseMsg* evictionResponse, const ReadMsg* read)
	{
		evictionResponse->addr = read->addr;
		evictionResponse->size = read->size;
		evictionResponse->solicitingMessage = read->MsgID();
	}

	void EventManager::FillInvalidateMsg(InvalidateMsg* m, DeviceID devID, Address generatingPC)
	{
		m->SetIDInfo(currentMsgStamp++,devID,generatingPC);
      m->newOwner = InvalidNodeID;
	}

	void EventManager::FillInvalidateResponseMsg(InvalidateResponseMsg* m, DeviceID devID, Address generatingPC)
	{
		m->SetIDInfo(currentMsgStamp++,devID,generatingPC);
	}

	void EventManager::FillReadMsg(ReadMsg* m, DeviceID devID, Address generatingPC)
	{
		m->SetIDInfo(currentMsgStamp++,devID,generatingPC);
		m->directoryLookup = false;
		//m->originalRequestingNode = InvalidNodeID;
	}

	void EventManager::FillReadResponseMsg(ReadResponseMsg* m, DeviceID devID, Address generatingPC)
	{
		m->directoryLookup = false;
     // m->evictionMessage = 0;
      m->exclusiveOwnership = false;
      m->isDirty = false;
		m->SetIDInfo(currentMsgStamp++,devID,generatingPC);
		m->solicitingMessage = 0;
	}

	void EventManager::InitializeInvalidateMsg(InvalidateMsg* invalidate, const ReadMsg* read)
	{
		invalidate->addr = read->addr;
		invalidate->size = read->size;
	}

	void EventManager::InitializeInvalidateResponseMsg(InvalidateResponseMsg* copy, const InvalidateResponseMsg* original)
	{
		copy->addr = original->addr;
		copy->blockAttached = original->blockAttached;
		copy->size = original->size;
		copy->solicitingMessage = original->solicitingMessage;
	}

	void EventManager::InitializeReadMsg(ReadMsg* copy, const ReadMsg* original)
	{
		copy->addr = original->addr;
		copy->alreadyHasBlock = original->alreadyHasBlock;
		copy->directoryLookup = original->directoryLookup;
		copy->onCompletedCallback = original->onCompletedCallback;
		copy->requestingExclusive = original->requestingExclusive;
		copy->size = original->size;
	}

	void EventManager::InitializeReadMsg(ReadMsg* read, const ReadResponseMsg* readResponse)
	{
		read->addr = readResponse->addr;
		read->size = readResponse->size;
	}

	void EventManager::InitializeReadResponseMsg(ReadResponseMsg* readResponse, const EvictionMsg* eviction)
	{
		readResponse->addr = eviction->addr;
		readResponse->blockAttached = eviction->blockAttached;
		//readResponse->evictionMessage = eviction->MsgID();
		readResponse->size = eviction->size;
		readResponse->solicitingMessage = eviction->MsgID();
	}

	void EventManager::InitializeReadResponseMsg(ReadResponseMsg* readResponse, const ReadMsg* read)
	{
		readResponse->addr = read->addr;
		readResponse->size = read->size;
		readResponse->solicitingMessage = read->MsgID();
	}

	void EventManager::InitializeReadResponseMsg(ReadResponseMsg* copy, const ReadResponseMsg* original)
	{
		copy->addr = original->addr;
		copy->blockAttached = original->blockAttached;
		copy->directoryLookup = original->directoryLookup;
		copy->exclusiveOwnership = original->exclusiveOwnership;
		copy->isDirty = original->isDirty;
		copy->satisfied = original->satisfied;
		copy->size = original->size;
		copy->solicitingMessage = original->solicitingMessage;
	}

	void EventManager::InitializeWriteMsg(WriteMsg* write, const EvictionMsg* evic)
	{
		write->addr = evic->addr;
		write->size = evic->size;
	}

	void EventManager::InitializeWriteMsg(WriteMsg* write, const ReadResponseMsg* original)
	{
		write->addr = original->addr;
		write->size = original->size;
	}

	ReadMsg* EventManager::CreateReadMsg(DeviceID devID, Address generatingPC)
	{
		ReadMsg* m = readPool.Take();
		FillReadMsg(m,devID,generatingPC);
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
		FillInvalidateMsg(m,devID,generatingPC);
		return m;
	}
	EvictionMsg* EventManager::CreateEvictionMsg(DeviceID devID, Address generatingPC)
	{
		EvictionMsg* m = evictionPool.Take();
		FillEvictionMsg(m,devID,generatingPC);
		return m;
	}
	ReadResponseMsg* EventManager::CreateReadResponseMsg(DeviceID devID, Address generatingPC)
	{
		ReadResponseMsg* m = readResponsePool.Take();
		FillReadResponseMsg(m,devID,generatingPC);
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
		FillInvalidateResponseMsg(m,devID,generatingPC);
		return m;
	}
	EvictionResponseMsg* EventManager::CreateEvictionResponseMsg(DeviceID devID, Address generatingPC)
	{
		EvictionResponseMsg* m = evictionResponsePool.Take();
		FillEvictionResponseMsg(m,devID,generatingPC);
		return m;
	}
   NetworkMsg* EventManager::CreateNetworkMsg(DeviceID devID, Address generatingPC)
	{
		NetworkMsg* m = networkPool.Take();
		m->isOverrideSource = false;
		m->SetIDInfo(currentMsgStamp++,devID,generatingPC);
		return m;
	}

   CacheNakMsg* EventManager::CreateCacheNakMsg(DeviceID devID, Address generatingPC)
	{
		CacheNakMsg* m = cacheNakPool.Take();
		FillBaseNakMsg(m,devID,generatingPC);
		return m;
	}
   DirectoryNakMsg* EventManager::CreateDirectoryNakMsg(DeviceID devID, Address generatingPC)
	{
		DirectoryNakMsg* m = directoryNakPool.Take();
		FillBaseNakMsg(m,devID,generatingPC);
		return m;
	}
   InterventionMsg* EventManager::CreateInterventionMsg(DeviceID devID, Address generatingPC)
	{
		InterventionMsg* m = interventionPool.Take();
		FillReadMsg(m,devID,generatingPC);
		return m;
	}
   InvalidateAckMsg* EventManager::CreateInvalidateAckMsg(DeviceID devID, Address generatingPC)
	{
		InvalidateAckMsg* m = invalidateAckPool.Take();
		FillInvalidateResponseMsg(m,devID,generatingPC);
		return m;
	}
   ReadReplyMsg* EventManager::CreateReadReplyMsg(DeviceID devID, Address generatingPC)
	{
		ReadReplyMsg* m = readReplyPool.Take();
		FillReadResponseMsg(m,devID,generatingPC);
		m->pendingInvalidates = 0;
		return m;
	}
   SpeculativeReplyMsg* EventManager::CreateSpeculativeReplyMsg(DeviceID devID, Address generatingPC)
	{
		SpeculativeReplyMsg* m = speculativeReplyPool.Take();
		FillReadResponseMsg(m,devID,generatingPC);
		return m;
	}
   TransferMsg* EventManager::CreateTransferMsg(DeviceID devID, Address generatingPC)
	{
		TransferMsg* m = transferPool.Take();
		FillEvictionMsg(m,devID,generatingPC);
      m->isDirty = false;
      m->isShared = false;
		return m;
	}
   WritebackMsg* EventManager::CreateWritebackMsg(DeviceID devID, Address generatingPC)
	{
		WritebackMsg* m = writebackPool.Take();
		FillEvictionMsg(m,devID,generatingPC);
      m->isDirty = false;
      m->isShared = false;
		return m;
	}
   WritebackAckMsg* EventManager::CreateWritebackAckMsg(DeviceID devID, Address generatingPC)
	{
		WritebackAckMsg* m = writebackAckPool.Take();
		FillEvictionResponseMsg(m,devID,generatingPC);
		return m;
	}
   WritebackRequestMsg* EventManager::CreateWritebackRequestMsg(DeviceID devID, Address generatingPC)
	{
		WritebackRequestMsg* m = writebackRequestPool.Take();
		FillEvictionMsg(m,devID,generatingPC);
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

		case(mt_CacheNak):
			{
				CacheNakMsg* m = CreateCacheNakMsg(msg->GeneratingDeviceID(),msg->GeneratingPC());
				*m = *((CacheNakMsg*)msg);
				ret = m;
				break;
			}
		case(mt_DirectoryNak):
			{
				DirectoryNakMsg* m = CreateDirectoryNakMsg(msg->GeneratingDeviceID(),msg->GeneratingPC());
				*m = *((DirectoryNakMsg*)msg);
				ret = m;
				break;
			}
		case(mt_Intervention):
			{
				InterventionMsg* m = CreateInterventionMsg(msg->GeneratingDeviceID(),msg->GeneratingPC());
				*m = *((InterventionMsg*)msg);
				ret = m;
				break;
			}
		case(mt_InvalidateAck):
			{
				InvalidateAckMsg* m = CreateInvalidateAckMsg(msg->GeneratingDeviceID(),msg->GeneratingPC());
				*m = *((InvalidateAckMsg*)msg);
				ret = m;
				break;
			}
		case(mt_ReadReply):
			{
				ReadReplyMsg* m = CreateReadReplyMsg(msg->GeneratingDeviceID(),msg->GeneratingPC());
				*m = *((ReadReplyMsg*)msg);
				ret = m;
				break;
			}
		case(mt_SpeculativeReply):
			{
				SpeculativeReplyMsg* m = CreateSpeculativeReplyMsg(msg->GeneratingDeviceID(),msg->GeneratingPC());
				*m = *((SpeculativeReplyMsg*)msg);
				ret = m;
				break;
			}
		case(mt_Transfer):
			{
				TransferMsg* m = CreateTransferMsg(msg->GeneratingDeviceID(),msg->GeneratingPC());
				*m = *((TransferMsg*)msg);
				ret = m;
				break;
			}
		case(mt_Writeback):
			{
				WritebackMsg* m = CreateWritebackMsg(msg->GeneratingDeviceID(),msg->GeneratingPC());
				*m = *((WritebackMsg*)msg);
				ret = m;
				break;
			}
		case(mt_WritebackAck):
			{
				WritebackAckMsg* m = CreateWritebackAckMsg(msg->GeneratingDeviceID(),msg->GeneratingPC());
				*m = *((WritebackAckMsg*)msg);
				ret = m;
				break;
			}
		case(mt_WritebackRequest):
			{
				WritebackRequestMsg* m = CreateWritebackRequestMsg(msg->GeneratingDeviceID(),msg->GeneratingPC());
				*m = *((WritebackRequestMsg*)msg);
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
		switch(msg->Type())
		{
			case(mt_Read): readPool.Return((ReadMsg*)msg); break;
			case(mt_Write): writePool.Return((WriteMsg*)msg); break;
			case(mt_Invalidate): invalidatePool.Return((InvalidateMsg*)msg); break;
			case(mt_Eviction): evictionPool.Return((EvictionMsg*)msg); break;
			case(mt_ReadResponse): readResponsePool.Return((ReadResponseMsg*)msg); break;
			case(mt_WriteResponse): writeResponsePool.Return((WriteResponseMsg*)msg); break;
			case(mt_InvalidateResponse): invalidateResponsePool.Return((InvalidateResponseMsg*)msg); break;
			case(mt_EvictionResponse): evictionResponsePool.Return((EvictionResponseMsg*)msg); break;
         case(mt_Network): networkPool.Return((NetworkMsg*)msg); break;

         case(mt_CacheNak): cacheNakPool.Return((CacheNakMsg*)msg); break;
         case(mt_DirectoryNak): directoryNakPool.Return((DirectoryNakMsg*)msg); break;
         case(mt_Intervention): interventionPool.Return((InterventionMsg*)msg); break;
         case(mt_InvalidateAck): invalidateAckPool.Return((InvalidateAckMsg*)msg); break;
         case(mt_ReadReply): readReplyPool.Return((ReadReplyMsg*)msg); break;
         case(mt_SpeculativeReply): speculativeReplyPool.Return((SpeculativeReplyMsg*)msg); break;
         case(mt_Transfer): transferPool.Return((TransferMsg*)msg); break;
         case(mt_Writeback): writebackPool.Return((WritebackMsg*)msg); break;
         case(mt_WritebackAck): writebackAckPool.Return((WritebackAckMsg*)msg); break;
         case(mt_WritebackRequest): writebackRequestPool.Return((WritebackRequestMsg*)msg); break;

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
