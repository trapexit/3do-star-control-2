// trackplayer.c
// 11/04/93 - (THUR) - added PauseTrack(), ResumeTrack(), RewindTrack().

#include "types.h"
#include "debug.h"
#include "operror.h"
#include "filefunctions.h"
#include "audio.h"
#include "audiox.h"
#include "music.h"

#include "trackplayer.h"

#define INFO_MESSAGES	(0)
#define DEBUG_MESSAGES	(0)
#define ERROR_MESSAGES	(1)

#include "rgm.h"

//************** C O N S T A N T S *****************

#define STACKSIZE		(8192L)

#define SPOOL_VALID_FLAG	(0)
#define SPOOL_LOADED_FLAG	(1)

//************** M A C R O S *****************

#define NOT_ERROR(val) ((val) >= 0L)

//*************** T Y P E S *******************

typedef enum
{
    SS_SPLICE = 0,
    SS_ADVANCE,
    SS_PAUSE,
    SS_RESUME,
    SS_REWIND,
    SS_VOLUME,
    SS_SET_LINEAR_MODE,
    SS_SET_LOOP_MODE,
    SS_FAST_FORWARD,
    SS_FAST_REVERSE,
    SS_STOP,
    SS_JUMP
} ss_command_type;

typedef union
{
	struct
	{
		int32	arg1;
		int32	arg2;
	} nargs;
	void	*parg1;
} ss_arg_type;

typedef struct
{
	int32			ssd_flags;
	void			*ssd_ss;
	int32			ssd_buffer_count;
	int32			ssd_buffer_size;
	int32			ssd_buffer_signals;
	int32			ssd_volume;
	Item			ssd_parent_task_item;
	int32			ssd_parent_signal_bit;
	Item			ssd_child_task_item;
	int32			ssd_child_signal_bit;
	void			*ssd_pre_allocated_buffer;
	Item			ssd_mixer_ins;
	ss_command_type		ssd_command;
	ss_arg_type		ssd_data;
	int32			ssd_error_code;
	int32			ssd_waiting;
	int32			ssd_blocking;
} SoundSpoolDataType;

typedef void (*SS_CommandFnPtrType) (ss_arg_type *);

//****** L O C A L   F U N C T I O N   H E A D E R S  ***********

static void sound_spool_send_command (int);
static void thread_soundspool_main (void);
static void thread_soundspool_process_commands (void);

static void ss_fnsplice(ss_arg_type *arg);
static void ss_fnadvance(ss_arg_type *arg);
static void ss_fnpause(ss_arg_type *arg);
static void ss_fnresume(ss_arg_type *arg);
static void ss_fnrewind(ss_arg_type *arg);
static void ss_fnvolume(ss_arg_type *arg);
static void ss_fnsetlinearmode(ss_arg_type *arg);
static void ss_fnsetloopmode(ss_arg_type *arg);
static void ss_fastforward(ss_arg_type *arg);
static void ss_fastreverse(ss_arg_type *arg);
static void ss_fnstop(ss_arg_type *arg);
static void ss_fnjump(ss_arg_type *arg);

//************* G L O B A L S ******************

static SS_CommandFnPtrType	SS_CommandFunctions[] = {
	ss_fnsplice,
	ss_fnadvance,
	ss_fnpause,
	ss_fnresume,
	ss_fnrewind,
	ss_fnvolume,
	ss_fnsetlinearmode,
	ss_fnsetloopmode,
	ss_fastforward,
	ss_fastreverse,
	ss_fnstop,
	ss_fnjump,
// add more functions here, add command types in enum ss_command_type, add glue fns to send new commands
};

SoundSpoolDataType	global_sound_spool	= {0, NULL, };			// mark as invalid

//******* R O U T I N E S **********

int32	OpenTrackPlayer(
	int32 priority,
	int32 buffer_count,
	int32 buffer_size,
	void *buffer_to_use,
	Item MixerIns
)
{
	if (OpenAudioFolio()) goto ABORT;

	global_sound_spool.ssd_mixer_ins = MixerIns;
	
	global_sound_spool.ssd_pre_allocated_buffer	= buffer_to_use;

	global_sound_spool.ssd_buffer_count			= buffer_count;
	global_sound_spool.ssd_buffer_size			= buffer_size;
	global_sound_spool.ssd_buffer_signals		= 0;

	global_sound_spool.ssd_volume				= 0;

	global_sound_spool.ssd_waiting = 0;
	
	global_sound_spool.ssd_parent_task_item		= KernelBase->kb_CurrentTask->t.n_Item;
	global_sound_spool.ssd_parent_signal_bit	= AllocSignal (0);

	if (! buffer_to_use)
	{
		global_sound_spool.ssd_child_task_item		=
			CreateThread("thread_player", (uint8) priority, thread_soundspool_main, STACKSIZE);
	}
	else
	{
		global_sound_spool.ssd_child_task_item		=
			CreateThread("thread_player", (uint8) priority-30, thread_soundspool_main, STACKSIZE);
		ControlMem(buffer_to_use, buffer_size, MEMC_OKWRITE, global_sound_spool.ssd_child_task_item);
		SetItemPri(global_sound_spool.ssd_child_task_item, (uint8)priority);
	}

	WaitSignal(global_sound_spool.ssd_parent_signal_bit);

	global_sound_spool.ssd_flags				= 0;
	global_sound_spool.ssd_error_code			= 0;
	setflg(SPOOL_VALID_FLAG, global_sound_spool.ssd_flags);

	return (TRUE);
ABORT:
	return (FALSE);
}

