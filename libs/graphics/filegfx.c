#include "gfxintrn.h"

PROC(
DWORD LoadGraphicFile, (pStr),
    ARG_END	(PVOID		pStr)
)
{
    FILE	*fp;

    if ((fp = OpenResFile (pStr, "rb", NULL_PTR, 0L, NULL_PTR)) != NULL)
    {
	MEM_HANDLE	hData;

	hData = _GetGraphicData (fp, LengthResFile (fp));
	CloseResFile (fp);
	return ((DWORD)hData);
    }	

    return (0);
}

