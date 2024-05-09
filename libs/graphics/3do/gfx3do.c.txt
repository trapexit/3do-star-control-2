#include "gfxintrn.h"
#include "defs3DO.h"

#define BUILD_FRAME	(1 << 0)
#define FIND_PAGE	(1 << 1)
#define FIRST_BATCH	(1 << 2)

BYTE	_batch_depth;
BYTE	_batch_flags;

PROC(
void BatchGraphics, (),
    ARG_VOID
)
{
//    CheckForTasks ();
    if (_get_context_draw_mode () == DRAW_SUBTRACTIVE)
        _batch_flags |= BUILD_FRAME;
    ++_batch_depth;
}

PROC(
void UnbatchGraphics, (),
    ARG_VOID
)
{
    if (_batch_depth && --_batch_depth == 0 &&
    	    TYPE_GET (_CurFramePtr->TypeIndexAndFlags) >= SCREEN_DRAWABLE)
	_ThreeDO_batch_cels (0);
}

PROC(
void ScreenOrigin, (Display, sx, sy),
    ARG		(FRAMEPTR	Display)
    ARG		(COORD		sx)
    ARG_END	(COORD		sy)
)
{
    extern int	_new_org_x, _new_org_y;
    
    _new_org_x = sx;
    _new_org_y = sy;
//    if (_batch_flags & FIND_PAGE)
	_batch_flags |= BUILD_FRAME;
}

PROC(
void FindDisplayPage, (Display, x, y),
    ARG		(FRAMEPTR	Display)
    ARG		(COORD		x)
    ARG_END	(COORD		y)
)
{
    extern int	_new_org_x, _new_org_y;
    
    _new_org_x = x;
    _new_org_y = y;
    if (!(_batch_flags & FIND_PAGE))
	_batch_flags |= FIND_PAGE | FIRST_BATCH;
}

PROC(
void WaitVBlank, (),
    ARG_VOID
)
{
    // WaitVBL ();
}

PROC(STATIC
DRAWABLE ThreeDO_alloc_image, (NumFrames, DrawableType, flags, width, height),
    ARG		(COUNT		NumFrames)
    ARG		(DRAWABLE_TYPE	DrawableType)
    ARG		(CREATE_FLAGS	flags)
    ARG		(SIZE		width)
    ARG_END	(SIZE		height)
)
{
    DWORD	data_byte_count;
    DRAWABLE	Drawable;

    data_byte_count = 0;
    if (flags & WANT_MASK)
	data_byte_count += (DWORD)SCAN_WIDTH (width) * height;
    if ((flags & WANT_PIXMAP) && DrawableType == RAM_DRAWABLE)
    {
	width = ((width << 1) + 3) & ~3;
	data_byte_count += (DWORD)width * height;
    }

    if (Drawable = AllocDrawable (NumFrames, data_byte_count * NumFrames))
    {
	if (DrawableType == RAM_DRAWABLE)
	{
	    COUNT	i;
	    DWORD	data_offs;
	    DRAWABLEPTR	DrawablePtr;
	    FRAMEPTR	F;

	    data_offs = sizeof (*F) * NumFrames;
	    DrawablePtr = LockDrawable (Drawable);
	    for (i = 0, F = &DrawablePtr->Frame[0]; i < NumFrames; ++i, ++F)
	    {
		F->DataOffs = data_offs;
		data_offs += data_byte_count - sizeof (*F);
	    }
	    UnlockDrawable (Drawable);
	}
//	else if (flags & WANT_PIXMAP)
//	{
//	    _ThreeDO_load_plut ();
//	}
    }

    return (Drawable);
}

PROC_GLOBAL(
void _threeDO_blt, (pClipRect, PrimPtr),
    ARG		(PRECT		pClipRect)
    ARG_END	(PRIMITIVEPTR	PrimPtr)
);

PROC_GLOBAL(
void _threeDO_fillrect_blt, (pClipRect, PrimPtr),
    ARG		(PRECT		pClipRect)
    ARG_END	(PRIMITIVEPTR	PrimPtr)
);

PROC(STATIC
void threeDO_cmap_blt, (pClipRect, PrimPtr),
    ARG		(PRECT		pClipRect)
    ARG_END	(PRIMITIVEPTR	PrimPtr)
)
{
    GRAPHICS_PRIM	PrimType;
    
    PrimType = GetPrimType (PrimPtr);
    SetPrimType (PrimPtr, STAMP_PRIM);
    
    _threeDO_blt (pClipRect, PrimPtr);
    
    SetPrimType (PrimPtr, PrimType);
}

PROC(STATIC
void ThreeDO_read_screen, (lpRect, DstFramePtr),
    ARG		(LPRECT		lpRect)
    ARG_END	(FRAMEPTR	DstFramePtr)
)
{
    _threedo_read_screen (lpRect, DstFramePtr);
}

PROC_PARAMETER(
void (*ThreeDO_func_array[]), (pClipRect, PrimPtr),
    ARG		(PRECT		pClipRect)
    ARG_END	(PRIMITIVEPTR	PrimPtr)
) =
{
    _threeDO_blt,
    _threeDO_blt,
    _threeDO_blt,
    _threeDO_blt,
    _text_blt,
    threeDO_cmap_blt,
    _rect_blt,
    _threeDO_blt,
    _fillpoly_blt,
    _fillpoly_blt,
};

static DISPLAY_INTERFACE	ThreeDO_interface =
{
    0,

    SCREEN_DEPTH,
    0,//VIEW_WIDTH,
    0,//VIEW_HEIGHT,

    ThreeDO_alloc_image,
    ThreeDO_read_screen,

    ThreeDO_func_array,
};

PROC(
void Load3DO, (pDisplay),
    ARG_END	(PDISPLAY_INTERFACE	*pDisplay)
)
{
_batch_flags = _batch_depth = 0;
ThreeDO_interface.DisplayFlags = 0;
    *pDisplay = &ThreeDO_interface;
    ThreeDO_interface.DisplayWidth = VIEW_WIDTH;
    ThreeDO_interface.DisplayHeight = VIEW_HEIGHT;
}
