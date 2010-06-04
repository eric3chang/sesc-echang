#include "TMInterface.h"
#include "OSSim.h"
#include "TMOpCodes.h"
#include "TMProcessor.h"
#include "ExecutionFlow.h"
#include "FetchEngine.h"
#include "TMMemory.h"
#include "ITMCollisionDetection.h"
#include "ITMCollisionManager.h"
#include "ITMVersioning.h"
#include "PerfectCollisionDetection.h"
#include "PerfectVersioning.h"
#include "PerfectCollisionManager.h"
#include "CollisionDetectionFlatten.h"
#include "CollisionDetectionNoBlend.h"
#include "LogTMCollisionDetection.h"
#include "LogTMSECollisionDetection.h"
#include "CollisionDetectionExcludeAllBut.h"

#ifdef TRANSACTIONAL_MEMORY
#ifdef TRANSACTIONAL_MEMORY
#ifndef SELECTED_COLLISION_DETECTION
//#define SELECTED_COLLISION_DETECTION new CollisionDetectionFlatten<CollisionDetectionNoBlend<PerfectCollisionDetection>>(4)
#define SELECTED_COLLISION_DETECTION new CollisionDetectionFlatten<CollisionDetectionNoBlend<PerfectCollisionDetection> >(64)
//#define SELECTED_COLLISION_DETECTION new LogTMCollisionDetection(4, SescConf->getInt("DataL2Node0","size") / (4 * 64), 64)
//#define SELECTED_COLLISION_DETECTION new LogTMCollisionDetection(4, SescConf->getInt("DataL2Node0","size") / (4 * 64), 64,true)
//#define SELECTED_COLLISION_DETECTION new LogTMSECollisionDetection(SescConf->getInt("","FilterSize"),64,1,new LogTMSECollisionDetection::H3Hash())
//#define SELECTED_COLLISION_DETECTION new LogTMSECollisionDetection(SescConf->getInt("","FilterSize"),64,3,new LogTMSECollisionDetection::H3Hash())
//#define SELECTED_COLLISION_DETECTION new CollisionDetectionExcludeAllBut(1, new CollisionDetectionFlatten<CollisionDetectionNoBlend<PerfectCollisionDetection>>(4))
//#define SELECTED_COLLISION_DETECTION new CollisionDetectionExcludeAllBut(1, new CollisionDetectionFlatten<CollisionDetectionNoBlend<PerfectCollisionDetection>>(64))
//#define SELECTED_COLLISION_DETECTION new CollisionDetectionExcludeAllBut(1, new LogTMCollisionDetection(4, SescConf->getInt("DataL2Node0","size") / (4 * 64), 64))
//#define SELECTED_COLLISION_DETECTION new CollisionDetectionExcludeAllBut(1, new LogTMCollisionDetection(4, SescConf->getInt("DataL2Node0","size") / (4 * 64), 64, true))
//#define SELECTED_COLLISION_DETECTION new CollisionDetectionExcludeAllBut(1, new LogTMSECollisionDetection(SescConf->getInt("","FilterSize"),64,1,new LogTMSECollisionDetection::H3Hash()))
//#define SELECTED_COLLISION_DETECTION new CollisionDetectionExcludeAllBut(1, new LogTMSECollisionDetection(SescConf->getInt("","FilterSize"),64,3,new LogTMSECollisionDetection::H3Hash()))
#endif
#ifndef SELECTED_COLLISION_MANAGER
#define SELECTED_COLLISION_MANAGER new PerfectCollisionManager();
#endif
#ifndef SELECTED_VERSIONING
#define SELECTED_VERSIONING new PerfectVersioning(64,false,0);
#endif
#else
#ifndef SELECTED_COLLISION_DETECTION
#define SELECTED_COLLISION_DETECTION NULL;
#endif
#ifndef SELECTED_COLLISION_MANAGER
#define SELECTED_COLLISION_MANAGER NULL;
#endif
#ifndef SELECTED_VERSIONING
#define SELECTED_VERSIONING NULL;
#endif
#endif
#ifdef TRANSACTIONAL_COMPOSITION_TRACKING
bool TMInterface::compositionRecordingEnabled = false;
TMInterface::TransCompositionRecord* TMInterface::compositionRecordSet = NULL;
#endif

std::vector<int> pidCompletionPattern;

