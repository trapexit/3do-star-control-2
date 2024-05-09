#ifndef _RAM_H
#define _RAM_H

PROC_GLOBAL(
void near _mem_ram_copy, (lpSrc, lpDst, size),
    ARG		(LPSTR		lpSrc)
    ARG		(LPSTR		lpDst)
    ARG_END	(MEM_SIZE	size)
);
PROC_GLOBAL(
void near _mem_ram_zero, (lpSrc, size),
    ARG		(LPSTR		lpSrc)
    ARG_END	(MEM_SIZE	size)
);
PROC_GLOBAL(
MEM_BOOL near _mem_ram_move, (src_loc, src_offs, dst_loc, dst_offs, size),
    ARG		(MEM_LOC	src_loc)
    ARG		(MEM_SIZE	src_offs)
    ARG		(MEM_LOC	dst_loc)
    ARG		(MEM_SIZE	dst_offs)
    ARG_END	(MEM_SIZE	size)
);
PROC_GLOBAL(
MEM_BOOL near _mem_ram_clear, (mem_loc, size),
    ARG		(MEM_LOC	mem_loc)
    ARG_END	(MEM_SIZE	size)
);
PROC_GLOBAL(
MEM_BOOL near _mem_ram_store, (lpSrc, dst_loc, dst_offs, size),
    ARG		(LPSTR		lpSrc)
    ARG		(MEM_LOC	dst_loc)
    ARG		(MEM_SIZE	dst_offs)
    ARG_END	(MEM_SIZE	size)
);
PROC_GLOBAL(
MEM_BOOL near _mem_ram_restore, (src_loc, src_offs, lpDst, size),
    ARG		(MEM_LOC	src_loc)
    ARG		(MEM_SIZE	src_offs)
    ARG		(LPSTR		lpDst)
    ARG_END	(MEM_SIZE	size)
);

#endif /* _RAM_H */

