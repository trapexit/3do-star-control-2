#include <ctype.h>
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
    BOOLEAN	ParagraphPad;

    ParagraphPad = FALSE;
    control_fp = out_fp = 0;
    while (--argc > 0)
    {
	++argv;
	if ((*argv)[0] == '-'
		&& toupper ((*argv)[1]) == 'P')
	    ParagraphPad = TRUE;
	else if (control_fp == 0)
	{
	    if ((control_fp = fopen (*argv, "r")) == NULL)
	    {
		printf ("Unable to access control file '%s'\n", argv[1]);
		exit (1);
	    }
	}
	else if (out_fp == 0)
	{
	    if ((out_fp = fopen (*argv, "wb")) == NULL)
	    {
		printf ("Unable to open output file '%s'\n", argv[2]);
		exit (2);
	    }
	}
    }

    if (control_fp == 0 || out_fp == 0)
	exit (3);

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
	else if (ParagraphPad)
	{
	    DWORD	pos;

	    pos = ftell (out_fp);
	    if (len = ((pos + (PARAGRAPH_SIZE - 1))
		    & ~(PARAGRAPH_SIZE - 1)) - pos)
	    {
		fseek (out_fp, sizeof (num_frames), SEEK_SET);
		fwrite (&len, sizeof (len), 1, out_fp);
		fseek (out_fp, (long)(pos + len), SEEK_SET);
	    }
	}

	for (i = 0; i < num_frames; ++i)
	{
	    fgets (buffer, sizeof (buffer), control_fp);
	    buffer[strlen (buffer) - 1] = '\0';
	    printf ("\t%s", buffer);
	    {
		FILE	*in_fp;

		if ((in_fp = fopen (buffer, "rb")) == NULL)
		{
		    printf ("Unable to access input file '%s'\n", buffer);
		    exit (3);
		}
		ffilelength (in_fp, &len);
		if (Pass == 0)
		{
		    if (ParagraphPad)
			len = (len + (PARAGRAPH_SIZE - 1))
				& ~(PARAGRAPH_SIZE - 1);
		}
		else
		{
		    printf (" -- at offset %lu\n", ftell (out_fp));
		    while (len)
		    {
			COUNT	l;

			l = fread (buffer, 1, sizeof (buffer), in_fp);
			fwrite (buffer, 1, l, out_fp);

			len -= l;
		    }

		    if (ParagraphPad)
			fseek (out_fp,
				(ftell (out_fp) + (PARAGRAPH_SIZE - 1))
				& ~(PARAGRAPH_SIZE - 1),
				SEEK_SET);
		}
		fclose (in_fp);
	    }

	    if (Pass == 0)
	    {
		printf (" -- %u bytes\n", len);
		fwrite (&len, sizeof (len), 1, out_fp);
	    }
	}
    }

    fclose (control_fp);
    fclose (out_fp);

    exit (0);
}

