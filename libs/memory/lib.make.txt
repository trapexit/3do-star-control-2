OLDOBJECTS = ç
		ALLOC.o ç
		COMPLEX.o ç
		DATAINFO.o ç
		HASH.o ç
		HNDLINFO.o ç
		INIT.o ç
		MEMERR.o ç
		MEMTYPE.o ç
		N_ARY.o ç
		RAM.o ç
		:3do:MEMIMP.o ç
		:3do:PRIMARY.o ç
		:3do:VRAM.o ç
		:3do:MEM3DO.o ç
		SIMPLE.o
		
NEWOBJECTS = ç
		:3do:simple.o ç
		:3do:mem3do.o
		
OBJECTS = ç
		ALLOC.o ç
		COMPLEX.o ç
		DATAINFO.o ç
		HASH.o ç
		HNDLINFO.o ç
		INIT.o ç
		MEMERR.o ç
		MEMTYPE.o ç
		N_ARY.o ç
		RAM.o ç
		:3do:MEMIMP.o ç
		:3do:PRIMARY.o ç
		:3do:VRAM.o ç
		:3do:MEM3DO.o ç
		SIMPLE.o
		
CC = armcc -zps1 -za1 -ff
INCLUDES = -i "{HardDisk}:src:libs",':3DO',':',"{3DOIncludes}"
FLAGS = -d __WATCOMC__=1 -d HANDLE_DIAG=1

memory.lib ÉÉ {OBJECTS}
	armlib -o -c memory.lib {OBJECTS}
	
.o É .C
	 {CC}  {INCLUDES} {FLAGS} {default}.C -o {default}.o
