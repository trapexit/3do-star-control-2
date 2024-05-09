/*
	File:		DuckPlayer.c

	Contains:	duck player

	Copyright:	— 1993 by The Duck Corporation. All rights reserved.
				This material constitutes confidential and proprietary
				information of The Duck Corporation and shall not be used by
				any Person or for any purpose except as expressly
				authorized in writing by The Duck Corporation.


	Change History (most recent first):

	PLR 037 12/23/93 dm		fixed for user satisfaction -- deallocation, quit at end, etc
	
	1	 9/10/93	vy		first checked in

	To Do:
*/
#include "Portfolio.h"
#include "Async.h"
#include "DuckPlayer.h"
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

#undef DEBUGF
#define DEBUGF(s)

#define	DITHER		0

#define	FFFRAMES	17

#define	REWFRAMES	23

bool	checkHeadBuf(player *plr);
void	initiateRead(player *plr);
void 	playerPrime(player *plr);

ScreenContext TheScreen;
//u32	bugbuf[16*1024];
//u32 	*deb = bugbuf;
u32 	*bullbuf;
u32		MaxFrameCnt=6000,ActualFrameCnt=0,ClearVal=0x08420842;

VDLEntry			*gRawVDLPtrArray[4];	// our VDLs (1 per screen)
Item				gVDLItems[4];			// new VDL items
Item				gVDLold[4];				// old VDL items
VdlChunk		*rawVDLPtr = NULL;
VDLDisplayCtlWord	gDmaWord;

ColorEcho MyCE;								// ce stuff

Item	gVRAMIOReq=0;						// SPORT used to clear screen

u32		forceheight=0;

void BugPrint(char *errmsg) {
	DEBUGF(("BUGPRINT:%s\n",errmsg));
}

void BugState(player *plr) {
	long i,j;
	DEBUGF(("Player state:\n"));
	DEBUGF(("file:[%s],size:%ld,offset:%ld\n",plr->fileName,plr->fileLength,plr->fileOffset));
	DEBUGF(("%ld Compression buffers; slop @%ld; slopsize %lx\n",
		plr->numMx,plr->slop,plr->slopSize));

for(i=0;i<plr->numMx;i++){
		DEBUGF(("[%ld]: @%p contains %ld buffers ",
			i,
			plr->bufStart[i],
			plr->buf[i].actFrames));
		DEBUGF(("out of %ld max\n",plr->buf[i].maxFrames));

		DEBUGF(("reading %lx\n",
			plr->buf[i].reading));
		for(j=0;j<plr->buf[i].actFrames;j++){
			DEBUGF(("   [[%ld]]: frameNumber:%lx, address:%lx,",
				j,
				plr->buf[i].unit[j].frameNumber,
				plr->buf[i].unit[j].address ));
			if((plr->buf[i].unit[j].flags)&FLAG_AUDBUSY) printf(" AUDBUSY "); else printf(" audbusy ");
			if((plr->buf[i].unit[j].flags)&FLAG_VIDBUSY) printf(" VIDBUSY "); else printf(" vidbusy ");
			DEBUGF(("\n"));
		}
	}
	DEBUGF(("%ld screens allocated   %ld is disp\n",plr->numScreens,plr->dispScreen));
	for(i=0;i<plr->numScreens;i++) {
		DEBUGF(("   [[%ld]]: frameNumber:%lx, address:%lx\n",
		i,
		plr->screen[i].frameNumber,
		plr->screen[i].address ));
	}
}

/*********************************************************************************

	clearScreen
	
	clear the screen indicated by the index ix	using a SPORT transfer
	
	player *plr
	u32		ix			index of the screen
	u32		pixpair		a RGBRGB vertical pixel pair
**********************************************************************************/

/*
Err clearScreen(player *plr,u32 ix,u32 pixpair) {
	Err e;
	e=SetVRAMPages(	gVRAMIOReq,
					plr->screen[ix].address,
					pixpair,
					TheScreen.sc_nFrameBufferPages,
					-1 );
	return(e);
}
*/
void clearScreen(player *plr,u32 ix,u32 pixpair) {

	int x,y;
	u32 *screen;
	u32 w,h;	

	screen=(u32*)(plr->screen[ix].address);
	for(y=0;y<120;y++) {
		for(x=0;x<320;x++) {
			*screen++=pixpair;
		}
	}

// 4-pixel border patrol:

	w=(plr->dxWH>>16)*4+8;
	h=(plr->dxWH & 0xFFFF)*2+4;
	screen=((u32*)(plr->screen[ix].address+plr->letterOff))-644;
	for(y=0;y<h;y++) {
		for(x=0;x<w;x++) {
			*screen++=0;
		}
		screen+=(320-w);
	}
}

