APP = nvram

OBJECTS = main.o nvram.o

CC = armcc -zps1 -za1 -ff
INCLUDES = -i "{3doincludes}"
FLAGS = -g

SYSLIBS	=	ç
		"{3DOLibs}StorageTuner.lib" ç
		"{3DOLibs}Lib3DO.lib"		ç
		"{3DOLibs}operamath.lib"	ç
		"{3DOLibs}filesystem.lib"	ç
		"{3DOLibs}graphics.lib"		ç
		"{3DOLibs}audio.lib"		ç
		"{3DOLibs}music.lib"		ç
		"{3DOLibs}input.lib"		ç
		"{3DOLibs}clib.lib"		ç
		"{3DOLibs}swi.lib"

{APP} ÉÉ {OBJECTS} {APP}.make
	armlink -aif -r -b 0x00 -workspace 0x4000 -d -o {APP} "{3DOLibs}cstartup.o" {OBJECTS} {SYSLIBS}
	modbin {APP} -stack 8000 -debug
	duplicate -y {APP} "{3DORemote}"
	beep
	
.o É .C {APP}.make
	 {CC} {INCLUDES} {FLAGS} {default}.C
	 
