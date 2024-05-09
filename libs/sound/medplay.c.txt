#include <string.h>
#include "sndintrn.h"
#include "timlib.h"

typedef struct
{
    BYTE	EffectArgs, Effect;
    BYTE	TrackInstrument;
    BYTE	Control;
    SBYTE	TrackVolume;
    BYTE	PeriodIndex;
    UWORD	TrackPeriod;
    UWORD	Depth;
} CHANNEL_STATE;

PROC(
void _MEDPlayer, (),
    ARG_VOID
)
{
    DWORD		NextTime;
    SWORD		CurTempo, TempoError, TempoFract, TempoBump;
#define AMIGA_TO_SPS(r)	(3579546L / (r))
    BYTE		CurTrack, Priority;
    UWORD		counter;
    UWORD		NumSteps;
    CHANNEL_STATE	ChannelInfo[MAX_CHANNELS];
    UWORD		BlockLine, PhysicalBlock, LogicalBlock;
    SONG_DESCPTR	CurSongPtr;
    BLOCK_DESCPTR	CurBlockPtr;
    LINE_DESCPTR	CurLinePtr;
    UWORD		PeriodTab[] =
    {
	856, 808, 762, 720, 678, 640, 604, 570,
	538, 508, 480, 453, 428, 404, 381, 360,
	339, 320, 302, 285, 269, 254, 240, 226,
	214, 202, 190, 180, 170, 160, 151, 143,
	135, 127, 120, 113, 107, 101,  95,  90,
	 85,  80,  75,  72,  68,  64,  60,  57,

	 	/* ExtraLoPeriodTab */
	3424, 3232, 3040, 2880, 2720, 2560,
	2416, 2288, 2160, 2032, 1920, 1808,
	1712, 1616, 1520, 1440, 1360, 1280,
	1208, 1144, 1080, 1016,  960,  904,

	 	/* ExtraHiPeriodTab */
	53, 50, 48, 45, 42, 40,
	38, 36, 34, 32, 30, 28,
	27, 25, 24, 22, 21, 20,
	19, 18, 17, 16, 15, 14,
    };
#define SIN_SHIFT	5
    SBYTE	SinTab[1 << SIN_SHIFT] =
    {
	0, 25, 49, 71, 90, 106, 117, 125,
	127, 125, 117, 106, 90, 71, 49, 25,
	0, -25, -49, -71, -90, -106, -117, -125,
	-127, -125, -117, -106, -90, -71, -49, -25,
    };

    CurSongPtr = (SONG_DESCPTR)_TrackList[MOD_TRACK].TrackPtr;
RestartSong:
    MEMSET ((CHANNEL_STATE *)&ChannelInfo[0], 0, sizeof (ChannelInfo));

    ChannelInfo[0].TrackVolume = CurSongPtr->TrackVolumeList[0];
    ChannelInfo[1].TrackVolume = CurSongPtr->TrackVolumeList[1];
    ChannelInfo[2].TrackVolume = CurSongPtr->TrackVolumeList[2];
    ChannelInfo[3].TrackVolume = CurSongPtr->TrackVolumeList[3];

    NumSteps = CurSongPtr->NumSteps;

    counter = NumSteps - 1;
    BlockLine = 0;
    PhysicalBlock = CurSongPtr->LogToPhysBlockList[LogicalBlock = 0];
    CurBlockPtr = (BLOCK_DESCPTR)CurSongPtr->BlockList[PhysicalBlock];
    CurLinePtr = CurBlockPtr->LineList;
    Priority = CurSongPtr->Priority;

    TempoBump = (1000 / 8) * CurSongPtr->Tempo;
    TempoError = TempoBump >> 1;
    TempoFract = (662 * (ONE_SECOND / 8)) % TempoBump;
    CurTempo = (662 * (ONE_SECOND / 8)) / TempoBump;

    SuspendTasking ();
    NextTime = GetTimeCounter () + CurTempo;
    while (1)
    {
#define ARPEGGIO_EFFECT			0x00
#define SLIDE_UP_EFFECT			0x01
#define SLIDE_DOWN_EFFECT		0x02
#define PORTAMENTO_EFFECT		0x03
#define VIBRATO_EFFECT			0x04
#define PORTAMENTO_VSLIDE_EFFECT	0x05
#define VIBRATO_VSLIDE_EFFECT		0x06
#define TREMOLO_EFFECT			0x07
#define SETHOLD_DECAY			0x08
#define SECOND_TEMPO_EFFECT		0x09
#define VOLUME_SLIDE_EFFECT1		0x0A
#define POSITION_JUMP_EFFECT		0x0B
#define VOLUME_EFFECT			0x0C
#define VOLUME_SLIDE_EFFECT2		0x0D
#define TEMPO_MISC_EFFECT		0x0F
	UWORD			Period;
	register CHANNEL_STATE	*pCInfo;

	if (++counter == NumSteps)
	{
	    counter = 0;
	    for (CurTrack = 0, pCInfo = &ChannelInfo[0];
		    CurTrack < MAX_CHANNELS; ++CurTrack, ++pCInfo)
	    {
		UWORD	textra;

  		Period = CurLinePtr->LineCommands[CurTrack][0];
		textra = MAKE_WORD (
			CurLinePtr->LineCommands[CurTrack][2],
			CurLinePtr->LineCommands[CurTrack][1]
			);
  		pCInfo->Effect = (BYTE)((textra & 0x0f00) >> 8);
  		pCInfo->EffectArgs = LOBYTE (textra);
  		if ((textra & 0xf000) || (Period & 0x80))
  		{
		    pCInfo->TrackInstrument =	/* Set Instrument No. */
			    ((textra & 0xf000) >> 12) - 1;
		    if (Period & 0x80)
			pCInfo->TrackInstrument += 16;
		    pCInfo->TrackVolume =
		  	    CurSongPtr->PresetList[pCInfo->TrackInstrument].volume;
  		}

		switch (pCInfo->Effect)
		{
		    case POSITION_JUMP_EFFECT:
			LogicalBlock = (UWORD)(pCInfo->EffectArgs - 1);
			BlockLine = CurBlockPtr->LastLine;	/* end block early */
			break;
		    case SECOND_TEMPO_EFFECT:
			if (pCInfo->EffectArgs <= 31)
			    NumSteps = pCInfo->EffectArgs;
			break;
		    case TEMPO_MISC_EFFECT:	/* set tempo */
			if (pCInfo->EffectArgs == 0)
			    BlockLine = CurBlockPtr->LastLine;	/* end block early */
			else if (pCInfo->EffectArgs <= 0xF0)
			{
			    TempoBump = (1000 / 8) * pCInfo->EffectArgs;
			    TempoError = TempoBump >> 1;
			    TempoFract = (662 * (ONE_SECOND / 8)) % TempoBump;
			    CurTempo = (662 * (ONE_SECOND / 8)) / TempoBump;
			}
			break;
		    case VOLUME_EFFECT:		/* Set Volume */
			if ((pCInfo->TrackVolume =
				(SBYTE)((pCInfo->EffectArgs >> 1) & 0xf8)
				+ ((pCInfo->EffectArgs >> 3) & 0xfe)
				+ (pCInfo->EffectArgs & 0x0f)) < 0)
			    pCInfo->TrackVolume = 0;
			break;
		}

  		if (Period &= ~0x80)
		{
		    COUNT	InstrumentNumber;

		    pCInfo->PeriodIndex = (BYTE)(Period - 1);

		    InstrumentNumber = pCInfo->TrackInstrument;
		    if (pCInfo->Effect != PORTAMENTO_EFFECT
			    && pCInfo->Effect != PORTAMENTO_VSLIDE_EFFECT)
		    {
			Period = PeriodTab[Period - 1];
			pCInfo->TrackPeriod = Period;
			SetChannelRate (CurTrack + MAX_CHANNELS,
				AMIGA_TO_SPS (Period),
				Priority);
		    }
		    SetChannelVolume (CurTrack + MAX_CHANNELS,
			    pCInfo->TrackVolume, Priority);
#ifdef DEBUG
printf ("PlayTrack (%d--%u): Period:%u, Volume:%u\n",
CurTrack,
InstrumentNumber,
pCInfo->TrackPeriod,
pCInfo->TrackVolume);
#else /* !DEBUG */

		    PlayChannel (CurTrack + MAX_CHANNELS,
			    (LPSTR)CurSongPtr->InstrumentList[InstrumentNumber],
			    CurSongPtr->PresetList[InstrumentNumber].SampleLength,
			    CurSongPtr->PresetList[InstrumentNumber].LoopBegin << 1,
			    CurSongPtr->PresetList[InstrumentNumber].LoopLength << 1,
			    Priority);
#endif /* DEBUG */
  		}
	    }

	    ++CurLinePtr;
	    if (BlockLine++ == CurBlockPtr->LastLine)
	    {
		BlockLine = 0;
		if (++LogicalBlock >= CurSongPtr->NumLogicalBlocks)
		{
		    ResumeTasking ();
		    if (!(CurSongPtr->Flags & PLAY_CONTINUOUS))
				/* end song */
			PLRStop (_TrackList[MOD_TRACK].TrackRef);

		    TaskSwitch ();
		    goto RestartSong;
		}

		PhysicalBlock = CurSongPtr->LogToPhysBlockList[LogicalBlock];
		CurBlockPtr =
			(BLOCK_DESCPTR)CurSongPtr->BlockList[PhysicalBlock];
		CurLinePtr = CurBlockPtr->LineList;
	    }
	}

	for (CurTrack = 0, pCInfo = &ChannelInfo[0];
		CurTrack < MAX_CHANNELS; ++CurTrack, ++pCInfo)
	{
	    SBYTE	Volume;

	    if ((pCInfo->Effect | pCInfo->EffectArgs) == 0)
		continue;

	    Volume = 0;
	    Period = 0;
	    switch (pCInfo->Effect)
	    {
		case ARPEGGIO_EFFECT:
		    switch (counter)
		    {
			case 0:
			    for (pCInfo->Control = 0;
				    pCInfo->Control < 36; ++pCInfo->Control)
			    {
				if (pCInfo->TrackPeriod >=
					PeriodTab[pCInfo->Control])
				    break;
			    }
			case 3:
			    Period = PeriodTab[pCInfo->Control
				    + (pCInfo->EffectArgs & 0x0f)];
			    break;
			case 1:
			case 4:
			    Period = PeriodTab[pCInfo->Control
				    + (pCInfo->EffectArgs >> 4)];
			    break;
			default: /* 2 or 5 */
			    Period = pCInfo->TrackPeriod;
			    break;
		    }
		    break;
		case SLIDE_UP_EFFECT:
#ifdef NEVER
		    if (NumSteps == 5 && !counter)
			break;
#endif /* NEVER */

#define HIGHEST_NOTE	113
		    pCInfo->TrackPeriod -= pCInfo->EffectArgs;
		    if (pCInfo->TrackPeriod < HIGHEST_NOTE)
			pCInfo->TrackPeriod = HIGHEST_NOTE;
		    break;
		case SLIDE_DOWN_EFFECT:
#ifdef NEVER
		    if (NumSteps == 5 && !counter)
			break;
#endif /* NEVER */

#define LOWEST_NOTE	856
		    pCInfo->TrackPeriod += pCInfo->EffectArgs;
		    if (pCInfo->TrackPeriod > LOWEST_NOTE)
			pCInfo->TrackPeriod = LOWEST_NOTE;
		    break;
		case TREMOLO_EFFECT:
		    if (pCInfo->EffectArgs)
		    {
			pCInfo->Control = pCInfo->EffectArgs;
			pCInfo->Depth = 0;
		    }
		    Volume = pCInfo->TrackVolume
			    + ((SinTab[(pCInfo->Depth >> 2) & ((1 << SIN_SHIFT) - 1)]
			    * (pCInfo->Control & 0x0F)) >> SIN_SHIFT);
		    pCInfo->Depth += (pCInfo->Control >> 3) & 0x3E;
		    break;
		case PORTAMENTO_EFFECT:
		case PORTAMENTO_VSLIDE_EFFECT:
		    if (pCInfo->EffectArgs)
			pCInfo->Control = pCInfo->EffectArgs;
		    Period = PeriodTab[pCInfo->PeriodIndex];
		    if (Period < pCInfo->TrackPeriod)
		    {
			pCInfo->TrackPeriod -= pCInfo->Control;
			if (pCInfo->TrackPeriod < Period)
			    pCInfo->TrackPeriod = Period;
		    }
		    else if (Period > pCInfo->TrackPeriod)
		    {
			pCInfo->TrackPeriod += pCInfo->Control;
			if (pCInfo->TrackPeriod > Period)
			    pCInfo->TrackPeriod = Period;
		    }
		    Period = 0;
		    if (pCInfo->Effect == PORTAMENTO_VSLIDE_EFFECT)
			goto SlideVolume;
		    break;
		case VIBRATO_EFFECT:
		case VIBRATO_VSLIDE_EFFECT:
		    if (pCInfo->EffectArgs)
		    {
			pCInfo->Control = pCInfo->EffectArgs;
			pCInfo->Depth = 0;
		    }
		    Period = pCInfo->TrackPeriod
			    + ((SinTab[(pCInfo->Depth >> 2) & ((1 << SIN_SHIFT) - 1)]
			    * (pCInfo->Control & 0x0F)) >> SIN_SHIFT);
		    pCInfo->Depth += (pCInfo->Control >> 3) & 0x3E;
		    if (pCInfo->Effect == VIBRATO_EFFECT)
			break;
		case VOLUME_SLIDE_EFFECT1:
		case VOLUME_SLIDE_EFFECT2:
SlideVolume:
		    if ((pCInfo->EffectArgs & 0xF0) == 0)
		    {
			if ((pCInfo->TrackVolume -=
				pCInfo->EffectArgs) < 0)
			    pCInfo->TrackVolume = 0;
		    }
		    else
		    {
			if ((pCInfo->TrackVolume +=
				(pCInfo->EffectArgs >> 4)) > MAX_TRACK_VOLUME)
			    pCInfo->TrackVolume = MAX_TRACK_VOLUME;
		    }
		    break;
		case VOLUME_EFFECT:
		    break;
		default:
		    continue;
	    }

	    if (Volume == 0)
		Volume = pCInfo->TrackVolume;
	    SetChannelVolume (CurTrack + MAX_CHANNELS,
		    (COUNT)Volume, Priority);

	    if (Period == 0)
		Period = pCInfo->TrackPeriod;
	    SetChannelRate (CurTrack + MAX_CHANNELS,
		    Period ? AMIGA_TO_SPS (Period) : 0, Priority);
#ifdef DEBUG
printf ("\t%u) Set Period(%u{%u}) and Volume(%d)\n", CurTrack, Period, pCInfo->TrackPeriod, pCInfo->TrackVolume);
#endif /* DEBUG */
	}

	do
	{
	    SWORD	NextTempo;

	    NextTempo = CurTempo;
	    if ((TempoError -= TempoFract) <= 0)
	    {
		++NextTempo;
		TempoError += TempoBump;
	    }
#ifndef DEBUG
	    ResumeTasking ();
	    SleepTask (NextTime);
	    SuspendTasking ();
	    NextTime += NextTempo;
#endif /* DEBUG */
	} while (_TrackList[MOD_TRACK].TrackPtr == 0);
    }
}


