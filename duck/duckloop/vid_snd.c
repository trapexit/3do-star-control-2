#include "types.h"
#include "nodes.h"
#include "mem.h"
#include "audio.h"
#include "filesystem.h"
#include "filefunctions.h"
#include "filestream.h"
#include "filestreamfunctions.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "event.h"

enum
{
    LOOPING_SAMPLE,
    ONESHOT_SAMPLE,
    NUM_SAMPLES
};

enum
{
    PLR_LOAD_SAMPLE = 1,
    PLR_UNLOAD_SAMPLE,
    PLR_PLAY_SAMPLE,
    PLR_STOP_SAMPLE,
    PLR_DIE,
    
    PLR_ASYNC = 0x1000,
    PLR_START_INS = 0x2000,
    PLR_STOP_INS = 0x4000,
    PLR_ONESHOT_SAMPLE = 0x8000,
    PLR_LOOPING_SAMPLE = 0x10000
};

#define PLR_FLAGS	(PLR_ASYNC | PLR_START_INS | PLR_STOP_INS \
			| PLR_ONESHOT_SAMPLE | PLR_LOOPING_SAMPLE)

#define START_LOOP_VOLUME	(0x1fff)
#define END_LOOP_VOLUME		(0x3fff)
#define ONESHOT_VOLUME		(0x5fff)

#define ONE_SECOND		239
#define FRAMES_PER_SEC		15

static int32	player_sig, parent_sig;
static Item	player_task, parent_task;
static char	sample_name[NUM_SAMPLES][20];
static int32	player_cmd;

static Item	output_ins, sample_ins[NUM_SAMPLES], sample[NUM_SAMPLES], sample_att[NUM_SAMPLES];

static int32	cue_sig;
static Item	cue_item;

static void
setup_sample (int32 which)
{
    int32		rate, channels, width, comp_type;
    Item		freq_knob;
    TagArg		t[2];
    static TagArg	tp[] =
			{
			    {AF_TAG_SAMPLE_RATE, 0},
			    {AF_TAG_CHANNELS, 0},
			    {AF_TAG_WIDTH, 0},
			    {AF_TAG_COMPRESSIONTYPE, 0},
			    TAG_END, 0
			};

    if ((sample[which] = LoadSample (sample_name[which])) < 0)
	return;
    
//DebugSample (sample[which]);

    GetAudioItemInfo (sample[which], tp);
    
    rate = (int32)tp[0].ta_Arg;
    channels = (int32)tp[1].ta_Arg;
    width = (int32)tp[2].ta_Arg;
    comp_type = (int32)tp[3].ta_Arg;
    
    if (channels > 1)
    {
	UnloadSample (sample[which]);
	sample[which] = -1;
	return;
    }

    sample_ins[which] = LoadInstrument ("varmono8.dsp", 0, 100);
    sample_att[which] = AttachSample (sample_ins[which], sample[which], 0);
    if (which == LOOPING_SAMPLE)
	LinkAttachments (sample_att[which], sample_att[which]);
    else
    {
	cue_item = CreateItem (MKNODEID (AUDIONODE, AUDIO_CUE_NODE), NULL);
	cue_sig = GetCueSignal (cue_item);
	MonitorAttachment (sample_att[which], cue_item, CUE_AT_END);
    }

    t[0].ta_Tag = AF_TAG_SET_FLAGS;
    t[0].ta_Arg = (void *)AF_ATTF_NOAUTOSTART;
    t[1].ta_Tag = TAG_END;
    
    SetAudioItemInfo (sample_att[which], t);
    
    freq_knob = GrabKnob (sample_ins[which], "Frequency");
    TweakKnob (freq_knob, ((rate >> 8) * 3) >> 10); // 3/4 is the magic scale factor!
    ReleaseKnob (freq_knob);

    if (which == LOOPING_SAMPLE)
	ConnectInstruments (sample_ins[which], "Output", output_ins, "Input0");
    else
	ConnectInstruments (sample_ins[which], "Output", output_ins, "Input1");
}

