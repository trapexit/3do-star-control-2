#include "types.h"
#include "debug.h"
#include "operror.h"
#include "filefunctions.h"
#include "audio.h"
#include "audiox.h"
#include "music.h"
#include "aiffplay.h"

#define BLOCK_SIZE	2048

#ifndef ID_SSND
#define ID_SSND	MAKE_ID('S','S','N','D')
#endif
#ifndef ID_FORM
#define ID_FORM	MAKE_ID('F','O','R','M')
#endif

static PLAYER	Player;

static char *
optimal_dsp_filename (
	int32	sample_rate_16_16,			// 0x56220000 = 22KHz, 0xAC440000 = 44KHz
	int32	channels,					// 1 = mono, 2 = stereo
	int32	sample_byte_width,			// 1 = 8bit, 2 = 16bit
	int32	compression_type)			// ID_SDX2, ID_ADP4 (not supported yet), NULL = none
{
	static char *dsp_names[] = { // [44KHz | 22KHz] [16 bit | 8 bit] [stereo | mono] [compressed | uncompressed]
		"halfmono8.dsp",			// 22k 8 mono uncompressed
		NULL,						// 22k 8 mono compressed
		"halfstereo8.dsp",			// 22k 8 stereo uncompressed
		NULL,						// 22k 8 stereo compressed
		"halfmonosample.dsp",		// 22k 16 mono uncompressed
		"dcsqxdhalfmono.dsp",		// 22k 16 mono compressed
		"halfstereosample.dsp",		// 22k 16 stereo uncompressed
		"dcsqxdhalfstereo.dsp",		// 22k 16 stereo compressed
		"fixedmono8.dsp",			// 44k 8 mono uncompressed
		NULL,						// 44k 8 mono compressed
		"fixedstereo8.dsp",			// 44k 8 stereo uncompressed
		NULL,						// 44k 8 stereo compressed
		"fixedmonosample.dsp",		// 44k 16 mono uncompressed
		"dcsqxdmono.dsp",			// 44k 16 mono compressed
		"fixedstereosample.dsp",	// 44k 16 stereo uncompressed
		"dcsqxdstereo.dsp"			// 44k 16 stereo compressed
	};

#define HERTZ_44100_16_16	(0xAC440000L)
#define ODF_COMPRESSED	(0)
#define ODF_STEREO		(1)
#define ODF_16BIT		(2)
#define ODF_44KHZ		(3)

	int		index	= 0;

	if (sample_rate_16_16 >= HERTZ_44100_16_16) {
		index	|= (1<<ODF_44KHZ);
	}

	if (channels == 2) {
		index	|= (1<<ODF_STEREO);
	}

	if (sample_byte_width == 2) {
		index	|= (1<<ODF_16BIT);
	}
	
	if (compression_type == ID_SDX2) {
		index	|= (1<<ODF_COMPRESSED);
	}

	return (dsp_names[index]);
}

static int32	get_sample_info (char *file_name, AIFF_INFO *aiff_info) {
	static TagArg	tp[] = {
		{AF_TAG_SAMPLE_RATE, 0},
		{AF_TAG_CHANNELS, 0},
		{AF_TAG_WIDTH, 0},
		{AF_TAG_COMPRESSIONTYPE, 0},
		{AF_TAG_DATA_SIZE, 0},
		{AF_TAG_NUMBITS, 0},					// width of each sample in bits
		{AF_TAG_COMPRESSIONRATIO, 0},			// 2 = 2:1

		TAG_END, 0
	};
	int32	result;
	Item	sample	= ScanSample (file_name, 0L);

	result	= GetAudioItemInfo (sample, tp);

	UnloadSample (sample);

	aiff_info->sample_rate			= (int32) tp[0].ta_Arg;
	aiff_info->channels			= (int32) tp[1].ta_Arg;
	aiff_info->width				= (int32) tp[2].ta_Arg;
	aiff_info->compression_type	= (int32) tp[3].ta_Arg;
	aiff_info->data_size			= (int32) tp[4].ta_Arg;
	aiff_info->num_bits			= (int32) tp[5].ta_Arg;
	aiff_info->compression_ratio	= (int32) tp[6].ta_Arg;

	aiff_info->dsp_filename		=	optimal_dsp_filename (
													aiff_info->sample_rate,
													aiff_info->channels,
													aiff_info->width,
													aiff_info->compression_type
												);
	return (TRUE);
}

