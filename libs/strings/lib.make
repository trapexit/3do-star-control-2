
OBJECTS = ç
		FILEINST.o ç
		GETSTR.o ç
		STRINGS.o ç
		RESINST.o

CC = armcc -zps1 -za1 -ff
INCLUDES = -i "{HardDisk}:src:libs",':3DO',':'
FLAGS = -d __WATCOMC__=1

strings.lib ÉÉ {OBJECTS}
	armlib -o -c strings.lib {OBJECTS}
	
.o É  .C
	 {CC} {INCLUDES} {FLAGS} {default}.C -o {default}.o
