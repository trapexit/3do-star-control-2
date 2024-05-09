#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "memintrn.h"

#ifdef REPORT_ERRORS

#define LIBNAME		"libmem"

#define COMPILED_ERRORS
#ifdef COMPILED_ERRORS

static PSTR	far mem_strs[] =
{
#include "mem_strs.h"
};

#else /* COMPILED_ERRORS */

#define ERR_STRING_PATH	"mem_strs"

#endif /* COMPILED_ERRORS */

PSTR		far _err_func_name;
ERR_ID		far _mem_err_id;

PROC(
MEM_BOOL near _mem_err_init, (),
    ARG_VOID
)
{
    return ((_mem_err_id =
#ifdef COMPILED_ERRORS
	    err_register (LIBNAME, ERR_IN_MEMORY, (PSTR)mem_strs,
	    sizeof (mem_strs) / sizeof (mem_strs[0]))) == -1 ?
#else /* COMPILED_ERRORS */
	    err_register (LIBNAME, ERR_IN_FILE, ERR_STRING_PATH,
	    0)) == -1 ?
#endif /* COMPILED_ERRORS */
    	    MEM_FAILURE : MEM_SUCCESS);
}

PROC(
MEM_BOOL near _mem_err_uninit, (),
    ARG_VOID
)
{
    return (err_unregister (_mem_err_id));
}

PROC(
void near _mem_err_fatal, (),
    ARG_VOID
)
{
    char	buf[500];

#ifdef COMPILED_ERRORS
    sprintf (buf, "%s", mem_strs[1]);
#else /* COMPILED_ERRORS */
    FILE	*fp;

    buf[0] = '\0';
    if ((fp = fopen (ERR_STRING_PATH, "r")) != NULL)
    {
	int	index;

	index = (int)MEM_NO_ERROR;
	while (fgets (buf, sizeof (buf) - 1, fp) != NULL)
	{
	    SWORD	in_string;
	    char	c, *str = buf, *dst = buf;

	    in_string = -1;
	    while (c = *str++)
	    {
		if (in_string == 0)
		{
		    if (c == '"')
		    {
			if (str[-2] != '\\')
			{
			    *dst++ = '\0';
			    ++in_string;
			    break;
			}
			--dst;
		    }
		    *dst++ = c;
		}
		else if (in_string < 0)
		{
		    if (c == '"')
			++in_string;
		}
	    }

	    if (in_string > 0 && index++ == (int)MEM_NOT_INITIALIZED)
		break;
	}

	fclose (fp);
    }
#endif /* COMPILED_ERRORS */

    fprintf (stderr, "(%s/%s): %s\n", LIBNAME, FUNC_NAME, buf);
    exit (1);
}

#endif /* REPORT_ERRORS */

