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
#include "audiox.h"

#define AUTO_ADVANCE
#define ABX_STREAM_IO
//#define PRE_OPEN_STREAMS
#define PRE_LOAD_BUFFER

//************* C O N S T A N T S ****************

#define DEBUG_MESSAGES		(0)
#define INFO_MESSAGES		(0)
#define ERROR_MESSAGES		(1)

#define DEFAULT_BLOCK_SIZE	(2 * 1024)
#define MAX_LOADED_TRACKS	(30)
#define MIXER_FILESPEC		"mixer2x2.dsp"
#define NUM_CHANNELS		2

#ifdef ABX_STREAM_IO
#define MAX_ABX_BYTES		(8 * DEFAULT_BLOCK_SIZE)
#define MIN_ABX_BYTES		(1024)
#define MAX_TOTAL_ABX_BYTES	(8 * MAX_ABX_BYTES)
#ifndef PRE_OPEN_STREAMS
#define MAX_PRE_LOAD_STREAMS	8
#endif
#else
#define ABX_WORK_SIZE		(8 * DEFAULT_BLOCK_SIZE)
#endif
#define MAX_ABX_SIZE		(4 * 1024) // be careful with this
#define MAX_ABX_FRAMES		(725) // enough for now
#define ABX_FRAME_SIZE		(8)
#define ABX_HEADER_SIZE		(10)

#define HERTZ_44100_16_16	(0xAC440000UL)
#define HERTZ_22050_16_16	(0x56220000UL)

//************ M A C R O S ****************

#include "rgm.h"

#define	SCALE_RATE(r)	((((r) >> 8) * 3) >> 10)

//************* T Y P E S ***************

enum
{
	AS_SA_OK_TO_SPLICE = 0,
	AS_SA_LOOP_ON_FLAG,
	AS_SA_ADVANCE_FLAG,
	AS_SA_REQUEST_STOP_PLAYING_FLAG,
	AS_SA_PRE_ALLOCATED_BUFFER_FLAG,
	AS_SA_FIRST_HAS_BEEN_LOADED_FLAG,
	AS_SA_INVALIDATE_BUFFERS_FLAG,
	AS_SA_ATTACHMENT_STARTED,
	AS_SA_BUFFER_UNDERFLOW,
	AS_SA_OWN_MIXER_INS
};

enum
{
	AS_BF_PARTIAL_FLAG = 0
};

enum
{
	AS_TR_IN_USE_FLAG = 0,
	AS_TR_IS_SPLICE_FLAG,
	AS_TR_DEALLOCATE_MINIMAL_FLAG,
	AS_TR_ABX_SND
};

enum
{
	ASP_STATE_IDLE = 0,
	ASP_STATE_PLAYING,
	ASP_STATE_PAUSED
};

enum
{
	LB_MORE_DATA = 0,
	LB_LAST_DATA,
	LB_NO_MORE_DATA,
	LB_ERROR
};

enum
{
	FORWARD = 0,
	REVERSE
};

#define FILESIZE(s)	(((uint32)(s)>>16)&0xffff)
#define UNCOMPSIZE(s)	((uint32)(s)&0xffff)

typedef struct
{
	uint32		fileaddress;
	uint32		sizes;
} ABX_FRAME;

typedef struct
{
	Item		asb_sample_item;
	Item		asb_attachment_item;
	int32		asb_file_block;
} BUFFER_TRACK_INFO;

typedef struct
{
	int32			asb_flags;
	int32			asb_data_size;
	void			*asb_data;
	Item			asb_cue_item;
	int32			asb_signal;
	int32			asb_track_number;
	BUFFER_TRACK_INFO	asb_track_info[MAX_LOADED_TRACKS];
} SPOOL_BUFFER;

typedef struct
{
	int32		abx_bufsize;
	int32		abx_FrameCt;
	int32		abx_FrameIdx;
	int32		abx_TotSiz;
	int32		abx_FileSize;
} ABX_INFO;

typedef struct
{
	int32		aspss_flags;
	int32		aspss_sample_rate;
	int32		aspss_sample_data_size;
	int32		aspss_sample_data_read;
	int32		aspss_sample_data_remaining;
	Item		aspss_sample_instrument;
	Item		aspss_IOreq;
	Item		aspss_file;
	int32		aspss_next_file_block;
	int32		aspss_cur_file_pos;
	int32		aspss_cur_file_block_offset;
	int32		aspss_start_file_block;
	int32		aspss_start_file_block_offset;
	int32		aspss_block_size;
	ABX_INFO	aspss_abx_info;
#ifndef PRE_OPEN_STREAMS
	char		aspss_filename[40]; // enough for "starcon2/comm/XXXXXXXX/XXXXXXXX.abx"
#endif
} SPOOL_TRACK;

typedef struct
{
	int32		asp_signals;
	int32		asp_state;
	int32		asp_loaded_buffer_signals;
	int32		asp_flags;

	int32		asp_track_count;
	int32		asp_stopped_track_number;
	int32		asp_current_load_track_number;
	int32		asp_current_playing_track_number;
	SPOOL_TRACK	asp_track_info[MAX_LOADED_TRACKS];

	int32		asp_current_load_buffer_index;
	int32		asp_buffer_count;
	int32		asp_buffers_size;
	SPOOL_BUFFER	*asp_buffers_control;

	int32		asp_data_buffer_size;
	void		*asp_data_buffer;

	int32		asp_channels;

	Item		asp_output_instrument;
	Item		asp_left_volume_knob;
	Item		asp_right_volume_knob;
#ifdef DELAY_STUFF
	Item		asp_delay_line, asp_delay_ins, asp_delay_att;
#endif
	ABX_FRAME	asp_abx_FrameList[MAX_ABX_FRAMES];
	char		asp_abx_buf[MAX_ABX_SIZE];
#ifdef ABX_STREAM_IO
	int32		asp_bytes_per_track;
#else
	char		asp_abx_workbuf[ABX_WORK_SIZE];
#endif
} SPOOL;

typedef struct
{
	uint32		afi_sample_rate;
	int32		afi_channels;
	int32		afi_width;
	int32		afi_compression_type;

	int32		afi_num_bits;
	int32		afi_compression_ratio;

	int32		afi_data_size;
	int32		afi_data_offset;
	char		*afi_dsp_filename;
} SAMPLE_FILE_INFO;

#define	ABX_FRAME_LIST	(spool->asp_abx_FrameList)

//*********** L O C A L  R O U T I N E  H E A D E R S ***********

static int32	rewind_track (SPOOL_TRACK *track, SPOOL *spool);
static int32	transfer_to_next_track (int32 load_signals, SPOOL *spool);
static int32	set_track_volume (SPOOL *spool, int32 left_volume, int32 right_volume,
			int32 track_index);
static int32	ReadDiskFileBlocks (SPOOL *spool, SPOOL_TRACK *track, void *buffer, int32 blocks);
static Item	myCreateSample (SAMPLE_FILE_INFO *aiff_info, int32 buffer_size, void *buffer);
static void	link_buffers (SPOOL *spool, int32 track);
static void	unlink_buffers (SPOOL *spool, int32 track_index);
static int32	load_track (char *filename, SPOOL *spool);
static int32	splice_track (char *filename, SPOOL *spool);
static void	unload_track (int32 track, SPOOL *spool);
static int32	load_sample_buffers (int32 load_signals, int32 track,
			SPOOL *spool, int32 *signals_to_wait_for);
static int32	fill_sample_buffer (int32 track_number, int32 buffer_number, SPOOL *spool);
static void	resize_sample_buffer_mem (SPOOL *spool, SPOOL_BUFFER *buffer, int32 start, int32 len, int32 track_index);
static char	*optimal_dsp_filename (uint32 sample_rate_16_16, int32 channels,
			int32 sample_byte_width, int32 compression_type);
static int32	get_sample_info (SPOOL *spool, SPOOL_TRACK *track, char *filename,
			SAMPLE_FILE_INFO *aiff_file_info);
static int32	set_attachment_flags (Item attachment, int32 flags);
static int32	stop_currently_playing_attachments (SPOOL *spool);
static int32	load_abx_info (SPOOL *spool, Item ioreq, ABX_INFO *abx_info, int32 *freq);
static void	tweak_file (SPOOL *spool, int32 dir);
static int32	adjust_track (SPOOL *spool, SPOOL_TRACK *track);
static void	toggle_connections (SPOOL *spool, SPOOL_TRACK *track, int connect);

//************* G L O B A L  R O U T I N E S ******************

void *
OpenSpooledAudio (int32 buffer_count, int32 buffer_size, void *pre_allocated_buffer, Item MixerIns)
{
	int32	total_buffer_size;
	SPOOL	*spool;
	int	i;

	OpenAudioFolio ();
	
	spool = (SPOOL	*) USER_ALLOCMEM (sizeof (SPOOL), MEMTYPE_FILL | 0);
	ON_NULL_ABORT(spool, "Couldn't allocate spool player structure\n");

	// allocate the huge data buffer

	buffer_size = ((buffer_size + DEFAULT_BLOCK_SIZE - 1) 
		/ DEFAULT_BLOCK_SIZE) * DEFAULT_BLOCK_SIZE;
	total_buffer_size = buffer_size * buffer_count;
	spool->asp_data_buffer_size = total_buffer_size;

	if (pre_allocated_buffer)
	{
		spool->asp_data_buffer = pre_allocated_buffer;
		setflg (AS_SA_PRE_ALLOCATED_BUFFER_FLAG, spool->asp_flags);
	}
	else
	{
		spool->asp_data_buffer = USER_ALLOCMEM (total_buffer_size, MEMTYPE_AUDIO);
	}
	ON_NULL_ABORT(spool->asp_data_buffer,"Couldn't allocate data buffer for samples\n");

	if (MixerIns < 0)
	{
		spool->asp_output_instrument = LoadInstrument (MIXER_FILESPEC, 0, 0);
		ON_NOT_ITEM_ABORT(spool->asp_output_instrument, "couldn't load mixer dsp file\n");
		setflg (AS_SA_OWN_MIXER_INS, spool->asp_flags);
	}
	else
	{
		spool->asp_output_instrument = MixerIns;
		clrflg (AS_SA_OWN_MIXER_INS, spool->asp_flags);
	}
	
	// get the knobs
	for (i = 0; i < NUM_CHANNELS; i++)
	{
		Item	left_volume_knob;
		Item	right_volume_knob;
		char	buf[20];
		
		sprintf (buf, "LeftGain%d", i);
		left_volume_knob = GrabKnob (spool->asp_output_instrument, buf);
		ON_NEG_ABORT(left_volume_knob,
			"couldn't grab left volume\n");
		ON_NEG_ABORT(TweakKnob (left_volume_knob, 0x0000),
			"couldn't turn off unwanted left volume\n");
			
		sprintf (buf, "RightGain%d", i);
		right_volume_knob = GrabKnob (spool->asp_output_instrument, buf);
		ON_NEG_ABORT(right_volume_knob,
			"couldn't grab right volume\n");
		ON_NEG_ABORT(TweakKnob (right_volume_knob, 0x0000),
			"couldn't turn off unwanted right volume\n");
			
		switch (i)
		{
		    case 0:
			spool->asp_left_volume_knob = left_volume_knob;
			ON_NEG_ABORT(ReleaseKnob (right_volume_knob), 
				"couldn't release unwanted right volume\n");
			break;
		    case 1:
			ON_NEG_ABORT(ReleaseKnob (left_volume_knob), 
				"couldn't release unwanted left volume\n");
			spool->asp_right_volume_knob = right_volume_knob;
			break;
		    default:
			ON_NEG_ABORT(ReleaseKnob (left_volume_knob), 
				"couldn't release unwanted left volume\n");
			ON_NEG_ABORT(ReleaseKnob (right_volume_knob), 
				"couldn't release unwanted right volume\n");
			break;
		}
	}
	
	// allocate sound buffer control structures
	spool->asp_buffer_count = buffer_count;
	spool->asp_buffers_size = buffer_size;
	spool->asp_buffers_control = 
		(SPOOL_BUFFER *) USER_ALLOCMEM (
			buffer_count*sizeof (SPOOL_BUFFER),
			MEMTYPE_FILL | 0);
	ON_NULL_ABORT(spool->asp_buffers_control, "Couldn't allocate buffer control structures\n");

#ifdef DELAY_STUFF
	{
	    TagArg	Tags[2];
    
	    spool->asp_delay_line = CreateDelayLine (256 * 2, 1, 1);
	    spool->asp_delay_ins = LoadInstrument ("delaymono.dsp", 0, 0);
	    spool->asp_delay_att = AttachSample (
		    spool->asp_delay_ins,
		    spool->asp_delay_line,
		    "OutFIFO");
	    Tags[0].ta_Tag = AF_TAG_START_AT;
	    Tags[0].ta_Arg = (void *)0;
	    Tags[1].ta_Tag = TAG_END;
	    SetAudioItemInfo (spool->asp_delay_att, Tags);
	}
#endif

	// partition off the buffers and allocate cue items and signals for the buffers
	{
		int 		i;
		int32		buffer_signals = 0L;
		char		*data_buffer = (char *) spool->asp_data_buffer;
		SPOOL_BUFFER	*buffer_control = spool->asp_buffers_control;
		
		for (i = 0; i < buffer_count; i++)
		{
			buffer_control->asb_data_size = 0L;
			buffer_control->asb_data = data_buffer;
			buffer_control->asb_cue_item = CreateItem (MKNODEID(AUDIONODE, AUDIO_CUE_NODE), NULL);
			ON_NOT_ITEM_ABORT(buffer_control->asb_cue_item, 
				"Couldn't allocate cue item for buffer\n");
			buffer_control->asb_signal = GetCueSignal (buffer_control->asb_cue_item);
			ON_NULL_ABORT(buffer_control->asb_signal,
				"Couldn't get signal from buffer cue\n");

			buffer_signals |= buffer_control->asb_signal;
			
			data_buffer += buffer_size;
			buffer_control	++;
		}
		spool->asp_signals = buffer_signals;
	}

	// return the signals

	// mark the samples as unused

	{
		int		i, j;
		SPOOL_TRACK	*spool_track = spool->asp_track_info;
		SPOOL_BUFFER	*buffers_control = spool->asp_buffers_control;

		for (i = 0; i < MAX_LOADED_TRACKS; i ++)
		{
			spool_track->aspss_sample_instrument =
				spool_track->aspss_file = 
				spool_track->aspss_IOreq = NULL_ITEM;

			for (j = 0; j < buffer_count; j++)
			{
				BUFFER_TRACK_INFO	*track_info;
				
				track_info = &(buffers_control[j].asb_track_info[i]);
				track_info->asb_sample_item = 
					track_info->asb_attachment_item = NULL_ITEM;
			}
			spool_track++;
		}
	}

	return (spool);

ABORT:
	CloseSpooledAudio (spool);
	return (NULL);
}


