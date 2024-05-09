/*
	File:		DuckDebug.h

	Contains:	duck debugging routines

	Copyright:	© 1993 by The Duck Corporation. All rights reserved.
				This material constitutes confidential and proprietary
				information of The Duck Corporation and shall not be used by
				any Person or for any purpose except as expressly
				authorized in writing by The Duck Corporation.

*/
#define JFLAG_HOLD 		1
#define JFLAG_DXBUSY 	2
#define JFLAG_READDONE 	4

/*typedef struct JournalEntry {
	u32		timeStamp;				// audio timer-based counter
	u32		frameNumber;			// audio outputting this frame
	u32		headBuf;				// head buffer for disk read
	u32		jFlags;					// journal flags	
} JournalEntry;

void JournalStore(u32 index,u32 flags);
void JournalView(u32 index);
*/


void QC(void *p);
void QL(void *p);
