#include "strintrn.h"

PROC(
BOOLEAN DestroyStringTable, (StringTable),
    ARG_END	(STRING_TABLE	StringTable)
)
{
    return (FreeStringTable (StringTable));
}

PROC(
STRING CaptureStringTable, (StringTable),
    ARG_END	(STRING_TABLE	StringTable)
)
{
    if (StringTable != 0)
    {
	COUNT		StringTableIndex;
	STRING_TABLEPTR	StringTablePtr;

	LockStringTable (StringTable, &StringTablePtr);
	StringTableIndex = GetStringTableIndex (StringTable);
	return (BUILD_STRING (StringTable, StringTableIndex));
    }

    return ((STRING)NULL_PTR);
}

PROC(
STRING_TABLE ReleaseStringTable, (String),
    ARG_END	(STRING	String)
)
{
    STRING_TABLE	StringTable;

    if ((StringTable = GetStringTable (String)) != 0)
	UnlockStringTable (StringTable);

    return (StringTable);
}

PROC(
STRING_TABLE GetStringTable, (String),
    ARG_END	(STRING	String)
)
{
    return ((STRING_TABLE)LOWORD (String));
}

PROC(
COUNT GetStringTableCount, (String),
    ARG_END	(STRING	String)
)
{
    COUNT		StringCount;
    STRING_TABLE	StringTable;

    if ((StringTable = GetStringTable (String)) == 0)
	StringCount = 0;
    else
    {
	STRING_TABLEPTR	StringTablePtr;

	LockStringTable (StringTable, &StringTablePtr);
	StringCount = StringTablePtr->StringCount;
	UnlockStringTable (StringTable);
    }

    return (StringCount);
}

PROC(
COUNT GetStringTableIndex, (String),
    ARG_END	(STRING	String)
)
{
    return (STRING_INDEX (String));
}

PROC(
STRING SetAbsStringTableIndex, (String, StringTableIndex),
    ARG		(STRING	String)
    ARG_END	(COUNT	StringTableIndex)
)
{
    STRING_TABLE	StringTable;

    if ((StringTable = GetStringTable (String)) == 0)
	String = 0;
    else
    {
	STRING_TABLEPTR	StringTablePtr;

	LockStringTable (StringTable, &StringTablePtr);
	StringTableIndex = StringTableIndex % StringTablePtr->StringCount;
	UnlockStringTable (StringTable);

	String = BUILD_STRING (StringTable, StringTableIndex);
    }

    return (String);
}

PROC(
STRING SetRelStringTableIndex, (String, StringTableOffs),
    ARG		(STRING	String)
    ARG_END	(SIZE	StringTableOffs)
)
{
    STRING_TABLE	StringTable;

    if ((StringTable = GetStringTable (String)) == 0)
	String = 0;
    else
    {
	STRING_TABLEPTR	StringTablePtr;
	COUNT		StringTableIndex;

	LockStringTable (StringTable, &StringTablePtr);
	while (StringTableOffs < 0)
	    StringTableOffs += StringTablePtr->StringCount;
	StringTableIndex = (STRING_INDEX (String) + StringTableOffs)
		% StringTablePtr->StringCount;
	UnlockStringTable (StringTable);

	String = BUILD_STRING (StringTable, StringTableIndex);
    }

    return (String);
}

PROC(
COUNT GetStringLength, (String),
    ARG_END	(STRING	String)
)
{
    COUNT		StringLength;
    STRING_TABLE	StringTable;

    if ((StringTable = GetStringTable (String)) == 0)
	StringLength = 0;
    else
    {
	COUNT		StringIndex;
	STRING_TABLEPTR	StringTablePtr;

	StringIndex = STRING_INDEX (String);
	LockStringTable (StringTable, &StringTablePtr);
	StringLength = (COUNT)(
		StringTablePtr->StringOffsets[StringIndex + 1]
		- StringTablePtr->StringOffsets[StringIndex]
		);
	UnlockStringTable (StringTable);
    }

    return (StringLength);
}

PROC(
STRINGPTR GetStringAddress, (String),
    ARG_END	(STRING	String)
)
{
    STRINGPTR		StringAddr;
    STRING_TABLE	StringTable;

    if ((StringTable = GetStringTable (String)) == 0)
	StringAddr = 0;
    else
    {
	COUNT		StringIndex;
	STRING_TABLEPTR	StringTablePtr;

	StringIndex = STRING_INDEX (String);
	LockStringTable (StringTable, &StringTablePtr);
	StringAddr = (STRINGPTR)StringTablePtr;
	FAR_PTR_ADD (&StringAddr, StringTablePtr->StringOffsets[StringIndex]);
	UnlockStringTable (StringTable);
    }

    return (StringAddr);
}

PROC(
BOOLEAN GetStringContents, (String, StringBuf, AppendSpace),
    ARG		(STRING		String)
    ARG		(STRINGPTR	StringBuf)
    ARG_END	(BOOLEAN	AppendSpace)
)
{
    STRINGPTR	StringAddr;
    COUNT	StringLength;

    if ((StringAddr = GetStringAddress (String)) != 0 &&
	    (StringLength = GetStringLength (String)) != 0)
    {
	MEMCPY (StringBuf, StringAddr, StringLength);
	if (AppendSpace)
	    StringBuf[StringLength++] = ' ';
	StringBuf[StringLength] = '\0';

	return (TRUE);
    }

    *StringBuf = '\0';
    return (FALSE);
}

