#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "compiler.h"
#include "mathlib.h"

PROC(
void CDECL main, (argc, argv),
    ARG		(int	argc)
    ARG_END	(char	*argv[])
)
{
    COUNT	i;
    time_t	t;

    time (&t);
    seed_random ((DWORD)t);

    for (i = 0; i < 10000; ++i)
    {
	COUNT	j;

	for (j = ((COUNT)random () % 6) + 3; j; --j)
	{
	    printf ("%c", (random () % 26) + 'a');
	}
	printf ("\n");
    }
}

