
OBJECTS = �
		:3do:TASK.o

CC = armcc -zps1 -za1 -ff
INCLUDES = -i '{HardDisk}:src:libs',':3DO',':',"{3DOIncludes}"
FLAGS = -d __WATCOMC__=1

task.lib �� {OBJECTS}
	armlib -o -c task.lib {OBJECTS}
	
.o �  .C
	 {CC} {INCLUDES} {FLAGS} {default}.C -o {default}.o
