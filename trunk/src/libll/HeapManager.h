#if !(defined HeapManager_h)
#define HeapManager_h

#include <set>
#include "Addressing.h"
#include "Snippets.h"
#include "nanassert.h"

class HeapManager {
private:
  // Reference counter for garbage collection
  size_t refCount;
  // Bottom and top of the heap address range
  VAddr heapAddrLb;
  VAddr heapAddrUb;
  // Used to determine minimum required heap size
  VAddr usedAddrLb;
  VAddr usedAddrUb;

  // Minimum block size. Everything is aligned to this size
  enum {MinBlockSize=64, MinBlockMask=MinBlockSize-1};
  size_t roundUp(size_t size){
    return (size+MinBlockMask)&(~MinBlockMask);
  }
  HeapManager(VAddr base, size_t size);
  ~HeapManager(void);
public:
  struct BlockInfo {
    VAddr addr;
    size_t  size;
    BlockInfo(VAddr addr, size_t size)
      : addr(addr), size(size){
    }
    struct lessBySize {
      bool operator()(const BlockInfo &x, const BlockInfo &y) const{
        return (x.size<y.size)||((x.size==y.size)&&(x.addr<y.addr));
      }
    };
    struct lessByAddr {
      bool operator()(const BlockInfo &x, const BlockInfo &y) const{
        return x.addr<y.addr;
      }
    };
  };
  typedef std::set<BlockInfo,BlockInfo::lessByAddr> BlocksByAddr;
  typedef std::set<BlockInfo,BlockInfo::lessBySize> BlocksBySize;
  BlocksByAddr busyByAddr;
  BlocksByAddr freeByAddr;
  BlocksBySize freeBySize;
  static HeapManager *create(VAddr base, size_t size){
    return new HeapManager(base,size);
  }
  void addReference(void) {
    refCount++;
  }
  void delReference(void) {
    I(refCount>0);
    refCount--;
    if(!refCount){
      delete this;
    }
  }

  VAddr allocate(size_t size);
  VAddr allocate(VAddr addr, size_t size);
  size_t deallocate(VAddr addr);
  size_t getAllocationSize(VAddr addr);

  bool isHeapAddr(VAddr addr) const{
    return (addr>=heapAddrLb)&&(addr<heapAddrUb);
  }
  VAddr getHeapAddrLb(void) const{
    return heapAddrLb;
  }
  VAddr getHeapAddrUb(void) const{
    return heapAddrUb;
  }

  void ForceBlockPopulation(VAddr start, size_t size);
  BlocksByAddr::iterator GetAllocationIterator();
  unsigned int GetAllocationCount();
  BlocksByAddr::iterator GetAllocationDetails(BlocksByAddr::iterator allocIt, VAddr& address, size_t& size);
};

#endif
