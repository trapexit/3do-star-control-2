#include "starcon.h"
#include "vargs.h"

PROC(
DWORD CDECL race_glue, (selector, args),
    ARG		(COUNT 		selector)
    ARG		(vararg_dcl	args)
    ARG_VAR
)
{
    return (0);
}

BOOLEAN
Battle ()
{
    return (FALSE);
}

INPUT_STATE
tactical_intelligence ()
{
    return (0);
}

