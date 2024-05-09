#include <io.h>
#include <string.h>
#include "reslib.h"
#include "sndlib.h"
#include "timlib.h"
#include "uc.h"
#include "fastf.h"

#define SQLCH 0x40		// Squelch byte flag
#define RESYNC 0x80 	// Resync byte flag.

#define DELTAMOD 0x30 	// Delta modulation bits.

#define ONEBIT 0x10 		// One bit delta modulate
#define TWOBIT 0x20 		// Two bit delta modulate
#define FOURBIT 0x30		// four bit delta modulate

#define MULTIPLIER 0x0F  // Bottom nibble contains multiplier value.
#define SQUELCHCNT 0x3F  // Bits for squelching.

static signed char trans[16*16] =
{
    -8,-7,-6,-5,-4,-3,-2,-1,1,2,3,4,5,6,7,8,			// Multiplier of 1
    -16,-14,-12,-10,-8,-6,-4,-2,2,4,6,8,10,12,14,16,		// Multiplier of 2
    -24,-21,-18,-15,-12,-9,-6,-3,3,6,9,12,15,18,21,24,		// Multiplier of 3
    -32,-28,-24,-20,-16,-12,-8,-4,4,8,12,16,20,24,28,32,	// Multiplier of 4
    -40,-35,-30,-25,-20,-15,-10,-5,5,10,15,20,25,30,35,40,	// Multiplier of 5
    -48,-42,-36,-30,-24,-18,-12,-6,6,12,18,24,30,36,42,48,	// Multiplier of 6
    -56,-49,-42,-35,-28,-21,-14,-7,7,14,21,28,35,42,49,56,	// Multiplier of 7
    -64,-56,-48,-40,-32,-24,-16,-8,8,16,24,32,40,48,56,64,	// Multiplier of 8
    -72,-63,-54,-45,-36,-27,-18,-9,9,18,27,36,45,54,63,72,	// Multiplier of 9
    -80,-70,-60,-50,-40,-30,-20,-10,10,20,30,40,50,60,70,80,	// Multiplier of 10
    -88,-77,-66,-55,-44,-33,-22,-11,11,22,33,44,55,66,77,88,	// Multiplier of 11
    -96,-84,-72,-60,-48,-36,-24,-12,12,24,36,48,60,72,84,96,	// Multiplier of 12
    -104,-91,-78,-65,-52,-39,-26,-13,13,26,39,52,65,78,91,104,	// Multiplier of 13
    -112,-98,-84,-70,-56,-42,-28,-14,14,28,42,56,70,84,98,112,	// Multiplier of 14
    -120,-105,-90,-75,-60,-45,-30,-15,15,30,45,60,75,90,105,120,// Multiplier of 15
    -128,-112,-96,-80,-64,-48,-32,-16,16,32,48,64,80,96,112,127,// Multiplier of 16
};

// This is used to make certain this C code is compatible when compiled on
// a 68000 based machine.  (Which it has been done and tested on.)
#define Get8086word(t)	MAKE_WORD ((t)[0], (t)[1])

// GetFreq will report the playback frequency of a particular ACOMP data
// file.
PROC(STATIC
COUNT GetFreq, (sound),
    ARG_END	(unsigned char	*sound)
)
{
    return( Get8086word(sound+2) );
}

#define FLIP_FLOP_SIZE	(48 * 1024)

#if 0
#define FLIP_FLOP_SIZE	(4 * 1024)
#define FLIP_FLOP_EXTRA	256
static BYTE	*flipflop, *curflop;
STATIC TASK	SpoolTask;

PROC_GLOBAL(
void PostPending, (sound, length, freq),
    ARG	(char	*sound)
    ARG	(int	length)
    ARG_END	(int	freq)
);

PROC(STATIC
BYTE *PostCurFlop, (freq),
    ARG_END	(COUNT	freq)
)
{
    PROC_GLOBAL(
    void WaitPending, (sound),
	ARG_END	(char	*sound)
    );

    PostPending (curflop, FLIP_FLOP_SIZE, freq);
    flipflop[FLIP_FLOP_SIZE * 2 + FLIP_FLOP_EXTRA - 1] =/* save id byte */
	    flipflop[FLIP_FLOP_SIZE * 2];
    curflop = flipflop + ((curflop - flipflop) ^ FLIP_FLOP_SIZE);
    WaitPending (curflop);

    return (SpoolTask == 0 ? 0 : curflop);
}
#endif

