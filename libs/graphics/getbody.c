#include "gfxintrn.h"

PROC(
MEM_HANDLE _GetGraphicData, (fp, length),
    ARG		(FILE		*fp)
    ARG_END	(DWORD		length)
)
{
    MEM_HANDLE	RData;

    if (RData = GetResourceData (fp, length, MEM_GRAPHICS))
    {
	DRAWABLEPTR	DrawablePtr;

	DrawablePtr = LockDrawable (RData);
	DrawablePtr->hDrawable = RData;
	UnlockDrawable (RData);
    }

    return (RData);
}


