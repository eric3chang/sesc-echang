#ifndef TM_PERFECT_VERSIONING_H
#define TM_PERFECT_VERSIONING_H

#include "ITMVersioning.h"
#include "estl.h"

class PerfectVersioning : public ITMVersioning
{
	class TransactionResult
	{
	public:
		int pid;
		uint32_t transID;
		std::vector<VAddr> versionSet;
	};
	std::vector<TransactionResult*> transResultBuffer;
	HASH_MAP<uint32_t,TransactionResult*> activeTransResultSet;
	class TransferEntry
	{
	public:
		bool isRead;
		VAddr address;
	};
	class VersionEntry
	{
	public:
		uint64_t versionTag;
		uint8_t data;
		VersionEntry(){}
		VersionEntry(uint64_t tag, uint8_t data)
		{
			versionTag = tag;
			this->data = data;
		}
	};
	TMMemory* mem;
	VAddr activeAddress;
	bool lastInstrWasAccess;
	uint32_t transOpCode;
	uint32_t oldTransLevel;
	uint32_t newTransLevel;
	bool accessIsRead;
	size_t accessSize;
	HASH_MAP<uint32_t,HASH_SET<VAddr> > lineSet;
	bool transferEnabled;
	size_t lineSize;
	VAddr CalcLineAddr(VAddr addr) const { return addr - (addr % lineSize); }
	std::vector<std::queue<TransferEntry> > necessaryTransfers;
	int currentCpu;
	size_t initialVersioningSize;
	std::vector<uint32_t> versioningAddr;
	std::vector<size_t> versioningSize;
	std::vector<size_t> currentBufferIndex;
	uint64_t currentStamp;
public:
	PerfectVersioning(size_t lineSize = 64, bool transferBlocks = false, size_t bufferSize = 0);
	virtual ~PerfectVersioning();

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

	virtual void DumpStats(std::ostream& out);
};

#endif
