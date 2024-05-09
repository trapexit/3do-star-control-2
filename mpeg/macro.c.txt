#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "decoders.h"
#include "video.h"
#include "util.h"
#include "proto.h"

#define STATIC

unsigned char cropTbl[NUM_CROP_ENTRIES];

STATIC void
ReconIMBlock(VidStream *vid_stream, int bnum)
{
  int row, col, row_size;
  unsigned char *dest;

  /* If block is luminance block... */

  if (bnum < 4) {

    /* Calculate row and col values for upper left pixel of block. */

    row = vid_stream->mblock.mb_row;
    col = vid_stream->mblock.mb_col;
    if (bnum > 1)
      row += 8;
    if (bnum & 1)
      col += 8;

    /* Set dest to luminance plane of current pict image. */

    dest = vid_stream->current->luminance;

    /* Establish row size. */

    row_size = vid_stream->mb_width;
  }
  /* Otherwise if block is Cr block... */

  else
  {
    if (bnum == 4)
      /* Set dest to Cr plane of current pict image. */
      dest = vid_stream->current->Cr;
    else
      /* Set dest to Cb plane of current pict image. */
      dest = vid_stream->current->Cb;

    /* Establish row size. */

    row_size = vid_stream->mb_width >> 1;

    /* Calculate row,col for upper left pixel of block. */

    row = vid_stream->mblock.mb_row >> 1;
    col = vid_stream->mblock.mb_col >> 1;
  }

  /*
   * For each pixel in block, set to cropped reconstructed value from inverse
   * dct.
   */
  {
    unsigned char *cm = cropTbl + MAX_NEG_CROP;
    DCTELEM *blockvals = &vid_stream->block.dct_recon[0];

    dest += row * row_size + col;
    for (row = 0; row < 4; row++) {
#define assertCrop(x)	assert(((x) >= -MAX_NEG_CROP) && \
			       ((x) <= 256+MAX_NEG_CROP))
      dest[0] = cm[blockvals[0]];
      assertCrop(blockvals[0]);
      dest[1] = cm[blockvals[1]];
      assertCrop(blockvals[1]);
      dest[2] = cm[blockvals[2]];
      assertCrop(blockvals[2]);
      dest[3] = cm[blockvals[3]];
      assertCrop(blockvals[3]);
      dest[4] = cm[blockvals[4]];
      assertCrop(blockvals[4]);
      dest[5] = cm[blockvals[5]];
      assertCrop(blockvals[5]);
      dest[6] = cm[blockvals[6]];
      assertCrop(blockvals[6]);
      dest[7] = cm[blockvals[7]];
      assertCrop(blockvals[7]);
      dest += row_size;

      dest[0] = cm[blockvals[8]];
      assertCrop(blockvals[8]);
      dest[1] = cm[blockvals[9]];
      assertCrop(blockvals[9]);
      dest[2] = cm[blockvals[10]];
      assertCrop(blockvals[10]);
      dest[3] = cm[blockvals[11]];
      assertCrop(blockvals[11]);
      dest[4] = cm[blockvals[12]];
      assertCrop(blockvals[12]);
      dest[5] = cm[blockvals[13]];
      assertCrop(blockvals[13]);
      dest[6] = cm[blockvals[14]];
      assertCrop(blockvals[14]);
      dest[7] = cm[blockvals[15]];
      assertCrop(blockvals[15]);
      dest += row_size;

      blockvals += 16;
    }
  }
}

/*
 *--------------------------------------------------------------
 *
 * ReconPMBlock --
 *
 *	Reconstructs forward predicted macroblocks.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *--------------------------------------------------------------
 */

