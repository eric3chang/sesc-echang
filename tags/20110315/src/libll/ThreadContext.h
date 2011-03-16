#ifndef THREADCONTEXT_H
#define THREADCONTEXT_H

#include <stdint.h>
#include <vector>
#include <set>
#include "Snippets.h"
#include "event.h"
#include "OSSim.h"
#include "HeapManager.h"
#include "icode.h"
#include "TMInterface.h"

/* For future support of the exec() call. Information needed about
 * the object file goes here.
 */
typedef struct objfile {
  int32_t rdata_size;
  int32_t db_size;
  char *objname;
} objfile_t, *objfile_ptr;

/* The maximum number of file descriptors per thread. */
#define MAX_FDNUM 20

// Simulated machine has 32-bit registers
typedef int32_t IntRegValue;

enum IntRegName{
  RetValReg   =  2,
  RetValHiReg =  2,
  RetValLoReg =  3,
  IntArg1Reg  =  4,
  IntArg2Reg  =  5,
  IntArg3Reg  =  6,
  IntArg4Reg  =  7,
  JmpPtrReg   = 25,
  GlobPtrReg  = 28,
  StkPtrReg   = 29,
  RetAddrReg  = 31
};

class ThreadContext {
  typedef std::vector<ThreadContext *> ContextVector;
  // Static variables
  static ContextVector pid2context;
  
  static Pid_t baseClonedPid;
  static Pid_t nextClonedPid;
  static ContextVector clonedPool;

  static size_t nThreads;

  static Pid_t baseActualPid;
  static Pid_t nextActualPid;
  static ContextVector actualPool;
  static ThreadContext *mainThreadContext;

  // Memory Mapping

  // Lower and upper bound for valid data addresses
  VAddr dataVAddrLb;
  VAddr dataVAddrUb;
  // Lower and upper bound for stack addresses in all threads
  VAddr allStacksAddrLb;
  VAddr allStacksAddrUb;
  // Lower and upper bound for stack addresses in this thread
  VAddr myStackAddrLb;
  VAddr myStackAddrUb;

  // Real address is simply virtual address plus this offset
  RAddr virtToRealOffset;

  // Local Variables
 public:
  IntRegValue reg[33];	// The extra register is for writes to r0
  int32_t lo;
  int32_t hi;
 private:
  uint32_t fcr0;	// floating point control register 0
  float fp[32];	        // floating point (and double) registers
  uint32_t fcr31;	// floating point control register 31
  icode_ptr picode;	// pointer to the next instruction (the "pc")
  int32_t pid;		// process id
  RAddr raddr;		// real address computed by an event function
  icode_ptr target;	// place to store branch target during delay slot

  HeapManager  *heapManager;  // Heap manager for this thread
  ThreadContext *parent;    // pointer to parent
  ThreadContext *youngest;  // pointer to youngest child
  ThreadContext *sibling;   // pointer to next older sibling
  int32_t *perrno;	            // pointer to the errno variable
  int32_t rerrno;		    // most recent errno for this thread

  char      *fd;	    // file descriptors; =1 means open, =0 means closed

private:

public:
  inline IntRegValue getIntReg(IntRegName name) const {
    return reg[name];
  }
  inline void setIntReg(IntRegName name, IntRegValue value) {
	  TMInterface::MarkRegisterChange(this->getPid(),(void*)&(reg[name]),sizeof(reg[name]));
    reg[name]=value;
  }
  
