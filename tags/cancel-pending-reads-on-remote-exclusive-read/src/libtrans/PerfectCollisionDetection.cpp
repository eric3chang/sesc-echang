#include "PerfectCollisionDetection.h"
#include "TMOpCodes.h"
#include <iostream>
#ifdef TRANSACTIONAL_MEMORY

PerfectCollisionDetection::~PerfectCollisionDetection(){}
void PerfectCollisionDetection::BeginRabbit(){}
void PerfectCollisionDetection::EndRabbit(){}
void PerfectCollisionDetection::Initialize(TMProcessor** procSet, int procCount, TMMemory* memory){}
void PerfectCollisionDetection::Destroy(){}
void PerfectCollisionDetection::ObserveAccess(bool isRead, int pid, uint32_t transactionEntry, VAddr address, size_t size)
{
	currentIsAccess = true;
	currentIsRead = isRead;
	currentCpu = pid;
	currentAccessSize = size;
	currentAddr = address;
	oldLevel = transactionEntry;
}
void PerfectCollisionDetection::ObserveSpecialInst(uint32_t opCode,
	uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5)
{}
void PerfectCollisionDetection::ObserveTransactionInst(int pid, uint32_t oldLevel, uint32_t newLevel, uint32_t opCode,
	uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5)
{
	currentIsAccess = false;
	currentCpu = pid;
	this->oldLevel = oldLevel;
	this->newLevel = newLevel;
	this->opCode = opCode;
}
void PerfectCollisionDetection::ObserveCacheEvict(int pid, VAddr address, int cacheLayer, int newCacheLayer, size_t size){}
void PerfectCollisionDetection::ClockTick(){}
void PerfectCollisionDetection::FinalizeTick(){}
void PerfectCollisionDetection::FinalizeInstruction()
{
	if(currentCpu != -1)
	{
		if(currentIsAccess)
		{
			if(!oldLevel)
			{
				currentCpu = -1;
				return;
			}
			I(activeTransactions.find(oldLevel) != activeTransactions.end());
			uint32_t lastAddr = 0;
			uint32_t lineAddr;
			HASH_SET<uint32_t>& target = currentIsRead?(activeTransactions[oldLevel].readSet):(activeTransactions[oldLevel].writeSet);
			for(uint32_t inputAddr = currentAddr; inputAddr < currentAddr + currentAccessSize; inputAddr++)
			{
				lineAddr = CalcLineAddr(inputAddr);
				if(lastAddr == lineAddr)
				{
					continue;
				}
				if(target.find(lineAddr) == target.end())
				{
					target.insert(lineAddr);
				}
				lastAddr = lineAddr;
			}
		}
		else
		{
			TransactionResult* t;
			switch(opCode)
			{
			case(TMOpCodes::TransBegin):
				I(activeTransactions.find(newLevel) == activeTransactions.end());
				activeTransactions[newLevel].pid = currentCpu;
				activeTransactions[newLevel].transLevel = newLevel;
				activeTransactions[newLevel].parentLevel = oldLevel;
				t = new TransactionResult;
				t->pid = currentCpu;
				t->transID = newLevel;
				transResultBuffer.push_back(t);
				activeTransResultSet[newLevel] = transResultBuffer.back();
				break;
			case(TMOpCodes::TransEnd):
				if(activeTransactions[oldLevel].parentLevel != 0)
				{//promote read and write sets to parent level
					for(HASH_SET<uint32_t>::iterator it = activeTransactions[oldLevel].readSet.begin(); it != activeTransactions[oldLevel].readSet.end(); it++)
					{
						if(activeTransactions[activeTransactions[oldLevel].parentLevel].readSet.find(*it) == activeTransactions[activeTransactions[oldLevel].parentLevel].readSet.end())
						{
							activeTransactions[activeTransactions[oldLevel].parentLevel].readSet.insert(*it);
						}
					}
					for(HASH_SET<uint32_t>::iterator it = activeTransactions[oldLevel].writeSet.begin(); it != activeTransactions[oldLevel].writeSet.end(); it++)
					{
						if(activeTransactions[activeTransactions[oldLevel].parentLevel].writeSet.find(*it) == activeTransactions[activeTransactions[oldLevel].parentLevel].writeSet.end())
						{
							activeTransactions[activeTransactions[oldLevel].parentLevel].writeSet.insert(*it);
						}
					}
				}
			case(TMOpCodes::TransAbort):
				I(activeTransactions.find(oldLevel) != activeTransactions.end());
				I(activeTransactions[oldLevel].pid == currentCpu);
				for(HASH_SET<uint32_t>::iterator it = activeTransactions[oldLevel].readSet.begin(); it != activeTransactions[oldLevel].readSet.end(); it++)
				{
					activeTransResultSet[oldLevel]->readSet.push_back(*it);
				}
				for(HASH_SET<uint32_t>::iterator it = activeTransactions[oldLevel].writeSet.begin(); it != activeTransactions[oldLevel].writeSet.end(); it++)
				{
					activeTransResultSet[oldLevel]->writeSet.push_back(*it);
				}
				activeTransResultSet.erase(oldLevel);
				activeTransactions.erase(oldLevel);
				break;
			default:
				I(0);
			}
		}
		currentCpu = -1;
	}
}
void PerfectCollisionDetection::RollbackInstruction()
{
	currentCpu = -1;
}
bool PerfectCollisionDetection::HasTransfers(int cpuID){ return false; }
void PerfectCollisionDetection::GetTransferInfo(bool& isRead, bool& isRequired, VAddr& address, size_t& transferSize){ I(0); }
void PerfectCollisionDetection::AcceptTransfer(){ I(0); }
void PerfectCollisionDetection::DenyTransfer(){ I(0); }
void PerfectCollisionDetection::ConfirmTransferCompleted(int cpuID, bool isRead, VAddr address){ I(0); }
uint32_t PerfectCollisionDetection::GetDelay(){ return 0; }
void PerfectCollisionDetection::MayNotDelay(){}
int PerfectCollisionDetection::CollisionCount()
{
	//bool detectBogusCollision = false;	//unused variable
	collisionSet.clear();
	if(currentCpu != -1 && currentIsAccess && oldLevel != 0)
	{
		CollisionReport c;
		for(HASH_MAP<uint32_t,TransData>::iterator it = activeTransactions.begin(); it != activeTransactions.end(); it++)
		{
			if(it->second.pid == currentCpu)
			{
				continue;
			}
			c.enemyPid = it->second.pid;
			c.enemyLevel = it->second.transLevel;
			I(c.enemyLevel != 0);
			I(oldLevel != 0);
			uint32_t lastLine = 0;
			uint32_t lineAddr;
			for(uint32_t inputAddr = currentAddr; inputAddr < currentAddr + currentAccessSize; inputAddr++)
			{
				lineAddr = CalcLineAddr(inputAddr);
				if(lineAddr == lastLine)
				{
					continue;
				}
				if(it->second.writeSet.find(lineAddr) != it->second.writeSet.end())
				{
					c.enemyIsRead = false;
					collisionSet.push_back(c);
				}
				else if(!currentIsRead && it->second.readSet.find(lineAddr) != it->second.readSet.end())
				{
					c.enemyIsRead = true;
					collisionSet.push_back(c);
				}
				lastLine = lineAddr;
			}
		}
	}
	collisionIndex = 0;
	return collisionSet.size();
}
void PerfectCollisionDetection::FetchCollision(
	int& local, uint32_t& localLevel, bool& localIsDetermined, bool& localIsRead,
	int& enemy, uint32_t& enemyLevel, bool& enemyIsDetermined, bool& enemyIsRead)
{
	localIsDetermined = enemyIsDetermined = true;
	local = currentCpu;
	localLevel = oldLevel;
	localIsRead = currentIsRead;
	enemy = collisionSet[collisionIndex].enemyPid;
	enemyLevel = collisionSet[collisionIndex].enemyLevel;
	enemyIsRead = collisionSet[collisionIndex].enemyIsRead;
	collisionIndex++;
}
void PerfectCollisionDetection::DumpStats(std::ostream& out)
{
	std::cout << "beginning collision detection dump" << std::endl;
	out << "string CollisionDetection Perfect int LineSize " << lineSize << " #" << std::endl;
	std::cout << "dumping result set" << std::endl;
	for(size_t i = 0; i < transResultBuffer.size(); i++)
	{
		out << "Group int Pid " << transResultBuffer[i]->pid << " int TransID " << transResultBuffer[i]->transID << " #" << std::endl;
		for(size_t j = 0; j < transResultBuffer[i]->readSet.size(); j++)
		{
			out << "int ReadSet " << transResultBuffer[i]->readSet[j] << " #" << std::endl;
		}
		for(size_t j = 0; j < transResultBuffer[i]->writeSet.size(); j++)
		{
			out << "int WriteSet " << transResultBuffer[i]->writeSet[j] << " #" << std::endl;
		}
		out << "EndGroup #" << std::endl;
	}
	std::cout << "ending collision detection dump" << std::endl;
}
#endif
