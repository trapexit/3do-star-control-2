/*
 * jrevdct.c
 *
 * Copyright (C) 1991, 1992, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains the basic inverse-DCT transformation subroutine.
 *
 * This implementation is based on an algorithm described in
 *   C. Loeffler, A. Ligtenberg and G. Moschytz, "Practical Fast 1-D DCT
 *   Algorithms with 11 Multiplications", Proc. Int'l. Conf. on Acoustics,
 *   Speech, and Signal Processing 1989 (ICASSP '89), pp. 988-991.
 * The primary algorithm described there uses 11 multiplies and 29 adds.
 * We use their alternate method with 12 multiplies and 32 adds.
 * The advantage of this method is that no data path contains more than one
 * multiplication; this allows a very simple and accurate implementation in
 * scaled fixed-point arithmetic, with a minimal number of shifts.
 * 
 * I've made lots of modifications to attempt to take advantage of the
 * sparse nature of the DCT matrices we're getting.  Although the logic
 * is cumbersome, it's straightforward and the resulting code is much
 * faster.
 *
 * A better way to do this would be to pass in the DCT block as a sparse
 * matrix, perhaps with the difference cases encoded.
 */

#include "video.h"
#include "proto.h"

#define GLOBAL			/* a function referenced thru EXTERNs */
  
/* We assume that right shift corresponds to signed division by 2 with
 * rounding towards minus infinity.  This is correct for typical "arithmetic
 * shift" instructions that shift in copies of the sign bit.  But some
 * C compilers implement >> with an unsigned shift.  For these machines you
 * must define RIGHT_SHIFT_IS_UNSIGNED.
 * RIGHT_SHIFT provides a proper signed right shift of an int quantity.
 * It is only applied with constant shift counts.  SHIFT_TEMPS must be
 * included in the variables of any routine using RIGHT_SHIFT.
 */
  
#ifdef RIGHT_SHIFT_IS_UNSIGNED
#define SHIFT_TEMPS	int shift_temp;
#define RIGHT_SHIFT(x,shft)  \
	((shift_temp = (x)) < 0 ? \
	 (shift_temp >> (shft)) | ((~((int) 0)) << (32-(shft))) : \
	 (shift_temp >> (shft)))
#else
#define SHIFT_TEMPS
#define RIGHT_SHIFT(x,shft)	((x) >> (shft))
#endif

/*
 * This routine is specialized to the case DCTSIZE = 8.
 */

#if DCTSIZE != 8
  Sorry, this code only copes with 8x8 DCTs. /* deliberate syntax err */
#endif


/*
 * A 2-D IDCT can be done by 1-D IDCT on each row followed by 1-D IDCT
 * on each column.  Direct algorithms are also available, but they are
 * much more complex and seem not to be any faster when reduced to code.
 *
 * The poop on this scaling stuff is as follows:
 *
 * Each 1-D IDCT step produces outputs which are a factor of sqrt(N)
 * larger than the true IDCT outputs.  The final outputs are therefore
 * a factor of N larger than desired; since N=8 this can be cured by
 * a simple right shift at the end of the algorithm.  The advantage of
 * this arrangement is that we save two multiplications per 1-D IDCT,
 * because the y0 and y4 inputs need not be divided by sqrt(N).
 *
 * We have to do addition and subtraction of the integer inputs, which
 * is no problem, and multiplication by fractional constants, which is
 * a problem to do in integer arithmetic.  We multiply all the constants
 * by CONST_SCALE and convert them to integer constants (thus retaining
 * CONST_BITS bits of precision in the constants).  After doing a
 * multiplication we have to divide the product by CONST_SCALE, with proper
 * rounding, to produce the correct output.  This division can be done
 * cheaply as a right shift of CONST_BITS bits.  We postpone shifting
 * as long as possible so that partial sums can be added together with
 * full fractional precision.
 *
 * The outputs of the first pass are scaled up by PASS1_BITS bits so that
 * they are represented to better-than-integral precision.  These outputs
 * require BITS_IN_JSAMPLE + PASS1_BITS + 3 bits; this fits in a 16-bit word
 * with the recommended scaling.  (To scale up 12-bit sample data further, an
 * intermediate int array would be needed.)
 *
 * To avoid overflow of the 32-bit intermediate results in pass 2, we must
 * have BITS_IN_JSAMPLE + CONST_BITS + PASS1_BITS <= 26.  Error analysis
 * shows that the values given below are the most effective.
 */

