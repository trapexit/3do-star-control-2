/*
 * Copyright (c) 1992 The Regents of the University of California.
 * All rights reserved.  
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */
/* This file contains C code that implements
 * the video decoder model.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "decoders.h"
#include "video.h"
#include "util.h"
#include "proto.h"

/* Declarations of functions. */
static void DoPictureDisplay();
static int ParseSeqHead();
static int ParseGOP();
static int ParsePicture();
static int ParseSlice();

char *ditherFlags;

/* Macro for returning 1 if num is positive, -1 if negative, 0 if 0. */

#define Sign(num) ((num > 0) ? 1 : ((num == 0) ? 0 : -1))

/* Declare global pointer to vid stream used for current decoding. */

VidStream *curVidStream = NULL;

/* Set up array for fast conversion from zig zag order to row/column
   coordinates.
*/

int zigzag[64][2] = {
  0, 0, 1, 0, 0, 1, 0, 2, 1, 1, 2, 0, 3, 0, 2, 1, 1, 2, 0, 3, 0, 4, 1, 3,
  2, 2, 3, 1, 4, 0, 5, 0, 4, 1, 3, 2, 2, 3, 1, 4, 0, 5, 0, 6, 1, 5, 2, 4,
  3, 3, 4, 2, 5, 1, 6, 0, 7, 0, 6, 1, 5, 2, 4, 3, 3, 4, 2, 5, 1, 6, 0, 7,
  1, 7, 2, 6, 3, 5, 4, 4, 5, 3, 6, 2, 7, 1, 7, 2, 6, 3, 5, 4, 4, 5, 3, 6,
  2, 7, 3, 7, 4, 6, 5, 5, 6, 4, 7, 3, 7, 4, 6, 5, 5, 6, 4, 7, 5, 7, 6, 6,
7, 5, 7, 6, 6, 7, 7, 7};
/* Array mapping zigzag to array pointer offset. */

int zigzag_direct[64] = {
  0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5, 12,
  19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28, 35,
  42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51,
58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63};
/* Set up array for fast conversion from row/column coordinates to
   zig zag order.
*/

int scan[8][8] = {
  {0, 1, 5, 6, 14, 15, 27, 28},
  {2, 4, 7, 13, 16, 26, 29, 42},
  {3, 8, 12, 17, 25, 30, 41, 43},
  {9, 11, 18, 24, 31, 40, 44, 53},
  {10, 19, 23, 32, 39, 45, 52, 54},
  {20, 22, 33, 38, 46, 51, 55, 60},
  {21, 34, 37, 47, 50, 56, 59, 61},
{35, 36, 48, 49, 57, 58, 62, 63}};
/* Initialize P and B skip flags. */

static int No_P_Flag = 0;
static int No_B_Flag = 0;

/* Max lum, chrom indices for illegal block checking. */

static int lmaxx;
static int lmaxy;
static int cmaxx;
static int cmaxy;

/*
  The following accounts for time and size  spent in various parsing acitivites
  if ANALYSIS has been defined.
*/

int totNumFrames = 0;

/*
 *--------------------------------------------------------------
 *
 * NewVidStream --
 *
 *	Allocates and initializes a VidStream structure. Takes
 *      as parameter requested size for buffer length.
 *
 * Results:
 *	A pointer to the new VidStream structure.
 *
 * Side effects:
 *      None.
 *
 *--------------------------------------------------------------
 */

