APP = duckloop

COMMOBJS =	�
		:objects:duckdebug.c.o �
		:objects:dx.s.o �
		:objects:dxg.s.o �
		:objects:ourvdl.c.o �
		:objects:sdmotion.c.o

APPOBJS = vid_snd.o sdplayer.o sdp.o {COMMOBJS}
LIBOBJS = vid_snd.lib.o sdplayer.lib.o sdp.lib.o {COMMOBJS}
		
CC = armcc -zps1 -za1 -ff
INCLUDES = -i "{3doincludes}","::duck.52","::src.new"

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

all � {APP}.lib {APP}

{APP}.lib � {LIBOBJS} makefile
	armlib -o -c {APP}.lib {LIBOBJS}

{APP} � {APPOBJS} makefile
	armlink -aif -r -b 0x00 -workspace 0x4000 -o {APP} -d "{3DOLibs}cstartup.o" {APPOBJS} {SYSLIBS}
	modbin {APP} -stack 8000 -debug
	duplicate -y {APP} "{3DORemote}"
	beep
	
.o � .c makefile
	 {CC} {INCLUDES} -D STANDALONE -g {default}.c
	 
.lib.o � .c
	 {CC} {INCLUDES} {default}.C -o {default}.lib.o
	 