#ifdef EIGHT_BIT_SAMPLES
#define PASS1_BITS  2
#else
#define PASS1_BITS  1		/* lose a little precision to avoid overflow */
#endif

#define ONE	((int) 1)

/* Definition of Contant integer scale factor. */

#define CONST_BITS 13

#define CONST_SCALE (ONE << CONST_BITS)

/* Convert a positive real constant to an integer scaled by CONST_SCALE.
 * IMPORTANT: if your compiler doesn't do this arithmetic at compile time,
 * you will pay a significant penalty in run time.  In that case, figure
 * the correct integer constant values and insert them by hand.
 */

#define FIX(x)	((int) ((x) * CONST_SCALE + 0.5))

/* Descale and correctly round an int value that's scaled by N bits.
 * We assume RIGHT_SHIFT rounds towards minus infinity, so adding
 * the fudge factor is correct for either sign of X.
 */

#define DESCALE(x,n)  RIGHT_SHIFT((x) + (ONE << ((n)-1)), n)

/* Multiply an int variable by an int constant to yield an int result.
 * For 8-bit samples with the recommended scaling, all the variable
 * and constant values involved are no more than 16 bits wide, so a
 * 16x16->32 bit multiply can be used instead of a full 32x32 multiply;
 * this provides a useful speedup on many machines.
 * There is no way to specify a 16x16->32 multiply in portable C, but
 * some C compilers will do the right thing if you provide the correct
 * combination of casts.
 * NB: for 12-bit samples, a full 32-bit multiplication will be needed.
 */

#ifdef EIGHT_BIT_SAMPLES
#ifdef SHORTxSHORT_32		/* may work if 'int' is 32 bits */
#define MULTIPLY(var,const)  (((short) (var)) * ((short) (const)))
#endif
#ifdef SHORTxLCONST_32		/* known to work with Microsoft C 6.0 */
#define MULTIPLY(var,const)  (((short) (var)) * ((int) (const)))
#endif
#endif

#define C0	-FIX(0.899976223)
#define C1	-FIX(2.562915447)
#define C2	FIX(1.175875602)
#define C3	-FIX(1.961570560)
#define C4	-FIX(0.390180644)
#define C5	FIX(0.298631336)
#define C6	FIX(2.053119869)
#define C7	FIX(3.072711026)
#define C8	FIX(1.501321110)
#define C9	FIX(0.541196100)
#define C10	-FIX(1.847759065)
#define C11	FIX(0.765366865)

/* Precomputed idct value arrays. */

static DCTBLOCK	PreIDCT[64];

#define MULTIPLY_TABLES
#ifdef MULTIPLY_TABLES
#define MAX_COEFF	2841
static int	cval_tab[12][(MAX_COEFF + 1) * 2];
#endif /* MULTIPLY_TABLES */

