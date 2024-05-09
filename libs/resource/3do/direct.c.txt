#include "../strings/strintrn.h"

PROC(
DIRENTRY_REF LoadDirEntryTable, (pattern, pnum_entries),
    ARG		(void *		pattern)
    ARG_END	(PCOUNT		pnum_entries)
)
{
    COUNT		num_entries, length;
    STRING_TABLE	StringTable;
    STRING_TABLEPTR	lpST;
    LPDWORD		lpLastOffs;

    num_entries = 0;
    StringTable = 0;
    do
    {
	COUNT	slen;
	int	dir;
	char	name[80];

	if (num_entries == 0)
	    length = 0;
	else
	{
	    slen = sizeof (STRING_TABLE_DESC)
		    + (num_entries * sizeof (DWORD));
	    StringTable = AllocResourceData (slen + length, 0);
	    LockStringTable (StringTable, &lpST);
	    if (lpST == 0)
	    {
		FreeStringTable (StringTable);
		StringTable = 0;

		break;
	    }

	    lpST->StringCount = num_entries;
	    lpLastOffs = &lpST->StringOffsets[0];
	    *lpLastOffs = slen;

	    num_entries = 0;
	    length = slen;
	}

      if (dir = open_directory ())
      {
	while (!read_directory (dir, pattern, name))
	{
	    slen = strlen (name);
	    length += slen;

	    if ((MEM_HANDLE)StringTable)
	    {
		LPSTR	lpStr;
		LPDWORD	lpLo, lpHi;

		strupr (name);

		lpLo = &lpST->StringOffsets[0];
		lpHi = lpLastOffs - 1;
		while (lpLo <= lpHi)
		{
		    char	c1, c2;
		    char	*	pStr;
		    COUNT	LocLen;
		    LPDWORD	lpMid;

		    lpMid = lpLo + ((lpHi - lpLo) >> 1);

		    LocLen = lpMid[1] - lpMid[0];
		    if (LocLen > slen)
			LocLen = slen;

		    lpStr = (LPSTR)lpST + lpMid[0];
		    pStr = name;
		    while (LocLen-- && (c1 = *lpStr++) == (c2 = *pStr++))
			;

		    if (c1 <= c2)
			lpLo = lpMid + 1;
		    else
			lpHi = lpMid - 1;
		}

		lpStr = (LPSTR)lpST + lpLo[0];
		MEMMOVE (lpStr + slen, lpStr, lpLastOffs[0] - lpLo[0]);
		MEMCPY (lpStr, name, slen);

		for (lpHi = lpLastOffs++; lpHi >= lpLo; --lpHi)
		    lpHi[1] = lpHi[0] + slen;
	    }

	    ++num_entries;
	}
	
	close_directory (dir);
      }
    } while (num_entries && (MEM_HANDLE)StringTable == 0);

    if ((MEM_HANDLE)StringTable == 0)
	*pnum_entries = 0;
    else
    {
	*pnum_entries = num_entries;

	UnlockStringTable (StringTable);
    }

    return ((DIRENTRY_REF)StringTable);
}
