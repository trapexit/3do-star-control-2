#include "gfxintrn.h"

PROC(
MAP_TYPE SetContextMapType, (MapType),
    ARG_END	(MAP_TYPE	MapType)
)
{
#ifndef MAPPING
    return (MAP_NOXFORM);
#else /* MAPPING */
    MAP_TYPE	oldMapType;

    if (!ContextActive ())
	oldMapType = MAP_NOXFORM;
    else if ((oldMapType = _get_context_map_type ()) != MapType)
    {
	SwitchContextMapType (MapType);
    }

    return (oldMapType);
#endif /* MAPPING */
}

PROC(
BOOLEAN SetContextWinOrigin, (lpOrg),
    ARG_END	(LPPOINT	lpOrg)
)
{
#ifndef MAPPING
    return (FALSE);
#else /* MAPPING */
    if (!ContextActive ())
	return (FALSE);

    SwitchContextWinOrigin (lpOrg);

    return (TRUE);
#endif /* MAPPING */
}

PROC(
BOOLEAN SetContextViewExtents, (lpExtent),
    ARG_END	(LPEXTENT	lpExtent)
)
{
#ifndef MAPPING
    return (FALSE);
#else /* MAPPING */
    if (!ContextActive ())
	return (FALSE);

    SwitchContextViewExtents (lpExtent);

    return (TRUE);
#endif /* MAPPING */
}

PROC(
BOOLEAN SetContextWinExtents, (lpExtent),
    ARG_END	(LPEXTENT	lpExtent)
)
{
#ifndef MAPPING
    return (FALSE);
#else /* MAPPING */
    if (!ContextActive ())
	return (FALSE);

    SwitchContextWinExtents (lpExtent);

    return (TRUE);
#endif /* MAPPING */
}

PROC(
BOOLEAN GetContextWinOrigin, (lpOrg),
    ARG_END	(LPPOINT	lpOrg)
)
{
#ifndef MAPPING
    return (FALSE);
#else /* MAPPING */
    if (!ContextActive ())
	return (FALSE);

    *lpOrg = _pCurContext->Map.WinOrg;

    return (TRUE);
#endif /* MAPPING */
}

PROC(
BOOLEAN GetContextViewExtents, (lpExtent),
    ARG_END	(LPEXTENT	lpExtent)
)
{
#ifndef MAPPING
    return (FALSE);
#else /* MAPPING */
    if (!ContextActive ())
	return (FALSE);

    *lpExtent = _pCurContext->Map.ViewExt;

    return (TRUE);
#endif /* MAPPING */
}

PROC(
BOOLEAN GetContextWinExtents, (lpExtent),
    ARG_END	(LPEXTENT	lpExtent)
)
{
#ifndef MAPPING
    return (FALSE);
#else /* MAPPING */
    if (!ContextActive ())
	return (FALSE);

    *lpExtent = _pCurContext->Map.WinExt;

    return (TRUE);
#endif /* MAPPING */
}

PROC(
BOOLEAN LOGtoDEV, (lpSrcPt, lpDstPt, NumPoints),
    ARG		(LPPOINT	lpSrcPt)
    ARG		(LPPOINT	lpDstPt)
    ARG_END	(COUNT		NumPoints)
)
{
#ifndef MAPPING
    return (FALSE);
#else /* MAPPING */
    if (!ContextActive () || NumPoints == 0)
	return (FALSE);

    {
	MAP_TYPE	MapType;

	_init_context_map (&MapType);
	if (MapType == MAP_NOXFORM)
	{
	    do
		*lpDstPt++ = *lpSrcPt++;
	    while (--NumPoints);
	}
	else
	{
	    do
	    {
		lpDstPt->x = LXtoDX (lpSrcPt->x);
		lpDstPt->y = LYtoDY (lpSrcPt->y);
		++lpSrcPt;
		++lpDstPt;
	    } while (--NumPoints);
	}
	_uninit_context_map ();

	return (TRUE);
    }
#endif /* MAPPING */
}

PROC(
BOOLEAN DEVtoLOG, (lpSrcPt, lpDstPt, NumPoints),
    ARG		(LPPOINT	lpSrcPt)
    ARG		(LPPOINT	lpDstPt)
    ARG_END	(COUNT		NumPoints)
)
{
#ifndef MAPPING
    return (FALSE);
#else /* MAPPING */
    if (!ContextActive () || NumPoints == 0)
	return (FALSE);

    {
	MAP_TYPE	MapType;

	_init_context_map (&MapType);
	if (MapType == MAP_NOXFORM)
	{
	    do
		*lpDstPt++ = *lpSrcPt++;
	    while (--NumPoints);
	}
	else
	{
	    do
	    {
		lpDstPt->x = DXtoLX (lpSrcPt->x);
		lpDstPt->y = DYtoLY (lpSrcPt->y);
		++lpSrcPt;
		++lpDstPt;
	    } while (--NumPoints);
	}
	_uninit_context_map ();

	return (TRUE);
    }
#endif /* MAPPING */
}

