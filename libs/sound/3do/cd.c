#include "types.h"
#include "item.h"
#include "msgport.h"
#include "kernel.h"
#include "kernelnodes.h"
#include "event.h"
#include "mem.h"
#include "trackplayer.h"

//#define DEBUG

#define NUM_RBA_BUFS		2
#define DEFAULT_RBA_BUFSIZE	(96 * 1024)
#define RBA_PRIORITY		(KernelBase->kb_CurrentTask->t.n_Priority + 10)
#define RBA_VOLUME		(0x40 << 8)

static void		*rba_buf;

typedef int	BOOLEAN;
typedef ubyte	*LPBYTE;

#define PROC(f,ar,ad)		f(ad
#define ARG(var)		var,
#define ARG_VOID		void)
#define ARG_END(var)		var)

PROC(
int CD_get_drive, (),
    ARG_VOID
)
{
    return (0);
}

PROC(
uint32 CD_get_volsize, (),
    ARG_VOID
)
{
    return (0);
}

PROC(
BOOLEAN _init_CD, (MixerIns),
    ARG_END	(Item	MixerIns)
)
{
    if (!rba_buf)
    {
	if (!(rba_buf = (char *)ThreedoAlloc (DEFAULT_RBA_BUFSIZE)))
	{
#ifdef DEBUG
printf ("couldn't allocate for RBA!\n");
#endif /* DEBUG */
	    return (FALSE);
	}

#ifdef DEBUG
printf ("Opening track player\n");
#endif /* DEBUG */
	OpenTrackPlayer (
	    RBA_PRIORITY,
	    NUM_RBA_BUFS,
	    DEFAULT_RBA_BUFSIZE >> 1,
	    rba_buf,
	    MixerIns
	    );
    }
    
    return (TRUE);
}

PROC(
BOOLEAN _is_red_book, (TrackInfo),
    ARG_END	(LPBYTE	TrackInfo)
)
{
    BOOLEAN	retval;
    
    retval = TrackInfo[0] == 'C'
	    && TrackInfo[1] == 'D'
	    && TrackInfo[2] == 'A';
    if (retval)
    {
	char	*p;
	
	p = (char *)&TrackInfo[3];
	do
	{
	    if (*p == '.')
	    {
		*(p + 4) = '\0';
		break;
	    }
	} while (*++p);
    }
    
    return (retval);
}

PROC(
BOOLEAN _play_red_book, (TrackInfo, Loop),
    ARG		(LPBYTE		TrackInfo)
    ARG_END	(BOOLEAN	Loop)
)
{
    if (rba_buf)
    {
#ifdef DEBUG
printf ("Splicing track '%s'\n", (char *)&TrackInfo[3]);
#endif /* DEBUG */
	if (SpliceTrack ((char *)&TrackInfo[3]) == 0)
	{
	    if (Loop)
		LoopedTrackPlayer ();
	    else
		LinearTrackPlayer ();


#ifdef DEBUG
printf ("Advancing track\n");
#endif /* DEBUG */
	    AdvanceTrack (RBA_VOLUME, RBA_VOLUME);
#ifdef DEBUG
printf ("Resuming track\n");
#endif /* DEBUG */
	    ResumeTrack ();
    
	    return (1);
	}
    }
    
    return (0);
}

PROC(
void _stop_red_book, (),
    ARG_VOID
)
{
//ResumeTasking ();
//while (SoundPlaying ())
//    Yield ();
//SuspendTasking ();
StopTrack ();
}

PROC(
BOOLEAN _red_book_playing, (),
    ARG_VOID
)
{
    if (PlayingTrack ())
    {
	Yield ();
	return (TRUE);
    }

    return (FALSE);
}

PROC(
void _pause_red_book, (),
    ARG_VOID
)
{
    PauseTrack ();
}

PROC(
void _resume_red_book, (),
    ARG_VOID
)
{
    ResumeTrack ();
}

PROC(
void _uninit_CD, (),
    ARG_VOID
)
{
    if (PlayingTrack ())
	StopTrack ();
    
    if (rba_buf)
    {
#ifdef DEBUG
printf ("Closing track player\n");
#endif /* DEBUG */
 	CloseTrackPlayer ();
	ThreedoFree (rba_buf, DEFAULT_RBA_BUFSIZE);
	rba_buf = 0;
    }
}
