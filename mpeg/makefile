
OBJECTS = �
		24bit.o �
		DECODERS.o �
		JREVDCT.o �
		MAIN.o �
		MACRO.o �
		MVECTOR.o �
		PARSEBLK.o �
		UTIL.o �
		GDITH.o �
		VIDEO.o

CC = armcc -zps1 -za1 -ff
INCLUDES = -i : -i "{3DOIncludes}"
FLAGS = -d NO_LRAND48 -d NDEBUG 

SYSLIBS	=	"{3DOLibs}Lib3DO.lib"		�
		"{3DOLibs}operamath.lib"	�
		"{3DOLibs}filesystem.lib"	�
		"{3DOLibs}graphics.lib"		�
		"{3DOLibs}audio.lib"		�
		"{3DOLibs}music.lib"		�
		"{3DOLibs}input.lib"		�
		"{3DOLibs}clib.lib"		�
		"{3DOLibs}swi.lib"

mpeg �� {OBJECTS}
	armlink -aif -r -b 0x00 -workspace 0x4000 -o mpeg "{3DOLibs}cstartup.o" {OBJECTS} �
	{SYSLIBS}
	modbin mpeg -stack 8000
	duplicate -y mpeg "{3DORemote}"
	
.o � .c
	{CC} {INCLUDES} {FLAGS} -o {default}.o {default}.c