static void
unload_sample (int32 which)
{
    if (which == LOOPING_SAMPLE)
	LinkAttachments (sample_att[which], 0);
    else
    {
	if (cue_item >= 0)
	    DeleteItem (cue_item);
    }
    
    if (sample_att[which] >= 0)
	DetachSample (sample_att[which]);
    if (sample[which] >= 0)
	UnloadSample (sample[which]);
    if (sample_ins[which] >= 0)
	UnloadInstrument (sample_ins[which]);

    // This apparently doesn't hurt if they're not connected... right?		
    DisconnectInstruments (sample_ins[which], "Output", output_ins, "Input0");
    DisconnectInstruments (sample_ins[which], "Output", output_ins, "Input1");
}

static void
expand_looping_sample ()
{
#define NUM_STEPS	10
    Item	left_vol_knob[2], right_vol_knob[2];
    int32	n, vol, t, verr;
    
    DisconnectInstruments (sample_ins[ONESHOT_SAMPLE], "Output", output_ins, "Input1");
    
    left_vol_knob[0] = GrabKnob (output_ins, "LeftGain0");
    right_vol_knob[0] = GrabKnob (output_ins, "RightGain0");

    left_vol_knob[1] = GrabKnob (output_ins, "LeftGain1");
    right_vol_knob[1] = GrabKnob (output_ins, "RightGain1");

    TweakKnob (left_vol_knob[1], 0); 
    TweakKnob (right_vol_knob[1], 0);
	
    ConnectInstruments (sample_ins[LOOPING_SAMPLE], "Output", output_ins, "Input1");

    // ramp up other side to START_LOOPING_VOLUME    
    vol = 0;
    verr = NUM_STEPS;
    t = GetAudioTime ();
    n = NUM_STEPS;
    do
    {
	vol += START_LOOP_VOLUME / NUM_STEPS;
	if ((verr -= (START_LOOP_VOLUME % NUM_STEPS)) <= 0)
	{
	    ++vol;
	    verr += NUM_STEPS;
	}
	
	TweakKnob (left_vol_knob[1], vol); 
	TweakKnob (right_vol_knob[1], vol);
	    
	do
	    Yield ();
	while (GetAudioTime () < t + (ONE_SECOND / 10));
	t = GetAudioTime ();
    } while (--n);
    
    // ramp up both sides to END_LOOPING_VOLUME    
    vol = START_LOOP_VOLUME;
    verr = NUM_STEPS;
    t = GetAudioTime ();
    n = NUM_STEPS;
    do
    {
	vol += (END_LOOP_VOLUME - START_LOOP_VOLUME) / NUM_STEPS;
	if ((verr -= ((END_LOOP_VOLUME - START_LOOP_VOLUME) % NUM_STEPS)) <= 0)
	{
	    ++vol;
	    verr += NUM_STEPS;
	}
	
	TweakKnob (left_vol_knob[0], vol); 
	TweakKnob (right_vol_knob[0], vol);
	TweakKnob (left_vol_knob[1], vol); 
	TweakKnob (right_vol_knob[1], vol);
	    
	do
	    Yield ();
	while (GetAudioTime () < t + (ONE_SECOND / 10));
	t = GetAudioTime ();
    } while (--n);
    
    ReleaseKnob (left_vol_knob[0]);
    ReleaseKnob (right_vol_knob[0]);

    ReleaseKnob (left_vol_knob[1]);
    ReleaseKnob (right_vol_knob[1]);
}

static void
start_sample (int32 which)
{
    if (sample[which] < 0)
	return;
	
    StartInstrument (sample_ins[which], 0);
    StartAttachment (sample_att[which], 0);
}

static void
stop_sample (int32 which)
{
    if (sample[which] < 0)
	return;

    StopAttachment (sample_att[which], 0);
    StopInstrument (sample_ins[which], 0);
}

static void
init_player ()
{
    Item	left_vol_knob, right_vol_knob;
    
    ChangeDirectory ("$boot/duckart");
    OpenAudioFolio ();
    
    output_ins = LoadInstrument ("mixer2x2.dsp", 0, 0);
    
    player_sig = AllocSignal (0);

    left_vol_knob = GrabKnob (output_ins, "LeftGain0");
    right_vol_knob = GrabKnob (output_ins, "RightGain0");

    TweakKnob (left_vol_knob, START_LOOP_VOLUME); 
    TweakKnob (right_vol_knob, START_LOOP_VOLUME);

    ReleaseKnob (left_vol_knob);
    ReleaseKnob (right_vol_knob);

    left_vol_knob = GrabKnob (output_ins, "LeftGain1");
    right_vol_knob = GrabKnob (output_ins, "RightGain1");

    TweakKnob (left_vol_knob, ONESHOT_VOLUME); 
    TweakKnob (right_vol_knob, ONESHOT_VOLUME);
    
    ReleaseKnob (left_vol_knob);
    ReleaseKnob (right_vol_knob);

    sample_ins[0] = sample_ins[1] = -1;
    sample[0] = sample[1] = -1;
    sample_att[0] = sample_att[1] = -1;
    cue_item = -1;
    cue_sig = 0;
}

