#   File:       
#   Target:     sound.make
#   Created:    Monday, August 9, 1993 1:01:32 PM


OBJECTS = �
		FILEINST.o �
		MEDPLAY.o �
		MODFUNCS.o �
		PLAY.o �
		RESINST.o �
		:3do:IBMSND.o �
		:3do:SOUND3DO.o �
		:3do:trackplayer.o �
		:3do:audiox.o �
		:3do:cd.o

CC = armcc -zps1 -za1 -ff
INCLUDES = -i "{HardDisk}:src:libs",':3DO',':',"{3DOIncludes}"
FLAGS = -d __WATCOMC__=1 -d DELAY_STUFF

sound.lib �� {OBJECTS}
	armlib -o -c sound.lib {OBJECTS}
	
.o �  .C
	 {CC} {INCLUDES} {FLAGS} {default}.C -o {default}.o