  inline IntRegValue getIntArg1(void) const{
    return getIntReg(IntArg1Reg);
  }
  inline IntRegValue getIntArg2(void) const{
    return getIntReg(IntArg2Reg);
  }
  inline IntRegValue getIntArg3(void) const{
    return getIntReg(IntArg3Reg);
  }
  inline IntRegValue getIntArg4(void) const{
    return getIntReg(IntArg4Reg);
  }
  inline VAddr getGlobPtr(void) const{
    return getIntReg(GlobPtrReg);
  }
  inline void setGlobPtr(VAddr addr){
    setIntReg(GlobPtrReg,addr);
  }
  inline VAddr getStkPtr(void) const{
    return getIntReg(StkPtrReg);
  }
  inline void setStkPtr(int32_t val){
    I(sizeof(val)==4);
    setIntReg(StkPtrReg,val);
  }
  inline void setRetVal(int32_t val){
    I(sizeof(val)==4);
    setIntReg(RetValReg,val);
  }
  inline void setRetVal64(int64_t val){
    I(sizeof(val)==8);
    uint64_t valLo=val;
    valLo&=0xFFFFFFFFll;
    uint64_t valHi=val;
    valHi>>=32;
    valHi&=0xFFFFFFFFll;
    setIntReg(RetValLoReg,(IntRegValue)valLo);
    setIntReg(RetValHiReg,(IntRegValue)valHi);
  }
  

  inline icode_ptr getPCIcode(void) const{
    I((pid!=-1)||(picode==&invalidIcode));
    return picode;
  }
  inline void setPCIcode(icode_ptr nextIcode){
    I((pid!=-1)||(nextIcode==&invalidIcode));
    picode=nextIcode;
  }
  
  inline icode_ptr getRetIcode(void) const{
    return addr2icode(getIntReg(RetAddrReg));
  }
  // Returns the pid of the context
  Pid_t getPid(void) const { return pid; }

  // Sets the pid of the context
  void setPid(Pid_t newPid){
    pid=newPid;
  }

  int32_t getErrno(void){
    return *perrno;
  }

  void setErrno(int32_t newErrno){
    I(perrno);
    *perrno=newErrno;
  }


  bool isCloned(void) const{ return (pid>=baseClonedPid); }

  void copy(const ThreadContext *src);

#if !(defined MIPS_EMUL)
  uint32_t getFPUControl31() const { return fcr31; }
  void setFPUControl31(uint32_t v) {
    fcr31 = v;
  }

  uint32_t getFPUControl0() const { return fcr0; }
  void setFPUControl0(uint32_t v) {
    fcr0 = v;
  }

  int32_t getREG(icode_ptr pi, int32_t R) { return reg[pi->args[R]];}
  void setREG(icode_ptr pi, int32_t R, int32_t val) { 
	  TMInterface::MarkRegisterChange(this->getPid(),(void*)&(reg[pi->args[R]]),sizeof(reg[pi->args[R]]));
    reg[pi->args[R]] = val;
  }

  void setREGFromMem(icode_ptr pi, int32_t R, int32_t *addr) {
#ifdef LENDIAN
    int32_t val = SWAP_WORD(*addr);
#else
    int32_t val = *addr;
#endif
#ifdef DEBUG_DUMP_INSTR
	if(osSim->isGoingRabbit())
	{
		std::cout << " value:" << val << " addr:" << (unsigned int)(real2virt((VAddr)addr));
	}
#endif
    setREG(pi, R, val);
  }

  float getFP( icode_ptr pi, int32_t R) { return fp[pi->args[R]]; }
  void  setFP( icode_ptr pi, int32_t R, float val) { 
	  TMInterface::MarkRegisterChange(this->getPid(),(void*)&(fp[pi->args[R]]),sizeof(fp[pi->args[R]]));
    fp[pi->args[R]] = val; 
  }
  void  setFPFromMem( icode_ptr pi, int32_t R, float *addr) { 
	  TMInterface::MarkRegisterChange(this->getPid(),(void*)&(fp[pi->args[R]]),sizeof(fp[pi->args[R]]));
    float *pos = &fp[pi->args[R]];
#ifdef LENDIAN
    uint32_t v1;
    v1 = *(uint32_t *)addr;
    v1 = SWAP_WORD(v1);
    *pos = *(float *)&v1;
#else
    *pos = *addr;
#endif
  }

  double getDP( icode_ptr pi, int32_t R) const { 
    return *(double *) &fp[pi->args[R]];
  }