int32
LoopSpooledAudio (void *spool_player)
{
	SPOOL	*spool = (SPOOL	*) spool_player;

	setflg(AS_SA_LOOP_ON_FLAG, spool->asp_flags);

	return(0L);
}

int32
LinearSpooledAudio (void *spool_player)
{
	SPOOL	*spool = (SPOOL	*) spool_player;

	clrflg(AS_SA_LOOP_ON_FLAG, spool->asp_flags);

	return(0L);
}

void
CloseSpooledAudio (void *spool_player)
{
	SPOOL	*spool = (SPOOL	*) spool_player;

	if (spool)
	{
		int	i;

		// de-allocate the cue items and signals for the buffers and kill the buffer ctrl memory
		if (spool->asp_buffers_control)
		{
			SPOOL_BUFFER	*buffer_control = spool->asp_buffers_control;

			for (i = 0; i < spool->asp_buffer_count; i++)
			{
				if (IS_ITEM(buffer_control->asb_cue_item))
					DeleteItem(buffer_control->asb_cue_item);
				buffer_control	++;
			}
			USER_FREEMEM (spool->asp_buffers_control,
				spool->asp_buffer_count*sizeof (SPOOL_BUFFER));
		}

		// get rid of our sample data buffer

		if (! (chkflg (AS_SA_PRE_ALLOCATED_BUFFER_FLAG, spool->asp_flags)))
		{
			if (spool->asp_data_buffer)
				USER_FREEMEM(spool->asp_data_buffer, spool->asp_data_buffer_size);
		}

#ifdef DELAY_STUFF
		if (IS_ITEM (spool->asp_delay_att))
			DetachSample (spool->asp_delay_att);
		if (IS_ITEM (spool->asp_delay_ins))
			UnloadInstrument (spool->asp_delay_ins);
		if (IS_ITEM (spool->asp_delay_line))
			DeleteDelayLine (spool->asp_delay_line);
#endif

		if (IS_ITEM(spool->asp_left_volume_knob))
			ReleaseKnob (spool->asp_left_volume_knob);
		if (IS_ITEM(spool->asp_right_volume_knob))
			ReleaseKnob (spool->asp_right_volume_knob);
		if (chkflg (AS_SA_OWN_MIXER_INS, spool->asp_flags)
				&& IS_ITEM(spool->asp_output_instrument))
			UnloadInstrument (spool->asp_output_instrument);

		// get rid of the structure itself
		USER_FREEMEM(spool, sizeof (SPOOL));
	}

	CloseAudioFolio();

	return;
}

int32
LoadSpooledAudioTrack (char *filename, void *spool_player)
{
	SPOOL		*spool = (SPOOL	*) spool_player;
	int32		current_load_track = spool->asp_current_load_track_number;
	SPOOL_TRACK	*current_track = &(spool->asp_track_info[current_load_track]);

	ON_TRUE_ABORT(chkflg(AS_TR_IN_USE_FLAG,current_track->aspss_flags),
		"Track in use in LoadSpooledAudioTrack()\n");
	if (load_track (filename, spool))
	    return (0L);
ABORT:
	return (-1L);
}

int32
SpliceSpooledAudioTrack (char *filename, void *spool_player)
{
	SPOOL		*spool = (SPOOL	*) spool_player;
	int32		current_load_track = spool->asp_current_load_track_number;
	SPOOL_TRACK	*current_track = &(spool->asp_track_info[current_load_track]);

	if (chkflg(AS_SA_OK_TO_SPLICE, spool->asp_flags))
	{
		ON_TRUE_ABORT(chkflg(AS_TR_IN_USE_FLAG,current_track->aspss_flags),
			"Track in use in SpliceSpooledAudioTrack()\n");
		splice_track (filename, spool);
	}
	else
	{
		ON_NEG_ABORT(LoadSpooledAudioTrack (filename, spool), "\n");
	}

	return (0L);
ABORT:
	return (-1L);
}

int32
PlaySpooledAudio (void *spool_player, int32 left_volume, int32 right_volume)
{
	SPOOL	*spool = (SPOOL *) spool_player;
	int32	track_index = spool->asp_current_playing_track_number;

	//** Link the buffers

	link_buffers(spool, track_index);

	if (spool->asp_state != ASP_STATE_PAUSED)
	{
		ON_FALSE_ABORT(ResumeSpooledAudio(spool),
			"Couldn't start sample in PlaySpooledAudio()\n");
	}

	//** Set the volume

	set_track_volume (spool, left_volume, right_volume, track_index);

	setflg (AS_SA_FIRST_HAS_BEEN_LOADED_FLAG, spool->asp_flags);

	return (TRUE);
ABORT:
	return (FALSE);
}

int32
AdvanceSpooledAudio (void *spool_player, int32 left_volume, int32 right_volume)
{
	SPOOL	*spool = (SPOOL *) spool_player;
	int32	track_index = spool->asp_current_playing_track_number;

	if (chkflg (AS_SA_FIRST_HAS_BEEN_LOADED_FLAG, spool->asp_flags))
	{
		int32	load_signals;

		load_signals = spool->asp_signals & ~spool->asp_loaded_buffer_signals;
		setflg(AS_SA_ADVANCE_FLAG, spool->asp_flags);
		transfer_to_next_track (load_signals, spool);
if (spool->asp_current_playing_track_number + 1 < spool->asp_track_count)
{
	SPOOL_TRACK	*track;
	
	track = &spool->asp_track_info[track_index];
	track->aspss_cur_file_pos = track->aspss_sample_data_size;
	spool->asp_current_playing_track_number++;
}
		set_track_volume (spool, left_volume, right_volume, track_index);
	}
	else
	{
#if defined(ABX_STREAM_IO) && !defined(PRE_OPEN_STREAMS)
		spool->asp_bytes_per_track = MAX_TOTAL_ABX_BYTES / spool->asp_track_count;
		if (spool->asp_bytes_per_track > MAX_ABX_BYTES)
			spool->asp_bytes_per_track = MAX_ABX_BYTES;
		else if (spool->asp_bytes_per_track < MIN_ABX_BYTES)
			ON_ABORT ("bytes_per_track too small in Advance\n");
#endif
		return (PlaySpooledAudio (spool, left_volume, right_volume));
	}

	return (TRUE);
ABORT:
	return (FALSE);
}

int32
SetVolumeSpooledAudio (void *spool_player, int32 left_volume, int32 right_volume)
{
	int32	track_index;

	track_index = ((SPOOL *) spool_player)->asp_current_playing_track_number;

	set_track_volume ((SPOOL *) spool_player, left_volume, right_volume, track_index);

	return (TRUE);
}

void
JumpSpooledAudio (void *spool_player)
{
    SPOOL	*spool;
    int32	i;
    
    spool = (SPOOL *)spool_player;
    if (!spool->asp_track_count)
	return;
	
    if (spool->asp_state == ASP_STATE_PLAYING)
    {
	PauseSpooledAudio (spool);
    }

    for (i = spool->asp_current_playing_track_number; i < spool->asp_track_count; i++)
    {
   	SPOOL_TRACK	*track;
	
	track = &spool->asp_track_info[i];
	track->aspss_cur_file_pos = track->aspss_sample_data_size;
    }
    
    spool->asp_current_playing_track_number = spool->asp_track_count - 1;
    spool->asp_stopped_track_number = -1;
}

void
FastForwardSpooledAudio (void *spool_player)
{
    SPOOL		*spool;
    
    spool = (SPOOL *)spool_player;
    if (!spool->asp_track_count)
	return;
	
    if (spool->asp_state == ASP_STATE_IDLE)
	spool->asp_state = ASP_STATE_PAUSED;
    else if (spool->asp_state == ASP_STATE_PLAYING)
    {
	PauseSpooledAudio (spool);
    }
	
    tweak_file (spool, FORWARD);
}

void
FastReverseSpooledAudio (void *spool_player)
{
    SPOOL		*spool;
    
    spool = (SPOOL *)spool_player;
    if (!spool->asp_track_count)
	return;
	
    if (spool->asp_state == ASP_STATE_IDLE)
	spool->asp_state = ASP_STATE_PAUSED;
    else if (spool->asp_state == ASP_STATE_PLAYING)
    {
	PauseSpooledAudio (spool);
    }
	
    tweak_file (spool, REVERSE);
}

int32
StopSpooledAudio (void *spool_player)
{
    int32	i;
    SPOOL	*spool;

    spool = (SPOOL *) spool_player;
    
    PauseSpooledAudio (spool);
    
    for (i = 0; i < spool->asp_track_count; i++)
	unload_track (i, spool);
	
    spool->asp_track_count = 0;
    spool->asp_current_load_track_number = 0;
    spool->asp_current_playing_track_number = 0;
    spool->asp_state = ASP_STATE_IDLE;
    
    clrflg (AS_SA_OK_TO_SPLICE, spool->asp_flags);
    clrflg (AS_SA_FIRST_HAS_BEEN_LOADED_FLAG, spool->asp_flags);
    
    return (TRUE);
}

