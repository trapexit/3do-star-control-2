#   File:       
#   Target:     sound.make
#   Created:    Monday, August 9, 1993 1:01:32 PM


OBJECTS = ç
		FILEINST.o ç
		MEDPLAY.o ç
		MODFUNCS.o ç
		PLAY.o ç
		RESINST.o ç
		:3do:IBMSND.o ç
		:3do:SOUND3DO.o ç
		:3do:trackplayer.o ç
		:3do:audiox.o ç
		:3do:cd.o

CC = armcc -zps1 -za1 -ff
INCLUDES = -i "{HardDisk}:src:libs",':3DO',':',"{3DOIncludes}"
FLAGS = -d __WATCOMC__=1 -d DELAY_STUFF

sound.lib ÉÉ {OBJECTS}
	armlib -o -c sound.lib {OBJECTS}
	
.o É  .C
	 {CC} {INCLUDES} {FLAGS} {default}.C -o {default}.o
