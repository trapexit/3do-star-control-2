#include "strintrn.h"

PROC(
MEM_HANDLE _GetStringData, (fp, length),
    ARG		(FILE		*fp)
    ARG_END	(DWORD		length)
)
{
    MEM_HANDLE	hData;

    if (hData = GetResourceData (fp, length, MEM_SOUND))
    {
	COUNT		StringCount;
	DWORD		StringOffs;
	LPDWORD		lpStringOffs;
	STRING_TABLEPTR	lpST;

	LockStringTable (hData, &lpST);
	StringCount = lpST->StringCount;

	lpStringOffs = lpST->StringOffsets;
	StringOffs = sizeof (STRING_TABLE_DESC)
		+ (sizeof (DWORD) * StringCount);
	do
	{
	    StringOffs += *lpStringOffs;
	    *lpStringOffs++ = StringOffs;
	} while (StringCount--);

	UnlockStringTable (hData);
    }

    return (hData);
}


