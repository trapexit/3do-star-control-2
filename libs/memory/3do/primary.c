#include <stdlib.h>
#include "memintrn.h"
#include "n_ary.h"

extern MEM_HEADER	_mem_primary_header;

//#define ALL_AT_ONCE

#ifdef ALL_AT_ONCE
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

//DWORD	cur_size = 2 * 1024000;

PROC(STATIC
MEM_BOOL near _mem_primary_init, (),
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


MemInfo.LargestBlockAvail = 0;
{
DWORD	lo, hi;

lo = 0;
hi = 2000 * 1024;//cur_size;
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
	if ((core_size = _mem_primary_header.limit) == 0
		|| core_size > MemInfo.LargestBlockAvail)
	    core_size = MemInfo.LargestBlockAvail;

	while ((_mem_primary_header.descriptor.addr = AllocDRAM (core_size)) == 0
		&& core_size > 128)
	    core_size -= 128;

//cur_size = core_size;

	if (_mem_primary_header.descriptor.addr)
	{
	    _mem_primary_header.limit = core_size;
	    _mem_primary_header.biggest_block = 0;
	    _mem_primary_header.block_size = 4;

//#ifdef DEBUG
printf ("Available primary memory = %lu at 0x%08x\n",
_mem_primary_header.limit,
_mem_primary_header.descriptor.addr);
//#endif /* DEBUG */
	    _mem_n_ary_init ();

	    return (MEM_SUCCESS);
	}
    }

    return (MEM_FAILURE);
}

PROC(STATIC
MEM_BOOL near _mem_primary_uninit, (),
    ARG_VOID
)
{
    PVOID	addr;

    if ((addr = (PVOID)_mem_primary_header.descriptor.addr) != 0)
    {
	_mem_primary_header.descriptor.addr = 0;

	FreeDRAM (addr, _mem_primary_header.limit);
	
_mem_primary_header.flags = MEM_MANAGED_ARENA
	    | MEM_PRIMARY
	    | MEM_GRAPHICS
	    | MEM_SOUND
	    | MEM_ADDRESSABLE
	    | MEM_NORMAL;

_mem_primary_header.limit = 0;
_mem_primary_header.biggest_block = 0;
_mem_primary_header.block_size = 0;

_mem_primary_header.allocp = 0;
_mem_primary_header.max_core = 0;
MEMSET (_mem_primary_header.prio_tab, 0, sizeof  (_mem_primary_header.prio_tab));

	return (MEM_SUCCESS);
    }

    return (MEM_FAILURE);
}
MEM_HEADER	_mem_primary_header =
{
    _mem_primary_init,
    _mem_primary_uninit,
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
#else
PROC(STATIC
MEM_BOOL near _mem_primary_init, (),
    ARG_VOID
)
{
    return (MEM_SUCCESS);
}

PROC(STATIC
MEM_BOOL near _mem_primary_uninit, (),
    ARG_VOID
)
{
    return (MEM_SUCCESS);
}

static MEM_LOC
_mem_primary_alloc (MEM_SIZE size, MEM_FLAGS flags)
{
    size = (size + 3) & ~3;
    if ((_mem_primary_header.allocp += size) > _mem_primary_header.max_core)
	_mem_primary_header.max_core = _mem_primary_header.allocp;

    return ((MEM_LOC)ThreedoAlloc (size));
}

static MEM_BOOL
_mem_primary_free (MEM_LOC p, MEM_SIZE size)
{
    if (size && p)
    {
	size = (size + 3) & ~3;
	ThreedoFree (p, size);
	
	_mem_primary_header.allocp -= size;
    }

    return (TRUE);
}

MEM_HEADER	_mem_primary_header =
{
    _mem_primary_init,
    _mem_primary_uninit,
    NULL,//_mem_ram_clear,
    NULL,//_mem_ram_move,
    _mem_primary_alloc,//_mem_n_ary_alloc,
    _mem_primary_free,//_mem_n_ary_free,
    MEM_NULL,
    NULL,//_mem_ram_store,
    NULL,//_mem_ram_restore,
    NULL,//_mem_n_ary_swap,
    NULL,//_mem_n_ary_biggest_block,
    MEM_NULL,
    /*MEM_MANAGED_ARENA
	    |*/ MEM_PRIMARY
	    | MEM_GRAPHICS
	    | MEM_SOUND
	    | MEM_ADDRESSABLE
	    | MEM_NORMAL,
};
#endif
