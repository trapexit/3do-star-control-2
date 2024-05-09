#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "compiler.h"

static char	buffer[512];

#define NUM_PASSES	2

PROC(
void CDECL main, (argc, argv),
    ARG		(int	argc)
    ARG_END	(char	*argv[])
)
{
    COUNT	Pass;
    FILE	*control_fp, *out_fp;

    if ((control_fp = fopen (argv[1], "r")) == NULL)
    {
	printf ("Unable to access control file '%s'\n", argv[1]);
	exit (1);
    }

    if ((out_fp = fopen (argv[2], "wb")) == NULL)
    {
	printf ("Unable to open output file '%s'\n", argv[2]);
	exit (1);
    }

    for (Pass = 0; Pass < NUM_PASSES; ++Pass)
    {
	COUNT	i, num_frames;
	DWORD	len;

	printf ("PASS %u\n", Pass);

	rewind (control_fp);
	fscanf (control_fp, "%u", &num_frames);
	fgets (buffer, sizeof (buffer), control_fp);	/* get end of line */
	if (Pass == 0)
	{
	    fwrite (&num_frames, sizeof (num_frames), 1, out_fp);
	    len = 0;
	    fwrite (&len, sizeof (len), 1, out_fp);
	}

	for (i = 0; i < num_frames; ++i)
	{
	    fgets (buffer, sizeof (buffer), control_fp);
	    printf ("\t%s", buffer);
	    len = strlen (buffer) - 1;
	    if (Pass)
		fwrite (buffer, (COUNT)len, 1, out_fp);
	    else
		fwrite (&len, sizeof (len), 1, out_fp);
	}
    }

    fclose (control_fp);
    fclose (out_fp);

    exit (0);
}

