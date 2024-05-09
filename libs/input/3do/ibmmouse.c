#include "compiler.h"

PROC(
BOOLEAN FindMouse, (),
    ARG_VOID
)
{
    return (FALSE);
}

PROC(
void MoveMouse, (x, y),
    ARG		(SWORD	x)
    ARG_END	(SWORD	y)
)
{
}

PROC(
BYTE LocateMouse, (px, py),
    ARG		(PSWORD	px)
    ARG_END	(PSWORD	py)
)
{
    return (0);
}