VidStream *
NewVidStream(bufLength)
  int bufLength;
{
  int i, j;
  VidStream *new;
  static unsigned char default_intra_matrix[64] = {
    8, 16, 19, 22, 26, 27, 29, 34,
    16, 16, 22, 24, 27, 29, 34, 37,
    19, 22, 26, 27, 29, 34, 34, 38,
    22, 22, 26, 27, 29, 34, 37, 40,
    22, 26, 27, 29, 32, 35, 40, 48,
    26, 27, 29, 32, 35, 40, 48, 58,
    26, 27, 29, 34, 38, 46, 56, 69,
  27, 29, 35, 38, 46, 56, 69, 83};

  /* Check for legal buffer length. */

  if (bufLength < 4)
    return NULL;

  /* Make buffer length multiple of 4. */

  bufLength = (bufLength + 3) >> 2;

  /* Allocate memory for new structure. */

  new = (VidStream *) MemAlloc(sizeof(VidStream));

  /* Initialize pointers to extension and user data. */

  new->group.ext_data = new->group.user_data =
    new->picture.extra_info = new->picture.user_data =
    new->picture.ext_data = new->slice.extra_info =
    new->ext_data = new->user_data = NULL;

  /* Copy default intra matrix. */

  for (i = 0; i < 8; i++) {
    for (j = 0; j < 8; j++) {
      new->intra_quant_matrix[j][i] = default_intra_matrix[i * 8 + j];
    }
  }

  /* Initialize crop table. */

  for (i = (-MAX_NEG_CROP); i < NUM_CROP_ENTRIES - MAX_NEG_CROP; i++) {
    extern unsigned char cropTbl[NUM_CROP_ENTRIES];

    if (i <= 0) {
      cropTbl[i + MAX_NEG_CROP] = 0;
    } else if (i >= 255) {
      cropTbl[i + MAX_NEG_CROP] = 255;
    } else {
      cropTbl[i + MAX_NEG_CROP] = i;
    }
  }

  /* Initialize non intra quantization matrix. */

  for (i = 0; i < 8; i++) {
    for (j = 0; j < 8; j++) {
      new->non_intra_quant_matrix[j][i] = 16;
    }
  }

  /* Initialize pointers to image spaces. */

  new->current = new->past = new->future = NULL;
  for (i = 0; i < RING_BUF_SIZE; i++) {
    new->ring[i] = NULL;
  }

  /* Create buffer. */

  new->buf_start = (unsigned int *) MemAlloc(bufLength * 4);

  /*
   * Set max_buf_length to one less than actual length to deal with messy
   * data without proper seq. end codes.
   */

  new->max_buf_length = bufLength - 1;

  /* Initialize bitstream i/o fields. */

  new->bit_offset = 0;
  new->buf_length = 0;
  new->buffer = new->buf_start;


  /* Return structure. */

  return new;
}



/*
 *--------------------------------------------------------------
 *
 * DestroyVidStream --
 *
 *	Deallocates a VidStream structure.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */
void
DestroyVidStream(astream)
  VidStream *astream;
{
  int i;

  if (astream->ext_data != NULL)
    MemFree(astream->ext_data);

  if (astream->user_data != NULL)
    MemFree(astream->user_data);

  if (astream->group.ext_data != NULL)
    MemFree(astream->group.ext_data);

  if (astream->group.user_data != NULL)
    MemFree(astream->group.user_data);

  if (astream->picture.extra_info != NULL)
    MemFree(astream->picture.extra_info);

  if (astream->picture.ext_data != NULL)
    MemFree(astream->picture.ext_data);

  if (astream->picture.user_data != NULL)
    MemFree(astream->picture.user_data);

  if (astream->slice.extra_info != NULL)
    MemFree(astream->slice.extra_info);

  if (astream->buf_start != NULL)
    MemFree(astream->buf_start);

  for (i = 0; i < RING_BUF_SIZE; i++) {
    if (astream->ring[i] != NULL) {
      DestroyPictImage(astream->ring[i]);
      astream->ring[i] = NULL;
    }
  }

  MemFree((char *) astream);
  UninitDisplay ();
}




