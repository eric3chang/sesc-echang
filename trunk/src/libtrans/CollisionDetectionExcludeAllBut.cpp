#include "CollisionDetectionExcludeAllBut.h"

CollisionDetectionExcludeAllBut::CollisionDetectionExcludeAllBut(uint32_t permitOpCode, ITMCollisionDetection* host)
{
	this->permitOpCode = permitOpCode;
	this->host = host;
	excluding = true;
	tickProcessing = false;
}
CollisionDetectionExcludeAllBut::~CollisionDetectionExcludeAllBut()
{
	delete host;
}
void CollisionDetectionExcludeAllBut::BeginRabbit()
{
	host->BeginRabbit();
}
void CollisionDetectionExcludeAllBut::EndRabbit()
{
	host->EndRabbit();
}
void CollisionDetectionExcludeAllBut::Initialize(TMProcessor** procSet, int procCount, TMMemory* memory)
{
	host->Initialize(procSet,procCount,memory);
}
void CollisionDetectionExcludeAllBut::Destroy()
{
	host->Destroy();
}
void CollisionDetectionExcludeAllBut::ObserveAccess(bool isRead, int pid, uint32_t transactionEntry, VAddr address, size_t size)
{
	this->isRead = isRead;
	this->pid = pid;
	this->transactionEntry = transactionEntry;
	this->addr = address;
	this->size = size;
	excluding = true;
}
void CollisionDetectionExcludeAllBut::ObserveSpecialInst(uint32_t opCode,
	uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5)
{
	if(excluding && opCode == permitOpCode)
	{
		excluding = false;
		host->ObserveAccess(isRead,pid,transactionEntry,addr,size);
	}
	else if(!excluding && opCode != permitOpCode)
	{
		host->ObserveSpecialInst(opCode,arg1,arg2,arg3,arg4,arg5);
	}
}
void CollisionDetectionExcludeAllBut::ObserveTransactionInst(int pid, uint32_t oldLevel, uint32_t newLevel, uint32_t opCode,
	uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5)
{
	host->ObserveTransactionInst(pid,oldLevel,newLevel,opCode,arg1,arg2,arg3,arg4,arg5);
	excluding = false;
}
void CollisionDetectionExcludeAllBut::ObserveCacheEvict(int pid, VAddr address, int cacheLayer, int newCacheLayer, size_t size)
{
	host->ObserveCacheEvict(pid,address,cacheLayer,newCacheLayer,size);
}
void CollisionDetectionExcludeAllBut::ClockTick()
{
	tickProcessing = true;
	host->ClockTick();
}
void CollisionDetectionExcludeAllBut::FinalizeTick()
{
	host->FinalizeTick();
	tickProcessing = false;
}
void CollisionDetectionExcludeAllBut::FinalizeInstruction()
{
	if(!excluding)
	{
		host->FinalizeInstruction();
	}
	excluding = true;
}
void CollisionDetectionExcludeAllBut::RollbackInstruction()
{
	if(!excluding)
	{
		host->RollbackInstruction();
	}
	excluding = true;
}
bool CollisionDetectionExcludeAllBut::HasTransfers(int cpuID)
{
	return host->HasTransfers(cpuID);
}
void CollisionDetectionExcludeAllBut::GetTransferInfo(bool& isRead, bool& isRequired, VAddr& address, size_t& transferSize)
{
	host->GetTransferInfo(isRead,isRequired,address,size);
}
void CollisionDetectionExcludeAllBut::AcceptTransfer()
{
	host->AcceptTransfer();
}
void CollisionDetectionExcludeAllBut::DenyTransfer()
{
	host->DenyTransfer();
}
void CollisionDetectionExcludeAllBut::ConfirmTransferCompleted(int cpuID, bool isRead, VAddr address)
{
	host->ConfirmTransferCompleted(cpuID,isRead,address);
}
uint32_t CollisionDetectionExcludeAllBut::GetDelay()
{
	if(!excluding)
	{
		return host->GetDelay();
	}
	else
	{
		return 0;
	}
}
void CollisionDetectionExcludeAllBut::MayNotDelay()
{
	if(!excluding)
	{
		host->MayNotDelay();
	}
}
int CollisionDetectionExcludeAllBut::CollisionCount()
{
	if(!excluding || tickProcessing)
	{
		return host->CollisionCount();
	}
	else
	{
		return 0;
	}
}
void CollisionDetectionExcludeAllBut::FetchCollision(
	int& local, uint32_t& localLevel, bool& localIsDetermined, bool& localIsRead,
	int& enemy, uint32_t& enemyLevel, bool& enemyIsDetermined, bool& enemyIsRead)
{
	if(!excluding || tickProcessing)
	{
		host->FetchCollision(local,localLevel,localIsDetermined,localIsRead,enemy,enemyLevel,enemyIsDetermined,enemyIsRead);
	}
}
void CollisionDetectionExcludeAllBut::DumpStats(std::ostream& out)
{
	host->DumpStats(out);
}