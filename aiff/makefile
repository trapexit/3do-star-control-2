#   File:       barf.make
#   Target:     barf
#   Created:    Monday, August 9, 1993 1:01:32 PM

OBJECTS = audiox.o trackplayer.o aiff.o oscill.o graphics.o

CC = armcc -zps1 -za1 -ff
INCLUDES = -i "{3doincludes}"
FLAGS = -d DELAY_STUFF -g

SYSLIBS	=	�
		"{3DOLibs}Lib3DO.lib"		�
		"{3DOLibs}operamath.lib"	�
		"{3DOLibs}filesystem.lib"	�
		"{3DOLibs}graphics.lib"		�
		"{3DOLibs}audio.lib"		�
		"{3DOLibs}music.lib"		�
		"{3DOLibs}input.lib"		�
		"{3DOLibs}clib.lib"		�
		"{3DOLibs}swi.lib"

aiff �� {OBJECTS}
	armlink -aif -r -b 0x00 -workspace 0x4000 -d -o aiff "{3DOLibs}cstartup.o" {OBJECTS} {SYSLIBS}
	modbin aiff -stack 8000 -debug
	duplicate -y aiff "{3DORemote}"
	beep
	
.o � .C makefile
	 {CC} {INCLUDES} {FLAGS} {default}.C
	 
