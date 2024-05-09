#include "strintrn.h"

PROC(
STRING_TABLE LoadStringTableFile, (pStr),
    ARG_END	(PVOID	pStr)
)
{
    FILE	*fp;

    if (fp = OpenResFile (pStr, "rb", NULL_PTR, 0L, NULL_PTR))
    {
	MEM_HANDLE	hData;

	hData = _GetStringData (fp, LengthResFile (fp));
	CloseResFile (fp);

	return (BUILD_STRING_TABLE (hData));
    }	

    return (0);
}

