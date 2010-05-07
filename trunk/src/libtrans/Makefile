#the following line has to be before the include
SIGNATURE=libtrans
include	../Makefile.defs
##############################################################################
.PHONY: all

all	: $(OBJ)/libtrans.a

##############################################################################
#                Objects
##############################################################################
OBJS	:= TMInterface.o TMProcessor.o TMMemory.o PerfectCollisionDetection.o PerfectVersioning.o \
	PerfectCollisionManager.o

##############################################################################
#                             Change Rules                                   # 
##############################################################################
.PHONY: eclean
eclean:
	-@rm -f $(OBJ)/libtrans.a 2>/dev/null

# DO NOT DELETE
_MAKEFILE_COMMON_SUBSHELL=1
include $(SRC_DIR)/Makefile.common

ifneq ($(MAKECMDGOALS),distclean)
ifneq ($(MAKECMDGOALS),clean)
-include $(DOTDEPEND)
endif
endif
