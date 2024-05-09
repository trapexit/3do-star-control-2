#include "gfxintrn.h"

PROC(
void DrawPolygon, (Polygon),
    ARG_END	(POLYGON	Polygon)
)
{
    SetPrimType (&_locPrim, POLY_PRIM);
    _locPrim.Object.Polygon = Polygon;

    DrawBatch (&_locPrim, 0, BATCH_SINGLE);
}

PROC(
void DrawFilledPolygon, (Polygon),
    ARG_END	(POLYGON	Polygon)
)
{
    SetPrimType (&_locPrim, POLYFILL_PRIM);
    _locPrim.Object.Polygon = Polygon;

    DrawBatch (&_locPrim, 0, BATCH_SINGLE);
}

PROC(
POLYREF CreatePolygon, (PtCount),
    ARG_END	(COUNT		PtCount)
)
{
    POLYREF	hPoly;

    hPoly = (POLYREF)NULL_PTR;
    if (PtCount > 0)
    {
	LPPOLYGON_DESC	lpPoly;

	if ((hPoly = AllocPolygon (PtCount)) != (POLYREF)NULL_PTR)
	{
	    if ((lpPoly = LockPolygon (hPoly)) == (LPPOLYGON_DESC)NULL_PTR)
	    {
		FreePolygon (hPoly);
		hPoly = (POLYREF)NULL_PTR;
	    }
	    else
	    {
		lpPoly->hPoly = hPoly;
		lpPoly->PtCount = PtCount;
		UnlockPolygon (hPoly);
	    }
	}
    }

    return (hPoly);
}

PROC(
BOOLEAN DestroyPolygon, (PolyRef),
    ARG_END	(POLYREF	PolyRef)
)
{
    return (FreePolygon (PolyRef));
}

PROC(
POLYGON CapturePolygon, (PolyRef),
    ARG_END	(POLYREF	PolyRef)
)
{
    LPPOLYGON_DESC	lpPoly;

    if (PolyRef == (POLYREF)NULL_PTR)
	lpPoly = (LPPOLYGON_DESC)NULL_PTR;
    else
    {
	lpPoly = LockPolygon (PolyRef);
	lpPoly->PtList = (LPPOINT)&lpPoly[1];
    }

    return ((POLYGON)lpPoly);
}

PROC(
POLYREF ReleasePolygon, (Polygon),
    ARG_END	(POLYGON	Polygon)
)
{
    POLYREF	PolyRef;

    PolyRef = (POLYREF)NULL_PTR;
    if (Polygon != (POLYGON)NULL_PTR)
    {
	PolyRef = ((LPPOLYGON_DESC)Polygon)->hPoly;
	UnlockPolygon (PolyRef);
    }

    return (PolyRef);
}

PROC(
BOOLEAN SetPolygonPoint, (Polygon, PtIndex, x, y),
    ARG		(POLYGON	Polygon)
    ARG		(COUNT		PtIndex)
    ARG		(COORD		x)
    ARG_END	(COORD		y)
)
{
    BOOLEAN	retval;

    retval = FALSE;
    if (Polygon != (POLYGON)NULL_PTR)
    {
	LPPOLYGON_DESC	lpPoly;
	LPPOINT		lpPt;

	lpPoly = (LPPOLYGON_DESC)Polygon;
	if (lpPoly->BoundRect.extent.width == 0)
	{
	    lpPoly->BoundRect.corner.x = x;
	    lpPoly->BoundRect.corner.y = y;
	    lpPoly->BoundRect.extent.width =
		    lpPoly->BoundRect.extent.height = 1;
	}
	else
	{
	    SIZE	delta, width, height;

	    width = height = 1;
	    if ((delta = x - lpPoly->BoundRect.corner.x) >= 0)
		width += delta;
	    else
	    {
		lpPoly->BoundRect.extent.width -= delta;
		lpPoly->BoundRect.corner.x = x;
	    }
	    if ((delta = y - lpPoly->BoundRect.corner.y) >= 0)
		height += delta;
	    else
	    {
		lpPoly->BoundRect.extent.height -= delta;
		lpPoly->BoundRect.corner.y = y;
	    }
	    if ((delta = width - lpPoly->BoundRect.extent.width) > 0)
		lpPoly->BoundRect.extent.width += delta;
	    if ((delta = height - lpPoly->BoundRect.extent.height) > 0)
		lpPoly->BoundRect.extent.height += delta;
	}

	lpPt = &lpPoly->PtList[PtIndex];
	lpPt->x = x;
	lpPt->y = y;

	retval = TRUE;
    }

    return (retval);
}

PROC(
BOOLEAN GetPolygonPoint, (Polygon, PtIndex, px, py),
    ARG		(POLYGON	Polygon)
    ARG		(COUNT		PtIndex)
    ARG		(PCOORD		px)
    ARG_END	(PCOORD		py)
)
{
    BOOLEAN	retval;

    retval = FALSE;
    if (Polygon != (POLYGON)NULL_PTR)
    {
	LPPOINT	lpPt;

	lpPt = &((LPPOLYGON_DESC)Polygon)->PtList[PtIndex];
	*px = lpPt->x;
	*py = lpPt->y;

	retval = TRUE;
    }

    return (retval);
}

