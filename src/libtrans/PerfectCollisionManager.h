#ifndef TM_PERFECT_COLLISIONMANAGER_H
#define TM_PERFECT_COLLISIONMANAGER_H

#include "ITMCollisionManager.h"

class PerfectCollisionManager : public ITMCollisionManager
{
	class TransactionResult
	{
	public:
		int pid;
		uint32_t transID;
		uint32_t parentID;
		bool committed;
		uint64_t beginStamp;
		uint64_t endStamp;
		uint64_t stallCycles;
		uint64_t stallBeforeStart;
		uint32_t readCount;
		uint32_t writeCount;
	};
	std::vector<TransactionResult*> transResultBuffer;
	HASH_MAP<uint32_t,TransactionResult*> activeTransResultSet;
	class CollisionInfo
	{
	public:
		int localPid;
		int enemyPid;
		uint32_t localLevel;
		uint32_t enemyLevel;
		bool localIsDetermined;
		bool localIsRead;
		bool enemyIsDetermined;
		bool enemyIsRead;
	};

	std::vector<TMProcessor*> procs;
	bool transInstr;
	uint32_t oldLevel;
	uint32_t newLevel;
	int pid;
	uint32_t opCode;
	uint32_t delay;
	int minBackoffTime;
	int maxBackoffTime;
	std::vector<HASH_SET<int> > pidDominance;
	std::vector<std::queue<uint32_t> > outerTransactions;
	HASH_MAP<uint32_t, uint32_t> parentSet;
	HASH_MAP<uint32_t, uint32_t> childSet;
	HASH_MAP<uint32_t, uint64_t> timeStamp;
//	HASH_SET<uint32_t> activeTransactionSet;
//	std::vector<std::queue<uint32_t> > pendingTransactions;
	std::vector<CollisionInfo> collisionSet;
	std::vector<uint32_t> abortSet;
	std::vector<int> pidDependancy;
	std::vector<HASH_SET<uint32_t> > levelDependancy;
	std::vector<int> backoff;
	int abortIndex;
	bool isAccess;
	bool isRead;
	TimeDelta_t onAbortStall;

public:
	virtual ~PerfectCollisionManager();

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

	virtual void ReportCollisionCount(int count);
	virtual void ReportCollision(
		int local, uint32_t localLevel, bool localIsDetermined, bool localIsRead,
		int enemy, uint32_t enemyLevel, bool enemyIsDetermined, bool enemyIsRead);

	virtual int AbortCount();
	virtual uint32_t AbortID();

	virtual void DumpStats(std::ostream& out);
};

#endif
