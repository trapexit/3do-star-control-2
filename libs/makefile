BARFOBJS	= barf.o
FILESYSOBJS	= filesys.o

CC = armcc -zps1 -za1 -ff -g
INCLUDES = -i "{HardDisk}:src:libs","{3DOIncludes}"
FLAGS = -d __WATCOMC__=1 -d _VW=292 -d _VH=216

LIBS	=	ç
		:graphics:graphics.lib ç
		:memory:memory.lib ç
		:task:task.lib ç
		:sound:sound.lib ç
		:strings:strings.lib ç
		:resource:resource.lib ç
		:decomp:decomp.lib ç
		:input:input.lib ç
		:math:math.lib ç
		:video:video.lib
		
SYSLIBS	=	ç
		"{3DOLibs}StorageTuner.lib" ç
		"{3DOLibs}Lib3DO.lib"	ç
		"{3DOLibs}operamath.lib"	ç
		"{3DOLibs}filesystem.lib"	ç
		"{3DOLibs}graphics.lib"		ç
		"{3DOLibs}audio.lib"		ç
		"{3DOLibs}music.lib"		ç
		"{3DOLibs}input.lib"		ç
		"{3DOLibs}clib.lib"		ç
		"{3DOLibs}swi.lib"

barf ÉÉ {BARFOBJS} {LIBS}
	armlink -aif -r -b 0x00 -workspace 0x4000 -d -o barf "{3DOLibs}cstartup.o" {BARFOBJS} {LIBS} {SYSLIBS}
	modbin barf -stack 8000 -debug
	duplicate -y barf "{3DORemote}"
	
filesys ÉÉ {FILESYSOBJS}
	armlink -aif -r -b 0x00 -workspace 0x4000 -d -o filesys "{3DOLibs}cstartup.o" {FILESYSOBJS} {SYSLIBS}
	modbin filesys -stack 8000 -debug
	duplicate -y filesys "{3DORemote}"
	
.o É  .C
	{CC} {INCLUDES} {FLAGS} {default}.C
	 
