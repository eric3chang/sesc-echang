/* 
   SESC: Super ESCalar simulator
   Copyright (C) 2003 University of Illinois.

   Contributed by Jose Renau
                  Basilio Fraguela
                  James Tuck
                  Wei Liu
                  Milos Prvulovic
                  Luis Ceze
                  Smruti Sarangi
                  Paul Sack
                  Karin Strauss

This file is part of SESC.

SESC is free software; you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation;
either version 2, or (at your option) any later version.

SESC is    distributed in the  hope that  it will  be  useful, but  WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should  have received a copy of  the GNU General  Public License along with
SESC; see the file COPYING.  If not, write to the  Free Software Foundation, 59
Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#ifndef _MSC_VER
#include <alloca.h>
#else
#include <cstdlib>
#endif
#include <iostream>
#include <fstream>
#include <sstream>
#include <typeinfo>

#include "icode.h"
#include "opcodes.h"

#include "SescConf.h"
#include "CacheCore.h"
#include "GStats.h"
#include "GMemorySystem.h"
#include "GProcessor.h"
#include "FetchEngine.h"
#include "TMInterface.h"
#include "zlibStreams.h"

#include <fstream>
#include <string>

#include "ThreadContext.h"
#include "OSSim.h"

#include "Devices/AllDeviceTypes.h"

OSSim   *osSim=0;
uint64_t globalInstructionStampCounter = 1;

/**********************
 * OSSim
 */

#ifdef DEBUG_PROC_DIAG
int gbCurrentPid;
int gbCurrentProcessor;
#endif
#ifdef DEBUG_HEAP_DIAG
bool* heapMask;
std::map<VAddr,int> allocSizeMap;
#endif

char *OSSim::benchName=0;

/*
 * 2010/08/06 eric
 * commented out because not being used
 *
static void sescConfSignal(int32_t sig)
{
  static bool sigFaulting=false;

  if( sigFaulting )
    abort();

  sigFaulting=true;

  MSG("signal %d received. Dumping partial statistics\n", sig);

  osSim->reportOnTheFly();

  Report::close();

  abort();
}
*/

void OSSim::reportOnTheFly(const char *file)
{
  char *tmp;
  
  if( !file )
    file = reportFile;

  tmp = (char *)malloc(strlen(file));
  strcpy(tmp,file);
  
  Report::openFile(tmp);

  SescConf->dump();
  
  report("Signal");

  Report::close();

  free(tmp);
}

OSSim::OSSim(int32_t argc, char **argv, char **envp)
  : traceFile(0) 
    ,snapshotGlobalClock(0)    
    ,finishWorkNowCB(&cpus)
{ 
  I(osSim == 0);
	rabbitModeCounter = 0;
  osSim = this;

  char *tmp=(char *)malloc(argc*50+4096);
  tmp[0] = 0;
  for(int32_t i = 0; i < argc; i++) {
    strcat(tmp,argv[i]);
    strcat(tmp," ");
  }

  benchRunning=tmp;

  benchSection = 0;


  // The problem of passign the environment is that the stack size is
  // different depending of the machine where the simulator is
  // running. This produces different results. Incredible but true
  // (ask Basilio if you don't believe it)
  char *nenv[2]; 
  nenv[0] = 0;
  processParams(argc,argv,nenv);
}