static void
uninit_player ()
{
    FreeSignal (player_sig);
    if (output_ins >= 0)
	UnloadInstrument (output_ins);
	
    CloseAudioFolio ();
}

static void
player_func ()
{
    init_player ();

    for (;;)
    {
	int32	async;
	
	async = player_cmd & PLR_ASYNC;
	if (player_cmd & PLR_START_INS)
	{
	    if (output_ins >= 0)
		StartInstrument (output_ins, 0);
	}
	
	switch (player_cmd & ~PLR_FLAGS)
	{
	    case PLR_LOAD_SAMPLE:
		if (player_cmd & PLR_LOOPING_SAMPLE)
		    setup_sample (LOOPING_SAMPLE);
		if ((player_cmd & PLR_ONESHOT_SAMPLE) && sample_name[ONESHOT_SAMPLE][0])
		    setup_sample (ONESHOT_SAMPLE);
		break;
	    case PLR_PLAY_SAMPLE:
		if (player_cmd & PLR_LOOPING_SAMPLE)
		    start_sample (LOOPING_SAMPLE);
		if (player_cmd & PLR_ONESHOT_SAMPLE)
		    start_sample (ONESHOT_SAMPLE);
		break;
	    case PLR_STOP_SAMPLE:
		if (player_cmd & PLR_LOOPING_SAMPLE)
		    stop_sample (LOOPING_SAMPLE);
		if (player_cmd & PLR_ONESHOT_SAMPLE)
		    stop_sample (ONESHOT_SAMPLE);
		break;
	    case PLR_UNLOAD_SAMPLE:
	        if (player_cmd & PLR_LOOPING_SAMPLE)
		    unload_sample (LOOPING_SAMPLE);
		if (player_cmd & PLR_ONESHOT_SAMPLE)
		    unload_sample (ONESHOT_SAMPLE);
		break;
	    case PLR_DIE:
	        uninit_player ();
		SendSignal (parent_task, parent_sig);
		WaitSignal (0); // wait forever
		break;
	}

	if (player_cmd & PLR_STOP_INS)
	{
	    if (output_ins >= 0)
		StopInstrument (output_ins, 0);
	}
	
	if (!async)
	    SendSignal (parent_task, parent_sig);
	player_cmd = 0;
	if (WaitSignal (player_sig | cue_sig) == cue_sig) // oneshot sound done
	{
	    expand_looping_sample ();
// this causes problems here -- why?
//	    unload_sample (ONESHOT_SAMPLE);
	}
    }
}

#ifdef STANDALONE
static uint32
ButtonPress ()
{
    ControlPadEventData	cp;
    
    GetControlPad (1, 0, &cp);

    return (cp.cped_ButtonBits);
}
#else
#define ButtonPress()	(AnyButtonPress (0))
#endif