/*
 *--------------------------------------------------------------
 *
 * NewPictImage --
 *
 *	Allocates and initializes a PictImage structure.
 *      The width and height of the image space are passed in
 *      as parameters.
 *
 * Results:
 *	A pointer to the new PictImage structure.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

PictImage *
NewPictImage(width, height)
  unsigned int width, height;
{
  PictImage *new;

  /* Allocate memory space for new structure. */

  new = (PictImage *) MemAlloc(sizeof(PictImage));


  /* Allocate memory for image spaces. */

  new->luminance = (unsigned char *) MemAlloc(width * height);
  new->Cr = (unsigned char *) MemAlloc(width * height / 4);
  new->Cb = (unsigned char *) MemAlloc(width * height / 4);

  /* Reset locked flag. */

  new->locked = 0;

  /* Return pointer to new structure. */

  return new;
}



/*
 *--------------------------------------------------------------
 *
 * DestroyPictImage --
 *
 *	Deallocates a PictImage structure.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */
void
DestroyPictImage(apictimage)
  PictImage *apictimage;
{
  if (apictimage->luminance != NULL) {
    MemFree(apictimage->luminance);
  }
  if (apictimage->Cr != NULL) {
    MemFree(apictimage->Cr);
  }
  if (apictimage->Cb != NULL) {
    MemFree(apictimage->Cb);
  }
  MemFree(apictimage);
}



/*
 *--------------------------------------------------------------
 *
 * mpegVidRsrc --
 *
 *      Parses bit stream until MB_QUANTUM number of
 *      macroblocks have been decoded or current slice or
 *      picture ends, whichever comes first. If the start
 *      of a frame is encountered, the frame is time stamped
 *      with the value passed in time_stamp. If the value
 *      passed in buffer is not null, the video stream buffer
 *      is set to buffer and the length of the buffer is
 *      expected in value passed in through length. The current
 *      video stream is set to vid_stream. If vid_stream
 *      is passed as NULL, a new VidStream structure is created
 *      and initialized and used as the current video stream.
 *
 * Results:
 *      A pointer to the video stream structure used.
 *
 * Side effects:
 *      Bit stream is irreversibly parsed. If a picture is completed,
 *      a function is called to display the frame at the correct time.
 *
 *--------------------------------------------------------------
 */