HASH_MAP<int32_t, std::vector<TMInterface::SpecOperation> > TMInterface::extraSpecialInstructions;
std::stack<TMInterface::StoredAccess*> TMInterface::StoredAccess::pool;
std::stack<TMInterface::TransOperation*> TMInterface::TransOperation::pool;
TMInterface::ULL_HASH_MAP<TMInterface::StoredAccess*> TMInterface::accessSet;
HASH_MAP<uint32_t,TMInterface::TransRecord> TMInterface::transRecordSet;
std::vector<uint32_t> TMInterface::lastTransactionSeen;
HASH_MAP<uint32_t,TMProcessorCheckpoint> TMInterface::checkpointSet;
HASH_MAP<uint32_t,TMInterface::PrivateMemory*> TMInterface::privateMemorySet;
std::vector<std::queue<uint32_t> > TMInterface::privateMemoryQueue;
std::vector<uint32_t> TMInterface::currentTransID;
std::vector<int> TMInterface::residualInstructionCount;
std::vector<bool> TMInterface::raceAheadStarted;
std::vector<bool> TMInterface::raceAheadFinished;
std::vector<bool> TMInterface::raceAheadMode;
std::vector<std::vector<TMInterface::SpecOperation> > TMInterface::pendingSpecOpSet;
std::vector<uint32_t> TMInterface::abortedBeforeSeenTransactions;
std::vector<uint32_t> TMInterface::correctionAbortedTransactions;
std::vector<uint32_t> TMInterface::correctedTransactions;
uint32_t TMInterface::globalTransactionID = 1;
std::vector<uint32_t> TMInterface::latestMemorySlice;
TMProcessor** TMInterface::procs = NULL;
int TMInterface::procCount = 0;
TMMemory* TMInterface::memory = NULL;
ITMCollisionDetection* TMInterface::collisionDetection = NULL;
ITMCollisionManager* TMInterface::collisionManager = NULL;
ITMVersioning* TMInterface::versioning = NULL;
bool TMInterface::goingRabbit = false;
bool TMInterface::forceInOrderTrans = true;
int TMInterface::deviceTransfering = -1;
uint32_t TMInterface::activeAddr = 0;
HASH_MAP<uint32_t,int> TMInterface::inProgressTransfer;
bool TMInterface::actionToProcess = false;
bool TMInterface::lastInstIsAccess;
bool TMInterface::lastInstIsRead;
int TMInterface::lastActionPid;
uint32_t TMInterface::accessAddr;
size_t TMInterface::accessSize;
icode* TMInterface::lastInstructionPtr;
uint32_t TMInterface::transOpCode;
uint32_t TMInterface::lastTransArg1;
uint32_t TMInterface::lastTransArg2;
uint32_t TMInterface::lastTransArg3;
uint32_t TMInterface::lastTransArg4;
uint32_t TMInterface::lastTransArg5;
void* TMInterface::lastRegEditLocation;
size_t TMInterface::lastRegEditSize;
bool TMInterface::deadInstruction = false;
bool TMInterface::runningAhead = false;
uint32_t TMInterface::retryTime = 0;
std::queue<uint32_t> TMInterface::pendingAborts;
bool TMInterface::handlingEmittedInstr = false;
void TMInterface::HandleEmittedInstructions()
{
	if(!handlingEmittedInstr)
	{
		handlingEmittedInstr = true;
		while(!pendingAborts.empty())
		{
			if(transRecordSet.find(pendingAborts.front()) != transRecordSet.end())
			{
				if(!transRecordSet[pendingAborts.front()].resolved)
				{
					AbortTransaction(pendingAborts.front());
				}
			}
			pendingAborts.pop();
		}
		handlingEmittedInstr = false;
	}
}
uint32_t TMInterface::CalcRetryTime()
{
	return std::max(std::max(collisionDetection->GetDelay(),collisionManager->GetDelay()),versioning->GetDelay());
}
void TMInterface::CheckCollisions()
{
	int collisionCount = collisionDetection->CollisionCount();
	collisionManager->ReportCollisionCount(collisionCount);
	for(int i = 0; i < collisionCount; i++)
	{
		int localPid, enemyPid;
		uint32_t localLevel, enemyLevel;
		bool localDetermined, enemyDetermined, localRead, enemyRead;
		collisionDetection->FetchCollision(
			localPid,localLevel,localDetermined,localRead,
			enemyPid,enemyLevel,enemyDetermined,enemyRead);
		collisionManager->ReportCollision(
			localPid,localLevel,localDetermined,localRead,
			enemyPid,enemyLevel,enemyDetermined,enemyRead);
	}
	if(collisionCount)
	{
		int abortCount = collisionManager->AbortCount();
		for(int i = 0; i < abortCount; i++)
		{
			pendingAborts.push(collisionManager->AbortID());
		}
	}
}
void TMInterface::CommitPrivateMemory(int pid)
{
	I(!privateMemoryQueue[pid].empty());
	while(!privateMemoryQueue[pid].empty())
	{
		uint32_t transID = privateMemoryQueue[pid].front();
		I(transID);
		I(transRecordSet.find(transID) != transRecordSet.end());
		I(checkpointSet.find(transID) != checkpointSet.end());
		I(privateMemorySet.find(transID) != privateMemorySet.end());
		I(privateMemorySet[transID]->previous == NULL);
		I(transRecordSet[transID].pid == pid);
		if(!transRecordSet[transID].resolved)
		{
			return;
		}
		privateMemoryQueue[pid].pop();
		I(privateMemorySet[transID]->committed);
		pidCompletionPattern.push_back(pid);
		if(privateMemorySet[transID]->TestIntegrity())
		{
			privateMemorySet[transID]->ApplyChanges();
			if(privateMemorySet[transID]->next)
			{
				I(latestMemorySlice[pid] != transID);
				I(privateMemorySet[transID]->next->transID == privateMemoryQueue[pid].front());
				I(privateMemorySet[transID]->next->previous == privateMemorySet[transID]);
				I(privateMemorySet.find(privateMemorySet[transID]->next->transID) != privateMemorySet.end());
				PrivateMemory* child = privateMemorySet[transID]->next;
				while(child)
				{
					privateMemorySet[transID]->RemoveFromUnder(child);
					child = child->next;
				}
				privateMemorySet[transID]->next->previous = NULL;
				privateMemorySet[transID]->next = NULL;
			}
			for(HASH_MAP<uint32_t,PrivateMemory*>::iterator it = privateMemorySet.begin(); it != privateMemorySet.end(); it++)
			{
				if(it->second->pid != pid)
				{
					it->second->IncorporateCommitted(privateMemorySet[transID]);
				}
			}
			if(latestMemorySlice[pid] == transID)
			{
				latestMemorySlice[pid] = 0;
			}
			delete privateMemorySet[transID];
			privateMemorySet.erase(transID);
		}
		else
		{
			I(!raceAheadMode[pid]);
			correctedTransactions.push_back(transID);
			while(!privateMemoryQueue[pid].empty())
			{
				residualInstructionCount[pid] = 0;
				I(transRecordSet.find(privateMemoryQueue[pid].front()) != transRecordSet.end());
				if(transRecordSet[privateMemoryQueue[pid].front()].progressObserved && transRecordSet[privateMemoryQueue[pid].front()].resolved)
				{
					I(transRecordSet[privateMemoryQueue[pid].front()].committed);
					correctionAbortedTransactions.push_back(privateMemoryQueue[pid].front());
					privateMemorySet[privateMemoryQueue[pid].front()]->Extract();
					delete privateMemorySet[privateMemoryQueue[pid].front()];
					privateMemorySet.erase(privateMemoryQueue[pid].front());
					TryRetireTransaction(privateMemoryQueue[pid].front());
					privateMemoryQueue[pid].pop();
				}
				else if(transRecordSet[privateMemoryQueue[pid].front()].progressObserved)
				{
					correctionAbortedTransactions.push_back(privateMemoryQueue[pid].front());
					AbortTransaction(privateMemoryQueue[pid].front());
				}
				else
				{
					abortedBeforeSeenTransactions.push_back(privateMemoryQueue[pid].front());
					AbortTransaction(privateMemoryQueue[pid].front());
				}
			}
			HandleEmittedInstructions();
			latestMemorySlice[pid] = 0;
			lastTransactionSeen[pid] = 0;
			currentTransID[pid] = 0;
			RerunCommittingTransaction(transID);
		}
		TryRetireTransaction(transID);
	}
}
void TMInterface::FixPrivateMemory(int pid)
{
	std::queue<uint32_t> tempBuffer = privateMemoryQueue[pid];
	while(!privateMemoryQueue[pid].empty())
	{
		privateMemoryQueue[pid].pop();
	}
	while(!tempBuffer.empty())
	{
		uint32_t transID = tempBuffer.front();
		I(transRecordSet.find(transID) != transRecordSet.end());
		I(privateMemorySet.find(transID) != privateMemorySet.end());
		if(transRecordSet[transID].resolved && !transRecordSet[transID].committed)
		{
			if(latestMemorySlice[pid] == transID)
			{
				if(privateMemorySet[transID]->previous)
				{
					latestMemorySlice[pid] = privateMemorySet[transID]->previous->transID;
				}
				else
				{
					latestMemorySlice[pid] = 0;
				}
			}
			if(privateMemorySet[transID]->previous)
			{
				privateMemorySet[transID]->previous->next = privateMemorySet[transID]->next;
			}
			privateMemorySet[transID]->previous = NULL;
			I(privateMemorySet[transID]->next == NULL);
			delete privateMemorySet[transID];
			privateMemorySet.erase(transID);
		}
		else
		{
			privateMemoryQueue[pid].push(transID);
		}
		tempBuffer.pop();
	}
}
void TMInterface::RerunCommittingTransaction(uint32_t transID)
{
	I(transRecordSet.find(transID) != transRecordSet.end());
	I(checkpointSet.find(transID) != checkpointSet.end());
	I(privateMemorySet.find(transID) != privateMemorySet.end());
	I(transRecordSet[transID].parentTransactionID == 0);
	procs[transRecordSet[transID].pid]->RestoreCheckpoint(checkpointSet[transID]);
	ExecutionFlow* flow = (ExecutionFlow*)osSim->cpus.getProcessor(transRecordSet[transID].pid)->currentFlow()->getFlow();
	raceAheadMode[transRecordSet[transID].pid] = true;
	std::cout << "Race ahead mode entered on " << transRecordSet[transID].pid << std::endl;
	flow->RaceAhead();
	raceAheadMode[transRecordSet[transID].pid] = false;
	I(raceAheadStarted[transRecordSet[transID].pid]);
	I(raceAheadFinished[transRecordSet[transID].pid]);
	raceAheadStarted[transRecordSet[transID].pid] = false;
	raceAheadFinished[transRecordSet[transID].pid] = false;
	std::cout << "Race ahead mode ended" << std::endl;
	residualInstructionCount[transRecordSet[transID].pid] = 0;
}
void TMInterface::ExecuteAccess(StoredAccess* m)
{
	collisionDetection->ObserveAccess(m->isRead,m->pid,m->transactionEntry,m->addr,m->accessSize);
	collisionManager->ObserveAccess(m->isRead,m->pid,m->transactionEntry,m->addr,m->accessSize);
	versioning->ObserveAccess(m->isRead,m->pid,m->transactionEntry,m->addr,m->accessSize);

	for(size_t i = 0; i < m->specOpSet.size(); i++)
	{
		collisionDetection->ObserveSpecialInst(m->specOpSet[i].opCode,m->specOpSet[i].arg1,m->specOpSet[i].arg2,m->specOpSet[i].arg3,m->specOpSet[i].arg4,m->specOpSet[i].arg5);
		collisionManager->ObserveSpecialInst(m->specOpSet[i].opCode,m->specOpSet[i].arg1,m->specOpSet[i].arg2,m->specOpSet[i].arg3,m->specOpSet[i].arg4,m->specOpSet[i].arg5);
		versioning->ObserveSpecialInst(m->specOpSet[i].opCode,m->specOpSet[i].arg1,m->specOpSet[i].arg2,m->specOpSet[i].arg3,m->specOpSet[i].arg4,m->specOpSet[i].arg5);
	}

	CheckCollisions();

	retryTime = CalcRetryTime();
	if(retryTime == 0)
	{
		collisionDetection->FinalizeInstruction();
		collisionManager->FinalizeInstruction();
		versioning->FinalizeInstruction();
	}
	else
	{
		collisionDetection->RollbackInstruction();
		collisionManager->RollbackInstruction();
		versioning->RollbackInstruction();
	}
	uint32_t preservedRetryTime = retryTime;
	HandleEmittedInstructions();
	retryTime = preservedRetryTime;
}
void TMInterface::ExecuteTransactionInst(TransOperation* op, bool forceNoDelay)
{
	collisionDetection->ObserveTransactionInst(op->pid,op->oldTransactionEntry,op->newTransactionEntry,op->opCode,op->arg1,op->arg2,op->arg3,op->arg4,op->arg5);
	collisionManager->ObserveTransactionInst(op->pid,op->oldTransactionEntry,op->newTransactionEntry,op->opCode,op->arg1,op->arg2,op->arg3,op->arg4,op->arg5);
	versioning->ObserveTransactionInst(op->pid,op->oldTransactionEntry,op->newTransactionEntry,op->opCode,op->arg1,op->arg2,op->arg3,op->arg4,op->arg5);

	for(size_t i = 0; i < op->specOpSet.size(); i++)
	{
		collisionDetection->ObserveSpecialInst(op->specOpSet[i].opCode,op->specOpSet[i].arg1,op->specOpSet[i].arg2,op->specOpSet[i].arg3,op->specOpSet[i].arg4,op->specOpSet[i].arg5);
		collisionManager->ObserveSpecialInst(op->specOpSet[i].opCode,op->specOpSet[i].arg1,op->specOpSet[i].arg2,op->specOpSet[i].arg3,op->specOpSet[i].arg4,op->specOpSet[i].arg5);
		versioning->ObserveSpecialInst(op->specOpSet[i].opCode,op->specOpSet[i].arg1,op->specOpSet[i].arg2,op->specOpSet[i].arg3,op->specOpSet[i].arg4,op->specOpSet[i].arg5);
	}

	CheckCollisions();

	if(forceNoDelay)
	{
		retryTime = 0;
		collisionDetection->MayNotDelay();
		collisionManager->MayNotDelay();
		versioning->MayNotDelay();
	}
	else
	{
		retryTime = CalcRetryTime();
	}
	if(retryTime == 0)
	{
		collisionDetection->FinalizeInstruction();
		collisionManager->FinalizeInstruction();
		versioning->FinalizeInstruction();
	}
	else
	{
		collisionDetection->RollbackInstruction();
		collisionManager->RollbackInstruction();
		versioning->RollbackInstruction();
	}
	uint32_t preservedRetryTime = retryTime;
	HandleEmittedInstructions();
	retryTime = preservedRetryTime;
}
bool TMInterface::BeginTransaction(uint32_t transID, bool force)
{
	I(transID);
	I(transRecordSet.find(transID) != transRecordSet.end());
	I(checkpointSet.find(transID) != checkpointSet.end());
	I(!transRecordSet[transID].resolved);
	I(privateMemorySet.find(transID) != privateMemorySet.end());
	TransRecord& tr = transRecordSet[transID];
	if(tr.progressObserved)
	{
		return true;
	}
	if(forceInOrderTrans && tr.dependantTransaction)
	{
		if(transRecordSet.find(tr.dependantTransaction) != transRecordSet.end() && !transRecordSet[tr.dependantTransaction].resolved)
		{
			if(!BeginTransaction(tr.dependantTransaction,force))
			{
				I(force == false);
				return false;
			}
		}
	}
	if(tr.parentTransactionID)
	{
		I(transRecordSet.find(tr.parentTransactionID) != transRecordSet.end());
		if(!BeginTransaction(tr.parentTransactionID,force))
		{
			I(force == false);
			return false;
		}
	}
	I(tr.transStartOp);
	if(!raceAheadMode[tr.pid] || raceAheadFinished[tr.pid])
	{
		ExecuteTransactionInst(tr.transStartOp,force | raceAheadMode[tr.pid]);
		if(retryTime)
		{
			I(force == false);
			return false;
		}
	}
	if(raceAheadMode[tr.pid])
	{
		raceAheadStarted[tr.pid] = true;
	}
	tr.progressObserved = true;
	return true;
}
void TMInterface::EndTransaction(uint32_t transID, bool force)
{
	I(transID);
	I(transRecordSet.find(transID) != transRecordSet.end());
	I(checkpointSet.find(transID) != checkpointSet.end());
	I(privateMemorySet.find(transID) != privateMemorySet.end());
	I(MayEndTransaction(transID));
	TransRecord& tr = transRecordSet[transID];
	I(tr.progressObserved);
	I(tr.transEndOp);
	I(tr.transEndOp->opCode == TMOpCodes::TransEnd);
	//uint32_t parentID = tr.parentTransactionID;	//unused variable
	if(!raceAheadMode[tr.pid])
	{
		ExecuteTransactionInst(tr.transEndOp,force);
		if(retryTime)
		{
			I(force == false);
			return;
		}
#ifdef TRANSACTIONAL_COMPOSITION_TRACKING
		if(compositionRecordingEnabled)
		{
			compositionRecordSet->RecordTransactionCommit(tr.transactionID,procs[tr.pid]->GetProcTime());
		}
#endif
	}
	if(lastTransactionSeen[tr.pid] == tr.transEndOp->oldTransactionEntry)
	{
		lastTransactionSeen[tr.pid] = tr.transEndOp->newTransactionEntry;
	}
	if(tr.transEndOp->newTransactionEntry)
	{
		transRecordSet[tr.transEndOp->newTransactionEntry].accessCounter--;
	}
	tr.resolved = true;
	tr.committed = true;
	I(privateMemorySet.find(transID) != privateMemorySet.end());
	privateMemorySet[transID]->committed = true;
	if(tr.parentTransactionID)
	{
		if(latestMemorySlice[tr.pid] == transID)
		{
			if(privateMemorySet[transID]->previous)
			{
				I(privateMemorySet[transID]->previous->transID == tr.parentTransactionID);
				latestMemorySlice[tr.pid] = privateMemorySet[transID]->previous->transID;
			}

		}
		privateMemorySet[transID]->MergeDown();
		if(MayEndTransaction(tr.parentTransactionID))
		{
			EndTransaction(tr.parentTransactionID,force);
		}
	}
	CommitPrivateMemory(tr.pid);
}	
void TMInterface::AbortTransaction(uint32_t transID)
{
	I(transID);
	I(transRecordSet.find(transID) != transRecordSet.end());
	I(checkpointSet.find(transID) != checkpointSet.end());
	AbortTransactionTree(transID);
	TransRecord& tr = transRecordSet[transID];
	if(tr.parentTransactionID)
	{
		I(transRecordSet.find(tr.parentTransactionID) != transRecordSet.end());
		I(!transRecordSet[tr.parentTransactionID].resolved);
	}
	if(!tr.progressObserved)
	{
		if(tr.parentTransactionID)
		{
			I(transRecordSet.find(tr.parentTransactionID) != transRecordSet.end());
			I(transRecordSet[tr.parentTransactionID].accessCounter > 0);
			transRecordSet[tr.parentTransactionID].accessCounter--;
		}
	}
	else if(!tr.resolved)
	{
		TransOperation to;
		to.arg1 = to.arg2 = to.arg3 = to.arg4 = to.arg5 = 0;
		to.pid = tr.pid;
		to.newTransactionEntry = tr.parentTransactionID;
		to.oldTransactionEntry = tr.transactionID;
		to.stamp = 0;
		to.opCode = TMOpCodes::TransAbort;
		if(!raceAheadMode[tr.pid])
		{
			ExecuteTransactionInst(&to,true);
			I(retryTime == 0);
		}
#ifdef TRANSACTIONAL_COMPOSITION_TRACKING
		if(compositionRecordingEnabled)
		{
			compositionRecordSet->RecordTransactionAbort(tr.transactionID,procs[tr.pid]->GetProcTime());
		}
#endif
		if(lastTransactionSeen[to.pid] == to.oldTransactionEntry)
		{
			lastTransactionSeen[to.pid] = to.newTransactionEntry;
		}
		if(to.newTransactionEntry)
		{
			I(transRecordSet.find(to.newTransactionEntry) != transRecordSet.end());
			transRecordSet[to.newTransactionEntry].accessCounter--;
		}
	}
	if(currentTransID[tr.pid] == tr.transactionID)
	{
		currentTransID[tr.pid] = tr.parentTransactionID;
	}
	tr.resolved = true;
	tr.committed = false;
	FixPrivateMemory(tr.pid);
	procs[tr.pid]->RestoreCheckpoint(checkpointSet[transID]);
	TryRetireTransaction(transID);
}
bool TMInterface::MayEndTransaction(uint32_t transID)
{
	I(transID);
	I(transRecordSet.find(transID) != transRecordSet.end());
	TransRecord& tr = transRecordSet[transID];
	if(tr.progressObserved && tr.transEndOp && tr.accessCounter == 0 && tr.resolved == false)
	{
		return true;
	}
	return false;
}

