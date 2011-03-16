#include "TMMemory.h"
#include "ThreadContext.h"
#include "globals.h"
#ifdef TRANSACTIONAL_MEMORY

TMMemory::TMMemory()
{
	ThreadContext* tc = ThreadContext::getContext(0);
	virtOffset = tc->getVirt2RealOffset();
	heap = tc->getHeapManager();
}
VAddr TMMemory::Allocate(size_t size)
{
	return heap->allocate(size);
}
void TMMemory::Free(VAddr addr)
{
	heap->deallocate(addr);
}
void TMMemory::Read(VAddr addr, size_t size, void* buffer)
{
	memcpy(buffer,GetRealAddress(addr),size);
}
void TMMemory::Write(VAddr addr, size_t size, void* buffer)
{
	memcpy(GetRealAddress(addr),buffer,size);
}
size_t TMMemory::GetMemorySize()
{
	return Heap_size;
}
VAddr TMMemory::HeapStart()
{
	return Heap_start;
}
VAddr TMMemory::HeapEnd()
{
	return Heap_end;
}
void* TMMemory::GetRealAddress(VAddr v)
{
	return (void*)(v + virtOffset);
}
#endif
