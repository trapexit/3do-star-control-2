#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "sndintrn.h"
#include "timlib.h"
#include "play.h"

PROC(
COUNT GetSampleRate, (Sound),
    ARG_END	(SOUND	Sound)
)
{
    SOUNDPTR	SPtr;
    COUNT	len;

    if (Sound == 0)
	return (0);

    SPtr = GetSoundAddress (Sound);
    len = GetSoundLength (Sound) - sizeof (SAMPLE_RATE);

    return (MAKE_WORD (SPtr[len], SPtr[len + 1]));
}

PROC(
COUNT GetSampleLength, (Sound),
    ARG_END	(SOUND	Sound)
)
{
    if (Sound == 0)
	return (0);

    return (GetSoundLength (Sound) - (sizeof (BYTE) + sizeof (SAMPLE_RATE)));
}

PROC(
LPBYTE GetSampleAddress, (Sound),
    ARG_END	(SOUND	Sound)
)
{
    return ((LPBYTE)GetSoundAddress (Sound));
}

PROC(
void _download_effects, (SoundRef),
    ARG_END	(SOUND_REF	SoundRef)
)
{
    COUNT	i;
    SOUND	S;

    S = CaptureSound (SoundRef);
    for (i = GetSoundCount (S); i > 0; --i, S = SetRelSoundIndex (S, 1))
    {
	LPBYTE	lpSnd;
	COUNT	SampleLength;

	lpSnd = GetSampleAddress (S);
	SampleLength = GetSampleLength (S);
	AllocHardwareSample (
		lpSnd,
		GetSampleRate (S),
		SampleLength, 0, 0
		);
    }
    ReleaseSound (S);
}

PROC(
BOOLEAN DestroySound, (SoundRef),
    ARG_END	(SOUND_REF	SoundRef)
)
{
    COUNT	i;
    SOUND	S;

    S = CaptureSound (SoundRef);
    for (i = GetSoundCount (S); i > 0; --i)
    {
	COUNT	SampleLength;
	LPBYTE	lpSnd;

	S = SetRelSoundIndex (S, -1);

	SampleLength = GetSampleLength (S);
	lpSnd = GetSampleAddress (S);
	FreeHardwareSample (lpSnd, SampleLength);
    }
    ReleaseSound (S);

    FreeHardwareSample (0, 0);
    return (DestroyStringTable (SoundRef));
}

static TASK	FadeTask;
static SIZE	TTotal;
static SIZE	volume_end;

static void fade_task ()
{
    SIZE		TDelta, volume_beg;
    DWORD		StartTime, CurTime;
    extern COUNT	_master_volume;

    volume_beg = _master_volume;
    StartTime = CurTime = GetTimeCounter ();
    do
    {
	CurTime = SleepTask (CurTime + 1);
	if ((TDelta = (SIZE)(CurTime - StartTime)) > TTotal)
	    TDelta = TTotal;

	SetMasterVolume ((COUNT)(volume_beg
		+ (SIZE)((long)(volume_end - volume_beg) * TDelta / TTotal)));
    } while (TDelta < TTotal);

    {
	TASK	T;

	T = FadeTask;
	FadeTask = 0;
	DeleteTask (T);
    }
}

PROC(STATIC
void CancelFade, (),
    ARG_VOID
)
{
    extern COUNT	_master_volume;
    
    volume_end = _master_volume;
    TTotal = 1;
    do
	TaskSwitch ();
    while (FadeTask);
    TaskSwitch ();
}

PROC(
DWORD FadeSound, (end_vol, TimeInterval),
    ARG		(BYTE	end_vol)
    ARG_END	(SIZE	TimeInterval)
)
{
    DWORD	TimeOut;

    if (FadeTask)
	CancelFade ();

    if ((TTotal = TimeInterval) <= 0)
	TTotal = 1;	/* prevent divide by zero and negative fade */
    volume_end = end_vol;
	
    if (TTotal > 1 && (FadeTask = AddTask (fade_task, 2048)))
    {
	TimeOut = GetTimeCounter () + TTotal + 1;
    }
    else
    {
	SetMasterVolume (end_vol);
	TimeOut = GetTimeCounter ();
    }

    return (TimeOut);
}