PROC(
void UnCompressAudio, (source, curflop),
    ARG		(unsigned char	*source)
    ARG_END	(unsigned char	*curflop)
)
{
    COUNT		slen,frame,freq;
    unsigned char	*dest;
    short		prev;
    long		ffcount;

    dest = curflop;
    ffcount = 0;//(curflop - flipflop) & (FLIP_FLOP_SIZE - 1);
//    curflop -= ffcount;

    slen = Get8086word(source);
    freq = GetFreq(source);
    source+=4;		    // Skip length, and then frequency word.
    frame = *source++;    // Frame size.
    source+=3;		     // Skip sqelch value, and maximum error allowed.
    prev = *source++;      // Get initial previous data point.
    *dest++ = prev ^ 0x80;
    slen--;			  // Decrement total sound length.
    if (++ffcount == FLIP_FLOP_SIZE)
	goto FlushBuf;
    while (slen > 0)
    {
	do
	{
	    COUNT		bytes;
	    unsigned char	sample;

	    sample = *source++;  // Get sample.
	    if (sample & RESYNC) // Is it a resync byte?
	    {
		--slen;  // Decrement output sample length.

		prev = (sample & 0x7F) << 1; // Store resync byte.
		*dest++ = prev ^ 0x80;
	    }
	    else if (sample & SQLCH) // Is it a squelch byte?
	    {
		bytes = sample & SQUELCHCNT;    // And off the number of squelch bytes
		slen -= bytes;    // Decrement total samples remaining count.

		MEMSET (dest, prev ^ 0x80, bytes);
		dest += bytes;
	    }
	    else	// Must be a delta modulate byte!!
	    {
		signed char	*base;

		slen -= frame; // Pulling one frame out.
			// Compute base address to multiplier table.
		base = trans + (sample & MULTIPLIER) * 16;
		switch (sample & DELTAMOD) // Delta mod resolution.
		{
		    case ONEBIT:
		    {
			short	up;

			up = base[8];     // Go up 1 bit.
			for (bytes = frame / 8; bytes; bytes--)
			{
			    unsigned char	mask;

			    sample = *source++;
			    for(mask = 0x80; mask; mask >>= 1)
			    {
				if ( sample & mask )
				    prev += up;
				else
				    prev -= up;
				if ( prev < 0 ) prev = 0;
				else if ( prev > 255 ) prev = 255;
				*dest++=prev ^ 0x80;
			    }
			}
			break;
		    }
		    case TWOBIT:
			base+=6; // Base address of two bit delta's.
			for (bytes = frame / 4; bytes; bytes--)
			{
			    sample = *source++;

			    prev += base[sample>>6];
			    if ( prev < 0 ) prev = 0;
			    else if ( prev > 255 ) prev = 255;
			    *dest++ = prev ^ 0x80;

			    prev += base[(sample>>4)&0x3];
			    if ( prev < 0 ) prev = 0;
			    else if ( prev > 255 ) prev = 255;
			    *dest++ = prev ^ 0x80;

			    prev += base[(sample>>2)&0x3];
			    if ( prev < 0 ) prev = 0;
			    else if ( prev > 255 ) prev = 255;
			    *dest++ = prev ^ 0x80;

			    prev += base[sample&0x3];
			    if ( prev < 0 ) prev = 0;
			    else if ( prev > 255 ) prev = 255;
			    *dest++ = prev ^ 0x80;
			}
			break;
		    case FOURBIT:
			for (bytes = frame / 2; bytes; bytes--)
			{
			    sample = *source++;

			    prev += base[sample>>4];
			    if ( prev < 0 ) prev = 0;
			    else if ( prev > 255 ) prev = 255;
			    *dest++ = prev ^ 0x80;

			    prev += base[sample&0x0F];
			    if ( prev < 0 ) prev = 0;
			    else if ( prev > 255 ) prev = 255;
			    *dest++ = prev ^ 0x80;
			}
			break;
		}
	    }
		// While still audio data to decompress....
	} while ((ffcount = dest - curflop) < FLIP_FLOP_SIZE && slen > 0);

	if (ffcount >= FLIP_FLOP_SIZE)
	{
FlushBuf:
	    if ((ffcount -= FLIP_FLOP_SIZE) > 0 && curflop != flipflop)
	    {
		MEMCPY (flipflop, &flipflop[FLIP_FLOP_SIZE * 2], ffcount);
		flipflop[FLIP_FLOP_SIZE * 2] =	/* retrieve id byte */
			flipflop[FLIP_FLOP_SIZE * 2 + FLIP_FLOP_EXTRA - 1];
	    }
	    if ((dest = PostCurFlop (freq)) == 0)
		break;
	    dest += ffcount;
	}
    }

    curflop = dest;
}