VidStream *
mpegVidRsrc(time_stamp, vid_stream)
  TimeStamp time_stamp;
  VidStream *vid_stream;
{
  static int first = 1;
  unsigned int data;
  int i, status;

  /* If vid_stream is null, create new VidStream structure. */

  if (vid_stream == NULL) {
    return NULL;
  }
  /*
   * Set global curVidStream to vid_stream. Necessary because bit i/o use
   * curVidStream and are not passed vid_stream. Also set global bitstream
   * parameters.
   */

  curVidStream = vid_stream;
  bitOffset = curVidStream->bit_offset;
#ifdef UTIL2
  curBits = *curVidStream->buffer << bitOffset;
#else
  curBits = *curVidStream->buffer;
#endif
  bufLength = curVidStream->buf_length;
  bitBuffer = curVidStream->buffer;

  /*
   * If called for the first time, find start code, make sure it is a
   * sequence start code.
   */

  if (first) {
    next_start_code();
    show_bits32(data);
    if (data != SEQ_START_CODE) {
      DestroyVidStream(curVidStream);
      exit(1);
    }
    first = 0;
  }
  /* Get next 32 bits (size of start codes). */

  show_bits32(data);

  /*
   * Process according to start code (or parse macroblock if not a start code
   * at all.
   */

  switch (data) {

  case SEQ_END_CODE:

    /* Display last frame. */

    if (vid_stream->future != NULL) {
      vid_stream->current = vid_stream->future;

  /* Convert to colormap space and dither. */

      DoDitherImage (vid_stream->current->luminance, vid_stream->current->Cr,
	    vid_stream->current->Cb, 0,
	    vid_stream->mb_height, vid_stream->mb_width);
    }
    
    /* Sequence done. Do the right thing. For right now, exit. */

#ifdef ANALYSIS
    PrintAllStats();
#endif

    if (loopFlag)
      longjmp(env, 1);

    DestroyVidStream(curVidStream);
    exit(0);
    break;

  case SEQ_START_CODE:

    /* Sequence start code. Parse sequence header. */

    if (ParseSeqHead(vid_stream) != PARSE_OK)
      goto error;

    /*
     * Return after sequence start code so that application above can use
     * info in header.
     */

    goto done;

  case GOP_START_CODE:

    /* Group of Pictures start code. Parse gop header. */

    if (ParseGOP(vid_stream) != PARSE_OK)
      goto error;


  case PICTURE_START_CODE:

    /* Picture start code. Parse picture header and first slice header. */

    status = ParsePicture(vid_stream, time_stamp);

    if (status == SKIP_PICTURE) {
      next_start_code();
      while (!next_bits(32, PICTURE_START_CODE)) {
	if (next_bits(32, GOP_START_CODE))
	  break;
	else if (next_bits(32, SEQ_END_CODE))
	  break;
	flush_bits(24);
	next_start_code();
      }
      goto done;
    } else if (status != PARSE_OK)
      goto error;


    if (ParseSlice(vid_stream) != PARSE_OK)
      goto error;
    break;

  default:

    /* Check for slice start code. */

    if ((data >= SLICE_MIN_START_CODE) && (data <= SLICE_MAX_START_CODE)) {

      /* Slice start code. Parse slice header. */

      if (ParseSlice(vid_stream) != PARSE_OK)
	goto error;
    }
    break;
  }

  /* Parse next MB_QUANTUM macroblocks. */
  for (i = 0; i < MB_QUANTUM; i++) {

    /* Check to see if actually a startcode and not a macroblock. */

    if (!next_bits(23, 0x00000000)) {

      /* Not start code. Parse Macroblock. */

      if (ParseMacroBlock(vid_stream) != PARSE_OK)
	goto error;

#ifdef ANALYSIS
      if (showmb_flag) {
	DoDitherImage(vid_stream->current->luminance, vid_stream->current->Cr,
		      vid_stream->current->Cb, 0,
		      vid_stream->mb_height, vid_stream->mb_width);
      }
#endif

    } else {

      /* Not macroblock, actually start code. Get start code. */

      next_start_code();
      show_bits32(data);

      /*
       * If start code is outside range of slice start codes, frame is
       * complete, display frame.
       */

      if ((data < SLICE_MIN_START_CODE) || (data > SLICE_MAX_START_CODE)) {

#ifdef ANALYSIS
	EndTime();
	stat_a[0].totsize += bitCountRead() - pictureSizeCount;
	if (showEachFlag) {
	  PrintOneStat();
	};

	CollectStats();
#endif

	DoPictureDisplay(vid_stream);
      }
      break;
    }
  }
  /* Return pointer to video stream structure. */

  goto done;

error:
  next_start_code();
  goto done;

done:

  /* Copy global bit i/o variables back into vid_stream. */

  vid_stream->buffer = bitBuffer;
  vid_stream->buf_length = bufLength;
  vid_stream->bit_offset = bitOffset;

  return vid_stream;

}



/*
 *--------------------------------------------------------------
 *
 * ParseSeqHead --
 *
 *      Assumes bit stream is at the begining of the sequence
 *      header start code. Parses off the sequence header.
 *
 * Results:
 *      Fills the vid_stream structure with values derived and
 *      decoded from the sequence header. Allocates the pict image
 *      structures based on the dimensions of the image space
 *      found in the sequence header.
 *
 * Side effects:
 *      Bit stream irreversibly parsed off.
 *
 *--------------------------------------------------------------
 */

