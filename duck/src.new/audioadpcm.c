/***************************************************************
**
**
** AudioAdpcm.c
**
** by Dan Miller
**
** (c) 1993, 1994 The Duck Corporation
**
** PROPRIETARY AND YET CONFIDENTIAL!!! DO NOT TOUCH, MUTILATE OR DISTRIBUTE!!!!!
**
** audio glue for DUCK 3DO PLAYER
**
** 2/27/94 -- fixes at Crystal Dynamics: created stuffHold() to fix pause/datarate bug
**
** presently used DSPP code: adpcmduck.ins34
**
** fixes for 3DO -- new header, no knob controls (2/14/94)
**
** DAN test stereo  .. InitAudio(samprate)
** AudioAdpcm.c2	-- Finally got a reacharound...
** AudioAdpcm.c1	-- Audio glue for Duck player
** adpcmtest.c10 -- InitAudio with all that portends...
** adpcmtest.c9 -- cleaned up & commented -- adpcmduck.ins20
** adpcmtest.c8 -- debugged packet header -- works with adpcmduck.ins19
**					Uses second magic word to stop DSP for debugging
** adpcmtest.c7 -- packet header -- works with adpcmduck.ins18
** adpcmtest.c6 -- separate malloc() & makesamp()
** adpcmtest.c5 -- dsp code gets input from FIFO, outputs to DAC
** adpcmtest.c4 -- negate instead of >>1
** adpcmtest.c3 -- dsp outputs directly to DAC's
**
***************************************************************/

/*
** 921116 PLB Modified for explicit mixer connect.
** 921118 PLB Added ChangeDirectory("/remote") for filesystem.
** 921202 PLB Converted to LoadInstrument and GrabKnob.
** 921203 PLB Use AUDIODATADIR instead of /remote.
** 930315 PLB Conforms to new API
*/

#include "types.h"
#include "filefunctions.h"
#include "debug.h"
#include "operror.h"
#include "stdio.h"

#include "stdlib.h"

#include "Portfolio.h"
#include "Init3DO.h"
#include "Parse3DO.h"
#include "Utils3DO.h"
#include "filestream.h"
#include "mem.h"

#include "kernelnodes.h"
#include "nodes.h"
#include "folio.h"
#include "item.h"
#include "event.h"
#include "codec.h"
#include "films.h"
#include "device.h"

#include "ducktypes.h"

#include "duckDebug.h"

/* Include this when using the Audio Folio */
#include "audio.h"

#undef DEBUGF
#define DEBUGF(s)
//#define DEBUGF(s) { printf s; }
#define DEBUGFLAG 0

#define NUMCHANNELS (1)
//#define NUMSAMPLES (32768)
#define NUMSAMPLES (50000)
#define SAMPSIZE (sizeof(int16)*NUMSAMPLES*NUMCHANNELS)

#define	PRT(x)	{ printf x; }
#define	ERR(x)	PRT(x)
#define	DBUG(x)	PRT(x)
#define BUTTONS ReadControlPad(0)

#define DspAck 0x09
#define DspTag 0x0A
#define BADDSPTAG 0xFFF0
#define LOGTBLSIZE 89

#define AUDSTART 1
#define AUDSTOP 0
#define AUDHOLD 0x101

#define	TAGLISTSIZE 200

/* Define Tags for StartInstrument */

/* Macro to simplify error checking. */
#define CHECKRESULT(val,name) \
	if (val < 0) \
	{ \
		Result = val; \
		ERR(("Failure in %s: $%x\n", name, val)); \
/*		PrintfSysErr(Result); */ \
		goto cleanup; \
	}

extern u32 *deb,bugbuf[];
u32 taglist[TAGLISTSIZE];
u32 lastAudCnt=0;
u32 lastAudTag=0;

