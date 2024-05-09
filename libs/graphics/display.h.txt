#ifndef _DISPLAY_H
#define _DISPLAY_H

/*
#define WANT_MASK	(CREATE_FLAGS)(1 << 0)
#define WANT_PIXMAP	(CREATE_FLAGS)(1 << 1)
*/
#define WANT_COMPRESSED	(CREATE_FLAGS)(1 << 2)
#define DOUBLE_RES	(CREATE_FLAGS)(1 << 3)

typedef struct
{
    CREATE_FLAGS	DisplayFlags;

    BYTE		DisplayDepth;
    COUNT		DisplayWidth, DisplayHeight;

    PROC_PARAMETER(
    DRAWABLE (*alloc_image), (NumFrames, DrawableType, flags, width, height),
	ARG		(COUNT		NumFrames)
	ARG		(DRAWABLE_TYPE	DrawableType)
	ARG		(CREATE_FLAGS	flags)
	ARG		(SIZE		width)
	ARG_END		(SIZE		height)
    );
    PROC_PARAMETER(
    void (*read_display), (lpRect, DstFramePtr),
	ARG		(LPRECT		lpRect)
	ARG_END		(FRAMEPTR	DstFramePtr)
    );
    PROC_PARAMETER(
    void (**graphics_func_array), (pClipRect, PrimPtr),
	ARG		(PRECT		pClipRect)
	ARG_END		(PRIMITIVEPTR	PrimPtr)
    );
} DISPLAY_INTERFACE;
typedef DISPLAY_INTERFACE	*PDISPLAY_INTERFACE;

extern PDISPLAY_INTERFACE	_pCurDisplay;

PROC_GLOBAL(
void (* mask_func_array[]), (pClipRect, PrimPtr),
    ARG		(PRECT		pClipRect)
    ARG_END	(PRIMITIVEPTR	PrimPtr)
);

#define AllocDrawableImage	(*_pCurDisplay->alloc_image)
#define DrawGraphicsFunc(pRect,pPtr)	\
	(*_pCurContext->func_array[GetPrimType(pPtr)]) (pRect, (PRIMITIVEPTR)(pPtr))
#define ReadDisplay		(*_pCurDisplay->read_display)
PROC_GLOBAL(
void CDECL _bitplane_blt,
	(pClipRect, DstFramePtr, src_x, src_y, SrcFramePtr, DrawMode),
    ARG		(PRECT		pClipRect)
    ARG		(FRAMEPTR	DstFramePtr)
    ARG		(COORD		src_x)
    ARG		(COORD		src_y)
    ARG		(FRAMEPTR	SrcFramePtr)
    ARG_END	(DRAW_MODE	DrawMode)
);

#define GetDisplayFlags()	(_pCurDisplay->DisplayFlags)
#define GetDisplayDepth()	(_pCurDisplay->DisplayDepth)
#define GetDisplayWidth()	(_pCurDisplay->DisplayWidth)
#define GetDisplayHeight()	(_pCurDisplay->DisplayHeight)

#define SetContextGraphicsFunctions()	\
do \
{ \
    if (_get_context_draw_dest () == DEST_MASK) \
	_pCurContext->func_array = mask_func_array; \
    else \
	_pCurContext->func_array = _pCurDisplay->graphics_func_array; \
} while (0)

#endif /* _DISPLAY_H */

