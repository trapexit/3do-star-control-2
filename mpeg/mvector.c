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
#include "video.h"
#include "proto.h"
#include "util.h"


/*
 *--------------------------------------------------------------
 *
 * ComputeVector --
 *
 *	Computes motion vector given parameters previously parsed
 *      and reconstructed.
 *
 * Results:
 *      Reconstructed motion vector info is put into recon_* parameters
 *      passed to this function. Also updated previous motion vector
 *      information.
 *
 * Side effects:
 *      None.
 *
 *--------------------------------------------------------------
 */

#define ComputeVector(recon_right_ptr, recon_down_ptr, \
	recon_right_prev, recon_down_prev, \
	f, full_pel_vector, \
	motion_h_code, motion_v_code, \
	motion_h_r, motion_v_r) \
									\
{									\
  register int new_vector, fact;					\
  int min, max;								\
									\
  /* The following procedure for the reconstruction of motion vectors 	\
     is a direct and simple implementation of the instructions given	\
     in the mpeg December 1991 standard draft. 				\
  */									\
									\
  fact = f;								\
  max = fact << 4;							\
  min = -max;								\
  if ((new_vector = motion_h_code) == 0)				\
    *recon_right_ptr = recon_right_prev;				\
  else {								\
    int	comp_h_r;							\
									\
    if (comp_h_r = fact - 1)						\
      comp_h_r -= motion_h_r;						\
    if ((new_vector *= fact) > 0) {					\
      new_vector -= comp_h_r;						\
      comp_h_r = -(fact << 5);						\
    }									\
    else {								\
      new_vector += comp_h_r;						\
      comp_h_r = (fact << 5);						\
    }									\
									\
    new_vector += recon_right_prev;					\
    if (new_vector < min || new_vector >= max)				\
      new_vector += comp_h_r;						\
    recon_right_prev = *recon_right_ptr = new_vector;			\
  }									\
									\
  if ((new_vector = motion_v_code) == 0)				\
    *recon_down_ptr = recon_down_prev;					\
  else {								\
    int	comp_v_r;							\
									\
    if (comp_v_r = fact - 1)						\
      comp_v_r -= motion_v_r;						\
    if ((new_vector *= fact) > 0) {					\
      new_vector -= comp_v_r;						\
      comp_v_r = -(fact << 5);						\
    }									\
    else {								\
      new_vector += comp_v_r;						\
      comp_v_r = (fact << 5);						\
    }									\
  									\
    new_vector += recon_down_prev;					\
    if (new_vector < min || new_vector >= max)				\
      new_vector += comp_v_r;						\
    recon_down_prev = *recon_down_ptr = new_vector;			\
  }									\
									\
  if (full_pel_vector)							\
  {									\
    *recon_right_ptr <<= 1;						\
    *recon_down_ptr <<= 1;						\
  }									\
}

/*
 *--------------------------------------------------------------
 *
 * ComputeForwVector --
 *
 *	Computes forward motion vector by calling ComputeVector
 *      with appropriate parameters.
 *
 * Results:
 *	Reconstructed motion vector placed in recon_right_for_ptr and
 *      recon_down_for_ptr.
 *
 * Side effects:
 *      None.
 *
 *--------------------------------------------------------------
 */

void 
ComputeForwVector(recon_right_for_ptr, recon_down_for_ptr)
     int *recon_right_for_ptr;
     int *recon_down_for_ptr;
{

  register Pict *picture;
  register Macroblock *mblock;

  picture = &(curVidStream->picture);
  mblock = &(curVidStream->mblock);

  ComputeVector(recon_right_for_ptr, recon_down_for_ptr,
		mblock->recon_right_for_prev, 
		mblock->recon_down_for_prev,
		picture->forw_f, picture->full_pel_forw_vector,
		mblock->motion_h_forw_code, mblock->motion_v_forw_code,
		mblock->motion_h_forw_r, mblock->motion_v_forw_r); 
}


/*
 *--------------------------------------------------------------
 *
 * ComputeBackVector --
 *
 *	Computes backward motion vector by calling ComputeVector
 *      with appropriate parameters.
 *
 * Results:
 *	Reconstructed motion vector placed in recon_right_back_ptr and
 *      recon_down_back_ptr.
 *
 * Side effects:
 *      None.
 *
 *--------------------------------------------------------------
 */

void 
ComputeBackVector(recon_right_back_ptr, recon_down_back_ptr)
     int *recon_right_back_ptr;
     int *recon_down_back_ptr;
{
  register Pict *picture;
  register Macroblock *mblock;

  picture = &(curVidStream->picture);
  mblock = &(curVidStream->mblock);

  ComputeVector(recon_right_back_ptr, recon_down_back_ptr,
		mblock->recon_right_back_prev, 
		mblock->recon_down_back_prev,
		picture->back_f, picture->full_pel_back_vector,
		mblock->motion_h_back_code, mblock->motion_v_back_code,
		mblock->motion_h_back_r, mblock->motion_v_back_r); 

}
