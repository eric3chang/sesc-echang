/* 
   SESC: Super ESCalar simulator
   Copyright (C) 2003 University of Illinois.

   Contributed by Jose Renau
                  Milos Prvulovic
                  James Tuck
                  Luis Ceze

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
#include <iostream>
#include "OSSim.h"
#include "ExecutionFlow.h"
#include "DInst.h"
#include "GMemoryOS.h"
#include "TraceGen.h"
#include "GMemorySystem.h"
#include "MemRequest.h"
#include "TMInterface.h"
#include "GProcessor.h"


#include "mintapi.h"
#include "opcodes.h"

using std::cout;
using std::endl;

ExecutionFlow::ExecutionFlow(int32_t cId, int32_t i, GMemorySystem *gmem)
  : GFlow(i, cId, gmem)
{
	simOpRabbitModeCounter = 0;
  picodePC = 0;
  ev = NoEvent;

  goingRabbit = false;

  // Copy the address space from initial context
  memcpy(&thread,ThreadContext::getMainThreadContext(),sizeof(ThreadContext));

  thread.setPid(-1);
  thread.setPicode(&invalidIcode);

  pendingDInst = 0;
}

int32_t ExecutionFlow::exeInst(void)
{
  // Instruction address
  int32_t iAddr = picodePC->addr;
  // Instruction flags
  short opflags=picodePC->opflags;
  // For load/store instructions, will contain the virtual address of data
  // For other instuctions, set to non-zero to return success at the end
  VAddr dAddrV=1;
  // For load/store instructions, the Real address of data
  // The Real address is the actual address in the simulator
  //   where the data is found. With versioning, the Real address
  //   actually points to the correct version of the data item
  RAddr dAddrR=0;
  
  // For load/store instructions, need to translate the data address
  if(opflags&E_MEM_REF) {
    // Get the Virtual address
         dAddrV = (*((int32_t *)&thread.reg[picodePC->args[RS]])) + picodePC->immed;
    // Get the Real address
		dAddrR = thread.virt2real(dAddrV, opflags);
	// Put the Real data address in the thread structure
    thread.setRAddr(dAddrR);
  }

  I(!thread.IsBusyWaiting());

  picodePC=(picodePC->func)(picodePC, &thread);
  I(picodePC);
  I(picodePC->addr != iAddr || thread.IsBusyWaiting());

  return (dAddrR)?dAddrV:1;
}

void ExecutionFlow::exeInstFast()
{
  I(goingRabbit);
  // This exeInstFast can be called when for sure there are no speculative
  int32_t iAddr   =picodePC->addr;
  short iFlags=picodePC->opflags;

	globalInstructionStampCounter++;

  if(iFlags&E_MEM_REF) {
    VAddr vaddr = (*((int32_t *)&thread.reg[picodePC->args[RS]])) + picodePC->immed;
    thread.setRAddr(thread.virt2real(vaddr, iFlags));
  }
#ifdef DEBUG_DUMP_INSTR
  std::cout << picodePC->dis_instr();
#endif
  if(picodePC->addr == 268494272)
	  std::cout <<  thread.getPid() << " Has reached SESC_EXIT" << std::endl;
    picodePC=(picodePC->func)(picodePC, &thread);
	I(iAddr != picodePC->addr);
}

void ExecutionFlow::switchIn(int32_t i)
{
  I(thread.getPid() == -1);
  I(thread.getPicode()==&invalidIcode);
  loadThreadContext(i);
  I(thread.getPid() == i);

  if( pendingDInst ) {
    pendingDInst->scrap();
    pendingDInst = 0;
  }
}

void ExecutionFlow::switchOut(int32_t i)
{
//  LOG("ExecutionFlow[%d] switchOut pid(%d) 0x%x @%lld", fid, i, picodePC->addr, globalClock);
  // Must be running thread i
  I(thread.getPid() == i);
  // Save the context of the thread
  saveThreadContext(i);
  // Now running no thread at all
  thread.setPid(-1);
  thread.setPicode(&invalidIcode);

  //  I(!pendingDInst);
  if( pendingDInst ) {
    pendingDInst->scrap();
    pendingDInst = 0;
  }
}

void ExecutionFlow::dump(const char *str) const
{
  if(picodePC == 0) {
    MSG("Flow(%d): context not ready", fid);
    return;
  }

  LOG("Flow(%d):pc=0x%x:%s", fid, (int)getPCInst()    // (int) because fake warning in gcc
      , str);

  LOG("Flowd:id=%3d:addr=0x%x:%s", fid, static_cast < unsigned >(picodePC->addr),
      str);
}

DInst *ExecutionFlow::executePC()
{
	TMInterface::MarkInstructionExecution((int)thread.getPid());
  // If there is an already executed instruction, just return it
  if(pendingDInst) {
    DInst *dinst = pendingDInst;
    pendingDInst = 0;
    return dinst;
  }
  DInst *dinst=0;
  // We will need the original picodePC later
  icode_ptr origPIcode=picodePC;
  I(picodePC);
  // Need to get the pid here because exeInst may switch to another thread
  Pid_t     origPid=thread.getPid();
  // Determine whether the instruction has a delay slot
  bool hasDelay= ((picodePC->opflags) == E_UFUNC);
  bool isEvent=false;

  // Library calls always have a delay slot and the target precalculated
  if(hasDelay && picodePC->target) {
    // If it is an event, set the flag
    isEvent=Instruction::getInst(picodePC->target->instID)->isEvent();
  }
  // Remember the ID of the instruction
  InstID cPC = getPCInst();

  int32_t vaddr = exeInst();

  dinst=DInst::createInst(cPC
                          ,vaddr
                          ,fid
							);
	globalInstructionStampCounter++;

  TMInterface::ProcessAction(dinst->instructionStamp);
	// If no delay slot, just return the instruction
  if(!hasDelay) {
    return dinst;
  }


  // The delay slot must execute
  I(thread.getPid()==origPid);
  I(getPCInst()==cPC+1);
  cPC = getPCInst();
  vaddr = exeInst();
  if(vaddr==0) {
    dinst->scrap();
    return 0;
  }

  I(vaddr);
  I(pendingDInst==0);
  // If a non-event branch with a delay slot, we reverse the order of the branch
  // and its delay slot, and first return the delay slot. The branch is still in
  // "pendingDInst".
  pendingDInst=dinst;
  I(pendingDInst->getInst()->getAddr());

  dinst = DInst::createInst(cPC
                            ,vaddr
                            ,fid
							);
  	globalInstructionStampCounter++;

  TMInterface::ProcessAction(dinst->instructionStamp);


  if(!isEvent) {
    // Reverse the order between the delay slot and the iBJ
    return dinst;
  }

  // Execute the actual event (but do not time it)
  I(thread.getPid()==origPid);
  //TODO 2010/08/28 Eric
  /*
  cout << "ExecutionFlow::executePC: globalInstructionStampCounter="
        << globalInstructionStampCounter << endl;
        */
  vaddr = exeInst();
  I(vaddr);
  if( ev == NoEvent ) {
    // This was a PreEvent (not notified see Event.cpp)
    // Only the delay slot instruction is "visible"
    return dinst;
  }

  I(ev != PreEvent);
  if(ev == FastSimBeginEvent ) {
    goRabbitMode();
    ev = NoEvent;
    return dinst;
  }
  cout << "FastSimBeginEvent" << endl;
  I(ev != FastSimBeginEvent);
  I(ev != FastSimEndEvent);
  I(pendingDInst);
  // In the case of the event, the original iBJ dissapears
  pendingDInst->scrap();
  // Create a fake dynamic instruction for the event
  pendingDInst = DInst::createInst(Instruction::getEventID(ev)
                                   ,evAddr
                                   ,fid
								   );
  	globalInstructionStampCounter++;

	TMInterface::ProcessAction(pendingDInst->instructionStamp);

  if( evCB ) {
    pendingDInst->addEvent(evCB);
    evCB = NULL;
  }
  ev=NoEvent;
  // Return the delay slot instruction. The fake event instruction will come next.
  return dinst;
}