static int
ParseSeqHead(vid_stream)
  VidStream *vid_stream;
{

  unsigned int data;
  int i;

  /* Flush off sequence start code. */

  flush_bits32;

  /* Get horizontal and vertical size of image space. */

  get_bitsn(12 + 12, data);
  vid_stream->h_size = (data >> 12) & ((1 << 12) - 1);
  vid_stream->v_size = data & ((1 << 12) - 1);

  /* Calculate macroblock width and height of image space. */

  vid_stream->mb_width = (vid_stream->h_size + 15) / 16;
  vid_stream->mb_height = (vid_stream->v_size + 15) / 16;

  /* If dither type is MBORDERED allocate ditherFlags. */

  if (ditherType == MBORDERED_DITHER) {
    ditherFlags = (char *) MemAlloc(vid_stream->mb_width*vid_stream->mb_height);
  }

  /* Initialize lmaxx, lmaxy, cmaxx, cmaxy. */

  vid_stream->mb_width <<= 4;
  vid_stream->mb_height <<= 4;
  lmaxx = vid_stream->mb_width-1;
  lmaxy = vid_stream->mb_height-1;
  cmaxx = (vid_stream->mb_width>>1)-1;
  cmaxy = (vid_stream->mb_height>>1)-1;

  /*
   * Initialize ring buffer of pict images now that dimensions of image space
   * are known.
   */

#ifdef SH_MEM
  if (display != NULL) {
    InstallXErrorHandler();
  }
#endif

  if (vid_stream->ring[0] == NULL) {
    for (i = 0; i < RING_BUF_SIZE; i++) {
      vid_stream->ring[i] = NewPictImage(vid_stream->mb_width,
					 vid_stream->mb_height);
    }
  }
#ifdef SH_MEM
  if (display != NULL) {
    DeInstallXErrorHandler();
  }
#endif

  /* Parse of aspect ratio, picture rate code, bit rate. */

  get_bitsn(4 + 4 + 18, data);
  vid_stream->bit_rate = data & ((1 << 18) - 1);
  data >>= 18;
  vid_stream->picture_rate = (unsigned char) (data & ((1 << 4) - 1));
  data >>= 4;
  vid_stream->aspect_ratio = (unsigned char) (data & ((1 << 4) - 1));

  /* Parse off bit rate. */

  get_bits18(data);

  /* Flush marker bit. */

  flush_bits(1);

  /* Parse off vbv buffer size. */
  /* Parse off constrained parameter flag. */
  /* Parse off intra_quant_matrix_flag. */

  get_bits12(data);
  vid_stream->vbv_buffer_size = data >> 2;

  if (data & (1 << 1)) {
    vid_stream->const_param_flag = TRUE;
  } else
    vid_stream->const_param_flag = FALSE;

  /*
   * If intra_quant_matrix_flag set, parse off intra quant matrix values.
   */

  if (data & (1 << 0)) {
    for (i = 0; i < 64; i++) {
      get_bits8(data);

      vid_stream->intra_quant_matrix[zigzag[i][1]][zigzag[i][0]] =
	(unsigned char) data;
    }
  }
  /*
   * If non intra quant matrix flag set, parse off non intra quant matrix
   * values.
   */

  get_bits1(data);
  if (data) {
    for (i = 0; i < 64; i++) {
      get_bits8(data);

      vid_stream->non_intra_quant_matrix[zigzag[i][1]][zigzag[i][0]] =
	(unsigned char) data;
    }
  }
  /* Go to next start code. */

  next_start_code();

  /*
   * If next start code is extension start code, parse off extension data.
   */

  show_bits32 (data);
  if (data == EXT_START_CODE) {
    flush_bits32;
    if (vid_stream->ext_data != NULL) {
      MemFree(vid_stream->ext_data);
      vid_stream->ext_data = NULL;
    }
    vid_stream->ext_data = get_ext_data();
    show_bits32 (data);
  }
  /* If next start code is user start code, parse off user data. */

  if (data == USER_START_CODE) {
    flush_bits32;
    if (vid_stream->user_data != NULL) {
      MemFree(vid_stream->user_data);
      vid_stream->user_data = NULL;
    }
    vid_stream->user_data = get_ext_data();
  }
  return PARSE_OK;
}