  void   setDP( icode_ptr pi, int32_t R, double val) { 
	TMInterface::MarkRegisterChange(this->getPid(),(void*)&(fp[pi->args[R]]),sizeof(double));
    *((double *)&fp[pi->args[R]]) = val; 
  }


  void   setDPFromMem( icode_ptr pi, int32_t R, double *addr) { 
	TMInterface::MarkRegisterChange(this->getPid(),(void*)&(fp[pi->args[R]]),sizeof(double));
    double *pos = (double *) &fp[pi->args[R]];
#ifdef LENDIAN
    uint64_t v1;
    v1 = *(uint64_t *)(addr);
    v1 = SWAP_LONG(v1);
    *pos = *(double *)&v1;
#else
    *pos = *addr;
#endif // LENDIAN
  }

  int32_t getWFP(icode_ptr pi, int32_t R) { return *(int32_t   *)&fp[pi->args[R]]; }
  void setWFP(icode_ptr pi, int32_t R, int32_t val) { 
	TMInterface::MarkRegisterChange(this->getPid(),(void*)&(fp[pi->args[R]]),sizeof(int32_t));
    *((int32_t   *)&fp[pi->args[R]]) = val; 
  }

  // Methods used by ops.m4 and coproc.m4 (mostly)
  int32_t getREGNUM(int32_t R) const { return reg[R]; }
  void setREGNUM(int32_t R, int32_t val) {
	TMInterface::MarkRegisterChange(this->getPid(),(void*)&(reg[R]),sizeof(reg[R]));
    reg[R] = val;
  }

  // FIXME: SPARC
  double getDPNUM(int32_t R) {return *((double *)&fp[R]); }
  void   setDPNUM(int32_t R, double val) {
	TMInterface::MarkRegisterChange(this->getPid(),(void*)&(fp[R]),sizeof(double));
    *((double *) &fp[R]) = val; 
  }

  // for constant (or unshifted) register indices
  float getFPNUM(int32_t i) const { return fp[i]; }
  void setFPNUM(int32_t i, float v) {
	TMInterface::MarkRegisterChange(this->getPid(),(void*)&(fp[i]),sizeof(fp[i]));
	  fp[i] = v;
  }
  int32_t getWFPNUM(int32_t i) const  { return *((int32_t *)&fp[i]); }

  RAddr getRAddr() const{
    return raddr;
  }
  void setRAddr(RAddr a){
    raddr = a;
  }

  void dump();
  void dumpStack();

  static void staticConstructor(void);

  static size_t size();
#endif // !(defined MIPS_EMUL)

  static ThreadContext *getContext(Pid_t pid);

  static ThreadContext *getMainThreadContext(void);

  static ThreadContext *newActual(void);
  static ThreadContext *newActual(Pid_t pid);
  static ThreadContext *newCloned(void);
  void free(void);
  static uint64_t getMemValue(RAddr p, unsigned dsize); 

  // BEGIN Memory Mapping
  bool isValidDataVAddr(VAddr vaddr) const{
    return (vaddr>=dataVAddrLb)&&(vaddr<dataVAddrUb);
  }

  void setHeapManager(HeapManager *newHeapManager){
    I(!heapManager);
    heapManager=newHeapManager;
    heapManager->addReference();
  }
  HeapManager *getHeapManager(void) const{
    I(heapManager);
    return heapManager;
  }
  inline void setStack(VAddr stackLb, VAddr stackUb){
    myStackAddrLb=stackLb;
    myStackAddrUb=stackUb;
  }
  inline VAddr getStackAddr(void) const{
    return myStackAddrLb;
  }
  inline VAddr getStackSize(void) const{
    return myStackAddrUb-myStackAddrLb;
  }
  void initAddressing(VAddr dataVAddrLb, VAddr dataVAddrUb,
		      MINTAddrType rMap, MINTAddrType mMap, MINTAddrType sTop);
  RAddr virt2real(VAddr vaddr, short opflags=E_READ | E_BYTE) const{
    //I(isValidDataVAddr(vaddr));
	 DebugAssert(isValidDataVAddr(vaddr));
    return virtToRealOffset+vaddr;
  }
  VAddr real2virt(RAddr raddr) const{
    VAddr vaddr=raddr-virtToRealOffset;
    I(isValidDataVAddr(vaddr));
    return vaddr;
  }
  RAddr getVirt2RealOffset() const
  {
	  return virtToRealOffset;
  }