/***********************************************************************************


DAN'S STUFF

************************************************************************************/
void drawBars(u32 *screen) {

	int x,y,pix;
	for(y=0;y<10;y++) {
		for(x=0;x<64;x++) {
			pix=x>>1;
			pix=pix+(pix<<5)+(pix<<10);
			pix+=(pix<<16);
			if(x&1) pix+=0x80008000;	// funky bit means brighter
			*screen++=pix;
			*screen++=pix;
			*screen++=pix;
			*screen++=pix;
			*screen++=pix;
		}
	}

}
#if LEAVEOUT
void whackCLUT(){
	u32 i,res,colorentry,col,dith;
#if DITHER
	dith=0;
#else
	dith=4;
#endif
	for(i=0;i<32;i++) {
		if(i>0) col=(i*8-dith); else col=0;
		if(col==120) col=118;
		if(col==16) col=14;
		colorentry = (i<<24)+(col<<16)+(col<<8)+col;
		res=SetScreenColor(TheScreen.sc_Screens[0],colorentry);
		res=SetScreenColor(TheScreen.sc_Screens[1],colorentry);
		res=SetScreenColor(TheScreen.sc_Screens[2],colorentry);
		res=SetScreenColor(TheScreen.sc_Screens[3],colorentry);
		//DEBUGF("SetScreenColor returns %x \n",res);
	}
}
#endif
//#if LEAVEOUT
void whackCLUT(){

/* set up a CLUT of 32 values betwixt default clut; note there are problems
	with some CLUT values that must be tweaked */

	u32 i,res,colorentry,col,funk,button;
	s32 value,clut,color;
	u32 customCLUT[32]={0,8,16,23,31,40,48,56,64,72,80,87,96,104,112,119,128,136,144,152,160,
	168,176,184,192,200,208,216,224,232,240,248};
//	u32 customCLUT[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//	0,0,0,0,0,0,0,0,0,0,0};
 
 
#if NOT DITHER
	for(i=1;i<32;i++) if(customCLUT[i]) customCLUT[i]-=4;	/* dither adds 3.5 on average */
#endif
	value=0;
	clut=0;
	color=0;
	button=0;
//	while(!(button & JOYSTART)) {
	while(0) {
		button=ReadControlPad(0);

/* dork clut according to button (A=red, B=green, C=blue, left/right=0 to 31;
	up/down=0 to 255 */
	
		if(button & JOYUP) {
			value++; if(value<0) value=0; if(value>255) value=255;
			DEBUGF(("value = %d\n",value));
		}
		if(button & JOYDOWN) {
			value--; if(value<0) value=0; if(value>255) value=255;
			DEBUGF(("value = %d\n",value));
		}
		if(button & JOYLEFT) {
			clut--; if(clut<0) clut=0; if(clut>31) clut=31;
			value=customCLUT[clut];
			DEBUGF(("clut = %d\n",clut));
		}
		if(button & JOYRIGHT) {
			clut++; if(clut<0) clut=0; if(clut>31) clut=31;
			value=customCLUT[clut];
			DEBUGF(("clut = %d\n",clut));
		}
		if(button & JOYFIREA) {
			value^=0x80;
			DEBUGF(("value = %d\n",value));
		}
/*		if(button & JOYFIREA) {
			color=0;
			DEBUGF(("color RED\n"));
		}
		if(button & JOYFIREB) {
			color=1;
			DEBUGF(("color GREEN\n"));
		}
		if(button & JOYFIREC) {
			color=2;
			DEBUGF(("color BLUE\n"));
		}
*/
		customCLUT[clut]=value;
		if(button&(JOYUP|JOYDOWN|JOYFIREA)) {
			for(i=0;i<32;i++) {
				col=customCLUT[i];
				colorentry = (i<<24)+(col<<16)+(col<<8)+col;
				res=SetScreenColor(TheScreen.sc_Screens[0],colorentry);
				res=SetScreenColor(TheScreen.sc_Screens[1],colorentry);
				res=SetScreenColor(TheScreen.sc_Screens[2],colorentry);
				res=SetScreenColor(TheScreen.sc_Screens[3],colorentry);
				//kprintf("SetScreenColor returns %x \n",res);
			}
			DisplayScreen(TheScreen.sc_Screens[0],0);
			DEBUGF(("customCLUT[32]={"));
			for(i=0;i<32;i++) DEBUGF(("%d,",customCLUT[i]));
			DEBUGF(("}\n"));
		}
	}
	for(i=0;i<32;i++) {
		col=customCLUT[i];
		colorentry = (i<<24)+(col<<16)+(col<<8)+col;
		res=SetScreenColor(TheScreen.sc_Screens[0],colorentry);
		res=SetScreenColor(TheScreen.sc_Screens[1],colorentry);
		res=SetScreenColor(TheScreen.sc_Screens[2],colorentry);
		res=SetScreenColor(TheScreen.sc_Screens[3],colorentry);
		//kprintf("SetScreenColor returns %x \n",res);
	}
}
//#endif

/*
 *  Open heart surgery...  Change the display control word for an active VDL 
 *	 and activate it.  return the old VDL so the caller can destroy it. 
 */
