#ifndef TM_INTERFACE_H
#define TM_INTERFACE_H

#include "estl.h"
#include "nanassert.h"
#include <stdint.h>
#include <vector>
#include <queue>
#include <stack>
#include <iostream>
#include <string.h>
#ifndef _MSC_VER
	#include "hash_fun_extra.h"
#endif

class ITMCollisionDetection;
class ITMCollisionManager;
class ITMVersioning;
class TMProcessor;
class TMMemory;
class TMProcessorCheckpoint;
class icode;

#ifndef _MSC_VER
//	typedef uintptr_t hash_uintptr_t;		// commented this out 2010-05-28
//	typedef uintptr_t compare_uintptr_t;	// commented this out 2010-05-28
#endif

class TMInterface
{
#ifdef TRANSACTIONAL_MEMORY
#ifdef TRANSACTIONAL_COMPOSITION_TRACKING
	class TransCompositionRecord
	{
	public:
		class Record
		{
		public:
			static const int8_t isRead = 1;
			static const int8_t isWrite = 2;
			static const int8_t isAlloc = 3;
			static const int8_t isFree = 4;
			static const int8_t isSubTrans = 5;
			class RecordEntry
			{
			public:
				int8_t action;
				uint32_t arg1;
				uint32_t arg2;
				RecordEntry(int8_t action, uint32_t arg1, uint32_t arg2)
				{
					this->action = action;
					this->arg1 = arg1;
					this->arg2 = arg2;
				}
			};
			int pid;
			bool committed;
			uint32_t parentTransID;
			uint32_t instrID;
			uint32_t transID;
			uint64_t timeStart;
			uint64_t timeEnd;
			std::vector<RecordEntry> entrySet;
			void RecordElement(int8_t action, uint32_t arg1, uint32_t arg2)
			{
				entrySet.push_back(RecordEntry(action,arg1,arg2));
			}
			void RecordCompact(std::ostream& os)
			{
				os.write((char*)(&pid),sizeof(pid));
				os.write((char*)(&transID),sizeof(transID));
				os.write((char*)(&parentTransID),sizeof(parentTransID));
				os.write((char*)(&instrID),sizeof(instrID));
				os.write((char*)(&timeStart),sizeof(timeStart));
				os.write((char*)(&timeEnd),sizeof(timeEnd));
				char commitState = (this->committed)?1:0;
				os.write((char*)(&commitState),sizeof(commitState));
				uint32_t entryCount = (uint32_t)entrySet.size();
				os.write((char*)(&entryCount),sizeof(entryCount));
				for(uint32_t i = 0; i < entryCount; i++)
				{
					os.write((char*)&(entrySet[i].action),sizeof(entrySet[i].action));
					os.write((char*)&(entrySet[i].arg1),sizeof(entrySet[i].arg1));
					os.write((char*)&(entrySet[i].arg2),sizeof(entrySet[i].arg2));
				}
			}
			void RecordVerbose(std::ostream& os)
			{
				I(0);
			}
		};
		bool compact;
		std::ostream* outStream;
		HASH_MAP<uint32_t,Record> entrySet;

