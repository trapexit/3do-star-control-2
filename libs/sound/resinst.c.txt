#include "sndintrn.h"

PROC(
BOOLEAN InstallMusicResType, (music_type),
    ARG_END	(COUNT	music_type)
)
{
    return (InstallResTypeVectors (music_type,
	    _GetMODData, FreeResourceData));
}

PROC(
SOUND_REF LoadSoundInstance, (res),
    ARG_END	(DWORD	res)
)
{
    SOUND_REF	SoundRef;

    if (SoundRef = LoadStringTableInstance (res))
	_download_effects (SoundRef);

    return (SoundRef);
}

PROC(
MUSIC_REF LoadMusicInstance, (res),
    ARG_END	(DWORD	res)
)
{
    MEM_HANDLE	hData;

    if (hData = GetResource (res))
    {
	DetachResource (res);

	_download_instruments (hData);
    }

    return ((MUSIC_REF)hData);
}