/* Pre compute singleton coefficient IDCT values. */
void
init_pre_idct ()
{
    int	i;
#ifdef MULTIPLY_TABLES
    int	const_tab[] =
    {
	C0, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11
    };

    for (i = 0; i < sizeof (const_tab) / sizeof (const_tab[0]); ++i)
    {
	int	j, val, C;

	C = const_tab[i];
	for (j = 0, val = 0; j < (MAX_COEFF + 1); ++j, val += C)
	{
	    cval_tab[i][MAX_COEFF + j] = val;
	    cval_tab[i][MAX_COEFF - j] = -val;
	}
    }
#undef C0 
#undef C1 
#undef C2 
#undef C3 
#undef C4 
#undef C5 
#undef C6 
#undef C7 
#undef C8 
#undef C9 
#undef C10
#undef C11
#define C0	0
#define C1	1
#define C2	2
#define C3	3
#define C4	4
#define C5	5
#define C6	6
#define C7	7
#define C8	8
#define C9	9
#define C10	10
#define C11	11

#undef PASS1_BITS
#define PASS1_BITS  0

#define MULTIPLY(var,const)  cval_tab[const][var + MAX_COEFF]
#endif /* MULTIPLY_TABLES */

    for (i = 0; i < 64; i++)
    {
	PreIDCT[i][i] = 1 << (CONST_BITS - 3);
	j_rev_dct (PreIDCT[i]);
    }
}

/*
 * Perform the inverse DCT on one block of coefficients.
 */

void
j_rev_dct_sparse (DCTBLOCK data, int pos)
{
    register DCTELEM	*dataptr;
    register DCTELEM	*ndataptr;
    register int	coeff, rr;

    /* If DC Coefficient. */
  
    dataptr = (DCTELEM *)data;
    if (pos == 0)
    {
	/* Compute 32 bit value to assign.  This speeds things up a bit */
	coeff = *dataptr >> 3;
//	if (coeff < 0)
//          coeff = (coeff-3)>>3;
//	else
//	    coeff = (coeff+4)>>3;
	if (sizeof (DCTELEM) == sizeof (short))
	    coeff = (coeff & 0xFFFF) | (coeff << 16);

#if (MSDOS && __WATCOMC__)
	{
	    extern void _memsetdd (int *, int, int);
#pragma aux _memsetdd parm [edi] [eax] [ecx] = \
    "rep	stosd" \
    modify [edi ecx];

	    _memsetdd ((int *)dataptr, coeff,
		    sizeof (DCTBLOCK) / sizeof (int));
	}
#else
	((int *)dataptr)[0] = ((int *)dataptr)[1] =
	((int *)dataptr)[2] = ((int *)dataptr)[3] =
	((int *)dataptr)[4] = ((int *)dataptr)[5] =
	((int *)dataptr)[6] = ((int *)dataptr)[7] =
	((int *)dataptr)[8] = ((int *)dataptr)[9] =
	((int *)dataptr)[10] = ((int *)dataptr)[11] =
	((int *)dataptr)[12] = ((int *)dataptr)[13] =
	((int *)dataptr)[14] = ((int *)dataptr)[15] =
	((int *)dataptr)[16] = ((int *)dataptr)[17] =
	((int *)dataptr)[18] = ((int *)dataptr)[19] =
	((int *)dataptr)[20] = ((int *)dataptr)[21] =
	((int *)dataptr)[22] = ((int *)dataptr)[23] =
	((int *)dataptr)[24] = ((int *)dataptr)[25] =
	((int *)dataptr)[26] = ((int *)dataptr)[27] =
	((int *)dataptr)[28] = ((int *)dataptr)[29] =
	((int *)dataptr)[30] = ((int *)dataptr)[31] = coeff;
	if (sizeof (DCTELEM) == sizeof (int))
	{
	    dataptr[32+0] = dataptr[32+1] =
	    dataptr[32+2] = dataptr[32+3] =
	    dataptr[32+4] = dataptr[32+5] =
	    dataptr[32+6] = dataptr[32+7] =
	    dataptr[32+8] = dataptr[32+9] =
	    dataptr[32+10] = dataptr[32+11] =
	    dataptr[32+12] = dataptr[32+13] =
	    dataptr[32+14] = dataptr[32+15] =
	    dataptr[32+16] = dataptr[32+17] =
	    dataptr[32+18] = dataptr[32+19] =
	    dataptr[32+20] = dataptr[32+21] =
	    dataptr[32+22] = dataptr[32+23] =
	    dataptr[32+24] = dataptr[32+25] =
	    dataptr[32+26] = dataptr[32+27] =
	    dataptr[32+28] = dataptr[32+29] =
	    dataptr[32+30] = dataptr[32+31] = coeff;
	}
#endif

	return;
    }
  
    /* Some other coefficient. */
    coeff = dataptr[pos];
    ndataptr = PreIDCT[pos];
    for (rr=0; rr<4; rr++)
    {
	dataptr[0] = (ndataptr[0] * coeff) >> (CONST_BITS-3);
	dataptr[1] = (ndataptr[1] * coeff) >> (CONST_BITS-3);
	dataptr[2] = (ndataptr[2] * coeff) >> (CONST_BITS-3);
	dataptr[3] = (ndataptr[3] * coeff) >> (CONST_BITS-3);
	dataptr[4] = (ndataptr[4] * coeff) >> (CONST_BITS-3);
	dataptr[5] = (ndataptr[5] * coeff) >> (CONST_BITS-3);
	dataptr[6] = (ndataptr[6] * coeff) >> (CONST_BITS-3);
	dataptr[7] = (ndataptr[7] * coeff) >> (CONST_BITS-3);
	dataptr[8] = (ndataptr[8] * coeff) >> (CONST_BITS-3);
	dataptr[9] = (ndataptr[9] * coeff) >> (CONST_BITS-3);
	dataptr[10] = (ndataptr[10] * coeff) >> (CONST_BITS-3);
	dataptr[11] = (ndataptr[11] * coeff) >> (CONST_BITS-3);
	dataptr[12] = (ndataptr[12] * coeff) >> (CONST_BITS-3);
	dataptr[13] = (ndataptr[13] * coeff) >> (CONST_BITS-3);
	dataptr[14] = (ndataptr[14] * coeff) >> (CONST_BITS-3);
	dataptr[15] = (ndataptr[15] * coeff) >> (CONST_BITS-3);
	dataptr += 16;
	ndataptr += 16;
    }
}

