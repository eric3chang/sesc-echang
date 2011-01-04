/* 
   SESC: Super ESCalar simulator
   Copyright (C) 2003 University of Illinois.

   Contributed by Jose Renau
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

#include "DInst.h"

#include "Cluster.h"
#include "FetchEngine.h"
#include "OSSim.h"
#include "LDSTBuffer.h"
#include "Resource.h"

#if (defined TLS)
#include "Epoch.h"
#endif

pool<DInst> DInst::dInstPool(256, "DInst");

#ifdef DEBUG
int32_t DInst::currentID=0;
#endif


DInst::DInst()
  :doAtSimTimeCB(this)
  ,doAtSelectCB(this)
  ,doAtExecutedCB(this)
{
  pend[0].init(this);
  pend[1].init(this);
  I(MAX_PENDING_SOURCES==2);
  nDeps = 0;
}

void DInst::dump(const char *str)
{
  fprintf(stderr,"%s:(%d)  DInst: vaddr=0x%x ", str, cId, (int)vaddr);
  if (executed) {
    fprintf(stderr, " executed");
  }else if (issued) {
    fprintf(stderr, " issued");
  }else{
    fprintf(stderr, " non-issued");
  }
  if (hasPending())
    fprintf(stderr, " has pending");
  if (!isSrc1Ready())
    fprintf(stderr, " has src1 deps");
  if (!isSrc2Ready()) 
    fprintf(stderr, " has src2 deps");
  
  inst->dump("");
}

void DInst::doAtSimTime()
{
  I( resource );

  I(!isExecuted());

  I(resource->getCluster());

  if (!isStallOnLoad())
    resource->getCluster()->wakeUpDeps(this);

  resource->simTime(this);
}

void DInst::doAtSelect()
{
  I(resource->getCluster());
  resource->getCluster()->select(this);
}

void DInst::doAtExecuted()
{
  I(RATEntry);
  if ( (*RATEntry) == this )
    *RATEntry = 0;

  I( resource );
  resource->executed(this);
}


DInst *DInst::createDInst(const Instruction *inst, VAddr va, int32_t cId)
{
  DInst *i = dInstPool.out();

  i->instructionStamp = globalInstructionStampCounter;
  i->inst       = inst;
  i->cId        = cId;
  i->wakeUpTime = 0;
  i->vaddr      = va;
  i->first      = 0;
#ifdef DEBUG
  i->ID = currentID++;
#endif
  i->resource  = 0;
  i->RATEntry  = 0;
  i->pendEvent = 0;
  i->fetch = 0;
  i->loadForwarded= false;
  i->issued       = false;
  i->executed     = false;
  i->depsAtRetire = false;
  i->deadStore    = false;
  i->resolved     = false;
  i->deadInst     = false;
  i->waitOnMemory = false;
  i->pend[0].isUsed = false;
  i->pend[1].isUsed = false;
    
  return i;
}

DInst *DInst::createInst(InstID pc, VAddr va, int32_t cId)
{
  const Instruction *inst = Instruction::getInst(pc);
  return createDInst(inst, va, cId);
}

DInst *DInst::clone() 
{
  DInst *newDInst = createDInst(inst, vaddr, cId);
  return newDInst;
}

void DInst::killSilently()
{
  I(getPendEvent()==0);
  I(getResource()==0);

  markIssued();
  markExecuted();
  if( getFetch() ) {
    getFetch()->unBlockFetch();
    IS(setFetch(0));
  }

  if (getInst()->isStore())
    LDSTBuffer::storeLocallyPerformed(this);
 
  while (hasPending()) {
    DInst *dstReady = getNextPending();

    if (!dstReady->isIssued()) {
      // Accross processor dependence
      if (dstReady->hasDepsAtRetire())
        dstReady->clearDepsAtRetire();
      
      I(!dstReady->hasDeps());
      continue;
    }
    if (dstReady->isExecuted()) {
      // The instruction got executed even though it has dependences. This is
      // because the instruction got silently killed (killSilently)
      if (!dstReady->hasDeps())
        dstReady->scrap();
      continue;
    }

    if (!dstReady->hasDeps()) {
      I(dstReady->isIssued());
      I(!dstReady->isExecuted());
      Resource *dstRes = dstReady->getResource();
      I(dstRes);
      dstRes->simTime(dstReady);
    }
  }

  I(!getFetch());

  if (hasDeps())
    return;
  
  I(nDeps == 0);   // No deps src

  I(!getFetch());
  dInstPool.in(this); 
}

void DInst::scrap()
{
  I(nDeps == 0);   // No deps src
  I(first == 0);   // no dependent instructions 

  I(!getFetch());
  dInstPool.in(this);
}

void DInst::destroy()
{
  I(nDeps == 0);   // No deps src

  I(!fetch); // if it block the fetch engine. it is unblocked again

  I(issued);
  I(executed);

  awakeRemoteInstructions();

  I(first == 0);   // no dependent instructions 
  if (first)
    LOG("Instruction pc=0x%x failed first is pc=0x%x",(int)inst->getAddr(),(int)first->getDInst()->inst->getAddr());

  scrap();
}

void DInst::awakeRemoteInstructions() 
{
  while (hasPending()) {
    DInst *dstReady = getNextPending();

    I(inst->isStore());
    I( dstReady->inst->isLoad());
    I(!dstReady->isExecuted());
    I( dstReady->hasDepsAtRetire());

    I( dstReady->isSrc2Ready()); // LDSTBuffer queue in src2, free by now

    dstReady->clearDepsAtRetire();
    if (dstReady->isIssued() && !dstReady->hasDeps()) {
      Resource *dstRes = dstReady->getResource();
      // Coherence would add the latency because the cache line must be brought
      // again (in theory it must be local to dinst processor and marked dirty
      I(dstRes); // since isIssued it should have a resource
      dstRes->simTime(dstReady);
    }
  }
}

#ifdef SESC_BAAD
void DInst::setFetch1Time()
{
  I(fetch1Time == 0);
  fetch1Time = globalClock;

  fetch1QHistUp->sample(fetch1QSize);
  fetch1QSize++;

  fetch1QHist1->sample(fetch1QSize);  
  fetch1QHist2->sample(fetch1QSize);  
}

void DInst::setFetch2Time()
{
  I(fetch2Time == 0);
  fetch2Time = globalClock;

  fetch1QHistDown->sample(fetch1QSize);
  fetch1QSize--;
  fetch2QHistUp->sample(fetch2QSize);
  fetch2QSize++;

  fetch1QHist2->sample(fetch1QSize);  

  fetch2QHist1->sample(fetch2QSize);  
  fetch2QHist2->sample(fetch2QSize);  
}

void DInst::setRenameTime()
{
  I(renameTime == 0);
  renameTime = globalClock;

  fetch2QHistDown->sample(fetch2QSize);
  fetch2QSize--;
  issueQHistUp->sample(issueQSize);
  issueQSize++;

  fetch2QHist2->sample(fetch2QSize);  

  issueQHist1->sample(issueQSize);  
  issueQHist2->sample(issueQSize);  
}

void DInst::setIssueTime()
{
  I(issueTime == 0);
  issueTime = globalClock;

  issueQHistDown->sample(issueQSize);
  issueQSize--;

  schedQHistUp->sample(schedQSize);
  schedQSize++;

  issueQHist2->sample(issueQSize);  

  schedQHist1->sample(schedQSize);  
  schedQHist2->sample(schedQSize);  
}

void DInst::setSchedTime()
{
  I(schedTime == 0);
  schedTime = globalClock;

  schedQHistDown->sample(schedQSize);
  schedQSize--;

  exeQHistUp->sample(exeQSize);
  exeQSize++;

  schedQHist2->sample(schedQSize);  

  exeQHist1->sample(exeQSize);
  exeQHist2->sample(exeQSize);
}

void DInst::setExeTime()
{
  I(exeTime == 0);
  exeTime = globalClock;

  exeQHistDown->sample(exeQSize);
  exeQSize--;

  retireQHistUp->sample(retireQSize);
  retireQSize++;

  exeQHist2->sample(exeQSize);

  retireQHist1->sample(retireQSize);
  retireQHist2->sample(retireQSize);
}

void DInst::setRetireTime()
{
  I(fetch1Time);
  I(fetch2Time);
  I(renameTime);
  I(issueTime);
  I(schedTime);
  I(exeTime);

  InstType i = inst->getOpcode();
  // Based on instruction type keep statistics
  avgFetch1QTime[i]->sample(fetch2Time-fetch1Time);
  avgFetch2QTime[i]->sample(renameTime-fetch2Time);
  avgIssueQTime[i]->sample(issueTime-renameTime);
  avgSchedQTime[i]->sample(schedTime-issueTime);
  avgExeQTime[i]->sample(exeTime-schedTime);
  avgRetireQTime[i]->sample(globalClock-exeTime);

  retireQHistDown->sample(retireQSize);
  retireQSize--;


  static int32_t nInsts = 0;
  nInsts++;
  if (getFetch()) {
    // Instruction that triggered a branch miss
    brdistHist1->sample(nInsts);
    nInsts = 0;
  }

#if 0
  int32_t pc = inst->getAddr();
  if (pc) {
#ifdef SESC_BAAD
    printf("BAAD: f1T=%lld f2T=%lld rT=%lld xT=%lld cT=%lld pc=0x%x op=%d:%d s1=%d s2=%d d=%u "
           ,fetch1Time
           ,fetch2Time
           ,renameTime
           ,exeTime
           ,globalClock
           ,pc,inst->getOpcode(),inst->getSubCode()
           ,inst->getSrc1(), inst->getSrc2(), inst->getDest()
           );

    if (inst->isMemory())
      printf(" nmB a=0x%x t=%d", getVaddr(), (int)(exeTime-schedTime));
    else if (getFetch())
      printf(" mB");
    else
      printf(" nmB");
#else
    printf("TR: %lld dest=%3d src1=%3d src2=%3d lat=%3d "
	   ,renameTime
           ,preg, pend[0].preg, pend[1].preg
           ,(int)(exeTime-schedTime));

    if (inst->getOpcode() == iALU)
      printf(" unit=0");
    else if (inst->getOpcode() == iMult)
      printf(" unit=1");
    else if (inst->getOpcode() == iDiv)
      printf(" unit=1");
    else if (inst->getOpcode() == iBJ)
      printf(" unit=2");
    else if (inst->getOpcode() == iLoad)
      printf(" unit=3");
    else if (inst->getOpcode() == iStore)
      printf(" unit=3");
    else
      printf(" unit=1");

    if (getFetch())
      printf(" FLUSH");
#endif

    printf("\n");
  }
#endif
}
#endif