int32	LinearTrackPlayer (void) {
	global_sound_spool.ssd_command		= SS_SET_LINEAR_MODE;

	sound_spool_send_command (1);

	return (global_sound_spool.ssd_error_code);
}

int32	LoopedTrackPlayer (void) {
	global_sound_spool.ssd_command		= SS_SET_LOOP_MODE;

	sound_spool_send_command (1);

	return (global_sound_spool.ssd_error_code);
}

int32	SpliceTrack(char *filespec) {
	global_sound_spool.ssd_command		= SS_SPLICE;
	global_sound_spool.ssd_data.parg1	= filespec;

	sound_spool_send_command (1);

	return (global_sound_spool.ssd_error_code);
}

int32	AdvanceTrack(int32 left_volume, int32 right_volume) {
	global_sound_spool.ssd_command			= SS_ADVANCE;
	global_sound_spool.ssd_data.nargs.arg1	= left_volume;
	global_sound_spool.ssd_data.nargs.arg2	= right_volume;

	sound_spool_send_command (1);

	return (global_sound_spool.ssd_error_code);
}

int32	JumpTrack() {
	global_sound_spool.ssd_command			= SS_JUMP;

	sound_spool_send_command (0);

	return (global_sound_spool.ssd_error_code);
}

int32	FastForward(void) {
	global_sound_spool.ssd_command		= SS_FAST_FORWARD;

	sound_spool_send_command (0);

	return (global_sound_spool.ssd_error_code);
}

int32	FastReverse(void) {
	global_sound_spool.ssd_command		= SS_FAST_REVERSE;

	sound_spool_send_command (0);

	return (global_sound_spool.ssd_error_code);
}

int32	PauseTrack(void) {
	global_sound_spool.ssd_command		= SS_PAUSE;

	sound_spool_send_command (0);

	return (global_sound_spool.ssd_error_code);
}

int32	StopTrack(void) {
	global_sound_spool.ssd_command		= SS_STOP;

	sound_spool_send_command (1);

	return (global_sound_spool.ssd_error_code);
}

int32	ResumeTrack(void){
	global_sound_spool.ssd_command		= SS_RESUME;

	sound_spool_send_command (0);

	return (global_sound_spool.ssd_error_code);
}

int32	RewindTrack(void){
	global_sound_spool.ssd_command		= SS_REWIND;

	sound_spool_send_command (1);

	return (global_sound_spool.ssd_error_code);
}


int32	VolumeTrack(int32 left_volume, int32 right_volume) {
	global_sound_spool.ssd_command			= SS_VOLUME;
	global_sound_spool.ssd_data.nargs.arg1	= left_volume;
	global_sound_spool.ssd_data.nargs.arg2	= right_volume;

	sound_spool_send_command (1);

	return (global_sound_spool.ssd_error_code);
}

int32
PlayingTrack ()
{
    if (!global_sound_spool.ssd_ss)
	return (0);
	
    return (SpooledAudioTrackPlaying (global_sound_spool.ssd_ss));
}

int32	GetSoundData (char *data) {
    if (!global_sound_spool.ssd_ss)
	return (0);
	
    return (SpooledAudioData (global_sound_spool.ssd_ss, data));
}

int32	GetSoundInfo (int32 *len, int32 *offs) {
    if (!global_sound_spool.ssd_ss)
	return (0);

    while (global_sound_spool.ssd_blocking) Yield ();	
    return (SpooledAudioInfo (global_sound_spool.ssd_ss, len, offs));
}

void	CloseTrackPlayer(void) {
	if (chkflg(SPOOL_VALID_FLAG, global_sound_spool.ssd_flags)) {
		PauseTrack ();

		CloseSpooledAudio (global_sound_spool.ssd_ss);
		DeleteThread (global_sound_spool.ssd_child_task_item);
		FreeSignal (global_sound_spool.ssd_parent_signal_bit);
		clrflg(SPOOL_VALID_FLAG, global_sound_spool.ssd_flags);
		CloseAudioFolio();
	}
}

static void sound_spool_send_command (int wait_for_sig)
{

	if (global_sound_spool.ssd_waiting)
	{
	    WaitSignal(global_sound_spool.ssd_parent_signal_bit);
	    global_sound_spool.ssd_waiting = 0;
	}
	
	if (chkflg(SPOOL_VALID_FLAG, global_sound_spool.ssd_flags))
	{
		SendSignal(global_sound_spool.ssd_child_task_item, global_sound_spool.ssd_child_signal_bit);
		if (!wait_for_sig)
			global_sound_spool.ssd_waiting = 1;
		else
			WaitSignal(global_sound_spool.ssd_parent_signal_bit);
	}
}