//int	min_coeff = 0x7FFF, max_coeff = -0x7FFF;
//int MULTIPLY (int v1, int v2)
//{
//#define MULTIPLY	MULTIPLY
//    if (v1 < min_coeff) min_coeff = v1;
//    if (v1 > max_coeff) max_coeff = v1;
//    return (v1 * v2);
//}

#ifndef MULTIPLY		/* default definition */
#define MULTIPLY(var,const)  ((var) * (const))
#endif

void
j_rev_dct (DCTBLOCK data)
{
    int			tmp, tmp0, tmp1, tmp2, tmp3;
    int			z0, z1, z2, z3;
    register DCTELEM	*dataptr;
    int			rowctr;
    SHIFT_TEMPS
   
    /* Pass 1: process rows. */
    /* Note results are scaled up by sqrt(8) compared to a true IDCT; */
    /* furthermore, we scale the results by 2**PASS1_BITS. */

    for (rowctr = DCTSIZE-1, dataptr = data;
	    rowctr >= 0; rowctr--, dataptr += DCTSIZE)
    {
	/* Due to quantization, we will usually find that many of the input
	 * coefficients are zero, especially the AC terms.  We can exploit this
	 * by short-circuiting the IDCT calculation for any row in which all
	 * the AC terms are zero.  In that case each output is equal to the
	 * DC coefficient (with scale factor as needed).
	 * With typical images and quantization tables, half or more of the
	 * row DCT calculations can be simplified this way.
	 */

	if ((sizeof (DCTELEM) == sizeof (short)
		&& dataptr[1] == 0
		&& ((int *)dataptr)[1] == 0
		&& ((int *)dataptr)[2] == 0
		&& ((int *)dataptr)[3] == 0)
		|| (sizeof (DCTELEM) == sizeof (int)
		&& dataptr[1] == 0
		&& dataptr[2] == 0
		&& dataptr[3] == 0
		&& dataptr[4] == 0
		&& dataptr[5] == 0
		&& dataptr[6] == 0
		&& dataptr[7] == 0))
	{
	    /* AC terms all zero */
	    if (dataptr[0])
	    {
		/* Compute a 32 bit value to assign. */
		tmp = dataptr[0] << PASS1_BITS;
		if (sizeof (DCTELEM) == sizeof (short))
		    tmp = (tmp & 0xFFFF) | (tmp << 16);
#if (MSDOS && __WATCOMC__)
		{
#pragma aux _memsetdd parm [edi] [eax] [ecx] = \
    "rep	stosd" \
    modify [edi ecx];
		    extern void _memsetdd (int *, int, int);

		    _memsetdd ((int *)dataptr, tmp,
			    (DCTSIZE * sizeof (DCTELEM)) / sizeof (int));
		}
#else
		((int *)dataptr)[0] = ((int *)dataptr)[1] =
			((int *)dataptr)[2] = ((int *)dataptr)[3] = tmp;
		if (sizeof (DCTELEM) == sizeof (int))
		    dataptr[4] = dataptr[5] = dataptr[6] = dataptr[7] = tmp;
#endif
	    }
      
	    continue;
	}

	/* Odd part per figure 8; the matrix is unitary and hence its
	 * transpose is its inverse.  i0..i3 are y7,y5,y3,y1 respectively.
	 */

	z0 = MULTIPLY(dataptr[7] + dataptr[1], C0);
	z1 = MULTIPLY(dataptr[5] + dataptr[3], C1);
	tmp2 = dataptr[7] + dataptr[3];
	tmp3 = dataptr[5] + dataptr[1];
	tmp = MULTIPLY(tmp2 + tmp3, C2);
	z2 = MULTIPLY(tmp2, C3) + tmp;
	z3 = MULTIPLY(tmp3, C4) + tmp;
		    
	tmp0 = MULTIPLY(dataptr[7], C5) + z0 + z2;
	tmp1 = MULTIPLY(dataptr[5], C6) + z1 + z3;
	tmp2 = MULTIPLY(dataptr[3], C7) + z1 + z2;
	tmp3 = MULTIPLY(dataptr[1], C8) + z0 + z3;

	/* Even part: reverse the even part of the forward DCT. */
	/* The rotator is sqrt(2)*c(-6). */

	z0 = (dataptr[0] + dataptr[4]) << CONST_BITS;
	z1 = (dataptr[0] - dataptr[4]) << CONST_BITS;
	tmp = MULTIPLY(dataptr[2] + dataptr[6], C9);
	z2 = tmp + MULTIPLY(dataptr[6], C10);
	z3 = tmp + MULTIPLY(dataptr[2], C11);

	/* Final output stage: inputs are tmp10..tmp13, tmp0..tmp3 */

	tmp = z0 + z3;
	dataptr[0] = (DCTELEM) DESCALE(tmp + tmp3, CONST_BITS-PASS1_BITS);
	dataptr[7] = (DCTELEM) DESCALE(tmp - tmp3, CONST_BITS-PASS1_BITS);
	tmp = z1 + z2;
	dataptr[1] = (DCTELEM) DESCALE(tmp + tmp2, CONST_BITS-PASS1_BITS);
	dataptr[6] = (DCTELEM) DESCALE(tmp - tmp2, CONST_BITS-PASS1_BITS);
	tmp = z1 - z2;
	dataptr[2] = (DCTELEM) DESCALE(tmp + tmp1, CONST_BITS-PASS1_BITS);
	dataptr[5] = (DCTELEM) DESCALE(tmp - tmp1, CONST_BITS-PASS1_BITS);
	tmp = z0 - z3;
	dataptr[3] = (DCTELEM) DESCALE(tmp + tmp0, CONST_BITS-PASS1_BITS);
	dataptr[4] = (DCTELEM) DESCALE(tmp - tmp0, CONST_BITS-PASS1_BITS);
    }

    /* Pass 2: process columns. */
    /* Note that we must descale the results by a factor of 8 == 2**3, */
    /* and also undo the PASS1_BITS scaling. */

    for (rowctr = DCTSIZE - 1,
	    dataptr = data; rowctr >= 0; rowctr--, dataptr++)
    {
	/* Columns of zeroes can be exploited in the same way as we did with rows.
	 * However, the row calculation has created many nonzero AC terms, so the
	 * simplification applies less often (typically 5% to 10% of the time).
	 * On machines with very fast multiplication, it's possible that the
	 * test takes more time than it's worth.  In that case this section
	 * may be commented out.
	 */

	/* Odd part per figure 8; the matrix is unitary and hence its
	 * transpose is its inverse.  i0..i3 are y7,y5,y3,y1 respectively.
	 */

	z0 = MULTIPLY(dataptr[DCTSIZE*7] + dataptr[DCTSIZE*1], C0);
	z1 = MULTIPLY(dataptr[DCTSIZE*5] + dataptr[DCTSIZE*3], C1);
	tmp2 = dataptr[DCTSIZE*7] + dataptr[DCTSIZE*3];
	tmp3 = dataptr[DCTSIZE*5] + dataptr[DCTSIZE*1];
	tmp = MULTIPLY(tmp2 + tmp3, C2);
	z2 = MULTIPLY(tmp2, C3) + tmp;
	z3 = MULTIPLY(tmp3, C4) + tmp;
		    
	tmp0 = MULTIPLY(dataptr[DCTSIZE*7], C5) + z0 + z2; 
	tmp1 = MULTIPLY(dataptr[DCTSIZE*5], C6) + z1 + z3;
	tmp2 = MULTIPLY(dataptr[DCTSIZE*3], C7) + z1 + z2;
	tmp3 = MULTIPLY(dataptr[DCTSIZE*1], C8) + z0 + z3;

	/* Even part: reverse the even part of the forward DCT. */
	/* The rotator is sqrt(2)*c(-6). */

	z0 = (dataptr[DCTSIZE*0] + dataptr[DCTSIZE*4]) << CONST_BITS;
	z1 = (dataptr[DCTSIZE*0] - dataptr[DCTSIZE*4]) << CONST_BITS;
	tmp = MULTIPLY(dataptr[DCTSIZE*2] + dataptr[DCTSIZE*6], C9);
	z2 = tmp + MULTIPLY(dataptr[DCTSIZE*6], C10);
	z3 = tmp + MULTIPLY(dataptr[DCTSIZE*2], C11);

	/* Final output stage: inputs are tmp10..tmp13, tmp0..tmp3 */

	tmp = z0 + z3;
	dataptr[DCTSIZE*0] = (DCTELEM) DESCALE(tmp + tmp3,
	    	CONST_BITS+PASS1_BITS+3);
	dataptr[DCTSIZE*7] = (DCTELEM) DESCALE(tmp - tmp3,
	    	CONST_BITS+PASS1_BITS+3);
	tmp = z1 + z2;
	dataptr[DCTSIZE*1] = (DCTELEM) DESCALE(tmp + tmp2,
	    	CONST_BITS+PASS1_BITS+3);
	dataptr[DCTSIZE*6] = (DCTELEM) DESCALE(tmp - tmp2,
	    	CONST_BITS+PASS1_BITS+3);
	tmp = z1 - z2;
	dataptr[DCTSIZE*2] = (DCTELEM) DESCALE(tmp + tmp1,
	    	CONST_BITS+PASS1_BITS+3);
	dataptr[DCTSIZE*5] = (DCTELEM) DESCALE(tmp - tmp1,
	    	CONST_BITS+PASS1_BITS+3);
	tmp = z0 - z3;
	dataptr[DCTSIZE*3] = (DCTELEM) DESCALE(tmp + tmp0,
	    	CONST_BITS+PASS1_BITS+3);
	dataptr[DCTSIZE*4] = (DCTELEM) DESCALE(tmp - tmp0,
		CONST_BITS+PASS1_BITS+3);
    }
}

