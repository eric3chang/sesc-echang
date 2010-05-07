#ifndef TM_VERSIONING_H
#define TM_VERSIONING_H

#include "TMMemory.h"
#include "TMProcessor.h"

class ITMVersioning
{
public:
	virtual ~ITMVersioning(){}

	virtual void BeginRabbit() = 0;
	virtual void EndRabbit() = 0;

	virtual void Initialize(TMProcessor** procSet, int procCount, TMMemory* memory) = 0;
	virtual void Destroy() = 0;

	virtual void ObserveAccess(bool isRead, int pid, uint32_t transactionEntry, VAddr address, size_t size) = 0;
	virtual void ObserveSpecialInst(uint32_t opCode,
		uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5) = 0;
	virtual void ObserveTransactionInst(int pid, uint32_t oldLevel, uint32_t newLevel, uint32_t opCode,
		uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5) = 0;
	virtual void ObserveCacheEvict(int pid, VAddr address, int cacheLayer, int newCacheLayer, size_t size) = 0;

	virtual void ClockTick() = 0;
	virtual void FinalizeTick() = 0;

	virtual void FinalizeInstruction() = 0;
	virtual void RollbackInstruction() = 0;

	virtual bool HasTransfers(int cpuID) = 0;
	virtual void GetTransferInfo(bool& isRead, bool& isRequired, VAddr& address, size_t& transferSize) = 0;
	virtual void AcceptTransfer() = 0;
	virtual void DenyTransfer() = 0;
	virtual void ConfirmTransferCompleted(int cpuID, bool isRead, VAddr address) = 0;

	virtual uint32_t GetDelay() = 0;
	virtual void MayNotDelay() = 0;

	virtual void DumpStats(std::ostream& out) = 0;
};

#endif