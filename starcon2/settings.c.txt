#include "starcon.h"

static MUSIC_REF	LastMusicRef;
static BOOLEAN		LastContinuous;
static BYTE		LastPriority;

PROC(
void ToggleMusic, (),
    ARG_VOID
)
{
    GLOBAL (glob_flags) ^= MUSIC_DISABLED;
    if (LastPriority <= 1)
    {
	if (GLOBAL (glob_flags) & MUSIC_DISABLED)
	    PLRStop (LastMusicRef);
	else if (LastMusicRef)
	    PLRPlaySong (LastMusicRef, LastContinuous, LastPriority);
    }
}

PROC(
void PlayMusic, (MusicRef, Continuous, Priority),
    ARG		(MUSIC_REF	MusicRef)
    ARG		(BOOLEAN	Continuous)
    ARG_END	(BYTE		Priority)
)
{
    LastMusicRef = MusicRef;
    LastContinuous = Continuous;
    LastPriority = Priority;

    if (
#ifdef NEVER
	Priority > 1
	||
#endif /* NEVER */
	!(GLOBAL (glob_flags) & MUSIC_DISABLED)
	)
    {
	PLRPlaySong (MusicRef, Continuous, Priority);
    }
}

PROC(
void StopMusic, (),
    ARG_VOID
)
{
     PLRStop (LastMusicRef);
     LastMusicRef = 0;
}

PROC(
void ResumeMusic, (),
    ARG_VOID
)
{
    PLRResume (LastMusicRef);
}

PROC(
void PauseMusic, (),
    ARG_VOID
)
{
    PLRPause (LastMusicRef);
}

PROC(
void ToggleSoundEffect, (),
    ARG_VOID
)
{
    GLOBAL (glob_flags) ^= SOUND_DISABLED;
}

PROC(
void PlaySoundEffect, (S, Channel, Priority),
    ARG		(SOUND	S)
    ARG		(COUNT	Channel)
    ARG_END	(BYTE	Priority)
)
{
    if (!(GLOBAL (glob_flags) & SOUND_DISABLED))
    {
	SetChannelVolume (Channel, MAX_VOLUME >> 1, Priority);
	SetChannelRate (Channel, GetSampleRate (S), Priority);
	PlayChannel (Channel,
		GetSampleAddress (S),
		GetSampleLength (S),
		0, 0, Priority);
    }
}

