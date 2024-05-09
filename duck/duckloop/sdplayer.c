/*
	File:		DuckPlayer.c

	Contains:	duck player '

	Copyright:	© 1993 by The Duck Corporation. All rights reserved.
				This material constitutes confidential and proprietary
				information of The Duck Corporation and shall not be used by
				any Person or for any purpose except as expressly
				authorized in writing by The Duck Corporation.


	Change History (most recent first):

	1	 9/10/93	vy		first checked in

	To Do:
*/
#include "Portfolio.h"
#include "Async.h"
#include "SDPlayer.h"
#include "DuckMotion.h"
#include "Init3DO.h"				// setupScreen
#include "Parse3DO.h"
#include "graphics.h"				// GetPixelAddress,prints
#include "operamath.h"
#include "audio.h"					// for timer
#include "hardware.h"				// for readcontrolpad
#include "DuckDebug.h"
#include "AudioAdpcm.h"
#include "colorecho.h"				// cells
#include "ourVDL.h"
#include "mem.h"					// free
#include "Utils3DO.h"				// control pad

#define MAX_SCREENS	4

bool	checkHeadBuf(player *plr);
void	initiateRead(player *plr);
void 	playerPrime(player *plr);

ScreenContext TheScreen;

#ifdef NEED_TO_CHANGE_VDL
VDLEntry			*gRawVDLPtrArray[MAX_SCREENS];	// our VDLs (1 per screen)
Item				gVDLItems[MAX_SCREENS];			// actual VDL items
VdlChunk		*rawVDLPtr = NULL;
VDLDisplayCtlWord	gDmaWord;
#endif

#if 0
u32		bugbuf[16*1024];
u32 	*deb = bugbuf;
u32 *bullbuf;

ColorEcho MyCE;							// ce stuff

Item	gVRAMIOReq;							// SPORT used to clear screen
#endif

#if 0
void BugPrint(char *errmsg) {
	printf("BUGPRINT:%s\n",errmsg);
}


void BugState(player *plr) {
	long i,j;
	printf("Player state:\n");
	printf("file:[%s],size:%ld,offset:%ld\n",plr->fileName,plr->fileLength,plr->fileOffset);
	printf("%ld Compression buffers; slop @%ld; slopsize %lx\n",
		plr->numMx,plr->slop,plr->slopSize);

for(i=0;i<plr->numMx;i++){
		printf("[%ld]: @%p contains %ld buffers ",
			i,
			plr->bufStart[i],
			plr->buf[i].actFrames);
		printf("out of %ld max\n",plr->buf[i].maxFrames);

		printf("reading %lx\n",
			plr->buf[i].reading);
		for(j=0;j<plr->buf[i].actFrames;j++){
			printf("   [[%ld]]: frameNumber:%lx, address:%lx,",
				j,
				plr->buf[i].unit[j].frameNumber,
				plr->buf[i].unit[j].address );
			if((plr->buf[i].unit[j].flags)&FLAG_AUDBUSY) printf(" AUDBUSY "); else printf(" audbusy ");
			if((plr->buf[i].unit[j].flags)&FLAG_VIDBUSY) printf(" VIDBUSY "); else printf(" vidbusy ");
			printf("\n");
		}
	}
	printf("%ld screens allocated   %ld is disp\n",plr->numScreens,plr->dispScreen);
	for(i=0;i<plr->numScreens;i++) {
		printf("   [[%ld]]: frameNumber:%lx, address:%lx\n",
		i,
		plr->screen[i].frameNumber,
		plr->screen[i].address );
	}
}
#endif

#ifdef NEED_TO_CHANGE_VDL
static Item	oldVDL[MAX_SCREENS];

/*
 *  Open heart surgery...  Change the display control word for an active VDL 
 *	 and activate it.  return the old VDL so the caller can destroy it. 
 */
void WriteVDLCtlWords(VDLDisplayCtlWord dmaWord, VDLEntry *activeVDLPtr,u32 i) {
	u32		scanLineEntry;
	Item		oldVDL;
	VDL_REC		*activeVDLArrayPtr = (VDL_REC *)activeVDLPtr;
	
	
	// write the control word into each VDL entry
	for (scanLineEntry = 0; scanLineEntry < kScreenHeight; scanLineEntry++)
		((VDL_REC *)(&activeVDLArrayPtr[scanLineEntry]))->displayControl = LONGWORD(dmaWord);

	// hand our raw vdls to the system, let it validate and copy them for it's own
	//  use
	gVDLItems[i] = SubmitVDL(gRawVDLPtrArray[i], (kVDLSize / 4), VDLTYPE_SIMPLE);
	if ( gVDLItems[i] < 0 ) 
	{
		printf( "Error: SubmitVDL() failed with error code %d\n", gVDLItems[0] );
		PrintfSysErr(gVDLItems[i]);
	}

	// activate the new VDL
	oldVDL = SetVDL(TheScreen.sc_Screens[i], gVDLItems[i]);
	if ( oldVDL < 0 ) 
	{
		printf( "Error: SetVDL() failed with error code %d\n", oldVDL );
		PrintfSysErr(oldVDL);
	}
	
	// Even though we've already told the system to use the the newly modified,
	//  the hardware is still using the old one.  because the system sets a
	//  screen to black if DeleteVDL is called for an active VDL, tell the
	//  hardware to use the new one before we toss the old one
//	DisplayScreen( TheScreen.sc_Screens[0], 0 );
	DeleteVDL(oldVDL);
}
#endif /* NEED_TO_CHANGE_VDL */

