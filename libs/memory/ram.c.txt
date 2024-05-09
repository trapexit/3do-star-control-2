#include "memintrn.h"

#define OPT_SIZE	(0x0000FFFF - PARAGRAPH_SIZE)

PROC(
void near _mem_ram_copy, (lpSrc, lpDst, size),
    ARG		(LPSTR		lpSrc)
    ARG		(LPSTR		lpDst)
    ARG_END	(MEM_SIZE	size)
)
{
    COUNT	loc_size;

    FAR_PTR_ADD (&lpDst, 0);
    FAR_PTR_ADD (&lpSrc, 0);
    do
    {
	loc_size = size >= (MEM_SIZE)OPT_SIZE ? OPT_SIZE : (COUNT)size;
	MEMMOVE (lpDst, lpSrc, loc_size);
	FAR_PTR_ADD (&lpDst, loc_size);
	FAR_PTR_ADD (&lpSrc, loc_size);
    } while (size -= loc_size);
}

PROC(
void near _mem_ram_zero, (lpSrc, size),
    ARG		(LPSTR		lpSrc)
    ARG_END	(MEM_SIZE	size)
)
{
    COUNT	loc_size;

    FAR_PTR_ADD (&lpSrc, 0);
    do
    {
	loc_size = size >= (MEM_SIZE)OPT_SIZE ? OPT_SIZE : (COUNT)size;
	MEMSET (lpSrc, 0, loc_size);
	FAR_PTR_ADD (&lpSrc, loc_size);
    } while (size -= loc_size);
}

PROC(
MEM_BOOL near _mem_ram_move, (src_loc, src_offs, dst_loc, dst_offs, size),
    ARG		(MEM_LOC	src_loc)
    ARG		(MEM_SIZE	src_offs)
    ARG		(MEM_LOC	dst_loc)
    ARG		(MEM_SIZE	dst_offs)
    ARG_END	(MEM_SIZE	size)
)
{
    LPSTR	lpSrc, lpDst;

    _mem_ram_lock (src_loc, &lpSrc);
    if (dst_loc == src_loc)
	lpDst = lpSrc;
    else
    {
	_mem_ram_lock (dst_loc, &lpDst);
    }

    FAR_PTR_ADD (&lpSrc, src_offs);
    FAR_PTR_ADD (&lpDst, dst_offs);
    _mem_ram_copy (lpSrc, lpDst, size);

    _mem_ram_unlock (src_loc);
    if (dst_loc != src_loc)
	_mem_ram_unlock (dst_loc);
	
    return (MEM_SUCCESS);
}

PROC(
MEM_BOOL near _mem_ram_clear, (mem_loc, size),
    ARG		(MEM_LOC	mem_loc)
    ARG_END	(MEM_SIZE	size)
)
{
    LPSTR	lpMem;

    _mem_ram_lock (mem_loc, &lpMem);
    _mem_ram_zero (lpMem, size);
    _mem_ram_unlock (mem_loc);
    
    return (MEM_SUCCESS);
}

PROC(
MEM_BOOL near _mem_ram_store, (lpSrc, dst_loc, dst_offs, size),
    ARG		(LPSTR		lpSrc)
    ARG		(MEM_LOC	dst_loc)
    ARG		(MEM_SIZE	dst_offs)
    ARG_END	(MEM_SIZE	size)
)
{
    LPSTR	lpDst;

    _mem_ram_lock (dst_loc, &lpDst);
    FAR_PTR_ADD (&lpDst, dst_offs);
    _mem_ram_copy (lpSrc, lpDst, size);
    _mem_ram_unlock (dst_loc);

    return (MEM_SUCCESS);
}

PROC(
MEM_BOOL near _mem_ram_restore, (src_loc, src_offs, lpDst, size),
    ARG		(MEM_LOC	src_loc)
    ARG		(MEM_SIZE	src_offs)
    ARG		(LPSTR		lpDst)
    ARG_END	(MEM_SIZE	size)
)
{
    LPSTR	lpSrc;

    _mem_ram_lock (src_loc, &lpSrc);
    FAR_PTR_ADD (&lpSrc, src_offs);
    _mem_ram_copy (lpSrc, lpDst, size);
    _mem_ram_unlock (src_loc);

    return (MEM_SUCCESS);
}