void OSSim::processParams(int32_t argc, char **argv, char **envp)
{ 
  const char *x6="XXXXXX";
  bool trace_flag = false;

  // Change, add parameters to mint
  char **nargv = (char **)malloc((20+argc)*sizeof(char *));
  int32_t nargc;
  
  nargv[0] = strdup(argv[0]);

  int32_t i;
  int32_t ni=1;

  nInst2Skip=0;
  nInst2Sim=0;

  bool useMTMarks = false;
  int32_t  mtId=0;

  simMarks.total = 0;
  simMarks.begin = 0;
  simMarks.end = (~0UL)-1;
  simMarks.mtMarks=false;

  const char *xtraPat=0;
  const char *reportTo=0;
  const char *confName=0;
  const char *extension=0;
  justTest=false;

  if( argc < 2 ) {
    fprintf(stderr,"%s usage:\n",argv[0]);
    fprintf(stderr,"\t-cTEXT      ; Configuration file. Overrides sesc.conf and SESCCONF\n");
    fprintf(stderr,"\t-xTEXT      ; Extra key added in the report file name\n");
    fprintf(stderr,"\t-dTEXT      ; Change the name of the report file\n");
    fprintf(stderr,"\t-fTEXT      ; Fix the extension of the report file\n");
    fprintf(stderr,"\t-t          ; Do not execute, just test the configuration file\n");
    fprintf(stderr,"\t-yINT       ; Number of instructions to simulate\n");
    fprintf(stderr,"\t-bTEXT      ; Benchmark specific configuration section\n");

    fprintf(stderr,"\t-wINT       ; Number of instructions to skip in Rabbit Mode (-w1 means forever)\n");
    fprintf(stderr,"\t-1INT -2INT ; Simulate between marks -1 and -2 (start in rabbitmode)\n");
    fprintf(stderr,"\t-sINT       ; Total amount of shared memory reserved\n");
    fprintf(stderr,"\t-hINT       ; Total amount of heap memory reserved\n");
    fprintf(stderr,"\t-kINT       ; Stack size per thread\n");
    fprintf(stderr,"\t-T          ; Generate trace-file\n");
    fprintf(stderr,"\n\nExamples:\n");
    fprintf(stderr,"%s -k65536 -dreportName ./simulation \n",argv[0]);
    fprintf(stderr,"%s -h0x8000000 -xtest ../tests/crafty <../tests/tt.in\n",argv[0]);
    exit(0);
  }
  
  for(i=1; i < argc; i++)
  {
    if(argv[i][0] == '-')
    {
      if( argv[i][1] == 'w' )
      {
        if( isdigit(argv[i][2]) )
          nInst2Skip = strtol(&argv[i][2], 0, 0 );
        else {
          i++;
          nInst2Skip = strtol(argv[i], 0, 0 );
        }
      }
      else if( argv[i][1] == 'y' )
      {
        if( isdigit(argv[i][2]) )
          nInst2Sim = strtol(&argv[i][2], 0, 0 );
        else {
          i++;
          nInst2Sim = strtol(argv[i], 0, 0 );
        }
      }
      else if( argv[i][1] == 'm' )
      {
        useMTMarks=true;
        simMarks.mtMarks=true;
        if( argv[i][2] != 0 ) 
          mtId = strtol(&argv[i][2], 0, 0 );
        else {
          i++;
          mtId = strtol(argv[i], 0, 0 );
        }
        idSimMarks[mtId].total = 0;
        idSimMarks[mtId].begin = 0;
        idSimMarks[mtId].end = (~0UL)-1;
        idSimMarks[mtId].mtMarks=false;
      }
      else if( argv[i][1] == '1' )
      {
        if( argv[i][2] != 0 ) {
          if( useMTMarks )
            idSimMarks[mtId].begin = strtol(&argv[i][2], 0, 0);
          else
            simMarks.begin = strtol(&argv[i][2], 0, 0 );
        } else {
          i++;
          if( useMTMarks )
            idSimMarks[mtId].begin = strtol(argv[i], 0, 0 );
          else
            simMarks.begin = strtol(argv[i], 0, 0 );
        }
        if(!useMTMarks)
          simMarks.total = 0;
      }
      else if( argv[i][1] == '2' )
      {
        if( argv[i][2] != 0 ) {
          if( useMTMarks)
            idSimMarks[mtId].end = strtol(&argv[i][2], 0, 0 );
          else
            simMarks.end = strtol(&argv[i][2], 0, 0 );
        } else {
          i++;
          if( useMTMarks )
            idSimMarks[mtId].end = strtol(argv[i], 0, 0 );
          else
            simMarks.end = strtol(argv[i], 0, 0 );
        }
        if(!useMTMarks)
          simMarks.total = 0;
      }
      else if( argv[i][1] == 'b' )
      {
        if( argv[i][2] != 0 )
          benchSection = &argv[i][2];
        else {
          i++;
          benchSection = argv[i];
        }
      }
      else if( argv[i][1] == 'c' )
      {
        if( argv[i][2] != 0 )
          confName = &argv[i][2];
        else {
          i++;
          confName = argv[i];
        }
      }
      else if( argv[i][1] == 'x' )
      {
        if( argv[i][2] != 0 )
          xtraPat = &argv[i][2];
        else {
          i++;
          xtraPat = argv[i];
        }
      }
      else if( argv[i][1] == 'd' )
      {
        I(reportTo==0);
        if( argv[i][2] != 0 )
          reportTo = &argv[i][2];
        else {
          i++;
          reportTo = argv[i];
        }
      }
      else if( argv[i][1] == 'f' )
      {
        I(extension==0);
        if( argv[i][2] != 0 )
          extension = &argv[i][2];
        else {
          i++;
          extension = argv[i];
        }
      }
      else if( argv[i][1] == 'P' )
      {
        justTest = true;
      }
      else if( argv[i][1] == 't' )
      {
        justTest = true;
      }
      else if( argv[i][1] == 'T' )
      {
        trace_flag = true;
      }
      else
      {
        nargv[ni] = strdup(argv[i]);
        ni++;
      }
      continue;
    }    //    if(argv[i][0] == '-')

    if(isdigit(argv[i][0])) {
      nargv[ni] = strdup(argv[i]);
      continue;
    }

    break;
  } //for(i=1; i < argc; i++)

  char *name = (i == argc) ? argv[0] : argv[i];
  {
#ifndef _WIN32
    char *p = strrchr(name, '/');
    if (p)
      name = p + 1;
#endif
  }
  benchName = strdup(name);
#ifdef _MSC_VER
  Objname = benchName;
#endif

  I(nInst2Skip>=0);

#ifndef QEMU_DRIVEN
  nargv[ni++]= strdup("--");
#endif
  
  for(; i < argc; i++) {
    nargv[ni] = strdup(argv[i]);
    ni++;
  }
  nargc = ni;

  SescConf = new SConfig(confName);   // First thing to do

  Instruction::initialize(nargc, nargv, envp);

  if( reportTo ) {
    reportFile = (char *)malloc(30 + strlen(reportTo));
    sprintf(reportFile, "%s.%s", reportTo, extension ? extension : x6);
  }else{
    if( getenv("REPORTFILE") ) {
      reportFile = strdup(getenv("REPORTFILE"));
    }else{
      reportFile = (char *)malloc(30 + 2*(strlen(benchName) + strlen(xtraPat?xtraPat:benchName)));
      if( xtraPat )
        sprintf(reportFile, "sesc_%s_%s.%s", xtraPat, benchName, extension ? extension : x6);
      else
        sprintf(reportFile, "sesc_%s.%s", benchName, extension ? extension : x6);
    }
  }
 
  char *finalReportFile = (char *)strdup(reportFile);
  Report::openFile(finalReportFile);

#ifdef SESC_THERM
  {
    thermFile = (char*)malloc(strlen(finalReportFile) + 7);
    char *pp = strrchr(finalReportFile,'.');
    *pp = 0;
    sprintf(thermFile, "%s.therm.%s",finalReportFile, pp + 1);
    ReportTherm::openFile(thermFile);
    strcpy(pp, thermFile + ((pp - finalReportFile) +6));
  }
#endif

  if (trace_flag) {
    traceFile = (char*)malloc(strlen(finalReportFile) + 7);
    char *p = strrchr(finalReportFile,'.');
    *p = 0;
    sprintf(traceFile, "%s.trace.%s",finalReportFile, p + 1);
    Report::openFile(traceFile);
      strcpy(p, traceFile + ((p - finalReportFile) +6));
  }

  free(finalReportFile);

#ifdef SESC_THERM
  free(thermFile);
#endif

  for(i=0; i < nargc; i++)
    free(nargv[i]);

  free(nargv);
  
#ifdef DEBUG_HEAP_DIAG
	heapMask = new bool[Heap_size];
	for(size_t i = 0; i < Heap_size; i++)
		heapMask[i] = false;
#endif
}

OSSim::~OSSim() 
{
#if (defined TLS)
  tls::Epoch::staticDestructor();
#endif
  Instruction::finalize();

#ifdef SESC_THERM
  ReportTherm::stopCB();
#endif

  free(benchRunning);
  free(reportFile);
  
  delete SescConf;

  free(benchName);
  if (trace())
    free(traceFile);

  //printf("destructed..\n");
}

void OSSim::eventSysconf(Pid_t ppid, Pid_t fid, int32_t flags)
{
  LOG("OSSim::sysconf(%d,%d,0x%x)", ppid, fid, flags);
  ProcessId *myProcessId=ProcessId::getProcessId(fid);
  // Can it be reconfigured on-the-fly?
  if(!myProcessId->sysconf(flags)){
    // Tre process can not be reconfigured while running where it is running
    // Make it non-runnable
    cpus.makeNonRunnable(myProcessId);
    // Now reconfigure it, this is the second try
    // Should always succeed
    I(myProcessId->sysconf(flags));
    // Make it runnable again
    cpus.makeRunnable(myProcessId);
  }
}

int32_t OSSim::eventGetconf(Pid_t curPid, Pid_t targPid)
{
  ProcessId *myProcessId=ProcessId::getProcessId(targPid);
  return myProcessId->getconf();
}

void OSSim::eventSpawn(Pid_t ppid, Pid_t fid, int32_t flags, bool stopped)
{
  if (NoMigration)
    flags |= SESC_FLAG_NOMIGRATE;
  
  LOG("OSSim::spawn(%d,%d,0x%x,%d)", ppid, fid, flags,stopped);
  ProcessId *procId = ProcessId::create(ppid, fid, flags);
  if(!stopped)
    cpus.makeRunnable(procId);
}

void OSSim::stop(Pid_t pid)
{
  // Get the procss descriptor
  ProcessId *proc = ProcessId::getProcessId(pid);
  // The descriptor should exist, and the process should be runnable
  I(proc&&((proc->getState()==ReadyState)||(proc->getState()==RunningState)));
  // Make the process non-runnable
  cpus.makeNonRunnable(proc);
}

void OSSim::unstop(Pid_t pid)
{
  // Get the procss descriptor
  ProcessId *proc = ProcessId::getProcessId(pid);
  // The descriptor should exist, and the process should be in the InvalidState
  I(proc&&(proc->getState()==InvalidState));
  // Make the process runnable
  cpus.makeRunnable(proc);
}

