#ifndef _MEMTYPE_H
#define _MEMTYPE_H

#define NUM_PRIO_DIVISIONS	10

typedef struct mem_queue
{
    MEM_HANDLE	head, tail;
} MEM_QUEUE;
typedef MEM_QUEUE	*PMEM_QUEUE;
typedef MEM_QUEUE	far *LPMEM_QUEUE;

typedef struct mem_header
{
    PROC_PARAMETER(
    MEM_BOOL (near *init_func), (),
	ARG_VOID
    );
    PROC_PARAMETER(
    MEM_BOOL (near *uninit_func), (),
	ARG_VOID
    );
    PROC_PARAMETER(
    MEM_BOOL (near *clear_func), (mem_loc, size),
	ARG	(MEM_LOC	mem_loc)
	ARG_END	(MEM_SIZE	size)
    );
    PROC_PARAMETER(
    MEM_BOOL (near *move_func),
	    (src_loc, src_offs, dst_loc, dst_offs, size),
	ARG	(MEM_LOC	src_loc)
	ARG	(MEM_SIZE	src_offs)
	ARG	(MEM_LOC	dst_loc)
	ARG	(MEM_SIZE	dst_offs)
	ARG_END	(MEM_SIZE	size)
    );
    PROC_PARAMETER(
    MEM_LOC (near *alloc_func), (size, flags),
	ARG	(MEM_SIZE	size)
	ARG_END	(MEM_FLAGS	flags)
    );
    PROC_PARAMETER(
    MEM_BOOL (near *free_func), (mem_loc, size),
	ARG	(MEM_LOC	mem_loc)
	ARG_END	(MEM_SIZE	size)
    );
    PROC_PARAMETER(
    MEM_SIZE (near *compact_func), (mem_desired),
	ARG_END	(MEM_SIZE	mem_desired)
    );
    PROC_PARAMETER(
    MEM_BOOL (near *store_func), (lpSrc, dst_loc, dst_offs, size),
	ARG	(LPSTR		lpSrc)
	ARG	(MEM_LOC	dst_loc)
	ARG	(MEM_SIZE	dst_offs)
	ARG_END	(MEM_SIZE	size)
    );
    PROC_PARAMETER(
    MEM_BOOL (near *restore_func), (src_loc, src_offs, lpDst, size),
	ARG	(MEM_LOC	src_loc)
	ARG	(MEM_SIZE	src_offs)
	ARG	(LPSTR		lpDst)
	ARG_END	(MEM_SIZE	size)
    );
    PROC_PARAMETER(
    MEM_BOOL (near *swap_func),
	    (desired_size, priority, src_type, dst_type),
	ARG	(PMEM_SIZE	desired_size)
	ARG	(MEM_PRIORITY	priority)
	ARG	(MEM_TYPE	src_type)
	ARG_END	(MEM_TYPE	dst_type)
    );
    PROC_PARAMETER(
    MEM_SIZE (near *biggest_block_func), (mem_desired),
	ARG_END	(MEM_SIZE	mem_desired)
    );
    PROC_PARAMETER(
    MEM_BOOL (near *less_core_func), (),
	ARG_VOID
    );
    MEM_FLAGS	flags;

    MEM_SIZE	limit, biggest_block, block_size;
    union
    {
	int	fd;
	LPVOID	addr;
	DWORD	loc;
    } descriptor;
    MEM_LOC	allocp;
    MEM_SIZE	max_core;
    MEM_QUEUE	prio_tab[NUM_PRIO_DIVISIONS];
} MEM_HEADER;
typedef MEM_HEADER	*PMEM_HEADER;
typedef MEM_HEADER	far *LPMEM_HEADER;

extern PMEM_HEADER	*_mem_tab;
extern MEM_PRIORITY	_mem_types;
extern PMEM_HEADER	_pCurHeader;

#define _mem_set_header(mt)	_pCurHeader = _mem_tab[mt]

PROC_GLOBAL(
MEM_TYPE near _mem_type_init,
	(primary_size, pmin_addressable, disk_name),
    ARG		(MEM_SIZE	primary_size)
    ARG		(PMEM_SIZE	pmin_addressable)
    ARG_END	(PSTR		disk_name)
);
PROC_GLOBAL(
void near _mem_type_uninit, (),
    ARG_VOID
);
PROC_GLOBAL(
MEM_LOC near _mem_type_alloc,
	(psize_request, orig_flags, priority, mem_type, pwhere),
    ARG		(PMEM_SIZE_REQUEST	psize_request)
    ARG		(MEM_FLAGS		orig_flags)
    ARG		(MEM_PRIORITY		priority)
    ARG		(MEM_TYPE		mem_type)
    ARG_END	(PMEM_TYPE		pwhere)
);
PROC_GLOBAL(
void near _mem_type_free, (mem_loc, size, type),
    ARG		(MEM_LOC	mem_loc)
    ARG		(MEM_SIZE	size)
    ARG_END	(MEM_TYPE	type)
);
PROC_GLOBAL(
MEM_SIZE near _mem_type_compact, (size, type),
    ARG		(MEM_SIZE	size)
    ARG_END	(MEM_TYPE	type)
);
PROC_GLOBAL(
MEM_BOOL near _mem_type_move,
	(src_loc, src_offs, src_type, dst_loc, dst_offs, dst_type, size),
    ARG		(MEM_LOC	src_loc)
    ARG		(MEM_SIZE	src_offs)
    ARG		(MEM_TYPE	src_type)
    ARG		(MEM_LOC	dst_loc)
    ARG		(MEM_SIZE	dst_offs)
    ARG		(MEM_TYPE	dst_type)
    ARG_END	(MEM_SIZE	size)
);
PROC_GLOBAL(
MEM_BOOL near _mem_type_swap, (desired_size, priority, type),
    ARG		(MEM_SIZE	desired_size)
    ARG		(MEM_PRIORITY	priority)
    ARG_END	(MEM_TYPE	type)
);
PROC_GLOBAL(
MEM_SIZE near _mem_type_get_memavailable, (size, priority, type),
    ARG		(MEM_SIZE	size)
    ARG		(MEM_PRIORITY	priority)
    ARG_END	(MEM_TYPE	type)
);

#define _mem_get_biggest_block(s)	(_pCurHeader->biggest_block_func == \
				    MEM_NULL ? _pCurHeader->biggest_block : \
				    (*_pCurHeader->biggest_block_func) (s))

#endif /* _MEMTYPE_H */