int32
PauseSpooledAudio (void *spool_player)
{
	SPOOL	*spool;
	Item	sample_instrument;

	spool = (SPOOL *) spool_player;
	// in case Service wants us to stop, but we have other plans (rewind, fast forward, etc.)
	clrflg (AS_SA_REQUEST_STOP_PLAYING_FLAG, spool->asp_flags);
	clrflg (AS_SA_BUFFER_UNDERFLOW, spool->asp_flags);
	if (spool->asp_state == ASP_STATE_PLAYING)
	{
		ON_FALSE_ABORT(stop_currently_playing_attachments(spool),
			"Couldn't stop buffer attachment item in PauseSpooledAudio()\n");

		sample_instrument = spool->asp_track_info[spool->asp_current_playing_track_number].aspss_sample_instrument;
		ON_NEG_ABORT(ReleaseInstrument(sample_instrument, NULL),
			"Couldn't release sample instrument in PauseSpooledAudio()\n");
		ON_NEG_ABORT(StopInstrument(sample_instrument, NULL),
			"Couldn't stop sample instrument in PauseSpooledAudio()\n");
		if (chkflg (AS_SA_OWN_MIXER_INS, spool->asp_flags))
		{
			ON_NEG_ABORT(ReleaseInstrument (spool->asp_output_instrument, NULL),
				"Couldn't release output instrument in PauseSpooledAudio()\n");
			ON_NEG_ABORT(StopInstrument (spool->asp_output_instrument, NULL),
				"Couldn't stop output instrument in PauseSpooledAudio()\n");
		}
		else
		    StopSound ();
#ifdef DELAY_STUFF
		ON_NEG_ABORT(ReleaseInstrument (spool->asp_delay_ins, NULL),
			"Couldn't release delay instrument in PauseSpooledAudio()\n");
		ON_NEG_ABORT(StopInstrument (spool->asp_delay_ins, NULL),
			"Couldn't stop delay instrument in PauseSpooledAudio()\n");
#endif

		setflg (AS_SA_INVALIDATE_BUFFERS_FLAG, spool->asp_flags);
		spool->asp_current_load_buffer_index = 0;

		spool->asp_state = ASP_STATE_PAUSED;
	}
	else if (spool->asp_state == ASP_STATE_IDLE)
	{
		spool->asp_stopped_track_number = -1;
		spool->asp_state = ASP_STATE_PAUSED;
		clrflg (AS_SA_INVALIDATE_BUFFERS_FLAG, spool->asp_flags);
	}

	spool->asp_loaded_buffer_signals = 0;

	return (TRUE);
ABORT:
	spool->asp_loaded_buffer_signals = 0;
	return (FALSE);
}

int32
SpooledAudioTrackPlaying (void *spool_player)
{
    SPOOL	*spool;
    
    spool = (SPOOL *)spool_player;
    if (spool->asp_state == ASP_STATE_IDLE)
    {
	return (0);
    }
    else if (spool->asp_state == ASP_STATE_PLAYING)
    {
	int32	buffer_index;
	
	for (buffer_index = 0; buffer_index < spool->asp_buffer_count; buffer_index++)
	{
	    Item		att;
	    Err			offset;
	    int32		buf_track;
	    BUFFER_TRACK_INFO	*buf_track_info;
	    SPOOL_BUFFER	*buffer;

	    buffer = &spool->asp_buffers_control[buffer_index];
	    if (!(buffer->asb_signal & spool->asp_loaded_buffer_signals))
		continue;

	    buf_track = buffer->asb_track_number;
	    buf_track_info = &buffer->asb_track_info[buf_track];
	    att = buf_track_info->asb_attachment_item;

	    if ((offset = WhereAttachment (att)) >= 0
		    && offset <= buffer->asb_data_size)
	    {
		return (buf_track + 1);
	    }
	}
    }

    return (spool->asp_current_playing_track_number + 1);
}

int32
ResumeSpooledAudio (void *spool_player)
{
	SPOOL		*spool;
	Item		first_buffer_attachment;
	Item		sample_instrument;
	SPOOL_TRACK	*track;

	spool = (SPOOL *) spool_player;

	track = &spool->asp_track_info[spool->asp_current_playing_track_number];
	if (spool->asp_state == ASP_STATE_PAUSED)
	{
		if (spool->asp_stopped_track_number != -1
				&& spool->asp_stopped_track_number != spool->asp_current_playing_track_number)
		{
			spool->asp_current_playing_track_number = spool->asp_stopped_track_number;
			track = &spool->asp_track_info[spool->asp_current_playing_track_number];
		}
//printf ("Resume: track %ld has cur_file_pos %ld\n", spool->asp_current_playing_track_number, track->aspss_cur_file_pos);

		if (!adjust_track (spool, track))
			ON_ABORT ("couldn't adjust_track in Resume\n");
	}
#ifndef PRE_LOAD_BUFFER
	else if (spool->asp_state == ASP_STATE_IDLE)
	{
		// make sure this track's info is loaded into global ABX info
		if (chkflg (AS_TR_ABX_SND, track->aspss_flags))
		{
			track->aspss_cur_file_pos = 0;
			if (!adjust_track (spool, track))
				ON_ABORT ("couldn't adjust_track in Resume\n");
		}
	}
#else
	else if (spool->asp_state == ASP_STATE_IDLE
			&& spool->asp_track_count > 1)
	{
		// make sure current track's info is loaded in global ABX stuff
		if (track->aspss_IOreq == 0)
		{
			track->aspss_IOreq = (Item)OpenDiskStream (
				track->aspss_filename,
				spool->asp_bytes_per_track
				);
			ON_NULL_ABORT (track->aspss_IOreq,
				"Couldn't OpenDiskStream in fill_sample_buffer\n");
		}
		
		ON_FALSE_ABORT (load_abx_info (spool, track->aspss_IOreq, 0, 0),
			"Couldn't load_abx_info in Resume\n");
	}
#endif

	if (spool->asp_state != ASP_STATE_PLAYING)
	{
int32	transfer;

transfer = 0;
		if (chkflg (AS_SA_INVALIDATE_BUFFERS_FLAG, spool->asp_flags))
		{
			int32	ret;

			ret = load_sample_buffers (
#ifndef PRE_LOAD_BUFFER
				spool->asp_signals,
#else
				spool->asp_signals & ~spool->asp_loaded_buffer_signals,
#endif
				spool->asp_current_playing_track_number,
				spool,
				NULL
				);
				
			if (ret == LB_ERROR)
				goto ABORT;
			else if (ret != LB_MORE_DATA)
			{
				int32	load_signals;
				int32	prev_state;

				prev_state = spool->asp_state;
				load_signals = spool->asp_signals & ~spool->asp_loaded_buffer_signals;
				spool->asp_state = ASP_STATE_PLAYING;
#ifdef AUTO_ADVANCE
				if (spool->asp_current_playing_track_number + 1 < spool->asp_track_count
						|| (chkflg (AS_SA_LOOP_ON_FLAG, spool->asp_flags)))
#else
				if (chkflg (AS_SA_LOOP_ON_FLAG, spool->asp_flags))
#endif
				{
					transfer_to_next_track (load_signals, spool);
if (!chkflg (AS_SA_LOOP_ON_FLAG, spool->asp_flags))
	transfer = 1;
				}
				else if (spool->asp_loaded_buffer_signals &&
						spool->asp_loaded_buffer_signals != spool->asp_signals)
				{
					// loaded 1 (not 2) buffers
					if (prev_state == ASP_STATE_IDLE
							&& spool->asp_track_count == 1)
						setflg (AS_SA_BUFFER_UNDERFLOW, spool->asp_flags);
					else if (prev_state == ASP_STATE_PAUSED
							&& spool->asp_track_count - 1 == spool->asp_current_playing_track_number)
						setflg (AS_SA_BUFFER_UNDERFLOW, spool->asp_flags);
					if (chkflg (AS_SA_BUFFER_UNDERFLOW, spool->asp_flags))
						unlink_buffers (spool, spool->asp_current_playing_track_number);
				}
			}
			clrflg (AS_SA_INVALIDATE_BUFFERS_FLAG, spool->asp_flags);
		}

		if (spool->asp_loaded_buffer_signals)
		{
#define HERTZ_22050_SCALED	(SCALE_RATE (HERTZ_22050_16_16))
#define HERTZ_44100_SCALED	(SCALE_RATE (HERTZ_44100_16_16))
			SPOOL_TRACK	*track;
    
			sample_instrument = spool->asp_track_info[spool->asp_current_playing_track_number].aspss_sample_instrument;

			track = &spool->asp_track_info[spool->asp_current_playing_track_number];
			if (track->aspss_sample_rate != HERTZ_22050_SCALED
					&& track->aspss_sample_rate != HERTZ_44100_SCALED)
			{
				Item	knob;

				knob = GrabKnob (sample_instrument, "Frequency");
				TweakKnob (knob, track->aspss_sample_rate);
				ReleaseKnob (knob);
			}

#ifdef DELAY_STUFF
			ON_NEG_ABORT(StartInstrument(spool->asp_delay_ins, NULL),
				"Couldn't start delay instrument in ResumeSpooledAudio()\n");
#endif
			ON_NEG_ABORT(StartInstrument(sample_instrument, NULL),
				"Couldn't start sample instrument in ResumeSpooledAudio()\n");
			if (chkflg (AS_SA_OWN_MIXER_INS, spool->asp_flags))
			{
				ON_NEG_ABORT(StartInstrument (spool->asp_output_instrument, NULL),
					"Couldn't start output instrument in ResumeSpooledAudio()\n");
			}

			first_buffer_attachment = spool->asp_buffers_control[0].asb_track_info[spool->asp_current_playing_track_number].asb_attachment_item;
			ON_NEG_ABORT(StartAttachment(first_buffer_attachment, NULL),
				"Couldn't start buffer attachment item in ResumeSpooledAudio()\n");
			setflg (AS_SA_ATTACHMENT_STARTED, spool->asp_flags);
if (transfer)
	spool->asp_current_playing_track_number++;

			spool->asp_state = ASP_STATE_PLAYING;
		}
		else
		{
			setflg (AS_SA_INVALIDATE_BUFFERS_FLAG, spool->asp_flags);
			spool->asp_state = ASP_STATE_IDLE;
		}
	}

	track->aspss_cur_file_block_offset = 0;
	return (TRUE);
ABORT:
					
	spool->asp_state = ASP_STATE_IDLE;
	track->aspss_cur_file_block_offset = 0;
	return (FALSE);
}

int32
RewindSpooledAudio (void *spool_player)
{
	SPOOL	*spool;
	int32	track_index;

	spool = (SPOOL *) spool_player;
	track_index = spool->asp_current_playing_track_number;

	rewind_track(spool->asp_track_info + track_index, spool);

	ON_FALSE_ABORT((LB_ERROR != load_sample_buffers(spool->asp_signals,track_index,spool,NULL)), 
		"Couldn't load sample buffers in RewindSpooledAudio()\n");

	return (TRUE);
ABORT:
	return (FALSE);
}


int32
ServiceSpooledAudio (void *spool_player, int32 load_signals, int32 *wait_signals)
{
	SPOOL	*spool = (SPOOL *) spool_player;
	int32	track_index = spool->asp_current_playing_track_number;
	int32	result;

	if (chkflg(AS_SA_REQUEST_STOP_PLAYING_FLAG, spool->asp_flags))
	{
		// Service gets called sometimes without need for load
		if (load_signals == 0)
			return (0L);
			
		// Stop playing the sample
		clrflg(AS_SA_REQUEST_STOP_PLAYING_FLAG, spool->asp_flags);
		if (spool->asp_state == ASP_STATE_PLAYING)
		{
			SPOOL_TRACK	*track;
			
			// set cur_file_pos to end of track -- we only get in here
			// if we are finished playing the track, right?
			PauseSpooledAudio(spool);
			track = &spool->asp_track_info[track_index];
			track->aspss_cur_file_pos = track->aspss_sample_data_size;
		}
			
		spool->asp_current_load_buffer_index = 0;
		spool->asp_loaded_buffer_signals = 0L;
		spool->asp_state = ASP_STATE_IDLE;
		*wait_signals = 0L;
	}
	else
	{
		if (spool->asp_state == ASP_STATE_PAUSED)
			*wait_signals = spool->asp_signals;
		else
		{
			SPOOL_TRACK	*track;
			
			spool->asp_loaded_buffer_signals &= ~load_signals;

			result = load_sample_buffers (load_signals, track_index, spool, wait_signals);
			if (result == LB_NO_MORE_DATA)
			{
#ifdef AUTO_ADVANCE
				if (spool->asp_current_playing_track_number + 1 < spool->asp_track_count
						|| (chkflg(AS_SA_LOOP_ON_FLAG, spool->asp_flags)))
#else /* AUTO_ADVANCE */
				if (chkflg (AS_SA_LOOP_ON_FLAG, spool->asp_flags))
#endif /* AUTO_ADVANCE */
				{
					transfer_to_next_track (load_signals & ~*wait_signals, spool);
if (!chkflg (AS_SA_LOOP_ON_FLAG, spool->asp_flags))
{
	track = &spool->asp_track_info[spool->asp_current_playing_track_number];
	// current track has come to an end!
	track->aspss_cur_file_pos = track->aspss_sample_data_size;
	spool->asp_current_playing_track_number++;
}
			
					*wait_signals = spool->asp_signals;
					clrflg (AS_SA_INVALIDATE_BUFFERS_FLAG, spool->asp_flags);
				}
				else if (chkflg (AS_SA_BUFFER_UNDERFLOW, spool->asp_flags))
				{
					clrflg (AS_SA_BUFFER_UNDERFLOW, spool->asp_flags);
					// just in case
					clrflg(AS_SA_REQUEST_STOP_PLAYING_FLAG, spool->asp_flags);
					// Stop playing the sample
					if (spool->asp_state == ASP_STATE_PLAYING)
					{
						// set cur_file_pos to end of track -- we only get in here
						// if we are finished playing the track, right?
						PauseSpooledAudio(spool);
						track = &spool->asp_track_info[track_index];
						track->aspss_cur_file_pos = track->aspss_sample_data_size;
					}
			
					spool->asp_current_load_buffer_index = 0;
					spool->asp_loaded_buffer_signals = 0L;
					spool->asp_state = ASP_STATE_IDLE;
					*wait_signals = 0L;
				}
				else
				{
					setflg(AS_SA_REQUEST_STOP_PLAYING_FLAG, spool->asp_flags);
					unlink_buffers(spool, track_index);
					*wait_signals = spool->asp_signals;

					setflg (AS_SA_INVALIDATE_BUFFERS_FLAG, spool->asp_flags); /* BVT Bug Fix */
				}
			}
		}
	}

	return (0L);
}

