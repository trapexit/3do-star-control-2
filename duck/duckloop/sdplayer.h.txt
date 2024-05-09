/*
	File:		DuckPlayer.h

	Contains:	the Duck player 

	Copyright:	© 1993 by The Duck Corporation. All rights reserved.
				This material constitutes confidential and proprietary
				information of The Duck Corporation and shall not be used by
				any Person or for any purpose except as expressly
				authorized in writing by The Duck Corporation.


	Change History (most recent first):

	1	 9/10/93	vy		first checked in

	To Do:
*/

#include "DuckTypes.h"
#include "Async.h"

// old
#define	kNumberOfBuffers 32		// Number of buffers
#define	kBufferSize 16*1024		// Size of each individual buffer

// new
#define kSizeMx 128*1024		// each mxBuf is this big
#define kNumX 16				//  frames one mxBuf can hold
#define kNumMx 4				// number of multiple-compressed-frame buffers

#define	kNumScreens 4			//  number of screen buffers
#define kSizeScreen 153600		//	each screen is this big

#define kNumAud	8				//	number of aud frames in buffer
#define kSizeAud 32*1024		//	each audio frame buffer this big

#define FLAG_AUDBUSY	1		// audio busy, set upon read, cleared when aud done
#define FLAG_AUDBUSY_M	0xFFFFFFFE
#define FLAG_VIDBUSY	2		// video busy
#define FLAG_VIDBUSY_M	0xFFFFFFFD

/*************************************************************************************

	BufIndexItem is used to associate a frame number to a buffer address.
	============

*/
typedef struct BufIndexItem {
	u32		flags;						// audbusy,vidbusy
	u32		frameNumber;				// frame number
	char	*address;					// address of buffer
} BufIndexItem;

/************************************************************************************/
/*************************************************************************************

	mxBuf is used maintain buffers containing multiple compressed frames.
	=====

*/
typedef struct mxBuf{
	u32				sizeBuf;				// each mxBuf this big
	u32				maxFrames;				// maximum number of frames
	u32				actFrames;				// actual number of frames
	BufIndexItem	unit[kNumX];			// frames in this buffer (maximum)
	bool			reading;				// a read has been requested for this buffer
	bool			pad1;
	bool			pad2;
	bool			pad3;
}	mxBuf;
/************************************************************************************/

/*************************************************************************************

	mAuds is used maintain multiple audio buffers
	========

*/
typedef struct	mAuds{
	u32				numScreens;					// number of screens
	u32				sizeAud;					// each buffer this big
	BufIndexItem	screen[kNumScreens];			// screen buffers
}	mAuds;

/*

	player is the main structure containing the state of the player
	======

*/

typedef struct player	{
// this section comes from the file directly
	u32			letterOff;				// calculated byte offset for letterbox start

// file stuff
	BlockFile	bf;
	char 		*fileName;				// File that we are reading from
	u32			fileLength;				// Length of file we are reading from
	u32			fileOffset;
		
// disk buffer info
	u32			numMx;					// number of multiple compression buffers
	char		*bufStart[kNumMx];		// start of each mxBuf
	mxBuf		buf[kNumMx];			// the mx buffers themselves
	u32			headBuf;				// index of next buffer to read into
	u32			tailBuf;				// index of tail buffer for dx
	char		*slop;					// slop belongs in the next buffer
	u32			slopSize;				// size of slop to copy manually
	u32			skip;					// crap before real data starts

// compression task state
	u32			dxMxBuf;					// index of mxBuf to decompress from
	u32			dxUnit;					// index of unit to decompress from

// audio task state
	u32			audMxBuf;				// index of mxBuf audio is in
	u32			audUnit;				// index
	u32			audFrame;				// last audio frame sent
	
// frame table
	u32			*frameTable;			// frame table contains disk offsets for frames
	s32			atFrame;				// last frame # read
	s32			frameTotal;				// total number of frames
// screen infor
	u32			numScreens;					// number of screens
	u32			dispScreen;					// just queued for display
	u32			lastDispScreen;				// last queued for display
	u32			dxScreen;					// decompress here
	BufIndexItem	screen[kNumScreens];		// screen buffers

// player state
	u32			state;					// as defined below
	s32			stillStep;				// size of step for still
	bool		readHold;				// last read held by stream tasks
	bool		showFrmNum;
	bool		pad2;
	bool		pad3;
	
// loaded from disk
	u32			dataVersion;			// just for verification
	u32			letterXOff;				// hor offset in pixels for letterbox
	u32			letterYOff;				// vertical offset in pixels for letterbox
	u32			dxWH	;				// width << 16 + height
	s16			protoDelta[8];			// array of 8 protodeltas
	s16			protoCDelta[8];			// these are for chroma
}	player;
	
	

/************************************************************************************/

#define STATE_STOP 	0
#define STATE_PLAY 	1
#define STATE_PAUSE 2
#define STATE_STILL 3
#define STATE_NULL	4
#define STATE_QUIT	99

/*************************************************************************************

	Exported calls
	==============

*/

bool	playerOpen(player *plr, char *filename);
void	playerClose(player *plr);
void	playerIdle(player *plr);
void 	playerDXBuf(player *plr);
void	queueScreen(player *plr,u32 frame);
void	playerRead(player *plr);
void 	playerAudio(player *plr);
void	playerPlay(player *plr, u32 cnt, u32 depth);
