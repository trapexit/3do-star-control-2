#include <stdio.h>
#include <stdlib.h>
#include "sndlib.h"

main ()
{
    InitSound ();

    while (1)
    {
	COUNT	sound_index;

	printf ("Sound #: ");
	if (fscanf (stdin, "%u", &sound_index) != 1)
	    break;

	PlaySound (sound_index);
    }

    UninitSound ();

    exit (0);