int32
SpooledAudioInfo (void *spool_player, int32 *len, int32 *offs)
{
    int32	track_index;
    SPOOL_TRACK	*track;
    int32	i, offsets[MAX_LOADED_TRACKS];
    SPOOL	*spool;

    spool = (SPOOL *)spool_player;    
    track_index = spool->asp_current_playing_track_number;

    offsets[0] = 0;
    *len = *offs = 0;
    track = spool->asp_track_info;
    for (i = 0; i < spool->asp_track_count; i++, track++)
    {
	*len += track->aspss_sample_data_size;
	if (i < track_index)
	    offsets[i + 1] = offsets[i] + track->aspss_sample_data_size;
    }

    if (spool->asp_state == ASP_STATE_PLAYING)
    {
	int32	buffer_index;
	
	for (buffer_index = 0; buffer_index < spool->asp_buffer_count; buffer_index++)
	{
	    Item		att;
	    Err			offset;
	    int32		buf_track;
	    BUFFER_TRACK_INFO	*buf_track_info;
	    SPOOL_BUFFER	*buffer;

	    buffer = &spool->asp_buffers_control[buffer_index];
	    if (!(buffer->asb_signal & spool->asp_loaded_buffer_signals))
		continue;

	    buf_track = buffer->asb_track_number;
	    buf_track_info = &buffer->asb_track_info[buf_track];
	    att = buf_track_info->asb_attachment_item;

	    if ((offset = WhereAttachment (att)) >= 0
		    && offset <= buffer->asb_data_size)
	    {
		track = &spool->asp_track_info[buf_track];
		if (!chkflg (AS_SA_ATTACHMENT_STARTED, spool->asp_flags))
		    offset = track->aspss_cur_file_block_offset;

//printf ("0: offsets[%ld] = %ld, file_block %ld, offset %ld ==> *offs %ld\n", buf_track, offsets[buf_track], buf_track_info->asb_file_block, offset, offsets[buf_track] + buf_track_info->asb_file_block * track->aspss_block_size + offset);
		offset += buf_track_info->asb_file_block * track->aspss_block_size;
		*offs = offsets[buf_track] + offset;

track->aspss_cur_file_pos = offset;
//printf ("0: offs %ld / len %ld\n", *offs, *len);		
		return (1);
	    }
	}
    }

    track = &spool->asp_track_info[track_index];
    *offs = offsets[track_index] + track->aspss_cur_file_pos;
//printf ("1: offsets[%ld] = %ld, cur_file_pos %ld ==> *offs %ld\n", track_index, offsets[track_index], track->aspss_cur_file_pos, *offs);
    
//printf ("1: offs %ld / len %ld\n", *offs, *len);		
    return (1);
}

#ifdef DELAY_STUFF
int32
SpooledAudioData (void *spool_player, char *data)
{
    TagArg	Tags[3];
    int32	size;
    char	buf[512];
    SPOOL	*spool;

    spool = (SPOOL *)spool_player;    
    
    if (spool->asp_state != ASP_STATE_PLAYING)
	return (0);

    Tags[0].ta_Tag = AF_TAG_ADDRESS;
    Tags[1].ta_Tag = AF_TAG_NUMBYTES;
    Tags[2].ta_Tag = TAG_END;
    if (GetAudioItemInfo (spool->asp_delay_line, Tags) < 0)
	return (0);

    size = (int32)Tags[1].ta_Arg;
    memcpy (buf, (char *)Tags[0].ta_Arg, size);
    
    {
	char	*p, *bp;
	int32	prev, cur;
	int32	b;
	
	prev = buf[0];
	if (buf[0] & 0x80)
	    prev |= 0xffffff00;
	bp = 0;
	b = 0;
	for (p = &buf[2]; p < &buf[sizeof (buf)]; p += 2)
	{
	    int32	d;
	    
	    cur = *p;
	    if (*p & 0x80)
		cur |= 0xffffff00;
	    if ((d = cur - prev) < 0)
		d = -d;
	    if (d > b)
	    {
		b = d;
		bp = p;
	    }
	    prev = cur;
	}
	
	if (bp)
	{
	    int32	s;

	    s = size - (bp - buf);
	    memcpy (data, bp, s);
	    memcpy (data + s, buf, size - s);
	}
	else
	    memcpy (data, buf, size);
    }
    
    return (size);
}
#else
int32
SpooledAudioData (void *spool_player, void **data)
{
    SPOOL	*spool;

    spool = (SPOOL *)spool_player;    
    if (spool->asp_state == ASP_STATE_PLAYING)
    {
	int32	buffer_index;
	
	for (buffer_index = 0; buffer_index < spool->asp_buffer_count; buffer_index++)
	{
	    Item		att;
	    Err			offset;
	    int32		track_index;
	    BUFFER_TRACK_INFO	*buf_track_info;
	    SPOOL_BUFFER	*buffer;
	    static char		loc_data[256];
	    
	    buffer = &spool->asp_buffers_control[buffer_index];
	    track_index = buffer->asb_track_number;
	    buf_track_info = &buffer->asb_track_info[track_index];
	    att = buf_track_info->asb_attachment_item;

	    if ((offset = WhereAttachment (att)) >= 0
		    && offset <= buffer->asb_data_size)
	    {
		memcpy (loc_data, (char *)buffer->asb_data + offset, 256);
		*data = (void *)loc_data;
	
		return (256);
	    }
	}
    }
    
    return (0);
}
#endif

//**************** L O C A L  R O U T I N E S ****************

static int32
rewind_track (SPOOL_TRACK *track, SPOOL *spool)
{
	track->aspss_sample_data_read = 0L;
	track->aspss_sample_data_remaining = track->aspss_sample_data_size;
	track->aspss_next_file_block = track->aspss_start_file_block;
	track->aspss_cur_file_pos = 0;

	return (0L);
}

/****************************
 * transfer_to_next_track
 *
 *	When we are called, the audio system has started playing
 *	the last buffer of the previous music track.
 */

static int32
transfer_to_next_track (int32 load_signals, SPOOL *spool)
{
	int32		track_index;
	SPOOL_TRACK	*track;

	track_index = spool->asp_current_playing_track_number;
	track = &(spool->asp_track_info[track_index]);

	if ((chkflg (AS_SA_LOOP_ON_FLAG, spool->asp_flags))
		&& (!(chkflg (AS_SA_ADVANCE_FLAG, spool->asp_flags))))
	{
		/* rewind the track
		 * play the rewound track
		 */

		rewind_track (track, spool);
		ON_FALSE_ABORT((LB_ERROR != load_sample_buffers(load_signals,track_index,spool,NULL)),
			"Couldn't load sample buffers in load_track()\n");
	}
	else
	{
		clrflg(AS_SA_ADVANCE_FLAG, spool->asp_flags);

		if (spool->asp_track_count == track_index + 1)
		{
			//** Stop playing sample
			setflg(AS_SA_REQUEST_STOP_PLAYING_FLAG, spool->asp_flags);

			//** unlink the sample buffers
			unlink_buffers(spool, track_index);
		}
		else
		{
			++track;
			track->aspss_cur_file_pos = 0;
			if (!adjust_track (spool, track))
				ON_ABORT ("couldn't adjust_track in transfer_to...\n");
	
			++track_index;
#if 0
			spool->asp_current_playing_track_number = track_index;
#endif
			
			ON_FALSE_ABORT (LB_ERROR != load_sample_buffers (load_signals,track_index,spool,NULL),
				"Couldn't load sample buffers in transfer_to_next_track ()\n");
		}
	}

	return (TRUE);
ABORT:
	return (FALSE);
}

static int32
set_track_volume (SPOOL *spool, int32 left_volume, int32 right_volume, int32 track_index)
{
	ON_NEG_ABORT(TweakKnob(spool->asp_left_volume_knob, left_volume),
		"Couldn't set left volume in set_track_volume()\n");
	ON_NEG_ABORT(TweakKnob(spool->asp_right_volume_knob, right_volume),
		"Couldn't set right volume in set_track_volume()\n");

	return (TRUE);
ABORT:
	return (FALSE);
}

static int32
load_sample_buffers (int32 load_signals, int32 track, SPOOL *spool, int32 *signals_to_wait_for)
{
	int32		buffer = spool->asp_current_load_buffer_index;
	SPOOL_BUFFER	*buffers = spool->asp_buffers_control;
	int		i;
	int32		return_value = LB_MORE_DATA;
	int32		cur_file_block_offset;

	spool->asp_loaded_buffer_signals &= ~load_signals;

	cur_file_block_offset = spool->asp_track_info[track].aspss_cur_file_block_offset;
    
	for (i = 0; i < spool->asp_buffer_count; i ++)
	{
		if (buffers[buffer].asb_signal & load_signals)
		{
			return_value = fill_sample_buffer (track, buffer, spool);
			
			switch (return_value)
			{
				case LB_MORE_DATA: // still more to go
					break;
				case LB_LAST_DATA: // this is the last buffer
					INC_CIRCULAR (buffer, spool->asp_buffer_count);
					goto EXIT;
				case LB_NO_MORE_DATA:	// after last buffer
					goto EXIT;
				case LB_ERROR:	// error
					goto EXIT;
					break;
			}
		}

		INC_CIRCULAR (buffer, spool->asp_buffer_count);
		spool->asp_track_info[track].aspss_cur_file_block_offset = 0;
	}
	
EXIT:
	spool->asp_track_info[track].aspss_cur_file_block_offset = cur_file_block_offset;
	spool->asp_current_load_buffer_index = buffer;
	if (signals_to_wait_for)
		*signals_to_wait_for = spool->asp_loaded_buffer_signals;

	return (return_value);
}

