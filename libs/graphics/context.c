#include "gfxintrn.h"

GRAPHICS_STATUS	_GraphicsStatusFlags;
CONTEXTPTR	_pCurContext;

PRIMITIVE	_locPrim;

FONTPTR		_CurFontPtr;

PROC(
CONTEXT SetContext, (Context),
    ARG_END	(CONTEXT	Context)
)
{
    CONTEXT	LastContext;

    if (Context != (LastContext = (CONTEXT)_pCurContext))
    {
	if (LastContext)
	{
	    FlushGraphics (TRUE);
	    UnsetContextFlags (
		    MAKE_WORD (0, GRAPHICS_ACTIVE | DRAWABLE_ACTIVE)
		    );
	    SetContextFlags (
		    MAKE_WORD (0, _GraphicsStatusFlags
			    & (GRAPHICS_ACTIVE | DRAWABLE_ACTIVE))
		    );

	    DeactivateContext ();
	}

	if (_pCurContext = (CONTEXTPTR)Context)
	{
	    ActivateContext ();

	    _GraphicsStatusFlags &= ~(GRAPHICS_ACTIVE | DRAWABLE_ACTIVE);
	    _GraphicsStatusFlags |= HIBYTE (_get_context_flags ());

	    SetPrimColor (&_locPrim, _get_context_fg_color ());

	    _CurFramePtr = (FRAMEPTR)_get_context_fg_frame ();
	    BGFrame = _get_context_bg_frame ();
	    _CurFontPtr = (FONTPTR)_get_context_font ();
	}
    }

    return (LastContext);
}

PROC(
CONTEXT_REF CreateContext, (),
    ARG_VOID
)
{
    CONTEXT_REF	ContextRef;

    if (ContextRef = AllocContext ())
    {
	CONTEXT	OldContext;

		/* initialize context */
	OldContext = SetContext (CaptureContext (ContextRef));
	SetContextMapType (MAP_NOXFORM);
	SetContextDrawState (DEST_PIXMAP | DRAW_REPLACE);
	SetContextForeGroundColor (
		BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1F, 0x1F), 0x0F)
		);
	SetContextBackGroundColor (
		BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x00), 0x00)
		);
	SetContextClipping (TRUE);
	ReleaseContext (SetContext (OldContext));
    }

    return (ContextRef);
}

PROC(
BOOLEAN DestroyContext, (ContextRef),
    ARG_END	(CONTEXT_REF	ContextRef)
)
{
    if (ContextRef == 0)
	return (FALSE);

    if (_pCurContext && _pCurContext->ContextRef == ContextRef)
	SetContext ((CONTEXT)0);

    return (FreeContext (ContextRef));
}

PROC(
CONTEXT CaptureContext, (ContextRef),
    ARG_END	(CONTEXT_REF	ContextRef)
)
{
    CONTEXTPTR	ContextPtr;

    if (ContextPtr = LockContext (ContextRef))
	ContextPtr->ContextRef = ContextRef;

    return ((CONTEXT)ContextPtr);
}

PROC(
CONTEXT_REF ReleaseContext, (Context),
    ARG_END	(CONTEXT	Context)
)
{
    CONTEXTPTR	ContextPtr;

    if (ContextPtr = (CONTEXTPTR)Context)
    {
	CONTEXT_REF	ContextRef;

	ContextRef = ContextPtr->ContextRef;
	UnlockContext (ContextRef);

	return (ContextRef);
    }

    return (0);
}

PROC(
DRAW_STATE GetContextDrawState, (),
    ARG_VOID
)
{
    if (!ContextActive ())
	return (DEST_PIXMAP | DRAW_REPLACE);

    return (_get_context_draw_state ());
}

PROC(
DRAW_STATE SetContextDrawState, (DrawState),
    ARG_END	(DRAW_STATE	DrawState)
)
{
    DRAW_STATE	oldState;

    if (!ContextActive ())
	return (DEST_PIXMAP | DRAW_REPLACE);

    if ((oldState = _get_context_draw_state ()) != DrawState)
    {
	SwitchContextDrawState (DrawState);
	SetContextGraphicsFunctions ();
    }

    return (oldState);
}

PROC(
COLOR SetContextForeGroundColor, (Color),
    ARG_END	(COLOR	Color)
)
{
    COLOR	oldColor;

    if (!ContextActive ())
	return (BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1F, 0x1F), 0x0F));

    if ((oldColor = _get_context_fg_color ()) != Color)
    {
	SwitchContextForeGroundColor (Color);
    }
    SetPrimColor (&_locPrim, Color);

    return (oldColor);
}

PROC(
COLOR SetContextBackGroundColor, (Color),
    ARG_END	(COLOR	Color)
)
{
    COLOR	oldColor;

    if (!ContextActive ())
	return (BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x00), 0x00));

    if ((oldColor = _get_context_bg_color ()) != Color)
    {
	SwitchContextBackGroundColor (Color);
    }

    return (oldColor);
}

PROC(
BG_FUNC SetContextBGFunc, (BGFunc),
    ARG_END	(BG_FUNC	BGFunc)
)
{
    BG_FUNC	oldBGFunc;

    if (!ContextActive ())
	return (0);

    if ((oldBGFunc = _get_context_bg_func ()) != BGFunc)
    {
	SwitchContextBGFunc (BGFunc);
    }

    return (oldBGFunc);
}

PROC(
BOOLEAN SetContextClipping, (ClipStatus),
    ARG_END	(BOOLEAN	ClipStatus)
)
{
    BOOLEAN	oldClipStatus;

    if (!ContextActive ())
	return (TRUE);

    oldClipStatus = (_get_context_flags () & BATCH_CLIP_GRAPHICS) != 0;
    if (ClipStatus)
    {
	SetContextFlags (BATCH_CLIP_GRAPHICS);
    }
    else
    {
	UnsetContextFlags (BATCH_CLIP_GRAPHICS);
    }

    return (oldClipStatus);
}

PROC(
BOOLEAN SetContextClipRect, (lpRect),
    ARG_END	(LPRECT	lpRect)
)
{
    if (!ContextActive ())
	return (FALSE);

    FlushGraphics (TRUE);
    if (lpRect)
	_pCurContext->ClipRect = *lpRect;
    else
	_pCurContext->ClipRect.extent.width = 0;

    return (TRUE);
}

PROC(
BOOLEAN GetContextClipRect, (lpRect),
    ARG_END	(LPRECT	lpRect)
)
{
    if (!ContextActive ())
	return (FALSE);

    *lpRect = _pCurContext->ClipRect;
    return (lpRect->extent.width != 0);
}