STATIC void
ReconPMBlock(VidStream *vid_stream, int bnum,
	int recon_right_for, int recon_down_for, int zflag)
{
  int row, col, row_size;
  unsigned char *dest, *past;
  int right_for, right_half_for, down_half_for;

  if (bnum < 4) {

    /* Set dest to luminance plane of current pict image. */
    dest = vid_stream->current->luminance;

    if (vid_stream->picture.code_type == B_TYPE) {
      past = vid_stream->past->luminance;
    } else {
      /* Set predicitive frame to current future frame. */
      past = vid_stream->future->luminance;
    }

    /* Establish row size. */

    row_size = vid_stream->mb_width;

    /* Calculate row,col of upper left pixel in block. */

    row = vid_stream->mblock.mb_row;
    col = vid_stream->mblock.mb_col;
    if (bnum > 1)
      row += 8;
    if (bnum & 1)
      col += 8;
  }
  /* Otherwise, block is NOT luminance block, ... */

  else {

    /* Construct motion vectors. */

    recon_right_for >>= 1;
    recon_down_for >>= 1;

    /* Establish row size. */

    row_size = vid_stream->mb_width >> 1;

    /* Calculate row,col of upper left pixel in block. */

    row = vid_stream->mblock.mb_row >> 1;
    col = vid_stream->mblock.mb_col >> 1;

    /* If block is Cr block... */

    if (bnum == 4) {

      /* Set dest to Cr plane of current pict image. */

      dest = vid_stream->current->Cr;

      if (vid_stream->picture.code_type == B_TYPE) {
	past = vid_stream->past->Cr;
      } else {
	past = vid_stream->future->Cr;
      }
    }
    /* Otherwise, block is Cb block... */

    else {

      /* Set dest to Cb plane of current pict image. */
      dest = vid_stream->current->Cb;

      if (vid_stream->picture.code_type == B_TYPE) {
	past = vid_stream->past->Cb;
      } else {
	past = vid_stream->future->Cb;
      }
    }
  }

  /* For each pixel in block... */
    dest += (row * row_size) + col;

    right_for = recon_right_for >> 1;
    right_half_for = recon_right_for & 0x1;
    down_half_for = recon_down_for & 0x1;
    past += (row + (recon_down_for >> 1)) * row_size + col + right_for;
    
    /*
     * Calculate predictive pixel value based on motion vectors and copy to
     * dest plane.
     */
    
    if (!(down_half_for | right_half_for)) {
      if (!zflag)
      {
        unsigned char *cm = cropTbl + MAX_NEG_CROP;
        DCTELEM *blockvals = &vid_stream->block.dct_recon[0];

	for (row = 0; row < 4; row++) {
	  dest[0] = cm[(int) past[0] + (int) blockvals[0]];
	  dest[1] = cm[(int) past[1] + (int) blockvals[1]];
	  dest[2] = cm[(int) past[2] + (int) blockvals[2]];
	  dest[3] = cm[(int) past[3] + (int) blockvals[3]];
	  dest[4] = cm[(int) past[4] + (int) blockvals[4]];
	  dest[5] = cm[(int) past[5] + (int) blockvals[5]];
	  dest[6] = cm[(int) past[6] + (int) blockvals[6]];
	  dest[7] = cm[(int) past[7] + (int) blockvals[7]];
	  dest += row_size;
	  past += row_size;
	  
	  dest[0] = cm[(int) past[0] + (int) blockvals[8]];
	  dest[1] = cm[(int) past[1] + (int) blockvals[9]];
	  dest[2] = cm[(int) past[2] + (int) blockvals[10]];
	  dest[3] = cm[(int) past[3] + (int) blockvals[11]];
	  dest[4] = cm[(int) past[4] + (int) blockvals[12]];
	  dest[5] = cm[(int) past[5] + (int) blockvals[13]];
	  dest[6] = cm[(int) past[6] + (int) blockvals[14]];
	  dest[7] = cm[(int) past[7] + (int) blockvals[15]];
	  dest += row_size;
	  past += row_size;

	  blockvals += 16;
	}
      } else {
	if (right_for & (0x1 | 0x2))
	{
	  /* No alignment, use byte copy */
	  for (row = 0; row < 4; row++) {
	    memcpy (dest, past, 8);
//	    dest[0] = past[0];
//	    dest[1] = past[1];
//	    dest[2] = past[2];
//	    dest[3] = past[3];
//	    dest[4] = past[4];
//	    dest[5] = past[5];
//	    dest[6] = past[6];
//	    dest[7] = past[7];
	    dest += row_size;
	    past += row_size;
	    
	    memcpy (dest, past, 8);
//	    dest[0] = past[0];
//	    dest[1] = past[1];
//	    dest[2] = past[2];
//	    dest[3] = past[3];
//	    dest[4] = past[4];
//	    dest[5] = past[5];
//	    dest[6] = past[6];
//	    dest[7] = past[7];
	    dest += row_size;
	    past += row_size;
	  }
	}
//	else if (right_for & 0x2)
//	{
//	  /* Half-word bit aligned, use 16 bit copy */
//	  for (row = 0; row < 4; row++) {
//	    ((short *)dest)[0] = ((short *)past)[0];
//	    ((short *)dest)[1] = ((short *)past)[1];
//	    ((short *)dest)[2] = ((short *)past)[2];
//	    ((short *)dest)[3] = ((short *)past)[3];
//	    dest += row_size;
//	    past += row_size;
//	    
//	    ((short *)dest)[0] = ((short *)past)[0];
//	    ((short *)dest)[1] = ((short *)past)[1];
//	    ((short *)dest)[2] = ((short *)past)[2];
//	    ((short *)dest)[3] = ((short *)past)[3];
//	    dest += row_size;
//	    past += row_size;
//	  }
//	}
	else
	{
	  for (row = 0; row < 4; row++) {
	    ((int *)dest)[0] = ((int *)past)[0];
	    ((int *)dest)[1] = ((int *)past)[1];
	    dest += row_size;
	    past += row_size;
	    
	    ((int *)dest)[0] = ((int *)past)[0];
	    ((int *)dest)[1] = ((int *)past)[1];
	    dest += row_size;
	    past += row_size;
	  }
	}
      }
    } else {
      unsigned char *rindex2;

      rindex2 = past + (down_half_for * row_size) + right_half_for;
      if (!zflag) {
        unsigned char *cm = cropTbl + MAX_NEG_CROP;
        DCTELEM *blockvals = &vid_stream->block.dct_recon[0];

	for (row = 0; row < 4; row++) {
	  dest[0] = cm[((int) (past[0] + rindex2[0]) >> 1) + blockvals[0]];
	  dest[1] = cm[((int) (past[1] + rindex2[1]) >> 1) + blockvals[1]];
	  dest[2] = cm[((int) (past[2] + rindex2[2]) >> 1) + blockvals[2]];
	  dest[3] = cm[((int) (past[3] + rindex2[3]) >> 1) + blockvals[3]];
	  dest[4] = cm[((int) (past[4] + rindex2[4]) >> 1) + blockvals[4]];
	  dest[5] = cm[((int) (past[5] + rindex2[5]) >> 1) + blockvals[5]];
	  dest[6] = cm[((int) (past[6] + rindex2[6]) >> 1) + blockvals[6]];
	  dest[7] = cm[((int) (past[7] + rindex2[7]) >> 1) + blockvals[7]];
	  dest += row_size;
	  past += row_size;
	  rindex2 += row_size;
	  
	  dest[0] = cm[((int) (past[0] + rindex2[0]) >> 1) + blockvals[8]];
	  dest[1] = cm[((int) (past[1] + rindex2[1]) >> 1) + blockvals[9]];
	  dest[2] = cm[((int) (past[2] + rindex2[2]) >> 1) + blockvals[10]];
	  dest[3] = cm[((int) (past[3] + rindex2[3]) >> 1) + blockvals[11]];
	  dest[4] = cm[((int) (past[4] + rindex2[4]) >> 1) + blockvals[12]];
	  dest[5] = cm[((int) (past[5] + rindex2[5]) >> 1) + blockvals[13]];
	  dest[6] = cm[((int) (past[6] + rindex2[6]) >> 1) + blockvals[14]];
	  dest[7] = cm[((int) (past[7] + rindex2[7]) >> 1) + blockvals[15]];
	  dest += row_size;
	  past += row_size;
	  rindex2 += row_size;

	  blockvals += 16;
	}
      } else
	for (row = 0; row < 4; row++) {
	  dest[0] = (int) (past[0] + rindex2[0]) >> 1;
	  dest[1] = (int) (past[1] + rindex2[1]) >> 1;
	  dest[2] = (int) (past[2] + rindex2[2]) >> 1;
	  dest[3] = (int) (past[3] + rindex2[3]) >> 1;
	  dest[4] = (int) (past[4] + rindex2[4]) >> 1;
	  dest[5] = (int) (past[5] + rindex2[5]) >> 1;
	  dest[6] = (int) (past[6] + rindex2[6]) >> 1;
	  dest[7] = (int) (past[7] + rindex2[7]) >> 1;
	  dest += row_size;
	  past += row_size;
	  rindex2 += row_size;
	  
	  dest[0] = (int) (past[0] + rindex2[0]) >> 1;
	  dest[1] = (int) (past[1] + rindex2[1]) >> 1;
	  dest[2] = (int) (past[2] + rindex2[2]) >> 1;
	  dest[3] = (int) (past[3] + rindex2[3]) >> 1;
	  dest[4] = (int) (past[4] + rindex2[4]) >> 1;
	  dest[5] = (int) (past[5] + rindex2[5]) >> 1;
	  dest[6] = (int) (past[6] + rindex2[6]) >> 1;
	  dest[7] = (int) (past[7] + rindex2[7]) >> 1;
	  dest += row_size;
	  past += row_size;
	  rindex2 += row_size;
	}
    }
}


/*
 *--------------------------------------------------------------
 *
 * ReconBMBlock --
 *
 *	Reconstructs back predicted macroblocks.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *--------------------------------------------------------------
 */

