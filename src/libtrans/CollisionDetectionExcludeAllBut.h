#ifndef TM_COLLISIONDETECTIONEXCLUDEALLBUT_H
#define TM_COLLISIONDETECTIONEXCLUDEALLBUT_H

#include "ITMCollisionDetection.h"

class CollisionDetectionExcludeAllBut : public ITMCollisionDetection
{
	uint32_t permitOpCode;
	ITMCollisionDetection* host;
	bool excluding;
	bool tickProcessing;
	bool isRead;
	int pid;
	uint32_t transactionEntry;
	VAddr addr;
	size_t size;
public:
	CollisionDetectionExcludeAllBut(uint32_t permitOpCode, ITMCollisionDetection* host);
	virtual ~CollisionDetectionExcludeAllBut();

	virtual void BeginRabbit();
	virtual void EndRabbit();

	virtual void Initialize(TMProcessor** procSet, int procCount, TMMemory* memory);
	virtual void Destroy();

	virtual void ObserveAccess(bool isRead, int pid, uint32_t transactionEntry, VAddr address, size_t size);
	virtual void ObserveSpecialInst(uint32_t opCode,
		uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5);
	virtual void ObserveTransactionInst(int pid, uint32_t oldLevel, uint32_t newLevel, uint32_t opCode,
		uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5);
	virtual void ObserveCacheEvict(int pid, VAddr address, int cacheLayer, int newCacheLayer, size_t size);

	virtual void ClockTick();
	virtual void FinalizeTick();

	virtual void FinalizeInstruction();
	virtual void RollbackInstruction();

	virtual bool HasTransfers(int cpuID);
	virtual void GetTransferInfo(bool& isRead, bool& isRequired, VAddr& address, size_t& transferSize);
	virtual void AcceptTransfer();
	virtual void DenyTransfer();
	virtual void ConfirmTransferCompleted(int cpuID, bool isRead, VAddr address);

	virtual uint32_t GetDelay();
	virtual void MayNotDelay();

	virtual int CollisionCount();
	virtual void FetchCollision(
		int& local, uint32_t& localLevel, bool& localIsDetermined, bool& localIsRead,
		int& enemy, uint32_t& enemyLevel, bool& enemyIsDetermined, bool& enemyIsRead);

	virtual void DumpStats(std::ostream& out);
};

#endif