Item SampleItem = 0;
Item Attachment = 0;
Item PlayControl = 0;
Item InitValue = 0;
int32 i, Result = -1, holdFlag=0;
int16 *AudData,*Dataend,*Databegin,*Datalast,*clean,temp;
TagArg Tags[10];
Item ourins = 0;
Item ourmix = 0;
int32 AddTbl[8]={-1,-1,-1,-1,2,4,6,8};
int32 LogTbl[89]={

0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xF, 
0x10, 0x12, 0x13, 0x15, 0x17, 0x1A, 0x1C, 0x1F, 
0x22, 0x26, 0x29, 0x2E, 0x32, 0x37, 0x3D, 0x43, 
0x4A, 0x51, 0x59, 0x62, 0x6C, 0x76, 0x82, 0x8F, 
0x9E, 0xAD, 0xBF, 0xD2, 0xE7, 0xFE, 0x117, 0x133, 
0x152, 0x174, 0x199, 0x1C2, 0x1EF, 0x220, 0x256, 0x292, 
0x2D4, 0x31D, 0x36C, 0x3C4, 0x424, 0x48E, 0x503, 0x583, 
0x610, 0x6AC, 0x756, 0x812, 0x8E1, 0x9C4, 0xABE, 0xBD1, 
0xCFF, 0xE4C, 0xFBA, 0x114D, 0x1308, 0x14EF, 0x1707, 0x1954, 
0x1BDD, 0x1EA6, 0x21B7, 0x2516,
0x28CB, 0x2CDF, 0x315C, 0x364C, 
0x3BBA, 0x41B2, 0x4844, 0x4F7E,
0x5771, 0x6030, 0x69CE, 0x7463, 
0x7FFF
};

void startAudio(void);
bool stuffHold(void);

#ifdef nocompile
void QC(void *p) {
	int i,j;
	char *poin;
	
	poin = (char*)p;
	for(j=0;j<16;j++) {
		printf("%p: ",poin);
		for(i=0;i<16;i++) {
			printf("%2x ",*poin++);
		}
		printf("\n");
	}
}
#endif

void showbug() {
	int32 eoB,eoC,eoD,eoE,eoF;
//	eoA = DSPReadEO(0x0A);
	eoB = DSPReadEO(0x0b);
	eoC = DSPReadEO(0x0C);
	eoD = DSPReadEO(0x0d);
	eoE = DSPReadEO(0x0E);
	eoF = DSPReadEO(0x0f);
	printf("0x30B: %x %x %x %x %x \n",eoB,eoC,eoD,eoE,eoF);
}

/*********************************************************************************

void clearTaglist()
	
**********************************************************************************/

void clearTaglist() {
		u32 i;
		for(i=0;i<TAGLISTSIZE;i++) {
			taglist[i]=BADDSPTAG;
		}
	}

/*********************************************************************************

	initAudio
	
	Initialize DSPP ADPCM code & buffer, get ready to play
	
	takes: samprate		22		22.05 khz mono
						44		44.1 khz mono
						45		44.1 khz stereo 
	
**********************************************************************************/