void ExecutionFlow::goRabbitMode(long long n2skip)
{
#ifdef DEBUG_PROC_DIAG
	gbCurrentPid = thread.getPid();
#endif
  int32_t nFastSims = 0;
  if( ev == FastSimBeginEvent ) {
    // Can't train cache in those cases. Cache only be train if the
    // processor did not even started to execute instructions
    trainCache = 0;
    nFastSims++;
  }

  if (n2skip) {
    goingRabbit++;
  }

  nExec=0;
  do {
    ev=NoEvent;
    if( n2skip > 0 )
      n2skip--;

    nExec++;

    if (goingRabbit)
      exeInstFast();
    else {
      exeInst();
    }

    if(thread.getPid() == -1) {
      ev=NoEvent;
      goingRabbit = false;
      return;
    }
    
    if( ev ) {
      if( evCB )
        evCB->call();
      else{
        // Those kind of events have no callbacks because they
        // go through the memory backend. Since we are in
        // FastMode and everything happens atomically, we
        // don't need to notify the backend.
        I(ev == ReleaseEvent ||
          ev == AcquireEvent ||
          ev == MemFenceEvent||
          ev == FetchOpEvent );
      }
    }

    if( n2skip == 0 && goingRabbit && nFastSims == 0 )
      break;
  }while(nFastSims || n2skip || goingRabbit);


  ev=NoEvent;

  goingRabbit--;
}

icode_ptr ExecutionFlow::getInstructionPointer(void)
{
  return picodePC; 
}

void ExecutionFlow::setInstructionPointer(icode_ptr picode) 
{ 
  thread.setPicode(picode);
  picodePC=picode;
}
void ExecutionFlow::RaceAhead()
{
	while(TMInterface::MayRaceAhead(thread.getPid()) && !thread.IsBusyWaiting())
	{
		TMInterface::MarkInstructionExecution((int)thread.getPid());
		DInst* d = executePC();
		I(d);
		d->scrap();
	}
}