STATIC void
ReconBMBlock(VidStream *vid_stream, int bnum,
	int recon_right_back, int recon_down_back, int zflag)
{
  int row, col, row_size;
  unsigned char *dest, *future;
  int right_back, right_half_back, down_half_back;

  /* If block is luminance block... */

  if (bnum < 4) {

    /* Set dest to luminance plane of current pict image. */
    dest = vid_stream->current->luminance;

    /* Set future to luminance plane of future frame. */
    future = vid_stream->future->luminance;

    /* Establish row size. */

    row_size = vid_stream->mb_width;

    /* Calculate row,col of upper left pixel in block. */

    row = vid_stream->mblock.mb_row;
    col = vid_stream->mblock.mb_col;
    if (bnum > 1)
      row += 8;
    if (bnum & 1)
      col += 8;
  }
  /* Otherwise, block is NOT luminance block, ... */

  else {

    /* Construct motion vectors. */

    recon_right_back >>= 1;
    recon_down_back >>= 1;

    /* Establish row size. */

    row_size = vid_stream->mb_width >> 1;

    /* Calculate row,col of upper left pixel in block. */

    row = vid_stream->mblock.mb_row >> 1;
    col = vid_stream->mblock.mb_col >> 1;

    /* If block is Cr block... */

    if (bnum == 4) {

      /* Set dest to Cr plane of current pict image. */
      dest = vid_stream->current->Cr;

      /* Set future to Cr plane of future image. */
      future = vid_stream->future->Cr;
    }
    /* Otherwise, block is Cb block... */

    else {

      /* Set dest to Cb plane of current pict image. */
      dest = vid_stream->current->Cb;

      /* Set future to Cb plane of future frame. */
      future = vid_stream->future->Cb;
    }
  }

  /* For each pixel in block do... */
    dest += (row * row_size) + col;

    right_back = recon_right_back >> 1;
    right_half_back = recon_right_back & 0x1;
    down_half_back = recon_down_back & 0x1;
    future += (row + (recon_down_back >> 1)) * row_size + col + right_back;

    if ((!right_half_back) && (!down_half_back)) {
      if (!zflag)
      {
        unsigned char *cm = cropTbl + MAX_NEG_CROP;
        DCTELEM *blockvals = &vid_stream->block.dct_recon[0];

	for (row = 0; row < 4; row++) {
	  dest[0] = cm[(int) future[0] + (int) blockvals[0]];
	  dest[1] = cm[(int) future[1] + (int) blockvals[1]];
	  dest[2] = cm[(int) future[2] + (int) blockvals[2]];
	  dest[3] = cm[(int) future[3] + (int) blockvals[3]];
	  dest[4] = cm[(int) future[4] + (int) blockvals[4]];
	  dest[5] = cm[(int) future[5] + (int) blockvals[5]];
	  dest[6] = cm[(int) future[6] + (int) blockvals[6]];
	  dest[7] = cm[(int) future[7] + (int) blockvals[7]];
	  dest += row_size;
	  future += row_size;
	  
	  dest[0] = cm[(int) future[0] + (int) blockvals[8]];
	  dest[1] = cm[(int) future[1] + (int) blockvals[9]];
	  dest[2] = cm[(int) future[2] + (int) blockvals[10]];
	  dest[3] = cm[(int) future[3] + (int) blockvals[11]];
	  dest[4] = cm[(int) future[4] + (int) blockvals[12]];
	  dest[5] = cm[(int) future[5] + (int) blockvals[13]];
	  dest[6] = cm[(int) future[6] + (int) blockvals[14]];
	  dest[7] = cm[(int) future[7] + (int) blockvals[15]];
	  dest += row_size;
	  future += row_size;

	  blockvals += 16;
	}
      } else {
	if (right_back & (0x1 | 0x2)) {
	  /* No alignment, use byte copy */
	  for (row = 0; row < 4; row++) {
	    memcpy (dest, future, 8);
//	    dest[0] = future[0];
//	    dest[1] = future[1];
//	    dest[2] = future[2];
//	    dest[3] = future[3];
//	    dest[4] = future[4];
//	    dest[5] = future[5];
//	    dest[6] = future[6];
//	    dest[7] = future[7];
	    dest += row_size;
	    future += row_size;
	    
	    memcpy (dest, future, 8);
//	    dest[0] = future[0];
//	    dest[1] = future[1];
//	    dest[2] = future[2];
//	    dest[3] = future[3];
//	    dest[4] = future[4];
//	    dest[5] = future[5];
//	    dest[6] = future[6];
//	    dest[7] = future[7];
	    dest += row_size;
	    future += row_size;
	  }
	}
//	else if (right_back & 0x2) {
//	  /* Half-word bit aligned, use 16 bit copy */
//	  for (row = 0; row < 4; row++) {
//	    ((short *)dest)[0] = ((short *)future)[0];
//	    ((short *)dest)[1] = ((short *)future)[1];
//	    ((short *)dest)[2] = ((short *)future)[2];
//	    ((short *)dest)[3] = ((short *)future)[3];
//	    dest += row_size;
//	    future += row_size;
//	    
//	    ((short *)dest)[0] = ((short *)future)[0];
//	    ((short *)dest)[1] = ((short *)future)[1];
//	    ((short *)dest)[2] = ((short *)future)[2];
//	    ((short *)dest)[3] = ((short *)future)[3];
//	    dest += row_size;
//	    future += row_size;
//	  }
//	}
	else {
	  /* Word aligned, use 32 bit copy */
	  for (row = 0; row < 4; row++) {
	    ((int *)dest)[0] = ((int *)future)[0];
	    ((int *)dest)[1] = ((int *)future)[1];
	    dest += row_size;
	    future += row_size;
	    
	    ((int *)dest)[0] = ((int *)future)[0];
	    ((int *)dest)[1] = ((int *)future)[1];
	    dest += row_size;
	    future += row_size;
	  }
	}
      }
    } else {
      unsigned char *rindex2;

      rindex2 = future + (down_half_back * row_size) + right_half_back;
      if (!zflag)
      {
        unsigned char *cm = cropTbl + MAX_NEG_CROP;
        DCTELEM *blockvals = &vid_stream->block.dct_recon[0];

	for (row = 0; row < 4; row++) {
	  dest[0] = cm[((int) (future[0] + rindex2[0]) >> 1) + blockvals[0]];
	  dest[1] = cm[((int) (future[1] + rindex2[1]) >> 1) + blockvals[1]];
	  dest[2] = cm[((int) (future[2] + rindex2[2]) >> 1) + blockvals[2]];
	  dest[3] = cm[((int) (future[3] + rindex2[3]) >> 1) + blockvals[3]];
	  dest[4] = cm[((int) (future[4] + rindex2[4]) >> 1) + blockvals[4]];
	  dest[5] = cm[((int) (future[5] + rindex2[5]) >> 1) + blockvals[5]];
	  dest[6] = cm[((int) (future[6] + rindex2[6]) >> 1) + blockvals[6]];
	  dest[7] = cm[((int) (future[7] + rindex2[7]) >> 1) + blockvals[7]];
	  dest += row_size;
	  future += row_size;
	  rindex2 += row_size;
	  
	  dest[0] = cm[((int) (future[0] + rindex2[0]) >> 1) + blockvals[8]];
	  dest[1] = cm[((int) (future[1] + rindex2[1]) >> 1) + blockvals[9]];
	  dest[2] = cm[((int) (future[2] + rindex2[2]) >> 1) + blockvals[10]];
	  dest[3] = cm[((int) (future[3] + rindex2[3]) >> 1) + blockvals[11]];
	  dest[4] = cm[((int) (future[4] + rindex2[4]) >> 1) + blockvals[12]];
	  dest[5] = cm[((int) (future[5] + rindex2[5]) >> 1) + blockvals[13]];
	  dest[6] = cm[((int) (future[6] + rindex2[6]) >> 1) + blockvals[14]];
	  dest[7] = cm[((int) (future[7] + rindex2[7]) >> 1) + blockvals[15]];
	  dest += row_size;
	  future += row_size;
	  rindex2 += row_size;

	  blockvals += 16;
	}
      } else
	for (row = 0; row < 4; row++) {
	  dest[0] = (int) (future[0] + rindex2[0]) >> 1;
	  dest[1] = (int) (future[1] + rindex2[1]) >> 1;
	  dest[2] = (int) (future[2] + rindex2[2]) >> 1;
	  dest[3] = (int) (future[3] + rindex2[3]) >> 1;
	  dest[4] = (int) (future[4] + rindex2[4]) >> 1;
	  dest[5] = (int) (future[5] + rindex2[5]) >> 1;
	  dest[6] = (int) (future[6] + rindex2[6]) >> 1;
	  dest[7] = (int) (future[7] + rindex2[7]) >> 1;
	  dest += row_size;
	  future += row_size;
	  rindex2 += row_size;
	  
	  dest[0] = (int) (future[0] + rindex2[0]) >> 1;
	  dest[1] = (int) (future[1] + rindex2[1]) >> 1;
	  dest[2] = (int) (future[2] + rindex2[2]) >> 1;
	  dest[3] = (int) (future[3] + rindex2[3]) >> 1;
	  dest[4] = (int) (future[4] + rindex2[4]) >> 1;
	  dest[5] = (int) (future[5] + rindex2[5]) >> 1;
	  dest[6] = (int) (future[6] + rindex2[6]) >> 1;
	  dest[7] = (int) (future[7] + rindex2[7]) >> 1;
	  dest += row_size;
	  future += row_size;
	  rindex2 += row_size;
	}
    }
}