static int32
fill_sample_buffer (int32 track_number, int32 buffer_number, SPOOL *spool)
{
	SPOOL_TRACK	*track;

	track = &(spool->asp_track_info[track_number]);
	if (! track->aspss_sample_data_remaining)
		goto EXIT_NO_MORE_DATA;

	{
		SPOOL_BUFFER	*buffer = &(spool->asp_buffers_control[buffer_number]);
		int32	data_size, blocks, blocks_read;
		
		if (!chkflg (AS_TR_ABX_SND, track->aspss_flags))
		{
			data_size = min (spool->asp_buffers_size, track->aspss_sample_data_remaining);
			blocks = (data_size + (DEFAULT_BLOCK_SIZE - 1)) / DEFAULT_BLOCK_SIZE;
			blocks_read = ReadDiskFileBlocks (
				spool,
				track,
				buffer->asb_data,
				blocks);
			buffer->asb_track_info[track_number].asb_file_block =
				track->aspss_next_file_block;
		}
		else
		{
#if defined(ABX_STREAM_IO) && !defined(PRE_OPEN_STREAMS)
			if (track->aspss_IOreq == 0)
			{
				track->aspss_IOreq = (Item)OpenDiskStream (
					track->aspss_filename,
					spool->asp_bytes_per_track
					);
				ON_NULL_ABORT (track->aspss_IOreq,
					"Couldn't OpenDiskStream in fill_sample_buffer\n");
			}
#endif
			buffer->asb_track_info[track_number].asb_file_block =
				track->aspss_abx_info.abx_bufsize * track->aspss_abx_info.abx_FrameIdx;
			data_size = min (spool->asp_buffers_size, track->aspss_sample_data_remaining);
//printf ("reading %ld bytes of track %ld into buffer %ld...", data_size, track_number, buffer_number);
			blocks_read = ReadDiskFileBlocks (
				spool,
				track,
				buffer->asb_data,
				data_size);
//printf ("done (%ld compressed)!\n", blocks_read);
			blocks = blocks_read;
		}		

		buffer->asb_track_number = track_number;
		ON_TRUE_ABORT ((blocks_read - blocks),
			"Couldn't read all requested disk file blocks in fill_sample_buffers()\n");

		spool->asp_loaded_buffer_signals |= buffer->asb_signal;

		track->aspss_sample_data_remaining -= data_size;

		if (data_size < spool->asp_buffers_size)
		{
			int32	extra;

			extra = spool->asp_buffers_size - data_size;
			memset ((char *)buffer->asb_data + data_size, 0, extra);
		}
		
		buffer->asb_data_size = data_size;
		
		if (! track->aspss_sample_data_read)
		{
			int32	offset;
			
			if (!chkflg (AS_TR_ABX_SND, track->aspss_flags))
				offset = track->aspss_start_file_block_offset;
			else
				offset = 0;
			
			resize_sample_buffer_mem (
				spool,
				buffer,
				offset,
				min (
					buffer->asb_data_size,
					spool->asp_buffers_size - offset
				),
				track_number
			);
			setflg (AS_BF_PARTIAL_FLAG, buffer->asb_flags);
		}
		else
		{
			int32	resize;
			
			if (! track->aspss_sample_data_remaining)
				resize = 1, setflg (AS_BF_PARTIAL_FLAG, buffer->asb_flags);
			else if (chkflg (AS_BF_PARTIAL_FLAG, buffer->asb_flags))
				resize = 1, clrflg (AS_BF_PARTIAL_FLAG, buffer->asb_flags);
			else if (track->aspss_cur_file_block_offset)
				resize = 1;
			else
				resize = 0;
				
			if (resize)
			{
				resize_sample_buffer_mem (
					spool,
					buffer,
					track->aspss_cur_file_block_offset,
					min (
						buffer->asb_data_size,
						spool->asp_buffers_size - track->aspss_cur_file_block_offset
					),
					track_number
					);
			}
		}

		track->aspss_sample_data_read		+= data_size;
		track->aspss_next_file_block		+= blocks_read;

		if (! track->aspss_sample_data_remaining)
		{
#if defined(ABX_STREAM_IO) && defined(PRE_OPEN_STREAMS)
			// minor (and perhaps useless?) optimization
			if (track_number + 1 < spool->asp_track_count)
			{
				track++;
				SeekDiskStream ((Stream *)track->aspss_IOreq, 0, SEEK_SET);
				ReadDiskStream ((Stream *)track->aspss_IOreq, (char *)0, 0);
			}
#endif
			goto EXIT_DONE;
		}
	}

	return (LB_MORE_DATA);
EXIT_DONE:
	return (LB_LAST_DATA);
EXIT_NO_MORE_DATA:
	return (LB_NO_MORE_DATA);
ABORT:
	return (LB_ERROR);
}

static void
resize_sample_buffer_mem (SPOOL *spool, SPOOL_BUFFER *buffer, int32 start, int32 len, int32 track_index)
{
	static TagArg	Tags_rsm[] =
	{
	    {AF_TAG_ADDRESS, (void *) 0},		// ptr to sample data
	    {AF_TAG_NUMBYTES, (void *) 0},		// # of bytes in sample table
	    {TAG_END, NULL}
	};
	
	// unlink the attachments because of an OS 'feature'
	unlink_buffers (spool, track_index);

	// then reset the sample's memory
	Tags_rsm[0].ta_Arg = (void *)((char *)buffer->asb_data + start);
	Tags_rsm[1].ta_Arg = (void *)len;

	SetAudioItemInfo (
		buffer->asb_track_info[track_index].asb_sample_item,
		Tags_rsm
		);

	// relink the attachemnts
	link_buffers (spool, track_index);
}

static void
link_buffers ( SPOOL *spool, int32 track_index)
{
	SPOOL_BUFFER	*buffers, *nextbuf;
	int32		buffer_count;

	buffers = spool->asp_buffers_control;
	buffer_count = spool->asp_buffer_count;

	// link the attachments in a circle for seamless multi-buffered playback

	for (; buffer_count; buffer_count--, buffers++)
	{
		if (buffer_count == 1)
		    nextbuf = spool->asp_buffers_control;
		else
		    nextbuf = buffers + 1;

		if (LinkAttachments(buffers->asb_track_info[track_index].asb_attachment_item,
			nextbuf->asb_track_info[track_index].asb_attachment_item))
		    break;
	}
}

static void
unlink_buffers (SPOOL *spool, int32 track_index)
{
	SPOOL_BUFFER	*buffers;
	int32		buffer_count;

	buffers = spool->asp_buffers_control;
	buffer_count = spool->asp_buffer_count;

	// link the attachments in a circle for seamless multi-buffered playback

	for (; buffer_count; buffer_count--, buffers++)
	{
		if (LinkAttachments(buffers->asb_track_info[track_index].asb_attachment_item, 0L))
			break;
	}
}

static int32
create_file_things (SPOOL_TRACK *track, char *filename)
{
#ifdef ABX_STREAM_IO
    if (!chkflg (AS_TR_ABX_SND, track->aspss_flags))
#endif
    {
	TagArg	targ[2];
	
	track->aspss_file = OpenDiskFile (filename);
	if (track->aspss_file < 0)
	    return (0);
	
	targ[0].ta_Tag = CREATEIOREQ_TAG_DEVICE;
	targ[0].ta_Arg = (void *)track->aspss_file;
	targ[1].ta_Tag = TAG_END;

	track->aspss_IOreq = CreateItem (MKNODEID (KERNELNODE, IOREQNODE), targ);
	if (track->aspss_IOreq < 0)
	    return (0);
	
	return (1);
    }
#ifdef ABX_STREAM_IO
    else
    {
#ifndef PRE_OPEN_STREAMS
	strcpy (track->aspss_filename, filename);
#endif
	if ((track->aspss_IOreq = (Item)OpenDiskStream (filename, MAX_ABX_BYTES)) == 0)
	    track->aspss_IOreq = (Item)OpenDiskStream (filename, MIN_ABX_BYTES);
	    
	return (track->aspss_IOreq);
    }
#endif
}

static int32
load_track (char *filename, SPOOL *spool)
{
	int32			track_number = spool->asp_current_load_track_number;
	SPOOL_TRACK		*current_track = &(spool->asp_track_info[track_number]);
	int32			buffer_count = spool->asp_buffer_count;
	SPOOL_BUFFER		*buffers = spool->asp_buffers_control;
	SAMPLE_FILE_INFO	aiff_file_info;

	ON_TRUE_ABORT(chkflg(AS_TR_IN_USE_FLAG,current_track->aspss_flags),
		"load track overflow\n");

	// tell the world that we are loaded
	setflg(AS_TR_IN_USE_FLAG, current_track->aspss_flags);
	
	if (!strcasecmp (&filename[strlen (filename) - strlen (".abx")], ".abx"))
	{
		setflg (AS_TR_ABX_SND, current_track->aspss_flags);
		current_track->aspss_block_size = 1;
	}
	else
	{
		clrflg (AS_TR_ABX_SND, current_track->aspss_flags);
		current_track->aspss_block_size = DEFAULT_BLOCK_SIZE;
	}

	ON_FALSE_ABORT(create_file_things (current_track, filename),
		"couldn't create file things\n");
	
	ON_FALSE_ABORT (get_sample_info (spool, current_track, filename, &aiff_file_info),
		"couldn't get aiff info\n");

	current_track->aspss_sample_rate = SCALE_RATE (aiff_file_info.afi_sample_rate);

	// get the instruments
	current_track->aspss_sample_instrument =
		LoadInstrument (aiff_file_info.afi_dsp_filename, 0, 100);

	ON_NOT_ITEM_ABORT(current_track->aspss_sample_instrument, 
		"couldn't load sample dsp file\n");

	// load the buffer control specific stuff
	{
		int32	i;
		Item	sample_instrument = current_track->aspss_sample_instrument;

		// allocate sample items for each buffer and attach the samples to the instrument

		for (i = 0; i < buffer_count; i ++)
		{
			BUFFER_TRACK_INFO	*bti = &(buffers[i].asb_track_info[track_number]);

			bti->asb_sample_item = myCreateSample (
				&aiff_file_info,
				spool->asp_buffers_size,
				buffers[i].asb_data
				);
			ON_NOT_ITEM_ABORT(bti->asb_sample_item, "Couldn't create sample\n");

			bti->asb_attachment_item = AttachSample(
				sample_instrument, 
				bti->asb_sample_item,
				0
				);
			ON_NOT_ITEM_ABORT(bti->asb_attachment_item, "Couldn't create sample\n");

			ON_ERROR_ABORT(set_attachment_flags(bti->asb_attachment_item, AF_ATTF_NOAUTOSTART),
				"Couldn't set attachment flags in load_track()\n");
		}

		// connect the instruments

		spool->asp_channels = aiff_file_info.afi_channels;

		toggle_connections (spool, current_track, 1);

		// link the attachments in a circle for seamless multi-buffered playback

		for (i = 0; i < buffer_count; i ++)
		{
			BUFFER_TRACK_INFO	*bti = &(buffers[i].asb_track_info[track_number]);
			BUFFER_TRACK_INFO	*bti_next = &(buffers[(i+1) % buffer_count].asb_track_info[track_number]);

			LinkAttachments(bti->asb_attachment_item, bti_next->asb_attachment_item);
			MonitorAttachment(bti->asb_attachment_item, buffers[i].asb_cue_item, CUE_AT_END );
		}

	}

	current_track->aspss_sample_data_size = aiff_file_info.afi_data_size;
	current_track->aspss_sample_data_read = 0L;

	// rewind
	rewind_track (current_track, spool);

#ifndef PRE_LOAD_BUFFER
	// force buffer load in Resume
	setflg (AS_SA_INVALIDATE_BUFFERS_FLAG, spool->asp_flags);
#else
	load_sample_buffers (spool->asp_signals,track_number,spool,NULL);
#if 0
	current_track->aspss_cur_file_pos =
		current_track->aspss_abx_info.abx_bufsize
		* current_track->aspss_abx_info.abx_FrameIdx;
#endif
	if (spool->asp_signals != spool->asp_loaded_buffer_signals)
	{
		// force buffer load in Resume
		setflg (AS_SA_INVALIDATE_BUFFERS_FLAG, spool->asp_flags);
	}
#endif
    
	// tell the world that we're loaded
	setflg(AS_TR_IN_USE_FLAG, current_track->aspss_flags);
	setflg(AS_SA_OK_TO_SPLICE, spool->asp_flags);

	spool->asp_track_count++;

	spool->asp_current_load_track_number++;

	return (TRUE);
ABORT:
	unload_track (track_number, spool);
	
	return (FALSE);
}