static int32	get_sample_position (
	int32	*file_block,
	int32	*block_offset,
	char	*filename)
{
typedef struct {
	int32	chunk_id;
	int32	chunk_size;
} chunk_header_type;
	int32				result;
	chunk_header_type	chunk_info;
	int32				form_name;
	int32				file_offset;
	Stream				*infile;

	infile	= OpenDiskStream (filename, 0L);

	ReadDiskStream (infile, (char *) &chunk_info, sizeof(chunk_header_type));
	ReadDiskStream (infile, (char *) &form_name, sizeof(int32));

	do
	{
		ReadDiskStream (infile, (char *) &chunk_info, sizeof(chunk_header_type));

		if (chunk_info.chunk_id != ID_SSND) {
			// make chunk size even
			if (chunk_info.chunk_size & 1L) chunk_info.chunk_size += 1;
		
			file_offset = SeekDiskStream (infile, chunk_info.chunk_size, SEEK_CUR);
		}
	} while (chunk_info.chunk_id != ID_SSND);
	file_offset = SeekDiskStream (infile, 0, SEEK_CUR);

	CloseDiskStream (infile);
	
	*file_block	= file_offset / BLOCK_SIZE;
	*block_offset	= file_offset % BLOCK_SIZE;

	return (TRUE);
}

static void
rewind_track (TRACK *track)
{
    track->sample_data_read = 0L;
    track->sample_data_remaining = track->sample_data_size;
    track->next_file_block = track->start_file_block;
    track->cur_file_block = track->start_file_block;
    track->last_file_pos = 0;
}

static void
splice_track (PLAYER *info)
{
    int32	track_number;
    TRACK	*cur_track;
    AIFF_INFO	aiff_info;
    TagArg	targ[2];
    int		i;

	if (info->track_idx == PLR_MAX_TRACKS)
	    track_number = 0;
	else
	    track_number = info->track_idx + 1;
	    
	cur_track = &info->tracks[track_number];

	get_sample_info (info->filename, &aiff_info);

	cur_track->sample_ins = LoadInstrument (aiff_info.dsp_filename, 0, 100);
	cur_track->output_ins	= LoadInstrument ("mixer4x2.dsp", 0, 0);

	if (aiff_info.channels == 1)
	{
		cur_track->left_vol_knob	= GrabKnob (cur_track->output_ins, "LeftGain0");
		cur_track->right_vol_knob	= GrabKnob (cur_track->output_ins, "RightGain0");
		TweakKnob (cur_track->left_vol_knob, 0x0000); 
		TweakKnob (cur_track->right_vol_knob, 0x0000);
	}
	else
	{
		cur_track->left_vol_knob	= GrabKnob (cur_track->output_ins, "LeftGain1");
		cur_track->right_vol_knob	= GrabKnob (cur_track->output_ins, "RightGain0");

		TweakKnob (cur_track->left_vol_knob, 0x0000); 
		TweakKnob (cur_track->right_vol_knob, 0x0000);

		ReleaseKnob (cur_track->left_vol_knob);
		ReleaseKnob (cur_track->right_vol_knob);

		cur_track->left_vol_knob = GrabKnob (cur_track->output_ins, "LeftGain0");
		cur_track->right_vol_knob = GrabKnob (cur_track->output_ins, "RightGain1");
	
		TweakKnob (cur_track->left_vol_knob, 0x0000); 
		TweakKnob (cur_track->right_vol_knob, 0x0000);
	}

	strcpy (cur_track->filename, info->filename);

	get_sample_position (
			&(cur_track->start_file_block),
			&(cur_track->start_file_block_offset),
			info->filename);

	cur_track->file	= OpenDiskFile (info->filename);
	
	targ[0].ta_Tag = CREATEIOREQ_TAG_DEVICE;
	targ[0].ta_Arg = (void *) cur_track->file;
	targ[1].ta_Tag = TAG_END;

	cur_track->IOreq = CreateItem(MKNODEID(KERNELNODE,IOREQNODE),targ);

		for (i = 0; i < info->buf_ct; i ++)
		{
			BUFFER_CTL	*buf_ctl;
			
			buf_ctl = &info->buf_ctl[i];

			buf_ctl->sample[track_number] = myCreateSample (&aiff_info, info->buf_size, buf_ctl->buf);
			buf_ctl->attachment[track_number] = AttachSample(cur_track->sample_ins, buf_ctl->sample[track_number], 0);
			set_attachment_flags (buf_ctl->attachment[track_number], AF_ATTF_NOAUTOSTART);
		}

		if (aiff_info.channels == 1)
			ConnectInstruments(cur_track->sample_ins, "Output", cur_track->output_ins, "Input0");
		else
		{
			ConnectInstruments(cur_track->sample_ins, "LeftOutput", cur_track->output_ins, "Input0");
			ConnectInstruments(cur_track->sample_ins, "RightOutput", cur_track->output_ins, "Input1");
		}

		for (i = 0; i < info->buf_ct; i++)
		{
			BUFFER_CTL	*this, *next;
			
			this = &info->buf_ctl[i];
			next = &info->buf_ctl[(i + 1) % info->buf_ct];

			LinkAttachments (this->attachment[track_number], next->attachment[track_number]);
			MonitorAttachment (this->attachment[track_number], info->buf_ctl[i].cue, CUE_AT_END);
		}

	cur_track->sample_data_size = aiff_info.data_size;
	cur_track->sample_data_read = 0L;
	cur_track->frame_multiplier_x4	=
		(4 * aiff_info.compression_ratio) / aiff_info.channels / aiff_info.width;

	rewind_track (cur_track);
	
//	load_sample_buffers(spool_player->asp_signals,track_number,spool_player,NULL));

//	spool_player->asp_track_count++;
//	INC_CIRCULAR(spool_player->asp_current_load_track_number, MAX_LOADED_TRACKS);
}

