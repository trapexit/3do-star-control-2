
OBJECTS = ç
		24bit.o ç
		DECODERS.o ç
		JREVDCT.o ç
		MAIN.o ç
		MACRO.o ç
		MVECTOR.o ç
		PARSEBLK.o ç
		UTIL.o ç
		GDITH.o ç
		VIDEO.o

CC = armcc -zps1 -za1 -ff
INCLUDES = -i : -i "{3DOIncludes}"
FLAGS = -d NO_LRAND48 -d NDEBUG 

SYSLIBS	=	"{3DOLibs}Lib3DO.lib"		ç
		"{3DOLibs}operamath.lib"	ç
		"{3DOLibs}filesystem.lib"	ç
		"{3DOLibs}graphics.lib"		ç
		"{3DOLibs}audio.lib"		ç
		"{3DOLibs}music.lib"		ç
		"{3DOLibs}input.lib"		ç
		"{3DOLibs}clib.lib"		ç
		"{3DOLibs}swi.lib"

mpeg ÉÉ {OBJECTS}
	armlink -aif -r -b 0x00 -workspace 0x4000 -o mpeg "{3DOLibs}cstartup.o" {OBJECTS} ç
	{SYSLIBS}
	modbin mpeg -stack 8000
	duplicate -y mpeg "{3DORemote}"
	
.o É .c
	{CC} {INCLUDES} {FLAGS} -o {default}.o {default}.c
