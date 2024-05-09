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
#ifdef MIPS
#include "types.h"
#include "system.h"
#endif

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "setjmp.h"

#ifndef X_WINDOWS
typedef int	Display;
typedef int	Visual;
typedef int	Window;
typedef int	XErrorEvent;
#else
#include "X11/Xlib.h"
#include "X11/Xutil.h"
#endif

#ifdef SH_MEM
#include "sys/ipc.h"
#include "sys/shm.h"
#include "X11/extensions/XShm.h"
#endif

/* Define Parsing error codes. */

#define SKIP_PICTURE -10
#define SKIP_TO_START_CODE -1
#define PARSE_OK 1

/* Define BOOLEAN, TRUE, and FALSE. */

#ifndef BOOLEAN
#define BOOLEAN int
#endif
#ifndef TRUE
#define TRUE	1
#endif
#ifndef FALSE
#define FALSE	0
#endif

/* Set ring buffer size. */

#define RING_BUF_SIZE 5

/* Macros for picture code type. */

#define I_TYPE 1
#define P_TYPE 2
#define B_TYPE 3
#define D_TYPE 4

/* Start codes. */

#define SEQ_END_CODE 0x000001b7
#define SEQ_START_CODE 0x000001b3
#define GOP_START_CODE 0x000001b8
#define PICTURE_START_CODE 0x00000100
#define SLICE_MIN_START_CODE 0x00000101
#define SLICE_MAX_START_CODE 0x000001af
#define EXT_START_CODE 0x000001b5
#define USER_START_CODE 0x000001b2

/* Number of macroblocks to process in one call to mpegVidRsrc. */

#define MB_QUANTUM 100

/* Macros used with macroblock address decoding. */

#define MB_STUFFING 34
#define MB_ESCAPE 35

/* Lock flags for pict images. */

#define DISPLAY_LOCK 0x01
#define PAST_LOCK 0x02
#define FUTURE_LOCK 0x04

#define HYBRID_DITHER 0
#define HYBRID2_DITHER 1
#define FS4_DITHER 2
#define FS2_DITHER 3
#define FS2FAST_DITHER 4
#define Twox2_DITHER 5
#define GRAY_DITHER 6
#define FULL_COLOR_DITHER 7
#define NO_DITHER 8
#define ORDERED_DITHER 9
#define MONO_DITHER 10
#define MONO_THRESHOLD 11
#define ORDERED2_DITHER 12
#define MBORDERED_DITHER 13

/* External declaration of row,col to zig zag conversion matrix. */

extern int scan[][8];

/* Temporary definition of time stamp structure. */

typedef int TimeStamp;

/* Structure with reconstructed pixel values. */

typedef struct pict_image {
  unsigned char *luminance;              /* Luminance plane.   */
  unsigned char *Cr;                     /* Cr plane.          */
  unsigned char *Cb;                     /* Cb plane.          */
  int locked;                            /* Lock flag.         */
  TimeStamp show_time;                   /* Presentation time. */

#ifdef SH_MEM
  XShmSegmentInfo shminfo;               /* Segment info.  */
  XImage *ximage;                        /* Ximage struct. */
#endif

} PictImage;

/* Group of pictures structure. */

typedef struct GoP {
  BOOLEAN drop_flag;                     /* Flag indicating dropped frame. */
  unsigned int tc_hours;                 /* Hour component of time code.   */
  unsigned int tc_minutes;               /* Minute component of time code. */
  unsigned int tc_seconds;               /* Second component of time code. */
  unsigned int tc_pictures;              /* Picture counter of time code.  */
  BOOLEAN closed_gop;                    /* Indicates no pred. vectors to
					    previous group of pictures.    */
  BOOLEAN broken_link;                   /* B frame unable to be decoded.  */
  char *ext_data;                        /* Extension data.                */
  char *user_data;                       /* User data.                     */
} GoP;

/* Picture structure. */

typedef struct pict {
  unsigned int temp_ref;                 /* Temporal reference.             */
  unsigned int code_type;                /* Frame type: P, B, I             */
  unsigned int vbv_delay;                /* Buffer delay.                   */
  BOOLEAN full_pel_forw_vector;          /* Forw. vectors specified in full
					    pixel values flag.              */
  unsigned int forw_r_size;              /* Used for vector decoding.       */
  unsigned int forw_f;                   /* Used for vector decoding.       */
  BOOLEAN full_pel_back_vector;          /* Back vectors specified in full 
					    pixel values flag.              */
  unsigned int back_r_size;              /* Used in decoding.               */
  unsigned int back_f;                   /* Used in decoding.               */
  char *extra_info;                      /* Extra bit picture info.         */
  char *ext_data;                        /* Extension data.                 */
  char *user_data;                       /* User data.                      */
} Pict;

/* Slice structure. */

typedef struct slice {
  unsigned int vert_pos;                 /* Vertical position of slice. */
  unsigned int quant_scale;              /* Quantization scale.         */
  char *extra_info;                      /* Extra bit slice info.       */
} Slice;

/* Macroblock structure. */

