#include "gfxintrn.h"

PROC(
void DrawLine, (lpLine),
    ARG_END	(LPLINE		lpLine)
)
{
    SetPrimType (&_locPrim, LINE_PRIM);
    _locPrim.Object.Line = *lpLine;

    DrawBatch (&_locPrim, 0, BATCH_SINGLE);
}

