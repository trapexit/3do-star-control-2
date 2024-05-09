#ifndef _VGA_H
#define _VGA_H

#ifdef X_MODE
#define SCREEN_OFFS	(0xA0000 << 2)
#define X_EXTRA		16
#define Y_EXTRA		16
#else /* !X_MODE */
#define SCREEN_OFFS	0xA0000
#define X_EXTRA		0
#define Y_EXTRA		0
#endif /* X_MODE */
#define VIEW_WIDTH	320
#define VIEW_HEIGHT	200
#define SCREEN_WIDTH	(VIEW_WIDTH + X_EXTRA * 2)
#define SCREEN_HEIGHT	(VIEW_HEIGHT + Y_EXTRA * 2)

#define VERT_SYNC	(1 << 0)
#define BUILD_PAGE	(1 << 1)

#define SCREEN_DEPTH	8

#define PACK_EOL	0
#define PACK_LITERAL	1
#define PACK_TRANS	2
#define PACK_REPEAT	3
#define PACK_SHIFT	6
#define PACK_TYPE(c)	((BYTE)((c)>>PACK_SHIFT))
#define PACK_COUNT(c)	((BYTE)(((c)&((1<<PACK_SHIFT)-1))+1))

typedef union
{
    UWORD	x;
    struct
    {
	BYTE	num_trans, num_solid;
    } h;
} RUN_DESC;

extern STAMP	_save_stamp;

extern BYTE	_vga_batch_flags;
extern long	_overlay_off,
		_screen_phys_off,
		_screen_log_off;

PROC_GLOBAL(
void _mask_blt, (pClipRect, PrimPtr),
    ARG		(PRECT		pClipRect)
    ARG_END	(PRIMITIVEPTR	PrimPtr)
);
PROC_GLOBAL(
void _mask_fillrect_blt, (pClipRect, PrimPtr),
    ARG		(PRECT		pClipRect)
    ARG_END	(PRIMITIVEPTR	PrimPtr)
);
PROC_GLOBAL(
void _batch_blt, (pClipRect, PrimPtr),
    ARG		(PRECT		pClipRect)
    ARG_END	(PRIMITIVEPTR	PrimPtr)
);
PROC_GLOBAL(
void _batch_sub_blt, (pClipRect, PrimPtr),
    ARG		(PRECT		pClipRect)
    ARG_END	(PRIMITIVEPTR	PrimPtr)
);
PROC_GLOBAL(
void _batch_sub_color_blt, (pClipRect, PrimPtr),
    ARG		(PRECT		pClipRect)
    ARG_END	(PRIMITIVEPTR	PrimPtr)
);
PROC_GLOBAL(
void _batch_flip_blt, (pClipRect, PrimPtr),
    ARG		(PRECT		pClipRect)
    ARG_END	(PRIMITIVEPTR	PrimPtr)
);
PROC_GLOBAL(
void _vga_fillrect_blt, (pClipRect, PrimPtr),
    ARG		(PRECT		pClipRect)
    ARG_END	(PRIMITIVEPTR	PrimPtr)
);
PROC_GLOBAL(
void _vga_line_blt, (pClipRect, PrimPtr),
    ARG		(PRECT		pClipRect)
    ARG_END	(PRIMITIVEPTR	PrimPtr)
);
PROC_GLOBAL(
void _flush_scans, (),
    ARG_VOID
);
PROC_GLOBAL(
BOOLEAN _decrease_rect, (pInvalidRect),
    ARG_END	(PRECT	pInvalidRect)
);

#endif /* _VGA_H */

