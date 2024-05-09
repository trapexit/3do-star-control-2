INC1		= /src/starcon2
INC2		= $(INC1)/planets
CFLAGS		= /AM /W4 /Ox /Gc /Gr /G2 -I$(INC2) -I$(INC1)
LDMAP		= $(@R)
MAKEFILE	= Makefile
OBJS		= calc.obj orbits.obj surface.obj

all:		$(OBJS)
