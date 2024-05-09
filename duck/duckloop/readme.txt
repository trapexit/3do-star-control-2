
SDP = Simple (Stupid) Duck Player

usage:  SDP [filename startframe numframes]

		if no arguments are given, will play VIDLOOP.DUK repeatedly

look in main() routine of sdp.c to see call to playloop()

FIXING FILENAMES:

fix duckdebug.h and ducktypes.h

OBJECTS DIRECTORY SHOULD CONTAIN:

	duckdebug.c.o
	dx.s.o
	dxg.s.o
	ourvdl.c.o
	sdmotion.c.o
	sdp.c.o
	sdlayer.c.o