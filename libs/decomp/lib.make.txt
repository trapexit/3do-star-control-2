
OBJECTS = ç
		LZENCODE.o ç
		LZDECODE.o ç
		UPDATE.o

CC = armcc -zps1 -za1 -ff
INCLUDES = -i "{HardDisk}:src:libs",':3DO',':'
FLAGS = -d __WATCOMC__=1

decomp.lib ÉÉ {OBJECTS}
	armlib -o -c decomp.lib {OBJECTS}
	
.o É  .C
	 {CC} {INCLUDES} {FLAGS} {default}.C -o {default}.o
	 
