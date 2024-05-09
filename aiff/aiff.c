#include "types.h"
#include "item.h"
#include "msgport.h"
#include "kernel.h"
#include "kernelnodes.h"
#include "event.h"
#include "mem.h"
#include "trackplayer.h"
#include "debug.h"
#include "event.h"
#include "Portfolio.h"
#include "Init3DO.h"
#include "Parse3DO.h"
#include "Utils3DO.h"
#include "filefunctions.h"
#include "BlockFile.h"

#define BUFSIZE		(1024 * 96)
#define NUM_BUFFERS	(2)
#define MY_VOL		(0x7fff)

CCB	plCCB[256];

static int32
myReadControlPad (int32 mask)
{
    ControlPadEventData	cped;
    
    GetControlPad (1, 0, &cped);
    return (cped.cped_ButtonBits);
}

void
main (int argc, char **argv)
{
#define ONE_SECOND	239
    char	*buf;
    int32	size;
    int32	track, track_ct, LastOscillTime, looping;
    char	*files[20];
    char	*fixed_files[] =
    		{
#if 1
		    "starcon2/comm/starbas/starb151.abx",
		    "starcon2/comm/starbas/starb173.abx",
		    "starcon2/comm/starbas/starb179.abx",
		    "starcon2/comm/starbas/starb186.abx",
		    "starcon2/comm/starbas/starb153.abx",
#else
		    "starcon2/comm/starbas/starb151.abx",
		    "starcon2/comm/starbas/starb163.abx",
		    "starcon2/comm/starbas/starb156.abx",
		    "starcon2/comm/starbas/starb158.abx",
		    "starcon2/comm/starbas/starb162.abx",
		    "starcon2/comm/starbas/starb174.abx",
		    "starcon2/comm/starbas/starb180.abx",
		    "starcon2/comm/starbas/starb182.abx",
		    "starcon2/comm/starbas/starb184.abx",
		    "starcon2/comm/starbas/starb187.abx",
		    "starcon2/comm/starbas/starb190.abx",
#endif
#if 0
		    "starcon2/comm/zoqfot/zoqfo000.abx",
		    "starcon2/comm/zoqfot/zoqfo001.abx",
		    "starcon2/comm/zoqfot/zoqfo002.abx",
		    "starcon2/comm/zoqfot/zoqfo003.abx",
		    "starcon2/comm/zoqfot/zoqfo004.abx",
		    "starcon2/comm/zoqfot/zoqfo005.abx",
		    "starcon2/comm/zoqfot/zoqfo006.abx",
		    "starcon2/comm/zoqfot/zoqfo007.abx",
		    "starcon2/comm/zoqfot/zoqfo008.abx",
		    "starcon2/comm/zoqfot/zoqfo009.abx",
#endif
#if 0
		    "starcon2/comm/melnorm/melno045.abx",
		    "starcon2/comm/melnorm/melno047.abx",
		    "starcon2/comm/melnorm/melno175.abx",
		    "starcon2/comm/melnorm/melno160.abx",
		    "starcon2/comm/melnorm/melno173.abx",
		    "starcon2/comm/melnorm/melno161.abx",
		    "starcon2/comm/melnorm/melno176.abx",
		    "starcon2/comm/melnorm/melno162.abx",
		    "starcon2/comm/melnorm/melno175.abx",
		    "starcon2/comm/melnorm/melno165.abx",
		    "starcon2/comm/melnorm/melno188.abx",
		    "starcon2/comm/melnorm/melno175.abx",
		    "starcon2/comm/melnorm/melno170.abx",
#endif
#if 0
		    "starcon2/comm/starbas/starb012.abx",
		    "starcon2/comm/starbas/starb003.abx",
		    "starcon2/comm/starbas/starb107.abx",
		    "starcon2/comm/starbas/starb102.abx",
		    "starcon2/comm/starbas/starb108.abx",
		    "starcon2/comm/starbas/starb102.abx",
		    "starcon2/comm/starbas/starb114.abx",
#endif
		    NULL
		};

    OpenAudioFolio ();
    InitEventUtility (2, 0, 0);
    Init3DO ();
    
    size = BUFSIZE;
    buf = malloc (size);

    OpenTrackPlayer(
	    100,
	    NUM_BUFFERS,
	    size / NUM_BUFFERS,
	    buf,
	    -1);
    looping = 0;

    argc--;
    argv++;
    
    track_ct = 0;
    if (argc == 0 && fixed_files[0])
    {
	argc = (sizeof (fixed_files) / sizeof (fixed_files[0])) - 1;
	argv = fixed_files;
    }
    
    while (argc--)
	files[track_ct++] = *argv++;
    files[track_ct] = 0;
    
    InitOscilloscope ((320 - 56) >> 1, (240 - 53) >> 1, 56, 53);
    InitSlider (85, 40, 150, 8);
Again:
    argc = track_ct;
    argv = files;
    while (argc--)
    {
	printf ("Track %ld is '%s'\n", track_ct - argc - 1, *argv);
	SpliceTrack (*(argv++));
    }
    
    AdvanceTrack (MY_VOL, MY_VOL);
    ResumeTrack ();
    track = PlayingTrack ();
    LastOscillTime = GetAudioTime ();
    do
    {
#define ControlReverse	(ControlLeft | ControlDown)
#define ControlForward	(ControlRight | ControlUp)
	int32	r;

	r = myReadControlPad (~0);
	if (r & (ControlReverse | ControlForward))
	{
	    int32	accel;
	    
	    if (r & ControlReverse)
		r &= ~ControlForward;
	    else 
		r &= ~ControlReverse;
	    accel = 5;
	    do
	    {
		int32	t;
		
		ClearScreen ();
		if (r & ControlForward)
		    FastForward ();
		else
		    FastReverse ();
		Oscilloscope (0);
		Slider ();
		batch_cels (1);
		t = GetAudioTime ();
		do
		    Yield ();
		while (GetAudioTime () < t + ONE_SECOND / accel);
		if (accel < 40)
		    ++accel;
	    } while (myReadControlPad (~0) & r);
	    ResumeTrack ();
	    track = PlayingTrack ();
	}
	else if (r & ControlX)
	{
	    goto Done;
	}
	else if (r & ControlStart)
	{
	    ClearScreen ();
	    Oscilloscope (0);
	    Slider ();
	    batch_cels (1);
	    PauseTrack ();
	    while (myReadControlPad (~0))
		Yield ();
	    while (!(r = myReadControlPad (~0)))
		Yield ();
	    while (myReadControlPad (~0))
		Yield ();
	    ResumeTrack ();
	    track = PlayingTrack ();
	}
	else if (r & ControlA)
	{
	    StopTrack ();
	    Yield ();
	    break;
	}
	else if (r & ControlB)
	{
	    JumpTrack ();
	    Yield ();
	}
	else if (r & ControlC)
	{
	    if (looping = !looping)
	    {
		printf ("Looping on!\n");
		LoopedTrackPlayer ();
	    }
	    else
	    {
		printf ("Looping off!\n");
		LinearTrackPlayer ();
	    }
	    while (myReadControlPad (~0))
		Yield ();
	}
	else if (GetAudioTime () > LastOscillTime + ONE_SECOND / 16)
	{
	    ClearScreen ();    
	    Oscilloscope (1);
	    Slider ();
	    batch_cels (1);
	    LastOscillTime = GetAudioTime ();
	}
    } while (1);

goto Again;

Done:
    StopTrack ();
    CloseTrackPlayer ();
    free (buf);
    
    CloseAudioFolio ();
    Uninit3DO ();
    KillEventUtility ();
}

void
StopSound ()
{
}