/************************************************************************************/

bool setupScreens(player *plr,u32 screencnt)	{
	u32 i;
	s32 errorCode;
	
	TheScreen.sc_nScreens = screencnt;
#ifdef STANDALONE
	if (!OpenGraphics(&TheScreen,(int)screencnt)
		|| !OpenSPORT())
		{
			return FALSE;
		}
#else
	FillInScreenContext (&TheScreen, 0);
#endif
	TheScreen.sc_curScreen = 0;
	
	
	for(i=0; i<kNumScreens; i++) {				// fill out screens structure
		plr->screen[i].address =
			(char*)GetPixelAddress(TheScreen.sc_Screens[i],0,0);//,16,46);
		plr->screen[i].frameNumber = 0xFFFFFFFF;
	}

#ifdef NEED_TO_CHANGE_VDL
// set up the VDL
	for ( i = 0; i < screencnt; i++ )
	{
		if ( AllocateVDL(&gRawVDLPtrArray[i], TheScreen.sc_Bitmaps[i] ) != noErr ) {
			printf("unable to allocate VDL\n");
			return false;
		}
	}
/*	if ( LoadImage( "/remote/Seafloor.img",plr->screen[0].address,
		&rawVDLPtr, &TheScreen ) == NULL ) {
			printf( "LoadImage failed\n" );
			return false;
		}
	// if the image has a VDL, merge it's data into our raw vdl
	if ( (rawVDLPtr != NULL) && (rawVDLPtr->chunk_size) ) 
	{
		// merge the image vdl so we see the image with it
		for ( i = 0; i < screencnt; i++ )
			MergeVDL((int32 *)&(rawVDLPtr->vdl[0]), gRawVDLPtrArray[i]);
		
		LONGWORD(gDmaWord) = ((VDL_REC *)(gRawVDLPtrArray[0]))->displayControl;
		
		// done with that, free up the memory 
		free(rawVDLPtr);
	}
*/
	for ( i = 0; i < screencnt; i++ )
	{
		// SubmitVDL wants it's size arg in WORDS
		gVDLItems[i] = SubmitVDL(gRawVDLPtrArray[i], (kVDLSize / 4), VDLTYPE_SIMPLE);
		if ( gVDLItems[i] < 0 ) 
		{
			printf( "Error: SubmitVDL() failed with error code %d\n", gVDLItems[0] );
			return false;
		}
{
    Screen	*screen;
    
    screen = (Screen *)LookupItem (TheScreen.sc_Screens[i]);
    oldVDL[i] = screen->scr_VDLItem;
}
	
		// activate the new VDL
		errorCode = SetVDL(TheScreen.sc_Screens[i], gVDLItems[i]);
		if ( errorCode < 0 ) 
		{
			printf( "Error: SetVDL() failed with error code %d\n", errorCode );
			PrintfSysErr(errorCode);
			return false;
		}
	}

	{
/* set h&v interp for frame & WINDOW!!! all pixels with msb set are window pixels;
   so be sure to set window & frame the same.  (blue source too!)
   
	following bits are set (see ourvdl.h in slideshow, 1.01):
   
	controlWord
	controlWordType
	bypassCLUT
	windowBlueLSBSource=2
	windowVerticalInterp
	windowHorizontalInterp
	blueLSBSource=2
	verticalInterpolation
	horizontalInterpolation
	
	0xC201602C
	
	if dither:
	
	0xC201702C	
*/
		long ourDma;
#if DITHER
		ourDma = 0xC201702C;
#else
		ourDma = 0xC201602C;
#endif
		LONGWORD(gDmaWord) = ourDma;
		for ( i = 0; i < screencnt; i++ )
			WriteVDLCtlWords(gDmaWord, gRawVDLPtrArray[i],0);
//		printf("VDL control word written,\t[%X]\n", LONGWORD(gDmaWord));
	}
		
//whackCLUT();
#endif /* NEED_TO_CHANGE_VDL */

	return TRUE;
}

void
uninitScreens ()
{
#ifdef NEED_TO_CHANGE_VDL
    int	i;
    
    for (i = 0; i < TheScreen.sc_nScreens; i++)
    {
	Screen	*screen;
	Item	curVDL;
    
	screen = (Screen *)LookupItem (TheScreen.sc_Screens[i]);
	curVDL = screen->scr_VDLItem;
	SetVDL (TheScreen.sc_Screens[i], oldVDL[i]);
	DeleteItem (curVDL);
    }
#endif /* NEED_TO_CHANGE_VDL */
}

/*********************************************************************************

	loadParams
	
	load global parameters into the beginning of the player structre

**********************************************************************************/
bool loadParams(player *plr,char *paramname) {
	u32 res;
	res=ReadFile(paramname,48,(long*)&plr->dataVersion,0);
	if(res == -1)
	{
#if 0
		BugPrint("loadParams: could not load");
#endif
		return(false);
	}

	plr->letterOff=(plr->letterYOff*320*2)+(plr->letterXOff*4);  // byte offset
	return(true);
}