  bool isHeapData(VAddr addr) const{
    I(heapManager);
    return heapManager->isHeapAddr(addr);
  }

  bool isLocalStackData(VAddr addr) const {
    return (addr>=myStackAddrLb)&&(addr<myStackAddrUb);
  }

  VAddr getStackTop() const{
    return myStackAddrLb;
  }
  // END Memory Mapping

  ThreadContext *getParent() const { return parent; }

  void useSameAddrSpace(ThreadContext *parent);
  void shareAddrSpace(ThreadContext *parent, int32_t share_all, int32_t copy_stack);
  void newChild(ThreadContext *child);
  void init();

  icode_ptr getPicode() const { return picode; }
  void setPicode(icode_ptr p) {
    picode = p;
  }

  icode_ptr getTarget() const { return target; }
  void setTarget(icode_ptr p) {
    target = p;
  }

  int32_t getperrno() const { return *perrno; }
  void setperrno(int32_t v) {
    I(perrno);
    *perrno = v;
  }

  int32_t getFD(int32_t id) const { return fd[id]; }
  void setFD(int32_t id, int32_t val) {
    fd[id] = val;
  }
  
  static void initMainThread();

  bool isBusyWaiting;
  static uint32_t globalBusyWaitStamp;
  uint32_t busyWaitStamp;

  void StopBusyWait()
  {
	  I(isBusyWaiting);
	  isBusyWaiting = false;
  }
  void StartBusyWait()
  {
	  I(!isBusyWaiting);
	  isBusyWaiting = true;
	  busyWaitStamp = globalBusyWaitStamp++;
  }
  bool IsBusyWaiting()
  {
	  return isBusyWaiting;
  }
  void StopBusyWaitStamped(uint32_t stamp)
  {
	  if(isBusyWaiting && stamp == busyWaitStamp)
	  {
		  StopBusyWait();
	  }
  }
  typedef CallbackMember1<ThreadContext,uint32_t,&ThreadContext::StopBusyWaitStamped> stopBusyWaitCB;
  void StopBusyWaitIn(TimeDelta_t t)
  {
	  stopBusyWaitCB::schedule(t,this,busyWaitStamp);
  }
  void BusyWaitFor(TimeDelta_t t)
  {
	  StartBusyWait();
	  StopBusyWaitIn(t);
  }
};

typedef ThreadContext mint_thread_t;

// This class helps extract function parameters for substitutions (e.g. in subs.cpp)
// First, prepare for parameter extraction by constructing an instance of MintFuncArgs.
// The constructor takes as a parameter the ThreadContext in which a function has just
// been called (i.e. right after the jal and the delay slot have been emulated
// Then get the parameters in order from first to last, using getInt32 or getInt64
// MintFuncArgs automatically takes care of getting the needed parameter from the register
// or from the stack, according to the MIPS III caling convention. In particular, it correctly
// implements extraction of 64-bit parameters, allowing lstat64 and similar functions to work
// The entire process of parameter extraction does not change the thread context in any way
class MintFuncArgs{
 private:
  const ThreadContext *myContext;
  const icode_t *myIcode;
  int32_t   curPos;
 public:
  MintFuncArgs(const ThreadContext *context, const icode_t *picode)
    : myContext(context), myIcode(picode), curPos(0)
    {
    }
  int32_t getInt32(void);
  int64_t getInt64(void);
  VAddr getVAddr(void){ return (VAddr)getInt32(); }
};

#define REGNUM(R) (*((int32_t *) &pthread->reg[R]))

#endif // THREADCONTEXT_H