void WriteVDLCtlWords(VDLDisplayCtlWord dmaWord, int32 *activeVDLPtr,u32 i) {
	u32		scanLineEntry;
	Item		oldVDL;
	VDL_REC		*activeVDLArrayPtr = (VDL_REC *)activeVDLPtr;
	
DEBUGF(("---->active: %p   raw[i]: %p\n",activeVDLArrayPtr,gRawVDLPtrArray[i]));	
	// write the control word into each VDL entry
//	for (scanLineEntry = 0; scanLineEntry < kScreenHeight; scanLineEntry++)
//		((VDL_REC *)(&activeVDLArrayPtr[scanLineEntry]))->displayControl = LONGWORD(dmaWord);
((VDL_REC *)(&activeVDLArrayPtr[0]))->displayControl = LONGWORD(dmaWord);

	// hand our raw vdls to the system, let it validate and copy them for it's own
	//  use
	gVDLItems[i] = SubmitVDL(gRawVDLPtrArray[i], (kVDLSize / 4), VDLTYPE_SIMPLE);
	if ( gVDLItems[i] < 0 ) 
	{
#ifdef KF_DEBUG
		printf( "Error (WriteVDLCtlWords): SubmitVDL() failed with error code %d\n", gVDLItems[0] );
		PrintfSysErr(gVDLItems[i]);
#endif
	}

	// activate the new VDL
	oldVDL = SetVDL(TheScreen.sc_Screens[i], gVDLItems[i]);
	if ( oldVDL < 0 ) 
	{
#ifdef KF_DEBUG
		printf( "Error: SetVDL() failed with error code %d\n", oldVDL );
		PrintfSysErr(oldVDL);
#endif
	}	
	DEBUGF(("'old' VDL = %x\n",oldVDL));
	gVDLold[i]=oldVDL;

	// Even though we've already told the system to use the the newly modified,
	//  the hardware is still using the old one.  because the system sets a
	//  screen to black if DeleteVDL is called for an active VDL, tell the
	//  hardware to use the new one before we toss the old one
//	DisplayScreen( TheScreen.sc_Screens[0], 0 );
//	DeleteVDL(oldVDL);					// don't delete; gonna reset later
}




/*	setupVDLs(i)

	set h&v interp for frame & WINDOW!!! all pixels with msb set are window pixels;
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

bool setupVDLs(u32 i) {
	s32 errorCode;
	long ourDma;
#if DITHER
		ourDma = 0xC201702C;
#else
		ourDma = 0xC201602C;
#endif

	if ( AllocateVDL(&gRawVDLPtrArray[i], TheScreen.sc_Bitmaps[i] ) != noErr ) {
#ifdef KF_DEBUG
		printf("unable to allocate VDL\n");
#endif
		return false;
	}
	DEBUGF(("AllocateVDL returns, gRAW = %p\n",gRawVDLPtrArray[i]));
/*
	// SubmitVDL wants it's size arg in WORDS
	gVDLItems[i] = SubmitVDL(gRawVDLPtrArray[i], (kVDLSize / 4), VDLTYPE_SIMPLE);
	if ( gVDLItems[i] < 0 ) 
	{
#ifdef KF_DEBUG
		printf( "Error (setupVDLs): SubmitVDL() failed with error code %d\n", gVDLItems[0] );
#endif
		return false;
	}

	// activate the new VDL
	errorCode = SetVDL(TheScreen.sc_Screens[i], gVDLItems[i]);
	if ( errorCode < 0 ) 
	{
#ifdef KF_DEBUG
		printf( "setupVDLs: SetVDL() failed with error code %d\n", errorCode );
		PrintfSysErr(errorCode);
#endif
		return false;
	}

	gVDLold[i]=errorCode;
	DEBUGF(("orig VDL = %x\n",errorCode));
*/
	LONGWORD(gDmaWord) = ourDma;
	WriteVDLCtlWords(gDmaWord, gRawVDLPtrArray[i],i);
	DEBUGF(("VDL control word written,\t[%X]\n", LONGWORD(gDmaWord)));
		
}
	
/************************************************************************************/

bool setupScreens(player *plr,u32 screencnt)	{
	u32 i;
	TheScreen.sc_curScreen = 0;
	
	
	for(i=0; i<kNumScreens; i++) {				// fill out screens structure
		plr->screen[i].address =
			(char*)GetPixelAddress(TheScreen.sc_Screens[i],0,0);//,16,46);
		plr->screen[i].frameNumber = 0xFFFFFFFF;
	}

// set up the VDL
	for ( i = 0; i < screencnt; i++ ) setupVDLs(i);
	
// DisplayScreen(TheScreen.sc_Screens[0],0);
// drawBars((u32*)GetPixelAddress(TheScreen.sc_Screens[0],0,0));

	whackCLUT();
	
// while(!ReadControlPad(0));
	
	return TRUE;
}

/*********************************************************************************

	fakeFrameTable
	
	for now, fake a frame table for 16K frames
	later, use LoadFrameIndex for variable size frames
	
	frame table is an array containing file offset(in bytes) of each frame,
	enumerated sequentially from 1
	
	u32 cnt		number of frames in index
	
	u32*	address of frame index or null
	
**********************************************************************************/
u32 *fakeFrameTable(u32 cnt) {
	u32 i;
	u32 *p;
	p = (u32*)AllocMem(cnt*4,MEMTYPE_ANY);			// try to allocate memory from dram
	if(p) {
		for(i=0;i<cnt;i++) {
			p[i] = i*((1024*16)-32);
		}
	} else {
		BugPrint("FrameIndex.c:FakeFrameIndex could not allocate memory\n");
		return(NULL);
	}
	return(p);
}



