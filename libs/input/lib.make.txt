
OBJECTS = ç
		INPUT.o ç
		:3do:IBMINP.o ç
		:3do:ibmmouse.o

CC = armcc -zps1 -za1 -ff
INCLUDES = -i "{HardDisk}:src:libs",':3DO',':',"{3DOIncludes}"
FLAGS = -d __WATCOMC__=1

input.lib ÉÉ {OBJECTS}
	armlib -o -c input.lib {OBJECTS}
	
.o É .C
	 {CC}  {INCLUDES} {FLAGS} {default}.C -o {default}.o

