#ifndef TM_PROCESSOR_H
#define TM_PROCESSOR_H

#include "estl.h"
#include "ThreadContext.h"
#include "GProcessor.h"
#include "icode.h"
#include "ThreadContext.h"

#ifdef TRANSACTIONAL_MEMORY
class TMProcessorCheckpoint
{
public:
	ThreadContext tc;
	uint32_t oldTransEntry;
};
class TMProcessor
{
	GProcessor* proc;
	HASH_MAP<uint64_t,ThreadContext> checkpointSet;
	HASH_MAP<uint64_t,uint32_t> transLevelSet;
	uint64_t currentCheckpointReturn;
public:
	TMProcessor(int index);

	int GetPID() const;

	void TakeCheckpoint(TMProcessorCheckpoint& cp);
	void RestoreCheckpoint(const TMProcessorCheckpoint& cp);

	VAddr GetPCAddr() const;
	icode_ptr GetPC() const;
	void SetPCAddr(VAddr pc);
	void SetPC(icode_ptr pc);
	void FlushInstructionWindow();

	uint64_t GetProcTime() const;

	void PrepareCheckpoint(uint64_t instID, icode_ptr target, uint32_t transLevel);
	void PrepareCheckpointReturn(uint64_t instID);
	void ExpireCheckpoint(uint64_t instID);
	void CancelCheckpointReturn();
	void StallProcessor(TimeDelta_t delay);
};
#endif

#endif
