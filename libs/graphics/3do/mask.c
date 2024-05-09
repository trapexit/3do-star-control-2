#include "gfxintrn.h"
#include "vga.h"

PROC(STATIC
void dummy_blt, (pClipRect, PrimPtr),
    ARG		(PRECT		pClipRect)
    ARG_END	(PRIMITIVEPTR	PrimPtr)
)
{
}

PROC_PARAMETER(
void (*mask_func_array[]), (pClipRect, PrimPtr),
    ARG		(PRECT		pClipRect)
    ARG_END	(PRIMITIVEPTR	PrimPtr)
) =
{
    dummy_blt,
    dummy_blt,
    dummy_blt,
    dummy_blt,
    dummy_blt,
    dummy_blt,
    dummy_blt,
    dummy_blt,
    dummy_blt,
    dummy_blt,
};