#ifdef STANDALONE
#define WAIT_FOR_INPUT
int
main (int argc, char **argv)
#else
void
duckloop (char *dukfile, char *loopfile)
#endif
{
#define NUM_FRAMES	90
    uint32	t, start_time;
    int32	err;
#ifdef STANDALONE
#ifdef WAIT_FOR_INPUT
    int32	done;
#endif
    char	*dukfile, *loopfile;
    
    dukfile = *(argv + 1);
    loopfile = *(argv + 2);
    if (OpenAudioFolio ()
 	    || InitEventUtility (2L, 0L, 0L))
	exit (0);
	
    ChangeDirectory ("$boot");
#endif

    parent_sig = AllocSignal (0);
    parent_task = KernelBase->kb_CurrentTask->t.n_Item;
    
    player_cmd = 0;
    
    if ((player_task = CreateThread ("play",
	    KernelBase->kb_CurrentTask->t.n_Priority, player_func, 4096)) < 0)
    {
	if (!InitVidLoop (dukfile, 0, NUM_FRAMES))
	    goto Cleanup;
	    
	while (!ButtonPress ())
	{
	    uint32	t;

	    t = GetAudioTime ();
	    DoVidLoop ();
	    while (GetAudioTime () < t + (ONE_SECOND / FRAMES_PER_SEC))
		Yield ();
	}
	UninitVidLoop ();
	
	goto Cleanup;
    }
    
    WaitSignal (parent_sig);

    // do before sound -- if there's memory problems, let's at least show the video    
    if (!InitVidLoop (dukfile, 0, NUM_FRAMES))
	goto Cleanup;
    
    player_cmd = PLR_LOAD_SAMPLE | PLR_ONESHOT_SAMPLE | PLR_LOOPING_SAMPLE;
    sprintf (sample_name[LOOPING_SAMPLE], "%s.aif", loopfile);
    sprintf (sample_name[ONESHOT_SAMPLE], "%s.aif", dukfile);
    SendSignal (player_task, player_sig);
    WaitSignal (parent_sig);

    DoVidLoop ();
    
    player_cmd = PLR_PLAY_SAMPLE | PLR_ONESHOT_SAMPLE | PLR_START_INS | PLR_ASYNC;
    SendSignal (player_task, player_sig);

    start_time = GetAudioTime ();
    err = FRAMES_PER_SEC;
#ifdef STANDALONE
#ifdef WAIT_FOR_INPUT
    done = 0;
    while (!done)
#else
    while (!ButtonPress ())
#endif
#else
    while (!ButtonPress ())
#endif
    {
	uint32	tix;
#ifdef STANDALONE
#ifdef WAIT_FOR_INPUT
	int32	b;
#endif
uint32	tt;
extern int32	framecount, frameticks;

tt = GetAudioTime ();
#endif
	tix = GetAudioTime () + (ONE_SECOND / FRAMES_PER_SEC);
	if ((err -= (ONE_SECOND % FRAMES_PER_SEC)) <= 0)
	{
	    tix++;
	    err += FRAMES_PER_SEC;
	}

	DoVidLoop ();

#ifdef STANDALONE
#ifdef WAIT_FOR_INPUT
MoreInput:
	b = ButtonPress ();
	switch (b)
	{
	    case ControlX:
		done = 1;
		break;
	    case ControlA:
		while (ButtonPress ());
		break;
	    case ControlC:
		break;
	    default:
	        goto MoreInput;
	}
#endif
#endif

	do
	    Yield ();
	while ((t = GetAudioTime ()) < tix);
	
	if (start_time && t >= start_time + (ONE_SECOND / 4))
	{
	    start_time = 0;
	    player_cmd = PLR_PLAY_SAMPLE | PLR_LOOPING_SAMPLE | PLR_ASYNC;
	    SendSignal (player_task, player_sig);
	}
#ifdef STANDALONE
framecount++;
frameticks += GetAudioTime () - tt;
#endif
    }	
    
    player_cmd = PLR_STOP_SAMPLE | PLR_ONESHOT_SAMPLE | PLR_LOOPING_SAMPLE | PLR_STOP_INS;
    SendSignal (player_task, player_sig);
    WaitSignal (parent_sig);
    
    player_cmd = PLR_UNLOAD_SAMPLE | PLR_ONESHOT_SAMPLE | PLR_LOOPING_SAMPLE;
    SendSignal (player_task, player_sig);
    WaitSignal (parent_sig);
    
    UninitVidLoop ();

Cleanup:
    if (player_task >= 0)
    {
	// can't possibly have loaded the samples if jumped here
	player_cmd = PLR_DIE;
	SendSignal (player_task, player_sig);
	WaitSignal (parent_sig);

	DeleteThread (player_task);
    }
    
    FreeSignal (parent_sig);

#ifdef STANDALONE
    KillEventUtility ();
    CloseAudioFolio ();
    
    exit (0);
#else
    DisplayCurScreen ();
    
    // shouldn't hurt, no?
    ScavengeMem ();
#endif
}

#ifdef STANDALONE
void *
ThreedoAlloc (int32 size)
{
    return (AllocMem (size, MEMTYPE_ANY));
}

void
ThreedoFree (void *p, int32 size)
{
    FreeMem (p, size);
}
#endif
