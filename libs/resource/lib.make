
OBJECTS = ç
		GETRES.o ç
		INIT.o ç
		LOADRES.o ç
		RESDATA.o ç
		:3do:FILECNTL.o ç
		:3do:direct.o ç
		:3do:nvram.o

CC = armcc -zps1 -za1 -ff
INCLUDES = -i "{HardDisk}:src:libs",':3DO',':',"{3DOIncludes}"
FLAGS = -d __WATCOMC__=1

resource.lib ÉÉ {OBJECTS}
	armlib -o -c resource.lib {OBJECTS}
	
.o É .C
	 {CC}  {INCLUDES} {FLAGS} {default}.C -o {default}.o
