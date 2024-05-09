#ifndef _RESLIB_H
#define _RESLIB_H

#ifndef SEGA
#include <stdio.h>
#endif /* SEGA */
#include "memlib.h"

typedef DWORD			RESOURCE;
typedef RESOURCE		*PRESOURCE;
typedef RESOURCE		near *NPRESOURCE;
typedef RESOURCE		far *LPRESOURCE;

typedef BYTE			RES_TYPE;
typedef COUNT			RES_INSTANCE;
typedef COUNT			RES_PACKAGE;

#define TYPE_BITS		8
#define INSTANCE_BITS		13
#define PACKAGE_BITS		11

#define MAX_TYPES		((1 << TYPE_BITS) - 1)	/* zero is invalid */
#define MAX_INSTANCES		(1 << INSTANCE_BITS)
#define MAX_PACKAGES		((1 << PACKAGE_BITS) - 1) /* zero is invalid */

#define GET_TYPE(res)		\
	((RES_TYPE)(res) & (RES_TYPE)((1 << TYPE_BITS) - 1))
#define GET_INSTANCE(res)	\
	((RES_INSTANCE)((res) >> TYPE_BITS) & ((1 << INSTANCE_BITS) - 1))
#define GET_PACKAGE(res)	\
	((RES_PACKAGE)((res) >> (TYPE_BITS + INSTANCE_BITS)) & \
	((1 << PACKAGE_BITS) - 1))
#define MAKE_RESOURCE(p,t,i)	\
	(((RESOURCE)(p) << (TYPE_BITS + INSTANCE_BITS)) | \
	((RESOURCE)(i) << TYPE_BITS) | \
	((RESOURCE)(t)))

PROC_GLOBAL(
FILE *OpenResFile, (filename, mode, buffer, buf_size, FileErrorFunc),
    ARG		(PVOID	filename)
    ARG		(char	*mode)
    ARG		(char	*buffer)
    ARG		(DWORD	buf_size)
    ARG_END
    (
	PROC_PARAMETER(
	BOOLEAN (*FileErrorFunc), (filename),
	    ARG_END	(PVOID	filename)
	)
    )
);
PROC_GLOBAL(
int ReadResFile, (lpBuf, size, count, fp),
    ARG		(LPVOID	lpBuf)
    ARG		(COUNT	size)
    ARG		(COUNT	count)
    ARG_END	(FILE	*fp)
);
PROC_GLOBAL(
int WriteResFile, (lpBuf, size, count, fp),
    ARG		(LPVOID	lpBuf)
    ARG		(COUNT	size)
    ARG		(COUNT	count)
    ARG_END	(FILE	*fp)
);
PROC_GLOBAL(
int GetResFileChar, (fp),
    ARG_END	(FILE	*fp)
);
PROC_GLOBAL(
int PutResFileChar, (ch, fp),
    ARG		(char	ch)
    ARG_END	(FILE	*fp)
);
PROC_GLOBAL(
long SeekResFile, (fp, offset, whence),
    ARG		(FILE	*fp)
    ARG		(long	offset)
    ARG_END	(int	whence)
);
PROC_GLOBAL(
long TellResFile, (fp),
    ARG_END	(FILE	*fp)
);
PROC_GLOBAL(
long LengthResFile, (fp),
    ARG_END	(FILE	*fp)
);
PROC_GLOBAL(
BOOLEAN CloseResFile, (fp),
    ARG_END	(FILE	*fp)
);
PROC_GLOBAL(
BOOLEAN DeleteResFile, (filename),
    ARG_END	(PVOID	filename)
);