/*
 *--------------------------------------------------------------
 *
 * ParseGOP --
 *
 *      Parses of group of pictures header from bit stream
 *      associated with vid_stream.
 *
 * Results:
 *      Values in gop header placed into video stream structure.
 *
 * Side effects:
 *      Bit stream irreversibly parsed.
 *
 *--------------------------------------------------------------
 */

static int
ParseGOP(vid_stream)
  VidStream *vid_stream;
{
  unsigned int data;

  /* Flush group of pictures start code. WWWWWWOOOOOOOSSSSSSHHHHH!!! */

  flush_bits32;

  /* Parse off drop frame flag. */

  get_bits1(data);
  if (data) {
    vid_stream->group.drop_flag = TRUE;
  } else
    vid_stream->group.drop_flag = FALSE;

  /* Parse off hour component of time code. */

  get_bits5(data);
  vid_stream->group.tc_hours = data;

  /* Parse off minute component of time code. */

  get_bits6(data);
  vid_stream->group.tc_minutes = data;

  /* Flush marker bit. */

  flush_bits(1);

  /* Parse off second component of time code. */

  get_bits6(data);
  vid_stream->group.tc_seconds = data;

  /* Parse off picture count component of time code. */

  get_bits6(data);
  vid_stream->group.tc_pictures = data;

  /* Parse off closed gop and broken link flags. */

  get_bits2(data);
  if (data > 1) {
    vid_stream->group.closed_gop = TRUE;
    if (data > 2) {
      vid_stream->group.broken_link = TRUE;
    } else
      vid_stream->group.broken_link = FALSE;
  } else {
    vid_stream->group.closed_gop = FALSE;
    if (data) {
      vid_stream->group.broken_link = TRUE;
    } else
      vid_stream->group.broken_link = FALSE;
  }

  /* Goto next start code. */

  next_start_code();

  /* If next start code is extension data, parse off extension data. */

  show_bits32 (data);
  if (data == EXT_START_CODE) {
    flush_bits32;
    if (vid_stream->group.ext_data != NULL) {
      MemFree(vid_stream->group.ext_data);
      vid_stream->group.ext_data = NULL;
    }
    vid_stream->group.ext_data = get_ext_data();
    show_bits32 (data);
  }
  /* If next start code is user data, parse off user data. */

  if (data == USER_START_CODE) {
    flush_bits32;
    if (vid_stream->group.user_data != NULL) {
      MemFree(vid_stream->group.user_data);
      vid_stream->group.user_data = NULL;
    }
    vid_stream->group.user_data = get_ext_data();
  }
  return PARSE_OK;
}



/*
 *--------------------------------------------------------------
 *
 * ParsePicture --
 *
 *      Parses picture header. Marks picture to be presented
 *      at particular time given a time stamp.
 *
 * Results:
 *      Values from picture header put into video stream structure.
 *
 * Side effects:
 *      Bit stream irreversibly parsed.
 *
 *--------------------------------------------------------------
 */