void initAudio(int32 samprate) {
	int32 index,i,result;
	
DEBUGF(("Initializing audio now -- bugbuf=%x\n",bugbuf));

/* load our DSP code as an instrument */

	if(samprate==44) {
		DEBUGF(("DSPP adpcmduck44m\n"));
		ourins = LoadInstrument("adpcmduck44m.dsp",(Item) NULL,(uint8)100);
	} else if(samprate==45) {
		DEBUGF(("DSPP adpcmduck44s.dsp\n"));
		ourins = LoadInstrument("adpcmduck44s.dsp",(Item) NULL,(uint8)100);
	} else if(samprate==22) {
		DEBUGF(("DSPP adpcmduck22m.dsp\n"));
		ourins = LoadInstrument("adpcmduck22m.dsp",(Item) NULL,(uint8)100);
	} else if(samprate==23) {
		DEBUGF(("DSPP = adpcmduck22s.dsp\n"));
		ourins = LoadInstrument("adpcmduck22s.dsp",(Item) NULL,(uint8)100);
	} else {
		printf("unsupported sample rate!\n");
	}

// printf("dsp from remote\n");	ourins = LoadInstrument("dspp/output.dsp",(Item) NULL,(uint8)100);
DEBUGF(("LoadInstrument returns %x\n",ourins));
	TraceAudio(0);
 
//	AudData = (int16 *)malloc ( SAMPSIZE+8 );		/* 8 byte header room */
//	printf("malloc returns %p \n",AudData);
	AudData = (int16 *)AllocMem( SAMPSIZE+8,MEMTYPE_DRAM);	/* 8 byte header room */
	DEBUGF(("malloc returns %p \n",AudData));
	Databegin=AudData;
	Dataend=Databegin+NUMSAMPLES;					/* for stuffAudio */

	Tags[0].ta_Tag = AF_TAG_CHANNELS;
	Tags[0].ta_Arg = (int32 *) NUMCHANNELS;
	Tags[1].ta_Tag = AF_TAG_SUSTAINBEGIN;			/* loop from beginning */
	Tags[1].ta_Arg = (int32 *) 0;
	Tags[2].ta_Tag = AF_TAG_SUSTAINEND;				/* until the end */
	Tags[2].ta_Arg = (int32 *) (NUMSAMPLES);		/* NEVER -1 !!!!!!!!!!!!!!!!!!!!
														NEVER NEVER NEVER !!!!!!! */
	Tags[3].ta_Tag = AF_TAG_NUMBYTES;				/* define size */
	Tags[3].ta_Arg = (int32 *) SAMPSIZE;
	Tags[4].ta_Tag = AF_TAG_ADDRESS;				/* and address of our sample */
	Tags[4].ta_Arg = (int32 *) AudData;
	Tags[5].ta_Tag = TAG_END;						/* abada that's all folks! */
	
	SampleItem = MakeSample ( 0 , Tags);			/* 0 means use our ADDR & DATA */
	CHECKRESULT(SampleItem,"MakeSample");
	
/* Attach the sample to the instrument. */
	Attachment = AttachSample(ourins, SampleItem, "Input");
	CHECKRESULT(Attachment,"AttachSample");

/* clean data area */

	clean=AudData;
	temp=0x7FFE;
	for (i=0; i<NUMSAMPLES*NUMCHANNELS; i++) {
		*clean++ = temp;
	}

/* read data file to play */

//	Result=ReadFile("DuckSound/testdsp.duk",SAMPSIZE,(void *)AudData,0);
	
/* get knobs */

	PlayControl=GrabKnob(ourins,"PlayControl");	
	if(PlayControl>0) {
	
/* oldstyle init, may be unnecessary with new DSPP code: */

		InitValue=GrabKnob(ourins,"InitValue");	
		TweakKnob(PlayControl,0);					/* stop */
		
	/* start decoder */
	
	//	Result=StartInstrument(ourmix,0);
	//	printf("Start returns %lx\n",Result);
		Result=StartInstrument(ourins,0);
		DEBUGF(("Start returns %lx\n",Result));
	
	/* initialize data tables in DSPP */
	
		for(i=0;i<LOGTBLSIZE;i++) {							/* LogTbl only */
			TweakKnob(PlayControl,0);
			while(DSPReadEO(DspAck)!=0);					/* wait for ack */
			index=0x8000+i;
			TweakKnob(InitValue,LogTbl[i]);					/* value to initialize */
			TweakKnob(PlayControl,index);					/* init | index */
			while(DSPReadEO(DspAck)!=index);				/* What I mean is, you have to
															   inter-fung here */
		}
		
	//	TweakKnob(PlayControl,0);							/* stop */
	//	while(DSPReadEO(DspAck)!=0);						/* wait for ack */
		
		TweakKnob(PlayControl,AUDSTART);					/* start something */
		while(DSPReadEO(DspAck)!=AUDSTART);					/* wait for ack */

		StopInstrument(ourins,0);
	}
	
	clearTaglist();
	
/* Do a little dance, then you drink a little water */

DEBUGF(("Audio init completed\n"));
	return;

cleanup:
/* The Audio Folio is immune to passing NULL values as Items. */
	DetachSample( Attachment );
	UnloadInstrument( ourins );
	
	CloseAudioFolio();
	printf("Major problem in InitAudio \n");


}

/*********************************************************************************

	uninitAudio()
	
	stop, unattach, and de-allocate
	
**********************************************************************************/

uninitAudio() {
	UnloadInstrument( ourins);
	FreeMem(Databegin, SAMPSIZE+8);
}

/*********************************************************************************

	getAudioTag()
	
	get tag most recently seen by DSP
	
**********************************************************************************/