void OSSim::setPriority(Pid_t pid, int32_t newPrio)
{
  ProcessId *proc = ProcessId::getProcessId(pid);
  I(proc);

  int32_t oldPrio=proc->getPriority();

  if(newPrio==oldPrio)
    return;

  // Set the new priority of the process
  ProcessId *otherProc=proc->setPriority(newPrio);
  if(newPrio>oldPrio){
    // Priority is better now, check if still running
    if(proc->getState()==RunningState){
      // Is there a process we need to swap with
      if(otherProc){
        // Get the cpu where the demoted process is running
        CPU_t cpu=proc->getCPU();
        // Switch the demoted process out
        cpus.switchOut(cpu,proc);
        // Switch the new process in
        cpus.switchIn(cpu,otherProc);
      }
    }
  }else{
    // Priority is worse now, check if ready but not already running
    if(proc->getState()==ReadyState){
      // Is there a process we need to swap with
      if(otherProc){
        // Get the cpu where the other process is running
        CPU_t cpu=otherProc->getCPU();
        // Switch the victim process out
        cpus.switchOut(cpu,otherProc);
        // Switch the promoted process in
        cpus.switchIn(cpu,proc);
      }
    }
  }
}

int32_t OSSim::getPriority(Pid_t pid)
{
  // Get the process descriptor
  ProcessId *proc = ProcessId::getProcessId(pid);
  // It should exist
  I(proc);
  // Return the priority of the process
  return proc->getPriority();
  
}

void OSSim::tryWakeupParent(Pid_t cpid) 
{
  ProcessId *proc = ProcessId::getProcessId(cpid);
  I(proc);
  Pid_t ppid = proc->getPPid();
  if (ppid < 0)
    return;
  
  ProcessId *pproc = ProcessId::getProcessId(ppid);
  // Does the parent process still exist?
  if(pproc == 0)
    return;
  
  if(pproc->getState()==WaitingState) {
    LOG("Waiting pid(%d) is awaked (child %d call)",ppid,cpid);
    pproc->setState(InvalidState);
    cpus.makeRunnable(pproc);
  }
}

void OSSim::eventExit(Pid_t cpid, int32_t err)
{
  LOG("OSSim::exit err[%d] (cpid %d)", err, cpid);
  ProcessId *proc = ProcessId::getProcessId(cpid);
  I(proc);
  // If not in InvalidState, removefrom the running queue
  if(proc->getState()==RunningState || proc->getState()==ReadyState )
    cpus.makeNonRunnable(proc);
  // Try to wakeup parent
  tryWakeupParent(cpid);
  // Destroy the process
  proc->destroy();
  // Free the ThreadContext  
  ThreadContext::getContext(cpid)->free();
 }

void OSSim::eventWait(Pid_t cpid)
{
  // All the threads have already finished
  if(cpid<0)
    return;
  LOG("OSSim::wait (cpid %d)", cpid);
  ProcessId *proc = ProcessId::getProcessId(cpid);
  if(proc->getNChilds()==0){
    // No child pending
    return;
  }
  // Should be still running
  I(proc->getState()==RunningState);
  // Make it non-runnable
  cpus.makeNonRunnable(proc);
  // Set state to WaitingState
  proc->setState(WaitingState);
}

#if !(defined MIPS_EMUL)
void OSSim::eventSaveContext(Pid_t pid)
{
  ProcessId *proc = ProcessId::getProcessId(pid);
  I(proc!=0);
  if(proc->getState()==RunningState){
    CPU_t cpu=proc->getCPU();
    cpus.getProcessor(cpu)->saveThreadContext(pid);
  }
}
#endif

ThreadContext *OSSim::getContext(Pid_t pid)
{
#if !(defined MIPS_EMUL)
  I(pid!=-1);
  ProcessId *proc = ProcessId::getProcessId(pid);
  if(proc->getState()==RunningState){
    CPU_t cpu=proc->getCPU();
    return cpus.getProcessor(cpu)->getThreadContext(pid);
  }
#endif
  return ThreadContext::getContext(pid);
}

int32_t OSSim::getContextRegister(Pid_t pid, int32_t regnum)
{
  ThreadContext *s = getContext(pid);
#if (defined MIPS_EMUL)
  I(0);
  return 0;
#else
  return (* (long *) ((long)(s->reg) + ((regnum) << 2)));
#endif
}

void OSSim::eventSetInstructionPointer(Pid_t pid, icode_ptr picode){
#if !(defined MIPS_EMUL)
  I(pid!=-1);
  ProcessId *proc = ProcessId::getProcessId(pid);
  if(proc->getState()==RunningState){
    CPU_t cpu=proc->getCPU();
    cpus.getProcessor(cpu)->setInstructionPointer(pid,picode);
  }else{
    ThreadContext::getContext(pid)->setPicode(picode);
  }
#endif
}
icode_ptr OSSim::eventGetInstructionPointer(Pid_t pid)
{
#if (defined MIPS_EMUL)
  return 0;
#else
  if(pid < 0) // -1
    return &invalidIcode;
  
  ProcessId *proc = ProcessId::getProcessId(pid);
  if(proc&&(proc->getState()==RunningState)){
    CPU_t cpu=proc->getCPU();
    return cpus.getProcessor(cpu)->getInstructionPointer(pid);
  }

  ThreadContext *context=ThreadContext::getContext(pid);
  I(context);

  return context->getPicode();
#endif
}

#if !(defined MIPS_EMUL)
void OSSim::eventLoadContext(Pid_t pid)
{
  ProcessId *proc = ProcessId::getProcessId(pid);
  if(proc->getState()==RunningState){
    CPU_t cpu=proc->getCPU();
    cpus.getProcessor(cpu)->loadThreadContext(pid);
  }
}
#endif

Pid_t OSSim::eventGetPPid(Pid_t pid)
{
  ProcessId *proc = ProcessId::getProcessId(pid);
  I(proc);
  return proc->getPPid();  
}

void OSSim::eventSetPPid(Pid_t pid, Pid_t ppid)
{
  ProcessId *proc = ProcessId::getProcessId(pid);
  I(proc);
  proc->setPPid(ppid);
}

int32_t OSSim::eventSuspend(Pid_t cpid, Pid_t pid)
{
  LOG("OSSim::suspend(%d) Received from pid %d",pid,cpid);
  ProcessId *proc = ProcessId::getProcessId(pid);
  if( proc == 0 ) {
    LOG("OSSim::suspend(%d) non existing process???", pid);
    return 0;
  }
  // Increment the suspend counter
  proc->incSuspendedCounter();
  // Check if process already suspended
  if(proc->getState()==SuspendedState){
    I(0);
    LOG("OSSim::suspend(%d) already suspended (recursive=%d)"
        , pid, proc->getSuspendedCounter());
    return 0;
  }
  // The process should be ready or running
  I((proc->getState()==ReadyState)||(proc->getState()==RunningState));
  // Need to suspend only if suspended counter is positive
  if(proc->getSuspendedCounter()>0){
    // The process is no longer runnable
    cpus.makeNonRunnable(proc);
    // Set the state to SuspendedState
    proc->setState(SuspendedState);
  }else{
    I(0);
    LOG("OSSim::suspend(%d,%d) OOO suspend/resume (%d)", cpid, pid, proc->getSuspendedCounter());
  }
  return 1;
}

