#include "gfxintrn.h"

extern void _threeDO_blt (RECT *, PRIMITIVE *);

PROC(
void _line_blt, (pClipRect, PrimPtr),
    ARG		(PRECT		pClipRect)
    ARG_END	(PRIMITIVEPTR	PrimPtr)
)
{
	_threeDO_blt (pClipRect, PrimPtr);
}
