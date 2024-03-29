# GNU Make project makefile autogenerated by Premake
ifndef config
  config=linux
endif

ifndef verbose
  SILENT = @
endif

ifndef CC
  CC = gcc
endif

ifndef CXX
  CXX = g++
endif

ifndef AR
  AR = ar
endif

ifeq ($(config),linux)
  OBJDIR     = obj/linux
  TARGETDIR  = .
  TARGET     = $(TARGETDIR)/WinSesc
  DEFINES   += -DLINUX
  INCLUDES  += -IDebug -Ilibmint -Ilibsesctherm -Ilibcore -Ilibsescspot -Ilibpint -Ilibll -Iobj -Ilibpower -Ilibsmp -Ilibvmem -Ilibnet -IRelease -Imisc -Ilibsuperlu -Ilibtm -Ilibapp -IMemSys -Ilibrst -IWorkingFolder -Ilibsuc -Ilibtrans -Ilibmem -Ilibemul -I../../../PersonalTools/includes
  CPPFLAGS  += -MMD -MP $(DEFINES) $(INCLUDES)
  CFLAGS    += $(CPPFLAGS) $(ARCH) -Wno-deprecated
  CXXFLAGS  += $(CFLAGS) 
  LDFLAGS   += -s
  LIBS      += 
  RESFLAGS  += $(DEFINES) $(INCLUDES) 
  LDDEPS    += 
  LINKCMD    = $(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(ARCH) $(LIBS)
  define PREBUILDCMDS
  endef
  define PRELINKCMDS
  endef
  define POSTBUILDCMDS
  endef
endif

OBJECTS := \
	$(OBJDIR)/utils.o \
	$(OBJDIR)/subst.o \
	$(OBJDIR)/mint_init.o \
	$(OBJDIR)/dump.o \
	$(OBJDIR)/opcodes.o \
	$(OBJDIR)/error_ops.o \
	$(OBJDIR)/exec.o \
	$(OBJDIR)/coproc.o \
	$(OBJDIR)/symtab.o \
	$(OBJDIR)/mendian.o \
	$(OBJDIR)/icode.o \
	$(OBJDIR)/non_mips.o \
	$(OBJDIR)/ops.o \
	$(OBJDIR)/AddressSpace.o \
	$(OBJDIR)/elf.o \
	$(OBJDIR)/sesctherm3Dregression.o \
	$(OBJDIR)/sesctherm3Dconfig.o \
	$(OBJDIR)/sesctherm3Dutil.o \
	$(OBJDIR)/sesctherm3Dchip.o \
	$(OBJDIR)/sesctherm3Dmodel.o \
	$(OBJDIR)/sesctherm3Ddatalibrary.o \
	$(OBJDIR)/sesctherm3Ddtm.o \
	$(OBJDIR)/sescthermPCB.o \
	$(OBJDIR)/sesctherm3Dgraphics.o \
	$(OBJDIR)/sesctherm.o \
	$(OBJDIR)/sesctherm3Ddebug.o \
	$(OBJDIR)/sesctherm3Ducool.o \
	$(OBJDIR)/sesctherm3Dlayerinfo.o \
	$(OBJDIR)/sesctherm3Dmaterial.o \
	$(OBJDIR)/sesctherm3Dbase.o \
	$(OBJDIR)/BPred.o \
	$(OBJDIR)/Profile.o \
	$(OBJDIR)/EnergyMgr.o \
	$(OBJDIR)/ValueTable.o \
	$(OBJDIR)/SMTProcessor.o \
	$(OBJDIR)/main.o \
	$(OBJDIR)/ProcessId.o \
	$(OBJDIR)/FetchEngine.o \
	$(OBJDIR)/Processor.o \
	$(OBJDIR)/HVersion.o \
	$(OBJDIR)/TaskHandler.o \
	$(OBJDIR)/RiskLoadProf.o \
	$(OBJDIR)/GMemorySystem.o \
	$(OBJDIR)/OSSim.o \
	$(OBJDIR)/DInst.o \
	$(OBJDIR)/RunningProcs.o \
	$(OBJDIR)/MemRequest.o \
	$(OBJDIR)/Pipeline.o \
	$(OBJDIR)/Cluster.o \
	$(OBJDIR)/ParProf.o \
	$(OBJDIR)/GLVID.o \
	$(OBJDIR)/SysCall.o \
	$(OBJDIR)/MemObj.o \
	$(OBJDIR)/DepWindow.o \
	$(OBJDIR)/GProcessor.o \
	$(OBJDIR)/LDSTBuffer.o \
	$(OBJDIR)/Epoch.o \
	$(OBJDIR)/TaskContext.o \
	$(OBJDIR)/VPred.o \
	$(OBJDIR)/GMemoryOS.o \
	$(OBJDIR)/Signature.o \
	$(OBJDIR)/MemBuffer.o \
	$(OBJDIR)/ASVersion.o \
	$(OBJDIR)/Checkpoint.o \
	$(OBJDIR)/LDSTQ.o \
	$(OBJDIR)/AdvancedStats.o \
	$(OBJDIR)/Resource.o \
	$(OBJDIR)/shape.o \
	$(OBJDIR)/temperature.o \
	$(OBJDIR)/temperature_grid.o \
	$(OBJDIR)/wire.o \
	$(OBJDIR)/flp.o \
	$(OBJDIR)/flp_desc.o \
	$(OBJDIR)/FBlocks.o \
	$(OBJDIR)/util.o \
	$(OBJDIR)/sescspot.o \
	$(OBJDIR)/RCutil.o \
	$(OBJDIR)/npe.o \
	$(OBJDIR)/Grids.o \
	$(OBJDIR)/temperature_block.o \
	$(OBJDIR)/PPCDecoder.o \
	$(OBJDIR)/TraceFlow.o \
	$(OBJDIR)/RSTFlow.o \
	$(OBJDIR)/QemuSescReader.o \
	$(OBJDIR)/MIPSInstruction.o \
	$(OBJDIR)/GFlow.o \
	$(OBJDIR)/QEMUFlow.o \
	$(OBJDIR)/ExecutionFlow.o \
	$(OBJDIR)/TraceReader.o \
	$(OBJDIR)/Events.o \
	$(OBJDIR)/TT6Reader.o \
	$(OBJDIR)/HeapManager.o \
	$(OBJDIR)/RSTIntruction.o \
	$(OBJDIR)/PPCInstruction.o \
	$(OBJDIR)/SPARCInstruction.o \
	$(OBJDIR)/QemuSparcInstruction.o \
	$(OBJDIR)/RSTReader.o \
	$(OBJDIR)/ThreadContext.o \
	$(OBJDIR)/Instruction.o \
	$(OBJDIR)/powermain.o \
	$(OBJDIR)/cacmain.o \
	$(OBJDIR)/wattch_setup.o \
	$(OBJDIR)/cacti_setup.o \
	$(OBJDIR)/orion_setup.o \
	$(OBJDIR)/SMPProtocol.o \
	$(OBJDIR)/SMPSystemBus.o \
	$(OBJDIR)/SMPMemRequest.o \
	$(OBJDIR)/MESIProtocol.o \
	$(OBJDIR)/SMPCache.o \
	$(OBJDIR)/smp.o \
	$(OBJDIR)/SMemorySystem.o \
	$(OBJDIR)/RoutingPolicy.o \
	$(OBJDIR)/ProtocolBase.o \
	$(OBJDIR)/RoutingTable.o \
	$(OBJDIR)/Message.o \
	$(OBJDIR)/NetAddrMap.o \
	$(OBJDIR)/Router.o \
	$(OBJDIR)/InterConn.o \
	$(OBJDIR)/PMessage.o \
	$(OBJDIR)/poolBench.o \
	$(OBJDIR)/netBench.o \
	$(OBJDIR)/rstexample.o \
	$(OBJDIR)/mkdep.o \
	$(OBJDIR)/CacheCoreBench.o \
	$(OBJDIR)/TMMemRequest.o \
	$(OBJDIR)/MESIProtocol.o \
	$(OBJDIR)/TMmain.o \
	$(OBJDIR)/TMProtocol.o \
	$(OBJDIR)/TMSystemBus.o \
	$(OBJDIR)/TMCache.o \
	$(OBJDIR)/TMMemorySystem.o \
	$(OBJDIR)/EventManager.o \
	$(OBJDIR)/MemorySystem.o \
	$(OBJDIR)/Connection.o \
	$(OBJDIR)/BaseMemDevice.o \
	$(OBJDIR)/TestMemory.o \
	$(OBJDIR)/SnoopyBus.o \
	$(OBJDIR)/MOESICache.o \
	$(OBJDIR)/SESCProcessorInterface.o \
	$(OBJDIR)/rz3iu.o \
	$(OBJDIR)/rstzip3.o \
	$(OBJDIR)/decompress_engine.o \
	$(OBJDIR)/cpuid.o \
	$(OBJDIR)/compress_engine.o \
	$(OBJDIR)/rz3_section.o \
	$(OBJDIR)/Rstzip.o \
	$(OBJDIR)/rstzip2if.o \
	$(OBJDIR)/Config.o \
	$(OBJDIR)/ThermTrace.o \
	$(OBJDIR)/SCTable.o \
	$(OBJDIR)/GStats.o \
	$(OBJDIR)/MSHR.o \
	$(OBJDIR)/conflexlex.o \
	$(OBJDIR)/callback.o \
	$(OBJDIR)/Snippets.o \
	$(OBJDIR)/GEnergy.o \
	$(OBJDIR)/Port.o \
	$(OBJDIR)/TraceGen.o \
	$(OBJDIR)/BloomFilter.o \
	$(OBJDIR)/ReportGen.o \
	$(OBJDIR)/CacheCore.o \
	$(OBJDIR)/nanassert.o \
	$(OBJDIR)/SescConf.o \
	$(OBJDIR)/zlibStreams.o \
	$(OBJDIR)/ReportTherm.o \
	$(OBJDIR)/TQueue.o \
	$(OBJDIR)/conflex.tab.o \
	$(OBJDIR)/TMMemory.o \
	$(OBJDIR)/PerfectCollisionManager.o \
	$(OBJDIR)/LogTMSECollisionDetection.o \
	$(OBJDIR)/CollisionDetectionExcludeAllBut.o \
	$(OBJDIR)/TMProcessor.o \
	$(OBJDIR)/PerfectVersioning.o \
	$(OBJDIR)/TMInterface.o \
	$(OBJDIR)/PerfectCollisionDetection.o \
	$(OBJDIR)/LogTMCollisionDetection.o \
	$(OBJDIR)/MemCtrl.o \
	$(OBJDIR)/Bus.o \
	$(OBJDIR)/Bank.o \
	$(OBJDIR)/PriorityBus.o \
	$(OBJDIR)/UglyMemRequest.o \
	$(OBJDIR)/MarkovPrefetcher.o \
	$(OBJDIR)/MemoryOS.o \
	$(OBJDIR)/AddressPrefetcher.o \
	$(OBJDIR)/AlwaysPrefetch.o \
	$(OBJDIR)/Cache.o \
	$(OBJDIR)/MemorySystem_Old.o \
	$(OBJDIR)/TLB.o \
	$(OBJDIR)/mtst1.o \
	$(OBJDIR)/TaggedPrefetcher.o \
	$(OBJDIR)/StridePrefetcher.o \
	$(OBJDIR)/ElfObject.o \
	$(OBJDIR)/FileSys.o \
	$(OBJDIR)/EmulInit.o \
	$(OBJDIR)/LinuxSys.o \
	$(OBJDIR)/SignalHandling.o \
	$(OBJDIR)/InstDesc.o \
	$(OBJDIR)/AddressSpace.o \
	$(OBJDIR)/X86InstDesc.o \

RESOURCES := \

SHELLTYPE := msdos
ifeq (,$(ComSpec)$(COMSPEC))
  SHELLTYPE := posix
endif
ifeq (/bin,$(findstring /bin,$(SHELL)))
  SHELLTYPE := posix
endif

.PHONY: clean prebuild prelink

all: $(TARGETDIR) $(OBJDIR) prebuild prelink $(TARGET)

$(TARGET): $(GCH) $(OBJECTS) $(LDDEPS) $(RESOURCES)
	@echo Linking WinSesc
	$(SILENT) $(LINKCMD)
	$(POSTBUILDCMDS)

$(TARGETDIR):
	@echo Creating $(TARGETDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(TARGETDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(TARGETDIR))
endif

$(OBJDIR):
	@echo Creating $(OBJDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(OBJDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(OBJDIR))
endif

clean:
	@echo Cleaning WinSesc
ifeq (posix,$(SHELLTYPE))
	$(SILENT) rm -f  $(TARGET)
	$(SILENT) rm -rf $(OBJDIR)
else
	$(SILENT) if exist $(subst /,\\,$(TARGET)) del $(subst /,\\,$(TARGET))
	$(SILENT) if exist $(subst /,\\,$(OBJDIR)) rmdir /s /q $(subst /,\\,$(OBJDIR))
endif

prebuild:
	$(PREBUILDCMDS)

prelink:
	$(PRELINKCMDS)

ifneq (,$(PCH))
$(GCH): $(PCH)
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
endif

$(OBJDIR)/utils.o: libmint/utils.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/subst.o: libmint/subst.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/mint_init.o: libmint/mint_init.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/dump.o: libmint/dump.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/opcodes.o: libmint/opcodes.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/error_ops.o: libmint/error_ops.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/exec.o: libmint/exec.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/coproc.o: libmint/coproc.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/symtab.o: libmint/symtab.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/mendian.o: libmint/mendian.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/icode.o: libmint/icode.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/non_mips.o: libmint/non_mips.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/ops.o: libmint/ops.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/AddressSpace.o: libmint/AddressSpace.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/elf.o: libmint/elf.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/sesctherm3Dregression.o: libsesctherm/sesctherm3Dregression.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/sesctherm3Dconfig.o: libsesctherm/sesctherm3Dconfig.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/sesctherm3Dutil.o: libsesctherm/sesctherm3Dutil.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/sesctherm3Dchip.o: libsesctherm/sesctherm3Dchip.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/sesctherm3Dmodel.o: libsesctherm/sesctherm3Dmodel.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/sesctherm3Ddatalibrary.o: libsesctherm/sesctherm3Ddatalibrary.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/sesctherm3Ddtm.o: libsesctherm/sesctherm3Ddtm.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/sescthermPCB.o: libsesctherm/sescthermPCB.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/sesctherm3Dgraphics.o: libsesctherm/sesctherm3Dgraphics.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/sesctherm.o: libsesctherm/sesctherm.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/sesctherm3Ddebug.o: libsesctherm/sesctherm3Ddebug.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/sesctherm3Ducool.o: libsesctherm/sesctherm3Ducool.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/sesctherm3Dlayerinfo.o: libsesctherm/sesctherm3Dlayerinfo.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/sesctherm3Dmaterial.o: libsesctherm/sesctherm3Dmaterial.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/sesctherm3Dbase.o: libsesctherm/sesctherm3Dbase.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/BPred.o: libcore/BPred.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/Profile.o: libcore/Profile.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/EnergyMgr.o: libcore/EnergyMgr.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/ValueTable.o: libcore/ValueTable.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/SMTProcessor.o: libcore/SMTProcessor.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/main.o: libcore/main.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/ProcessId.o: libcore/ProcessId.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/FetchEngine.o: libcore/FetchEngine.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/Processor.o: libcore/Processor.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/HVersion.o: libcore/HVersion.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/TaskHandler.o: libcore/TaskHandler.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/RiskLoadProf.o: libcore/RiskLoadProf.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/GMemorySystem.o: libcore/GMemorySystem.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/OSSim.o: libcore/OSSim.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/DInst.o: libcore/DInst.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/RunningProcs.o: libcore/RunningProcs.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/MemRequest.o: libcore/MemRequest.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/Pipeline.o: libcore/Pipeline.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/Cluster.o: libcore/Cluster.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/ParProf.o: libcore/ParProf.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/GLVID.o: libcore/GLVID.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/SysCall.o: libcore/SysCall.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/MemObj.o: libcore/MemObj.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/DepWindow.o: libcore/DepWindow.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/GProcessor.o: libcore/GProcessor.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/LDSTBuffer.o: libcore/LDSTBuffer.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/Epoch.o: libcore/Epoch.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/TaskContext.o: libcore/TaskContext.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/VPred.o: libcore/VPred.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/GMemoryOS.o: libcore/GMemoryOS.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/Signature.o: libcore/Signature.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/MemBuffer.o: libcore/MemBuffer.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/ASVersion.o: libcore/ASVersion.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/Checkpoint.o: libcore/Checkpoint.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/LDSTQ.o: libcore/LDSTQ.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/AdvancedStats.o: libcore/AdvancedStats.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/Resource.o: libcore/Resource.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/shape.o: libsescspot/shape.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/temperature.o: libsescspot/temperature.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/temperature_grid.o: libsescspot/temperature_grid.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/wire.o: libsescspot/wire.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/flp.o: libsescspot/flp.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/flp_desc.o: libsescspot/flp_desc.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/FBlocks.o: libsescspot/FBlocks.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/util.o: libsescspot/util.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/sescspot.o: libsescspot/sescspot.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/RCutil.o: libsescspot/RCutil.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/npe.o: libsescspot/npe.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/Grids.o: libsescspot/Grids.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/temperature_block.o: libsescspot/temperature_block.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/PPCDecoder.o: libpint/PPCDecoder.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/TraceFlow.o: libll/TraceFlow.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/RSTFlow.o: libll/RSTFlow.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/QemuSescReader.o: libll/QemuSescReader.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/MIPSInstruction.o: libll/MIPSInstruction.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/GFlow.o: libll/GFlow.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/QEMUFlow.o: libll/QEMUFlow.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/ExecutionFlow.o: libll/ExecutionFlow.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/TraceReader.o: libll/TraceReader.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/Events.o: libll/Events.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/TT6Reader.o: libll/TT6Reader.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/HeapManager.o: libll/HeapManager.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/RSTIntruction.o: libll/RSTIntruction.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/PPCInstruction.o: libll/PPCInstruction.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/SPARCInstruction.o: libll/SPARCInstruction.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/QemuSparcInstruction.o: libll/QemuSparcInstruction.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/RSTReader.o: libll/RSTReader.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/ThreadContext.o: libll/ThreadContext.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/Instruction.o: libll/Instruction.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/powermain.o: libpower/powermain.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/cacmain.o: libpower/cacmain.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/wattch_setup.o: libpower/wattch/wattch_setup.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/cacti_setup.o: libpower/cacti/cacti_setup.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/orion_setup.o: libpower/orion/orion_setup.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/SMPProtocol.o: libsmp/SMPProtocol.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/SMPSystemBus.o: libsmp/SMPSystemBus.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/SMPMemRequest.o: libsmp/SMPMemRequest.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/MESIProtocol.o: libsmp/MESIProtocol.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/SMPCache.o: libsmp/SMPCache.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/smp.o: libsmp/smp.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/SMemorySystem.o: libsmp/SMemorySystem.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/RoutingPolicy.o: libnet/RoutingPolicy.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/ProtocolBase.o: libnet/ProtocolBase.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/RoutingTable.o: libnet/RoutingTable.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/Message.o: libnet/Message.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/NetAddrMap.o: libnet/NetAddrMap.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/Router.o: libnet/Router.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/InterConn.o: libnet/InterConn.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/PMessage.o: libnet/PMessage.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/poolBench.o: misc/poolBench.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/netBench.o: misc/netBench.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/rstexample.o: misc/rstexample.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/mkdep.o: misc/mkdep.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/CacheCoreBench.o: misc/CacheCoreBench.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/TMMemRequest.o: libtm/TMMemRequest.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/MESIProtocol.o: libtm/MESIProtocol.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/TMmain.o: libtm/TMmain.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/TMProtocol.o: libtm/TMProtocol.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/TMSystemBus.o: libtm/TMSystemBus.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/TMCache.o: libtm/TMCache.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/TMMemorySystem.o: libtm/TMMemorySystem.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/EventManager.o: MemSys/EventManager.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/MemorySystem.o: MemSys/MemorySystem.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/Connection.o: MemSys/Connection.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/BaseMemDevice.o: MemSys/Devices/BaseMemDevice.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/TestMemory.o: MemSys/Devices/TestMemory.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/SnoopyBus.o: MemSys/Devices/SnoopyBus.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/MOESICache.o: MemSys/Devices/MOESICache.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/SESCProcessorInterface.o: MemSys/Devices/SESCProcessorInterface.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/rz3iu.o: librst/rz3iu.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/rstzip3.o: librst/rstzip3.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/decompress_engine.o: librst/decompress_engine.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/cpuid.o: librst/cpuid.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/compress_engine.o: librst/compress_engine.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/rz3_section.o: librst/rz3_section.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/Rstzip.o: librst/Rstzip.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/rstzip2if.o: librst/rstzip2if.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/Config.o: libsuc/Config.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/ThermTrace.o: libsuc/ThermTrace.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/SCTable.o: libsuc/SCTable.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/GStats.o: libsuc/GStats.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/MSHR.o: libsuc/MSHR.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/conflexlex.o: libsuc/conflexlex.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/callback.o: libsuc/callback.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/Snippets.o: libsuc/Snippets.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/GEnergy.o: libsuc/GEnergy.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/Port.o: libsuc/Port.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/TraceGen.o: libsuc/TraceGen.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/BloomFilter.o: libsuc/BloomFilter.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/ReportGen.o: libsuc/ReportGen.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/CacheCore.o: libsuc/CacheCore.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/nanassert.o: libsuc/nanassert.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/SescConf.o: libsuc/SescConf.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/zlibStreams.o: libsuc/zlibStreams.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/ReportTherm.o: libsuc/ReportTherm.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/TQueue.o: libsuc/TQueue.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/conflex.tab.o: libsuc/conflex.tab.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/TMMemory.o: libtrans/TMMemory.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/PerfectCollisionManager.o: libtrans/PerfectCollisionManager.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/LogTMSECollisionDetection.o: libtrans/LogTMSECollisionDetection.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/CollisionDetectionExcludeAllBut.o: libtrans/CollisionDetectionExcludeAllBut.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/TMProcessor.o: libtrans/TMProcessor.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/PerfectVersioning.o: libtrans/PerfectVersioning.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/TMInterface.o: libtrans/TMInterface.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/PerfectCollisionDetection.o: libtrans/PerfectCollisionDetection.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/LogTMCollisionDetection.o: libtrans/LogTMCollisionDetection.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/MemCtrl.o: libmem/MemCtrl.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/Bus.o: libmem/Bus.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/Bank.o: libmem/Bank.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/PriorityBus.o: libmem/PriorityBus.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/UglyMemRequest.o: libmem/UglyMemRequest.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/MarkovPrefetcher.o: libmem/MarkovPrefetcher.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/MemoryOS.o: libmem/MemoryOS.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/AddressPrefetcher.o: libmem/AddressPrefetcher.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/AlwaysPrefetch.o: libmem/AlwaysPrefetch.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/Cache.o: libmem/Cache.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/MemorySystem_Old.o: libmem/MemorySystem_Old.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/TLB.o: libmem/TLB.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/mtst1.o: libmem/mtst1.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/TaggedPrefetcher.o: libmem/TaggedPrefetcher.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/StridePrefetcher.o: libmem/StridePrefetcher.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/ElfObject.o: libemul/ElfObject.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/FileSys.o: libemul/FileSys.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/EmulInit.o: libemul/EmulInit.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/LinuxSys.o: libemul/LinuxSys.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/SignalHandling.o: libemul/SignalHandling.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/InstDesc.o: libemul/InstDesc.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/AddressSpace.o: libemul/AddressSpace.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/X86InstDesc.o: libemul/X86InstDesc.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<

-include $(OBJECTS:%.o=%.d)
