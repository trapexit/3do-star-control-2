
OBJECTS = �
		GETRES.o �
		INIT.o �
		LOADRES.o �
		RESDATA.o �
		:3do:FILECNTL.o �
		:3do:direct.o �
		:3do:nvram.o

CC = armcc -zps1 -za1 -ff
INCLUDES = -i "{HardDisk}:src:libs",':3DO',':',"{3DOIncludes}"
FLAGS = -d __WATCOMC__=1

resource.lib �� {OBJECTS}
	armlib -o -c resource.lib {OBJECTS}
	
.o � .C
	 {CC}  {INCLUDES} {FLAGS} {default}.C -o {default}.o
