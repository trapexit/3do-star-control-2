/****************************************************************************
* FILE:	random.c
* DESC:	a library of random number generators for general purpose use.
*
* References:
*	"Random Number Generators: Good ones are hard to find" S.K.Park & K.W.Miller
*	Communications of the ACM, Vol31 Number 10, October 1988, Pp 1192-1201
* 
* HISTORY:	Created	 1/23/1989
* LAST CHANGED:
* 
*	Copyright (c) 1989, Robert Leyland and Fred Ford
****************************************************************************/

/* ----------------------------INCLUDES----------------------------------- */
#include "mthintrn.h"	/* get the externs for error checking */

/* ----------------------------DEFINES------------------------------------ */
/* constants for linear congruential random number generator from CACM article
   referenced above */
#define A 16807		/* a relatively prime number -- also M div Q */
#define M 2147483647L		/* 0xFFFFFFFF / 2 */
#define Q 127773L		/* M div A */
#define R 2836			/* M mod A */

/* ----------------------------STATIC DATA-------------------------------- */

static DWORD		seed = 12345L;	/* random number seed */

/* ----------------------------CODE--------------------------------------- */

/*****************************************************************************
* FUNC: DWORD	random()
* 
* DESC: random number generator
* 
* NOTES:
* 
* HISTORY:	Created By Robert leyland
* 
*****************************************************************************/

PROC(
DWORD random, (),
    ARG_VOID
)
{
    if ((seed = A * (seed % Q) - R * (seed / Q)) > M)
	return (seed -= M);
    else if (seed)
	return (seed);
    else
	return (seed = 1L);
}

/*****************************************************************************
* FUNC: DWORD	seed_random(DWORD l)
* 
* DESC:	set the seed for the random number generator to parameter "l", and
*	return the value of the previously active seed, to allow for multiple
*	random number streams.
* 
* NOTES: if the seed is not valid it will be coerced into a valid range
* 
* HISTORY:	Created By Robert leyland
* 
*****************************************************************************/

PROC(
DWORD seed_random, (new_seed),
    ARG_END	(DWORD	new_seed)
)
{
    DWORD	old_seed;

    /* coerce the seed to be in the range 1..M */
    if (new_seed == 0L)	/* 0 becomes 1 */
	new_seed = 1;
    else if (new_seed > M)	/* and less than M */
	new_seed -= M;

    old_seed = seed;
    seed = new_seed;
    return (old_seed);
}