u32 getAudioTag(void) {
	u32 tag,att;

//	if(holdFlag) tag=lastAudTag;
//	else {
//		att=WhereAttachment(Attachment);
//		tag=taglist[att/lastAudCnt];
//		lastAudTag=tag;
//	}

	att=WhereAttachment(Attachment);
	tag=taglist[att/lastAudCnt];
	if(tag==BADDSPTAG) tag=lastAudTag;
	lastAudTag=tag;

#if DEBUGFLAG
 if((deb-bugbuf)>=4096) deb=bugbuf;
//printf("bugg\n");
 *deb++=0xaaaaaaaa;
 *deb++=tag;
 *deb++=att;
 *deb++=holdFlag;
 *deb=0xffffffff;
// printf("Audio tag is %ld\n",tag);
#endif

	return(tag);
}


/*********************************************************************************

	startAudio
	
	Start audio playing
	
**********************************************************************************/

void startAudio() {
	int32 result,oldtag,chktag;
	
	oldtag=getAudioTag();
	holdFlag=0;
#if DEBUGFLAG
 *deb++=0xbbbbbbbb;
 *deb++=oldtag;
 *deb++=0;
 *deb++=holdFlag;
#endif
//	result=TweakKnob(PlayControl,AUDSTART);							/* play */
	result=StartInstrument(ourins,0);
	while((chktag=getAudioTag())==oldtag) {
#if DEBUGFLAG
 *deb++=0xabcdef00;
 *deb++=chktag;
 *deb++=0;
 *deb++=holdFlag;
#endif
}

							/* wait for different tag */
	while((chktag=getAudioTag())==BADDSPTAG);						/* and let it be good */
// printf("startAudio tag = %ld\n",chktag);
}

/*********************************************************************************

	holdAudio
	
	
	
**********************************************************************************/

void holdAudio(u32 reason) {
	int32 result;
	u32 oldFlag;
	
	oldFlag = holdFlag;
	holdFlag |= reason;
	if(!oldFlag) {
//		result=TweakKnob(PlayControl,AUDHOLD);				/* hold */
//		result=StopInstrument(ourins,0);
		stuffHold();
//		DEBUGF(("holdAudio reason: %x\n",reason));
//		clearTaglist();
#if DEBUGFLAG
 *deb++ = 0xcccccccc;
 *deb++ = reason;
 *deb++ = 0;
 *deb++ = holdFlag;
#endif


	}
}

/*********************************************************************************

	unholdAudio
	
	Start audio playing again
	
**********************************************************************************/

void unholdAudio(reason) {
	int32 result;
	u32 oldFlag;

	oldFlag = holdFlag;
	holdFlag = 	holdFlag &(0xFFFFFFFF ^ reason);		// clear reason flag
	if(oldFlag && (!holdFlag)) {
		holdFlag=0;
//		result=TweakKnob(PlayControl,AUDSTART);					/* play */
//		result=StartInstrument(ourins,0);
//		DEBUGF(("unholdAudio reason: %x\n",reason));
#if DEBUGFLAG
 *deb++ = 0xdddddddd;
 *deb++ = reason;
 *deb++ = 0;
 *deb++ = holdFlag;
#endif

	}
}

/*********************************************************************************

	stopAudio
	
	Start audio not playing
	
**********************************************************************************/

void stopAudio() {
	int32 result;
	
//	result=TweakKnob(PlayControl,AUDSTOP);					/* play not */
	result=StopInstrument(ourins,0);
}


/*********************************************************************************

	stuffAudio
	
	stuff audio buffer with a frame of lovin' audio
	
	void *audbits	actual audio bitstream
	u32 audcnt		byte count of audio frame to be stuffed
	u32 tag			presently frame number
	
	return	true if move successful and audio busy may be released
	
	for now, big buffer, so reacharound is A-OK (must be greater than maximum video
	read-ahead)
	
	our audio header:
	
	u16		magic
	u16		numsamp
	u16		tag
	u16		firstindexL
	u16		firstindexR
	
	Format for DSPP:
	
	u16		magic
	u8		firstindexL
	u8		firstindexR
	data...
	
**********************************************************************************/
extern u32				gdbg;

