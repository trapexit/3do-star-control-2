#include <stdlib.h>
#include "memintrn.h"
#include "n_ary.h"

extern MEM_HEADER	_mem_quaternary_header;

PROC_GLOBAL(
void *AllocVRAM, (size),
    ARG_END	(DWORD	size)
);
PROC_GLOBAL(
void FreeVRAM, (p, size),
    ARG		(void	*p)
    ARG_END	(DWORD	size)
);

// #define FreeVRAM(p,s)	free(p)
// #define AllocVRAM(s)	malloc(s)

PROC(
MEM_BOOL near _mem_quaternary_init, (),
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

extern DWORD	vcur_size;

MemInfo.LargestBlockAvail = 0;
{
DWORD	lo, hi;

lo = 0;
hi = 100000; //vcur_size;
do
{
    void	*p;
    DWORD	mid;

    mid = (lo + hi) >> 1;
    if ((p = AllocVRAM (mid)) == 0)
	hi = mid - 1;
    else
    {
	MemInfo.LargestBlockAvail = mid;
	
	FreeVRAM (p, mid);
	lo = mid + 1;
    }
} while (lo < hi);
}
    if (MemInfo.LargestBlockAvail)
    {
		/* limit set initially by _mem_imp_init */
	if ((core_size = _mem_quaternary_header.limit) == 0
		|| core_size > MemInfo.LargestBlockAvail)
	    core_size = MemInfo.LargestBlockAvail;

	while ((_mem_quaternary_header.descriptor.addr = AllocVRAM (core_size)) == 0
		&& core_size > 128)
	    core_size -= 128;

vcur_size = core_size;

	if (_mem_quaternary_header.descriptor.addr)
	{
	    _mem_quaternary_header.limit = core_size;
	    _mem_quaternary_header.biggest_block = 0;
	    _mem_quaternary_header.block_size = 4;

//#ifdef DEBUG
printf ("Available quaternary memory = %lu at 0x%08x\n",
_mem_quaternary_header.limit,
_mem_quaternary_header.descriptor.addr);
//#endif /* DEBUG */
	    _mem_n_ary_init ();

	    return (MEM_SUCCESS);
	}
    }

    return (MEM_FAILURE);
}

PROC(STATIC
MEM_BOOL near _mem_quaternary_uninit, (),
    ARG_VOID
)
{
    PVOID	addr;

    if ((addr = (PVOID)_mem_quaternary_header.descriptor.addr) != 0)
    {
	_mem_quaternary_header.descriptor.addr = 0;

	FreeVRAM (addr, _mem_quaternary_header.limit);
_mem_quaternary_header.flags = MEM_MANAGED_ARENA
	    | MEM_PRIMARY
	    | MEM_GRAPHICS
	    | MEM_SOUND
	    | MEM_ADDRESSABLE
	    | MEM_NORMAL;

_mem_quaternary_header.limit = 0;
_mem_quaternary_header.biggest_block = 0;
_mem_quaternary_header.block_size = 0;

_mem_quaternary_header.allocp = 0;
_mem_quaternary_header.max_core = 0;
MEMSET (_mem_quaternary_header.prio_tab, 0, sizeof  (_mem_quaternary_header.prio_tab));

	return (MEM_SUCCESS);
    }

    return (MEM_FAILURE);
}

MEM_HEADER	_mem_quaternary_header =
{
    _mem_quaternary_init,
    _mem_quaternary_uninit,
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
