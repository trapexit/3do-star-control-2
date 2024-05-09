#include "gfxintrn.h"

PROC(
void DrawPoint, (lpPoint),
    ARG_END	(LPPOINT	lpPoint)
)
{
    SetPrimType (&_locPrim, POINT_PRIM);
    _locPrim.Object.Point = *lpPoint;

    DrawBatch (&_locPrim, 0, BATCH_SINGLE);
}

PROC(
void DrawRectangle, (lpRect),
    ARG_END	(LPRECT		lpRect)
)
{
    SetPrimType (&_locPrim, RECT_PRIM);
    _locPrim.Object.Rect = *lpRect;

    DrawBatch (&_locPrim, 0, BATCH_SINGLE);
}

PROC(
void DrawFilledRectangle, (lpRect),
    ARG_END	(LPRECT		lpRect)
)
{
    SetPrimType (&_locPrim, RECTFILL_PRIM);
    _locPrim.Object.Rect = *lpRect;

    DrawBatch (&_locPrim, 0, BATCH_SINGLE);
}

PROC(
void _rect_blt, (pClipRect, PrimPtr),
    ARG		(PRECT		pClipRect)
    ARG_END	(PRIMITIVEPTR	PrimPtr)
)
{
    COORD	x, y;
    RECTPTR	RectPtr;
    PRIMITIVE	locPrim;

    locPrim = *PrimPtr;
    SetPrimType (&locPrim, RECTFILL_PRIM);

    RectPtr = &PrimPtr->Object.Rect;
    locPrim.Object.Rect = *pClipRect;
    if (locPrim.Object.Rect.corner.x == RectPtr->corner.x)
    {
	locPrim.Object.Rect.extent.width = 1;
	DrawGraphicsFunc (&locPrim.Object.Rect, &locPrim);
	locPrim.Object.Rect.extent.width = pClipRect->extent.width;
    }
    if (locPrim.Object.Rect.corner.y == RectPtr->corner.y)
    {
	locPrim.Object.Rect.extent.height = 1;
	DrawGraphicsFunc (&locPrim.Object.Rect, &locPrim);
	locPrim.Object.Rect.extent.height = pClipRect->extent.height;
    }
    if ((x = locPrim.Object.Rect.corner.x
	    + locPrim.Object.Rect.extent.width) ==
	    RectPtr->corner.x + RectPtr->extent.width)
    {
	locPrim.Object.Rect.corner.x = x - 1;
	locPrim.Object.Rect.extent.width = 1;
	DrawGraphicsFunc (&locPrim.Object.Rect, &locPrim);
	locPrim.Object.Rect.corner.x = pClipRect->corner.x;
	locPrim.Object.Rect.extent.width = pClipRect->extent.width;
    }
    if ((y = locPrim.Object.Rect.corner.y
	    + locPrim.Object.Rect.extent.height) ==
	    RectPtr->corner.y + RectPtr->extent.height)
    {
	locPrim.Object.Rect.corner.y = y - 1;
	locPrim.Object.Rect.extent.height = 1;
	DrawGraphicsFunc (&locPrim.Object.Rect, &locPrim);
    }
}