static void
play_track (PLAYER *info)
{
}

void
init_player (PLAYER *info)
{
    int 	i, j;
    char	*bufp;
    BUFFER_CTL	*buf_ctl;
    TRACK	*track;

    bufp = (char *)info->buf;
    info->buf_sigs = 0;
    buf_ctl = info->buf_ctl;
    for (i = 0; i < info->buf_ct; i++, buf_ctl++)
    {
	buf_ctl->buf_size = 0L;
	buf_ctl->buf = bufp;
	buf_ctl->cue = CreateItem (MKNODEID (AUDIONODE, AUDIO_CUE_NODE), NULL);
	buf_ctl->cue_sig = GetCueSignal (buf_ctl->cue);

	info->buf_sigs |= buf_ctl->cue_sig;
	
	bufp += info->buf_size;
    }
    
    track = info->tracks;
    for (i = 0; i < PLR_MAX_TRACKS; i++, buf_ctl++, track++)
    {
	track->sample_ins = track->output_ins = track->left_vol_knob
		= track->right_vol_knob = track->file = track->IOreq = 0;

	for (j = 0; j < info->buf_ct; j++)
	    info->buf_ctl[j].sample[i] = info->buf_ctl[j].attachment[i] = 0;
    }
}

static void
player_proc ()
{
    if (OpenAudioFolio ())
	return;
	
    init_player (&Player);

    Player.child_sig = AllocSignal (0);

    while (1)
    {
	int32	sigs;
	
	sigs = WaitSignal (Player.child_sig);
	
	if (sigs & Player.child_sig)
	{
	    switch (Player.which)
	    {
		case PLR_SPLICE:
		    splice_track (&Player);
		    break;
		case PLR_PLAY:
		    play_track (&Player);
		    break;
#if 0
		case PLR_ADVANCE:
		    advance_track (&Player);
		    break;
		case PLR_FAST_FORWARD:
		    ff_track (&Player);
		    break;
		case PLR_FAST_REVERSE:
		    fr_track (&Player);
		    break;
		case PLR_PAUSE:
		    pause_track (&Player);
		    break;
		case PLR_RESUME:
		    resume_track (&Player);
		    break;
		case PLR_REWIND:
		    rewind_track (&Player);
		    break;
		case PLR_VOLUME:
		    volume_track (&Player);
		    break;
#endif
	    }
	    
	    Player.which = PLR_UNDEFINED;
	}
	
	SendSignal (Player.parent, Player.parent_sig);
    }
    
    CloseAudioFolio ();
    WaitSignal (0);
}

int32
OpenTrackPlayer (int32 priority, int32 buf_ct, int32 buf_size, void *buf)
{
    Player.buf = buf;
    Player.buf_ct = buf_ct;
    Player.buf_size = buf_size;
	
    Player.left_vol = Player.right_vol = 0x7fff;

    Player.parent = KernelBase->kb_CurrentTask->t.n_Item;
    Player.parent_sig = AllocSignal (0);

    Player.child = CreateThread ("aiff_player", priority, player_proc, 4096);
    
    ControlMem (buf, buf_size, MEMC_OKWRITE, Player.child);
    
    WaitSignal (Player.parent_sig);
	
    return (TRUE);
}

static void
send_child_msg ()
{
    SendSignal (Player.child, Player.child_sig);
}

void
LinearTrackPlayer (void)
{
//    Player.mode = PLR_LINEAR;
}

void
LoopedTrackPlayer (void)
{
//    Player.mode = PLR_LOOPED;
}

void
SpliceTrack (char *filespec)
{
    Player.which = PLR_SPLICE;
    Player.filename = filespec;
    
    send_child_msg ();
}

void
PlayTrack ()
{
    Player.which = PLR_PLAY;

    send_child_msg ();
}

void
AdvanceTrack ()
{
    Player.which = PLR_ADVANCE;

    send_child_msg ();
}

void
FastForward ()
{
    Player.which = PLR_FAST_FORWARD;

    send_child_msg ();
}

void
FastReverse ()
{
    Player.which = PLR_FAST_REVERSE;

    send_child_msg ();
}

void
PauseTrack ()
{
    Player.which = PLR_PAUSE;

    send_child_msg ();
}

void
ResumeTrack ()
{
    Player.which = PLR_RESUME;

    send_child_msg ();
}

void
RewindTrack ()
{
    Player.which = PLR_REWIND;

    send_child_msg ();
}


void
VolumeTrack (int32 left_vol, int32 right_vol)
{
    Player.which = PLR_VOLUME;
    Player.left_vol = left_vol;
    Player.right_vol = right_vol;

    send_child_msg ();
}

void
CloseTrackPlayer ()
{
    PauseTrack ();
    CloseSpooledAudio (&Player);
    DeleteThread (Player.child);
}