static int32
splice_track (char *filename, SPOOL *spool)
{
	int32			track_number = spool->asp_current_load_track_number;
	int32			previous_track_number = NUM_PREVIOUS(track_number, MAX_LOADED_TRACKS);
	SPOOL_TRACK		*current_track = &(spool->asp_track_info[track_number]);
	SPOOL_TRACK		*previous_track = &(spool->asp_track_info[previous_track_number]);
	int32			buffer_count = spool->asp_buffer_count;
	SPOOL_BUFFER		*buffers = spool->asp_buffers_control;
	SAMPLE_FILE_INFO	aiff_file_info;

	ON_TRUE_ABORT(chkflg(AS_TR_IN_USE_FLAG,current_track->aspss_flags),
		"splice track overflow\n");
	ON_TRUE_ABORT(spool->asp_track_count == MAX_LOADED_TRACKS,
		"splice track overflow\n");
	setflg(AS_TR_IS_SPLICE_FLAG, current_track->aspss_flags);
	setflg(AS_TR_IN_USE_FLAG, current_track->aspss_flags);

	if (!strcasecmp (&filename[strlen (filename) - strlen (".abx")], ".abx"))
	{
		setflg (AS_TR_ABX_SND, current_track->aspss_flags);
		current_track->aspss_block_size = 1;
	}
	else
	{
		clrflg (AS_TR_ABX_SND, current_track->aspss_flags);
		current_track->aspss_block_size = DEFAULT_BLOCK_SIZE;
	}

	ON_FALSE_ABORT(create_file_things (current_track, filename),
		"couldn't create file things\n");
		
	ON_FALSE_ABORT (get_sample_info (spool, current_track, filename, &aiff_file_info),
		"couldn't get aiff info\n");

	current_track->aspss_sample_rate = SCALE_RATE (aiff_file_info.afi_sample_rate);
		
	setflg(AS_TR_DEALLOCATE_MINIMAL_FLAG, current_track->aspss_flags);

	/***********************************
	 * - copy over items from previous
	 * - create new needed items
	 * - pre-load buffer(s)
	 * - link attachment old->new
	 */

	/*******************************
	 * Copy over items from previous
	 */

	current_track->aspss_sample_instrument = previous_track->aspss_sample_instrument;

	{
		int		i;
		SPOOL_BUFFER	*buffer = buffers;

		for (i = 0; i < buffer_count; i ++)
		{
			buffer->asb_track_info[track_number].asb_sample_item = 
				buffer->asb_track_info[previous_track_number].asb_sample_item;

			buffer->asb_track_info[track_number].asb_attachment_item = 
				buffer->asb_track_info[previous_track_number].asb_attachment_item;

			buffer	++;
		}
	}

	current_track->aspss_sample_data_size = aiff_file_info.afi_data_size;
	current_track->aspss_sample_data_remaining = aiff_file_info.afi_data_size;
	current_track->aspss_sample_data_read = 0L;

	spool->asp_current_load_track_number++;

	spool->asp_track_count++;

#if defined(ABX_STREAM_IO) && !defined(PRE_OPEN_STREAMS)
	if (spool->asp_track_count > MAX_PRE_LOAD_STREAMS
			&& chkflg (AS_TR_ABX_SND, current_track->aspss_flags))
	{
		if (spool->asp_track_count == MAX_PRE_LOAD_STREAMS + 1)
		{
			int32		i;
			SPOOL_TRACK	*track;
		
			i = spool->asp_track_count - 1;
			track = current_track - 1;
			do
			{
				CloseDiskStream ((Stream *)track->aspss_IOreq);
				track->aspss_IOreq = 0;
				track--;
			} while (--i);
		}
		
		CloseDiskStream ((Stream *)current_track->aspss_IOreq);
		current_track->aspss_IOreq = 0;
	}
#endif

	return (TRUE);
ABORT:
	unload_track (track_number, spool);
	
	return (FALSE);
}

static void
unload_track (int32 track_index, SPOOL *spool)
{
	SPOOL_TRACK	*track = &(spool->asp_track_info[track_index]);
	int32		buffer_count = spool->asp_buffer_count;
	SPOOL_BUFFER	*buffers = spool->asp_buffers_control;

	clrflg (AS_TR_IN_USE_FLAG, track->aspss_flags);

#ifdef ABX_STREAM_IO
	if (!chkflg (AS_TR_ABX_SND, track->aspss_flags))
#endif
	{
		if (IS_ITEM(track->aspss_IOreq))
			DeleteItem (track->aspss_IOreq);
		if (IS_ITEM(track->aspss_file))
			CloseDiskFile (track->aspss_file);
		track->aspss_IOreq = track->aspss_file = NULL_ITEM;
	}
#ifdef ABX_STREAM_IO
	else
	{
		if (track->aspss_IOreq)
			CloseDiskStream ((Stream *)track->aspss_IOreq);
		track->aspss_IOreq = 0;
	}
#endif

	// probably not necessary, but why tempt fate?!
	clrflg (AS_TR_ABX_SND, track->aspss_flags);
	
	if (! (chkflg(AS_TR_DEALLOCATE_MINIMAL_FLAG, track->aspss_flags)))
	{
		int	i;

		// ** unload the buffer specific stuff

		for (i = 0; i < buffer_count; i ++)
		{
			BUFFER_TRACK_INFO	*buffer_track_info;
	
			buffer_track_info = &(buffers[i].asb_track_info[track_index]);
			if (IS_ITEM(buffer_track_info->asb_attachment_item))
			{
				DetachSample (buffer_track_info->asb_attachment_item);
#if 0
				DeleteItem (buffer_track_info->asb_attachment_item);
#endif
			}

			if (IS_ITEM(buffer_track_info->asb_sample_item))
			{
				UnloadSample (buffer_track_info->asb_sample_item);
				DeleteItem (buffer_track_info->asb_sample_item);
			}

			buffer_track_info->asb_attachment_item =
				buffer_track_info->asb_sample_item = NULL_ITEM;
		}

		toggle_connections (spool, track, 0);

		if (IS_ITEM(track->aspss_sample_instrument))
			UnloadInstrument (track->aspss_sample_instrument);

		track->aspss_sample_instrument = NULL_ITEM;
	}
	else
		clrflg (AS_TR_DEALLOCATE_MINIMAL_FLAG, track->aspss_flags);
}

static char *
optimal_dsp_filename (
	uint32	sample_rate_16_16,	// 0x56220000 = 22KHz, 0xAC440000 = 44KHz
	int32	channels,		// 1 = mono, 2 = stereo
	int32	sample_byte_width,	// 1 = 8bit, 2 = 16bit
	int32	compression_type	// ID_SDX2, ID_ADP4 (not supported yet), NULL = none
	)
{
#define ODF_COMPRESSED		(1)
#define ODF_STEREO		(2)
#define ODF_16BIT		(4)
#define ODF_VAR_KHZ		(0)
#define ODF_22_KHZ		(8)
#define ODF_44_KHZ		(16)
	// [44KHz | 22KHz | !(22KHz||44KHz)]
	// [16 bit | 8 bit] 
	// [stereo | mono]
	// [compressed | uncompressed]
	static char *dsp_names[] =
	{
		"varmono8.dsp",			// !(22||44) 8 mono uncompressed
		NULL,				// !(22||44) 8 mono compressed
		NULL,				// !(22||44) 8 stereo uncompressed
		NULL,				// !(22||44) 8 stereo compressed
		"varmono16.dsp",		// !(22||44) 16 mono uncompressed
		NULL,				// !(22||44) 16 mono compressed
		NULL,				// !(22||44) 16 stereo uncompressed
		NULL,				// !(22||44) 16 stereo compressed
		"halfmono8.dsp",		// 22k 8 mono uncompressed
		NULL,				// 22k 8 mono compressed
		"halfstereo8.dsp",		// 22k 8 stereo uncompressed
		NULL,				// 22k 8 stereo compressed
		"halfmonosample.dsp",		// 22k 16 mono uncompressed
		"dcsqxdhalfmono.dsp",		// 22k 16 mono compressed
		"halfstereosample.dsp",		// 22k 16 stereo uncompressed
		"dcsqxdhalfstereo.dsp",		// 22k 16 stereo compressed
		"fixedmono8.dsp",		// 44k 8 mono uncompressed
		NULL,				// 44k 8 mono compressed
		"fixedstereo8.dsp",		// 44k 8 stereo uncompressed
		NULL,				// 44k 8 stereo compressed
		"fixedmonosample.dsp",		// 44k 16 mono uncompressed
		"dcsqxdmono.dsp",		// 44k 16 mono compressed
		"fixedstereosample.dsp",	// 44k 16 stereo uncompressed
		"dcsqxdstereo.dsp"		// 44k 16 stereo compressed
	};
	int		index;

	if (sample_rate_16_16 == HERTZ_44100_16_16)
		index = ODF_44_KHZ;
	else if (sample_rate_16_16 == HERTZ_22050_16_16)
		index = ODF_22_KHZ;
	else
		index = ODF_VAR_KHZ;

	if (sample_byte_width == 2)
		index += ODF_16BIT;

	if (channels == 2)
		index += ODF_STEREO;

	if (compression_type == ID_SDX2)
		index += ODF_COMPRESSED;

	return (dsp_names[index]);
}

static int32
get_sample_info (SPOOL *spool, SPOOL_TRACK *track, char *filename, SAMPLE_FILE_INFO *aiff_file_info)
{
    if (!chkflg (AS_TR_ABX_SND, track->aspss_flags))
    {
	static TagArg	tags_gsi[] =
	{
		{AF_TAG_SAMPLE_RATE, 0},
		{AF_TAG_CHANNELS, 0},
		{AF_TAG_WIDTH, 0},
		{AF_TAG_COMPRESSIONTYPE, 0},
		{AF_TAG_DATA_SIZE, 0},
		{AF_TAG_NUMBITS, 0},				// width of each sample in bits
		{AF_TAG_COMPRESSIONRATIO, 0},			// 2 = 2:1
		{AF_TAG_DATA_OFFSET, 0},

		TAG_END, 0
	};
	int32	result;
	Item	sample;

	sample = ScanSample (filename, 100L);
	ON_NOT_ITEM_ABORT(sample, "Couldn't scan sample in get_sample_info()\n");
	
	result = GetAudioItemInfo (sample, tags_gsi);
	ON_ERROR_ABORT(result, "Couldn't get audio item info in get_sample_info()\n");

	UnloadSample (sample);

	aiff_file_info->afi_sample_rate = (int32) tags_gsi[0].ta_Arg;
	aiff_file_info->afi_channels = (int32) tags_gsi[1].ta_Arg;
	aiff_file_info->afi_width = (int32) tags_gsi[2].ta_Arg;
	aiff_file_info->afi_compression_type = (int32) tags_gsi[3].ta_Arg;
	aiff_file_info->afi_data_size = (int32) tags_gsi[4].ta_Arg;
	aiff_file_info->afi_num_bits = (int32) tags_gsi[5].ta_Arg;
	aiff_file_info->afi_compression_ratio = (int32) tags_gsi[6].ta_Arg;
	aiff_file_info->afi_data_offset = (int32) tags_gsi[7].ta_Arg;

	aiff_file_info->afi_dsp_filename = optimal_dsp_filename (
		aiff_file_info->afi_sample_rate,
		aiff_file_info->afi_channels,
		aiff_file_info->afi_width,
		aiff_file_info->afi_compression_type
		);

	ON_NULL_ABORT(aiff_file_info->afi_dsp_filename,
		"Couldn't get optimal DSP file name in get_sample_info()\n");

	track->aspss_start_file_block =
		aiff_file_info->afi_data_offset / DEFAULT_BLOCK_SIZE;
	track->aspss_start_file_block_offset = 
		aiff_file_info->afi_data_offset % DEFAULT_BLOCK_SIZE;
		
	return (TRUE);

ABORT:
	if (IS_ITEM(sample))
		UnloadSample (sample);
	
	return (FALSE);
    }
    else
    {
	int32		abxfreq, ret;
	ABX_INFO	abx_info;

	ret = load_abx_info (spool, track->aspss_IOreq, &abx_info, &abxfreq);
	
	if (!ret)
	    return (FALSE);
    
	track->aspss_abx_info.abx_FrameIdx = 0;
	track->aspss_abx_info.abx_FrameCt = abx_info.abx_FrameCt;
	track->aspss_abx_info.abx_bufsize = abx_info.abx_bufsize;
	
	aiff_file_info->afi_sample_rate = abxfreq << 16;
	aiff_file_info->afi_channels = 1;
	aiff_file_info->afi_width = 1;
	aiff_file_info->afi_data_size = abx_info.abx_TotSiz;
	aiff_file_info->afi_num_bits = 8;
	aiff_file_info->afi_compression_ratio = 1;
	aiff_file_info->afi_compression_type = 0;
	aiff_file_info->afi_data_offset = 0;
	
	aiff_file_info->afi_dsp_filename = optimal_dsp_filename (
		aiff_file_info->afi_sample_rate,
		aiff_file_info->afi_channels,
		aiff_file_info->afi_width,
		aiff_file_info->afi_compression_type
		);

	if (!aiff_file_info->afi_dsp_filename)
	    return (FALSE);

	track->aspss_start_file_block = 0;
	track->aspss_start_file_block_offset =
		ABX_HEADER_SIZE + track->aspss_abx_info.abx_FrameCt * ABX_FRAME_SIZE;
		
	return (TRUE);
    }
}

