#ifndef _PLAY_H
#define _PLAY_H

enum
{
    MOD_TRACK,
    RED_BOOK_TRACK,

    NUM_TRACK_TYPES
};

typedef struct
{
    MUSIC_REF	TrackRef;
    LPBYTE	TrackPtr;
} TRACK_DESC;
extern TRACK_DESC	VOLATILE _TrackList[NUM_TRACK_TYPES];

#define MAX_CHANNELS		4
#define MAX_TRACKS		16
#define MAX_INSTRUMENTS		63
#define MAX_BLOCKS		256
#define MAX_TRACK_VOLUME	64

typedef unsigned short		SAMPLE_RATE;

typedef struct
{
    COUNT	LoopBegin, LoopLength;
    COUNT	SampleLength;
    BYTE	volume, transposition;
} INSTRUMENT_DESC;

typedef struct
{
    BYTE	LineCommands[MAX_CHANNELS][3];
} LINE_DESC;
typedef LINE_DESC	*PLINE_DESC;
typedef LINE_DESC	near *NPLINE_DESC;
typedef LINE_DESC	far *LPLINE_DESC;

#define LINE_DESCPTR	LPLINE_DESC

typedef struct
{
    BYTE	NumTracks, LastLine;
    LINE_DESC	LineList[1];
} BLOCK_DESC;
typedef BLOCK_DESC	*PBLOCK_DESC;
typedef BLOCK_DESC	near *NPBLOCK_DESC;
typedef BLOCK_DESC	far *LPBLOCK_DESC;

#define BLOCK_DESCPTR	LPBLOCK_DESC

#define PLAY_CONTINUOUS	(1 << 0)

typedef struct
{
    INSTRUMENT_DESC	PresetList[MAX_INSTRUMENTS];
    BYTE		NumPhysicalBlocks, NumLogicalBlocks;
    BYTE		LogToPhysBlockList[MAX_BLOCKS];
    BYTE		Tempo, NumSteps;
    BYTE		Priority, Flags;
    BYTE		TrackVolumeList[MAX_TRACKS];
    BYTE		MasterVolume;
    BYTE		NumInstruments;

    DWORD		InstrumentList[MAX_INSTRUMENTS];
    DWORD		BlockList[1];
} SONG_DESC;
typedef SONG_DESC	*PSONG_DESC;
typedef SONG_DESC	near *NPSONG_DESC;
typedef SONG_DESC	far *LPSONG_DESC;

#define SONG_DESCPTR	LPSONG_DESC

PROC_GLOBAL(
void _download_effects, (SoundRef),
    ARG_END	(SOUND_REF	SoundRef)
);
PROC_GLOBAL(
BOOLEAN _download_instruments, (MusicRef),
    ARG_END	(MUSIC_REF	MusicRef)
);

#include "redbook.h"

#endif /* _PLAY_H */