STATIC void
ReconBiMBlock(VidStream *vid_stream, int bnum,
	int recon_right_for, int recon_down_for,
	int recon_right_back, int recon_down_back, int zflag)
{
  int row, col, row_size;
  unsigned char *dest, *past, *future;

  /* If block is luminance block... */

  if (bnum < 4) {

    /* Set dest to luminance plane of current pict image. */
    dest = vid_stream->current->luminance;

    /* Set past to luminance plane of past frame. */
    past = vid_stream->past->luminance;

    /* Set future to luminance plane of future frame. */
    future = vid_stream->future->luminance;

    /* Establish row size. */

    row_size = vid_stream->mb_width;

    /* Calculate row,col of upper left pixel in block. */

    row = vid_stream->mblock.mb_row;
    col = vid_stream->mblock.mb_col;
    if (bnum > 1)
      row += 8;
    if (bnum & 0x01)
      col += 8;
  }
  /* Otherwise, block is NOT luminance block, ... */

  else {

    /* Construct motion vectors. */

    recon_right_for >>= 1;
    recon_down_for >>= 1;
    recon_right_back >>= 1;
    recon_down_back >>= 1;

    /* Establish row size. */

    row_size = vid_stream->mb_width >> 1;

    /* Calculate row,col of upper left pixel in block. */

    row = vid_stream->mblock.mb_row >> 1;
    col = vid_stream->mblock.mb_col >> 1;

    /* If block is Cr block... */

    if (bnum == 4) {

      /* Set dest to Cr plane of current pict image. */
      dest = vid_stream->current->Cr;

      /* Set past to Cr plane of past image. */
      past = vid_stream->past->Cr;

      /* Set future to Cr plane of future image. */
      future = vid_stream->future->Cr;
    }
    /* Otherwise, block is Cb block... */

    else {

      /* Set dest to Cb plane of current pict image. */
      dest = vid_stream->current->Cb;

      /* Set past to Cb plane of past frame. */
      past = vid_stream->past->Cb;

      /* Set future to Cb plane of future frame. */
      future = vid_stream->future->Cb;
    }
  }

  /* For each pixel in block... */
  dest += (row * row_size) + col;
  past += (row + (recon_down_for >> 1)) * row_size
	  + (col + (recon_right_for >> 1));
  future += (row + (recon_down_back >> 1)) * row_size
	  + (col + (recon_right_back >> 1));

  if (!zflag) {
    unsigned char *cm = cropTbl + MAX_NEG_CROP;
    DCTELEM *blockvals = &vid_stream->block.dct_recon[0];

    for (row = 0; row < 4; row++) {
      dest[0] = cm[((int) (past[0] + future[0]) >> 1) + blockvals[0]];
      dest[1] = cm[((int) (past[1] + future[1]) >> 1) + blockvals[1]];
      dest[2] = cm[((int) (past[2] + future[2]) >> 1) + blockvals[2]];
      dest[3] = cm[((int) (past[3] + future[3]) >> 1) + blockvals[3]];
      dest[4] = cm[((int) (past[4] + future[4]) >> 1) + blockvals[4]];
      dest[5] = cm[((int) (past[5] + future[5]) >> 1) + blockvals[5]];
      dest[6] = cm[((int) (past[6] + future[6]) >> 1) + blockvals[6]];
      dest[7] = cm[((int) (past[7] + future[7]) >> 1) + blockvals[7]];
      dest += row_size;
      past += row_size;
      future += row_size;

      dest[0] = cm[((int) (past[0] + future[0]) >> 1) + blockvals[8]];
      dest[1] = cm[((int) (past[1] + future[1]) >> 1) + blockvals[9]];
      dest[2] = cm[((int) (past[2] + future[2]) >> 1) + blockvals[10]];
      dest[3] = cm[((int) (past[3] + future[3]) >> 1) + blockvals[11]];
      dest[4] = cm[((int) (past[4] + future[4]) >> 1) + blockvals[12]];
      dest[5] = cm[((int) (past[5] + future[5]) >> 1) + blockvals[13]];
      dest[6] = cm[((int) (past[6] + future[6]) >> 1) + blockvals[14]];
      dest[7] = cm[((int) (past[7] + future[7]) >> 1) + blockvals[15]];
      dest += row_size;
      past += row_size;
      future += row_size;

      blockvals += 16;
    }
  } else
    for (row = 0; row < 4; row++) {
      dest[0] = (int) (past[0] + future[0]) >> 1;
      dest[1] = (int) (past[1] + future[1]) >> 1;
      dest[2] = (int) (past[2] + future[2]) >> 1;
      dest[3] = (int) (past[3] + future[3]) >> 1;
      dest[4] = (int) (past[4] + future[4]) >> 1;
      dest[5] = (int) (past[5] + future[5]) >> 1;
      dest[6] = (int) (past[6] + future[6]) >> 1;
      dest[7] = (int) (past[7] + future[7]) >> 1;
      dest += row_size;
      past += row_size;
      future += row_size;

      dest[0] = (int) (past[0] + future[0]) >> 1;
      dest[1] = (int) (past[1] + future[1]) >> 1;
      dest[2] = (int) (past[2] + future[2]) >> 1;
      dest[3] = (int) (past[3] + future[3]) >> 1;
      dest[4] = (int) (past[4] + future[4]) >> 1;
      dest[5] = (int) (past[5] + future[5]) >> 1;
      dest[6] = (int) (past[6] + future[6]) >> 1;
      dest[7] = (int) (past[7] + future[7]) >> 1;
      dest += row_size;
      past += row_size;
      future += row_size;
    }
}

/*
 *--------------------------------------------------------------
 *
 * ProcessSkippedPFrameMBlocks --
 *
 *	Processes skipped macroblocks in P frames.
 *
 * Results:
 *	Calculates pixel values for luminance, Cr, and Cb planes
 *      in current pict image for skipped macroblocks.
 *
 * Side effects:
 *	Pixel values in pict image changed.
 *
 *--------------------------------------------------------------
 */