		TransCompositionRecord(std::ostream* os, bool compact)
		{
			this->outStream = os;
			this->compact = compact;
		}
		~TransCompositionRecord()
		{
			delete outStream;
		}
		void InitiateTransaction(int pid, uint32_t transID, uint32_t parent, uint32_t instrID)
		{
			I(entrySet.find(transID) == entrySet.end());
			if(parent)
			{
				I(entrySet.find(parent) != entrySet.end());
				entrySet[parent].RecordElement(Record::isSubTrans,transID,0);
			}
			Record r;
			r.pid = pid;
			r.transID = transID;
			r.instrID = instrID;
			r.parentTransID = parent;
			r.timeStart = r.timeEnd = 0;
			r.committed = true;
			entrySet[transID] = r;
		}
		void FinalizeTransaction(uint32_t transID)
		{
			I(entrySet.find(transID) != entrySet.end());
			if(compact)
			{
				entrySet[transID].RecordCompact(*outStream);
			}
			else
			{
				entrySet[transID].RecordVerbose(*outStream);
			}
			entrySet.erase(transID);
		}
		void RecordRead(uint32_t transID, uint32_t instrAddr, uint32_t dataAddr)
		{
			I(entrySet.find(transID) != entrySet.end());
			entrySet[transID].RecordElement(Record::isRead,instrAddr,dataAddr);
		}
		void RecordWrite(uint32_t transID, uint32_t instrAddr, uint32_t dataAddr)
		{
			I(entrySet.find(transID) != entrySet.end());
			entrySet[transID].RecordElement(Record::isWrite,instrAddr,dataAddr);
		}
		void RecordAccess(uint32_t transID, uint32_t instrAddr, uint32_t dataAddr, bool isRead)
		{
			if(isRead)
			{
				RecordRead(transID,instrAddr,dataAddr);
			}
			else
			{
				RecordWrite(transID,instrAddr,dataAddr);
			}
		}
		void RecordAllocate(uint32_t transID, uint32_t addr, uint32_t size)
		{
			I(entrySet.find(transID) != entrySet.end());
			entrySet[transID].RecordElement(Record::isAlloc,addr,size);
		}
		void RecordFree(uint32_t transID, uint32_t addr, uint32_t size)
		{
			I(entrySet.find(transID) != entrySet.end());
			entrySet[transID].RecordElement(Record::isFree,addr,size);
		}
		void RecordTransactionBegin(uint32_t transID, uint64_t timeStamp)
		{
			I(entrySet.find(transID) != entrySet.end());
			entrySet[transID].timeStart = timeStamp;
		}
		void RecordTransactionCommit(uint32_t transID, uint64_t timeStamp)
		{
			I(entrySet.find(transID) != entrySet.end());
			entrySet[transID].timeEnd = timeStamp;
		}
		void RecordTransactionAbort(uint32_t transID, uint64_t timeStamp)
		{
			I(entrySet.find(transID) != entrySet.end());
			entrySet[transID].timeStart = timeStamp;
			entrySet[transID].committed = false;
		}
		void RecordBarrierExit(uint64_t time)
		{
			//const uint64_t limit32Bit = 4294967296ll;	// unused variable
			int pid = -1;
			outStream->write((char*)&pid,sizeof(pid));
			outStream->write((char*)&time,sizeof(time));
		}
	};
	static bool compositionRecordingEnabled;
	static TransCompositionRecord* compositionRecordSet;
#endif
	class SpecOperation
	{
	public:
		uint32_t opCode;
		uint32_t arg1;
		uint32_t arg2;
		uint32_t arg3;
		uint32_t arg4;
		uint32_t arg5;
		SpecOperation(uint32_t opCode, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5)
		{
			this->opCode = opCode;
			this->arg1 = arg1;
			this->arg2 = arg2;
			this->arg3 = arg3;
			this->arg4 = arg4;
			this->arg5 = arg5;
		}
	};
	class TransOperation
	{
	public:
		uint32_t opCode;
		uint32_t arg1;
		uint32_t arg2;
		uint32_t arg3;
		uint32_t arg4;
		uint32_t arg5;
		std::vector<SpecOperation> specOpSet;
		uint32_t oldTransactionEntry;
		uint32_t newTransactionEntry;
		int pid;
		uint64_t stamp;
	private:
		static std::stack<TransOperation*> pool;
		void FillFields(int pid, uint32_t oldEntry, uint32_t newEntry, uint32_t opCode, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5, uint64_t stamp)
		{
			this->opCode = opCode;
			this->arg1 = arg1;
			this->arg2 = arg2;
			this->arg3 = arg3;
			this->arg4 = arg4;
			this->arg5 = arg5;
			this->pid = pid;
			oldTransactionEntry = oldEntry;
			newTransactionEntry = newEntry;
			this->stamp = stamp;
		}
	public:
		static TransOperation* Create(int pid, uint32_t oldEntry, uint32_t newEntry, uint32_t opCode, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5, uint64_t stamp)
		{
			if(pool.empty())
			{
				pool.push(new TransOperation());
			}
			TransOperation* ret = pool.top();
			pool.pop();
			ret->FillFields(pid,oldEntry,newEntry,opCode,arg1,arg2,arg3,arg4,arg5,stamp);
			return ret;
		}
		void Dispose()
		{
			specOpSet.clear();
			pool.push(this);
		}
		void AddSpecOp(const SpecOperation& so)
		{
			specOpSet.push_back(so);
		}
	};
	class StoredAccess
	{
	public:
		uint32_t addr;
		size_t accessSize;
		bool isRead;
		int pid;
		uint64_t stamp;
		std::vector<SpecOperation> specOpSet;
		uint32_t transactionEntry;
		uint32_t dependantTransaction;
		bool retired;
	private:
		StoredAccess(){}
		static std::stack<StoredAccess*> pool;
		void FillFields(int pid, uint32_t transactionEntry, uint32_t dependantTransaction, uint32_t addr, size_t accessSize, bool isRead, uint64_t stamp)
		{
			this->addr = addr;
			this->pid = pid;
			this->accessSize = accessSize;
			this->isRead = isRead;
			this->stamp = stamp;
			this->transactionEntry = transactionEntry;
			this->dependantTransaction = dependantTransaction;
			retired = false;
		}
	public:
		void AddSpecOp(const SpecOperation& so)
		{
			specOpSet.push_back(so);
		}
		void Dispose()
		{
			specOpSet.clear();
			pool.push(this);
		}
		static StoredAccess* Create(int pid, uint32_t transactionEntry, uint32_t dependantTransaction, uint32_t addr, size_t accessSize, bool isRead, uint64_t stamp)
		{
			if(pool.empty())
			{
				pool.push(new StoredAccess());
			}
			StoredAccess* ret = pool.top();
			pool.pop();
			ret->FillFields(pid, transactionEntry, dependantTransaction, addr, accessSize, isRead, stamp);
			return ret;
		}
	};
	class PrivateMemory
	{
	public:
		static const size_t BlockSize = 8; // must be (x % 8 = 0) && (x > 8)  largest memory chunk is a double, 8 bytes in one access
		class PrivateMemoryBlock
		{
		public:
			uint8_t newData[BlockSize];
			uint8_t oldData[BlockSize];
			bool valid[BlockSize];
			bool isRead[BlockSize];
			bool isWrite[BlockSize];
			bool relevant[BlockSize];
			PrivateMemoryBlock()
			{
				for(size_t i = 0; i < BlockSize; i++)
				{
					valid[i] = false;
					relevant[i] = false;
					isRead[i] = isWrite[i] = false;
					newData[i] = oldData[i] = 0xcd;
				}
			}
			void CheckMemory()
			{
				for(size_t i = 0; i < BlockSize; i++)
				{
					if(valid[i])
					{
						I(relevant[i] == (isRead[i] || isWrite[i]));
						if(!isWrite[i])
						{
							I(oldData[i] == newData[i]);
						}
					}
					else
					{
						I(!relevant[i]);
						I(!isRead[i]);
						I(!isWrite[i]);
						I(newData[i] == oldData[i]);
						I(newData[i] == 0xcd);
					}
				}
			}
		};
		typedef HASH_MAP<uintptr_t,PrivateMemoryBlock
#ifndef _MSC_VER
//			,hash_uintptr_t, compare_uintptr_t	// commented this out 2010-05-28
#endif
			> AddrHashMap;
		AddrHashMap memMap;
		HASH_SET<int> pidReliance;
		PrivateMemory* previous;
		PrivateMemory* next;
		uint32_t transID;
		bool committed;
		int pid;
		PrivateMemory(){}
		PrivateMemory(int pid, PrivateMemory* parent, uint32_t transID)
		{
			this->pid = pid;
			previous = parent;
			if(parent)
			{
				I(!parent->next);
				parent->next = this;
				memMap = parent->memMap;
				pidReliance = parent->pidReliance;
			}
			next = NULL;
			committed = false;
			this->transID = transID;
		}
		uintptr_t TranslateAddress(uintptr_t addr, size_t size, bool isRead)
		{
			uintptr_t addrTag = addr / (uintptr_t)BlockSize;
			uintptr_t blockAddr = (addrTag * (uintptr_t)BlockSize);
			uintptr_t offset = addr - blockAddr;
			I(offset + size <= 8);
			I(memMap.find(addrTag) != memMap.end());
			PrivateMemoryBlock& block = memMap[addrTag];
			for(size_t i = (size_t)offset; i <(size_t)offset + size; i++)
			{
				I(block.valid[i]);
				block.relevant[i] = true;
				if(isRead)
				{
					block.isRead[i] = true;
				}
				else
				{
					block.isWrite[i] = true;
				}
			}
			return (uintptr_t) (&block.newData[offset]);
		}
		bool TestIntegrity()
		{
			I(previous == NULL);
			for(AddrHashMap::const_iterator it = memMap.begin(); it != memMap.end(); it++)
			{
				for(size_t i = 0; i < BlockSize; i++)
				{
					if(it->second.valid[i])
					{
						if(it->second.relevant[i])
						{
							I(it->second.isRead[i] || it->second.isWrite[i]);
							if(it->second.oldData[i] != *((uint8_t*)(it->first * BlockSize + i)))
							{
								return false;
							}
						}
						else
						{
							I(!it->second.isRead[i] && !it->second.isWrite[i]);
						}
					}
				}
			}
			return true;
		}
		void Extract()
		{
			if(previous)
			{
				previous->next = next;
			}
			if(next)
			{
				next->previous = previous;
			}
			next = previous = NULL;
		}
		void MergeDown()
		{
			I(previous);
			I(previous->next == this);
			previous->memMap = memMap;
			previous->pidReliance = pidReliance;
			Extract();
		}
		bool RelevantOverlap(PrivateMemory* pm)
		{
			I(pm->pid != pid);
			pidReliance.insert(pm->pid);
			return true;
			if(pidReliance.find(pm->pid) != pidReliance.end())
			{
				return true;
			}
			for(AddrHashMap::iterator it = pm->memMap.begin(); it != pm->memMap.end(); it++)
			{
				if(memMap.find(it->first) == memMap.end())
				{
					continue;
				}
				I(memMap.find(it->first) != memMap.end());
				for(size_t i = 0; i < BlockSize; i++)
				{
					if(it->second.isWrite[i] && (memMap[it->first].isWrite[i] || memMap[it->first].isRead[i]))
					{
						pidReliance.insert(pm->pid);
						return true;
					}
				}
			}
			return false;
		}
		void IncorporateCommitted(PrivateMemory* pm)
		{
			I(pm->pid != pid);
			if(RelevantOverlap(pm))
			{
				for(AddrHashMap::iterator it = pm->memMap.begin(); it != pm->memMap.end(); it++)
				{
					for(size_t i = 0; i < BlockSize; i++)
					{
						if(it->second.valid[i] && it->second.isWrite[i])
						{
							I(it->second.relevant[i]);
							I(*((uint8_t*)(it->first * BlockSize + i)) == it->second.newData[i]);
							if(!memMap[it->first].valid[i])
							{
								memMap[it->first].oldData[i] = memMap[it->first].newData[i] = it->second.oldData[i];
								memMap[it->first].valid[i] = true;
							}
						}
					}
				}
			}
		}
		void ApplyChanges()
		{
			I(previous == NULL);
			for(AddrHashMap::const_iterator it = memMap.begin(); it != memMap.end(); it++)
			{
				for(size_t i = 0; i < BlockSize; i++)
				{
					if(it->second.valid[i] && it->second.isWrite[i])
					{
						I(it->second.relevant[i]);
						I(*((uint8_t*)(it->first * BlockSize + i)) == it->second.oldData[i]);
						*((uint8_t*)(it->first * BlockSize + i)) = it->second.newData[i];
					}
				}
			}
		}
		void RemoveFromUnder(PrivateMemory* pm)
		{
			I(pid == pm->pid);
			for(AddrHashMap::iterator it = memMap.begin(); it != memMap.end(); it++)
			{
				if(pm->memMap.find(it->first) != pm->memMap.end())
				{
					for(size_t i = 0; i < BlockSize; i++)
					{
						if(it->second.valid[i] && it->second.relevant[i])
						{
							I( it->second.oldData[i] == pm->memMap[it->first].oldData[i]);
							I(pm->memMap[it->first].valid[i]);
							I(pm->memMap[it->first].relevant[i]);
							I(*((uint8_t*)(it->first * BlockSize + i)) == it->second.newData[i]);
							pm->memMap[it->first].oldData[i] = it->second.newData[i];
						}
					}
				}
			}
		}
		void RegisterOldValue(uintptr_t addr, size_t size)
		{
			uintptr_t addrTag = addr / (uintptr_t)BlockSize;
			uintptr_t blockAddr = (addrTag * (uintptr_t)BlockSize);
			uintptr_t offset = addr - blockAddr;
			I(offset + size <= 8);
			PrivateMemoryBlock& block = memMap[addrTag];
			I(((size_t)(blockAddr + offset)) == addr);
			for(size_t i = (size_t)offset; i <(size_t)offset + size; i++)
			{
				if(!block.valid[i])
				{
					block.valid[i] = true;
					I(block.oldData[i] == block.newData[i] && block.oldData[i] == 0xcd);
					block.oldData[i] = block.newData[i] = *((uint8_t*)(blockAddr + i));
					I(block.relevant[i] == false);
				}
			}
		}
		void CheckMemory(uintptr_t addr)
		{
			uintptr_t addrTag = addr / (uintptr_t)BlockSize;
			I(memMap.find(addrTag) != memMap.end());
			PrivateMemoryBlock& block = memMap[addrTag];
			block.CheckMemory();
		}
		bool KeepingOldValuesOf(int pid)
		{
			return pidReliance.find(pid) != pidReliance.end();
		}
	};
	class TransRecord
	{
	public:
		TransOperation* transStartOp;
		TransOperation* transEndOp;
		int accessCounter;
		int dependantAccessCounter;
		bool progressObserved;
		uint32_t transactionID;
		uint32_t parentTransactionID;
		uint32_t dependantTransaction;
		icode* transStartCode;
		int pid;
		bool resolved;
		bool committed;

