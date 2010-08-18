#include "TMProcessor.h"
#include "FetchEngine.h"
#ifdef TRANSACTIONAL_MEMORY

TMProcessor::TMProcessor(int index)
{
	proc = osSim->cpus.getProcessor(index);
	currentCheckpointReturn = 0;
}
int TMProcessor::GetPID() const
{
	return proc->currentFlow()->getPid();
}
void TMProcessor::TakeCheckpoint(TMProcessorCheckpoint& cp)
{
	I(currentCheckpointReturn);
	I(checkpointSet.find(currentCheckpointReturn) != checkpointSet.end());
	cp.tc.copy(&(checkpointSet[currentCheckpointReturn]));
	cp.oldTransEntry = transLevelSet[currentCheckpointReturn];
}
void TMProcessor::RestoreCheckpoint(const TMProcessorCheckpoint& cp)
{
	int initialPid = GetPID();
	proc->switchOut(initialPid);
	ThreadContext::getContext(initialPid)->copy(&cp.tc);
	proc->switchIn(initialPid);
}
VAddr TMProcessor::GetPCAddr() const
{
	return GetPC()->addr;
}
icode_ptr TMProcessor::GetPC() const
{
   std::cout << "proc=" << proc;
   std::cout << " proc->currentFlow()=" << proc->currentFlow();
   std::cout << " proc->currentFlow()->getInstructionPointer()=" << proc->currentFlow()->getInstructionPointer();
   std::cout << std::endl;
	return proc->currentFlow()->getInstructionPointer();
}
void TMProcessor::SetPCAddr(VAddr pc)
{
	SetPC(addr2icode(pc));
}
void TMProcessor::SetPC(icode_ptr pc)
{
	proc->currentFlow()->setInstructionPointer(pc);
}
void TMProcessor::FlushInstructionWindow()
{
//	proc->purgeInstructionWindow();
}
uint64_t TMProcessor::GetProcTime() const
{
	return globalClock;
}
void TMProcessor::PrepareCheckpoint(uint64_t instID, icode_ptr target, uint32_t transLevel)
{
	I(checkpointSet.find(instID) == checkpointSet.end());
	int initialPid = GetPID();
	proc->switchOut(initialPid);
	checkpointSet[instID].copy(ThreadContext::getContext(initialPid));
	checkpointSet[instID].setPCIcode(target);
	transLevelSet[instID] = transLevel;
	proc->switchIn(initialPid);
}
void TMProcessor::PrepareCheckpointReturn(uint64_t instID)
{
	currentCheckpointReturn = instID;
}
void TMProcessor::ExpireCheckpoint(uint64_t instID)
{
	I(checkpointSet.find(instID) != checkpointSet.end());
	currentCheckpointReturn = 0;
	checkpointSet.erase(instID);
	transLevelSet.erase(instID);
}
void TMProcessor::CancelCheckpointReturn()
{
	currentCheckpointReturn = 0;
}
void TMProcessor::StallProcessor(TimeDelta_t delay)
{
	proc->BusyWaitFor(delay);
}

#endif
	

