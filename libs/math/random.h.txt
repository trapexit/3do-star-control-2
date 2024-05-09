/****************************************************************************
* FILE:	random.h
* DESC:	definitions and externs for random number generators
* 
* HISTORY:	Created	 6/ 6/1989
* LAST CHANGED:
* 
*	Copyright (c) 1989, Robert Leyland and Scott Anderson
****************************************************************************/

/* ----------------------------DEFINES------------------------------------ */

#ifndef SLOW_N_STUPID
#define TABLE_SIZE	1117		/* a "nice" prime number */
#define _FAST_	fast_random()
#else	/* FAST_N_UGLY */
#define TABLE_SIZE	( (1 << 10) - 1 )
#define _FAST_	( random_table[ fast_index++ & TABLE_SIZE ] )
#endif


#define	FASTRAND(n)			( (int) ( (unsigned int)_FAST_ % (n) ) )
#define	SFASTRAND(n)		( (int)_FAST_ % (n) )
#define	AND_FASTRAND(n)	( (int)_FAST_ & (n) )

#define	RAND(n)				( (int) ( (unsigned int)random() % (n) ) )
#define	SRAND(n)				( (int)random() % (n) )
#define	AND_RAND(n)			( (int)random() & (n) )

#define INDEXED_RANDOM(x)	(random_table[x])

/* ----------------------------GLOBALS/EXTERNS---------------------------- */

extern DWORD far	random_table[TABLE_SIZE];
extern COUNT		fast_index;			/* fast random cycling index */

