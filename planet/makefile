GLOWOBJS	= glow.o
PLANETOBJS	= planet.o pl_stuff.o mapcel.a
DDAOBJS		= dda.o

CC = armcc -zps1 -za1 -ff -g -Wa
ASM = armasm -g
INCLUDES = -i "{3DOIncludes}"

SYSLIBS	=	"{3DOLibs}Lib3DO.lib"	�
		"{3DOLibs}operamath.lib"	�
		"{3DOLibs}filesystem.lib"	�
		"{3DOLibs}graphics.lib"		�
		"{3DOLibs}audio.lib"		�
		"{3DOLibs}music.lib"		�
		"{3DOLibs}input.lib"		�
		"{3DOLibs}clib.lib"		�
		"{3DOLibs}swi.lib"

planet �� {PLANETOBJS}
	armlink -aif -r -b 0x00 -workspace 0x4000 -d -o planet "{3DOLibs}cstartup.o" {PLANETOBJS} {SYSLIBS}
	modbin planet -stack 8000 -debug
	duplicate -y planet "{3DORemote}"
	
glow �� {GLOWOBJS}
	armlink -aif -r -b 0x00 -workspace 0x4000 -d -o glow "{3DOLibs}cstartup.o" {GLOWOBJS} {SYSLIBS}
	modbin glow -stack 8000 -debug
	duplicate -y glow "{3DORemote}"
	
dda �� {DDAOBJS}
	armlink -aif -r -b 0x00 -workspace 0x4000 -d -o dda "{3DOLibs}cstartup.o" {DDAOBJS} {SYSLIBS}
	modbin dda -stack 8000 -debug
	duplicate -y dda "{3DORemote}"
	
.o �  .C
	 {CC} {INCLUDES} {FLAGS} {default}.C
	 
.a �  .s
	 {ASM} {INCLUDES} -o {default}.a {default}.s
