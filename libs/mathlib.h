#ifndef _MATHLIB_H
#define _MATHLIB_H

#include "compiler.h"

PROC_GLOBAL(
DWORD seed_random, (seed),
    ARG_END	(DWORD	seed)
);
PROC_GLOBAL(
DWORD random, (),
    ARG_VOID
);
PROC_GLOBAL(
COUNT square_root, (value),
    ARG_END	(DWORD	value)
);

#endif /* _MATHLIB_H */


