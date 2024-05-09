#include <stdlib.h>
#include "memintrn.h"
#include "n_ary.h"

extern MEM_HEADER	_mem_tertiary_header;

PROC_GLOBAL(
void *AllocDRAM, (size),
    ARG_END	(DWORD	size)
);
PROC_GLOBAL(
void FreeDRAM, (p, size),
    ARG		(void	*p)
    ARG_END	(DWORD	size)
);

// #define FreeDRAM(p,s)	free(p)
// #define AllocDRAM(s)	malloc(s)

PROC(
MEM_BOOL near _mem_tertiary_init, (),
    ARG_VOID
)
{
    MEM_SIZE		core_size;
    struct
    {
	DWORD	LargestBlockAvail;
	DWORD	MaxUnlockedPage;
	DWORD	LargestLockablePage;
	DWORD	LinAddrSpace;
	DWORD	NumFreePagesAvail;
	DWORD	NumPhysicalPagesFree;
	DWORD	TotalPhysicalPages;
	DWORD	FreeLinAddrSpace;
	DWORD	SizeOfPageFile;
	DWORD	Reserved[3 * 50];
    } MemInfo;

extern DWORD	cur_size;

MemInfo.LargestBlockAvail = 0;
{
DWORD	lo, hi;

lo = 0;
hi = cur_size;
do
{
    void	*p;
    DWORD	mid;

    mid = (lo + hi) >> 1;
    if ((p = AllocDRAM (mid)) == 0)
	hi = mid - 1;
    else
    {
	MemInfo.LargestBlockAvail = mid;
	
	FreeDRAM (p, mid);
	lo = mid + 1;
    }
} while (lo < hi);
}
    if (MemInfo.LargestBlockAvail)
    {
		/* limit set initially by _mem_imp_init */
	if ((core_size = _mem_tertiary_header.limit) == 0
		|| core_size > MemInfo.LargestBlockAvail)
	    core_size = MemInfo.LargestBlockAvail;

	while ((_mem_tertiary_header.descriptor.addr = AllocDRAM (core_size)) == 0
		&& core_size > 128)
	    core_size -= 128;

cur_size = core_size;

	if (_mem_tertiary_header.descriptor.addr)
	{
	    _mem_tertiary_header.limit = core_size;
	    _mem_tertiary_header.biggest_block = 0;
	    _mem_tertiary_header.block_size = 4;

//#ifdef DEBUG
printf ("Available tertiary memory = %lu at 0x%08x\n",
_mem_tertiary_header.limit,
_mem_tertiary_header.descriptor.addr);
//#endif /* DEBUG */
	    _mem_n_ary_init ();

	    return (MEM_SUCCESS);
	}
    }

    return (MEM_FAILURE);
}

PROC(STATIC
MEM_BOOL near _mem_tertiary_uninit, (),
    ARG_VOID
)
{
    PVOID	addr;

    if ((addr = (PVOID)_mem_tertiary_header.descriptor.addr) != 0)
    {
	_mem_tertiary_header.descriptor.addr = 0;

	FreeDRAM (addr, _mem_tertiary_header.limit);
	return (MEM_SUCCESS);
    }

    return (MEM_FAILURE);
}

MEM_HEADER	_mem_tertiary_header =
{
    _mem_tertiary_init,
    _mem_tertiary_uninit,
    _mem_ram_clear,
    _mem_ram_move,
    _mem_n_ary_alloc,
    _mem_n_ary_free,
    MEM_NULL,
    _mem_ram_store,
    _mem_ram_restore,
    _mem_n_ary_swap,
    _mem_n_ary_biggest_block,
    MEM_NULL,
    MEM_MANAGED_ARENA
	    | MEM_PRIMARY
	    | MEM_GRAPHICS
	    | MEM_SOUND
	    | MEM_ADDRESSABLE
	    | MEM_NORMAL,
};
