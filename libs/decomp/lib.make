
OBJECTS = �
		LZENCODE.o �
		LZDECODE.o �
		UPDATE.o

CC = armcc -zps1 -za1 -ff
INCLUDES = -i "{HardDisk}:src:libs",':3DO',':'
FLAGS = -d __WATCOMC__=1

decomp.lib �� {OBJECTS}
	armlib -o -c decomp.lib {OBJECTS}
	
.o �  .C
	 {CC} {INCLUDES} {FLAGS} {default}.C -o {default}.o
	 
