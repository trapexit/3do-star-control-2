#include "sndintrn.h"

PROC(
SOUND_REF LoadSoundFile, (pStr),
    ARG_END	(PVOID	pStr)
)
{
    SOUND_REF	SoundRef;

    if (SoundRef = LoadStringTableFile (pStr))
	_download_effects (SoundRef);

    return (SoundRef);
}

PROC(
MUSIC_REF LoadMusicFile, (pStr),
    ARG_END	(PVOID	pStr)
)
{
    FILE	*fp;

    if (fp = OpenResFile (pStr, "rb", NULL_PTR, 0L, NULL_PTR))
    {
	MEM_HANDLE	hData;

	if (hData = _GetMODData (fp, LengthResFile (fp)))
	    _download_instruments (hData);

	CloseResFile (fp);

	return ((MUSIC_REF)hData);
    }

    return (0);
}

