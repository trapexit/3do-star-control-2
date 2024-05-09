#include "gfxintrn.h"

FRAMEPTR	_CurFramePtr;

PROC(
FRAME SetContextFGFrame, (Frame),
    ARG_END	(FRAME	Frame)
)
{
    FRAME	LastFrame;

    if (Frame != (LastFrame = (FRAME)_CurFramePtr))
    {
	if (LastFrame)
	{
	    FlushGraphics (TRUE);
	    DeactivateDrawable ();
	}

	if (_CurFramePtr = (FRAMEPTR)Frame)
	    ActivateDrawable ();

	if (ContextActive ())
	{
	    SwitchContextFGFrame (Frame);
	    SetContextGraphicsFunctions ();
	}
    }

    return (LastFrame);
}

PROC(
FRAME SetContextBGFrame, (Frame),
    ARG_END	(FRAME	Frame)
)
{
    FRAME	LastFrame;

    LastFrame = BGFrame;
    BGFrame = Frame;
    if (ContextActive ())
	SwitchContextBGFrame (Frame);

    return (LastFrame);
}

PROC(
DRAWABLE _request_drawable,
	(NumFrames, DrawableType, flags, width, height),
    ARG		(COUNT		NumFrames)
    ARG		(DRAWABLE_TYPE	DrawableType)
    ARG		(CREATE_FLAGS	flags)
    ARG		(SIZE		width)
    ARG_END	(SIZE		height)
)
{
    DRAWABLE	Drawable;

    if (Drawable = AllocDrawableImage (
	    NumFrames, DrawableType, flags, width, height
	    ))
    {
	DRAWABLEPTR	DrawablePtr;

	if ((DrawablePtr = LockDrawable (Drawable)) == 0)
	{
	    FreeDrawable (Drawable);
	    Drawable = 0;
	}
	else
	{
	    FRAMEPTR	FramePtr;

	    DrawablePtr->hDrawable = GetDrawableHandle (Drawable);
	    TYPE_SET (DrawablePtr->FlagsAndIndex, flags << FTYPE_SHIFT);
	    INDEX_SET (DrawablePtr->FlagsAndIndex, NumFrames - 1);

	    FramePtr = &DrawablePtr->Frame[NumFrames - 1];
	    while (NumFrames--)
	    {
		TYPE_SET (FramePtr->TypeIndexAndFlags, DrawableType);
		INDEX_SET (FramePtr->TypeIndexAndFlags, NumFrames);
		SetFrameBounds (FramePtr, width, height);
		--FramePtr;
	    }
	    UnlockDrawable (Drawable);
	}
    }

    return (Drawable);
}

PROC(
DRAWABLE CreateDisplay, (CreateFlags, pwidth, pheight),
    ARG		(CREATE_FLAGS	CreateFlags)
    ARG		(PSIZE		pwidth)
    ARG_END	(PSIZE		pheight)
)
{
    DRAWABLE	Drawable;

    if (!DisplayActive ())
	return (0);

    if (Drawable = _request_drawable (
	    1, SCREEN_DRAWABLE,
	    CreateFlags & (WANT_PIXMAP | (GetDisplayFlags () & WANT_MASK)),
	    GetDisplayWidth (),
	    GetDisplayHeight ()
	    ))
    {
	FRAMEPTR	F;

	if ((F = CaptureDrawable (Drawable)) == 0)
	    DestroyDrawable (Drawable);
	else
	{
	    *pwidth = GetFrameWidth (F);
	    *pheight = GetFrameHeight (F);

	    ScreenOrigin (F, 0, 0);
	    ReleaseDrawable (F);
	    return (Drawable);
	}
    }

    *pwidth = *pheight = 0;
    return (0);
}

PROC(
DRAWABLE CreateDrawable, (CreateFlags, width, height, num_frames),
    ARG		(CREATE_FLAGS	CreateFlags)
    ARG		(SIZE		width)
    ARG		(SIZE		height)
    ARG_END	(COUNT		num_frames)
)
{
    DRAWABLE	Drawable;

    if (!DisplayActive ())
	return (0);

    if (Drawable = _request_drawable (
	    num_frames, RAM_DRAWABLE,
	    CreateFlags & (WANT_MASK | WANT_PIXMAP),
	    width, height
	    ))
    {
	FRAMEPTR	F;

	if (F = CaptureDrawable (Drawable))
	{
	    ReleaseDrawable (F);

	    return (Drawable);
	}
    }

    return (0);
}

