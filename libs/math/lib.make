
OBJECTS = ç
	RANDOM.o ç
	SQRT.o

CC = armcc -zps1 -za1 -ff
INCLUDES = -i "{HardDisk}:src:libs",':3DO',':'
FLAGS = -d __WATCOMC__=1

math.lib ÉÉ {OBJECTS}
	armlib -o -c math.lib {OBJECTS}
	
.o É  .C
	 {CC} {INCLUDES} {FLAGS} {default}.C -o {default}.o
	 
