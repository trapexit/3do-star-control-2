#include <string.h>
#include "sndintrn.h"

PROC(STATIC
BYTE near FindPeriod, (AmigaRate),
    ARG_END	(UWORD	AmigaRate)
)
{
    SIZE	lo, hi, mid;
    UWORD	p, PeriodTab[] =
    {
	3424, 3232, 3040, 2880, 2720, 2560,
	2416, 2288, 2160, 2032, 1920, 1808,
	1712, 1616, 1520, 1440, 1360, 1280,
	1208, 1144, 1080, 1016,  960,  904,

	856, 808, 762, 720, 678, 640, 604, 570,
	538, 508, 480, 453, 428, 404, 381, 360,
	339, 320, 302, 285, 269, 254, 240, 226,
	214, 202, 190, 180, 170, 160, 151, 143,
	135, 127, 120, 113, 107, 101,  95,  90,
	 85,  80,  75,  72,  68,  64,  60,  57,

	53, 50, 48, 45, 42, 40,
	38, 36, 34, 32, 30, 28,
	27, 25, 24, 22, 21, 20,
	19, 18, 17, 16, 15, 14,
    };
#define PERIOD_TAB_SIZE		(sizeof (PeriodTab) / sizeof (PeriodTab[0]))

    if (AmigaRate == 0)
	return (0);

    lo = 0;
    hi = PERIOD_TAB_SIZE - 1;
    do
    {
	mid = (lo + hi) >> 1;
	p = PeriodTab[mid];
	if (p == AmigaRate)
	    goto ExactMatch;
	else if (p < AmigaRate)
	    hi = mid - 1;
	else
	    lo = mid + 1;
    } while (lo <= hi);

    if (hi < 0)
	mid = 0;
    else if (lo >= PERIOD_TAB_SIZE)
	mid = PERIOD_TAB_SIZE - 1;
    else if (PeriodTab[hi] - AmigaRate < AmigaRate - PeriodTab[lo])
	mid = hi;
    else
	mid = lo;

ExactMatch:
    if (mid < 24)
	mid += 48;
    else if (mid < 24 + 48)
	mid -= 24;

    return ((BYTE)(mid + 1));
}