int32_t OSSim::eventResume(Pid_t cpid, Pid_t pid)
{
  LOG("OSSim::resume(%d,%d)", cpid, pid);
  ProcessId *proc = ProcessId::getProcessId(pid);
  if( proc == 0 ) {
    LOG("OSSim::resume(%d,%d) non existing process???", cpid, pid);
    return 0;
  }
  // Decrement the suspend counter
  proc->decSuspendedCounter();
  // If process is in SuspendedState
  if(proc->getState()==SuspendedState){
    // If the suspend count is not positive
    if(proc->getSuspendedCounter()<=0){
      // Make the process runnable
      proc->setState(InvalidState);
      cpus.makeRunnable(proc);
    }
    return 1;
  }else{
    I(0);
    LOG("OSSim::resume(%d,%d) OOO suspend/resume (%d)", cpid, pid, proc->getSuspendedCounter());
  }
  return 0;
}

int32_t OSSim::eventYield(Pid_t curPid, Pid_t yieldPid)
{
//  LOG("OSSim::yield(%d,%d)", curPid, yieldPid);
  ProcessId  *curProc=ProcessId::getProcessId(curPid);
  // The current process should be running
  I(curProc->getState()==RunningState);
  // get the CPU where the current process is running
  CPU_t cpu=curProc->getCPU();
  // Get the ProcessId of the new process
  ProcessId *yieldProc;
  if(yieldPid<0){
    // No specific new process, get next ready process
    yieldProc=ProcessId::queueGet(cpu);
  }else{
    // Specific ready process, get its ProcessId
    yieldProc=ProcessId::getProcessId(yieldPid);
    // there should be such a process
    if(!yieldProc){
      LOG("OSSim::yield(%d) to non existing process???", yieldPid);
      return 0;
    }
  }
  // Do nothing if no process to yield to
  if(!yieldProc)
    return 1;
  // Do nothing if the new process already running
  if(yieldProc->getState()==RunningState)
    return 1;
  // The new process should not be suspended
  if(yieldProc->getState()==SuspendedState){
    LOG("OSSim::yield(%d) to a suspended process???", yieldPid);
    return 0;
  }
  // The new process should not be pinned to a other processor
  if(yieldProc->isPinned()&&(yieldProc->getCPU()!=cpu)){
    LOG("OSSim::yield(%d) to a NOMIGRABLE process in another CPU", yieldPid);
    return 0;
  }
  // Remove current process from the processor
  cpus.switchOut(cpu,curProc);
  // Put the new process on that processor
  cpus.switchIn(cpu,yieldProc);
  return 1;
}

void OSSim::initBoot()
{
  static bool alreadyBoot = false;
  if (alreadyBoot)
    return;
  alreadyBoot = true;

  ProcessId::boot();

  // FIXME2: Change this for a static method in MemoryOS so that MemoryOS::boot
  // is called instead (it would call all the reserved memorysystem). Once this
  // is done, remove GMemorySystem from GProcessor.
  for(size_t i=0; i < cpus.size(); i++) {
    I(cpus.getProcessor(i));
    I(cpus.getProcessor(i)->getMemorySystem());
    I(cpus.getProcessor(i)->getMemorySystem()->getMemoryOS());

    cpus.getProcessor(i)->getMemorySystem()->getMemoryOS()->boot();
  }
  MemSys_system.Initialize(std::string(SescConf->getCharPtr("","MemorySystemConfig")));

  // read it so it gets dumped 
  const char *technology = SescConf->getCharPtr("","technology");
  frequency = SescConf->getDouble(technology,"frequency");

  if (SescConf->checkBool("","NoMigration"))
    NoMigration = SescConf->getBool("","NoMigration");
  else
    NoMigration = false;

  for(int i = 0; i < 256; i++)
  {
	  if(SescConf->checkInt("","ProcessorSkew",i))
	  {
		  processorSkewCounter[i] = SescConf->getInt("","ProcessorSkew",i);
	  }
	  else
	  {
		  processorSkewCounter[i] = 0;
	  }
  }

  if(!LoadCheckpoint())
  {
	eventSpawn(-1,0,0);
  }

}

void OSSim::preBoot()
{
  static bool alreadyBoot = false;
  if (alreadyBoot)
    return;
  alreadyBoot = true;

  SescConf->dump();

  SescConf->lock();       // All the objects should be loaded

  time_t t = time(0);
  Report::field("OSSim:beginTime=%s", ctime(&t));

  Report::field("OSSim:bench=%s", benchRunning);
  Report::field("OSSim:benchName=%s", benchName);
  if( nInst2Skip ) 
    Report::field("OSSim:rabbit=%lld",nInst2Skip);

  if( nInst2Sim )
    Report::field("OSSim:nInst2Sim=%lld",nInst2Sim);
  else{// 0 would never stop 
    nInst2Sim = ((~0ULL) - 1024)/2;
  }
  TMInterface::Initialize();
  FetchEngine::setnInst2Sim(nInst2Sim);

#ifdef QEMU_DRIVEN
  n_inst_stop = nInst2Sim;
  n_inst_start= nInst2Skip;
#endif  

#ifdef TS_PROFILING
  profiler = new Profile();
#endif  


#ifdef TS_RISKLOADPROF
  riskLoadProf = new RiskLoadProf();
#endif

  if( justTest ) {
    MSG("Configuration tested");
    return;
  }

  gettimeofday(&stTime, 0);
  if( nInst2Skip ) {
      MSG("Start rabbit mode forever...");
    if (nInst2Skip == 1) {
		RabbitModeStart();
      nInst2Skip = 0;
    }
	else
	{
      MSG("Start Skipping Initialization (skipping %lld instructions)...",nInst2Skip);
		GProcessor *proc = pid2GProcessor(0);
		proc->goRabbitMode(nInst2Skip);
		MSG("...End Skipping Initialization (Rabbit mode)");
	}
  }else if( simMarks.begin || simMarks.mtMarks ) {
    unsigned i=0;

    MSG("Start Skipping Initialization (multithreaded mode)...");
    
    GProcessor *proc = pid2GProcessor(0);
    proc->goRabbitMode(1);
    
    I(cpus.getProcessor(0)==proc);
    
    I( idSimMarks.size() < cpus.size() );

    MSG("Rabbit mode preempted by new thread.");

    while(!enoughMTMarks1()) {
      MSG("Rabbit mode preempted by new thread.");

      if( !cpus.getProcessor(i)->availableFlows() ) {
        cpus.getProcessor(i)->goRabbitMode(1);
      }

      i++;
      if(i==cpus.size())
        i=0;
    }
    MSG("...End Skipping Initialization (Rabbit mode)");
    //MSG("Start Skipping Initialization (skipping %ld simulation marks)...", simMarks.begin);
    //GProcessor *proc = pid2GProcessor(0);
    //proc->goRabbitMode(1);
    //MSG("...End Skipping Initialization (Rabbit mode)");
  }
}

void OSSim::postBoot()
{
  // Launch threads
  cpus.run();
  simFinish();
  TMInterface::Destroy();
}