#ifdef ABX_STREAM_IO
static int32
load_abx_info (SPOOL *spool, Item ioreq, ABX_INFO *abx_info, int32 *freq)
{
#define DEF_ABX_FREQ	11025
#define SWAPLONG(l)	(int32)((uint32)(l)[0] | ((uint32)(l)[1] << 8) | ((uint32)(l)[2] << 16) | ((uint32)(l)[3] << 24))
#define SWAPSHORT(s)	(int32)((uint32)(s)[0] | ((uint32)(s)[1] << 8))
    struct
    {
	unsigned char	TotalFrames[2];
	unsigned char	TotSiz[4];
	unsigned char	bufsize[2], freq[2];
    } abx;
    int32	i, fct;
    ABX_FRAME	*abxf;
    Stream	*sp;
    
    sp = (Stream *)ioreq;

    if (SeekDiskStream (sp, 0, SEEK_SET) < 0)
	return (FALSE);
    if (ReadDiskStream (sp, (char *)&abx, ABX_HEADER_SIZE) < 0)
	return (FALSE);

    fct = SWAPSHORT (abx.TotalFrames);
    
    if (abx_info)
    {
	abx_info->abx_FrameCt = fct;
	abx_info->abx_bufsize = SWAPSHORT (abx.bufsize);
 	abx_info->abx_TotSiz = SWAPLONG (abx.TotSiz);
    }

    if (freq)
    {
	*freq = SWAPSHORT (abx.freq);
	if (*freq == 0)
	    *freq = DEF_ABX_FREQ;
    }
    
    abxf = spool->asp_abx_FrameList;
    do
    {
	int32	ct;
	struct
	{
	    unsigned char	fileaddress[4];
	    unsigned char	fsize[2];
	    unsigned char	usize[2];
	} flist[100];
	
	if (fct > sizeof (flist) / sizeof (flist[0]))
	    ct = sizeof (flist) / sizeof (flist[0]);
	else
	    ct = fct;
	    
	if (ReadDiskStream (sp, (char *)flist, sizeof (flist[0]) * ct) < 0)
	    return (FALSE);
	    
	fct -= ct;

	for (i = 0; i < ct; i++, abxf++)
	{
	    abxf->fileaddress = SWAPLONG (flist[i].fileaddress);
	    abxf->sizes = (SWAPSHORT (flist[i].fsize) << 16) | SWAPSHORT (flist[i].usize);
	}
    } while (fct > 0);
    
    return (TRUE);
}
#else
static int32
load_abx_info (SPOOL *spool, Item ioreq, ABX_INFO *abx_info, int32 *freq)
{
#define DEF_ABX_FREQ	11025
#define SWAPLONG(l)	(int32)((uint32)(l)[0] | ((uint32)(l)[1] << 8) | ((uint32)(l)[2] << 16) | ((uint32)(l)[3] << 24))
#define SWAPSHORT(s)	(int32)((uint32)(s)[0] | ((uint32)(s)[1] << 8))
    struct
    {
	unsigned char	TotalFrames[2];
	unsigned char	TotSiz[4];
	unsigned char	bufsize[2], freq[2];
    } abx;
    int32	i, fct, byte_offs;
    ABX_FRAME	*abxf;
    IOInfo	info;
    char	*workbufptr;
    
    memset (&info, 0, sizeof (info));
	
    info.ioi_Command = CMD_READ;
    info.ioi_Recv.iob_Buffer = spool->asp_abx_workbuf;
    info.ioi_Recv.iob_Len = ((10 + MAX_ABX_FRAMES * ABX_FRAME_SIZE + DEFAULT_BLOCK_SIZE - 1)
	    / DEFAULT_BLOCK_SIZE) * DEFAULT_BLOCK_SIZE;
    info.ioi_Offset = 0;

    if (DoIO (ioreq, &info) < 0)
	return (FALSE);
    byte_offs = ABX_HEADER_SIZE;
    workbufptr = spool->asp_abx_workbuf + byte_offs;
	
    memcpy (&abx, spool->asp_abx_workbuf, ABX_HEADER_SIZE);

    fct = SWAPSHORT (abx.TotalFrames);
    
    if (abx_info)
    {
	abx_info->abx_FrameCt = fct;
	abx_info->abx_bufsize = SWAPSHORT (abx.bufsize);
 	abx_info->abx_TotSiz = SWAPLONG (abx.TotSiz);
    }

    if (freq)
    {
	*freq = SWAPSHORT (abx.freq);
	if (*freq == 0)
	    *freq = DEF_ABX_FREQ;
    }
    
    abxf = spool->asp_abx_FrameList;
    do
    {
	int32	ct, len;
	struct
	{
	    unsigned char	fileaddress[4];
	    unsigned char	fsize[2];
	    unsigned char	usize[2];
	} flist[100];
	
	if (fct > sizeof (flist) / sizeof (flist[0]))
	    ct = sizeof (flist) / sizeof (flist[0]);
	else
	    ct = fct;

	len = ct * sizeof (flist[0]);
    
	memcpy (flist, workbufptr, len);
	    
	byte_offs += len;
	workbufptr += len;
	
	fct -= ct;

	for (i = 0; i < ct; i++, abxf++)
	{
	    abxf->fileaddress = SWAPLONG (flist[i].fileaddress);
	    abxf->fsize = (SWAPSHORT (flist[i].fsize) << 16) | SWAPSHORT (flist[i].usize);
	}
    } while (fct > 0);
    
    return (TRUE);
}
#endif

#define MEMSET	memset

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
#define MAKE_WORD(lo,hi)	((lo)|((hi)<<8))
#define Get8086word(t)	MAKE_WORD ((t)[0], (t)[1])

// GetFreq will report the playback frequency of a particular ACOMP data
// file.
static
int GetFreq (unsigned char	*sound)
{
    return( Get8086word(sound+2) );
}

#define FLIP_FLOP_SIZE	(abx_bufsize)

#define SIGN_BYTE	0x80

static void
UnCompressAudio (unsigned char	*source, unsigned char	*curflop, int32 abx_bufsize)
{
    int		slen,frame,freq;
    unsigned char	*dest;
    int		prev;
    long		ffcount;

    dest = curflop;
    ffcount = 0;

    slen = Get8086word(source);
    freq = GetFreq(source);
    source+=4;		    // Skip length, and then frequency word.
    frame = *source++;    // Frame size.
    source+=3;		     // Skip sqelch value, and maximum error allowed.
    prev = *source++;      // Get initial previous data point.
    *dest++ = prev ^ SIGN_BYTE;
    slen--;			  // Decrement total sound length.
    ++ffcount;
    while (slen > 0)
    {
	do
	{
	    int		bytes;
	    unsigned char	sample;

	    sample = *source++;  // Get sample.
	    if (sample & RESYNC) // Is it a resync byte?
	    {
		--slen;  // Decrement output sample length.

		prev = (sample & 0x7F) << 1; // Store resync byte.
		*dest++ = prev ^ SIGN_BYTE;
	    }
	    else if (sample & SQLCH) // Is it a squelch byte?
	    {
		bytes = sample & SQUELCHCNT;    // And off the number of squelch bytes
		slen -= bytes;    // Decrement total samples remaining count.

		MEMSET (dest, prev ^ SIGN_BYTE, bytes);
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
			int	up;

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
				*dest++=prev ^ SIGN_BYTE;
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
			    *dest++ = prev ^ SIGN_BYTE;

			    prev += base[(sample>>4)&0x3];
			    if ( prev < 0 ) prev = 0;
			    else if ( prev > 255 ) prev = 255;
			    *dest++ = prev ^ SIGN_BYTE;

			    prev += base[(sample>>2)&0x3];
			    if ( prev < 0 ) prev = 0;
			    else if ( prev > 255 ) prev = 255;
			    *dest++ = prev ^ SIGN_BYTE;

			    prev += base[sample&0x3];
			    if ( prev < 0 ) prev = 0;
			    else if ( prev > 255 ) prev = 255;
			    *dest++ = prev ^ SIGN_BYTE;
			}
			break;
		    case FOURBIT:
			for (bytes = frame / 2; bytes; bytes--)
			{
			    sample = *source++;

			    prev += base[sample>>4];
			    if ( prev < 0 ) prev = 0;
			    else if ( prev > 255 ) prev = 255;
			    *dest++ = prev ^ SIGN_BYTE;

			    prev += base[sample&0x0F];
			    if ( prev < 0 ) prev = 0;
			    else if ( prev > 255 ) prev = 255;
			    *dest++ = prev ^ SIGN_BYTE;
			}
			break;
		}
	    }
		// While still audio data to decompress....
	} while ((ffcount = dest - curflop) < FLIP_FLOP_SIZE && slen > 0);
    }
}

static int32
ReadDiskFileBlocks (SPOOL *spool, SPOOL_TRACK *track, void *buffer, int32 blocks)
{
    if (!chkflg (AS_TR_ABX_SND, track->aspss_flags))
    {
	IOInfo	info;
    
	memset (&info, 0, sizeof (info));
	
	info.ioi_Command = CMD_READ;
	info.ioi_Recv.iob_Buffer = buffer;
	info.ioi_Recv.iob_Len = DEFAULT_BLOCK_SIZE * blocks;
	info.ioi_Offset = track->aspss_next_file_block;

	if (DoIO (track->aspss_IOreq, &info) < 0)
	    return (0);
	else
	    return (blocks);
    }
    else
    {
	int32		blocks_read;
	unsigned char	*bufptr;
	ABX_FRAME	*abxf;

#ifdef ABX_STREAM_IO
	if (SeekDiskStream (
		(Stream *)track->aspss_IOreq,
		track->aspss_next_file_block + track->aspss_start_file_block_offset,
		SEEK_SET
		) < 0)
	    return (0);

	blocks_read = 0;
	bufptr = (unsigned char *)buffer;
	abxf = &ABX_FRAME_LIST[track->aspss_abx_info.abx_FrameIdx];
	do
	{
	    if (ReadDiskStream (
		    (Stream *)track->aspss_IOreq,
		    (char *)spool->asp_abx_buf,
		    FILESIZE (abxf->sizes)
		    ) < 0)
		return (0);
		
	    blocks_read += FILESIZE (abxf->sizes);
	    UnCompressAudio (
		    (unsigned char *)spool->asp_abx_buf,
		    bufptr,
		    track->aspss_abx_info.abx_bufsize
		    );
	    
	    track->aspss_abx_info.abx_FrameIdx++;

	    if ((blocks -= UNCOMPSIZE (abxf->sizes)) <= 0)
		break;
	    
	    bufptr += UNCOMPSIZE (abxf->sizes);	    
	    abxf++;
	} while (1);
#else
	int32	byte_offs, read_len;
	IOInfo	info;
	char	*workbufptr;

	byte_offs = track->aspss_next_file_block + track->aspss_start_file_block_offset;
	memset (&info, 0, sizeof (info));

	info.ioi_Command = CMD_READ;
	info.ioi_Recv.iob_Buffer = spool->asp_abx_workbuf;
	if (blocks < ABX_WORK_SIZE)
	    read_len = ((blocks + DEFAULT_BLOCK_SIZE - 1)
		    / DEFAULT_BLOCK_SIZE) * DEFAULT_BLOCK_SIZE;
	else
	    read_len = ABX_WORK_SIZE;
	info.ioi_Recv.iob_Len = read_len;
	info.ioi_Offset = byte_offs / DEFAULT_BLOCK_SIZE;
	if (DoIO (track->aspss_IOreq, &info) < 0)
	    return (0);
	    
	byte_offs %= DEFAULT_BLOCK_SIZE;
	workbufptr = spool->asp_abx_workbuf + byte_offs;

	blocks_read = 0;
	bufptr = (unsigned char *)buffer;
	abxf = &ABX_FRAME_LIST[track->aspss_abx_info.abx_FrameIdx];
	do
	{
	    int32	len;
	    
	    len = FILESIZE (abxf->sizes);
	    if (byte_offs + len > read_len)
	    {
		int32	l;

		// copy what's left from last read
		l = read_len - byte_offs;
		memcpy (spool->asp_abx_buf, spool->asp_abx_workbuf + byte_offs, l);

		byte_offs += l;		
		info.ioi_Offset += byte_offs / DEFAULT_BLOCK_SIZE;
		if (blocks < ABX_WORK_SIZE)
		    read_len = ((blocks + DEFAULT_BLOCK_SIZE - 1)
			    / DEFAULT_BLOCK_SIZE) * DEFAULT_BLOCK_SIZE;
		else
		    read_len = ABX_WORK_SIZE;
		info.ioi_Recv.iob_Len = read_len;
		if (DoIO (track->aspss_IOreq, &info) < 0)
		    return (0);
		len -= l;
		byte_offs %= DEFAULT_BLOCK_SIZE;
		workbufptr = spool->asp_abx_workbuf + byte_offs;
		memcpy ((char *)spool->asp_abx_buf + l, workbufptr, len);
	    }
	    else
		memcpy (spool->asp_abx_buf, workbufptr, len);
				
	    byte_offs += len;
	    workbufptr += len;

	    blocks_read += FILESIZE (abxf->sizes);
	    UnCompressAudio (
		    (unsigned char *)spool->asp_abx_buf,
		    bufptr,
		    track->aspss_abx_info.abx_bufsize
		    );
	    
	    track->aspss_abx_info.abx_FrameIdx++;

	    if ((blocks -= UNCOMPSIZE (abxf->sizes)) <= 0)
		break;
	    
	    bufptr += UNCOMPSIZE (abxf->sizes);	    
	    abxf++;
	} while (1);
#endif

	return (blocks_read);
    }
}

