#   File:       barf.make
#   Target:     barf
#   Created:    Monday, August 9, 1993 1:01:32 PM

OBJECTS = ç
		test.o

CC = armcc -g -zps1 -za1
CC2 = armcc -g -zps0 -za1
INCLUDES = -i "{3doincludes}"
FLAGS = -d __WATCOMC__=1 -d _VW=292 -d _VH=216

#PROFLIB	=	:profile:profile.lib
PROFLIB	=

		
SYSLIBS	=	"{3DOLibs}Lib3DO.lib"		ç
		"{3DOLibs}operamath.lib"	ç
		"{3DOLibs}filesystem.lib"	ç
		"{3DOLibs}graphics.lib"		ç
		"{3DOLibs}audio.lib"		ç
		"{3DOLibs}music.lib"		ç
		"{3DOLibs}input.lib"		ç
		"{3DOLibs}clib.lib"		ç
		"{3DOLibs}swi.lib"

test ÉÉ {OBJECTS}
	armlink -aif -r -b 0x00 -workspace 0x4000 -debug -o test "{3DOLibs}cstartup.o" {OBJECTS} {SYSLIBS}
	modbin test -stack 8000
	duplicate -y test "{3DORemote}"
	beep
	
test.o É  test.C
	 {CC} {INCLUDES} {FLAGS} test.C
	 
line.o É  line.c
	 {CC2} -i {3DOINCLUDES} {FLAGS} line.c
	 
cpdump.o É  cpdump.c
	 {CC2} {INCLUDES} -i {3DOINCLUDES} {FLAGS} cpdump.c
	 
