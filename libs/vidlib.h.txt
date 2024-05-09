#ifndef _VIDLIB_H
#define _VIDLIB_H

#include "compiler.h"

typedef enum
{
    NO_FMV = 0,
    HARDWARE_FMV,
    SOFTWARE_FMV
} VIDEO_TYPE;

#define MODES_UNDEFINED	    ((BYTE)~0)

typedef DWORD	VIDEO_REF;

PROC_GLOBAL(
BOOLEAN InitVideo, (UseCDROM),
    ARG_END	(BOOLEAN	UseCDROM)
);
PROC_GLOBAL(
void UninitVideo, (),
    ARG_VOID
);

PROC_GLOBAL(
BYTE SetVideoMode, (new_mode),
    ARG_END	(BYTE	new_mode)
);
PROC_GLOBAL(
BYTE GetVideoMode, (),
    ARG_VOID
);

PROC_GLOBAL(
VIDEO_REF LoadVideoFile, (pStr),
    ARG_END	(PVOID	pStr)
);
PROC_GLOBAL(
BOOLEAN DestroyVideo, (VideoRef),
    ARG_END	(VIDEO_REF	VideoRef)
);
PROC_GLOBAL(
VIDEO_TYPE VidPlay, (VidRef, loopname, uninit),
    ARG		(VIDEO_REF	VidRef)
    ARG		(char		*loopname)
    ARG_END	(BOOLEAN	uninit)
);
PROC_GLOBAL(
void VidStop, (),
    ARG_VOID
);
PROC_GLOBAL(
BOOLEAN VidPlaying, (),
    ARG_VOID
);

#endif /* _VIDLIB_H */
