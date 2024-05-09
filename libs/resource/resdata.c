#include "resintrn.h"

static INDEX_HEADERPTR	CurResHeaderPtr;

PROC(
void near _set_current_index_header, (ResHeaderPtr),
    ARG_END	(INDEX_HEADERPTR	ResHeaderPtr)
)
{
    CurResHeaderPtr = ResHeaderPtr;
}

PROC(
INDEX_HEADERPTR	near _get_current_index_header, (),
    ARG_VOID
)
{
    return (CurResHeaderPtr);
}