u32 *loadFrameTable(char *framename) {
	u32 res,i;
	u32 *p;
	p = (u32*)AllocMem((MaxFrameCnt*4)+16,MEMTYPE_ANY);		// try to allocate memory from dram
	if(p) {
		for(i=0;i<MaxFrameCnt;i++){							// clear frame table
			p[i] = 0xFFFFF;
		}
		res=ReadFile(framename,MaxFrameCnt*4,(long*)p,0);
		if(res == -1) {BugPrint("FrameIndex: could not load file\n"); return(NULL);}
//DEBUGF("frametable is %ld long\n",res);
		for(i=0;i<MaxFrameCnt;i++){	
			if(p[i] == 0xFFFFF) {							// find end of table
				ActualFrameCnt=i;
				break;
			}
		}
		DEBUGF(("ActualFrameCnt=%d\n",ActualFrameCnt));
	} else {
		BugPrint("FrameIndex.c:FakeFrameIndex could not allocate memory\n");
		return(NULL);
	}
	return(p);
}
/*********************************************************************************

	loadParams
	
	load globar parameters into the beginning of the player structre
	
	ÑÑÑ should really load from disk

**********************************************************************************/
/*bool loadParams(player *plr,char *parmname) {
	plr->dataVersion = 0;		// fake version
	plr->letterXOff=16;			// hor offset in pixels for letterbox
	plr->letterYOff=40;			// vertical offset in pixels for letterbox
	plr->dxWH = ((288/4)<<16)+(140/4);		// width and height in blocks

	plr->protoDelta[0] = 1;		// funky
	plr->protoDelta[1] = -1;	// funky
	plr->protoDelta[2] = 3;		// funky
	plr->protoDelta[3] = -6;
	plr->protoDelta[4] = 6;
	plr->protoDelta[5] = -12;
	plr->protoDelta[6] = 12;
	plr->protoDelta[7] = -12;

	plr->protoCDelta[0] = 0;
	plr->protoCDelta[1] = -1;
	plr->protoCDelta[2] = 1;
	plr->protoCDelta[3] = -2;
	plr->protoCDelta[4] = 2;
	plr->protoCDelta[5] = -4;
	plr->protoCDelta[6] = 4;
	plr->protoCDelta[7] = -4;

// this is calculated, do even for disk load!!!!

	plr->letterOff=(plr->letterYOff*320*2)+(plr->letterXOff*4);  // byte offset
	return(true);
}
*/
bool loadParams(player *plr,char *paramname) {
	u32 res;
	res=ReadFile(paramname,48,(long*)&plr->dataVersion,0);
	if(res == -1) {BugPrint("loadParams: could not load"); return(false);}

	plr->letterOff=(plr->letterYOff*320*2)+(plr->letterXOff*4);  // byte offset
if(forceheight) {
	plr->letterOff=(320*(240-forceheight))+(plr->letterXOff*4);
	DEBUGF(("forceheight makes letterOff = %ld\n",plr->letterOff));
}
	return(true);
}

/*********************************************************************************

	playerOpen
	
	Initialize the player:
	
	player *	plr			player structure pointer
	char *		filename	the movie filename
	
	bool					success flag
	
**********************************************************************************/
bool	playerOpen(player *plr, char *filename,u32 samprate){
	int	n,i;
	char name[128];
	
	initAudio(samprate);				// one-stop shopping for our customers
	
	strcpy(name,"duckart/");			// ÑÑÑbullshit for disk
	strcat(name,filename);
	strcat(name,".hdr");
	if(! loadParams(plr,name)) return(false);
	
	
	strcpy(name,"duckart/");
	strcat(name,filename);
	strcat(name,".duk");
//	plr->fileName = filename;					// save filename in structure
	if(!AsyncOpen(name)) {
		BugPrint("playerOpen: unable to open block file\n");
		return false;
	}
	
	plr->fileOffset = 0;
	plr->headBuf = 0;											// for now
	plr->tailBuf = 0;

// for every mxBuf, allocate memory and fill out the bufStart field

	plr->numMx= kNumMx;							// do not use constants from now on
	for(n=0; n < plr->numMx; n++) {
		plr->buf[n].sizeBuf = kSizeMx;			// do not use constants from now on
		plr->buf[n].maxFrames = kNumX;
DEBUGF(("setting maxFrames to %lx\n",plr->buf[n].maxFrames));
		for(i=0; i<plr->buf[n].maxFrames; i++) {
			plr->buf[n].unit[i].flags = 0;			// no flags yet
			plr->buf[n].unit[i].frameNumber = 0;
			plr->buf[n].unit[i].address = NULL;
		}			
		plr->bufStart[n] =
			(char*)AllocMem(plr->buf[n].sizeBuf,MEMTYPE_ANY);
		if (!plr->bufStart[n]) {
			BugPrint("playerOpen: unable to allocate buffers\n");
			return false;
		}
		plr->buf[n].actFrames = 0;
		plr->buf[n].reading = false;
	}

// initialize frame table
	strcpy(name,"duckart/");
	strcat(name,filename);
	strcat(name,".frm");
DEBUGF(("loading frame table %s\n",name));
	plr->frameTable = loadFrameTable(name);
	if(!plr->frameTable) {
		BugPrint("playerOpen: unable to initialize frame table\n");
		return false;
	}
	
// some player-global stuff

	plr->slop = 0;
	plr->slopSize = 0;
	plr->atFrame = 0;							// last frame actually read
	plr->headBuf = 0;
	plr->tailBuf = 3;

// initialize screens (4 presently)

	strcpy(name,"duckart/");
	strcat(name,filename);
// strcpy(name,"duckart/default");
	strcat(name,".tbl");
	plr->numScreens = kNumScreens;		// do not use constants
	if(!setupScreens(plr,plr->numScreens)) {
		BugPrint("playerOpen: unable to initialize screens\n");
		return(false);
	}
	if(!gVRAMIOReq) {
		DEBUGF(("Allocating VRAMIOReq\n"));
		gVRAMIOReq = GetVRAMIOReq();				// allocate ioreq
	}
	for(n=0;n<TheScreen.sc_nScreens;n++) clearScreen(plr,n,ClearVal);
	plr->dxScreen = 0;
	plr->dispScreen = 0;				// buf
	initDX(plr->protoDelta,plr->protoCDelta,name);	// initialize decompression tables		*/
	
	plr->dxMxBuf = 0x0;
	plr->dxUnit = 0x0;
	
	plr->audMxBuf = 0;
	plr->audUnit = 0;
	plr->audFrame = 0xFFFFFFFF;
	
	plr->readHold = true;
	
	plr->showFrmNum = false;
return true;
}