STATIC void
ProcessSkippedPFrameMBlocks(VidStream *vid_stream)
{
  int row_size, half_row, mb_row, mb_col, row, col, rr;
  int addr, row_incr, half_row_incr, crow, ccol;
  int *dest, *src, *dest1, *src1;

  /* Calculate row sizes for luminance and Cr/Cb macroblock areas. */

  row_size = vid_stream->mb_width;
  half_row = (row_size >> 1);
  row_incr = row_size >> 2;
  half_row_incr = half_row >> 2;

  /* For each skipped macroblock, do... */

  for (addr = vid_stream->mblock.past_mb_addr + 1;
       addr < vid_stream->mblock.mb_address; addr++) {

    /* Calculate macroblock row and col. */

    mb_row = addr / (vid_stream->mb_width >> 4);
    mb_col = addr % (vid_stream->mb_width >> 4);

    /* Calculate upper left pixel row,col for luminance plane. */

    row = mb_row << 4;
    col = mb_col << 4;


    /* For each row in macroblock luminance plane... */

    dest = (int *)(vid_stream->current->luminance + (row * row_size) + col);
    src = (int *)(vid_stream->future->luminance + (row * row_size) + col);

    for (rr = 0; rr < 8; rr++) {

      /* Copy pixel values from last I or P picture. */

      dest[0] = src[0];
      dest[1] = src[1];
      dest[2] = src[2];
      dest[3] = src[3];
      dest += row_incr;
      src += row_incr;

      dest[0] = src[0];
      dest[1] = src[1];
      dest[2] = src[2];
      dest[3] = src[3];
      dest += row_incr;
      src += row_incr;
    }

    /*
     * Divide row,col to get upper left pixel of macroblock in Cr and Cb
     * planes.
     */

    crow = row >> 1;
    ccol = col >> 1;

    /* For each row in Cr, and Cb planes... */

    dest = (int *)(vid_stream->current->Cr + (crow * half_row) + ccol);
    src = (int *)(vid_stream->future->Cr + (crow * half_row) + ccol);
    dest1 = (int *)(vid_stream->current->Cb + (crow * half_row) + ccol);
    src1 = (int *)(vid_stream->future->Cb + (crow * half_row) + ccol);

    for (rr = 0; rr < 4; rr++) {

      /* Copy pixel values from last I or P picture. */

      dest[0] = src[0];
      dest[1] = src[1];

      dest1[0] = src1[0];
      dest1[1] = src1[1];

      dest += half_row_incr;
      src += half_row_incr;
      dest1 += half_row_incr;
      src1 += half_row_incr;

      dest[0] = src[0];
      dest[1] = src[1];

      dest1[0] = src1[0];
      dest1[1] = src1[1];

      dest += half_row_incr;
      src += half_row_incr;
      dest1 += half_row_incr;
      src1 += half_row_incr;
    }

  }

  vid_stream->mblock.recon_right_for_prev = 0;
  vid_stream->mblock.recon_down_for_prev = 0;
}



/*
 *--------------------------------------------------------------
 *
 * ReconSkippedBlock --
 *
 *	Reconstructs predictive block for skipped macroblocks
 *      in B Frames.
 *
 * Results:
 *	No return values.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

STATIC void
ReconSkippedBlock(unsigned char *source, unsigned char *dest,
	int row, int col, int row_size,
	int right, int down, int right_half, int down_half, int width)
{
  int rr;
  unsigned char *source2;

  source += ((row + down) * row_size) + col + right;

  if (width == 16) {
    if ((!right_half) && (!down_half)) {
	if (right & (0x1 | 0x2)) {
	  /* No alignment, use bye copy */
	  for (rr = 0; rr < 16; rr++) {
	    memcpy (dest, source, 16);
//	    dest[0] = source[0];
//	    dest[1] = source[1];
//	    dest[2] = source[2];
//	    dest[3] = source[3];
//	    dest[4] = source[4];
//	    dest[5] = source[5];
//	    dest[6] = source[6];
//	    dest[7] = source[7];
//	    dest[8] = source[8];
//	    dest[9] = source[9];
//	    dest[10] = source[10];
//	    dest[11] = source[11];
//	    dest[12] = source[12];
//	    dest[13] = source[13];
//	    dest[14] = source[14];
//	    dest[15] = source[15];
	    dest += 16;
	    source += row_size;
	  }
	}
//	else if (right & 0x2)
//	{
//	  /* Half-word bit aligned, use 16 bit copy */
//	  for (rr = 0; rr < 16; rr++) {
//	    ((short *)dest)[0] = ((short *)source)[0];
//	    ((short *)dest)[1] = ((short *)source)[1];
//	    ((short *)dest)[2] = ((short *)source)[2];
//	    ((short *)dest)[3] = ((short *)source)[3];
//	    ((short *)dest)[4] = ((short *)source)[4];
//	    ((short *)dest)[5] = ((short *)source)[5];
//	    ((short *)dest)[6] = ((short *)source)[6];
//	    ((short *)dest)[7] = ((short *)source)[7];
//	    ((short *)dest)[8] = ((short *)source)[8];
//	    dest += 16;
//	    source += row_size;
//	  }
//	}
	else
	{
	  /* Word aligned, use 32 bit copy */
	  for (rr = 0; rr < 16; rr++) {
	    ((int *)dest)[0] = ((int *)source)[0];
	    ((int *)dest)[1] = ((int *)source)[1];
	    ((int *)dest)[2] = ((int *)source)[2];
	    ((int *)dest)[3] = ((int *)source)[3];
	    dest += 16;
	    source += row_size;
	  }
	}
    } else {
      source2 = source + right_half + (row_size * down_half);
      for (rr = 0; rr < width; rr++) {
	dest[0] = (int) (source[0] + source2[0]) >> 1;
	dest[1] = (int) (source[1] + source2[1]) >> 1;
	dest[2] = (int) (source[2] + source2[2]) >> 1;
	dest[3] = (int) (source[3] + source2[3]) >> 1;
	dest[4] = (int) (source[4] + source2[4]) >> 1;
	dest[5] = (int) (source[5] + source2[5]) >> 1;
	dest[6] = (int) (source[6] + source2[6]) >> 1;
	dest[7] = (int) (source[7] + source2[7]) >> 1;
	dest[8] = (int) (source[8] + source2[8]) >> 1;
	dest[9] = (int) (source[9] + source2[9]) >> 1;
	dest[10] = (int) (source[10] + source2[10]) >> 1;
	dest[11] = (int) (source[11] + source2[11]) >> 1;
	dest[12] = (int) (source[12] + source2[12]) >> 1;
	dest[13] = (int) (source[13] + source2[13]) >> 1;
	dest[14] = (int) (source[14] + source2[14]) >> 1;
	dest[15] = (int) (source[15] + source2[15]) >> 1;
	dest += width;
	source += row_size;
	source2 += row_size;
      }
    }
  } else {			/* (width == 8) */
    assert(width == 8);
    if ((!right_half) && (!down_half)) {
      if (right & (0x1 | 0x2)) {
	for (rr = 0; rr < width; rr++) {
	  memcpy (dest, source, 8);
//	  dest[0] = source[0];
//	  dest[1] = source[1];
//	  dest[2] = source[2];
//	  dest[3] = source[3];
//	  dest[4] = source[4];
//	  dest[5] = source[5];
//	  dest[6] = source[6];
//	  dest[7] = source[7];
	  dest += 8;
	  source += row_size;
	}
      }
//      else if (right & 0x02) {
//	for (rr = 0; rr < width; rr++) {
//	  ((short *)dest)[0] = ((short *)source)[0];
//	  ((short *)dest)[1] = ((short *)source)[1];
//	  ((short *)dest)[2] = ((short *)source)[2];
//	  ((short *)dest)[3] = ((short *)source)[3];
//	  dest += 8;
//	  source += row_size;
//	}
//      }
      else {
	for (rr = 0; rr < width; rr++) {
	  ((int *)dest)[0] = ((int *)source)[0];
	  ((int *)dest)[1] = ((int *)source)[1];
	  dest += 8;
	  source += row_size;
	}
      }
    } else {
      source2 = source + right_half + (row_size * down_half);
      for (rr = 0; rr < width; rr++) {
	dest[0] = (int) (source[0] + source2[0]) >> 1;
	dest[1] = (int) (source[1] + source2[1]) >> 1;
	dest[2] = (int) (source[2] + source2[2]) >> 1;
	dest[3] = (int) (source[3] + source2[3]) >> 1;
	dest[4] = (int) (source[4] + source2[4]) >> 1;
	dest[5] = (int) (source[5] + source2[5]) >> 1;
	dest[6] = (int) (source[6] + source2[6]) >> 1;
	dest[7] = (int) (source[7] + source2[7]) >> 1;
	dest += width;
	source += row_size;
	source2 += row_size;
      }
    }
  }
}


