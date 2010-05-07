#include "LogTMCollisionDetection.h"
#include "TMOpCodes.h"

bool LogTMCollisionDetectionBase::IsInSet(int pid, VAddr addr, bool read)
{
	uint32_t tag = CalcTag(addr);
	for(uint32_t i = 0; i < associativity; i++)
	{
		CollisionDataEntry& cd = GetDataEntry(pid,tag,i);
		if(cd.valid && cd.tag == tag)
		{
			if(read)
			{
				return cd.readSet;
			}
			else
			{
				return cd.writeSet;
			}
		}
	}
	return false;
}
void LogTMCollisionDetectionBase::AddToSet(int pid, VAddr addr, bool read)
{
	uint32_t tag = CalcTag(addr);
	for(uint32_t i = 0; i < associativity; i++)
	{
		CollisionDataEntry& cd = GetDataEntry(pid,tag,i);
		if(!cd.valid)
		{
			cd.tag = tag;
			cd.valid = true;
		}
		if(cd.valid && cd.tag == tag)
		{
			if(read)
			{
				cd.readSet = true;
			}
			else
			{
				cd.writeSet = true;
			}
			return;
		}
	}
	if(IsOverflown(pid,addr))
	{
		return;
	}
	if(setSpecificOverflow)
	{
		overflow[pid][tag % overflow[pid].size()] = true;
	}
	else
	{
		for(size_t i = 0; i < overflow[pid].size(); i++)
		{
			overflow[pid][i] = true;
		}
	}
}
void LogTMCollisionDetectionBase::ClearCollisionData(int pid)
{
	for(size_t i = 0; i < overflow[pid].size(); i++)
	{
		overflow[pid][i] = false;
	}
	for(size_t i = 0; i < collisionVector[pid].size(); i++)
	{
		collisionVector[pid][i].valid = false;
		collisionVector[pid][i].readSet = false;
		collisionVector[pid][i].writeSet = false;
		collisionVector[pid][i].tag = 0;
	}
}
LogTMCollisionDetectionBase::LogTMCollisionDetectionBase(size_t associativity, size_t cacheWidth, size_t lineSize, bool setSpecific)
{
	this->associativity = associativity;
	this->setSpecificOverflow = setSpecific;
	this->cacheWidth = cacheWidth;
	this->lineSize = lineSize;
	lastAddress = 0;
	lastTransID = newTransID = 0;
	lastPid = -1;
	isRead = isAccess = false;
	transOpCode = 0;
	accessSize = 0;
}
LogTMCollisionDetectionBase::~LogTMCollisionDetectionBase(){}
void LogTMCollisionDetectionBase::BeginRabbit(){}
void LogTMCollisionDetectionBase::EndRabbit(){}
void LogTMCollisionDetectionBase::Initialize(TMProcessor** procSet, int procCount, TMMemory* memory)
{
	collisionVector.resize(procCount);
	for(int i = 0; i < procCount; i++)
	{
		overflow.push_back(std::vector<bool>(cacheWidth));
		for(size_t j = 0; j < overflow[i].size(); j++)
		{
			overflow[i][j] = false;
		}
		currentTransaction.push_back(0);
		collisionVector[i].resize(associativity * cacheWidth);
	}
}
void LogTMCollisionDetectionBase::Destroy(){}
void LogTMCollisionDetectionBase::ObserveAccess(bool isRead, int pid, uint32_t transactionEntry, VAddr address, size_t size)
{
	isAccess = true;
	lastPid = pid;
	this->isRead = isRead;
	lastTransID = transactionEntry;
	accessSize = size;
	lastAddress = address;
}
void LogTMCollisionDetectionBase::ObserveSpecialInst(uint32_t opCode,
	uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5)
{

}
void LogTMCollisionDetectionBase::ObserveTransactionInst(int pid, uint32_t oldLevel, uint32_t newLevel, uint32_t opCode,
	uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5)
{
	isAccess = false;
	lastPid = pid;
	lastTransID = oldLevel;
	newTransID = newLevel;
	transOpCode = opCode;
}
void LogTMCollisionDetectionBase::ObserveCacheEvict(int pid, VAddr address, int cacheLayer, int newCacheLayer, size_t size){}
void LogTMCollisionDetectionBase::ClockTick(){lastPid = -1;}
void LogTMCollisionDetectionBase::FinalizeTick(){}
void LogTMCollisionDetectionBase::FinalizeInstruction()
{
	if(isAccess)
	{
		if(lastTransID)
		{
			I(lastTransID == currentTransaction[lastPid]);
			AddToSet(lastPid,lastAddress,isRead);
		}
	}
	else
	{
		switch(transOpCode)
		{
		case(TMOpCodes::TransBegin):
			I(newTransID);
			I(currentTransaction[lastPid] == 0);
			currentTransaction[lastPid] = newTransID;
			ClearCollisionData(lastPid);
			transResultBuffer.push_back(new TransactionResult());
			activeTransResultSet[newTransID] = transResultBuffer.back();
			transResultBuffer.back()->pid = lastPid;
			transResultBuffer.back()->transID = newTransID;
			break;
		case(TMOpCodes::TransEnd):
		case(TMOpCodes::TransAbort):
			I(lastTransID);
			I(newTransID == 0);
			I(currentTransaction[lastPid] == lastTransID);
			currentTransaction[lastPid] = newTransID; // = 0;
			for(size_t i = 0; i < overflow[lastPid].size(); i++)
			{
				if(overflow[lastPid][i])
				{
					activeTransResultSet[lastTransID]->overflowed++;
				}
			}
			for(size_t i = 0; i < collisionVector[lastPid].size(); i++)
			{
				if(collisionVector[lastPid][i].valid && collisionVector[lastPid][i].readSet)
				{
					activeTransResultSet[lastTransID]->readSet.push_back(lineSize * collisionVector[lastPid][i].tag);
				}
				if(collisionVector[lastPid][i].valid && collisionVector[lastPid][i].writeSet)
				{
					activeTransResultSet[lastTransID]->writeSet.push_back(lineSize * collisionVector[lastPid][i].tag);
				}
			}
			activeTransResultSet.erase(lastTransID);
			ClearCollisionData(lastPid);
			break;
		default:
			I(0);
		}
	}
	lastPid = -1;
}
void LogTMCollisionDetectionBase::RollbackInstruction()
{
	lastPid = -1;
}
bool LogTMCollisionDetectionBase::HasTransfers(int cpuID)
{
	return false;
}
void LogTMCollisionDetectionBase::GetTransferInfo(bool& isRead, bool& isRequired, VAddr& address, size_t& transferSize)
{
	I(0);
}
void LogTMCollisionDetectionBase::AcceptTransfer()
{
	I(0);
}
void LogTMCollisionDetectionBase::DenyTransfer()
{
	I(0);
}
void LogTMCollisionDetectionBase::ConfirmTransferCompleted(int cpuID, bool isRead, VAddr address)
{
	I(0);
}
uint32_t LogTMCollisionDetectionBase::GetDelay()
{
	return 0;
}
void LogTMCollisionDetectionBase::MayNotDelay(){}
int LogTMCollisionDetectionBase::CollisionCount()
{
	collisionPids.clear();
	if(lastPid >= 0 && isAccess && currentTransaction[lastPid] && lastTransID)
	{
		I(lastTransID == currentTransaction[lastPid]);
		int procCount = (int) currentTransaction.size();
		for(int i = 0; i < procCount; i++)
		{
			if(lastPid != i && currentTransaction[i] && ExistsCollision(i,lastAddress,isRead))
			{
				collisionIndex = 0;
				collisionPids.push_back(i);
			}
		}
	}
	return (int)collisionPids.size();
}
void LogTMCollisionDetectionBase::FetchCollision(
	int& local, uint32_t& localLevel, bool& localIsDetermined, bool& localIsRead,
	int& enemy, uint32_t& enemyLevel, bool& enemyIsDetermined, bool& enemyIsRead)
{
	local = lastPid;
	localLevel = currentTransaction[lastPid];
	localIsDetermined = true;
	localIsRead = isRead;
	enemy = collisionPids[collisionIndex];
	enemyLevel = currentTransaction[collisionPids[collisionIndex]];
	enemyIsDetermined = false;
	enemyIsRead = false;
	collisionIndex++;
}
void LogTMCollisionDetectionBase::DumpStats(std::ostream& out)
{
	std::cout << "beginning collision detection dump" << std::endl;
	out << "string CollisionDetection LogTM int LineSize " << lineSize << " #" << std::endl;
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
		out << "int Overflow " << transResultBuffer[i]->overflowed << " #" << std::endl;
		out << "EndGroup #" << std::endl;
	}
	std::cout << "ending collision detection dump" << std::endl;
}