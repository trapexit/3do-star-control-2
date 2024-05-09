#include "gfxintrn.h"

PROC(
void DrawStamp, (lpStamp),
    ARG_END	(LPSTAMP	lpStamp)
)
{
    SetPrimType (&_locPrim, STAMP_PRIM);
    _locPrim.Object.Stamp = *lpStamp;

    DrawBatch (&_locPrim, 0, BATCH_SINGLE);
}

PROC(
void DrawFilledStamp, (lpStamp),
    ARG_END	(LPSTAMP	lpStamp)
)
{
    SetPrimType (&_locPrim, STAMPFILL_PRIM);
    _locPrim.Object.Stamp = *lpStamp;

    DrawBatch (&_locPrim, 0, BATCH_SINGLE);
}

PROC(
void DrawStampCMap, (lpStampCMap),
    ARG_END	(LPSTAMP_CMAP	lpStampCMap)
)
{
    SetPrimType (&_locPrim, STAMPCMAP_PRIM);
    _locPrim.Object.StampCMap = *lpStampCMap;

    DrawBatch (&_locPrim, 0, BATCH_SINGLE);
}