static int
ParsePicture(vid_stream, time_stamp)
  VidStream *vid_stream;
  TimeStamp time_stamp;
{
  unsigned int data;
  int i;

  /* Flush header start code. */
  flush_bits32;

  /* Parse off temporal reference and picture type. */
  get_bitsn(10 + 3, data);
  vid_stream->picture.code_type = data & ((1 << 3) - 1);
  vid_stream->picture.temp_ref = data >> 3;

  if ((vid_stream->picture.code_type == B_TYPE) &&
      (No_B_Flag ||
	   (vid_stream->past == NULL) ||
	   (vid_stream->future == NULL)))
    return SKIP_PICTURE;

  if ((vid_stream->picture.code_type == P_TYPE) &&
      (No_P_Flag || (vid_stream->future == NULL)))
    return SKIP_PICTURE;

#ifdef ANALYSIS
  StartTime();
  stat_a[0].frametype = vid_stream->picture.code_type;
  stat_a[0].number = 1;
  stat_a[0].totsize = 45;
  pictureSizeCount = bitCountRead();
#endif

  /* Parse off vbv buffer delay value. */
  get_bits16(data);
  vid_stream->picture.vbv_delay = data;

  /* If P or B type frame... */

  if ((vid_stream->picture.code_type == P_TYPE)
	  || (vid_stream->picture.code_type == B_TYPE)) {

    /* Parse off forward vector full pixel flag and forw_r_code */
    get_bitsn(1 + 3, data);

    /* Decode forw_r_code into forw_r_size and forw_f. */
    vid_stream->picture.forw_r_size = (data & ((1 << 3) - 1)) - 1;
    vid_stream->picture.forw_f = (1 << vid_stream->picture.forw_r_size);
    if (data & (1 << 3))
      vid_stream->picture.full_pel_forw_vector = TRUE;
    else
      vid_stream->picture.full_pel_forw_vector = FALSE;

  /* If B type frame... */

  if (vid_stream->picture.code_type == B_TYPE) {

    /* Parse off back vector full pixel flag and back_r_code. */
    get_bitsn(1 + 3, data);

    /* Decode back_r_code into back_r_size and back_f. */
    vid_stream->picture.back_r_size = (data & ((1 << 3) - 1)) - 1;
    vid_stream->picture.back_f = (1 << vid_stream->picture.back_r_size);
    if (data & (1 << 3))
      vid_stream->picture.full_pel_back_vector = TRUE;
    else
      vid_stream->picture.full_pel_back_vector = FALSE;
  }
  }
  /* Get extra bit picture info. */

  if (vid_stream->picture.extra_info != NULL) {
    MemFree(vid_stream->picture.extra_info);
    vid_stream->picture.extra_info = NULL;
  }
  vid_stream->picture.extra_info = get_extra_bit_info();

  /* Goto next start code. */
  next_start_code();

  /* If start code is extension start code, parse off extension data. */

  show_bits32 (data);
  if (data == EXT_START_CODE) {
    flush_bits32;

    if (vid_stream->picture.ext_data != NULL) {
      MemFree(vid_stream->picture.ext_data);
      vid_stream->picture.ext_data = NULL;
    }
    vid_stream->picture.ext_data = get_ext_data();
    show_bits32 (data);
  }
  /* If start code is user start code, parse off user data. */

  if (data == USER_START_CODE) {
    flush_bits32;

    if (vid_stream->picture.user_data != NULL) {
      MemFree(vid_stream->picture.user_data);
      vid_stream->picture.user_data = NULL;
    }
    vid_stream->picture.user_data = get_ext_data();
  }
  /* Find a pict image structure in ring buffer not currently locked. */

  i = 0;

  while (vid_stream->ring[i]->locked != 0) {
    if (++i >= RING_BUF_SIZE) {
      perror("Fatal error. Ring buffer full.");
      exit(1);
    }
  }

  /* Set current pict image structure to the one just found in ring. */

  vid_stream->current = vid_stream->ring[i];

  /* Set time stamp. */

  vid_stream->current->show_time = time_stamp;

  /* Reset past macroblock address field. */

  vid_stream->mblock.past_mb_addr = -1;

  return PARSE_OK;
}



/*
 *--------------------------------------------------------------
 *
 * ParseSlice --
 *
 *      Parses off slice header.
 *
 * Results:
 *      Values found in slice header put into video stream structure.
 *
 * Side effects:
 *      Bit stream irreversibly parsed.
 *
 *--------------------------------------------------------------
 */

