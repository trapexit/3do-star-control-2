/*
	File:		Async.h

	Contains:	Asynchronous file routines

	Copyright:	� 1993 by The Duck Corporation. All rights reserved.
				This material constitutes confidential and proprietary
				information of The Duck Corporation and shall not be used by
				any Person or for any purpose except as expressly
				authorized in writing by The Duck Corporation.

	BufIndex is a data structure that binds the frame number to a specific
	buffer location.  It is useful for maintaining buffers for audio, compressed
	video, and screen data.

	Change History (most recent first):

	1	 9/10/93	vy		first checked in

	To Do:
*/


/*

	Support for 3D0
	
*/

#include	"Portfolio.h"
#include "DuckTypes.h"
#ifndef __Async_h
#define __Async_h
#define DEVICEBLOCKSIZE 2048		// a good number to keep in mind
//#define DEVICEBLOCKSIZE 2336		// a good number to keep in mind
#define OPT_MODE1 (( (u32)CDROM_DEFAULT_DENSITY <<29) | (CDROM_DEFAULT_RECOVERY<<27) | (CDROM_Address_Blocks<<25) | (CDROM_READAHEAD_ENABLED<<23) | (12 << 19) | (CDROM_DOUBLE_SPEED<<14) | (CDROM_PITCH_NORMAL<<12) | (2048<<0))
#define OPT_MODE2 (( (u32)CDROM_DATA <<29) | (CDROM_DEFAULT_RECOVERY<<27) | (CDROM_Address_Blocks<<25) | (CDROM_READAHEAD_ENABLED<<23) | (12 << 19) | (CDROM_DOUBLE_SPEED<<14) | (CDROM_PITCH_NORMAL<<12) | (DEVICEBLOCKSIZE<<0))

typedef struct BlockFile {
	Item		f;			// File device
	Item		fIOReq;		// File IOReq
	FileStatus	fStatus;	// Status record
} BlockFile;

bool	AsyncOpen(char *name);
u32		AsyncRead(char *buffer, u32 count, u32 offset);
bool	AsyncCheck(void);
void	AsyncClose(void);

#endif
