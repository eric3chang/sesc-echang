#ifndef TM_MEMORY_H
#define TM_MEMORY_H

#include "Addressing.h"
#include "HeapManager.h"
#ifdef TRANSACTIONAL_MEMORY

class TMMemory
{
	HeapManager* heap;
	RAddr virtOffset;
public:
	TMMemory();

	VAddr Allocate(size_t size);
	void Free(VAddr addr);

	void Read(VAddr addr, size_t size, void* buffer);
	void Write(VAddr addr, size_t size, void* buffer);

	size_t GetMemorySize();
	VAddr HeapStart();
	VAddr HeapEnd();

	void* GetRealAddress(VAddr v);
};
#endif

#endif
