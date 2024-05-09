#include "sndintrn.h"
#include "timlib.h"

TRACK_DESC	VOLATILE _TrackList[NUM_TRACK_TYPES];

PROC(
BOOLEAN _download_instruments, (MusicRef),
    ARG_END	(MUSIC_REF	MusicRef)
)
{
    BOOLEAN		retval;
    SONG_DESCPTR	SongPtr;

    if (MusicRef == 0)
	SongPtr = (SONG_DESCPTR)_TrackList[MOD_TRACK].TrackPtr;
    else
    {
	LockMODData (MusicRef, &SongPtr);
    }

    if (retval = !_is_red_book ((LPBYTE)SongPtr))
    {
	COUNT		i;
	LPBYTE		lpSnd;
	BLOCK_DESCPTR	BlockPtr;

	BlockPtr = (BLOCK_DESCPTR)&SongPtr->BlockList[SongPtr->NumPhysicalBlocks];
	for (i = 0; i < SongPtr->NumPhysicalBlocks; ++i)
	{
	    FAR_PTR_ADD (&BlockPtr, 0);
	    SongPtr->BlockList[i] = (DWORD)BlockPtr;

	    BlockPtr = (BLOCK_DESCPTR)&BlockPtr->LineList[BlockPtr->LastLine + 1];
	}

	lpSnd = (LPBYTE)BlockPtr;
	FAR_PTR_ADD (&lpSnd, 0);
	for (i = 0; i < SongPtr->NumInstruments; ++i)
	{
	    COUNT	SampleLength;

	    if (SampleLength = SongPtr->PresetList[i].SampleLength)
	    {
		if (MusicRef == 0)
		    SongPtr->InstrumentList[i] = (DWORD)lpSnd;
		else if (!FreeHardwareSample (lpSnd, SampleLength))
		    AllocHardwareSample (
			    lpSnd, 8363L, SampleLength,
			    SongPtr->PresetList[i].LoopBegin << 1,
			    SongPtr->PresetList[i].LoopLength << 1
			    );

		FAR_PTR_ADD (&lpSnd, SampleLength + 1);
	    }
	}

#ifdef DEBUG
if (MusicRef == 0)
{
    printf ("TEMPO: %u\n", SongPtr->Tempo);
    printf ("NUMSTEPS: %u\n", SongPtr->NumSteps);
    printf ("INSTRUMENTS:\n");
    for (i = 0; i < SongPtr->NumInstruments; ++i)
    {
	printf ("\t%d(%d))    %u, %u, %u, %u = 0x%08lx\n",
		i, ((LPBYTE)SongPtr->InstrumentList[i])[SongPtr->PresetList[i].SampleLength],
		SongPtr->PresetList[i].volume,
		SongPtr->PresetList[i].LoopBegin,
		SongPtr->PresetList[i].LoopLength,
		SongPtr->PresetList[i].SampleLength,
		SongPtr->InstrumentList[i]);
    }
    printf ("BLOCKS:\n");
    printf ("\tPhysical Blocks = %u\n", SongPtr->NumPhysicalBlocks);
    printf ("\tLogical Blocks = %u\n", SongPtr->NumLogicalBlocks);
    for (i = 0; i < SongPtr->NumLogicalBlocks; ++i)
    {
	UWORD		b, j;
	LINE_DESCPTR	LinePtr;

	b = SongPtr->LogToPhysBlockList[i];
	printf ("\t\tLogical Block %u ---> Physical Block %u", i, b);
	BlockPtr = (BLOCK_DESCPTR)SongPtr->BlockList[b];
	LinePtr = BlockPtr->LineList;
	for (j = 0; j <= BlockPtr->LastLine; ++j)
	{
	    UWORD	k;

	    printf ("\n\t");
	    for (k = 0; k < BlockPtr->NumTracks; ++k)
	    {
		UWORD	extra;

		extra = MAKE_WORD (
			LinePtr->LineCommands[k][2],
			LinePtr->LineCommands[k][1]
			);
		printf ("%02x %04x    ",
			LinePtr->LineCommands[k][0], extra);
	    }
	    ++LinePtr;
	}
	printf ("\n");
    }
}
#endif /* DEBUG */
    }

    UnlockMODData (MusicRef);
    return (retval);
}

static TASK	PLR_task;

