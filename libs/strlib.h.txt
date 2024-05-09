#ifndef _STRLIB_H
#define _STRLIB_H

#include "compiler.h"

typedef DWORD	STRING_TABLE;
typedef DWORD	STRING;
typedef LPBYTE	STRINGPTR;

PROC_GLOBAL(
BOOLEAN InstallStringTableResType, (string_type),
    ARG_END	(COUNT	string_type)
);
PROC_GLOBAL(
STRING_TABLE LoadStringTableInstance, (res),
    ARG_END	(DWORD	res)
);
PROC_GLOBAL(
STRING_TABLE LoadStringTableFile, (pStr),
    ARG_END	(PVOID	pStr)
);
PROC_GLOBAL(
BOOLEAN DestroyStringTable, (StringTable),
    ARG_END	(STRING_TABLE	StringTable)
);
PROC_GLOBAL(
STRING CaptureStringTable, (StringTable),
    ARG_END	(STRING_TABLE	StringTable)
);
PROC_GLOBAL(
STRING_TABLE ReleaseStringTable, (String),
    ARG_END	(STRING	String)
);
PROC_GLOBAL(
STRING_TABLE GetStringTable, (String),
    ARG_END	(STRING	String)
);
PROC_GLOBAL(
COUNT GetStringTableCount, (String),
    ARG_END	(STRING	String)
);
PROC_GLOBAL(
COUNT GetStringTableIndex, (String),
    ARG_END	(STRING	String)
);
PROC_GLOBAL(
STRING SetAbsStringTableIndex, (String, StringTableIndex),
    ARG		(STRING	String)
    ARG_END	(COUNT	StringTableIndex)
);
PROC_GLOBAL(
STRING SetRelStringTableIndex, (String, StringTableOffs),
    ARG		(STRING	String)
    ARG_END	(SIZE	StringTableOffs)
);
PROC_GLOBAL(
COUNT GetStringLength, (String),
    ARG_END	(STRING	String)
);
PROC_GLOBAL(
STRINGPTR GetStringAddress, (String),
    ARG_END	(STRING	String)
);
PROC_GLOBAL(
BOOLEAN GetStringContents, (String, StringBuf, AppendSpace),
    ARG		(STRING		String)
    ARG		(STRINGPTR	StringBuf)
    ARG_END	(BOOLEAN	AppendSpace)
);

#endif /* _STRLIB_H */

