#ifndef TM_PERFECT_COLLISIONDETECTION_H
#define TM_PERFECT_COLLISIONDETECTION_H

#include "ITMCollisionDetection.h"

class PerfectCollisionDetection : public ITMCollisionDetection
{
	class TransactionResult
	{
	public:
		std::vector<uint32_t> readSet;
		std::vector<uint32_t> writeSet;
		uint32_t transID;
		int pid;
	};
	std::vector<TransactionResult*> transResultBuffer;
	HASH_MAP<uint32_t,TransactionResult*> activeTransResultSet;
	class TransData
	{
	public:
		int pid;
		uint32_t transLevel;
		uint32_t parentLevel;
		HASH_SET<uint32_t> readSet;
		HASH_SET<uint32_t> writeSet;
	};
	class CollisionReport
	{
	public:
		int enemyPid;
		uint32_t enemyLevel;
		bool enemyIsRead;
	};
	uint32_t currentAddr;
	int currentCpu;
	bool currentIsRead;
	bool currentIsAccess;
	size_t currentAccessSize;
	uint32_t opCode;
	uint32_t oldLevel;
	uint32_t newLevel;
	HASH_MAP<uint32_t,TransData> activeTransactions;
	uint32_t lineSize;
	int collisionIndex;
	std::vector<CollisionReport> collisionSet;

	uint32_t CalcLineAddr(uint32_t addr) const 
	{ 
		return addr - (addr % lineSize); 
	}
public:
	PerfectCollisionDetection(uint32_t lineSize = 64)
	{
		this->lineSize = lineSize;
		currentCpu = -1;
	}
	virtual ~PerfectCollisionDetection();

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