/*********************************************************************************

	checkHeadBuf
	
	check headBuf to see if it is free.
	This means that all active subbuffers must have their AUD_BUSY and VID_BUSY flags
	cleared.
	
		
	player *	plr			the player
	
	u32			index or 0xFFFFFFFF if all busy
	
**********************************************************************************/
bool	checkHeadBuf(player *plr){
	u32 j;
	u32 flags;

	flags = 0;
	for(j=0; j<plr->buf[plr->headBuf].actFrames; j++) {			// for every actual buffer
		flags = flags |
				plr->buf[plr->headBuf].unit[j].flags;		// or all flags
	}
	if(flags == 0) return(true);						// found
	else return(false);
}


/*********************************************************************************

	initiateRead
	
	initiate a read into an mxBuf.
	Ñ	using the frame index, calculate number of frames that will fit
	Ñ	handle slop
	Ñ	in mxBuf, set reading flag and actFrames
	Ñ	in every bufIndexItem, set VID_BUSY and AUD_BUSY and calc addresses and frame#
	Ñ	start asynchronous read
	
	player *	plr			the player
	
	
	
	| <SECSIZE slop | frame1 | ... | frame n | <SECSIZE slop for next frame |		
		
**********************************************************************************/
void initiateRead(player *plr){
	s32 bufsize,i,cnt,temp;
	char *startaddr,*addr;

	mxBuf *theBuf;
	BufIndexItem *theUnit;
	u32		b,*f;							// for frame offset calc

	theBuf = &(plr->buf[plr->headBuf]);				// this is head mxbuffer
// copy slop
	memcpy(plr->bufStart[plr->headBuf],
			plr->slop,
			plr->slopSize);

	addr = plr->bufStart[plr->headBuf];				// addr stores bitstream addr
	startaddr =  
		addr + plr->slopSize;						// startaddr is where disk reads
	addr+=plr->skip;								// skip trash in beginning
// quantize buf to DEVICEBLOCKSIZE
	bufsize = (((theBuf->sizeBuf - plr->slopSize-plr->skip)/DEVICEBLOCKSIZE)*DEVICEBLOCKSIZE) + plr->slopSize;

	f = &plr->frameTable[plr->atFrame];				// f is a pointer into frame table
	b = *f;											// beginning frame offset
	i=0;
	while(((*(f+1))-b) <= bufsize){						// see how many frames fit
		theUnit = &theBuf->unit[i++];
		theUnit->flags = FLAG_AUDBUSY | FLAG_VIDBUSY;
		theUnit->frameNumber = plr->atFrame++;		// store frame #
		theUnit->address = addr;					// address of buffer
		addr -= (*f++)-(*(f));						// size of this frame
	}
	theBuf->actFrames = i;							// actual frames in this buffer

	plr->slop = addr;

	temp =(s32)(addr-startaddr+DEVICEBLOCKSIZE-(u32)1)/DEVICEBLOCKSIZE; 		// temp is number of sectors, actually
	cnt = temp*DEVICEBLOCKSIZE;								// cnt is byte count of full read
	plr->slopSize = cnt - ((u32)(addr-startaddr));
	theBuf->reading = true;							// reading this buffer;
	AsyncRead(startaddr,cnt,plr->fileOffset);
	plr->fileOffset+=temp;							// up current sector
	plr->skip = 0;									// no longer useful
}
/*********************************************************************************
	| <DEVICEBLOCKSIZE skip | frame |	

	loadStill
	
	load an mxBuf with a unique frame
	
	player *	plr			the player
		
**********************************************************************************/
void loadStill(player *plr){
	mxBuf *theBuf;
	BufIndexItem *theUnit;
	u32 *fp,off,cnt,frm;
	s32 rem;
	theBuf = &(plr->buf[plr->headBuf]);			// this is head mxbuffer
	theUnit = &theBuf->unit[0];					// only unit 1 counts
//DEBUGF("atframe %ld\n",plr->atFrame);
	frm = plr->atFrame;
	fp = &plr->frameTable[frm];					// fp is a pointer into frame table
//DEBUGF("frametabel entry %ld\n",*fp++);
	off = DivRemS32(&rem,(s32)*fp,DEVICEBLOCKSIZE);		// qtze to prev sector bound
	theUnit->address = plr->bufStart[plr->headBuf] + rem;
	cnt = (rem+(-(*fp++) + (*fp))+DEVICEBLOCKSIZE-1)/DEVICEBLOCKSIZE*DEVICEBLOCKSIZE;
/*DEBUGF("frame %ld. Buf %p actually %p, disk offset %lx, length %ld\n",
	frm,plr->bufStart[plr->headBuf],theUnit->address,
	off,cnt);*/
	while(!	AsyncCheck());	
	AsyncRead(plr->bufStart[plr->headBuf],cnt,off);
	while(!	AsyncCheck());
	
//QC(theUnit->address);	
}
/*********************************************************************************

seek to frame at plr->atFrame

seek to restore play peace
		
**********************************************************************************/
void playerSeek(player *plr,u32 frm){
	u32 *fp;
	plr->atFrame = frm;
	fp = &plr->frameTable[frm];					// fp is a pointer into frame table
	plr->fileOffset = *fp / DEVICEBLOCKSIZE;				// qtze to prev 2K bound
	plr->skip = *fp - (plr->fileOffset * DEVICEBLOCKSIZE);		// first frame skip value
}