		TransRecord()
		{
			transStartOp = NULL;
			transEndOp = NULL;
		}
		TransRecord(icode* transStartCode, TransOperation* start, int pid, uint32_t id, uint32_t parent, uint32_t dependant)
		{
			this->transStartOp = start;
			this->transEndOp = NULL;
			this->accessCounter = 0;
			this->progressObserved = false;
			this->transactionID = id;
			this->parentTransactionID = parent;
			this->transStartCode = transStartCode;
			this->pid = pid;
			this->resolved = false;
			this->dependantTransaction = dependant;
			this->dependantAccessCounter = 0;
			this->committed = false;
		};
		void Dispose()
		{			
			if(transStartOp)
			{
				transStartOp->Dispose();
				transStartOp = NULL;
			}
			if(transEndOp)
			{
				transEndOp->Dispose();
				transEndOp = NULL;
			}
		}
	};

	template <class T>
	class ULL_HASH_MAP : public HASH_MAP<uint64_t,T
#ifndef _MSC_VER
//			,hash_uintptr_t, compare_uintptr_t	// commented this out 2010-05-28
#endif
		> {};
	typedef HASH_SET<uint64_t
#ifndef _MSC_VER
//			,hash_uintptr_t, compare_uintptr_t	// commented this out 2010-05-28
#endif
		> ULL_HASH_SET;