#if 0
PROC(
void StopSpooledAudio, (),
    ARG_VOID
)
{
    TASK	T;

    SuspendTasking ();
    if (T = SpoolTask)
    {
	SpoolTask = 0;
	while (flipflop)
	    TaskSwitch ();
	ResumeTasking ();
	FastClose ();
	DeleteTask (T);
	SuspendTasking ();
    }
    ResumeTasking ();
}

#define CACHE_SIZE	(64 * 1024)

static struct
{
    COUNT	TotalFrames;
    DWORD	TotSiz;
    COUNT	bufsize, freq;
} abx;

PROC(STATIC
void spool_task, (),
    ARG_VOID
)
{
    PROC_GLOBAL(
    void *realalloc, (size),
	ARG_END	(DWORD	size)
    );

    SuspendTasking ();
    if (flipflop = realalloc (FLIP_FLOP_SIZE * 2 + FLIP_FLOP_EXTRA))
    {
	COUNT	FrameNo;
	char	*scratch;
	DWORD	OldFreq;
	ABH	FList[50];
	PROC_GLOBAL(
	void realfree, (ptr),
	    ARG_END	(void	*ptr)
	);
	PROC_GLOBAL(
	void *memalloc, (size),
	    ARG_END	(DWORD	size)
	);
	PROC_GLOBAL(
	void memfree, (ptr),
	    ARG_END	(void	*ptr)
	);

	curflop = flipflop;

	FastRead (FList, sizeof (FList[0]) * abx.TotalFrames);
	OldFreq = SetAudioBaseRate (abx.freq);

	scratch = memalloc (abx.bufsize);
	for (FrameNo = 0; FrameNo < abx.TotalFrames && SpoolTask; ++FrameNo)
	{
	    FastRead (scratch, FList[FrameNo].fsize);  // Read compressed data frame.
	    abx.bufsize = 0;
	    UnCompressAudio (scratch);
	}
	memfree (scratch);

	if (SpoolTask)
	{
	    if (FrameNo =
		    (curflop - flipflop) & (FLIP_FLOP_SIZE - 1))
	    {
		MEMSET (curflop, 0x00, FLIP_FLOP_SIZE - FrameNo);
		curflop -= FrameNo;
		curflop = PostCurFlop (abx.freq);
	    }

	    if (curflop)
		WaitPending (
			flipflop + ((curflop - flipflop) ^ FLIP_FLOP_SIZE)
			);
	}
	PostPending (NULL_PTR, 0, 0);
	SetAudioBaseRate (OldFreq);

	realfree (flipflop);
	flipflop = 0;
    }
    ResumeTasking ();

    StopSpooledAudio ();
    while (1)
	TaskSwitch ();
}

PROC(STATIC
void start_spool, (),
    ARG_VOID
)
{
    StopSpooledAudio ();

    SpoolTask = AddTask (spool_task, 4096);
    TaskSwitch ();
}

PROC(
void PlaySpooledAudioFile, (aud_file),
    ARG_END	(char	*aud_file)
)
{
    if (FastOpen (aud_file, CACHE_SIZE))
    {
	FastRead (&abx, sizeof (abx));
	start_spool ();
	while (abx.bufsize)
	    TaskSwitch ();
    }
}
#endif
