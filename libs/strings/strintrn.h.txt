#ifndef _STRINTRN_H
#define _STRINTRN_H

#include <stdio.h>
#include <string.h>
#include "strlib.h"
#include "memlib.h"
#include "reslib.h"

typedef struct string_table
{
    COUNT	StringCount;
    DWORD	StringOffsets[1];
} STRING_TABLE_DESC;
typedef STRING_TABLE_DESC	near *NPSTRING_TABLE_DESC;
typedef STRING_TABLE_DESC	far *LPSTRING_TABLE_DESC;
typedef STRING_TABLE_DESC	*PSTRING_TABLE_DESC;

#define STRING_TABLEPTR		LPSTRING_TABLE_DESC

#define AllocStringTable(s)	AllocResourceData((s),MEM_SOUND)
#define LockStringTable(h,ps)	LockResourceData((MEM_HANDLE)LOWORD(h),ps)
#define UnlockStringTable(h)	UnlockResourceData ((MEM_HANDLE)LOWORD(h))
#define FreeStringTable(h)	FreeResourceData ((MEM_HANDLE)LOWORD(h))

#define STRING_INDEX(S)		((COUNT)HIWORD (S))
#define BUILD_STRING(h,i)	((STRING_TABLE)MAKE_DWORD(h,i))
#define BUILD_STRING_TABLE(h)	(STRING_TABLE)(h)

PROC_GLOBAL(
MEM_HANDLE _GetStringData, (fp, length),
    ARG		(FILE		*fp)
    ARG_END	(DWORD		length)
);

#endif /* _STRINTRN_H */