bool stuffAudio(char *audbits,u32 audcnt,u32 tag) {
	int i;
	int16 *src;
	u32 *data32,*src32,off;

// DEBUGF(("StuffAudio: audcnt=%d\n",audcnt));

/* put tag in taglist */

	Datalast=(int16*)audbits;
	off=((char*)AudData)-((char*)Databegin);
	lastAudCnt=audcnt-6;
	if(off>lastAudCnt*TAGLISTSIZE) printf("taglist overflow! -- %d\n",off);
	else {
		taglist[off/lastAudCnt]=tag;
		taglist[off/lastAudCnt+1]=tag;
	}
	src = (int16 *)audbits;
	
/* put L & R samples into hi & lo byte after Magik: */

	src[1]=(int16) (src[3]<<8) + src[4];

// DEBUGF ("src[1] = %x\n",src[1]));

/* copy header */

	*AudData++ = *src++; if(AudData==Dataend) AudData=Databegin;
	*AudData++ = *src++; if(AudData==Dataend) AudData=Databegin;
	src++;
	src++;
	src++;
	audcnt-=10;
	
// *deb++ = 0xbbbbbbbb;
// *deb++ = (u32)audbits;
// *deb++ = (u32)AudData;
// *deb++ = *( ((u32 *)src) +1);

/*	if(!(audcnt & 3)) {		*/
	if(0) {
// DEBUGF(("yes!\n"));
		data32=(u32*)AudData;
		src32=(u32*)src;
		for(i=0; i<(audcnt>>2); i++) {
			*data32++ = *src32++;
			if(data32==(u32*)Dataend) {
				data32=(u32*)Databegin;				/* reacharound to the top */
// *deb++ = 0xbbbbbbbb;
// *deb++ = (u32)audbits;
// *deb++ = (u32)AudData;
// *deb++ = *( ((u32 *)src) +1);

			}
		}
		AudData=(int16*)data32;
	} else {
// DEBUGF(("no!\n"));
		for(i=0; i<(audcnt>>1); i++) {
			*AudData++ = *src++;
			if(AudData==Dataend) {
				AudData=Databegin;				/* reacharound to the top */
			}
		}
	}
	
// DEBUGF(("stuffAudio called with buffer %p,cnt %ld, tag %ld\n",
//	audbits,audcnt,tag));

	return (true);
}

/*********************************************************************************

	StuffHold
	
	stuff audio buffer with nothing in particular to hold audio clock
char *audbits,u32 audcnt,u32 tag		
**********************************************************************************/

bool stuffHold() {
//	stuffAudio((char*)Datalast,lastAudCnt+6,lastAudTag);
	char *audbits;
	u32 audcnt;
	u32 tag;
	int i;
	int16 *src;
	u32 *data32,*src32,off;

	audbits=(char*)Datalast;
	audcnt=lastAudCnt+6;
	tag=lastAudTag;
/* put tag in taglist */

	off=((char*)AudData)-((char*)Databegin);
	if(off>lastAudCnt*TAGLISTSIZE) printf("taglist overflow! -- %d\n",off);
	else {
		taglist[off/lastAudCnt]=tag;
		taglist[off/lastAudCnt+1]=tag;
	}
	src = (int16 *)audbits;
	
/* put L & R samples into hi & lo byte after Magik: */

	src[1]=(int16) (src[3]<<8) + src[4];

/* copy header */

	*AudData++ = *src++; if(AudData==Dataend) AudData=Databegin;
						 if(src==Dataend) src=Databegin;
	*AudData++ = *src++; if(AudData==Dataend) AudData=Databegin;
						 if(src==Dataend) src=Databegin;
	src++;				 if(src==Dataend) src=Databegin;
	src++;				 if(src==Dataend) src=Databegin;
	src++;				 if(src==Dataend) src=Databegin;
	audcnt-=10;
	
// *deb++ = 0xbbbbbbbb;
// *deb++ = (u32)audbits;
// *deb++ = (u32)AudData;
// *deb++ = *( ((u32 *)src) +1);

	for(i=0; i<(audcnt>>1); i++) {
		*AudData++ = *src++;
		if(AudData==Dataend) AudData=Databegin;
		if(src==Dataend) src=Databegin;
	}
	return;
}

/*********************************************************************************

	flushAudio
	
	reset audio buffer pointer so we can stuff stuff anew
		
**********************************************************************************/

bool flushAudio() {
	AudData=Databegin;				/* reacharound to the top */
	clean=AudData;
	temp=0xFFF0;
	for (i=0; i<NUMSAMPLES*NUMCHANNELS; i++) {
		*clean++ = temp;
	}
	return (true);
}