/*********************************************************************************

	playerPrime
	
	pre-load all mxBufs
	
	player *	plr			the player
		
**********************************************************************************/

void playerPrime(player *plr) {
	u32		i;

	i = plr->headBuf;	

	plr->headBuf = i;								// read first buffer
	initiateRead(plr);								// nothing to do yet
	while(!	AsyncCheck());	
	plr->buf[i].reading = false;
	i++;

	plr->headBuf = i;								// read first buffer
	initiateRead(plr);								// nothing to do yet
	
	plr->dxScreen = 2;								// decompress 2 screens
	plr->lastDispScreen = 0;
	plr->dispScreen = 1;		
	playerDXBuf(plr);
	playerDXBuf(plr);
	playerAudio(plr);
	playerAudio(plr);
	playerAudio(plr);
	playerAudio(plr);
	playerAudio(plr);

	playerAudio(plr);
	playerAudio(plr);
	playerAudio(plr);
	playerAudio(plr);
	playerAudio(plr);
	
	plr->lastDispScreen = 3;
	plr->dispScreen = 0;							// now, allow dx to do its job
	while(!	AsyncCheck());	
	plr->buf[i].reading = false;
	
	plr->headBuf = i++;								// wound up all buffers, sit on first
	plr->readHold = false;							// read is done, hold for decompress
}

/*********************************************************************************

	queueScreen
	
	queue the frame for display at next VBlank
	
	scan through the screens
	do not check for head up ass
	set dispScreen to one queued
	
**********************************************************************************/
void queueScreen(player *plr,u32 framenum) {
	u32 i;

	for(i=0;i<plr->numScreens;i++) {
		if(plr->screen[plr->dispScreen].frameNumber == framenum) {
			DisplayScreen(TheScreen.sc_Screens[plr->dispScreen],0);
			unholdAudio(reasonQueue);
			return;
		} else {
			plr->lastDispScreen = plr->dispScreen++;
			plr->dispScreen &= 3;					// skip to next screen
		}
	}
	holdAudio(reasonQueue);									// give peace a chance

// DEBUGF("--> Framenum: %x\n",framenum);
// DEBUGF("--> Screen frames: %x %x %x %x\n",	plr->screen[0].frameNumber,
//											plr->screen[1].frameNumber,
//											plr->screen[2].frameNumber,
//											plr->screen[3].frameNumber);
											
//DEBUGF("A very bad case in queueScreen %ld\n",framenum);

	plr->lastDispScreen = plr->dispScreen++;
	plr->dispScreen &= 3;							// let dx try to catch up
}

/*********************************************************************************
	playerDXBuf
	
	decompress a frame
	prime to buf-1!!

	Ñ if screen queued for display or previously just queued, just return
	Ñ if buffer reading, just return
	Ñ increment dx subbuf, rotating through buffers
	Ñ decompress buffer
	Ñ clear video busy flag for this buffer
	Ñ update dxScreen
	
**********************************************************************************/
void playerDXBuf(player *plr) {
	u32	dxscr;
	char *cframe;						// points to beginning of frame data
	BufIndexItem *unit;					// pointer to this unit in mxBuf
	mxBuf *mxb;							// points to the mxBuf

	dxscr = plr->dxScreen;				// index of dxScreen
	if(dxscr == plr->lastDispScreen) return;	// can't decompress
	if(dxscr == plr->dispScreen) return;	// can't decompress

	mxb = & plr->buf[plr->dxMxBuf];
//DEBUGF(".");
	if(mxb->reading) {					// choke if reading
//		DEBUGF("playerDX hit a reading buffer %ld\n",plr->dxMxBuf);
		holdAudio(reasonDX); 
		return;
	} else unholdAudio(reasonDX);

	unit = &mxb->unit[plr->dxUnit];
	cframe = unit->address;
	duckDX((cframe + *(u32*)cframe + 8),
			plr->screen[plr->dxScreen].address+plr->letterOff,
			plr->dxWH,
			320);

	unit->flags &= FLAG_VIDBUSY_M;
	plr->screen[dxscr].frameNumber = unit->frameNumber;		// set screen frame#

	if(++plr->dxUnit >= mxb->actFrames) {	// inc buffer indices,wrapping
		plr->dxUnit = 0;
		if(++plr->dxMxBuf >= plr->numMx)
			plr->dxMxBuf = 0;
	}
if(plr->showFrmNum){
	GrafCon GCon;
	char frmstr[32];
	MoveTo ( &GCon, 30,50 );
	sprintf(frmstr,"%ld",plr->screen[plr->dxScreen].frameNumber);
	DrawText8( &GCon, TheScreen.sc_BitmapItems[plr->dxScreen], frmstr );
}
	plr->dxScreen +=1;
	plr->dxScreen &=3; //ÑÑÑ should use remainder
	
}

/*********************************************************************************

	isDoneReading
	
	Ñ check to see if read is finished
	Ñ if so, update head mxBuffer index
	
	return:	bool indicating whether last read done or not
	
**********************************************************************************/

