#DUCKOBJPATH = ::duckobjs
DUCKOBJPATH = ::duck.52:objects:
#DUCKINCPATH = ::duckincs
DUCKINCPATH = ::duck.52

OBJECTS = �
		duck.o �
		"{DUCKOBJPATH}DuckMotion.c.o" �
		"{DUCKOBJPATH}dx.s.o" �
		"{DUCKOBJPATH}dxg.s.o" �
		"{DUCKOBJPATH}Async.c.o" �
		"{DUCKOBJPATH}DuckPlayer.c.o" �
		"{DUCKOBJPATH}AudioADPCM.c.o" �
		"{DUCKOBJPATH}ourVDL.c.o" �
		"{DUCKOBJPATH}DuckDebug.c.o"


CC = armcc -zps1 -za1 -ff
INCLUDES = -i {DUCKINCPATH},"{3DOIncludes}"

SYSLIBS	=	"{3DOLibs}Lib3DO.lib"		�
		"{3DOLibs}operamath.lib"	�
		"{3DOLibs}filesystem.lib"	�
		"{3DOLibs}graphics.lib"		�
		"{3DOLibs}audio.lib"		�
		"{3DOLibs}music.lib"		�
		"{3DOLibs}input.lib"		�
		"{3DOLibs}clib.lib"		�
		"{3DOLibs}swi.lib"

duck �� {OBJECTS} makefile
	armlink -aif -r -b 0x00 -workspace 0x4000 -o duck "{3DOLibs}cstartup.o" {OBJECTS} {SYSLIBS}
	modbin duck -stack 8000
	stripaif duck
	duplicate -y duck "{3DORemote}"
	beep
	
.o � .C
	 {CC} {INCLUDES} {default}.C
	 
