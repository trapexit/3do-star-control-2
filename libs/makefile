BARFOBJS	= barf.o
FILESYSOBJS	= filesys.o

CC = armcc -zps1 -za1 -ff -g
INCLUDES = -i "{HardDisk}:src:libs","{3DOIncludes}"
FLAGS = -d __WATCOMC__=1 -d _VW=292 -d _VH=216

LIBS	=	�
		:graphics:graphics.lib �
		:memory:memory.lib �
		:task:task.lib �
		:sound:sound.lib �
		:strings:strings.lib �
		:resource:resource.lib �
		:decomp:decomp.lib �
		:input:input.lib �
		:math:math.lib �
		:video:video.lib
		
SYSLIBS	=	�
		"{3DOLibs}StorageTuner.lib" �
		"{3DOLibs}Lib3DO.lib"	�
		"{3DOLibs}operamath.lib"	�
		"{3DOLibs}filesystem.lib"	�
		"{3DOLibs}graphics.lib"		�
		"{3DOLibs}audio.lib"		�
		"{3DOLibs}music.lib"		�
		"{3DOLibs}input.lib"		�
		"{3DOLibs}clib.lib"		�
		"{3DOLibs}swi.lib"

barf �� {BARFOBJS} {LIBS}
	armlink -aif -r -b 0x00 -workspace 0x4000 -d -o barf "{3DOLibs}cstartup.o" {BARFOBJS} {LIBS} {SYSLIBS}
	modbin barf -stack 8000 -debug
	duplicate -y barf "{3DORemote}"
	
filesys �� {FILESYSOBJS}
	armlink -aif -r -b 0x00 -workspace 0x4000 -d -o filesys "{3DOLibs}cstartup.o" {FILESYSOBJS} {SYSLIBS}
	modbin filesys -stack 8000 -debug
	duplicate -y filesys "{3DORemote}"
	
.o �  .C
	{CC} {INCLUDES} {FLAGS} {default}.C
	 