bool isDoneReading(player *plr) {
	if(AsyncCheck()) {	// only if last read finished,
		plr->buf[plr->headBuf].reading = false;	// mark mxBUF as no longer reading
		plr->headBuf++;							// update head buffer
		plr->headBuf &=3;						//ÑÑÑ should use remainder
		return(true);
	} else {
		return(false);
	}
}	

/*********************************************************************************

	allDone
	
	Ñ wait till all outstanding reads are done
	Ñ clear all flags
	
	
**********************************************************************************/

void allDone(player *plr) {
	u32 i,j;
	while(!	AsyncCheck);			// wait till finished
	for(i=0;i<plr->numMx;i++){						// for every mxBuf,     
		plr->buf[i].reading = false;
		plr->buf[i].actFrames = 0;
		for(j=0;j<plr->buf[i].maxFrames;j++) {
			plr->buf[i].unit[j].flags = 0;			// clear all fags
		}
	}

	plr->slop = 0;
	plr->slopSize = 0;

	plr->headBuf = 0;
	plr->tailBuf = 3;
	plr->dxScreen = 0;
	plr->dispScreen = 0;				// buf
	plr->lastDispScreen = 3;
	plr->dxMxBuf = 0x0;
	plr->dxUnit = 0x0;
	
	plr->audMxBuf = 0;
	plr->audUnit = 0;
	plr->audFrame = 0xFFFFFFFF;
	
	plr->readHold = true;
}	

/*********************************************************************************

	playerRead
	
	if coming from hold,
		if buffer available,
			start a new read
	else
		if done reading
			if buffer available
				start a new read
			else
				set hold state

	return:	bool indicating whether last read done or not
	
**********************************************************************************/

void playerRead(player *plr) {
	if(plr->readHold) {							// if hold is on,
		if(checkHeadBuf(plr)) {					// see if still busy
			initiateRead(plr);					// no, initiate a new read (last read is done)
			plr->readHold = false;				// and get rid of hold state
		}
	} else {									// no hold,
		if(isDoneReading(plr)) {				// see if finished last read,
			if(checkHeadBuf(plr)) {				// yes, is buffer available?
				initiateRead(plr);				// yes, start a new read
			} else {
				plr->readHold = true;			// buffer not ready, set hold state
			}
		}
	}											
}


void playerClose(player *plr) {
	u32 i,n,VDLsize;
	Item newVDL;

	VDLsize=240*sizeof(VDL_REC);
	
	AsyncClose();
	
	uninitAudio();
	
	for ( i = 0; i < plr->numScreens; i++ )
	{
		newVDL = SetVDL(TheScreen.sc_Screens[i], gVDLold[i]);
		DEBUGF(("deleting new VDL %x, which was %x; set old VDL %x\n",
				newVDL,gVDLItems[i],gVDLold[i]));
		DeleteVDL(gVDLItems[i]);
		DEBUGF(("Deallocating VDL at %p size %x\n",gRawVDLPtrArray[i],VDLsize));
		FreeMem(gRawVDLPtrArray[i],VDLsize);
	}
	for(n=0; n < plr->numMx; n++) {
		DEBUGF(("deallocating buffer at %p\n",plr->bufStart[n]));
		FreeMem(plr->bufStart[n],plr->buf[n].sizeBuf);
	}
	DEBUGF(("deallocating frame table at%p\n",plr->frameTable));
	FreeMem(plr->frameTable,(MaxFrameCnt*4)+16);
	
/*	QL(bugbuf);
	QL(bugbuf+64);
	QL(bugbuf+128);
	QL(bugbuf+128+64);
	QL(bugbuf+256);
*/
}