static Item
myCreateSample (SAMPLE_FILE_INFO *aiff_info, int32 buffer_size, void *buffer)
{
	static TagArg	tags_mcs[] =
	{
		{AF_TAG_WIDTH,			(void *) 0}, 	// bytes per frame before compression
		{AF_TAG_CHANNELS,		(void *) 0},	// 1..2
		{AF_TAG_SAMPLE_RATE,		(void *) 0},	// sample rate
		{AF_TAG_COMPRESSIONTYPE,	(void *) 0},	// ID_SDX2, ID_ADP4, ???
		{AF_TAG_NUMBITS,		(void *) 0},	// width of each sample in bits
		{AF_TAG_COMPRESSIONRATIO,	(void *) 0},	// 2 = 2:1

		{AF_TAG_ADDRESS,		(void *) 0},	// ptr to sample data
		{AF_TAG_NUMBYTES,		(void *) 0},	// # of bytes in sample table
		{AF_TAG_FRAMES,			(void *) 0},	// # of data points in sample
		{TAG_END,			NULL}
	};
	Item	sample;
	int	i;
	int32	frames = aiff_info->afi_compression_ratio * buffer_size / aiff_info->afi_channels / aiff_info->afi_width;

	i = 0;
	tags_mcs[i++].ta_Arg = (void *) aiff_info->afi_width;
	tags_mcs[i++].ta_Arg = (void *) aiff_info->afi_channels;
	tags_mcs[i++].ta_Arg = (void *) aiff_info->afi_sample_rate;
	tags_mcs[i++].ta_Arg = (void *) aiff_info->afi_compression_type;
	tags_mcs[i++].ta_Arg = (void *) aiff_info->afi_num_bits;
	tags_mcs[i++].ta_Arg = (void *) aiff_info->afi_compression_ratio;
	tags_mcs[i++].ta_Arg = (void *) buffer;
	tags_mcs[i++].ta_Arg = (void *) buffer_size;
	tags_mcs[i++].ta_Arg = (void *) frames;

	sample = CreateItem (MKNODEID(AUDIONODE,AUDIO_SAMPLE_NODE), tags_mcs);
	
	ON_NOT_ITEM_ABORT(sample, "Couldn't create sample\n");

	return (sample);

ABORT:
	if (IS_ITEM(sample))
		DeleteItem (sample);

	return (NULL_ITEM);
}

static int32
set_attachment_flags (Item attachment, int32 flags)
{
	static TagArg	tags_saf[] =
	{
		{AF_TAG_SET_FLAGS,	(void *) 0},
		{TAG_END,		NULL}
	};

	tags_saf[0].ta_Arg = (void *) (flags);

	return (SetAudioItemInfo (attachment, tags_saf));
}

static int32
stop_currently_playing_attachments (SPOOL *spool)
{
	Item		att;
	int32		buffer_index;
	int32		track_index;
	SPOOL_TRACK	*track;
    
	track_index = spool->asp_current_playing_track_number;
	spool->asp_stopped_track_number = -1;

	track = &(spool->asp_track_info[track_index]);
	track->aspss_cur_file_pos = track->aspss_sample_data_size;

	clrflg (AS_SA_ATTACHMENT_STARTED, spool->asp_flags);
	for (buffer_index = 0; buffer_index < spool->asp_buffer_count; buffer_index ++) 
	{
	    Err			offset;
	    BUFFER_TRACK_INFO	*buf_track_info;
	    SPOOL_BUFFER	*buffer;

	    buffer = &spool->asp_buffers_control[buffer_index];
	    track_index = buffer->asb_track_number;
	    buf_track_info = &buffer->asb_track_info[track_index];
	    att = buf_track_info->asb_attachment_item;
	    
	    if (!(spool->asp_loaded_buffer_signals & buffer->asb_signal))
	    {
		ReleaseAttachment (att, NULL);
		StopAttachment(att, NULL);
		continue;
	    }

	    if ((offset = WhereAttachment (att)) >= 0
		    && offset <= buffer->asb_data_size)
	    {
		track = &spool->asp_track_info[track_index];
		track->aspss_cur_file_pos =
			buf_track_info->asb_file_block
			* track->aspss_block_size
			+ offset;
		spool->asp_stopped_track_number = track_index;
		spool->asp_current_playing_track_number = track_index;
	    }
	    ReleaseAttachment (att, NULL);
	    StopAttachment(att, NULL);
	}

//printf ("Pause 0: track %ld cur_file_pos %ld\n", spool->asp_current_playing_track_number, track->aspss_cur_file_pos);
	return (TRUE);
ABORT:
	track->aspss_cur_file_pos = 0;
//printf ("Pause 1: track %ld cur_file_pos %ld\n", spool->asp_current_playing_track_number, track->aspss_cur_file_pos);
	return (FALSE);
}

static void
tweak_file (SPOOL *spool, int32 dir)
{
#define NUM_BLOCKS	2
#define DATA_SIZE	(NUM_BLOCKS * DEFAULT_BLOCK_SIZE)
    SPOOL_TRACK	*track;
    int32	cur_pos, track_index, delta;

    track_index = spool->asp_current_playing_track_number;	    
    track = &spool->asp_track_info[track_index];
    cur_pos = track->aspss_cur_file_pos;

    if (dir == FORWARD)
    {
	delta = cur_pos + DATA_SIZE - track->aspss_sample_data_size;
	if (delta > 0)
	{
	    track->aspss_cur_file_pos = track->aspss_sample_data_size;
	    if (track_index < spool->asp_track_count - 1)
	    {
		spool->asp_current_playing_track_number++;
		track++;
		track->aspss_cur_file_pos = delta;
	    }
	}
	else
	    track->aspss_cur_file_pos += DATA_SIZE;
    }
    else // REVERSE
    {
	delta = cur_pos - DATA_SIZE;
	if (delta < 0)
	{
	    track->aspss_cur_file_pos = 0;
	    if (track_index > 0)
	    {
		spool->asp_current_playing_track_number--;
		track--;
		track->aspss_cur_file_pos = track->aspss_sample_data_size + delta;
	    }
	}
	else
	    track->aspss_cur_file_pos = delta;
    }
    
    spool->asp_stopped_track_number = -1;
}

static int32
adjust_track (SPOOL *spool, SPOOL_TRACK *track)
{
	if (!chkflg (AS_TR_ABX_SND, track->aspss_flags))
	{
		track->aspss_next_file_block = track->aspss_cur_file_pos / DEFAULT_BLOCK_SIZE;
		track->aspss_cur_file_block_offset = track->aspss_cur_file_pos % DEFAULT_BLOCK_SIZE;
		if (track->aspss_next_file_block < track->aspss_start_file_block)
			track->aspss_next_file_block = track->aspss_start_file_block;
		track->aspss_sample_data_read =
			(track->aspss_next_file_block - track->aspss_start_file_block)
			* DEFAULT_BLOCK_SIZE;
		track->aspss_sample_data_remaining = track->aspss_sample_data_size
			- track->aspss_sample_data_read;
	}
	else
	{
		uint32	i, data_offs;
		
		if (spool->asp_track_count > 1)
		{
#if defined(ABX_STREAM_IO) && !defined(PRE_OPEN_STREAMS)
			if (track->aspss_IOreq == 0)
			{
				track->aspss_IOreq = (Item)OpenDiskStream (
					track->aspss_filename,
					spool->asp_bytes_per_track
					);
				if (track->aspss_IOreq == 0)
				    return (0);
			}
#endif
			if (!load_abx_info (spool, track->aspss_IOreq, 0, 0))
			    return (0);
		}

		track->aspss_abx_info.abx_FrameIdx = 0;

		data_offs = 0;
		for (i = 0; i < track->aspss_abx_info.abx_FrameCt; i++)
		{
		    if (data_offs >= track->aspss_cur_file_pos)
			break;
		    data_offs += UNCOMPSIZE (ABX_FRAME_LIST[i].sizes);
		}
		
		track->aspss_abx_info.abx_FrameIdx = i;

//printf ("adjust: cur_file_pos %ld, next_file_block %ld (FrameIdx %ld -- offs %ld)\n",
//	track->aspss_cur_file_pos, track->aspss_next_file_block, i, ABX_FRAME_LIST[i].fileaddress);
		track->aspss_next_file_block = ABX_FRAME_LIST[i].fileaddress
			- track->aspss_start_file_block_offset;
		track->aspss_cur_file_block_offset = 0;
		track->aspss_sample_data_read = data_offs;
		track->aspss_sample_data_remaining = track->aspss_sample_data_size
			- track->aspss_sample_data_read;
	}
	
	return (1);
}

static void
toggle_connections (SPOOL *spool, SPOOL_TRACK *track, int connect)
{
	if (connect)
	{
		if (spool->asp_channels == 1)
		{
			ConnectInstruments(
				track->aspss_sample_instrument,
				"Output",
				spool->asp_output_instrument,
				"Input0"
				);
			ConnectInstruments(
				track->aspss_sample_instrument,
				"Output",
				spool->asp_output_instrument,
				"Input1"
				);
#ifdef DELAY_STUFF
			ConnectInstruments (
				track->aspss_sample_instrument,
				"Output",
				spool->asp_delay_ins,
				"Input"
				);
#endif
		}
		else
		{
			ConnectInstruments(
				track->aspss_sample_instrument,
				"LeftOutput",
				spool->asp_output_instrument,
				"Input0"
				);
			ConnectInstruments(
				track->aspss_sample_instrument,
				"RightOutput",
				spool->asp_output_instrument,
				"Input1"
				);
#ifdef DELAY_STUFF
			ConnectInstruments (
				track->aspss_sample_instrument,
				"LeftOutput",
				spool->asp_delay_ins,
				"Input"
				);
#endif
		}
	}
	else
	{
		if (spool->asp_channels == 1)
		{
#ifdef DELAY_STUFF
			DisconnectInstruments(
				track->aspss_sample_instrument,
				"Output",
				spool->asp_delay_ins,
				"Input"
				);
#endif
			DisconnectInstruments(
				track->aspss_sample_instrument,
				"Output",
				spool->asp_output_instrument,
				"Input0"
				);
			DisconnectInstruments(
				track->aspss_sample_instrument,
				"Output",
				spool->asp_output_instrument,
				"Input1"
				);
		}
		else
		{
#ifdef DELAY_STUFF
			DisconnectInstruments(
				track->aspss_sample_instrument,
				"LeftOutput",
				spool->asp_delay_ins,
				"Input"
				);
#endif
			DisconnectInstruments(
				track->aspss_sample_instrument,
				"LeftOutput",
				spool->asp_output_instrument,
				"Input0"
				);
			DisconnectInstruments(
				track->aspss_sample_instrument,
				"RightOutput",
				spool->asp_output_instrument,
				"Input1"
				);
		}
	}
}
