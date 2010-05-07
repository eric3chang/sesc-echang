#include "PerfectCollisionManager.h"
#include "TMOpCodes.h"
#include <iostream>
#ifdef TRANSACTIONAL_MEMORY

PerfectCollisionManager::~PerfectCollisionManager(){}
void PerfectCollisionManager::BeginRabbit(){}
void PerfectCollisionManager::EndRabbit(){}
void PerfectCollisionManager::Initialize(TMProcessor** procSet, int procCount, TMMemory* memory)
{
	backoff.resize(procCount);
	for(int i = 0; i < procCount; i++)
	{
		backoff[i] = 0;
		procs.push_back(procSet[i]);
		pidDependancy.push_back(-1);
	}
	levelDependancy.resize(procCount);
	outerTransactions.resize(procCount);
	minBackoffTime = 1;
	maxBackoffTime = 4;
	I(maxBackoffTime < 32000);
	isAccess = false;
	isRead = false;
	delay = 0;
	onAbortStall = 100;
	pidDominance.resize(procCount);
}
void PerfectCollisionManager::Destroy(){}
void PerfectCollisionManager::ObserveAccess(bool isRead, int pid, uint32_t transactionEntry, VAddr address, size_t size)
{
	delay = 0;
	transInstr = false;
	isAccess = true;
	this->oldLevel = transactionEntry;
	this->isRead = isRead;
	if(pidDependancy[pid] != -1)
	{
		delay = backoff[pid];
		backoff[pid] = std::min(maxBackoffTime,backoff[pid] * 2);
		if(transactionEntry)
		{
			I(activeTransResultSet.find(transactionEntry) != activeTransResultSet.end());
			activeTransResultSet[transactionEntry]->stallCycles += delay;
		}
	}
}
void PerfectCollisionManager::ObserveSpecialInst(uint32_t opCode,
	uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5)
{}
void PerfectCollisionManager::ObserveTransactionInst(int pid, uint32_t oldLevel, uint32_t newLevel, uint32_t opCode,
	uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5)
{
	delay = 0;
	transInstr = true;
	this->opCode = opCode;
	this->pid = pid;
	this->newLevel = newLevel;
	this->oldLevel = oldLevel;
	if(pidDependancy[pid] != -1)
	{
		delay = backoff[pid];
		backoff[pid] = std::min(maxBackoffTime,backoff[pid] * 2);
	}
}
void PerfectCollisionManager::ObserveCacheEvict(int pid, VAddr address, int cacheLayer, int newCacheLayer, size_t size){}
void PerfectCollisionManager::ClockTick(){}
void PerfectCollisionManager::FinalizeTick(){}
void PerfectCollisionManager::FinalizeInstruction()
{
	if(transInstr)
	{
		TransactionResult* t;
		switch(opCode)
		{
		case(TMOpCodes::TransBegin):
			if(oldLevel == 0)
			{
				outerTransactions[pid].push(newLevel);
			}
			parentSet[newLevel] = oldLevel;
			childSet[oldLevel] = newLevel;
			timeStamp[newLevel] = (timeStamp.find(oldLevel) == timeStamp.end()) ? procs[pid]->GetProcTime() : timeStamp[oldLevel];
			std::cout << "[" << pid << "] Transaction " << newLevel << " started at " << procs[pid]->GetProcTime() << std::endl;
			t = new TransactionResult;			
			t->pid = pid;
			t->parentID = oldLevel;
			t->transID = newLevel;
			t->stallBeforeStart = t->stallCycles = t->readCount = t->writeCount = 0;
			t->beginStamp = procs[pid]->GetProcTime();
			transResultBuffer.push_back(t);
			activeTransResultSet[newLevel] = transResultBuffer.back();
			break;
		case(TMOpCodes::TransAbort):
		case(TMOpCodes::TransEnd):
			std::cout << "[" << pid << "] Transaction " << oldLevel << " ended at " << procs[pid]->GetProcTime();
			if(TMOpCodes::TransAbort == opCode)
			{
				std::cout << " [aborted]" << std::endl;
			}
			else
			{
				std::cout << " [committed]" << std::endl;
			}
			parentSet.erase(oldLevel);
			childSet.erase(newLevel);
			timeStamp.erase(oldLevel);
			if(newLevel == 0)
			{
				while(!outerTransactions[pid].empty() && parentSet.find(outerTransactions[pid].front()) == parentSet.end())
				{
					outerTransactions[pid].pop();
				}
			}
			for(size_t i = 0; i < pidDependancy.size(); i++)
			{
				if(pidDependancy[i] == pid)
				{
					if(levelDependancy[i].find(oldLevel) != levelDependancy[i].end())
					{
						levelDependancy[i].erase(oldLevel);
						if(levelDependancy[i].empty())
						{
							backoff[i] = 0;
							pidDependancy[i] = -1;
						}
					}
				}
			}
			pidDominance[pid].clear();
			backoff[pid] = 0;
			pidDependancy[pid] = -1;
			levelDependancy[pid].clear();
			activeTransResultSet[oldLevel]->endStamp = procs[pid]->GetProcTime();
			activeTransResultSet[oldLevel]->committed = (opCode == TMOpCodes::TransEnd);
			activeTransResultSet.erase(oldLevel);
			break;
		default:
			I(0);
		}
		transInstr = false;
	}
	else if (isAccess)
	{
		if(oldLevel)
		{
			I(activeTransResultSet.find(oldLevel) != activeTransResultSet.end());
			if(isRead)
			{
				activeTransResultSet[oldLevel]->readCount++;
			}
			else
			{
				activeTransResultSet[oldLevel]->writeCount++;
			}
		}
	}
}
void PerfectCollisionManager::RollbackInstruction()
{
	transInstr = false;
}
bool PerfectCollisionManager::HasTransfers(int cpuID){ return false; }
void PerfectCollisionManager::GetTransferInfo(bool& isRead, bool& isRequired, VAddr& address, size_t& transferSize){ I(0); }
void PerfectCollisionManager::AcceptTransfer(){ I(0); }
void PerfectCollisionManager::DenyTransfer(){ I(0); }
void PerfectCollisionManager::ConfirmTransferCompleted(int cpuID, bool isRead, VAddr address){}
uint32_t PerfectCollisionManager::GetDelay()
{
	uint32_t tempDelay = delay;
	delay = 0;
	return tempDelay;
}
void PerfectCollisionManager::MayNotDelay(){}
void PerfectCollisionManager::ReportCollisionCount(int count)
{
	collisionSet.clear();
}
void PerfectCollisionManager::ReportCollision(
	int local, uint32_t localLevel, bool localIsDetermined, bool localIsRead,
	int enemy, uint32_t enemyLevel, bool enemyIsDetermined, bool enemyIsRead)
{
	CollisionInfo c;
	c.localPid = local;
	c.enemyPid = enemy;
	c.localLevel = localLevel;
	c.enemyLevel = enemyLevel;
	c.localIsRead = localIsRead;
	c.enemyIsRead = enemyIsRead;
	c.localIsDetermined = localIsDetermined;
	c.enemyIsDetermined = enemyIsDetermined;
	collisionSet.push_back(c);
}
int PerfectCollisionManager::AbortCount()
{
	abortSet.clear();
	pid = collisionSet[0].localPid;
	HASH_SET<uint32_t> abortMask;
	uint64_t localTimeStamp = 0xFFFFFFFFFFFFFFFF;
	uint64_t enemyTimeStamp = 0xFFFFFFFFFFFFFFFF;
	if(pidDependancy[pid] == -1)
	{
		pidDependancy[pid] = collisionSet[0].enemyPid;
		bool triggerAbort = false;
		for(size_t i = 0; i < collisionSet.size(); i++)
		{
			I(pid == collisionSet[i].localPid);
			if(collisionSet[i].enemyPid == pidDependancy[pid])
			{
				I(timeStamp.find(collisionSet[i].localLevel) != timeStamp.end());
				I(timeStamp.find(collisionSet[i].enemyLevel) != timeStamp.end());
				localTimeStamp = std::min(localTimeStamp,timeStamp[collisionSet[i].localLevel]);
				enemyTimeStamp = std::min(enemyTimeStamp,timeStamp[collisionSet[i].enemyLevel]);
				if(levelDependancy[pid].find(collisionSet[i].enemyLevel) == levelDependancy[pid].end())
				{
					I(collisionSet[i].enemyLevel != 0);
					levelDependancy[pid].insert(collisionSet[i].enemyLevel);
				}
			}
		}
		if(pidDominance[pid].find(pidDependancy[pid]) != pidDominance[pid].end())
		{
			triggerAbort = true;
			std::cout << "Dominating ";
		}
		else
		{
			HASH_SET<int> transitiveReliance;
			transitiveReliance.insert(pid);
			int enemy = pidDependancy[pid];
			while(enemy != -1 && triggerAbort == false)
			{
				if(transitiveReliance.find(enemy) == transitiveReliance.end())
				{
					transitiveReliance.insert(enemy);
					enemy = pidDependancy[enemy];
				}
				else
				{
					triggerAbort = true;
				}
			}
		}
		if(triggerAbort)
		{
			std::cout << "Abort triggered" << std::endl;
			if(enemyTimeStamp < localTimeStamp || (enemyTimeStamp == localTimeStamp && pid > pidDependancy[pid]))
			{//abort self
				for(HASH_SET<uint32_t>::const_iterator it = levelDependancy[pidDependancy[pid]].begin(); it != levelDependancy[pidDependancy[pid]].end(); it++)
				{
					if(abortMask.find(*it) == abortMask.end())
					{
						abortMask.insert(*it);
					}
				}
				pidDominance[pidDependancy[pid]].insert(pid);
			}
			else 
			{//abort enemy
				for(HASH_SET<uint32_t>::const_iterator it = levelDependancy[pid].begin(); it != levelDependancy[pid].end(); it++)
				{
					if(abortMask.find(*it) == abortMask.end())
					{
						abortMask.insert(*it);
					}
				}
				pidDominance[pid].insert(pidDependancy[pid]);
			}
		}
		else
		{
			std::cout << "Stall triggered" << std::endl;
			delay = backoff[pid] = minBackoffTime;
		}
	}
	for(HASH_SET<uint32_t>::const_iterator it = abortMask.begin(); it != abortMask.end(); it++)
	{
		abortSet.push_back(*it);
	}
	abortIndex = 0;
	return abortSet.size(); 
}
uint32_t PerfectCollisionManager::AbortID()
{
	uint32_t transID = abortSet[abortIndex];
	abortIndex++;
	return transID;
}

void PerfectCollisionManager::DumpStats(std::ostream& out)
{
	out << "string CollisionManager Perfect #" << std::endl;
	for(size_t i = 0; i < transResultBuffer.size(); i++)
	{
		out << "int Pid " << transResultBuffer[i]->pid << " int TransID " << transResultBuffer[i]->transID << " int StartTime " << transResultBuffer[i]->beginStamp << " int EndTime " << transResultBuffer[i]->endStamp << " int ParentID " << transResultBuffer[i]->parentID << " int StallCycles " << transResultBuffer[i]->stallCycles << " int DelayCycles " << transResultBuffer[i]->stallBeforeStart << " int ReadCount " << transResultBuffer[i]->readCount << " int WriteCount " << transResultBuffer[i]->writeCount << " bool Committed ";
		if(transResultBuffer[i]->committed)
		{
			out << "true";
		}
		else
		{
			out << "false";
		}
		out << " #" << std::endl;
	}
}
#endif