/*********************************************************************************

	playerAudio
	
	attempt to move audio part of subbufer to audio buffer area
	if successful, release busy bit for that sub
	maintain state infor for audio
	
**********************************************************************************/
void playerAudio(player *plr){
	BufIndexItem *myUnit;
	if(plr->buf[plr->audMxBuf].reading) return;			// choke if reading
	myUnit = &(plr->buf[plr->audMxBuf].unit[plr->audUnit]);
	if(myUnit->flags & FLAG_AUDBUSY) {					// if flag set,
		if(stuffAudio(myUnit->address+8,				
		  *(u32*)myUnit->address,
		  myUnit->frameNumber)) {						// if stuff success

			myUnit->flags &= FLAG_AUDBUSY_M;			// clear busy flag for unit
// inc buffer indices,wrapping
			if(++plr->audUnit >= plr->buf[plr->audMxBuf].actFrames) {
				plr->audUnit = 0;
				if(++plr->audMxBuf >= plr->numMx)
					plr->audMxBuf = 0;
			} // no need to inc buffers
		} // could not stuff audio
	} // buffer not busy (either sent already or reached all way round
}
/*********************************************************************************

	doState
	
	update player state information
	
	
**********************************************************************************/
void doState(player *plr,u32 cnt,u32 ctl) {
	static bool debounce=false;
	switch(ctl) {
		case JOYSTART:
			if(debounce) break;
			else debounce = true;
			switch(plr->state) {
				case STATE_STOP:
					plr->state = STATE_PLAY;
					break;
				case STATE_PLAY:
					plr->state = STATE_PAUSE;
					break;
				case STATE_PAUSE:
					unholdAudio(0xFFFFFFFF);
					plr->state = STATE_PLAY;
					break;
				case STATE_STILL:
					stopAudio();
					flushAudio();
					allDone(plr);
					playerSeek(plr,plr->atFrame);
					playerPrime(plr);
					startAudio();
					plr->state = STATE_PLAY;
					break;
				case STATE_QUIT:
					break;
			}
			break;
		case JOYLEFT:
			switch(plr->state) {
				case STATE_PLAY:					// from play to still
					plr->atFrame = plr->screen[plr->dispScreen].frameNumber;
					break;
			}
			plr->state = STATE_STILL;
/*DEBUGF("%lx\n",plr->frameTable[plr->atFrame]);
DEBUGF(("%lx\n",plr->frameTable[plr->atFrame-1]));*/
plr->atFrame+=-1;
			if(plr->atFrame < 0) plr->atFrame = 0;
			break;
		case JOYRIGHT:
			switch(plr->state) {
				case STATE_PLAY:					// from play to still
					plr->atFrame = plr->screen[plr->dispScreen].frameNumber;
					break;
			}
			plr->state = STATE_STILL;
			if(plr->frameTable[plr->atFrame++] == 0xFFFFF) plr->atFrame--;
			break;
		case JOYUP:
			switch(plr->state) {
				case STATE_PLAY:					// from play to still
					plr->atFrame = plr->screen[plr->dispScreen].frameNumber;
					break;
			}
			plr->state = STATE_STILL;
			plr->atFrame+=FFFRAMES;
			if(plr->atFrame >= cnt) plr->atFrame = cnt-1;
			break;
		case JOYDOWN:
			switch(plr->state) {
				case STATE_PLAY:					// from play to still
					plr->atFrame = plr->screen[plr->dispScreen].frameNumber;
					break;
			}
			plr->state = STATE_STILL;
			plr->atFrame-=REWFRAMES;
			if(plr->atFrame < 0) plr->atFrame = 0;
			break;
		case JOYFIREB:
			if(debounce) break;
			else debounce = true;
			plr->showFrmNum = plr->showFrmNum?false:true;
			break;
		case JOYFIREC:
DEBUGF(("QUIT COMMAND RECEIVED!!!\n"));
			plr->state = STATE_QUIT;
			break;
		default:
			debounce = false;
	}

}

/*********************************************************************************

	playerDo
	
	test main loop
	
	
**********************************************************************************/
u32 playerDo(player *plr,u32 ctl){
	u32 i;
	
	switch(plr->state) {
		case STATE_STOP:
			break;
		case STATE_STILL: {
			char *a,*p;

			holdAudio(reasonOther);
			DisplayScreen(TheScreen.sc_Screens[plr->dispScreen],0);			// disp old
			if(++plr->dispScreen >= plr->numScreens) plr->dispScreen = 0;	// update
			loadStill(plr);
			p = plr->buf[plr->headBuf].unit[0].address;
			a = (p+*(u32*)p+8);
			duckDX(a,plr->screen[plr->dispScreen].address+plr->letterOff,
					plr->dxWH,
					320);
if(gdbg) drawBars((u32*)(plr->screen[plr->dispScreen].address+plr->letterOff));
			if(plr->showFrmNum){
				GrafCon GCon;
				char frmstr[32];
				MoveTo ( &GCon, 30,50 );
				sprintf(frmstr,"%ld",plr->atFrame);
				DrawText8( &GCon, TheScreen.sc_BitmapItems[plr->dispScreen], frmstr );
			}
//	if(++plr->headBuf >= plr->numMx) plr->headBuf=0;	// select new headbuf
			break; }
		case STATE_PLAY:
			playerRead(plr);
			queueScreen(plr,getAudioTag());
			playerAudio(plr);
			playerDXBuf(plr);
//ce_DrawNextScreen(&TheScreen, &MyCE );

			break;
		case STATE_PAUSE:
			holdAudio(reasonOther);
			break;
	}				
	doState(plr,ActualFrameCnt,ctl);
	if(plr->state != STATE_QUIT) { // do not bother to check for end if over
		if(plr->screen[plr->dispScreen].frameNumber >= ActualFrameCnt) {
			plr->state = STATE_QUIT;				// quit at end
		}
	}
	if(plr->state==STATE_QUIT) return(0);
	else return(0xFFFFFFFF);
}


/*********************************************************************************

	playerStart
		
**********************************************************************************/
void playerStart(player *plr,u32 cnt){
	u32 i;
	
	if(cnt>ActualFrameCnt-48) cnt=ActualFrameCnt-48;			// assume 48 padded frames
	if(cnt==0) cnt=ActualFrameCnt-48;							// bit'o'kludge, so
	ActualFrameCnt=cnt;
	i=0;														// we can use big count
	allDone(plr);												// to play whole file
	playerSeek(plr,0);
//DEBUGF("deb is %p\n",deb);
	playerPrime(plr);
	startAudio();
	plr->state = STATE_PLAY;
}

/*********************************************************************************

	playerStop
		
**********************************************************************************/

void playerStop() {
	stopAudio();
}

/*********************************************************************************/


void playerPlay(player *plr,u32 cnt) {
	u32 playctl;
	
	playerStart(plr,cnt);
	playctl=ReadControlPad(0xFFFFFFFF);
	while(playerDo(plr,playctl)) {
		playctl=ReadControlPad(0xFFFFFFFF);
	}
	playerStop();
}



/*{
u32 cpad;
cpad = ReadControlPad(0xFFFFFFFF);
if(cpad) = 0x80 )

DEBUGF("%lx\n",OH NO!);
}

*/







