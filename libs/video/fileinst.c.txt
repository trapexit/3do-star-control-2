#include "vidlib.h"

PROC(
VIDEO_REF LoadVideoFile, (pStr),
    ARG_END	(PVOID	pStr)
)
{
    PROC_GLOBAL(
    VIDEO_REF _init_video_file, (pStr),
	ARG_END	(PVOID	pStr)
    );

    return (_init_video_file (pStr));
}


