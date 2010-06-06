#ifndef LOGTMSE_COLLISION_DETECTION_H
#define LOGTMSE_COLLISION_DETECTION_H

#include "ITMCollisionDetection.h"
#include "estl.h"
#include "CollisionDetectionFlatten.h"
#include "CollisionDetectionNoBlend.h"

#define LogTMSECollisionDetection CollisionDetectionFlatten<CollisionDetectionNoBlend<LogTMSECollisionDetectionBase> >

class LogTMSECollisionDetectionBase : public ITMCollisionDetection
{
public:
	class RollingHash
	{
	public:
		virtual uint32_t Hash(uint32_t, int) = 0;
	};
	class BasicHash : public RollingHash
	{
	public:
		virtual uint32_t Hash(uint32_t input, int)
		{
			return input;
		}
	};
	class AddressSliceHash : public RollingHash
	{
		uint32_t sliceWidth;
	public:
		virtual uint32_t Hash(uint32_t input, int iteration)
		{
			while(iteration > 0)
			{
				input = input / sliceWidth;
				iteration--;
			}
			return input;
		}
		AddressSliceHash(uint32_t width)
		{
			sliceWidth = width;
		}
	};
	class H3Hash : public RollingHash
	{
	public:
		static const uint32_t H3[64][16];
		virtual uint32_t Hash(uint32_t input, int iteration)
		{
			uint32_t result = 0;
			for(int i = 0; input > 0; i++)
			{
				if(input & 1)
				{
					result ^= H3[i][iteration];
				}
				input = input / 2;
			}
			return result;
		}
	};
private:
	class TransactionResult
	{
	public:
		int pid;
		uint32_t transID;
		uint32_t readSetPopulation;
		uint32_t writeSetPopulation;
		TransactionResult()
		{
			pid = -1;
			transID = 0;
		}
	};
	std::vector<TransactionResult*> transResultBuffer;
	HASH_MAP<uint32_t,TransactionResult*> activeTransResultSet;

	size_t lineSize;
	int iterationCount;
	size_t readFilterSize;
	size_t writeFilterSize;
	RollingHash* hash;

	VAddr lastAddress;
	uint32_t lastTransID;
	uint32_t newTransID;
	uint32_t transOpCode;
	size_t accessSize;
	int lastPid;
	bool isRead;
	bool isAccess;
	int collisionIndex;

	std::vector<int> collisionPids;

	std::vector<std::vector<bool> > readFilter;
	std::vector<std::vector<bool> > writeFilter;
	std::vector<int> readPopulation;
	std::vector<int> writePopulation;

	std::vector<uint32_t> currentTransaction;

	bool IsInReadSet(int pid, VAddr addr);
	bool IsInWriteSet(int pid, VAddr addr);
	void AddToSet(int pid, VAddr addr, bool read);
	void ClearCollisionData(int pid);
	bool ExistsCollision(int pid, VAddr addr, bool isRead)
	{
		if(IsInWriteSet(pid,addr))
		{
			return true;
		}
		if(!isRead)
		{
			return IsInReadSet(pid,addr);
		}
		return false;
	}
	uint32_t CalcTag(VAddr addr)
	{
		return ((uint32_t)addr) / lineSize;
	}
public:
	LogTMSECollisionDetectionBase(uint32_t filterSize, size_t lineSize, int filterIteration, RollingHash* hash);
	virtual ~LogTMSECollisionDetectionBase();

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
