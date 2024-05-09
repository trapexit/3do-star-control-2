#ifndef _SNDLIB_H
#define _SNDLIB_H

#include "strlib.h"
#include "memlib.h"

typedef STRING_TABLE	SOUND_REF;
typedef STRING		SOUND;
typedef STRINGPTR	SOUNDPTR;

#define InitSoundResources	InitStringTableResources
#define CaptureSound		CaptureStringTable
#define ReleaseSound		ReleaseStringTable
#define GetSoundRef		GetStringTable
#define GetSoundCount		GetStringTableCount
#define GetSoundIndex		GetStringTableIndex
#define SetAbsSoundIndex	SetAbsStringTableIndex
#define SetRelSoundIndex	SetRelStringTableIndex
#define GetSoundLength		GetStringLength
#define GetSoundAddress		GetStringAddress
#define GetSoundContents	GetStringContents

typedef MEM_HANDLE	MUSIC_REF;

PROC_GLOBAL(
BOOLEAN InitSound, (argc, argv),
    ARG		(int	argc)
    ARG_END	(char	*argv[])
);
PROC_GLOBAL(
void UninitSound, (),
    ARG_VOID
);
PROC_GLOBAL(
SOUND_REF LoadSoundFile, (pStr),
    ARG_END	(PVOID	pStr)
);
PROC_GLOBAL(
MUSIC_REF LoadMusicFile, (pStr),
    ARG_END	(PVOID	pStr)
);
PROC_GLOBAL(
BOOLEAN InstallMusicResType, (music_type),
    ARG_END	(COUNT	music_type)
);
PROC_GLOBAL(
SOUND_REF LoadSoundInstance, (res),
    ARG_END	(DWORD	res)
);
PROC_GLOBAL(
MUSIC_REF LoadMusicInstance, (res),
    ARG_END	(DWORD	res)
);
PROC_GLOBAL(
BOOLEAN DestroySound, (SoundRef),
    ARG_END	(SOUND_REF	SoundRef)
);
PROC_GLOBAL(
BOOLEAN DestroyMusic, (MusicRef),
    ARG_END	(MUSIC_REF	MusicRef)
);

#define MAX_CHANNELS		4
#define MAX_VOLUME		255

PROC_GLOBAL(
void PLRPlaySong, (MusicRef, Continuous, Priority),
    ARG		(MUSIC_REF	MusicRef)
    ARG		(BOOLEAN	Continuous)
    ARG_END	(BYTE		Priority)
);
PROC_GLOBAL(
void PLRStop, (MusicRef),
    ARG_END	(MUSIC_REF	MusicRef)
);
PROC_GLOBAL(
BOOLEAN PLRPlaying, (MusicRef),
    ARG_END	(MUSIC_REF	MusicRef)
);
PROC_GLOBAL(
void PLRPause, (MusicRef),
    ARG_END	(MUSIC_REF	MusicRef)
);
PROC_GLOBAL(
void PLRResume, (MusicRef),
    ARG_END	(MUSIC_REF	MusicRef)
);
PROC_GLOBAL(
void PlayChannel, (Channel, lpSample, SampLen, LoopBegin, LoopLen, Priority),
    ARG		(COUNT	Channel)
    ARG		(LPVOID	lpSample)
    ARG		(COUNT	SampLen)
    ARG		(COUNT	LoopBegin)
    ARG		(COUNT	LoopLen)
    ARG_END	(BYTE	Priority)
);
PROC_GLOBAL(
BOOLEAN ChannelPlaying, (Channel),
    ARG_END	(COUNT	Channel)
);
PROC_GLOBAL(
void StopChannel, (Channel, Priority),
    ARG		(COUNT	Channel)
    ARG_END	(BYTE	Priority)
);
PROC_GLOBAL(
void SetMasterVolume, (Volume),
    ARG_END	(COUNT	Volume)
);
PROC_GLOBAL(
void SetChannelVolume, (Channel, Volume, Priority),
    ARG		(COUNT	Channel)
    ARG		(COUNT	Volume)
    ARG_END	(BYTE	Priority)
);
PROC_GLOBAL(
void SetChannelRate, (Channel, Rate, Priority),
    ARG		(COUNT	Channel)
    ARG		(DWORD	Rate)
    ARG_END	(BYTE	Priority)
);

PROC_GLOBAL(
void StopSound, (),
    ARG_VOID
);
PROC_GLOBAL(
BOOLEAN SoundPlaying, (),
    ARG_VOID
);

PROC_GLOBAL(
BOOLEAN AllocHardwareSample, (lpSnd, SampleRate, SampleLength, LoopBegin, LoopLen),
    ARG		(LPBYTE	lpSnd)
    ARG		(DWORD	SampleRate)
    ARG		(COUNT	SampleLength)
    ARG		(COUNT	LoopBegin)
    ARG_END	(COUNT	LoopLen)
);
PROC_GLOBAL(
BOOLEAN FreeHardwareSample, (lpSnd, SampleLength),
    ARG		(LPBYTE	lpSnd)
    ARG_END	(COUNT	SampleLength)
);

PROC_GLOBAL(
COUNT GetSampleRate, (Sound),
    ARG_END	(SOUND	Sound)
);
PROC_GLOBAL(
COUNT GetSampleLength, (Sound),
    ARG_END	(SOUND	Sound)
);
PROC_GLOBAL(
LPBYTE GetSampleAddress, (Sound),
    ARG_END	(SOUND	Sound)
);
PROC_GLOBAL(
DWORD FadeSound, (end_vol, TimeInterval),
    ARG		(BYTE	end_vol)
    ARG_END	(SIZE	TimeInterval)
);

PROC_GLOBAL(
void SetSoundTracking, (toggle),
    ARG_END	(BOOLEAN	toggle)
);
PROC_GLOBAL(
void GetSoundTrackingBuf, (pdigi_buf),
    ARG_END	(PSBYTE		pdigi_buf)
);

#endif /* _SNDLIB_H */