PROC(
MEM_HANDLE _GetMODData, (fp, length),
    ARG		(FILE		*fp)
    ARG_END	(DWORD		length)
)
{
#define MOD_NAME_SIZE		20
#define SAMPLE_NAME_SIZE	22
#define MIN_SAMPLES		15
#define ADDITIONAL_SAMPLES	16
#define MAX_LOGICAL_BLOCKS	128
#define NUM_UNKNOWN_BYTES	4
#define COMMON_MOD_ERROR	4
    MEM_HANDLE	h;
    COUNT	i, NumSamples;
    DWORD	Total, l;
    COUNT	SampleLength, BytesRead;
    SONG_DESC	SD;
    char	buf[256];

    if (length < sizeof (buf))
    {
	ReadResFile (buf, 1, length, fp);
	buf[length++] = '\0';
	if (!_is_red_book ((LPBYTE)buf))
	    h = 0;
	else
	{
	    LPBYTE	lpBuf;

	    h = AllocMODData (length);
	    LockMODData (h, &lpBuf);
	    if (h == 0 || lpBuf == 0)
	    {
		FreeMODData (h);
		h = 0;
	    }
	    else
	    {
		MEMCPY (lpBuf, buf, length);
		UnlockMODData (h);
	    }
	}

	return (h);
    }

    length += COMMON_MOD_ERROR;

    BytesRead = ReadResFile (buf, 1, MOD_NAME_SIZE, fp);

    Total = 0L;
    NumSamples = 0;
    MEMSET ((PSTR)&SD, 0, sizeof (SD));
    do
    {
	BytesRead += ReadResFile (buf, 1, SAMPLE_NAME_SIZE + 8, fp);
	l = (DWORD)MAKE_WORD (
		buf[SAMPLE_NAME_SIZE + 1],
		buf[SAMPLE_NAME_SIZE + 0]
		) << 1;
	if (l >= 0x0000FFFFL)
		/* sample too big */
	    return (0);

	if (SampleLength = (COUNT)l)
	{
	    ++NumSamples;

	    Total += l;
	    SD.PresetList[SD.NumInstruments].SampleLength = SampleLength;
	    SD.PresetList[SD.NumInstruments].volume = (BYTE)MAKE_WORD (
		    buf[SAMPLE_NAME_SIZE + 3],
		    buf[SAMPLE_NAME_SIZE + 2]
		    );
	    SD.PresetList[SD.NumInstruments].LoopBegin = MAKE_WORD (
		    buf[SAMPLE_NAME_SIZE + 5],
		    buf[SAMPLE_NAME_SIZE + 4]
		    );
	    SD.PresetList[SD.NumInstruments].LoopLength = MAKE_WORD (
		    buf[SAMPLE_NAME_SIZE + 7],
		    buf[SAMPLE_NAME_SIZE + 6]
		    );
	    if ((SampleLength = SD.PresetList[SD.NumInstruments].LoopLength) <= 1)
	    {
		SD.PresetList[SD.NumInstruments].LoopBegin = 0;
		SD.PresetList[SD.NumInstruments].LoopLength = 0;
	    }
	}

	if (++SD.NumInstruments == MIN_SAMPLES
		|| SD.NumInstruments == MIN_SAMPLES + ADDITIONAL_SAMPLES)
	{
#define BLOCK_LENGTH		1024L
#define SAFETY			10
	    l = Total
		    + (BytesRead
		    + MAX_LOGICAL_BLOCKS
		    + NUM_UNKNOWN_BYTES
		    + 2); /* # log blocks */
	    SD.NumPhysicalBlocks = (COUNT)(
		    (length - l) / BLOCK_LENGTH
		    );
#ifdef DEBUG
printf ("%u) %lu -- %lu\n",
SD.NumPhysicalBlocks,
l + BLOCK_LENGTH * SD.NumPhysicalBlocks + SAFETY,
length);
#endif /* DEBUG */
	    if (l + BLOCK_LENGTH * SD.NumPhysicalBlocks + SAFETY > length)
		break;
	}
    } while (SD.NumInstruments < MIN_SAMPLES + ADDITIONAL_SAMPLES);

    ReadResFile (buf, 1, 2, fp);
    SD.NumLogicalBlocks = buf[0];
    ReadResFile (SD.LogToPhysBlockList,
	    1, MAX_LOGICAL_BLOCKS
	    + (SD.NumInstruments == MIN_SAMPLES ? 0 : NUM_UNKNOWN_BYTES),
	    fp);

    if (SD.NumPhysicalBlocks > SD.NumLogicalBlocks)
    {
	SD.NumPhysicalBlocks = SD.LogToPhysBlockList[0];
	for (i = 1; i < SD.NumLogicalBlocks; ++i)
	{
	    if (SD.LogToPhysBlockList[i] > (BYTE)SD.NumPhysicalBlocks)
		SD.NumPhysicalBlocks = SD.LogToPhysBlockList[i];
	}
	++SD.NumPhysicalBlocks;
    }

    {
#define LINES_PER_BLOCK	64
	SONG_DESCPTR	SongPtr;

#ifdef DEBUG
printf ("%lu bytes needed for samples\n", Total);
printf ("sizeof (SD) = %u + %u physical blocks\n", sizeof (SD), SD.NumPhysicalBlocks);
#endif /* DEBUG */
	Total += NumSamples
		+ ((sizeof (SD)
		+ (sizeof (DWORD) * (SD.NumPhysicalBlocks - 1))
		+ (DWORD)(sizeof (BLOCK_DESC)
			+ (sizeof (LINE_DESC) * (LINES_PER_BLOCK - 1)))
		 * SD.NumPhysicalBlocks));
#ifdef DEBUG
printf ("%lu bytes allocated\n", Total);
#endif /* DEBUG */
	h = AllocMODData (Total);
	LockMODData (h, &SongPtr);

	if (h == 0 || SongPtr == 0)
	{
	    FreeMODData (h);
	    h = 0;
	}
	else
	{
	    LPBYTE		lpSnd;
	    BLOCK_DESCPTR	BlockPtr;

	    SD.Tempo = 33;
	    SD.NumSteps = 6;
	    SD.MasterVolume = MAX_TRACK_VOLUME;
	    for (i = 0; i < MAX_TRACKS; ++i)
		SD.TrackVolumeList[i] = MAX_TRACK_VOLUME;

	    *SongPtr = SD;
	    BlockPtr = (BLOCK_DESCPTR)&SongPtr->BlockList[SD.NumPhysicalBlocks];

#ifdef DEBUG
Total -= (LPSTR)BlockPtr - (LPSTR)SongPtr;
Total -= (DWORD)(sizeof (BLOCK_DESC)
	+ (sizeof (LINE_DESC) * (LINES_PER_BLOCK - 1)))
	* SD.NumPhysicalBlocks;
printf ("%lu bytes left for samples\n", Total);
#endif /* DEBUG */
	    {
		COUNT	j, k;

#define BYTES_PER_LINE	16
		for (i = 0; i < (COUNT)SD.NumPhysicalBlocks; ++i)
		{
		    LINE_DESCPTR	CurLinePtr;

		    FAR_PTR_ADD (&BlockPtr, 0);
		    BlockPtr->NumTracks = MAX_CHANNELS;
		    BlockPtr->LastLine = LINES_PER_BLOCK - 1;
		    for (j = 0, CurLinePtr = BlockPtr->LineList;
			    j < LINES_PER_BLOCK; ++j, ++CurLinePtr)
		    {
			PBYTE	pbuf;

			ReadResFile ((PVOID)(pbuf = (PBYTE)buf),
				1, BYTES_PER_LINE, fp);
#define COMMANDS_PER_LINE	4
			for (k = 0; k < COMMANDS_PER_LINE; ++k)
			{
			    UWORD	AmigaRate;

			    AmigaRate = MAKE_WORD (pbuf[1], pbuf[0]);
			    CurLinePtr->LineCommands[k][0] = FindPeriod (
				    AmigaRate & ~0xF000
				    );
			    if (AmigaRate & 0x1000)
				CurLinePtr->LineCommands[k][0] |= 0x80;
			    pbuf += 2;

			    CurLinePtr->LineCommands[k][1] = pbuf[0];
			    CurLinePtr->LineCommands[k][2] = pbuf[1];
			    switch (LONIBBLE (pbuf[0]))
			    {
				case 0xF:
				    CurLinePtr->LineCommands[k][1] =
					    MAKE_BYTE (0x9, HINIBBLE (pbuf[0]));
				    break;
				case 0xD:
				    CurLinePtr->LineCommands[k][1] =
					    MAKE_BYTE (0xF, HINIBBLE (pbuf[0]));
				    CurLinePtr->LineCommands[k][2] = 0;
				    break;
				case 0x8:
				case 0x9:
				case 0xE:
				    CurLinePtr->LineCommands[k][1] =
					    MAKE_BYTE (0x0, HINIBBLE (pbuf[0]));
				    CurLinePtr->LineCommands[k][2] = 0;
#ifdef DEBUG
printf ("Unimplemented command 0x%x with data 0x%02x\n",
LONIBBLE (pbuf[0]), pbuf[1]);
#endif /* DEBUG */
				    break;
			    }
			    pbuf += 2;
			}
		    }

		    BlockPtr = (BLOCK_DESCPTR)&BlockPtr->LineList[BlockPtr->LastLine + 1];
		 }

	    }

	    lpSnd = (LPBYTE)BlockPtr;
	    FAR_PTR_ADD (&lpSnd, 0);
	    for (i = 0; i < SD.NumInstruments; ++i)
	    {
		if (SampleLength = SD.PresetList[i].SampleLength)
		{
		    ReadResFile (lpSnd, 1, SampleLength, fp);
		    lpSnd[SampleLength] = (BYTE)~0;

		    FAR_PTR_ADD (&lpSnd, SampleLength + 1);
#ifdef DEBUG
Total -= SampleLength + 1;
#endif /* DEBUG */
		}
	    }

#ifdef DEBUG
printf ("%lu bytes left\n", Total);
#endif /* DEBUG */
	    UnlockMODData (h);
	}
    }

    return (h);
}

