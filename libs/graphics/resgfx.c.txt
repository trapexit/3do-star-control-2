#include "gfxintrn.h"

STATIC COUNT	_gfx_type;

PROC(
BOOLEAN InstallGraphicResType, (gfx_type),
    ARG_END	(COUNT	gfx_type)
)
{
    _gfx_type = gfx_type;
    InstallResTypeVectors (gfx_type, _GetGraphicData, mem_release);
    return (TRUE);
}

PROC(
DWORD LoadGraphicInstance, (res),
    ARG_END	(DWORD	res)
)
{
    MEM_HANDLE	hData;

    if (hData = GetResource (res))
	DetachResource (res);

    return ((DWORD)hData);
}

PROC(
DWORD LoadGraphic, (res),
    ARG_END	(DWORD	res)
)
{
    if (_gfx_type)
	return (LoadGraphicInstance (res));
    else
	return (LoadGraphicFile ((PVOID)res));
}

