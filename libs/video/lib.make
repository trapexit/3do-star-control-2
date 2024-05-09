
OBJECTS = ç
		fileinst.o ç
		:3do:video.o

CC = armcc -zps1 -za1 -ff
INCLUDES = -i "{HardDisk}:src:libs",':3DO',':',"{3DOIncludes}"
FLAGS = -d __WATCOMC__=1

video.lib ÉÉ {OBJECTS}
	armlib -o -c video.lib {OBJECTS}
	
.o	É	.c
	 {CC} {INCLUDES} {FLAGS} {default}.C -o {default}.o


