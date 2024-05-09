#ifndef _MEMLIB_H
#define _MEMLIB_H

#include "compiler.h"

#define MEM_NULL	0L
#define NULL_HANDLE	(MEM_HANDLE)MEM_NULL

typedef unsigned long	MEM_SIZE;
typedef MEM_SIZE	*PMEM_SIZE;
typedef long		MEM_OFFS;

typedef UWORD		MEM_HANDLE;
typedef MEM_HANDLE	*PMEM_HANDLE;
typedef MEM_HANDLE	near *NPMEM_HANDLE;
typedef MEM_HANDLE	far *LPMEM_HANDLE;

typedef BOOLEAN		MEM_BOOL;
#define MEM_FAILURE	FALSE
#define MEM_SUCCESS	TRUE

enum
{
    SAME_MEM_USAGE = 0,
    MEM_SIMPLE,
    MEM_COMPLEX
};
typedef BYTE			MEM_USAGE;

#define NO_MEM_USAGE		(MEM_USAGE)SAME_MEM_USAGE
#define DEFAULT_MEM_USAGE	(MEM_USAGE)MEM_SIMPLE

enum
{
    MEM_NO_ACCESS = 0,
    MEM_READ_ONLY,
    MEM_WRITE_ONLY,
    MEM_READ_WRITE
};
typedef BYTE			MEM_ACCESS_MODE;

enum
{
    MEM_SEEK_REL,
    MEM_SEEK_ABS
};
typedef BYTE			MEM_SEEK_MODE;

enum
{
    MEM_FORWARD,
    MEM_BACKWARD
};
typedef BYTE			MEM_DIRECTION;

typedef struct mem_size_request
{
    MEM_SIZE		min_size, size, max_size;
    MEM_DIRECTION	direction;
} MEM_SIZE_REQUEST;

typedef MEM_SIZE_REQUEST	far *LPMEM_SIZE_REQUEST;
typedef SBYTE			MEM_PRIORITY;
typedef UWORD			MEM_FLAGS;

#define DEFAULT_MEM_FLAGS	(MEM_FLAGS)0
#define SAME_MEM_FLAGS		(MEM_FLAGS)0
#define MEM_CONSTRAINED		(MEM_FLAGS)(1 << 2)
#define MEM_DISCARDABLE		(MEM_FLAGS)(1 << 3)
#define MEM_NODISCARD		(MEM_FLAGS)(1 << 4)
#define MEM_DISCARDED		(MEM_FLAGS)(1 << 5)
#define MEM_ACCESSED		(MEM_FLAGS)(1 << 6)
#define MEM_STATIC		(MEM_FLAGS)(1 << 7)
#define MEM_PRIMARY		(MEM_FLAGS)(1 << 8)
#define MEM_ZEROINIT		(MEM_FLAGS)(1 << 9)
#define MEM_GRAPHICS		(MEM_FLAGS)(1 << 10)
#define MEM_SOUND		(MEM_FLAGS)(1 << 11)

#define DEFAULT_MEM_PRIORITY	(MEM_PRIORITY)0
#define SAME_MEM_PRIORITY	(MEM_PRIORITY)0
#define HIGHEST_MEM_PRIORITY	(MEM_PRIORITY)1
#define LOWEST_MEM_PRIORITY	(MEM_PRIORITY)100

PROC_GLOBAL(
MEM_BOOL mem_init, (core_size, pmin_addressable, disk_name),
    ARG		(MEM_SIZE	core_size)
    ARG		(PMEM_SIZE	pmin_addressable)
    ARG_END	(PSTR		disk_name)
);
PROC_GLOBAL(
MEM_BOOL mem_uninit, (),
    ARG_VOID
);

PROC_GLOBAL(
MEM_HANDLE PASCAL mem_allocate, (size, flags, priority, usage),
    ARG		(MEM_SIZE	size)
    ARG		(MEM_FLAGS	flags)
    ARG		(MEM_PRIORITY	priority)
    ARG_END	(MEM_USAGE	usage)
);
#define mem_request(size)	\
	mem_allocate((MEM_SIZE)(size), DEFAULT_MEM_FLAGS, DEFAULT_MEM_PRIORITY, DEFAULT_MEM_USAGE)
