#!/bin/bash
mipseb-linux-gcc -g -mips2 -mabi=32 -Wa,-non_shared -mno-abicalls -c -o sesc_thread.o sesc_thread.c

