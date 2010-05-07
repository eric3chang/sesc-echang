#ifndef LOGTM_COLLISION_DETECTION_H
#define LOGTM_COLLISION_DETECTION_H

#include "ITMCollisionDetection.h"
#include "estl.h"
#include "CollisionDetectionFlatten.h"
#include "CollisionDetectionNoBlend.h"

#define LogTMCollisionDetection CollisionDetectionFlatten<CollisionDetectionNoBlend<LogTMCollisionDetectionBase> >

class LogTMCollisionDetectionBase : public ITMCollisionDetection
{
	class TransactionResult
	{
	public:
		int pid;
		uint32_t transID;
		uint32_t overflowed;
		std::vector<uint32_t> readSet;
		std::vector<uint32_t> writeSet;
		TransactionResult()
		{
			pid = -1;
			transID = 0;
			overflowed = 0;
		}
	};
	std::vector<TransactionResult*> transResultBuffer;
	HASH_MAP<uint32_t,TransactionResult*> activeTransResultSet;
	class CollisionDataEntry
	{
	public:
		bool valid;
		uint32_t tag;
		bool readSet;
		bool writeSet;
		CollisionDataEntry()
		{
			valid = false;
		}
	};

	size_t associativity;
	size_t cacheWidth;
	size_t lineSize;

	VAddr lastAddress;
	uint32_t lastTransID;
	uint32_t newTransID;
	uint32_t transOpCode;
	size_t accessSize;
	bool setSpecificOverflow;
	int lastPid;
	bool isRead;
	bool isAccess;
	int collisionIndex;

	std::vector<int> collisionPids;

	std::vector<std::vector<CollisionDataEntry> > collisionVector;
	std::vector<uint32_t> currentTransaction;
	std::vector<std::vector<bool> > overflow;

	bool IsInSet(int pid, VAddr addr, bool read);
	void AddToSet(int pid, VAddr addr, bool read);
	void ClearCollisionData(int pid);
	uint32_t CalcTag(VAddr addr) const
	{
		return ((uint32_t)addr) / lineSize;
	}
	CollisionDataEntry& GetDataEntry(int pid, uint32_t tag, uint32_t iteration)
	{
		I(iteration < associativity);
		return collisionVector[pid][cacheWidth * iteration + (tag % cacheWidth)];
	}
	bool IsOverflown(int pid, VAddr addr) const
	{
		uint32_t tag = CalcTag(addr);
		return overflow[pid][tag % overflow[pid].size()];
	}
	bool ExistsCollision(int pid, VAddr addr, bool isRead)
	{
		return IsOverflown(pid,addr) || IsInSet(pid,addr,false) || ((isRead)?(IsInSet(pid,addr,true)):(false));
	}
public:
	LogTMCollisionDetectionBase(size_t associativity, size_t cacheWidth, size_t lineSize, bool setSpecific = false);
	virtual ~LogTMCollisionDetectionBase();

	virtual void BeginRabbit();
	virtual void EndRabbit();

	virtual void Initialize(TMProcessor** procSet, int procCount, TMMemory* memory);
	virtual void Destroy();

	virtual void ObserveAccess(bool isRead, int pid, uint32_t transactionEntry, VAddr address, size_t size);
	virtual void ObserveSpecialInst(uint32_t opCode, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5);
	virtual void ObserveTransactionInst(int pid, uint32_t oldLevel, uint32_t newLevel, uint32_t opCode, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5);
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