PROC_GLOBAL(
MEM_HANDLE InitResourceSystem, (resfile, resindex_type, FileErrorFunc),
    ARG		(PVOID		resfile)
    ARG		(COUNT		resindex_type)
    ARG_END
    (
	PROC_PARAMETER(
	BOOLEAN (*FileErrorFunc), (filename),
	    ARG_END	(PVOID	filename)
	)
    )
);
PROC_GLOBAL(
BOOLEAN UninitResourceSystem, (),
    ARG_VOID
);
PROC_GLOBAL(
BOOLEAN InstallResTypeVectors, (res_type, load_func, free_func),
    ARG		(COUNT		res_type)
    ARG
    (
	PROC_PARAMETER(
	MEM_HANDLE (*load_func), (fp, len),
	    ARG		(FILE		*fp)
	    ARG_END	(DWORD		len)
	)
    )
    ARG_END
    (
	PROC_PARAMETER(
	BOOLEAN (*free_func), (handle),
	    ARG_END	(MEM_HANDLE	handle)
	)
    )
);
PROC_GLOBAL(
MEM_HANDLE GetResource, (res),
    ARG_END	(RESOURCE	res)
);
PROC_GLOBAL(
MEM_HANDLE DetachResource, (res),
    ARG_END	(RESOURCE	res)
);
PROC_GLOBAL(
BOOLEAN FreeResource, (res),
    ARG_END	(RESOURCE	res)
);
PROC_GLOBAL(
COUNT CountResourceTypes, (),
    ARG_VOID
);

PROC_GLOBAL(
MEM_HANDLE OpenResourceIndexFile, (resfile),
    ARG_END	(PVOID		resfile)
);
PROC_GLOBAL(
MEM_HANDLE OpenResourceIndexInstance, (res),
    ARG_END	(DWORD		res)
);
PROC_GLOBAL(
MEM_HANDLE SetResourceIndex, (hRH),
    ARG_END	(MEM_HANDLE	hRH)
);
PROC_GLOBAL(
BOOLEAN CloseResourceIndex, (hRH),
    ARG_END	(MEM_HANDLE	hRH)
);

PROC_GLOBAL(
MEM_HANDLE GetResourceData, (fp, length, mem_flags),
    ARG		(FILE		*fp)
    ARG		(DWORD		length)
    ARG_END	(MEM_FLAGS	mem_flags)
);

#define RESOURCE_PRIORITY	DEFAULT_MEM_PRIORITY
#define RESOURCE_DATAPTR	LPBYTE

#define AllocResourceData(s,mf)		\
    mem_allocate ((MEM_SIZE)(s)+(PARAGRAPH_SIZE-1), \
	    (mf), RESOURCE_PRIORITY, MEM_SIMPLE)
#define LockResourceData(h,lp)		\
do \
{ \
    *(lp) = mem_lock ((MEM_HANDLE)(h)); \
    if (GET_SEGMENT (lp) && GET_OFFSET (lp)) \
    { \
	SET_OFFSET (lp, 0); \
	SET_SEGMENT (lp, GET_SEGMENT (lp) + 1); \
    } \
} while (0)
#define UnlockResourceData		mem_unlock
#define FreeResourceData		mem_release

#include "strlib.h"

typedef STRING_TABLE	DIRENTRY_REF;
typedef STRING		DIRENTRY;
typedef STRINGPTR	DIRENTRYPTR;

PROC_GLOBAL(
DIRENTRY_REF LoadDirEntryTable, (pattern, pnum_entries),
    ARG		(PVOID		pattern)
    ARG_END	(PCOUNT		pnum_entries)
);
#define CaptureDirEntryTable		CaptureStringTable
#define ReleaseDirEntryTable		ReleaseStringTable
#define DestroyDirEntryTable		DestroyStringTable
#define GetDirEntryTableRef		GetStringTable
#define GetDirEntryTableCount		GetStringTableCount
#define GetDirEntryTableIndex		GetStringTableIndex
#define SetAbsDirEntryTableIndex	SetAbsStringTableIndex
#define SetRelDirEntryTableIndex	SetRelStringTableIndex
#define GetDirEntryLength		GetStringLength
#define GetDirEntryAddress		GetStringAddress
#define GetDirEntryContents		GetStringContents

#endif /* _RESLIB_H */