typedef struct macroblock {
  int mb_row, mb_col;			 /* Macroblock row and column.       */
  int mb_address;                        /* Macroblock address.              */
  int past_mb_addr;                      /* Previous mblock address.         */
  int motion_h_forw_code;                /* Forw. horiz. motion vector code. */
  unsigned int motion_h_forw_r;          /* Used in decoding vectors.        */
  int motion_v_forw_code;                /* Forw. vert. motion vector code.  */
  unsigned int motion_v_forw_r;          /* Used in decdoinge vectors.       */
  int motion_h_back_code;                /* Back horiz. motion vector code.  */
  unsigned int motion_h_back_r;          /* Used in decoding vectors.        */
  int motion_v_back_code;                /* Back vert. motion vector code.   */
  unsigned int motion_v_back_r;          /* Used in decoding vectors.        */
  unsigned int cbp;                      /* Coded block pattern.             */
  BOOLEAN mb_intra;                      /* Intracoded mblock flag.          */
  BOOLEAN bpict_past_forw;               /* Past B frame forw. vector flag.  */
  BOOLEAN bpict_past_back;               /* Past B frame back vector flag.   */
  int past_intra_addr;                   /* Addr of last intracoded mblock.  */
  int recon_right_for_prev;              /* Past right forw. vector.         */
  int recon_down_for_prev;               /* Past down forw. vector.          */
  int recon_right_back_prev;             /* Past right back vector.          */
  int recon_down_back_prev;              /* Past down back vector.           */
} Macroblock;

/* Misc DCT definitions */
#define DCTSIZE		8	/* The basic DCT block is 8x8 samples */

typedef int	DCTELEM;
typedef DCTELEM	DCTBLOCK[DCTSIZE * DCTSIZE];
 
/* Block structure. */

typedef struct block
{
    DCTBLOCK	dct_recon;		       /* Reconstructed dct coeff matrix. */
    DCTELEM	dct_dc_y_past;                 /* Past lum. dc dct coefficient.   */
    DCTELEM	dct_dc_cr_past;                /* Past cr dc dct coefficient.     */
    DCTELEM	dct_dc_cb_past;                /* Past cb dc dct coefficient.     */
} Block;

/* Video stream structure. */

typedef struct vid_stream {
  unsigned int h_size;                         /* Horiz. size in pixels.     */
  unsigned int v_size;                         /* Vert. size in pixels.      */
  unsigned int mb_height;                      /* Vert. size in mblocks.     */
  unsigned int mb_width;                       /* Horiz. size in mblocks.    */
  unsigned char aspect_ratio;                  /* Code for aspect ratio.     */
  unsigned char picture_rate;                  /* Code for picture rate.     */
  unsigned int bit_rate;                       /* Bit rate.                  */
  unsigned int vbv_buffer_size;                /* Minimum buffer size.       */
  BOOLEAN const_param_flag;                    /* Contrained parameter flag. */
  unsigned char intra_quant_matrix[8][8];      /* Quantization matrix for
						  intracoded frames.         */
  unsigned char non_intra_quant_matrix[8][8];  /* Quanitization matrix for 
						  non intracoded frames.     */
  char *ext_data;                              /* Extension data.            */
  char *user_data;                             /* User data.                 */
  GoP group;                                   /* Current group of pict.     */
  Pict picture;                                /* Current picture.           */
  Slice slice;                                 /* Current slice.             */
  Macroblock mblock;                           /* Current macroblock.        */
  Block block;                                 /* Current block.             */
  int state;                                   /* State of decoding.         */
  int bit_offset;                              /* Bit offset in stream.      */
  unsigned int *buffer;                        /* Pointer to next byte in
						  buffer.                    */
  int buf_length;                              /* Length of remaining buffer.*/
  unsigned int *buf_start;                     /* Pointer to buffer start.   */
  int max_buf_length;                          /* Max length of buffer.      */
  PictImage *past;                             /* Past predictive frame.     */
  PictImage *future;                           /* Future predictive frame.   */
  PictImage *current;                          /* Current frame.             */
  PictImage *ring[RING_BUF_SIZE];              /* Ring buffer of frames.     */
} VidStream;   

/* Declarataion of global display pointer. */
extern Display *display;

/* Declaration of global pointer to current video stream. */

extern VidStream *curVidStream;

/* Shared memory flag. */
extern int shmemFlag;

/* Quiet mode flag. */
extern int quietFlag;

/* Dither flags external declaration. */
extern char *ditherFlags;

#define GLOBAL			/* a function referenced thru EXTERNs */
  
/*
 * We use a lookup table to make sure values stay in the 0..255 range.
 * Since this is cropping (ie, x = (x < 0)?0:(x"255)?255:x; ), wee call this
 * table the "crop table".
 * MAX_NEG_CROP is the maximum neg/pos value we can handle.
 */

#define MAX_NEG_CROP 384
#define NUM_CROP_ENTRIES (256+2*MAX_NEG_CROP)
#ifdef SH_MEM
extern int gXErrorFlag;
#endif

extern int ditherType;

extern int totNumFrames;
extern int loopFlag;
extern int noDisplayFlag;
extern jmp_buf env;

#ifdef ANALYSIS
extern unsigned int bitCount;
extern int showEachFlag;
extern unsigned int cacheHit[8][8];
extern unsigned int cacheMiss[8][8];
#endif

