#include "gfxintrn.h"

PROC(
FRAME CaptureDrawable, (Drawable),
    ARG_END	(DRAWABLE	Drawable)
)
{
    DRAWABLEPTR	DrawablePtr;

    if (DrawablePtr = LockDrawable (Drawable))
    {
	COUNT	FrameIndex;

	FrameIndex = GetDrawableIndex (Drawable);
	return ((FRAME)&DrawablePtr->Frame[FrameIndex]);
    }

    return (0);
}

PROC(
DRAWABLE ReleaseDrawable, (FramePtr),
    ARG_END	(FRAMEPTR	FramePtr)
)
{
    if (FramePtr != 0)
    {
	DRAWABLE	Drawable;
	DRAWABLEPTR	DrawablePtr;

	DrawablePtr = GetFrameParentDrawable (FramePtr);
	Drawable = BUILD_DRAWABLE (
		DrawablePtr->hDrawable, INDEX_GET (FramePtr->TypeIndexAndFlags)
		);
	UnlockDrawable (Drawable);

	return (Drawable);
    }

    return (0);
}

PROC(
MEM_HANDLE GetFrameHandle, (FramePtr),
    ARG_END	(FRAMEPTR	FramePtr)
)
{
    DRAWABLEPTR	DrawablePtr;

    if (FramePtr == 0)
	return (0);

    DrawablePtr = GetFrameParentDrawable (FramePtr);
    return (DrawablePtr->hDrawable);
}

PROC(
COUNT GetFrameCount, (FramePtr),
    ARG_END	(FRAMEPTR	FramePtr)
)
{
    DRAWABLEPTR	DrawablePtr;

    if (FramePtr == 0)
	return (0);

    DrawablePtr = GetFrameParentDrawable (FramePtr);
    return (INDEX_GET (DrawablePtr->FlagsAndIndex) + 1);
}

PROC(
COUNT GetFrameIndex, (FramePtr),
    ARG_END	(FRAMEPTR	FramePtr)
)
{
    if (FramePtr == 0)
	return (0);

    return (INDEX_GET (FramePtr->TypeIndexAndFlags));
}

PROC(
FRAME SetAbsFrameIndex, (FramePtr, FrameIndex),
    ARG		(FRAMEPTR	FramePtr)
    ARG_END	(COUNT		FrameIndex)
)
{
    if (FramePtr != 0)
    {
	DRAWABLEPTR	DrawablePtr;

	DrawablePtr = GetFrameParentDrawable (FramePtr);

	FrameIndex = FrameIndex
		% (INDEX_GET (DrawablePtr->FlagsAndIndex) + 1);
	FramePtr = (FRAMEPTR)&DrawablePtr->Frame[FrameIndex];
    }

    return ((FRAME)FramePtr);
}

PROC(
FRAME SetRelFrameIndex, (FramePtr, FrameOffs),
    ARG		(FRAMEPTR	FramePtr)
    ARG_END	(SIZE		FrameOffs)
)
{
    if (FramePtr != 0)
    {
	COUNT		num_frames;
	DRAWABLEPTR	DrawablePtr;

	DrawablePtr = GetFrameParentDrawable (FramePtr);
	num_frames = INDEX_GET (DrawablePtr->FlagsAndIndex) + 1;
	if (FrameOffs < 0)
	{
	    while ((FrameOffs += num_frames) < 0)
		;
	}

	FrameOffs = (INDEX_GET (FramePtr->TypeIndexAndFlags) + FrameOffs) % num_frames;
	FramePtr = (FRAMEPTR)&DrawablePtr->Frame[FrameOffs];
    }

    return ((FRAME)FramePtr);
}

PROC(
FRAME SetEquFrameIndex, (DstFramePtr, SrcFramePtr),
    ARG		(FRAMEPTR	DstFramePtr)
    ARG_END	(FRAMEPTR	SrcFramePtr)
)
{
    if (DstFramePtr && SrcFramePtr)
	return ((FRAME)(
		(LPBYTE)((GetFrameParentDrawable (DstFramePtr))->Frame)
		+ ((LPBYTE)SrcFramePtr -
		(LPBYTE)((GetFrameParentDrawable (SrcFramePtr))->Frame))
		));

    return (0);
}

PROC(
FRAME IncFrameIndex, (FramePtr),
    ARG_END	(FRAMEPTR	FramePtr)
)
{
    DRAWABLEPTR	DrawablePtr;

    if (FramePtr == 0)
	return (0);

    DrawablePtr = GetFrameParentDrawable (FramePtr);
    if (INDEX_GET (FramePtr->TypeIndexAndFlags) < INDEX_GET (DrawablePtr->FlagsAndIndex))
	return ((FRAME)++FramePtr);
    else
	return ((FRAME)DrawablePtr->Frame);
}

PROC(
FRAME DecFrameIndex, (FramePtr),
    ARG_END	(FRAMEPTR	FramePtr)
)
{
    if (FramePtr == 0)
	return (0);

    if (INDEX_GET (FramePtr->TypeIndexAndFlags))
	return ((FRAME)--FramePtr);
    else
    {
	DRAWABLEPTR	DrawablePtr;

	DrawablePtr = GetFrameParentDrawable (FramePtr);
	return ((FRAME)&DrawablePtr->Frame[INDEX_GET (DrawablePtr->FlagsAndIndex)]);
    }
}

PROC(
void XFlipFrame, (FramePtr),
    ARG_END	(FRAMEPTR	FramePtr)
)
{
    if (FramePtr)
    {
	if (GetFrameFlags (FramePtr) & X_FLIP)
	{
	    SubFrameFlags (FramePtr, X_FLIP);
	}
	else
	{
	    AddFrameFlags (FramePtr, X_FLIP);
	}
    }
}