void OSSim::simFinish()
{
  // Work finished, dump statistics
  report("Final");

  time_t currentTime;
  struct tm * timeInfo;
  time ( &currentTime );
  timeInfo = localtime ( &currentTime );

  time_t t = time(0);
  Report::field("OSSim:endTime=%s", ctime(&t));

  Report::close();

  if(SescConf->checkCharPtr("","MemDeviceReportFile"))
  {
	  stringstream filenameBuffer, timeBuffer;
	  filenameBuffer << SescConf->getCharPtr("","MemDeviceReportFile");

	  timeBuffer.width(2);
	  timeBuffer.fill('0');
	  timeBuffer << std::right << (timeInfo->tm_mon+1);
	  timeBuffer.width(2);
	  timeBuffer.fill('0');
	  timeBuffer << std::right << timeInfo->tm_mday;
	  timeBuffer.width(2);
	  timeBuffer.fill('0');
	  timeBuffer << std::right << timeInfo->tm_hour;
	  timeBuffer.width(2);
	  timeBuffer.fill('0');
	  timeBuffer << std::right << timeInfo->tm_min;
	  //tempBuffer << ".memDevResults";

	  string filename = filenameBuffer.str();
     std::ofstream out(filename.c_str(),std::ios::out);

	  //std::ofstream out(SescConf->getCharPtr("","MemDeviceReportFile"),std::ios::out);
     //out << "Group string ReportFileName " << SescConf->getCharPtr("","MemDeviceReportFile");
	  out << "Group string ReportFileName " << filename;
     if(SescConf->checkCharPtr("","BenchName"))
     {
        out << " string BenchName " << SescConf->getCharPtr("","BenchName");
     }

     out << endl << endl;
     out << "DateTime" << endl;
     out << "DateTime:DateTime:" << timeBuffer.str() << endl;
     /*
      * //don't really care about processorSkews right now
     for(unsigned int i = 0; i < cpus.size(); i++)
     {
        if(SescConf->checkInt("","ProcessorSkew",i))
        {
           out << " int ProcessorSkew" << i << " " << SescConf->getInt("","ProcessorSkew",i);
        }
        else
        {
           out << " int ProcessorSkew" << i << " 0";
        }
     }
     */
     //out << " #" << std::endl;
     //out << "int TotalRunTime " << globalClock << " #" << std::endl;
     out << endl;
     out << "TotalRunTime" << endl;
     out << "TotalRunTime:TotalRunTime:" << globalClock << endl << endl;
     std::cout << "Dumping stats" << std::endl;

     using Memory::BaseMemDevice;
     std::vector<BaseMemDevice*>deviceSet = MemSys_system.getDeviceSet();

		long long unsigned int totalCacheExclusiveReadHits = 0;
		long long unsigned int totalCacheSharedReadHits = 0;
		long long unsigned int totalCacheExclusiveReadMisses = 0;
		long long unsigned int totalCacheSharedReadMisses = 0;
		long long unsigned int totalCacheWriteHits = 0;
		long long unsigned int totalCacheWriteMisses = 0;

		long long unsigned int totalL1CacheExclusiveReadHits = 0;
		long long unsigned int totalL1CacheSharedReadHits = 0;
		long long unsigned int totalL1CacheExclusiveReadMisses = 0;
		long long unsigned int totalL1CacheSharedReadMisses = 0;
		long long unsigned int totalL1CacheWriteHits = 0;
		long long unsigned int totalL1CacheWriteMisses = 0;

		long long unsigned int totalL2CacheExclusiveReadHits = 0;
		long long unsigned int totalL2CacheSharedReadHits = 0;
		long long unsigned int totalL2CacheExclusiveReadMisses = 0;
		long long unsigned int totalL2CacheSharedReadMisses = 0;
		long long unsigned int totalL2CacheWriteHits = 0;
		long long unsigned int totalL2CacheWriteMisses = 0;

		// for use by directories
		long long unsigned int totalLatency = 0;
		long long unsigned int totalReadResponses = 0;
		long long unsigned int totalLatencySimple = 0;
		long long unsigned int totalReadResponsesSimple = 0;
		int directoryCount = 0;

		// for use by processors
		unsigned long long totalReadCount = 0;
		unsigned long long totalWriteCount = 0;
		unsigned long long totalTotalOperations = 0;

		for (std::vector<BaseMemDevice*>::iterator i=deviceSet.begin(); i!=deviceSet.end(); i++)
		{
			BaseMemDevice* ptr = *i;
			unsigned int cacheExclusiveReadHits = 0;
			unsigned int cacheExclusiveReadMisses = 0;
			unsigned int cacheSharedReadHits = 0;
			unsigned int cacheSharedReadMisses = 0;
			unsigned int cacheWriteHits = 0;
			unsigned int cacheWriteMisses = 0;
			bool isL1Cache = false;
			bool isL2Cache = false;

			// if memdevice is a type of cache, do special processing to get total hits and total misses
			bool isMSICache = (typeid(*ptr)==typeid(Memory::MSICache));
			bool isMESICache = (typeid(*ptr)==typeid(Memory::MESICache));
			bool isMOESICache = (typeid(*ptr)==typeid(Memory::MOESICache));
			bool isBIPMOESICache = (typeid(*ptr)==typeid(Memory::BIPMOESICache));
			bool isBIPDirectory = (typeid(*ptr)==typeid(Memory::BIPDirectory));
			bool isOriginDirectory = (typeid(*ptr)==typeid(Memory::OriginDirectory));
			bool isProcessor = (typeid(*ptr)==typeid(Memory::SESCProcessorInterface));

			if (isMSICache)
			{
				Memory::MSICache *tempCache = (Memory::MSICache*)ptr;
				string deviceName = tempCache->DeviceName();
				size_t findL1 = deviceName.find("L1");
				size_t findL2 = deviceName.find("L2");

				if (findL1!=string::npos)
				{
					isL1Cache = true;
				}
				if (findL2!=string::npos)
				{
					isL2Cache = true;
				}

				cacheExclusiveReadHits = tempCache->getExclusiveReadHits();
				cacheSharedReadHits = tempCache->getSharedReadHits();
				cacheExclusiveReadMisses = tempCache->getExclusiveReadMisses();
				cacheSharedReadMisses = tempCache->getSharedReadMisses();
				cacheWriteHits = tempCache->getWriteHits();
				cacheWriteMisses = tempCache->getWriteMisses();
			}
			else if (isMESICache)
			{
				Memory::MESICache *tempCache = (Memory::MESICache*)ptr;
				string deviceName = tempCache->DeviceName();
				size_t findL1 = deviceName.find("L1");
				size_t findL2 = deviceName.find("L2");

				if (findL1!=string::npos)
				{
					isL1Cache = true;
				}
				if (findL2!=string::npos)
				{
					isL2Cache = true;
				}

				cacheExclusiveReadHits = tempCache->getExclusiveReadHits();
				cacheSharedReadHits = tempCache->getSharedReadHits();
				cacheExclusiveReadMisses = tempCache->getExclusiveReadMisses();
				cacheSharedReadMisses = tempCache->getSharedReadMisses();
				cacheWriteHits = tempCache->getWriteHits();
				cacheWriteMisses = tempCache->getWriteMisses();
			}
			else if (isMOESICache)
			{
				Memory::MOESICache *tempCache = (Memory::MOESICache*)ptr;
				string deviceName = tempCache->DeviceName();
				size_t findL1 = deviceName.find("L1");
				size_t findL2 = deviceName.find("L2");

				if (findL1!=string::npos)
				{
					isL1Cache = true;
				}
				if (findL2!=string::npos)
				{
					isL2Cache = true;
				}

				cacheExclusiveReadHits = tempCache->getExclusiveReadHits();
				cacheExclusiveReadMisses = tempCache->getExclusiveReadMisses();
				cacheSharedReadHits = tempCache->getSharedReadHits();
				cacheSharedReadMisses = tempCache->getSharedReadMisses();
				cacheWriteHits = tempCache->getWriteHits();
				cacheWriteMisses = tempCache->getWriteMisses();
			}
			else if (isBIPMOESICache)
			{
				Memory::BIPMOESICache *tempCache = (Memory::BIPMOESICache*)ptr;
				string deviceName = tempCache->DeviceName();
				size_t findL1 = deviceName.find("L1");
				size_t findL2 = deviceName.find("L2");

				if (findL1!=string::npos)
				{
					isL1Cache = true;
				}
				if (findL2!=string::npos)
				{
					isL2Cache = true;
				}

				cacheExclusiveReadHits = tempCache->getExclusiveReadHits();
				cacheExclusiveReadMisses = tempCache->getExclusiveReadMisses();
				cacheSharedReadHits = tempCache->getSharedReadHits();
				cacheSharedReadMisses = tempCache->getSharedReadMisses();
				cacheWriteHits = tempCache->getWriteHits();
				cacheWriteMisses = tempCache->getWriteMisses();
			}
			else if (isBIPDirectory)
			{
				Memory::BIPDirectory *tempDevice = (Memory::BIPDirectory*)ptr;

				//totalLatency += tempDevice->GetTotalLatency();
				//totalReadResponses += tempDevice->GetTotalReadResponses();
				//totalLatencySimple += tempDevice->GetTotalLatencySimple();
				//totalReadResponsesSimple += tempDevice->GetTotalReadResponsesSimple();
				directoryCount++;
			}
			else if (isOriginDirectory)
			{
				Memory::OriginDirectory *tempDevice = (Memory::OriginDirectory*)ptr;

				//totalLatency += tempDevice->GetTotalLatency();
				//totalReadResponses += tempDevice->GetTotalReadResponses();
				directoryCount++;
			}
			else if (isProcessor)
			{
				Memory::SESCProcessorInterface *tempDevice = (Memory::SESCProcessorInterface*)ptr;
				totalReadCount += tempDevice->GetReadCount();
				totalWriteCount += tempDevice->GetWriteCount();
				totalTotalOperations += tempDevice->GetTotalOperations();
			}

			out << ptr->DeviceName() << std::endl;
			ptr->DumpStats(out);
			out << std::endl;

			if (isMSICache || isMESICache || isMOESICache || isBIPMOESICache)
			{
				if (isL1Cache)
				{
					totalL1CacheExclusiveReadHits += cacheExclusiveReadHits;
					totalL1CacheSharedReadHits += cacheSharedReadHits;
					totalL1CacheExclusiveReadMisses += cacheExclusiveReadMisses;
					totalL1CacheSharedReadMisses += cacheSharedReadMisses;
					totalL1CacheWriteHits += cacheWriteHits;
					totalL1CacheWriteMisses += cacheWriteMisses;
				}
				if (isL2Cache)
				{
					totalL2CacheExclusiveReadHits += cacheExclusiveReadHits;
					totalL2CacheSharedReadHits += cacheSharedReadHits;
					totalL2CacheExclusiveReadMisses += cacheExclusiveReadMisses;
					totalL2CacheSharedReadMisses += cacheSharedReadMisses;
					totalL2CacheWriteHits += cacheWriteHits;
					totalL2CacheWriteMisses += cacheWriteMisses;
				}
				totalCacheExclusiveReadHits += cacheExclusiveReadHits;
				totalCacheSharedReadHits += cacheSharedReadHits;
				totalCacheExclusiveReadMisses += cacheExclusiveReadMisses;
				totalCacheSharedReadMisses += cacheSharedReadMisses;
				totalCacheWriteHits += cacheWriteHits;
				totalCacheWriteMisses += cacheWriteMisses;
			}
		} // end memdevice for loops

		out << "TotalCache" << endl;
		out << "TotalCache:" << "totalL1CacheExclusiveReadHits:" << totalL1CacheExclusiveReadHits << std::endl;
		out << "TotalCache:" << "totalL1CacheExclusiveReadMisses:" << totalL1CacheExclusiveReadMisses << std::endl;
		out << "TotalCache:" << "totalL1CacheSharedReadHits:" << totalL1CacheSharedReadHits << std::endl;
		out << "TotalCache:" << "totalL1CacheSharedReadMisses:" << totalL1CacheSharedReadMisses << std::endl;
		out << "TotalCache:" << "totalL1CacheWriteHits:" << totalL1CacheWriteHits << std::endl;
		out << "TotalCache:" << "totalL1CacheWriteMisses:" << totalL1CacheWriteMisses << std::endl;
		//out << "TotalCache:" << std::endl;
		out << "TotalCache:" << "totalL2CacheExclusiveReadHits:" << totalL2CacheExclusiveReadHits << std::endl;
		out << "TotalCache:" << "totalL2CacheExclusiveReadMisses:" << totalL2CacheExclusiveReadMisses << std::endl;
		out << "TotalCache:" << "totalL2CacheSharedReadHits:" << totalL2CacheSharedReadHits << std::endl;
		out << "TotalCache:" << "totalL2CacheSharedReadMisses:" << totalL2CacheSharedReadMisses << std::endl;
		out << "TotalCache:" << "totalL2CacheWriteHits:" << totalL2CacheWriteHits << std::endl;
		out << "TotalCache:" << "totalL2CacheWriteMisses:" << totalL2CacheWriteMisses << std::endl;
		//out << "TotalCache:" << std::endl;
		out << "TotalCache:" << "totalCacheExclusiveReadHits:" << totalCacheExclusiveReadHits << std::endl;
		out << "TotalCache:" << "totalCacheExclusiveReadMisses:" << totalCacheExclusiveReadMisses << std::endl;
		out << "TotalCache:" << "totalCacheSharedReadHits:" << totalCacheSharedReadHits << std::endl;
		out << "TotalCache:" << "totalCacheSharedReadMisses:" << totalCacheSharedReadMisses << std::endl;
		out << "TotalCache:" << "totalCacheWriteHits:" << totalCacheWriteHits << std::endl;
		out << "TotalCache:" << "totalCacheWriteMisses:" << totalCacheWriteMisses << std::endl;
		out << endl;

		out << "TotalDirectory" << endl;
		out << "TotalDirectory:" << "directoryCount:" << directoryCount << endl;
		out << "TotalDirectory:" << "totalLatency:" << totalLatency << endl;
		out << "TotalDirectory:" << "totalReadResponses:" << totalReadResponses << endl;
		out << "TotalDirectory:" << "totalLatencySimple:" << totalLatencySimple << endl;
		out << "TotalDirectory:" << "totalReadResponsesSimple:" << totalReadResponsesSimple << endl;
		unsigned long long averageWaitTime = 0;
		if (totalReadResponses != 0)
		{
			averageWaitTime = totalLatency/totalReadResponses;
		}
		unsigned long long averageWaitTimeSimple = 0;
		if (totalReadResponsesSimple != 0)
		{
			averageWaitTimeSimple = totalLatencySimple/totalReadResponsesSimple;
		}
		out << "TotalDirectory:" << "averageWaitTime:" << averageWaitTime << endl;
		out << "TotalDirectory:" << "averageWaitTimeSimple:" << averageWaitTimeSimple << endl;
		out << endl;

		out << "TotalProcessor" << endl;
		out << "TotalProcessor:totalReadCount:" << totalReadCount << endl;
		out << "TotalProcessor:totalWriteCount:" << totalWriteCount << endl;
		out << "TotalProcessor:totalTotalOperations:" << totalTotalOperations << endl;

		out << endl;

     //TMInterface::DumpStats(out);
     out << "EndGroup #" << std::endl;
     out.close();
  }

  if(SescConf->checkCharPtr("","ReportFile"))
  {
	  std::ofstream out(SescConf->getCharPtr("","ReportFile"),std::ios::out);
	  out << "Group string ReportFileName " << SescConf->getCharPtr("","ReportFile");
	  if(SescConf->checkCharPtr("","BenchName"))
	  {
		  out << " string BenchName " << SescConf->getCharPtr("","BenchName");
	  }
	  for(unsigned int i = 0; i < cpus.size(); i++)
	  {
		  if(SescConf->checkInt("","ProcessorSkew",i))
		  {
			  out << " int ProcessorSkew" << i << " " << SescConf->getInt("","ProcessorSkew",i);
		  }
		  else
		  {
			  out << " int ProcessorSkew" << i << " 0";
		  }
	  }
	  out << " #" << std::endl;
	  out << "int TotalRunTime " << globalClock << " #" << std::endl;
	  std::cout << "Dumping stats" << std::endl;
	  TMInterface::DumpStats(out);
	  out << "EndGroup #" << std::endl;
	  out.close();
  }
}

