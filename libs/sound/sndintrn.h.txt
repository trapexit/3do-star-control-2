#ifndef _SNDINTRN_H
#define _SNDINTRN_H

#include <stdio.h>
#include "sndlib.h"
#include "reslib.h"
#include "play.h"

PROC_GLOBAL(
MEM_HANDLE _GetMODData, (fp, length),
    ARG		(FILE		*fp)
    ARG_END	(DWORD		length)
);

#define AllocMODData(s)	AllocResourceData((s),MEM_SOUND)
#define LockMODData	LockResourceData
#define UnlockMODData	UnlockResourceData
#define FreeMODData	FreeResourceData

#endif /* _SNDINTRN_H */

