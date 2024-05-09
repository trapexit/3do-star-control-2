#ifndef _MEMINTRN_H
#define _MEMINTRN_H

#include <string.h>
#include "memlib.h"
#ifndef _TIMLIB_H
#include "timlib.h"
#else /* _TIMLIB_H */
typedef BYTE		SEMAPHORE;
#define SetSemaphore(psem)
#define ClearSemaphore(psem)
#endif /* _TIMLIB_H */

#define MEM_NORMAL		(MEM_FLAGS)(1 << 12)
#define MEM_DUP			(MEM_FLAGS)(1 << 13)
#define MEM_ACCESS_PRIMARY	(MEM_FLAGS)(1 << 14)
#define MEM_ADDRESSABLE		(MEM_FLAGS)(1 << 15)
#define MEM_ALLOC_FLAGS		(MEM_CONSTRAINED | MEM_DISCARDABLE \
				 | MEM_NODISCARD | MEM_ZEROINIT \
				 | MEM_PRIMARY | MEM_GRAPHICS | MEM_SOUND)

#define MEM_MANAGED_ARENA	(MEM_FLAGS)(1 << 0)
#define MEM_ARENA_CREATED	(MEM_FLAGS)(1 << 1)

typedef DWORD		MEM_LOC;
typedef BYTE		MEM_COUNT;
typedef SBYTE		MEM_TYPE;

#define INVALID_MEM_TYPE	(MEM_TYPE)-1
#define DEFAULT_MEM_TYPE	(MEM_TYPE)0

typedef MEM_LOC			*PMEM_LOC;
typedef MEM_LOC			far *LPMEM_LOC;
typedef MEM_SIZE		far *LPMEM_SIZE;
typedef MEM_FLAGS		*PMEM_FLAGS;
typedef MEM_FLAGS		far *LPMEM_FLAGS;
typedef MEM_PRIORITY		*PMEM_PRIORITY;
typedef MEM_PRIORITY		far *LPMEM_PRIORITY;
typedef MEM_USAGE		*PMEM_USAGE;
typedef MEM_USAGE		far *LPMEM_USAGE;
typedef MEM_TYPE		*PMEM_TYPE;
typedef MEM_TYPE		far *LPMEM_TYPE;
typedef MEM_SIZE_REQUEST	*PMEM_SIZE_REQUEST;

typedef struct mem_attrs
{
    MEM_LOC		virtual;	/* position in that type of memory */
    MEM_SIZE		size;		/* in bytes */
    MEM_FLAGS		flags;
    MEM_PRIORITY	priority;
    MEM_TYPE		where;		/* memory type where data is stored */
} MEM_ATTRS;
typedef MEM_ATTRS	*PMEM_ATTRS;

typedef struct mem_info
{
    MEM_HANDLE		pred_info, succ_info;

    MEM_ATTRS		attrs;

    MEM_COUNT		ref_count;	/* for mem_dup's */
    MEM_COUNT		access_count;	/* for mem_access'es */
} MEM_INFO;
typedef MEM_INFO	far *LPMEM_INFO;

typedef struct simple_handle
{
    MEM_HANDLE		mem_info;
    
    MEM_USAGE		usage;
    MEM_COUNT		access_count;
} SIMPLE_HANDLE;
typedef SIMPLE_HANDLE		*PSIMPLE_HANDLE;
typedef SIMPLE_HANDLE		near *NPSIMPLE_HANDLE;
typedef SIMPLE_HANDLE		far *LPSIMPLE_HANDLE;
 
typedef struct complex_handle
{
    MEM_HANDLE		mem_info;

    MEM_USAGE		usage;
    MEM_COUNT		access_count;

    LPVOID		physical;/* pointer user gets to accessed memory */
    struct
    {
	MEM_TYPE	mem_type;
    	MEM_LOC		primary;
	MEM_SIZE	curpos, cursize;	/* position in memory */
	MEM_DIRECTION	curdir;			/* current access direction */
	MEM_ACCESS_MODE	access_mode;
    } window;
} COMPLEX_HANDLE;
typedef COMPLEX_HANDLE		*PCOMPLEX_HANDLE;
typedef COMPLEX_HANDLE		near *NPCOMPLEX_HANDLE;
typedef COMPLEX_HANDLE		far *LPCOMPLEX_HANDLE;

PROC_GLOBAL(
MEM_FLAGS near _mem_get_alloc_flags, (flags),
    ARG_END	(MEM_FLAGS	flags)
);
PROC_GLOBAL(
MEM_PRIORITY near _mem_get_alloc_priority, (priority),
    ARG_END	(MEM_PRIORITY	priority)
);
PROC_GLOBAL(
MEM_USAGE near _mem_get_alloc_usage, (usage),
    ARG_END	(MEM_USAGE	usage)
);

PROC_GLOBAL(
void near _mem_alloc_data, (pmem_attrs),
    ARG_END	(PMEM_ATTRS	pmem_attrs)
);

#define _mem_get_memtype(p)	(MEM_TYPE)((p)==DEFAULT_MEM_PRIORITY ? \
					DEFAULT_MEM_PRIORITY : \
					(((p) - HIGHEST_MEM_PRIORITY) / \
					(MEM_PRIORITY)((LOWEST_MEM_PRIORITY - \
					HIGHEST_MEM_PRIORITY \
					+ (MEM_PRIORITY)1) / \
					(MEM_PRIORITY)_mem_types + 1)))
#define _mem_get_num_memtypes()	_mem_types
#define _mem_get_last_memtype()	(_mem_get_num_memtypes () - (MEM_TYPE)1)

#include "memerr.h"
#include "memtype.h"
#include "memhash.h"
#include "memimp.h"
#include "hndlinfo.h"
#include "ram.h"

extern MEM_TYPE		_mem_types;
extern SEMAPHORE	_MemorySem;

#endif /* _MEMINTRN_H */

