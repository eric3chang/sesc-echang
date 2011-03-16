#include "PerfectVersioning.h"
#include "TMOpCodes.h"
#ifdef TRANSACTIONAL_MEMORY

PerfectVersioning::PerfectVersioning(size_t lineSize, bool transferBlocks, size_t bufferSize)
{
	this->lineSize = lineSize;
	transferEnabled = transferBlocks;
	initialVersioningSize = bufferSize;
	currentCpu = -1;
}
PerfectVersioning::~PerfectVersioning(){}
void PerfectVersioning::BeginRabbit(){}
void PerfectVersioning::EndRabbit(){}
void PerfectVersioning::Initialize(TMProcessor** procSet, int procCount, TMMemory* memory)
{
	mem = memory;
	necessaryTransfers.resize(procCount);
}
void PerfectVersioning::Destroy(){}
void PerfectVersioning::ObserveAccess(bool isRead, int pid, uint32_t transactionEntry, VAddr address, size_t size)
{
	activeAddress = address;
	accessIsRead = isRead;
	currentCpu = pid;
	accessSize = size;
	lastInstrWasAccess = true;
	oldTransLevel = transactionEntry;
}
void PerfectVersioning::ObserveSpecialInst(uint32_t opCode,
										   uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5){}
void PerfectVersioning::ObserveTransactionInst(int pid, uint32_t oldLevel, uint32_t newLevel, uint32_t opCode,
	uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5)
{
	currentCpu = pid;
	lastInstrWasAccess = false;
	transOpCode = opCode;
	oldTransLevel = oldLevel;
	newTransLevel = newLevel;
}
void PerfectVersioning::ObserveCacheEvict(int pid, VAddr address, int cacheLayer, int newCacheLayer, size_t size){}
void PerfectVersioning::ClockTick(){}
void PerfectVersioning::FinalizeTick(){}
void PerfectVersioning::FinalizeInstruction()
{
	if(lastInstrWasAccess)
	{
		if(!accessIsRead && oldTransLevel != 0)
		{
			VAddr lastAddr = 0;
			I(lineSet.find(oldTransLevel) != lineSet.end());
			for(size_t i = 0; i < accessSize; i++)
			{
				VAddr curAddr = CalcLineAddr(activeAddress + i);
				if(curAddr != lastAddr)
				{
					lastAddr = curAddr;
					if(lineSet[oldTransLevel].find(curAddr) == lineSet[oldTransLevel].end())
					{
						lineSet[oldTransLevel].insert(curAddr);
						if(transferEnabled)
						{
							TransferEntry te;
							te.isRead = false;
							te.address = versioningAddr[currentCpu] + currentBufferIndex[currentCpu];
							currentBufferIndex[currentCpu] += lineSize;
							necessaryTransfers[currentCpu].push(te);
						}						
					}
				}
			}
		}
	}
	else
	{
		TransactionResult* t;
		switch(transOpCode)
		{
		case(TMOpCodes::TransBegin):
			if(oldTransLevel)
			{
				I(lineSet.find(oldTransLevel) != lineSet.end());
			}
			I(newTransLevel);
			I(lineSet.find(newTransLevel) == lineSet.end());
			lineSet[newTransLevel];
			t = new TransactionResult;
			t->pid = currentCpu;
			t->transID = newTransLevel;
			transResultBuffer.push_back(t);
			activeTransResultSet[newTransLevel] = transResultBuffer.back();
			break;
		case(TMOpCodes::TransEnd):
			I(oldTransLevel);
			I(lineSet.find(oldTransLevel) != lineSet.end());
			if(newTransLevel)
			{
				I(lineSet.find(newTransLevel) != lineSet.end());
				for(HASH_SET<VAddr>::iterator it = lineSet[oldTransLevel].begin(); it != lineSet[oldTransLevel].end(); it++)
				{
					if(lineSet[newTransLevel].find(*it) == lineSet[newTransLevel].end())
					{
						lineSet[newTransLevel].insert(*it);
					}
				}
			}
			for(HASH_SET<VAddr>::iterator it = lineSet[oldTransLevel].begin(); it != lineSet[oldTransLevel].end(); it++)
			{
				activeTransResultSet[oldTransLevel]->versionSet.push_back(*it);
			}
			activeTransResultSet.erase(oldTransLevel);
			lineSet.erase(oldTransLevel);
			break;
		case(TMOpCodes::TransAbort):
			I(oldTransLevel);
			I(lineSet.find(oldTransLevel) != lineSet.end());
			if(newTransLevel)
			{
				I(lineSet.find(newTransLevel) != lineSet.end());
			}
			if(transferEnabled)
			{
				while(!necessaryTransfers[currentCpu].empty())
				{
					necessaryTransfers[currentCpu].pop();
				}
			}
			for(HASH_SET<VAddr>::iterator it = lineSet[oldTransLevel].begin(); it != lineSet[oldTransLevel].end(); it++)
			{
				activeTransResultSet[oldTransLevel]->versionSet.push_back(*it);
			}
			activeTransResultSet.erase(oldTransLevel);
			lineSet.erase(oldTransLevel);
			break;
		default:
			I(0);
		}
	}
	currentCpu = -1;
}
void PerfectVersioning::RollbackInstruction()
{
	currentCpu = -1;
}
bool PerfectVersioning::HasTransfers(int cpuID)
{
	I(currentCpu == -1);
	if(necessaryTransfers[cpuID].empty())
	{
		return false;
	}
	currentCpu = cpuID;
	return true;
}
void PerfectVersioning::GetTransferInfo(bool& isRead, bool& isRequired, VAddr& address, size_t& transferSize)
{
	I(transferEnabled);
	I(currentCpu >= 0 && currentCpu < (int)necessaryTransfers.size());
	I(!necessaryTransfers[currentCpu].empty());
	isRead = necessaryTransfers[currentCpu].front().isRead;
	isRequired = true;
	address = necessaryTransfers[currentCpu].front().address;
	transferSize = lineSize;
}
void PerfectVersioning::AcceptTransfer()
{
	I(transferEnabled);
	I(currentCpu >= 0 && currentCpu< (int)necessaryTransfers.size());
	I(!necessaryTransfers[currentCpu].empty());
	necessaryTransfers[currentCpu].pop();
	currentCpu = -1;
}
void PerfectVersioning::DenyTransfer()
{
	I(transferEnabled);
	I(currentCpu >= 0 && currentCpu < (int)necessaryTransfers.size());
	I(!necessaryTransfers[currentCpu].empty());
	currentCpu = -1;
}
void PerfectVersioning::ConfirmTransferCompleted(int cpuID, bool isRead, VAddr address){}
uint32_t PerfectVersioning::GetDelay(){ return 0; }
void PerfectVersioning::MayNotDelay(){}
void PerfectVersioning::DumpStats(std::ostream& out)
{
	out << "string Versioning Perfect int LineSize " << lineSize << " bool TransfersEnabled ";
	if(transferEnabled)
	{
		out << "true";
	}
	else
	{
		out << "false";
	}
	out << " #" << std::endl;
	for(size_t i = 0; i < transResultBuffer.size(); i++)
	{
		out << "Group int Pid " << transResultBuffer[i]->pid << " int TransID " << transResultBuffer[i]->transID << " #" << std::endl;
		for(size_t j = 0; j < transResultBuffer[i]->versionSet.size(); j++)
		{
			out << "int LoggedAddress " << transResultBuffer[i]->versionSet[j] << " #" << std::endl;
		}
		out << "EndGroup #" << std::endl;
	}
}
#endif