static int
ParseSlice(vid_stream)
  VidStream *vid_stream;
{
  unsigned int data;

  /* Flush slice start code. */

  flush_bits(24);

  /* Parse off slice vertical position. */

  get_bitsn(8 + 5, data);
  vid_stream->slice.vert_pos = data >> 5;
  vid_stream->slice.quant_scale = data & ((1 << 5) - 1);

  /* Parse off extra bit slice info. */

  if (vid_stream->slice.extra_info != NULL) {
    MemFree(vid_stream->slice.extra_info);
    vid_stream->slice.extra_info = NULL;
  }
  vid_stream->slice.extra_info = get_extra_bit_info();

  /* Reset past intrablock address. */

  vid_stream->mblock.past_intra_addr = -2;

  /* Reset previous recon motion vectors. */

  vid_stream->mblock.recon_right_for_prev = 0;
  vid_stream->mblock.recon_down_for_prev = 0;
  vid_stream->mblock.recon_right_back_prev = 0;
  vid_stream->mblock.recon_down_back_prev = 0;

  /* Reset macroblock address. */

  vid_stream->mblock.mb_address = ((vid_stream->slice.vert_pos - 1) *
				   (vid_stream->mb_width>>4)) - 1;

  /* Reset past dct dc y, cr, and cb values. */

  vid_stream->block.dct_dc_y_past = 1024;
  vid_stream->block.dct_dc_cr_past = 1024;
  vid_stream->block.dct_dc_cb_past = 1024;

  return PARSE_OK;
}

/*
 *--------------------------------------------------------------
 *
 * DoPictureDisplay --
 *
 *	Converts image from Lum, Cr, Cb to colormap space. Puts
 *      image in lum plane. Updates past and future frame
 *      pointers. Dithers image. Sends to display mechanism.
 *
 * Results:
 *	Pict image structure locked if displaying or if frame
 *      is needed as past or future reference.
 *
 * Side effects:
 *	Lum plane pummelled.
 *
 *--------------------------------------------------------------
 */

static void
DoPictureDisplay(vid_stream)
  VidStream *vid_stream;
{
//  /* Convert to colormap space and dither. */
//
//    DoDitherImage(vid_stream->current->luminance, vid_stream->current->Cr,
//	    vid_stream->current->Cb, 0,
//	    vid_stream->mb_height, vid_stream->mb_width);

  /* Update past and future references if needed. */

    if ((vid_stream->picture.code_type == I_TYPE) || (vid_stream->picture.code_type == P_TYPE))
    {
	if (vid_stream->future == NULL)
	{
	    vid_stream->future = vid_stream->current;
	    vid_stream->future->locked |= FUTURE_LOCK;
	    return;
	}
	else
	{
	    if (vid_stream->past != NULL)
	    {
		vid_stream->past->locked &= ~PAST_LOCK;
	    }
	    vid_stream->past = vid_stream->future;
	    vid_stream->past->locked &= ~FUTURE_LOCK;
	    vid_stream->past->locked |= PAST_LOCK;
	    vid_stream->future = vid_stream->current;
	    vid_stream->future->locked |= FUTURE_LOCK;
	    vid_stream->current = vid_stream->past;
	}
    }

  /* Convert to colormap space and dither. */

    DoDitherImage (vid_stream->current->luminance, vid_stream->current->Cr,
	    vid_stream->current->Cb, 0,
	    vid_stream->mb_height, vid_stream->mb_width);
//    ExecuteDisplay (0,
//	    vid_stream->mb_width,
//	    vid_stream->mb_height);
}



/*
 *--------------------------------------------------------------
 *
 * ToggleBFlag --
 *
 *	Called to set no b frame processing flag.
 *
 * Results:
 *      No_B_Flag flag is toggled from present value to opposite value.
 *
 * Side effects:
 *      None.
 *
 *--------------------------------------------------------------
 */

void
ToggleBFlag()
{
  if (No_B_Flag) {
    No_B_Flag = 0;
  } else
    No_B_Flag = 1;
}




/*
 *--------------------------------------------------------------
 *
 * TogglePFlag --
 *
 *	Called to set no p frame processing flag.
 *
 * Results:
 *      No_P_Flag flag is toggled from present value to opposite value.
 *
 * Side effects:
 *      None.
 *
 *--------------------------------------------------------------
 */

void
TogglePFlag()
{
  if (No_P_Flag) {
    No_P_Flag = 0;
  } else
    No_P_Flag = 1;
}
