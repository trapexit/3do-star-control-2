APP = duckloop

COMMOBJS =	ç
		:objects:duckdebug.c.o ç
		:objects:dx.s.o ç
		:objects:dxg.s.o ç
		:objects:ourvdl.c.o ç
		:objects:sdmotion.c.o

APPOBJS = vid_snd.o sdplayer.o sdp.o {COMMOBJS}
LIBOBJS = vid_snd.lib.o sdplayer.lib.o sdp.lib.o {COMMOBJS}
		
CC = armcc -zps1 -za1 -ff
INCLUDES = -i "{3doincludes}","::duck.52","::src.new"

SYSLIBS	=	ç
		"{3DOLibs}Lib3DO.lib"		ç
		"{3DOLibs}operamath.lib"	ç
		"{3DOLibs}filesystem.lib"	ç
		"{3DOLibs}graphics.lib"		ç
		"{3DOLibs}audio.lib"		ç
		"{3DOLibs}music.lib"		ç
		"{3DOLibs}input.lib"		ç
		"{3DOLibs}clib.lib"		ç
		"{3DOLibs}swi.lib"

all É {APP}.lib {APP}

{APP}.lib É {LIBOBJS} makefile
	armlib -o -c {APP}.lib {LIBOBJS}

{APP} É {APPOBJS} makefile
	armlink -aif -r -b 0x00 -workspace 0x4000 -o {APP} -d "{3DOLibs}cstartup.o" {APPOBJS} {SYSLIBS}
	modbin {APP} -stack 8000 -debug
	duplicate -y {APP} "{3DORemote}"
	beep
	
.o É .c makefile
	 {CC} {INCLUDES} -D STANDALONE -g {default}.c
	 
.lib.o É .c
	 {CC} {INCLUDES} {default}.C -o {default}.lib.o
	 