static void thread_soundspool_main (void)
{
	if (! OpenAudioFolio())
	{

		ChangeDirectory ("$boot");

		global_sound_spool.ssd_ss	=
				OpenSpooledAudio(
					global_sound_spool.ssd_buffer_count,
					global_sound_spool.ssd_buffer_size,
					global_sound_spool.ssd_pre_allocated_buffer,
					global_sound_spool.ssd_mixer_ins
				);

		global_sound_spool.ssd_child_signal_bit	= AllocSignal (0);

		SendSignal(global_sound_spool.ssd_parent_task_item, global_sound_spool.ssd_parent_signal_bit);

		thread_soundspool_process_commands();
	}
	CloseAudioFolio();
	WaitSignal(0);   /* Waits forever. Don't return! */
}

static void thread_soundspool_process_commands (void)
{
	while (1)
	{
		register int32	signals;

		signals = WaitSignal (global_sound_spool.ssd_child_signal_bit
			| global_sound_spool.ssd_buffer_signals);

global_sound_spool.ssd_blocking = 1;
		if (signals & global_sound_spool.ssd_child_signal_bit)
		{
			SS_CommandFunctions[global_sound_spool.ssd_command](&global_sound_spool.ssd_data);
			SendSignal(global_sound_spool.ssd_parent_task_item, global_sound_spool.ssd_parent_signal_bit);
			signals &= ~global_sound_spool.ssd_child_signal_bit;
		}

		ServiceSpooledAudio (
			global_sound_spool.ssd_ss, signals,
			&global_sound_spool.ssd_buffer_signals
			);
global_sound_spool.ssd_blocking = 0;
	}
}

static void ss_fnsplice(ss_arg_type *arg) {
	global_sound_spool.ssd_error_code	= SpliceSpooledAudioTrack ((char *) arg->parg1, global_sound_spool.ssd_ss);
	if (NOT_ERROR(global_sound_spool.ssd_error_code)) {
		setflg(SPOOL_LOADED_FLAG, global_sound_spool.ssd_flags);
	}

	global_sound_spool.ssd_buffer_signals	= 0;
}

static void ss_fnadvance(ss_arg_type *arg) {
	if (chkflg(SPOOL_LOADED_FLAG, global_sound_spool.ssd_flags)) {

		global_sound_spool.ssd_error_code =
			AdvanceSpooledAudio	(
				global_sound_spool.ssd_ss,
				global_sound_spool.ssd_data.nargs.arg1,
				global_sound_spool.ssd_data.nargs.arg2
			);

		if ((NOT_ERROR(global_sound_spool.ssd_error_code)) && (! global_sound_spool.ssd_buffer_signals)) {
			global_sound_spool.ssd_error_code =
				ServiceSpooledAudio (global_sound_spool.ssd_ss, 0L, &global_sound_spool.ssd_buffer_signals);
		}
	}
	else {
		global_sound_spool.ssd_error_code	= -1;
	}
}

static void ss_fastforward(ss_arg_type *arg) {
	FastForwardSpooledAudio(global_sound_spool.ssd_ss);

	global_sound_spool.ssd_error_code	= 0;
}

static void ss_fastreverse(ss_arg_type *arg) {
	FastReverseSpooledAudio(global_sound_spool.ssd_ss);

	global_sound_spool.ssd_error_code	= 0;
}

static void ss_fnpause(ss_arg_type *arg) {
	PauseSpooledAudio(global_sound_spool.ssd_ss);

	global_sound_spool.ssd_error_code	= 0;
}

static void ss_fnstop(ss_arg_type *arg) {
	StopSpooledAudio(global_sound_spool.ssd_ss);
	clrflg(SPOOL_LOADED_FLAG, global_sound_spool.ssd_flags);

	global_sound_spool.ssd_error_code	= 0;
}

static void ss_fnjump(ss_arg_type *arg) {
	JumpSpooledAudio(global_sound_spool.ssd_ss);

	global_sound_spool.ssd_error_code	= 0;
}

static void ss_fnresume(ss_arg_type *arg) {
	if (chkflg(SPOOL_LOADED_FLAG, global_sound_spool.ssd_flags))
		ResumeSpooledAudio(global_sound_spool.ssd_ss);

	global_sound_spool.ssd_error_code	= 0;
}

static void ss_fnrewind(ss_arg_type *arg) {
	RewindSpooledAudio(global_sound_spool.ssd_ss);

	global_sound_spool.ssd_error_code	= 0;
}

static void ss_fnvolume(ss_arg_type *arg) {
	SetVolumeSpooledAudio (
		global_sound_spool.ssd_ss,
		global_sound_spool.ssd_data.nargs.arg1,
		global_sound_spool.ssd_data.nargs.arg2
	);

	global_sound_spool.ssd_error_code	= 0;
}

static void ss_fnsetlinearmode(ss_arg_type *arg) {
	LinearSpooledAudio(global_sound_spool.ssd_ss);
	global_sound_spool.ssd_error_code	= 0;
}

static void ss_fnsetloopmode(ss_arg_type *arg) {
	LoopSpooledAudio(global_sound_spool.ssd_ss);
	global_sound_spool.ssd_error_code	= 0;
}



