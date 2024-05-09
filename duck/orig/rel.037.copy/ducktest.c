/*
	File:		DuckTest.c

	Written by:	Dan Miller and Victor Yurkovsky

	Copyright:	© 1993 by The Duck Corporation. All rights reserved.
				This material constitutes confidential and proprietary
				information of The Duck Corporation and shall not be used by
				any person (or bozo) for any purpose except as expressly
				authorized in writing by The Duck Corporation.


	Duck Player released version 0.37 (REL 037)
	
	This is a test program to play Duck 3DO compressed Audio/Video files.
	At present, four (4) data files must be found by the player.  These files
	will reside in a directory called "DuckArt", which should be in the
	root directory.  The files are:
	
	filename.duk
	filename.hdr
	filename.tbl
	filename.frm
	
	where 'filename' specifies the compressed material (i.e., "doors.duk" etc.).
	
	In addition, a system file for the audio DSP must be found in the "DSPP"
	folder in the root directory.  Which file you need is dictated by the audio
	format.  The possible files are:
	
	adpcm22m2.dsp		22 khz mono
	adpcm44m2.dsp		44 khz mono
	adpcm22s2.dsp		22 khz stereo
	adpcm44s2.dsp		44 khz stereo
	
	To compile, type:
	c ducktest
	in the MPW Worksheet window; then type:
	link
	to link with the .o files.  link will copy ducktest into your remote directory.
	
	To run, type ducktest in the terminal window of the debugger.  Make sure you don't
	have a CD in the drive when you reset the 3DO, or it will look for datafiles there.
	
	This program has been hardcoded to play the 'doors' piece.  During play, the
	joystick can be used to fast-forward, step-frame, and rewind.  Joystick button
	C quits the player and returns to the calling program.  The rest should be self-
	explanatory, but in case it is not, call Dan Miller at 685-4000 or 213-0349.
	
	*/


#include "DuckTypes.h"
#include "Portfolio.h"
#include "Init3DO.h"				// setupScreen
#include "mem.h"
#include "DuckPlayer.h"
#include "audio.h"

#define BASE_DIR	"/cd-rom"
#define BUTTONS ReadControlPad(0)

/* This version of the Duck player uses screens 0 thru 3 of ScreenContext TheScreen.
	If this is not convenient, it should be OK to copy relevant information into this
	structure.  Here is the typedef for ScreenContext with info on how it is used by
	the player:
	
	typedef struct ScreenContext_TAG
	{
	int32 	sc_nScreens;						// 4
	int32 	sc_curScreen;						// initialized to zero
	int32 	sc_nFrameBufferPages;				// you'll have to calculate this
	int32 	sc_nFrameByteCount;					// unused
	Item 	sc_Screens[MAXSCREENS];				// item numbers for Screens
	Item	sc_BitmapItems[MAXSCREENS];			// item numbers for Bitmaps
	Bitmap	*sc_Bitmaps[MAXSCREENS];			// pointers to Bitmaps
	} ScreenContext;
	
*/

extern ScreenContext TheScreen;

int main(int argc, char** argv){
	player plr;						/* this is the main player struct */
	bool res;
	u32	*testptr,i;
	
/* rumor has it this is not the 3DO-approved method of setting default
	directories, so supply your own.
	
	Duck player expects files to exist in DuckArt, a directory in the
	present path: */
	
	if (ChangeDirectory(BASE_DIR) < 0)
		printf("Cannot change directory!");
		
/* we need a screenContext with four (4) screens: */

	if (!OpenGraphics(&TheScreen,4)
		|| !OpenSPORT())
		{
			return FALSE;
		}
		
/* number crunching please: */

	if (OpenMathFolio())
	{
		printf(("Math Folio could not be opened!\n"));
		return(-1);
	}
		
/* needs audio, of course. */

	if (OpenAudioFolio())
	{
		printf(("Audio Folio could not be opened!\n"));
		return(-1);
	}

/* we'll play this file a few times, to prove it works: */

	for(i=0;i<3;i++) {
  
/*	playerOpen(player plr, char *name, u32 samprate)

	player is our main player struct, see Duckplayer.h
	
	name is file to play sans extensions
	
	samprate:

	Audio comes in 4 flavors.  WARNING -- Duck player loads a VERY LARGE INSTRUMENT
	into the DSPP chip, which outputs directly to the DAC's.  You may have to dis-
	connect and unload instruments before calling initAudio!

	samprate	description			DSPP instrument file required
	
	22			22 khz mono			adpcm22m2.dsp
	23			22 khz stereo		adpcm22s2.dsp
	44			44 khz mono			adpcm44m2.dsp
	45			44 khz stereo		adpcm44s2.dsp
	
	The doors clip is 44 khz mono.
*/
		
		res = playerOpen(&plr,"doors",44);
		if(res) {
		
/* play clip.  For now, Player demands frame count for proper play: */
	
			playerPlay(&plr,150);			// actually 1550 frames
		
/* Close player, releasing precious resources: */
	
			playerClose(&plr);
	
/*			testptr=(u32*)AllocMem(800000,MEMTYPE_DRAM);
			printf("MEMORY ALLOCATION TEST: testptr=%p\n",testptr);
			FreeMem(testptr,800000);	*/
		
/* exit politely: */
	
			printf("Thank you for using DUCK technology\n");
		
		} else {
			printf("Player open failed\n");
		}	
	}

	printf("GOODBYE\n");
	ShutDown();
	return(0);

}