	static HASH_MAP<int32_t, std::vector<SpecOperation> > extraSpecialInstructions;
	static ULL_HASH_MAP<StoredAccess*> accessSet;
	static HASH_MAP<uint32_t,TransRecord> transRecordSet;
	static std::vector<uint32_t> lastTransactionSeen;
	static HASH_MAP<uint32_t,TMProcessorCheckpoint> checkpointSet;
	static HASH_MAP<uint32_t,PrivateMemory*> privateMemorySet;
	static std::vector<std::queue<uint32_t> > privateMemoryQueue;
	static std::vector<uint32_t> currentTransID;
	static std::vector<int> residualInstructionCount;
	static std::vector<bool> raceAheadStarted;
	static std::vector<bool> raceAheadFinished;
	static std::vector<bool> raceAheadMode;
	static std::vector<std::vector<SpecOperation> > pendingSpecOpSet;
	static std::vector<uint32_t> abortedBeforeSeenTransactions;
	static std::vector<uint32_t> correctionAbortedTransactions;
	static std::vector<uint32_t> correctedTransactions;
	static uint32_t globalTransactionID;
	static std::vector<uint32_t> latestMemorySlice;

	static TMProcessor** procs;
	static int procCount;
	static TMMemory* memory;
	static ITMCollisionDetection* collisionDetection;
	static ITMCollisionManager* collisionManager;
	static ITMVersioning* versioning;

