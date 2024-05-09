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
#define NO_SANITY_CHECKS
#include <assert.h>
#include "video.h"
#include "proto.h"
#include "decoders.h"

/* External declarations. */

extern int zigzag_direct[];

/* Macro for returning 1 if num is positive, -1 if negative, 0 if 0. */

#define Sign(num) ((num > 0) ? 1 : ((num == 0) ? 0 : -1))


/*
 *--------------------------------------------------------------
 *
 * ParseReconBlock --
 *
 *	Parse values for block structure from bitstream.
 *      n is an indication of the position of the block within
 *      the macroblock (i.e. 0-5) and indicates the type of 
 *      block (i.e. luminance or chrominance). Reconstructs
 *      coefficients from values parsed and puts in 
 *      block.dct_recon array in vid stream structure.
 *      sparseFlag is set when the block contains only one
 *      coeffictient and is used by the IDCT.
 *
 * Results:
 *	
 *
 * Side effects:
 *      Bit stream irreversibly parsed.
 *
 *--------------------------------------------------------------
 */

void
ParseReconBlock(n)
     int n;
{
#ifdef RISC
    unsigned int	temp_curBits;
    int			temp_bitOffset;
    int			temp_bufLength;
    unsigned int	*temp_bitBuffer;
#endif
    Block		*blockPtr = &curVidStream->block;
#define DCT_dc_y_past blockPtr->dct_dc_y_past
#define DCT_dc_cr_past blockPtr->dct_dc_cr_past
#define DCT_dc_cb_past blockPtr->dct_dc_cb_past

#define DECODE_DCT_COEFF_FIRST DecodeDCTCoeffFirst
#define DECODE_DCT_COEFF_NEXT DecodeDCTCoeffNext

    if (bufLength < 100)
	correct_underflow();

#ifdef RISC
    temp_curBits = curBits;
    temp_bitOffset = bitOffset;
    temp_bufLength = bufLength;
    temp_bitBuffer = bitBuffer;
#endif

    {
    /*
     * Copy the globals curBits, bitOffset, bufLength, and bitBuffer
     * into local variables with the same names, so the macros use the
     * local variables instead.  This allows register allocation and
     * can provide 1-2 fps speedup.  On machines with not so many registers,
     * don't do this.
     */
#ifdef RISC
	register unsigned int curBits = temp_curBits;
	register int bitOffset = temp_bitOffset;
	register int bufLength = temp_bufLength;
	register unsigned int *bitBuffer = temp_bitBuffer;
#endif
	int		i, level, run, pos, first_pos, qscale;
	DCTELEM		*reconptr;
	unsigned char	*qmatrixptr;

	reconptr = &blockPtr->dct_recon[0];
#if (MSDOS && __WATCOMC__)
	{
	    extern void _memsetdd (int *, int, int);
#pragma aux _memsetdd parm [edi] [eax] [ecx] = \
    "rep	stosd" \
    modify [edi ecx];

	    _memsetdd ((int *)reconptr, 0,
		    sizeof (blockPtr->dct_recon) / sizeof (int));
	}
#else
	memset ((char *)reconptr, 0, sizeof (blockPtr->dct_recon));
#endif

	if (curVidStream->mblock.mb_intra)
	{
	    int			diff, size;
	    unsigned int	next16bits;

	    /*
	     * This code has been hand optimized to get by the normal
	     * bit parsing routines.  We get some speedup by grabbing
	     * the next 16 bits and parsing things locally.
	     * Thus, calls are translated as:
	     *
	     *	show_bitsX  <-->   next16bits >> (16-X)
	     *	get_bitsX   <-->   val = next16bits >> (16-flushed-X);
	     *			   flushed += X;
	     *			   next16bits &= bitMask[flushed];
	     *	flush_bitsX <-->   flushed += X;
	     *			   next16bits &= bitMask[flushed];
	     *
	     * I've streamlined the code a lot, so that we don't have to mask
	     * out the low order bits and a few of the extra adds are removed.
	     *	bsmith
	     */
	    show_bits16 (next16bits);
	    if (n < 4)	/* Get the luminance bits. */
	    {
		i = next16bits >> (16-7);
		level = dct_dc_size_luminance[i].num_bits;
		size = dct_dc_size_luminance[i].value;

		if (diff = size)
		{
		    next16bits &= bitMask[16+level];
		    level += size;
		    diff = next16bits >> (16-level);
	            if (!(diff & bitTest[32-size]))
		    {
			diff = rBitMask[size] | (diff + 1);
		    }
		}

		size = diff << 3;
		if (n == 0 && curVidStream->mblock.mb_address -
			curVidStream->mblock.past_intra_addr > 1) 
		    size += 1024;
		else
		    size += DCT_dc_y_past;
		DCT_dc_y_past = size;
	    }
	    else	/* Get the chrominance bits.  */
	    {
		i = next16bits >> (16-8);
		level = dct_dc_size_chrominance[i].num_bits;
		size = dct_dc_size_chrominance[i].value;
	
		if (diff = size)
		{
		    next16bits &= bitMask[16+level];
		    level += size;
		    diff = next16bits >> (16-level);
	            if (!(diff & bitTest[32-size]))
		    {
			diff = rBitMask[size] | (diff + 1);
		    }
		}
	
		size = diff << 3;
		if (n == 4)
		{
		    if (curVidStream->mblock.mb_address -
			    curVidStream->mblock.past_intra_addr > 1) 
			size += 1024;
		    else
			size += DCT_dc_cr_past;
		    DCT_dc_cr_past = size;
		}
		else
		{
		    if (curVidStream->mblock.mb_address -
			  curVidStream->mblock.past_intra_addr > 1) 
			size += 1024;
		    else
			size += DCT_dc_cb_past;
		    DCT_dc_cb_past = size;
		}
	    }
	    flush_bits(level);
      
	    i = 0;
	    reconptr[pos = first_pos = 0] = size;

	    qscale = curVidStream->slice.quant_scale;
	    qmatrixptr = curVidStream->intra_quant_matrix[0];
	}
	else
	{
	    qscale = curVidStream->slice.quant_scale;
	    qmatrixptr = curVidStream->non_intra_quant_matrix[0];
      
	    DECODE_DCT_COEFF_FIRST(run, level);
	    i = run;

	    pos = first_pos = zigzag_direct[i];
	    reconptr[pos] = (level * qscale * qmatrixptr[pos]) >> 3;
	}
    
	if (curVidStream->picture.code_type != D_TYPE)
	{
	    while (1)
	    {
		DECODE_DCT_COEFF_NEXT(run, level);

		i = i + run + 1;
		pos = zigzag_direct[i];
		reconptr[pos] = (level * qscale * qmatrixptr[pos]) >> 3;
	    }
DoneDCTDecode:

#ifdef ANALYSIS 
	    {
		extern unsigned int *mbCoeffPtr;
		mbCoeffPtr[pos]++;
	    }
#endif
	    flush_bits(2);
	}

	if (pos == first_pos)
	    j_rev_dct_sparse (reconptr, pos);
	else
	    j_rev_dct (reconptr);

#ifdef RISC
	temp_curBits = curBits;
	temp_bitOffset = bitOffset;
	temp_bufLength = bufLength;
	temp_bitBuffer = bitBuffer;
#endif
    }

#ifdef RISC
    curBits = temp_curBits;
    bitOffset = temp_bitOffset;
    bufLength = temp_bufLength;
    bitBuffer = temp_bitBuffer;
#endif
}


