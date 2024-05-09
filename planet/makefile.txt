GLOWOBJS	= glow.o
PLANETOBJS	= planet.o pl_stuff.o mapcel.a
DDAOBJS		= dda.o

CC = armcc -zps1 -za1 -ff -g -Wa
ASM = armasm -g
INCLUDES = -i "{3DOIncludes}"

SYSLIBS	=	"{3DOLibs}Lib3DO.lib"	ç
		"{3DOLibs}operamath.lib"	ç
		"{3DOLibs}filesystem.lib"	ç
		"{3DOLibs}graphics.lib"		ç
		"{3DOLibs}audio.lib"		ç
		"{3DOLibs}music.lib"		ç
		"{3DOLibs}input.lib"		ç
		"{3DOLibs}clib.lib"		ç
		"{3DOLibs}swi.lib"

planet ÉÉ {PLANETOBJS}
	armlink -aif -r -b 0x00 -workspace 0x4000 -d -o planet "{3DOLibs}cstartup.o" {PLANETOBJS} {SYSLIBS}
	modbin planet -stack 8000 -debug
	duplicate -y planet "{3DORemote}"
	
glow ÉÉ {GLOWOBJS}
	armlink -aif -r -b 0x00 -workspace 0x4000 -d -o glow "{3DOLibs}cstartup.o" {GLOWOBJS} {SYSLIBS}
	modbin glow -stack 8000 -debug
	duplicate -y glow "{3DORemote}"
	
dda ÉÉ {DDAOBJS}
	armlink -aif -r -b 0x00 -workspace 0x4000 -d -o dda "{3DOLibs}cstartup.o" {DDAOBJS} {SYSLIBS}
	modbin dda -stack 8000 -debug
	duplicate -y dda "{3DORemote}"
	
.o É  .C
	 {CC} {INCLUDES} {FLAGS} {default}.C
	 
.a É  .s
	 {ASM} {INCLUDES} -o {default}.a {default}.s