	static bool goingRabbit;
	static bool forceInOrderTrans;

	static int deviceTransfering;
	static uint32_t activeAddr;
	static HASH_MAP<uint32_t,int> inProgressTransfer;

	static bool actionToProcess;
	static bool lastInstIsAccess;
	static bool lastInstIsRead;
	static int lastActionPid;
	static uint32_t accessAddr;
	static size_t accessSize;
	static icode* lastInstructionPtr;
	static uint32_t transOpCode;
	static uint32_t lastTransArg1;
	static uint32_t lastTransArg2;
	static uint32_t lastTransArg3;
	static uint32_t lastTransArg4;
	static uint32_t lastTransArg5;
	static void* lastRegEditLocation;
	static size_t lastRegEditSize;
	static bool deadInstruction;
	static bool runningAhead;

	static uint32_t retryTime;

	static std::queue<uint32_t> pendingAborts;
	static bool handlingEmittedInstr;

	static void HandleEmittedInstructions();
	static uint32_t CalcRetryTime();
	static void CheckCollisions();

	static void CommitPrivateMemory(int pid);
	static void FixPrivateMemory(int pid);
	static void RerunCommittingTransaction(uint32_t transID);

	static void ExecuteAccess(StoredAccess* m);
	static void ExecuteTransactionInst(TransOperation* op, bool forceNoDelay = false);