/*
 *--------------------------------------------------------------
 *
 * ProcessSkippedBFrameMBlocks --
 *
 *	Processes skipped macroblocks in B frames.
 *
 * Results:
 *	Calculates pixel values for luminance, Cr, and Cb planes
 *      in current pict image for skipped macroblocks.
 *
 * Side effects:
 *	Pixel values in pict image changed.
 *
 *--------------------------------------------------------------
 */

STATIC void
ProcessSkippedBFrameMBlocks(VidStream *vid_stream)
{
  int row_size, half_row, mb_row, mb_col, row, col, rr;
  int right_half_for, down_half_for, c_right_half_for, c_down_half_for;
  int right_half_back, down_half_back, c_right_half_back, c_down_half_back;
  int addr, right_for, down_for;
  int recon_right_for, recon_down_for;
  int recon_right_back, recon_down_back;
  int right_back, down_back;
  int c_right_for, c_down_for;
  int c_right_back, c_down_back;
  unsigned char forw_lum[256];
  unsigned char forw_cr[64], forw_cb[64];
  unsigned char back_lum[256], back_cr[64], back_cb[64];
  int row_incr, half_row_incr;
  int ccol, crow;

  /* Calculate row sizes for luminance and Cr/Cb macroblock areas. */

  row_size = vid_stream->mb_width;
  half_row = (row_size >> 1);
  row_incr = row_size >> 2;
  half_row_incr =  half_row >> 2;

  /* Establish motion vector codes based on full pixel flag. */

  if (vid_stream->picture.full_pel_forw_vector) {
    recon_right_for = vid_stream->mblock.recon_right_for_prev << 1;
    recon_down_for = vid_stream->mblock.recon_down_for_prev << 1;
  } else {
    recon_right_for = vid_stream->mblock.recon_right_for_prev;
    recon_down_for = vid_stream->mblock.recon_down_for_prev;
  }

  if (vid_stream->picture.full_pel_back_vector) {
    recon_right_back = vid_stream->mblock.recon_right_back_prev << 1;
    recon_down_back = vid_stream->mblock.recon_down_back_prev << 1;
  } else {
    recon_right_back = vid_stream->mblock.recon_right_back_prev;
    recon_down_back = vid_stream->mblock.recon_down_back_prev;
  }


  /* Calculate motion vectors. */
  
  if (vid_stream->mblock.bpict_past_forw) {
    right_for = recon_right_for >> 1;
    down_for = recon_down_for >> 1;
    right_half_for = recon_right_for & 0x1;
    down_half_for = recon_down_for & 0x1;
    
    recon_right_for >>= 1;
    recon_down_for >>= 1;
    c_right_for = recon_right_for >> 1;
    c_down_for = recon_down_for >> 1;
    c_right_half_for = recon_right_for & 0x1;
    c_down_half_for = recon_down_for & 0x1;
    
  }
  if (vid_stream->mblock.bpict_past_back) {
    right_back = recon_right_back >> 1;
    down_back = recon_down_back >> 1;
    right_half_back = recon_right_back & 0x1;
    down_half_back = recon_down_back & 0x1;
    
    recon_right_back >>= 1;
    recon_down_back >>= 1;
    c_right_back = recon_right_back >> 1;
    c_down_back = recon_down_back >> 1;
    c_right_half_back = recon_right_back & 0x1;
    c_down_half_back = recon_down_back & 0x1;
    
  }
  /* For each skipped macroblock, do... */
  
  for (addr = vid_stream->mblock.past_mb_addr + 1;
       addr < vid_stream->mblock.mb_address; addr++) {
    
    /* Calculate macroblock row and col. */
    
    mb_row = addr / (vid_stream->mb_width >> 4);
    mb_col = addr % (vid_stream->mb_width >> 4);
    
    /* Calculate upper left pixel row,col for luminance plane. */
    
    row = mb_row << 4;
    col = mb_col << 4;
    crow = row / 2;
    ccol = col / 2;
    
    /* If forward predicted, calculate prediction values. */
    
    if (vid_stream->mblock.bpict_past_forw) {
      
      ReconSkippedBlock(vid_stream->past->luminance, forw_lum,
			row, col, row_size, right_for, down_for,
			right_half_for, down_half_for, 16);
      ReconSkippedBlock(vid_stream->past->Cr, forw_cr, crow,
			ccol, half_row,
			c_right_for, c_down_for, c_right_half_for, c_down_half_for, 8);
      ReconSkippedBlock(vid_stream->past->Cb, forw_cb, crow,
			ccol, half_row,
			c_right_for, c_down_for, c_right_half_for, c_down_half_for, 8);
    }
    /* If back predicted, calculate prediction values. */
    
    if (vid_stream->mblock.bpict_past_back) {
      ReconSkippedBlock(vid_stream->future->luminance, back_lum,
			row, col, row_size, right_back, down_back,
			right_half_back, down_half_back, 16);
      ReconSkippedBlock(vid_stream->future->Cr, back_cr, crow,
			ccol, half_row,
			c_right_back, c_down_back,
			c_right_half_back, c_down_half_back, 8);
      ReconSkippedBlock(vid_stream->future->Cb, back_cb, crow,
			ccol, half_row,
			c_right_back, c_down_back,
			c_right_half_back, c_down_half_back, 8);
    }
    if (vid_stream->mblock.bpict_past_forw &&
	!vid_stream->mblock.bpict_past_back) {
      
      int *dest, *dest1;
      int *src, *src1;
      dest = (int *)(vid_stream->current->luminance + (row * row_size) + col);
      src = (int *)forw_lum;
      
      for (rr = 0; rr < 16; rr++) {
	
	/* memcpy(dest, forw_lum+(rr<<4), 16);  */
	dest[0] = src[0];
	dest[1] = src[1];
	dest[2] = src[2];
	dest[3] = src[3];
	dest += row_incr;
	src += 4;
      }
      
      dest = (int *)(vid_stream->current->Cr + (crow * half_row) + ccol);
      dest1 = (int *)(vid_stream->current->Cb + (crow * half_row) + ccol);
      src = (int *)forw_cr;
      src1 = (int *)forw_cb;
      
      for (rr = 0; rr < 8; rr++) {
	/*
	 * memcpy(dest, forw_cr+(rr<<3), 8); memcpy(dest1, forw_cb+(rr<<3),
	 * 8);
	 */
	
	dest[0] = src[0];
	dest[1] = src[1];
	
	dest1[0] = src1[0];
	dest1[1] = src1[1];
	
	dest += half_row_incr;
	dest1 += half_row_incr;
	src += 2;
	src1 += 2;
      }
    } else if (vid_stream->mblock.bpict_past_back &&
	       !vid_stream->mblock.bpict_past_forw) {
      
      int *src, *src1;
      int *dest, *dest1;
      dest = (int *)(vid_stream->current->luminance + (row * row_size) + col);
      src = (int *)back_lum;
      
      for (rr = 0; rr < 16; rr++) {
	dest[0] = src[0];
	dest[1] = src[1];
	dest[2] = src[2];
	dest[3] = src[3];
	dest += row_incr;
	src += 4;
      }
      
      
      dest = (int *)(vid_stream->current->Cr + (crow * half_row) + ccol);
      dest1 = (int *)(vid_stream->current->Cb + (crow * half_row) + ccol);
      src = (int *)back_cr;
      src1 = (int *)back_cb;
      
      for (rr = 0; rr < 8; rr++) {
	/*
	 * memcpy(dest, back_cr+(rr<<3), 8); memcpy(dest1, back_cb+(rr<<3),
	 * 8);
	 */
	
	dest[0] = src[0];
	dest[1] = src[1];
	
	dest1[0] = src1[0];
	dest1[1] = src1[1];
	
	dest += half_row_incr;
	dest1 += half_row_incr;
	src += 2;
	src1 += 2;
      }
    } else {
      
      unsigned char *src1, *src2, *src1a, *src2a;
      unsigned char *dest, *dest1;
      dest = vid_stream->current->luminance + (row * row_size) + col;
      src1 = forw_lum;
      src2 = back_lum;
      
      for (rr = 0; rr < 16; rr++) {
	dest[0] = (int) (src1[0] + src2[0]) >> 1;
	dest[1] = (int) (src1[1] + src2[1]) >> 1;
	dest[2] = (int) (src1[2] + src2[2]) >> 1;
	dest[3] = (int) (src1[3] + src2[3]) >> 1;
	dest[4] = (int) (src1[4] + src2[4]) >> 1;
	dest[5] = (int) (src1[5] + src2[5]) >> 1;
	dest[6] = (int) (src1[6] + src2[6]) >> 1;
	dest[7] = (int) (src1[7] + src2[7]) >> 1;
	dest[8] = (int) (src1[8] + src2[8]) >> 1;
	dest[9] = (int) (src1[9] + src2[9]) >> 1;
	dest[10] = (int) (src1[10] + src2[10]) >> 1;
	dest[11] = (int) (src1[11] + src2[11]) >> 1;
	dest[12] = (int) (src1[12] + src2[12]) >> 1;
	dest[13] = (int) (src1[13] + src2[13]) >> 1;
	dest[14] = (int) (src1[14] + src2[14]) >> 1;
	dest[15] = (int) (src1[15] + src2[15]) >> 1;
	dest += row_size;
	src1 += 16;
	src2 += 16;
      }
      
      
      dest = vid_stream->current->Cr + (crow * half_row) + ccol;
      dest1 = vid_stream->current->Cb + (crow * half_row) + ccol;
      src1 = forw_cr;
      src2 = back_cr;
      src1a = forw_cb;
      src2a = back_cb;
      
      for (rr = 0; rr < 8; rr++) {
	dest[0] = (int) (src1[0] + src2[0]) >> 1;
	dest[1] = (int) (src1[1] + src2[1]) >> 1;
	dest[2] = (int) (src1[2] + src2[2]) >> 1;
	dest[3] = (int) (src1[3] + src2[3]) >> 1;
	dest[4] = (int) (src1[4] + src2[4]) >> 1;
	dest[5] = (int) (src1[5] + src2[5]) >> 1;
	dest[6] = (int) (src1[6] + src2[6]) >> 1;
	dest[7] = (int) (src1[7] + src2[7]) >> 1;
	dest += half_row;
	src1 += 8;
	src2 += 8;
	
	dest1[0] = (int) (src1a[0] + src2a[0]) >> 1;
	dest1[1] = (int) (src1a[1] + src2a[1]) >> 1;
	dest1[2] = (int) (src1a[2] + src2a[2]) >> 1;
	dest1[3] = (int) (src1a[3] + src2a[3]) >> 1;
	dest1[4] = (int) (src1a[4] + src2a[4]) >> 1;
	dest1[5] = (int) (src1a[5] + src2a[5]) >> 1;
	dest1[6] = (int) (src1a[6] + src2a[6]) >> 1;
	dest1[7] = (int) (src1a[7] + src2a[7]) >> 1;
	dest1 += half_row;
	src1a += 8;
	src2a += 8;
      }
    }
    
    if (ditherType == MBORDERED_DITHER) {
      ditherFlags[addr] = 1;
    }
  }
}


