
OBJECTS = ç
		CLIPLINE.o ç
		CONTEXT.o ç
		DRAWABLE.o ç
		FILEGFX.o ç
		FONT.o ç
		FRAME.o ç
		GETBODY.o ç
		INTERSEC.o ç
		LINE.o ç
		LOADDISP.o ç
		MAKEPOLY.o ç
		MAP.o ç
		PIXMAP.o ç
		POLY.o ç
		RECT.o ç
		RESGFX.o ç
		STAMP.o ç
		:3do:BOXINT.a ç
		:3do:IMAGEINT.a ç
		:3do:CMAP.o ç
		:3do:DISPLAY.o ç
		:3do:GFX3DO.o ç
		:3do:THREEDO.o ç
		:3do:MASK.o ç
		:3do:ourvdl.o ç
		:3do:mapcel.a
		
#		:3do:OVERWIN.o

CC = armcc -zps1 -za1 -ff
ASM = armasm
INCLUDES = -i "{HardDisk}:src:libs",':3DO',':',"{3DOIncludes}"
FLAGS = -d __WATCOMC__=1 -d V{3dorelease}

graphics.lib ÉÉ {OBJECTS} lib.make
	armlib -o -c graphics.lib {OBJECTS}
	
.a	É	.s
	 {ASM} {default}.s -o {default}.a 
.o	É	.c
	 {CC} {INCLUDES} {FLAGS} {default}.C -o {default}.o


