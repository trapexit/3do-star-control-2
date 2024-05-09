#include "vidlib.h"
#include "timlib.h"
#include "inplib.h"
#include "gfxlib.h"

static int	DuckItem;

PROC(
BOOLEAN InitVideo, (UseCDROM),
    ARG_END	(BOOLEAN	UseCDROM)
)
{
    DuckItem = -1;
    return (TRUE);
}

PROC(
void UninitVideo, (),
    ARG_VOID
)
{
}

PROC(
BYTE SetVideoMode, (mode),
    ARG_END	(BYTE	mode)
)
{
    return (MODES_UNDEFINED);
}

PROC(
BYTE GetVideoMode, (),
    ARG_VOID
)
{
    return (MODES_UNDEFINED);
}

PROC(
VIDEO_REF _init_video_file, (pStr),
    ARG_END	(PVOID	pStr)
)
{
    return ((VIDEO_REF)pStr);
}

static BOOLEAN	fmv_uninit;

PROC(
void VidStop, (),
    ARG_VOID
)
{
    if (DuckItem != -1)
    {
	if (DuckItem)
	{
	    if (LookupItem (DuckItem))
		DeleteItem (DuckItem);
	}
	DuckItem = -1;

	if (fmv_uninit)
	{
	    Init3DO ();
	    InitSound (0, (char **)0);
	
	    InitGameKernel ();
	}
    }
}

PROC(
VIDEO_TYPE VidPlay, (VidRef, loopname, uninit),
    ARG		(VIDEO_REF	VidRef)
    ARG		(char		*loopname)
    ARG_END	(BOOLEAN	uninit)
)
{
    DuckItem = -1;
    if (VidRef)
    {
	if (loopname) // means simple looping video -- we add the audio
	    duckloop ((char *)VidRef, loopname);
	else
	{
	    char	buf[20];

	    if (fmv_uninit = uninit)
	    {
		UninitGameKernel ();
    
		UninitSound ();
		Uninit3DO ();
	    }
	    
	    sprintf (buf, "duck %s", (char *)VidRef);
	    if (!((DuckItem = LoadProgramPrio (buf, 180)) < 0))
		return (SOFTWARE_FMV);
	}
    }
    
    return (NO_FMV);
}

PROC(
BOOLEAN VidPlaying, (),
    ARG_VOID
)
{
    return (DuckItem > 0 && LookupItem (DuckItem));
}

PROC(
BOOLEAN DestroyVideo, (VideoRef),
    ARG_END	(VIDEO_REF	VideoRef)
)
{
    return (TRUE);
}