void TMInterface::RetireAccess(uint64_t stamp)
{
	I(accessSet.find(stamp) != accessSet.end());
	StoredAccess* a = accessSet[stamp];
	if(a->retired)
	{
		return;
	}
	I(!a->transactionEntry || transRecordSet.find(a->transactionEntry) != transRecordSet.end());
	I(!a->dependantTransaction || transRecordSet.find(a->dependantTransaction) != transRecordSet.end());
	if(a->transactionEntry)
	{
		I(transRecordSet.find(a->transactionEntry) != transRecordSet.end());
		I(transRecordSet[a->transactionEntry].accessCounter);
		transRecordSet[a->transactionEntry].accessCounter--;
		TryRetireTransaction(a->transactionEntry);
	}
	if(a->dependantTransaction)
	{
		I(transRecordSet.find(a->dependantTransaction) != transRecordSet.end());
		I(transRecordSet[a->dependantTransaction].dependantAccessCounter);
		transRecordSet[a->dependantTransaction].dependantAccessCounter--;
		TryRetireTransaction(a->dependantTransaction);
	}
	a->retired = true;
}
void TMInterface::DestroyAccess(uint64_t stamp)
{
	I(accessSet.find(stamp) != accessSet.end());
	StoredAccess* a = accessSet[stamp];
	I(a->retired);
	a->Dispose();
	accessSet.erase(stamp);
}
bool TMInterface::IsDeadAccess(uint64_t stamp)
{
	I(accessSet.find(stamp) != accessSet.end());
	StoredAccess* a = accessSet[stamp];
	if(a->dependantTransaction)
	{
		if(transRecordSet.find(a->dependantTransaction) == transRecordSet.end())
		{
			return true;
		}
		if(transRecordSet[a->dependantTransaction].resolved && !transRecordSet[a->dependantTransaction].committed)
		{
			return true;
		}
	}
	return false;
}
void TMInterface::AbortTransactionTree(uint32_t transID)
{
	I(transID);
	I(transRecordSet.find(transID) != transRecordSet.end());
	TransRecord& tr = transRecordSet[transID];
	std::vector<uint32_t> garbageList;
	for(HASH_MAP<uint32_t,TransRecord>::iterator it = transRecordSet.begin(); it != transRecordSet.end(); it++)
	{
		if(it->second.pid == tr.pid && it->second.transactionID > tr.transactionID)
		{
			garbageList.push_back(it->first);
		}
	}
	for(size_t i = 0; i < garbageList.size(); i++)
	{
		if(transRecordSet.find(garbageList[i]) != transRecordSet.end())
		{
			AbortTransaction(garbageList[i]);
		}
	}
}
void TMInterface::TryRetireTransaction(uint32_t transID)
{
	I(transID);
	I(transRecordSet.find(transID) != transRecordSet.end());
	I(checkpointSet.find(transID) != checkpointSet.end());
	if(transRecordSet[transID].accessCounter || transRecordSet[transID].dependantAccessCounter || !transRecordSet[transID].resolved)
	{
		return;
	}
	if(privateMemorySet.find(transID) != privateMemorySet.end())
	{
		if(privateMemorySet[transID]->previous)
		{
			return;
		}
		if(privateMemorySet[transID]->next)
		{
			I(latestMemorySlice[privateMemorySet[transID]->pid] != transID);
			I(privateMemorySet[transID]->next->previous == privateMemorySet[transID]);
			privateMemorySet[transID]->next->previous = NULL;
			privateMemorySet[transID]->next = NULL;
		}
		else
		{
//			I(latestMemorySlice[privateMemorySet[transID]->pid] == transID);
//			if(privateMemorySet[transID]->previous)
//			{
//				latestMemorySlice[privateMemorySet[transID]->pid] = privateMemorySet[transID]->previous->transID;
//			}
//			else
//			{
//				latestMemorySlice[privateMemorySet[transID]->pid] = 0;
//			}
		}
		delete privateMemorySet[transID];
		privateMemorySet.erase(transID);
	}
#ifdef TRANSACTIONAL_COMPOSITION_TRACKING
	if(compositionRecordingEnabled)
	{
		compositionRecordSet->FinalizeTransaction(transID);
	}
#endif
	transRecordSet.erase(transID);
	checkpointSet.erase(transID);
}
#endif
void TMInterface::ClockTick()
{
#ifdef TRANSACTIONAL_MEMORY
	collisionDetection->ClockTick();
	collisionManager->ClockTick();
	versioning->ClockTick();
	CheckCollisions();
	collisionDetection->FinalizeTick();
	collisionManager->FinalizeTick();
	versioning->FinalizeTick();
	HandleEmittedInstructions();
#endif
}
bool TMInterface::HasTransfers(int cpuID)
{
#ifdef TRANSACTIONAL_MEMORY
	if(collisionDetection->HasTransfers(cpuID))
	{
		deviceTransfering = 1;
		return true;
	}
	else if(collisionManager->HasTransfers(cpuID))
	{
		deviceTransfering = 2;
		return true;
	}
	else if(versioning->HasTransfers(cpuID))
	{
		deviceTransfering = 3;
		return true;
	}
	return false;
#else
	return false;
#endif
}
void TMInterface::GetTransferInfo(bool& isRead, bool& isRequired, uint32_t& addr, size_t& size)
{
#ifdef TRANSACTIONAL_MEMORY
	switch(deviceTransfering)
	{
	case(1):
		collisionDetection->GetTransferInfo(isRead,isRequired,addr,size);
		break;
	case(2):
		collisionManager->GetTransferInfo(isRead,isRequired,addr,size);
		break;
	case(3):
		versioning->GetTransferInfo(isRead,isRequired,addr,size);
		break;
	default:
		I(0);
	}
	activeAddr = addr;
#endif
}
void TMInterface::AcceptTransfer()
{
#ifdef TRANSACTIONAL_MEMORY
	inProgressTransfer.insert(std::pair<VAddr,int>(activeAddr,deviceTransfering));
	switch(deviceTransfering)
	{
	case(1):
		collisionDetection->AcceptTransfer();
		break;
	case(2):
		collisionManager->AcceptTransfer();
		break;
	case(3):
		versioning->AcceptTransfer();
		break;
	default:
		I(0);
	}
	deviceTransfering = 0;
	activeAddr = 0;
#endif
}
void TMInterface::DenyTransfer()
{
#ifdef TRANSACTIONAL_MEMORY
	switch(deviceTransfering)
	{
	case(1):
		collisionDetection->DenyTransfer();
		break;
	case(2):
		collisionManager->DenyTransfer();
		break;
	case(3):
		versioning->DenyTransfer();
		break;
	default:
		I(0);
	}
	deviceTransfering = 0;
	activeAddr = 0;
#endif
}
void TMInterface::ConfirmTransferCompleted(int cpuID, bool isRead, uint32_t address)
{
#ifdef TRANSACTIONAL_MEMORY
	I(inProgressTransfer.find(address) != inProgressTransfer.end());
	int device = inProgressTransfer[address];
	switch(device)
	{
	case(1):
		collisionDetection->ConfirmTransferCompleted(cpuID,isRead,address);
		break;
	case(2):
		collisionManager->ConfirmTransferCompleted(cpuID,isRead,address);
		break;
	case(3):
		versioning->ConfirmTransferCompleted(cpuID,isRead,address);
		break;
	default:
		I(0);
	}
	inProgressTransfer.erase(address);
#endif
}
void TMInterface::ObserveAccess(uint64_t instrID)
{
#ifdef TRANSACTIONAL_MEMORY
	if(accessSet.find(instrID) == accessSet.end())
	{
		retryTime = 0;
		return;
	}
	StoredAccess* m = accessSet[instrID];
	I(m->stamp == instrID);
	if(m->transactionEntry)
	{
		if(IsDeadAccess(instrID))
		{//this is a dead instruction, belonging to a transaction that aborted
			retryTime = 0;
			deadInstruction = true;
			RetireAccess(instrID);
			DestroyAccess(instrID);
			return;
		}
		else
		{
			deadInstruction = false;
		}
		I(transRecordSet.find(m->transactionEntry) != transRecordSet.end());
		TransRecord* tr = &(transRecordSet[m->transactionEntry]);
		if(!tr->progressObserved)
		{
			BeginTransaction(tr->transactionID);
			if(retryTime)
			{
				return;
			}
#ifdef TRANSACTIONAL_COMPOSITION_TRACKING
			if(compositionRecordingEnabled)
			{
				compositionRecordSet->RecordTransactionBegin(tr->transactionID,procs[tr->pid]->GetProcTime());
			}
#endif
		}
		if(tr->accessCounter > 0)
		{//may be false if access is being repeated to end a transaction but has already been made globally visible
			uint32_t memTransEntry = m->transactionEntry;
			if(!raceAheadMode[tr->pid])
			{
				ExecuteAccess(m);
				if(retryTime)
				{
					return;
				}
			}
			RetireAccess(instrID);
			if(transRecordSet.find(memTransEntry) == transRecordSet.end())
			{// the transaction aborted, no further action needed
				DestroyAccess(instrID);
				return;
			}
		}
		if(MayEndTransaction(tr->transactionID))
		{
			if(tr->transEndOp->opCode == TMOpCodes::TransEnd)
			{
				EndTransaction(tr->transactionID);
			}
			else
			{
				I(tr->transEndOp->opCode == TMOpCodes::TransAbort);
				AbortTransaction(tr->transactionID);
			}
			if(retryTime)
			{
				return;
			}
		}
		DestroyAccess(instrID);
	}
	else if(m->dependantTransaction)
	{
		if(IsDeadAccess(m->stamp))
		{
			deadInstruction = true;
		}
		else
		{
			deadInstruction = false;
			ExecuteAccess(m);
			if(retryTime)
			{
				return;
			}
		}
		RetireAccess(instrID);
		DestroyAccess(instrID);
	}
#endif
}
uint32_t TMInterface::RetryTime()
{
#ifdef TRANSACTIONAL_MEMORY
	uint32_t tempRetryTime = retryTime;
	retryTime = 0;
	return tempRetryTime;
#else
	return 0;
#endif
}
void TMInterface::Initialize()
{
#ifdef TRANSACTIONAL_MEMORY
	collisionDetection = SELECTED_COLLISION_DETECTION;
	collisionManager = SELECTED_COLLISION_MANAGER;
	versioning = SELECTED_VERSIONING;
	procCount = (int)osSim->getNumCPUs();
	procs = new TMProcessor*[procCount];
#ifdef TRANSACTIONAL_COMPOSITION_TRACKING
	if(SescConf->checkCharPtr("","CompositionResultFile"))
	{
		compositionRecordingEnabled = true;
		std::ofstream* of = new std::ofstream(SescConf->getCharPtr("","CompositionResultFile"),std::ios::out | std::ios::binary);
		compositionRecordSet = new TransCompositionRecord(of,true);
	}
	else
	{
		compositionRecordingEnabled = false;
	}
#endif
	for(int i = 0; i < procCount; i++)
	{
		procs[i] = new TMProcessor(i);
		lastTransactionSeen.push_back(0);
		residualInstructionCount.push_back(0);
		raceAheadStarted.push_back(false);
		raceAheadFinished.push_back(false);
		currentTransID.push_back(0);
		latestMemorySlice.push_back(0);
		raceAheadMode.push_back(false);
	}
	int specialInstMax = SescConf->getRecordMax("TMSpecialInstr","InstrAddr");
	for(int i = SescConf->getRecordMin("TMSpecialInstr","InstrAddr"); i <= specialInstMax; i++)
	{
		if(SescConf->checkInt("TMSpecialInstr","InstrAddr",i) && SescConf->checkInt("TMSpecialInstr","Opcode",i))
		{
			int instrAddr = SescConf->getInt("TMSpecialInstr","InstrAddr",i);
			int opcode = SescConf->getInt("TMSpecialInstr","Opcode",i);
			int arg1 = (SescConf->checkInt("TMSpecialInstr","Arg1",i))?(SescConf->getInt("TMSpecialInstr","Arg1",i)):0;
			int arg2 = (SescConf->checkInt("TMSpecialInstr","Arg2",i))?(SescConf->getInt("TMSpecialInstr","Arg2",i)):0;
			int arg3 = (SescConf->checkInt("TMSpecialInstr","Arg3",i))?(SescConf->getInt("TMSpecialInstr","Arg3",i)):0;
			int arg4 = (SescConf->checkInt("TMSpecialInstr","Arg4",i))?(SescConf->getInt("TMSpecialInstr","Arg4",i)):0;
			int arg5 = (SescConf->checkInt("TMSpecialInstr","Arg5",i))?(SescConf->getInt("TMSpecialInstr","Arg5",i)):0;
			extraSpecialInstructions[instrAddr].push_back(SpecOperation(opcode,arg1,arg2,arg3,arg4,arg5));
		}
	}
	memory = new TMMemory();

	pendingSpecOpSet.resize(procCount);
	privateMemoryQueue.resize(procCount);

	collisionDetection->Initialize(procs,procCount,memory);
	collisionManager->Initialize(procs,procCount,memory);
	versioning->Initialize(procs,procCount,memory);
#endif
}
void TMInterface::Destroy()
{
#ifdef TRANSACTIONAL_MEMORY
	collisionDetection->Destroy();
	collisionManager->Destroy();
	versioning->Destroy();
#ifdef TRANSACTIONAL_COMPOSITION_TRACKING
	if(compositionRecordSet)
	{
		delete compositionRecordSet;
	}
#endif
	for(int i = 0; i < procCount; i++)
	{
		delete procs[i];
	}
	delete [] procs;
	delete memory;
	delete collisionDetection;
	delete collisionManager;
	delete versioning;
#endif
}
void TMInterface::BeginRabbit()
{
#ifdef TRANSACTIONAL_MEMORY
	goingRabbit = true;
	collisionDetection->BeginRabbit();
	collisionManager->BeginRabbit();
	versioning->BeginRabbit();
#endif
}
void TMInterface::EndRabbit()
{
#ifdef TRANSACTIONAL_MEMORY
	goingRabbit = false;
	collisionDetection->EndRabbit();
	collisionManager->EndRabbit();
	versioning->EndRabbit();
#endif
}
void TMInterface::MarkRegisterChange(int pid, void* oldAddr, size_t size)
{
#ifdef TRANSACTIONAL_MEMORY
#endif
}
void TMInterface::MarkLoad(icode* picode, int pid, uint32_t addr, size_t size)
{
#ifdef TRANSACTIONAL_MEMORY
	if(extraSpecialInstructions.find(picode->addr) != extraSpecialInstructions.end())
	{
		for(size_t i = 0; i < extraSpecialInstructions[picode->addr].size(); i++)
		{
			const SpecOperation& s = extraSpecialInstructions[picode->addr][i];
			MarkSpecOp(picode, pid, s.opCode, s.arg1, s.arg2, s.arg3, s.arg4, s.arg5);
		}
	}
	lastActionPid = pid;
#ifdef TRANSACTIONAL_ONLY
	if(lastTransactionSeen[pid] == 0)
	{
		pendingSpecOpSet[pid].clear();
		return;
	}
#endif
	actionToProcess = true;
	lastInstructionPtr = picode;
	lastInstIsAccess = true;
	lastInstIsRead = true;
	accessAddr = addr;
	accessSize = size;
#endif
}
void TMInterface::MarkStore(icode* picode, int pid, uint32_t addr, size_t size)
{
#ifdef TRANSACTIONAL_MEMORY
	if(extraSpecialInstructions.find(picode->addr) != extraSpecialInstructions.end())
	{
		for(size_t i = 0; i < extraSpecialInstructions[picode->addr].size(); i++)
		{
			const SpecOperation& s = extraSpecialInstructions[picode->addr][i];
			MarkSpecOp(picode, pid, s.opCode, s.arg1, s.arg2, s.arg3, s.arg4, s.arg5);
		}
	}
	lastActionPid = pid;
#ifdef TRANSACTIONAL_ONLY
	if(lastTransactionSeen[pid] == 0)
	{
		pendingSpecOpSet[pid].clear();
		return;
	}
#endif
	I(size <= 8);
	I(size > 0);
	actionToProcess = true;
	lastInstructionPtr = picode;
	lastInstIsAccess = true;
	lastInstIsRead = false;
	accessAddr = addr;
	accessSize = size;
#endif
}
void TMInterface::MarkSpecOp(icode* picode, int pid, uint32_t opCode, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5)
{
#ifdef TRANSACTIONAL_MEMORY
	pendingSpecOpSet[pid].push_back(SpecOperation(opCode,arg1,arg2,arg3,arg4,arg5));
#endif
}
void TMInterface::MarkTransOp(icode* picode, int pid, uint32_t opCode, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5)
{
#ifdef TRANSACTIONAL_MEMORY
	if(extraSpecialInstructions.find(picode->addr) != extraSpecialInstructions.end())
	{
		for(size_t i = 0; i < extraSpecialInstructions[picode->addr].size(); i++)
		{
			const SpecOperation& s = extraSpecialInstructions[picode->addr][i];
			MarkSpecOp(picode, pid, s.opCode, s.arg1, s.arg2, s.arg3, s.arg4, s.arg5);
		}
	}
	lastActionPid = pid;
	actionToProcess = true;
	lastInstructionPtr = picode;
	lastInstIsAccess = false;
	transOpCode = opCode;
	lastTransArg1 = arg1;
	lastTransArg2 = arg2;
	lastTransArg3 = arg3;
	lastTransArg4 = arg4;
	lastTransArg5 = arg5;
#endif
}
void TMInterface::MarkBarrierExit()
{
#ifdef TRANSACTIONAL_MEMORY
#ifdef TRANSACTIONAL_COMPOSITION_TRACKING
	compositionRecordSet->RecordBarrierExit(procs[0]->GetProcTime());
#endif
#endif
}