void OSSim::report(const char *str)
{

#ifdef TS_RISKLOADPROF
  riskLoadProf->finalize();
#endif

  ProcessId::report(str);

  for(size_t i=0;i<cpus.size();i++) {
    GProcessor *gproc = cpus.getProcessor(i);
    if( gproc )
      gproc->report(str);
  }
  
  Report::field("OSSim:reportName=%s", str);

  timeval endTime;
  gettimeofday(&endTime, 0);

  double msecs = (endTime.tv_sec - stTime.tv_sec) * 1000 
    + (endTime.tv_usec - stTime.tv_usec) / 1000;

  Report::field("OSSim:msecs=%8.2f:nCPUs=%d:nCycles=%lld"
                ,(double)msecs / 1000
                ,cpus.size()
                ,globalClock);

  Report::field("OSSim:pseudoreset=%lld",snapshotGlobalClock);

  // GStats must be the last to be called because previous ::report
  // can update statistics
  GStats::report(str);

#if (defined TLS)
  tls::Epoch::report();
#endif
}

GProcessor *OSSim::pid2GProcessor(Pid_t pid)
{
  I(ProcessId::getProcessId(pid));
  int32_t cpu = ProcessId::getProcessId(pid)->getCPU();
  // -1 when it has never started to execute
  I(cpu>=0);
    
  return cpus.getProcessor(cpu);
}