PROC(
DRAWABLE CopyDrawable, (Drawable),
    ARG_END	(DRAWABLE	Drawable)
)
{
    DRAWABLEPTR	DrawablePtr;

    if (DrawablePtr = LockDrawable (Drawable))
    {
	DRAWABLE	CopyDrawable;
	DWORD		size;

	if (TYPE_GET (DrawablePtr->Frame[0].TypeIndexAndFlags) == SCREEN_DRAWABLE)
	    CopyDrawable = 0;
	else if (CopyDrawable = AllocDrawable (1,
		(size = mem_get_size ((MEM_HANDLE)Drawable))
		- sizeof (DRAWABLE_DESC)))
	{
	    DRAWABLEPTR	CopyDrawablePtr;

	    if ((CopyDrawablePtr = LockDrawable (CopyDrawable)) == 0)
	    {
		FreeDrawable (CopyDrawable);
		CopyDrawable = 0;
	    }
	    else
	    {
		LPBYTE	lpDst, lpSrc;

		lpDst = (LPBYTE)CopyDrawablePtr;
		lpSrc = (LPBYTE)DrawablePtr;
		do
		{
		    COUNT	num_bytes;

		    num_bytes = size >= 0x7FFF ? 0x7FFF : (COUNT)size;
		    MEMCPY (lpDst, lpSrc, num_bytes);
		    FAR_PTR_ADD (&lpDst, num_bytes);
		    FAR_PTR_ADD (&lpSrc, num_bytes);
		    size -= num_bytes;
		} while (size);
		CopyDrawablePtr->hDrawable = (MEM_HANDLE)CopyDrawable;
		UnlockDrawable (CopyDrawable);
	    }
	}
	UnlockDrawable (Drawable);

	return (CopyDrawable);
    }

    return (0);
}

PROC(
BOOLEAN DestroyDrawable, (Drawable),
    ARG_END	(DRAWABLE	Drawable)
)
{
    DRAWABLEPTR	DrawablePtr;

    if (LOWORD (Drawable) == GetFrameHandle (_CurFramePtr))
	SetContextFGFrame ((FRAME)0);
    if (LOWORD (Drawable) == GetFrameHandle (BGFrame))
	SetContextBGFrame ((FRAME)0);

    if (DrawablePtr = LockDrawable (Drawable))
    {
	UnlockDrawable (Drawable);
	FreeDrawable (Drawable);

	return (TRUE);
    }

    return (FALSE);
}

PROC(
BOOLEAN GetFrameRect, (FramePtr, pRect),
    ARG		(FRAMEPTR	FramePtr)
    ARG_END	(PRECT		pRect)
)
{
    if (FramePtr)
    {
	pRect->corner.x = -GetFrameHotX (FramePtr);
	pRect->corner.y = -GetFrameHotY (FramePtr);
	pRect->extent.width = GetFrameWidth (FramePtr);
	pRect->extent.height = GetFrameHeight (FramePtr);

	return (TRUE);
    }

    return (FALSE);
}

PROC(
HOT_SPOT SetFrameHot, (FramePtr, HotSpot),
    ARG		(FRAMEPTR	FramePtr)
    ARG_END	(HOT_SPOT	HotSpot)
)
{
    if (FramePtr)
    {
	HOT_SPOT	OldHot;

	OldHot = GetFrameHotSpot (FramePtr);
    	SetFrameHotSpot (FramePtr, HotSpot);

    	return (OldHot);
    }

    return (MAKE_HOT_SPOT (0, 0));
}

PROC(
HOT_SPOT GetFrameHot, (FramePtr),
    ARG_END	(FRAMEPTR	FramePtr)
)
{
    if (FramePtr)
    {
	return (GetFrameHotSpot (FramePtr));
    }

    return (MAKE_HOT_SPOT (0, 0));
}