/*
 *--------------------------------------------------------------
 *
 * ParseMacroBlock --
 *
 *      Parseoff macroblock. Reconstructs DCT values. Applies
 *      inverse DCT, reconstructs motion vectors, calculates and
 *      set pixel values for macroblock in current pict image
 *      structure.
 *
 * Results:
 *      Here's where everything really happens. Welcome to the
 *      heart of darkness.
 *
 * Side effects:
 *      Bit stream irreversibly parsed off.
 *
 *--------------------------------------------------------------
 */

int
ParseMacroBlock(vid_stream)
  VidStream *vid_stream;
{
  int addr_incr;
  unsigned int data;
  int recon_right_for, recon_down_for, recon_right_back,
      recon_down_back;
  BOOLEAN mb_quant, mb_motion_forw, mb_motion_back, mb_pattern;
  int no_dith_flag = 0;

#ifdef ANALYSIS
  mbSizeCount = bitCountRead();
#endif

  /*
   * Parse off macroblock address increment and add to macroblock address.
   */
  do {
    DecodeMBAddrInc(addr_incr);
    if (addr_incr == MB_ESCAPE) {
      vid_stream->mblock.mb_address += 33;
      addr_incr = MB_STUFFING;
    }
  } while (addr_incr == MB_STUFFING);
  vid_stream->mblock.mb_address += addr_incr;

  if (vid_stream->mblock.mb_address > ((vid_stream->mb_height *
				       vid_stream->mb_width)>>(4+4)) - 1)
    return SKIP_TO_START_CODE;

  /*
   * If macroblocks have been skipped, process skipped macroblocks.
   */

  if (vid_stream->mblock.mb_address - vid_stream->mblock.past_mb_addr > 1) {
    if (vid_stream->picture.code_type == P_TYPE)
      ProcessSkippedPFrameMBlocks(vid_stream);
    else if (vid_stream->picture.code_type == B_TYPE)
      ProcessSkippedBFrameMBlocks(vid_stream);
  }
  /* Set past macroblock address to current macroblock address. */
  vid_stream->mblock.past_mb_addr = vid_stream->mblock.mb_address;

  /* Based on picture type decode macroblock type. */
  switch (vid_stream->picture.code_type) {
  case I_TYPE:
    DecodeMBTypeI(mb_quant, mb_motion_forw, mb_motion_back, mb_pattern,
		  vid_stream->mblock.mb_intra);
    break;

  case P_TYPE:
    DecodeMBTypeP(mb_quant, mb_motion_forw, mb_motion_back, mb_pattern,
		  vid_stream->mblock.mb_intra);
    break;

  case B_TYPE:
    DecodeMBTypeB(mb_quant, mb_motion_forw, mb_motion_back, mb_pattern,
		  vid_stream->mblock.mb_intra);
    break;
  }

  /* If quantization flag set, parse off new quantization scale. */

  if (mb_quant == TRUE) {
    get_bits5(data);
    vid_stream->slice.quant_scale = data;
  }
  /* If forward motion vectors exist... */
  if (mb_motion_forw == TRUE) {

    /* Parse off and decode horizontal forward motion vector. */
    DecodeMotionVectors(vid_stream->mblock.motion_h_forw_code);

    /* If horiz. forward r data exists, parse off. */

    if ((vid_stream->picture.forw_f != 1) &&
	(vid_stream->mblock.motion_h_forw_code != 0)) {
      get_bitsn(vid_stream->picture.forw_r_size, data);
      vid_stream->mblock.motion_h_forw_r = data;
    }
    /* Parse off and decode vertical forward motion vector. */
    DecodeMotionVectors(vid_stream->mblock.motion_v_forw_code);

    /* If vert. forw. r data exists, parse off. */

    if ((vid_stream->picture.forw_f != 1) &&
	(vid_stream->mblock.motion_v_forw_code != 0)) {
      get_bitsn(vid_stream->picture.forw_r_size, data);
      vid_stream->mblock.motion_v_forw_r = data;
    }
  }
  /* If back motion vectors exist... */
  if (mb_motion_back == TRUE) {

    /* Parse off and decode horiz. back motion vector. */
    DecodeMotionVectors(vid_stream->mblock.motion_h_back_code);

    /* If horiz. back r data exists, parse off. */

    if ((vid_stream->picture.back_f != 1) &&
	(vid_stream->mblock.motion_h_back_code != 0)) {
      get_bitsn(vid_stream->picture.back_r_size, data);
      vid_stream->mblock.motion_h_back_r = data;
    }
    /* Parse off and decode vert. back motion vector. */
    DecodeMotionVectors(vid_stream->mblock.motion_v_back_code);

    /* If vert. back r data exists, parse off. */

    if ((vid_stream->picture.back_f != 1) &&
	(vid_stream->mblock.motion_v_back_code != 0)) {
      get_bitsn(vid_stream->picture.back_r_size, data);
      vid_stream->mblock.motion_v_back_r = data;
    }
  }
#ifdef ANALYSIS
  if (vid_stream->mblock.mb_intra) {
    stat_a[0].i_mbnum++;
    mbCBPPtr = stat_a[0].i_mbcbp;
    mbCoeffPtr = stat_a[0].i_mbcoeff;
    mbSizePtr = &(stat_a[0].i_mbsize);
  } else if (mb_motion_back && mb_motion_forw) {
    stat_a[0].bi_mbnum++;
    mbCBPPtr = stat_a[0].bi_mbcbp;
    mbCoeffPtr = stat_a[0].bi_mbcoeff;
    mbSizePtr = &(stat_a[0].bi_mbsize);
  } else if (mb_motion_back) {
    stat_a[0].b_mbnum++;
    mbCBPPtr = stat_a[0].b_mbcbp;
    mbCoeffPtr = stat_a[0].b_mbcoeff;
    mbSizePtr = &(stat_a[0].b_mbsize);
  } else {
    stat_a[0].p_mbnum++;
    mbCBPPtr = stat_a[0].p_mbcbp;
    mbCoeffPtr = stat_a[0].p_mbcoeff;
    mbSizePtr = &(stat_a[0].p_mbsize);
  }
#endif

  /* If mblock pattern flag set, parse and decode CBP (code block pattern). */
  if (mb_pattern == TRUE) {
    DecodeCBP(vid_stream->mblock.cbp);
  }
  /* Otherwise, set CBP to zero. */
  else
    vid_stream->mblock.cbp = 0;


#ifdef ANALYSIS
  mbCBPPtr[vid_stream->mblock.cbp]++;
#endif

  /* Reconstruct motion vectors depending on picture type. */
  if (vid_stream->picture.code_type == P_TYPE) {

    /*
     * If no forw motion vectors, reset previous and current vectors to 0.
     */

    if (!mb_motion_forw) {
      recon_right_for = 0;
      recon_down_for = 0;
      vid_stream->mblock.recon_right_for_prev = 0;
      vid_stream->mblock.recon_down_for_prev = 0;
    }
    /*
     * Otherwise, compute new forw motion vectors. Reset previous vectors to
     * current vectors.
     */

    else {
      ComputeForwVector(&recon_right_for, &recon_down_for);
    }
  }
  else if (vid_stream->picture.code_type == B_TYPE) {

    /* Reset prev. and current vectors to zero if mblock is intracoded. */

    if (vid_stream->mblock.mb_intra) {
      vid_stream->mblock.recon_right_for_prev = 0;
      vid_stream->mblock.recon_down_for_prev = 0;
      vid_stream->mblock.recon_right_back_prev = 0;
      vid_stream->mblock.recon_down_back_prev = 0;
    } else {

      /* If no forw vectors, current vectors equal prev. vectors. */

      if (!mb_motion_forw) {
	recon_right_for = vid_stream->mblock.recon_right_for_prev;
	recon_down_for = vid_stream->mblock.recon_down_for_prev;
      }
      /*
       * Otherwise compute forw. vectors. Reset prev vectors to new values.
       */

      else {
        ComputeForwVector(&recon_right_for, &recon_down_for);
      }

      /* If no back vectors, set back vectors to prev back vectors. */

      if (!mb_motion_back) {
	recon_right_back = vid_stream->mblock.recon_right_back_prev;
	recon_down_back = vid_stream->mblock.recon_down_back_prev;
      }
      /* Otherwise compute new vectors and reset prev. back vectors. */

      else {
	ComputeBackVector(&recon_right_back, &recon_down_back);
      }

      /*
       * Store vector existance flags in structure for possible skipped
       * macroblocks to follow.
       */

      vid_stream->mblock.bpict_past_forw = mb_motion_forw;
      vid_stream->mblock.bpict_past_back = mb_motion_back;
    }
  }

    {
	int		mask, i, skip;
	extern int	display_width, display_height;

	/* Calculate macroblock row and column from address. */
	vid_stream->mblock.mb_row = (vid_stream->mblock.mb_address
		/ (vid_stream->mb_width >> 4)) << 4;
	vid_stream->mblock.mb_col = (vid_stream->mblock.mb_address
		% (vid_stream->mb_width >> 4)) << 4;

	skip = (vid_stream->mblock.mb_col > display_width + 16
		|| vid_stream->mblock.mb_row > display_height + 16);

	/* For each possible block in macroblock. */
	for (mask = 1 << 5, i = 0; mask; mask >>= 1, i++)
	{
	    int	zero_block_flag;

	    /* If block exists... */
	    if ((vid_stream->mblock.mb_intra) || (vid_stream->mblock.cbp & mask))
	    {
		if (skip)
		{
		    ParseAwayBlock(i);
		    continue;
		}

		zero_block_flag = 0;
		ParseReconBlock(i);
	    }
	    else
	    {
		zero_block_flag = 1;
	    }

	    if (!skip)
	    {
	        if (i == 3)
		    skip = (ditherType == GRAY_DITHER
			    || ditherType == MONO_DITHER
			    || ditherType == MONO_THRESHOLD);

		/* If macroblock is intra coded... */
		if (vid_stream->mblock.mb_intra)
		    ReconIMBlock(vid_stream, i);
		else if (mb_motion_forw && mb_motion_back)
		    ReconBiMBlock(vid_stream, i, recon_right_for, recon_down_for,
			    recon_right_back, recon_down_back, zero_block_flag);
		else if (mb_motion_forw || (vid_stream->picture.code_type == P_TYPE))
		    ReconPMBlock(vid_stream, i, recon_right_for, recon_down_for,
			    zero_block_flag);
		else if (mb_motion_back)
		    ReconBMBlock(vid_stream, i, recon_right_back, recon_down_back,
			    zero_block_flag);
	    }
	}
    }

    /* If D Type picture, flush marker bit. */
    if (vid_stream->picture.code_type == D_TYPE)
    {
	flush_bits(1);
    }

    /* If macroblock was intracoded, set macroblock past intra address. */
    if (vid_stream->mblock.mb_intra)
	vid_stream->mblock.past_intra_addr = vid_stream->mblock.mb_address;

#ifdef ANALYSIS
  *mbSizePtr += bitCountRead() - mbSizeCount;
#endif
  return PARSE_OK;
}

