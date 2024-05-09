#define AIFF_FILE_LEN		200
#define PLR_MAX_TRACKS		10
#define PLR_MAX_BUFFERS		16

typedef struct
{
    int32	sample_rate;
    int32	channels;
    int32	width;
    int32	compression_type;

    int32	num_bits;
    int32	compression_ratio;

    int32	data_size;
    char	*dsp_filename;
} AIFF_INFO;

typedef struct
{
    char	filename[AIFF_FILE_LEN];
    Item	sample_ins;
    Item	output_ins;
    Item	left_vol_knob, right_vol_knob;
    Item	file;
    Item	IOreq;
    int32	sample_data_size;
    int32	sample_data_read;
    int32	sample_data_remaining;
    int32	next_file_block;
    int32	start_file_block;
    int32	start_file_block_offset;
    int32	cur_file_block;
    int32	last_file_pos;
    int32	frame_multiplier_x4;
} TRACK;

typedef struct
{
    int32	buf_size;
    void	*buf;
    Item	cue;
    int32	cue_sig;
    Item	attachment[PLR_MAX_TRACKS];
    Item	sample[PLR_MAX_TRACKS];
} BUFFER_CTL;

typedef struct
{
    void	*buf;
    int32	buf_ct;
    int32	buf_size;
    char	*filename;
    Item	child, parent;
    int32	child_sig, parent_sig;
    int32	which;
    int32	left_vol, right_vol;
    int32	track_idx;
    TRACK	tracks[PLR_MAX_TRACKS];
    BUFFER_CTL	buf_ctl[PLR_MAX_BUFFERS];
    int32	buf_sigs;
} PLAYER;

enum
{
    PLR_LINEAR,
    PLR_LOOPED
};

enum
{
    PLR_UNDEFINED = 0,
    PLR_SPLICE,
    PLR_PLAY,
    PLR_ADVANCE,
    PLR_FAST_FORWARD,
    PLR_FAST_REVERSE,
    PLR_PAUSE,
    PLR_RESUME,
    PLR_REWIND,
    PLR_VOLUME
};