PROC_GLOBAL(
MEM_BOOL mem_release, (handle),
    ARG_END	(MEM_HANDLE	handle)
);
PROC_GLOBAL(
MEM_HANDLE mem_reallocate, (handle, size, flags, priority, usage),
    ARG		(MEM_HANDLE	handle)
    ARG		(MEM_SIZE	size)
    ARG		(MEM_FLAGS	flags)
    ARG		(MEM_PRIORITY	priority)
    ARG_END	(MEM_USAGE	usage)
);
#define mem_resize(h, size)	\
	mem_reallocate(h, (MEM_SIZE)size, SAME_MEM_FLAGS, SAME_MEM_PRIORITY, SAME_MEM_USAGE)
PROC_GLOBAL(
MEM_HANDLE mem_static, (lpVoid, size, usage),
    ARG		(LPVOID		lpVoid)
    ARG		(MEM_SIZE	size)
    ARG_END	(MEM_USAGE	usage)
);
PROC_GLOBAL(
MEM_HANDLE mem_dup, (handle, usage),
    ARG		(MEM_HANDLE	handle)
    ARG_END	(MEM_USAGE	usage)
);

PROC_GLOBAL(
LPVOID mem_simple_access, (handle),
    ARG_END	(MEM_HANDLE	handle)
);
PROC_GLOBAL(
MEM_BOOL mem_simple_unaccess, (handle),
    ARG_END	(MEM_HANDLE	handle)
);

#define mem_lock	mem_simple_access
#define mem_unlock	mem_simple_unaccess

PROC_GLOBAL(
LPVOID mem_complex_access, (handle, pos, lpSizeRequest, access_mode),
    ARG		(MEM_HANDLE		handle)
    ARG		(MEM_SIZE		pos)
    ARG		(LPMEM_SIZE_REQUEST	lpSizeRequest)
    ARG_END	(MEM_ACCESS_MODE	access_mode)
);
PROC_GLOBAL(
MEM_BOOL mem_complex_unaccess, (handle),
    ARG_END	(MEM_HANDLE	handle)
);
PROC_GLOBAL(
LPVOID mem_complex_seek, (handle, offset, lpSizeRequest, seek_mode),
    ARG		(MEM_HANDLE		handle)
    ARG		(MEM_OFFS		offset)
    ARG		(LPMEM_SIZE_REQUEST	lpSizeRequest)
    ARG_END	(MEM_SEEK_MODE		seek_mode)
);

PROC_GLOBAL(
MEM_SIZE mem_get_memavailable, (size, priority),
    ARG		(MEM_SIZE	size)
    ARG_END	(MEM_PRIORITY	priority)
);

PROC_GLOBAL(
MEM_SIZE mem_get_size, (handle),
    ARG_END	(MEM_HANDLE	handle)
);
PROC_GLOBAL(
MEM_USAGE mem_get_usage, (handle),
    ARG_END	(MEM_HANDLE	handle)
);
PROC_GLOBAL(
MEM_BOOL mem_set_flags, (handle, mem_flags),
    ARG		(MEM_HANDLE	handle)
    ARG_END	(MEM_FLAGS	mem_flags)
);
PROC_GLOBAL(
MEM_FLAGS mem_get_flags, (handle),
    ARG_END	(MEM_HANDLE	handle)
);
PROC_GLOBAL(
MEM_BOOL mem_set_priority, (handle, priority),
    ARG		(MEM_HANDLE	handle)
    ARG_END	(MEM_PRIORITY	mem_priority)
);
PROC_GLOBAL(
MEM_PRIORITY mem_get_priority, (handle),
    ARG_END	(MEM_HANDLE	handle)
);

#endif /* _MEMLIB_H */

