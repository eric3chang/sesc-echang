add_library (mint
   coproc.cpp
   dump.cpp
   elf.cpp
   error_ops.cpp
   exec.cpp
   icode.cpp
   mendian.cpp
   mint_init.cpp
   non_mips.cpp
   opcodes.cpp
   ops.cpp
   subst.cpp
   symtab.cpp
   utils.cpp
)

target_link_libraries (mint "${PROJECT_BINARY_DIR}/libll/libll.a")
