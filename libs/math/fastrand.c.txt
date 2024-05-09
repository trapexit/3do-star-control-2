#include "mthintrn.h"
#include "random.h"	/* get the externs for error checking */

/* ----------------------------GLOBAL DATA-------------------------------- */

DWORD far	random_table[TABLE_SIZE];
COUNT		fast_index = 0;	/* fast random cycling index */

/* ----------------------------PROTOTYPES--------------------------------- */

DWORD	indexed_random_table(COUNT index);
void	reseed_fast_random();
DWORD	fast_random();
DWORD	seed_fast_random(DWORD seed);

/*****************************************************************************
* FUNC: DWORD	seed_fast_random(DWORD seed)
* 
* DESC:generate a small table of random numbers for later use by fast_random()
* 
* NOTES: the table has a prime number of entries
* 
* HISTORY:	Created By Robert Leyland
* 
*****************************************************************************/

DWORD
seed_fast_random(DWORD seed)
{
	DWORD	retval;
	COUNT		index;

	retval = seed_random(seed);

	for( index = 0; index < TABLE_SIZE; index++ )
		random_table[index] = random();

	fast_index = 0;		/* must be set to the start of the table for
							repeatability... */

	return (retval);
}


/*****************************************************************************
* FUNC: DWORD	fast_random()
* 
* DESC:return the next random number from the random number table
* 
* NOTES:
* 
* HISTORY:	Created By Robert Leyland
* 
*****************************************************************************/

DWORD
fast_random()
{
	fast_index++;
	if( fast_index == TABLE_SIZE )
		fast_index = 0;

	return( random_table[fast_index] );
}

/*****************************************************************************
* FUNC: void	reseed_fast_random()
* 
* DESC:shifts the table pointer by calling the "real" random
* 
* NOTES:
* 
* HISTORY:	Created By Robert Leyland
* 
*****************************************************************************/

void
reseed_fast_random()
{
	fast_index = (COUNT)(random() % TABLE_SIZE);
	
}


/*****************************************************************************
* FUNC: DWORD	indexed_random_table(COUNT index)
* 
* DESC:return a "random" number from the random number table, as indexed
*		for repeatability
* 
* NOTES:used by texturing functions
* 
* HISTORY:	Created By Robert Leyland
* 
*****************************************************************************/

DWORD
indexed_random_table(COUNT index)
{
	if( index > TABLE_SIZE )			/* only do % if really needed */
		index = index % TABLE_SIZE;

	return(random_table[index]);
}