uint32_t TMInterface::DoAllocate(int pid, size_t size)
{
	VAddr addr = ThreadContext::getContext(pid)->getHeapManager()->allocate(size);
#ifdef TRANSACTIONAL_COMPOSITION_TRACKING
	if(compositionRecordingEnabled && currentTransID[pid])
	{
		compositionRecordSet->RecordAllocate(currentTransID[pid],addr,(uint32_t)size);
	}
#endif
	return addr;
}
void TMInterface::DoFree(int pid, uint32_t addr)
{
	uint32_t size = (uint32_t)ThreadContext::getContext(pid)->getHeapManager()->deallocate(addr);
#ifdef TRANSACTIONAL_COMPOSITION_TRACKING
	if(compositionRecordingEnabled && currentTransID[pid])
	{
		compositionRecordSet->RecordAllocate(currentTransID[pid],addr,size);
	}
#endif
}
void TMInterface::ProcessAction(uint64_t instrID)
{
#ifdef TRANSACTIONAL_MEMORY
	if(actionToProcess)
	{
		if(lastInstIsAccess)
		{
			I(accessSet.find(instrID) == accessSet.end());
			uint32_t dependant;
			if(transRecordSet.find(lastTransactionSeen[lastActionPid]) != transRecordSet.end() && lastTransactionSeen[lastActionPid])
			{
				dependant = (transRecordSet[lastTransactionSeen[lastActionPid]].resolved)?currentTransID[lastActionPid]:lastTransactionSeen[lastActionPid];
			}
			else
			{
				lastTransactionSeen[lastActionPid] = 0;
				dependant = 0;
			}
			StoredAccess* a = StoredAccess::Create(lastActionPid,currentTransID[lastActionPid], dependant, accessAddr, accessSize, lastInstIsRead, instrID);
			for(size_t i = 0; i < pendingSpecOpSet[lastActionPid].size(); i++)
			{
				a->AddSpecOp(pendingSpecOpSet[lastActionPid][i]);
			}
			pendingSpecOpSet[lastActionPid].clear();
			accessSet.insert(std::pair<uint64_t,StoredAccess*>(instrID,a));
			if(currentTransID[lastActionPid])
			{
				I(a->transactionEntry);
				transRecordSet[a->transactionEntry].accessCounter++;
			}
			if(lastTransactionSeen[lastActionPid])
			{
				I(a->dependantTransaction);
				transRecordSet[a->dependantTransaction].dependantAccessCounter++;
			}
			privateMemorySet[latestMemorySlice[lastActionPid]]->CheckMemory(ThreadContext::getContext(0)->virt2real(accessAddr));
		}
		else
		{
			uint32_t currentTrans = currentTransID[lastActionPid];
			I(transOpCode == TMOpCodes::TransBegin || transRecordSet.find(currentTransID[lastActionPid]) != transRecordSet.end());
			uint32_t newTrans = (transOpCode == TMOpCodes::TransBegin)?(globalTransactionID++):(transRecordSet[currentTransID[lastActionPid]].parentTransactionID);
			TransOperation* to = TransOperation::Create(lastActionPid,currentTrans,newTrans,transOpCode,lastTransArg1,lastTransArg2,lastTransArg3,lastTransArg4,lastTransArg5,instrID);
			for(size_t i = 0; i < pendingSpecOpSet[lastActionPid].size(); i++)
			{
				to->AddSpecOp(pendingSpecOpSet[lastActionPid][i]);
			}
			pendingSpecOpSet[lastActionPid].clear();
			if(transOpCode == TMOpCodes::TransBegin)
			{
				I(newTrans);
#ifdef TRANSACTIONAL_COMPOSITION_TRACKING
				if(compositionRecordingEnabled)
				{
					compositionRecordSet->InitiateTransaction(lastActionPid,newTrans,currentTrans,procs[lastActionPid]->GetPCAddr());
				}
#endif
				transRecordSet[newTrans] = TransRecord(lastInstructionPtr,to,lastActionPid,newTrans,currentTrans,lastTransactionSeen[lastActionPid]);
				procs[lastActionPid]->PrepareCheckpoint(instrID,lastInstructionPtr,currentTrans);
				procs[lastActionPid]->PrepareCheckpointReturn(instrID);
				procs[lastActionPid]->TakeCheckpoint(checkpointSet[newTrans]);
				procs[lastActionPid]->CancelCheckpointReturn();
				procs[lastActionPid]->ExpireCheckpoint(instrID);
				I(privateMemorySet.find(newTrans) == privateMemorySet.end());
				if(latestMemorySlice[lastActionPid])
				{
					I(privateMemorySet.find(latestMemorySlice[lastActionPid]) != privateMemorySet.end());
				}
				privateMemorySet[newTrans] = new PrivateMemory(lastActionPid,(latestMemorySlice[lastActionPid])?(privateMemorySet[latestMemorySlice[lastActionPid]]):NULL,newTrans);
				I(privateMemorySet.find(newTrans) != privateMemorySet.end() && privateMemorySet[newTrans]);
				if(latestMemorySlice[lastActionPid])
				{
					I(privateMemorySet.find(latestMemorySlice[lastActionPid]) != privateMemorySet.end());
					privateMemorySet[latestMemorySlice[lastActionPid]]->next = privateMemorySet[newTrans];
				}
				latestMemorySlice[lastActionPid] = newTrans;
				lastTransactionSeen[lastActionPid] = newTrans;
				if(currentTrans)
				{
					I(transRecordSet.find(currentTrans) != transRecordSet.end());
					transRecordSet[currentTrans].accessCounter++;
				}
				else
				{
					privateMemoryQueue[lastActionPid].push(newTrans);
				}
			}
			else
			{
				I(currentTransID[lastActionPid]);
				I(transRecordSet.find(currentTrans) != transRecordSet.end());
				I(transRecordSet.find(currentTrans)->second.transEndOp == NULL);
				lastTransactionSeen[lastActionPid] = currentTrans;
				transRecordSet[currentTrans].transEndOp = to;
				if(MayEndTransaction(currentTrans))
				{
					if(transRecordSet[currentTrans].transEndOp->opCode == TMOpCodes::TransEnd)
					{
						EndTransaction(currentTrans);
					}
					else
					{
						I(transRecordSet[currentTrans].transEndOp->opCode == TMOpCodes::TransAbort);
						AbortTransaction(currentTrans);
					}
				}
			}
			currentTransID[lastActionPid] = newTrans;
		}
		actionToProcess = false;
		if((goingRabbit || raceAheadMode[lastActionPid]) && !lastInstIsAccess)
		{
			if(transOpCode == TMOpCodes::TransBegin)
			{
				I(currentTransID[lastActionPid]);
				BeginTransaction(currentTransID[lastActionPid]);
			}
		}
		else if((goingRabbit || raceAheadMode[lastActionPid]) && lastInstIsAccess)
		{
			ObserveAccess(instrID);
		}
	}
#endif
}
void TMInterface::DumpStats(std::ostream& out)
{
#ifdef TRANSACTIONAL_MEMORY
	for(size_t i = 0; i < abortedBeforeSeenTransactions.size(); i++)
	{
		out << "int AbsTrans " << abortedBeforeSeenTransactions[i] << " #" << std::endl;
	}
	for(size_t i = 0; i < correctionAbortedTransactions.size(); i++)
	{
		out << "int CorrectionAbortedTrans " << correctionAbortedTransactions[i] << " #" << std::endl;
	}
	for(size_t i = 0; i < correctedTransactions.size(); i++)
	{
		out << "int CorrectedTrans " << correctedTransactions[i] << " #" << std::endl;
	}
	for(size_t i = 0; i < pidCompletionPattern.size(); i++)
	{
		out << "int PidCompletionSlot " << i << " int Pid " << pidCompletionPattern[i] << " #" << std::endl;
	}
	collisionDetection->DumpStats(out);
	collisionManager->DumpStats(out);
	versioning->DumpStats(out);
#endif
}
uintptr_t TMInterface::RedirectAddr(int pid, bool isRead, uintptr_t addr, size_t size)
{
#ifdef TRANSACTIONAL_MEMORY
	I((int)addr > Private_start && (int)addr < Private_end);
#ifdef TRANSACTIONAL_COMPOSITION_TRACKING
	if(compositionRecordingEnabled && currentTransID[pid])
	{
		compositionRecordSet->RecordAccess(currentTransID[pid],procs[pid]->GetPCAddr(),ThreadContext::getMainThreadContext()->real2virt(addr),isRead);
	}
#endif
	if(latestMemorySlice[pid])
	{
		I(privateMemorySet.find(latestMemorySlice[pid]) != privateMemorySet.end());
		privateMemorySet[latestMemorySlice[pid]]->RegisterOldValue(addr,size);
		uintptr_t ret = privateMemorySet[latestMemorySlice[pid]]->TranslateAddress(addr,size,isRead);
		return ret;
	}
	else
	{
		if(!isRead)
		{
			for(HASH_MAP<uint32_t,PrivateMemory*>::iterator it = privateMemorySet.begin(); it != privateMemorySet.end(); it++)
			{
				if(it->second->KeepingOldValuesOf(pid))
				{
					it->second->RegisterOldValue(addr,size);
				}
			}
		}
		return addr;
	}
#else
	return addr;
#endif
}
void TMInterface::EraseMemInstr(uint64_t stamp)
{
#ifdef TRANSACTIONAL_MEMORY
#ifdef TRANSACTIONAL_ONLY
	if(accessSet.find(stamp) == accessSet.end())
	{
		return;
	}
#endif
	I(accessSet.find(stamp) != accessSet.end());
	if(!(accessSet[stamp]->transactionEntry == 0) && !IsDeadAccess(stamp))
	{
		I(accessSet[stamp]->transactionEntry);
		I(transRecordSet.find(accessSet[stamp]->transactionEntry) != transRecordSet.end());
		I(transRecordSet[accessSet[stamp]->transactionEntry].accessCounter > 0);
//		if(!transRecordSet[accessSet[stamp]->transactionEntry].progressObserved)
//		{
//			BeginTransaction(accessSet[stamp]->transactionEntry,true);
//		}
		uint32_t transEntry = accessSet[stamp]->transactionEntry;
		RetireAccess(stamp);
		DestroyAccess(stamp);
		if(MayEndTransaction(transEntry))
		{
			EndTransaction(transEntry,true);
		}
	}
	else
	{
		RetireAccess(stamp);
		DestroyAccess(stamp);
	}
#endif
}
bool TMInterface::MayForward(uint64_t load, uint64_t store)
{
#ifdef TRANSACTIONAL_MEMORY
	if(accessSet.find(load) == accessSet.end() || accessSet.find(store) == accessSet.end())
	{
		return true;
	}
	if(accessSet[load]->transactionEntry == accessSet[store]->transactionEntry)
	{
		return true;
	}
	return false;
#else
	return true;
#endif
}
bool TMInterface::MayRaceAhead(int pid)
{
	I(raceAheadMode[pid]);
	runningAhead = !raceAheadStarted[pid] || !raceAheadFinished[pid] || residualInstructionCount[pid] > 0;
	return runningAhead;
}
void TMInterface::MarkInstructionExecution(int pid)
{
	if(raceAheadMode[pid] && raceAheadStarted[pid])
	{
		if(currentTransID[pid] == 0)
		{
			residualInstructionCount[pid]--;
			raceAheadFinished[pid] = true;
		}
		else if(raceAheadFinished[pid])
		{
			residualInstructionCount[pid] = 0;
		}
	}
	else if(currentTransID[pid] == 0)
	{
		residualInstructionCount[pid]++;
	}
	else
	{
		residualInstructionCount[pid] = 0;
	}
}
bool TMInterface::IsTransacting(int pid)
{
	return !(privateMemoryQueue[pid].empty());
}