ProcessIdState OSSim::getState(Pid_t pid)
{
  I(ProcessId::getProcessId(pid));
  return ProcessId::getProcessId(pid)->getState();
}

GProcessor *OSSim::id2GProcessor(CPU_t cpu)
{
  I(cpus.getProcessor(cpu));
  I(cpus.getProcessor(cpu)->getId() == cpu);
    
  return cpus.getProcessor(cpu);
}

void OSSim::registerProc(GProcessor *core)
{
  cpus.setProcessor(core->getId(),core);
}

void OSSim::unRegisterProc(GProcessor *core)
{
  I(!core->hasWork());
  for(size_t i=0;i<cpus.size();i++){
    if(cpus.getProcessor(i)==core){
      cpus.setProcessor(i,0);
      return;
    }
  }
}

Pid_t OSSim::contextSwitch(CPU_t cpu, Pid_t nPid)
{
  // This is the process we displaced to run the target process
  Pid_t oldPid=-1;
  ProcessId *newProc = ProcessId::getProcessId(nPid);
  I(newProc);
  GProcessor *newCore=cpus.getProcessor(cpu);
  I(newCore);
  // Get the cpu where the target process is already running
  CPU_t runCpu=(newProc->getState()==RunningState)?newProc->getCPU():-1;
  // If already running on the target processor, do nothing
  if(runCpu==cpu)
    return -1;
  // If target core has no available flows, make one     
  if(!newCore->availableFlows()){
    oldPid=newCore->findVictimPid();
    ProcessId *oldProc=ProcessId::getProcessId(oldPid);
    cpus.switchOut(cpu, oldProc);
  }
  // Is the target process already running on another cpu?
  if(runCpu!=-1){
    // Get another process to run on original cpu
    ProcessId *repProc=ProcessId::queueGet(runCpu);
    // Get the target process out of old core
    cpus.switchOut(runCpu, newProc);
    // Get the replacement process (if any) into the old core
    if(repProc)
      cpus.switchIn(runCpu,repProc);
    // Get the target process in the target cpu
    cpus.switchIn(cpu, newProc);
  }else{
    // If new process not ready, make it ready
    if(newProc->getState()!=ReadyState){
      // Make it prefer the target cpu
      newProc->setCPU(cpu);
      // Make it ready
      newProc->setState(InvalidState);
      cpus.makeRunnable(newProc);
      // The target cpu is prefered by the target process, and
      // the target cpu has available flows. Thus, the target
      // process should be now running on the target cpu
      I(newProc->getCPU()==cpu);
      I(newProc->getState()==RunningState);
    }else{
      // The new process is already ready, just switch it in
      cpus.switchIn(cpu,newProc);
    }
  }
  return oldPid;
}

