/*
	File:		StorageManager.h

	Contains:	Interface to the Storage Manager

	Written by:	Francis Stanbach

	Copyright:	© 1993 by The 3DO Company. All rights reserved.
				This material constitutes confidential and proprietary
				information of the 3DO Company and shall not be used by
				any Person or for any purpose except as expressly
				authorized in writing by the 3DO Company.

	Change History (most recent first):

				  4/25/94	fjs		Started adding headers for calls to storage

	To Do:
*/

#include "types.h"
#include "init3DO.h"

typedef struct SMParameterBlock
{
	void*	bufferBegin;
	void*	bufferCurrent;
	void*	bufferEnd;
	uint32	bytesNeeded;
	ScreenContext *screenPtr;
	
} SMParameterBlock;

void StorageManagerBrowse(SMParameterBlock* smpb);