	static bool BeginTransaction(uint32_t transID, bool force = false);
	static void EndTransaction(uint32_t transID, bool force = false);
	static void AbortTransaction(uint32_t transID);
	static bool MayEndTransaction(uint32_t transID);
	static void RetireAccess(uint64_t stamp);
	static void DestroyAccess(uint64_t stamp);
	static bool IsDeadAccess(uint64_t stamp);
	static void AbortTransactionTree(uint32_t transID);
	static void TryRetireTransaction(uint32_t transID);
#endif
public:
	static void ClockTick();

	static bool HasTransfers(int cpuID);
	static void GetTransferInfo(bool& isRead, bool& isRequired, uint32_t& addr, size_t& size);
	static void AcceptTransfer();
	static void DenyTransfer();
	static void ConfirmTransferCompleted(int cpuID, bool isRead, uint32_t address);

	static void ObserveAccess(uint64_t instrID);
	static uint32_t RetryTime();

	static void Initialize();
	static void Destroy();

	static void BeginRabbit();
	static void EndRabbit();
	//These are to mark the functional execution of certain types of instructions
	static void MarkRegisterChange(int pid, void* oldAddr, size_t size);
	static void MarkLoad(icode* picode, int pid, uint32_t addr, size_t size);
	static void MarkStore(icode* picode, int pid, uint32_t addr, size_t size);
	static void MarkSpecOp(icode* picode, int pid, uint32_t opCode, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5);
	static void MarkTransOp(icode* picode, int pid, uint32_t opCode, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5);
	static void MarkBarrierExit();
	static uint32_t DoAllocate(int pid, size_t size);
	static void DoFree(int pid, uint32_t addr);
	static void ProcessAction(uint64_t instrID);

	static void DumpStats(std::ostream& out);

	static uintptr_t RedirectAddr(int pid, bool isRead, uintptr_t addr, size_t size);
	static void EraseMemInstr(uint64_t stamp);
	static bool IsDeadInstruction()
	{
#ifdef TRANSACTIONAL_MEMORY
		return deadInstruction; 
#else
		return false;
#endif
	}
	static bool MayForward(uint64_t load, uint64_t store);
	static bool MayRaceAhead(int pid);
	static void MarkInstructionExecution(int pid);

	static bool IsTransacting(int pid);
};

#endif
