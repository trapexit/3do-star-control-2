#include "strintrn.h"

PROC(
BOOLEAN InstallStringTableResType, (string_type),
    ARG_END	(COUNT	string_type)
)
{
    return (InstallResTypeVectors (string_type,
	    _GetStringData, FreeResourceData));
}

PROC(
STRING_TABLE LoadStringTableInstance, (res),
    ARG_END	(DWORD	res)
)
{
    MEM_HANDLE	hData;

    if (hData = GetResource (res))
    {
	DetachResource (res);
    }

    return (BUILD_STRING_TABLE (hData));
}