void OSSim::SaveCheckpoint()
{
	if(SescConf->checkInt("","ShouldLoadCheckpoint") == false || SescConf->checkCharPtr("","CheckpointName") == false)
	{
		return;
	}
	if(SescConf->getInt("","ShouldLoadCheckpoint") == 1)
	{
		return;
	}
	ZlibOut out(SescConf->getCharPtr("","CheckpointName"));
	uint32_t procCount = getNumCPUs();
	std::cout << "Processor Count = " << procCount << std::endl;
	uint32_t allocCount = getContext(0)->getHeapManager()->GetAllocationCount();
	std::cout << "Allocation Count = " << allocCount << std::endl;
	uint32_t memorySize = Mem_size;
	std::cout << "Memory Size = " << memorySize << std::endl;
	uint32_t barrierCount = mint_barrier_set.size();
	std::cout << "Barrier set size = " << barrierCount << std::endl;

	out.Write((char*)&procCount,sizeof(procCount));
	out.Write((char*)&allocCount,sizeof(allocCount));
	out.Write((char*)&memorySize,sizeof(memorySize));
	out.Write((char*)&barrierCount,sizeof(barrierCount));
	for(HASH_MAP<uint32_t,mint_barrier_str>::iterator i = mint_barrier_set.begin(); i != mint_barrier_set.end(); i++)
	{
		uint32_t barrierAddr = i->first;
		out.Write((char*)&barrierAddr,sizeof(barrierAddr));
		uint32_t barrierPopulation = i->second.waitingSet.size();
		out.Write((char*)&barrierPopulation,sizeof(barrierAddr));
		for(HASH_SET<uint32_t>::iterator cpuid = i->second.waitingSet.begin(); cpuid != i->second.waitingSet.end(); cpuid++)
		{
			uint32_t cpu = *cpuid;
			out.Write((char*)&cpu,sizeof(cpu));
		}
	}
	for(uint32_t i = 0; i < procCount; i++)
	{
		int pidScheduled = cpus.getProcessor(i)->findVictimPid();
		out.Write((char*)&pidScheduled,sizeof(pidScheduled));
		if(pidScheduled != -1)
		{
			while(cpus.getProcessor(i)->getInstructionPointer(i)->next == NULL)
			{
				cpus.getProcessor(i)->goRabbitMode(1);//get it off of delay slot instructions
			}
			cpus.getProcessor(i)->switchOut(pidScheduled);
			cpus.getProcessor(i)->switchIn(pidScheduled);
			std::cout << "Pid:" << pidScheduled << std::endl;
			ThreadContext* context = ThreadContext::getContext(pidScheduled);
			int parentPid = (context->getParent())?(context->getParent()->getPid()):-1;
			unsigned int regValue[32];
			float fRegValue[32];
			unsigned int fpControl0,fpControl31;
			uint8_t stalling = (context->isBusyWaiting)?1:0;
			int lo = context->lo,hi = context->hi;
			int32_t runningState = ProcessId::getProcessId(pidScheduled)->getconf();

			for(int j = 0; j < 32; j++)
			{
				regValue[j] = context->getREGNUM(j);
				fRegValue[j] = context->getFPNUM(j);
			}
			fpControl0 = context->getFPUControl0();
			fpControl31 = context->getFPUControl31();
			unsigned int programCounter = (unsigned int)icode2addr(context->getPCIcode());
			unsigned int stackLb = context->getStackAddr(), 
				stackUb = context->getStackAddr() + context->getStackSize();
			out.Write((char*)&stackLb,sizeof(memorySize));
			out.Write((char*)&stackUb,sizeof(memorySize));
			out.Write((char*)&parentPid,sizeof(parentPid));
			out.Write((char*)&lo,sizeof(lo));
			out.Write((char*)&hi,sizeof(hi));
			out.Write((char*)&regValue[0],sizeof(unsigned int) * 32);
			out.Write((char*)&fRegValue[0],sizeof(float) * 32);
			out.Write((char*)&fpControl0,sizeof(fpControl0));
			out.Write((char*)&fpControl31,sizeof(fpControl31));
			out.Write((char*)&programCounter,sizeof(programCounter));
			out.Write((char*)&runningState,sizeof(runningState));
			out.Write((char*)&stalling,sizeof(stalling));
		}
	}
	out.Write((char*)Private_start,memorySize);
	HeapManager* hm = getContext(0)->getHeapManager();
	HeapManager::BlocksByAddr::iterator it = hm->GetAllocationIterator();
	for(unsigned int j = 0; j < allocCount; j++)
	{
		unsigned int startVAddr;
		size_t size;
		it = hm->GetAllocationDetails(it,startVAddr,size);
		out.Write((char*)&startVAddr, sizeof(startVAddr));
		out.Write((char*)&size, sizeof(size));
	}
	out.Close();
	std::cout << "Checkpoint saved" << std::endl;
	if(SescConf->checkInt("","DieAfterCheckpointTaken") == true && SescConf->getInt("","DieAfterCheckpointTaken") == 1)
	{
		std::cout << "Marker to die after checkpoint, shutting down" << std::endl;
		exit(0);
	}
}
bool OSSim::LoadCheckpoint()
{
	if(SescConf->checkInt("","ShouldLoadCheckpoint") == false || SescConf->checkCharPtr("","CheckpointName") == false)
	{
		return false;
	}
	if(SescConf->getInt("","ShouldLoadCheckpoint") == 0)
	{
		return false;
	}
	std::cout << "Loading checkpoint..." << std::endl;
	ZlibIn in(SescConf->getCharPtr("","CheckpointName"));
	uint32_t procCount;
	uint32_t allocCount;
	uint32_t memorySize;
	uint32_t barrierCount;

	in.Read((char*)&procCount,sizeof(procCount));
	in.Read((char*)&allocCount,sizeof(allocCount));
	in.Read((char*)&memorySize,sizeof(memorySize));
	// the following can sometimes catch when checkpoint files are missing
	I(memorySize == Mem_size);
	in.Read((char*)&barrierCount,sizeof(barrierCount));
	for(uint32_t i = 0; i < barrierCount; i++)
	{
		uint32_t barrierAddr;
		in.Read((char*)&barrierAddr,sizeof(barrierAddr));
		uint32_t barrierPopulation;
		in.Read((char*)&barrierPopulation,sizeof(barrierAddr));
		for(uint32_t cpuid = 0; cpuid != barrierPopulation; cpuid++)
		{
			uint32_t cpu;
			in.Read((char*)&cpu,sizeof(cpu));
			mint_barrier_set[barrierAddr].waitingSet.insert(cpu);
		}
	}


	for(unsigned short i = 0; i < procCount; i++)
	{
		int pidScheduled;
		in.Read((char*)&pidScheduled,sizeof(pidScheduled));
		if(pidScheduled != -1)
		{
			std::cout << "Pid: " << pidScheduled << std::endl;
			ThreadContext* tc;
			if(pidScheduled == 0)
				tc = ThreadContext::getContext(pidScheduled);
			else
				tc = ThreadContext::newActual();

			int parentPid;// = getContext(pidScheduled)->getParent()->getPid();
			unsigned int regValue[32];
			float fRegValue[32];
			unsigned int fpControl0,fpControl31;
			int lo,hi;
			int32_t runningState;
			uint8_t stalling;

			unsigned int programCounter;
			unsigned int stackLb, stackUb;
			in.Read((char*)&stackLb,sizeof(stackLb));
			in.Read((char*)&stackUb,sizeof(stackUb));
			in.Read((char*)&parentPid,sizeof(parentPid));
			in.Read((char*)&lo,sizeof(lo));
			in.Read((char*)&hi,sizeof(hi));
			in.Read((char*)&regValue[0],sizeof(unsigned int) * 32);
			in.Read((char*)&fRegValue[0],sizeof(float) * 32);
			in.Read((char*)&fpControl0,sizeof(fpControl0));
			in.Read((char*)&fpControl31,sizeof(fpControl31));
			in.Read((char*)&programCounter,sizeof(programCounter));
			in.Read((char*)&runningState,sizeof(runningState));
			in.Read((char*)&stalling,sizeof(stalling));

			if(pidScheduled != 0)
			{
				tc->shareAddrSpace(getContext(0),true,false);
				tc->init();
			}
			for(int j = 0; j < 32; j++)
			{
				tc->setREGNUM(j,regValue[j]);
				tc->setFPNUM(j,fRegValue[j]);
			}
			tc->setFPUControl0(fpControl0);
			tc->setFPUControl31(fpControl31);
			tc->lo = lo;
			tc->hi = hi;
			tc->setStack(stackLb,stackUb);
			tc->setPCIcode(addr2icode((intptr_t)programCounter));
			tc->isBusyWaiting = (stalling == 1);
			eventSpawn(parentPid,pidScheduled,runningState);
			contextSwitch(i,tc->getPid());
		}
	}
	std::cout << "Reading memory..." << std::endl;
	in.Read((char*)Private_start,memorySize);
	HeapManager* hm = getContext(0)->getHeapManager();
	for(unsigned int j = 0; j < allocCount; j++)
	{
		unsigned int startVAddr;
		size_t size;
		in.Read((char*)&startVAddr, sizeof(startVAddr));
		in.Read((char*)&size, sizeof(size));
		hm->ForceBlockPopulation(startVAddr, size);
	}
	in.Close();
	std::cout << "Checkpoint loaded" << std::endl;
	return true;
}

void OSSim::RabbitModeStart()
{
	rabbitModeCounter++;
}

void OSSim::RabbitModeStop()
{
   // 2010/08/28 hack to always stop rabbit mode
   //rabbitModeCounter = 0;
   
	rabbitModeCounter--;
	if(rabbitModeCounter < 0)
	{
		rabbitModeCounter = 0;
	}
}