/*
 *--------------------------------------------------------------
 *
 * ParseAwayBlock --
 *
 *	Parses off block values, throwing them away.
 *      Used with grayscale dithering.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *      None.
 *
 *--------------------------------------------------------------
 */

void
ParseAwayBlock(n)
     int n;
{
  unsigned int diff;
  unsigned int size, run;
  int level;

  if (bufLength < 100)
    correct_underflow();

  if (curVidStream->mblock.mb_intra) {

    /* If the block is a luminance block... */

    if (n < 4) {

      /* Parse and decode size of first coefficient. */

      DecodeDCTDCSizeLum(size);
    }

    /* Otherwise, block is chrominance block... */

    else {

      /* Parse and decode size of first coefficient. */

      DecodeDCTDCSizeChrom(size);
    }

    /* Parse first coefficient. */

    if (size != 0) {
      get_bitsn(size, diff);
    }
  }

  /* Otherwise, block is not intracoded... */

  else {

    /* Decode and set first coefficient. */

    DECODE_DCT_COEFF_FIRST(run, level);
  }

  /* If picture is not D type (i.e. I, P, or B)... */

  if (curVidStream->picture.code_type != D_TYPE) {

    /* While end of macroblock has not been reached... */

    while (1) {

      /* Get the dct_coeff_next */

      DECODE_DCT_COEFF_NEXT(run, level);
    }
DoneDCTDecode:

    /* End_of_block */

    flush_bits(2);
  }
}