PROC(
void PLRPlaySong, (MusicRef, Continuous, Priority),
    ARG		(MUSIC_REF	MusicRef)
    ARG		(BOOLEAN	Continuous)
    ARG_END	(BYTE		Priority)
)
{
    SONG_DESCPTR	SongPtr;

    LockMODData (MusicRef, &SongPtr);
    if (SongPtr)
    {
	if (_is_red_book ((LPBYTE)SongPtr))
	{
	    PLRStop (_TrackList[RED_BOOK_TRACK].TrackRef);

	    _TrackList[RED_BOOK_TRACK].TrackPtr = (LPBYTE)SongPtr;
	    _TrackList[RED_BOOK_TRACK].TrackRef = MusicRef;
	    _play_red_book ((LPBYTE)SongPtr, Continuous);
	}
	else
	{
	    PROC_GLOBAL(
	    void _MEDPlayer, (),
		ARG_VOID
	    );

	    PLRStop (_TrackList[MOD_TRACK].TrackRef);

	    _TrackList[MOD_TRACK].TrackPtr = (LPBYTE)SongPtr;
	    _TrackList[MOD_TRACK].TrackRef = MusicRef;

	    _download_instruments ((MUSIC_REF)0);

	    _set_sound_interrupt ();
	    SongPtr->Priority = Priority;
	    if (Continuous)
		SongPtr->Flags |= PLAY_CONTINUOUS;
	    if ((PLR_task = AddTask (_MEDPlayer, 1024)) == 0)
		PLRStop (_TrackList[MOD_TRACK].TrackRef);
	}
    }
}

PROC(
void PLRStop, (MusicRef),
    ARG_END	(MUSIC_REF	MusicRef)
)
{
    COUNT	i;

    SuspendTasking ();

    for (i = 0; i < NUM_TRACK_TYPES; ++i)
    {
	if (_TrackList[i].TrackPtr
		&& (MusicRef == _TrackList[i].TrackRef
		|| MusicRef == (MUSIC_REF)~0))
	{
	    TASK	T;
	    MUSIC_REF	MRef;

	    MRef = _TrackList[i].TrackRef;
	    _TrackList[i].TrackPtr = 0;
	    _TrackList[i].TrackRef = 0;

	    if (i == RED_BOOK_TRACK)
	    {
		_stop_red_book ();
		T = 0;
	    }
	    else
	    {
		_clr_sound_interrupt ();

		for (i = 0; i < MAX_CHANNELS; ++i)
		{
		    StopChannel (i + MAX_CHANNELS, 0xFE);
		}

		do
		{
		    TaskSwitch ();
		    for (i = 0; i < MAX_CHANNELS; ++i)
		    {
			if (ChannelPlaying (i + MAX_CHANNELS))
		    	break;
		    }
		} while (i < MAX_CHANNELS);

		T = PLR_task;
		PLR_task = 0;
	    }

	    UnlockMODData (MRef);
	    if (T)
	    {
		ResumeTasking ();
		DeleteTask (T);
		SuspendTasking ();
	    }
	}
    }

    ResumeTasking ();
}

PROC(
BOOLEAN PLRPlaying, (MusicRef),
    ARG_END	(MUSIC_REF	MusicRef)
)
{
    COUNT	i;

    for (i = 0; i < NUM_TRACK_TYPES; ++i)
    {
	if (_TrackList[i].TrackPtr)
	{
	    if (i == RED_BOOK_TRACK
		    && !_red_book_playing ())
		PLRStop (_TrackList[i].TrackRef);
	    else if (MusicRef == _TrackList[i].TrackRef
		    || MusicRef == (MUSIC_REF)~0)
		return (TRUE);
	}
    }

    return (FALSE);
}

PROC(
void PLRPause, (MusicRef),
    ARG_END	(MUSIC_REF	MusicRef)
)
{
    COUNT	i;

    for (i = 0; i < NUM_TRACK_TYPES; ++i)
    {
	if (_TrackList[i].TrackPtr
		&& (MusicRef == _TrackList[i].TrackRef
		|| MusicRef == (MUSIC_REF)~0))
	{
	    if (i == MOD_TRACK)
		FadeSound (0, ONE_SECOND);
	    else if (_red_book_playing ())
		_pause_red_book ();
	    else
		PLRStop (_TrackList[i].TrackRef);
	}
    }
}

PROC(
void PLRResume, (MusicRef),
    ARG_END	(MUSIC_REF	MusicRef)
)
{
    COUNT	i;

    for (i = 0; i < NUM_TRACK_TYPES; ++i)
    {
	if (_TrackList[i].TrackPtr
		&& (MusicRef == _TrackList[i].TrackRef
		|| MusicRef == (MUSIC_REF)~0))
	{
	    if (i == MOD_TRACK)
		FadeSound (128, ONE_SECOND);
	    else if (_red_book_playing ())
		_resume_red_book ();
	    else
		PLRStop (_TrackList[i].TrackRef);
	}
    }
}

PROC(
BOOLEAN DestroyMusic, (MusicRef),
    ARG_END	(MUSIC_REF	MusicRef)
)
{
    if (MusicRef && _download_instruments (MusicRef))
	FreeHardwareSample (0, 0);

    return (FreeMODData (MusicRef));
}

