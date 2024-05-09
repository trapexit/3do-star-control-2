#ifndef _GFXINTRN_H
#define _GFXINTRN_H

#include <stdio.h>
#include <string.h>
#define _GFX_PROTOS
#include "gfxlib.h"
#include "reslib.h"
#include "context.h"
#include "drawable.h"
#include "font.h"
#include "display.h"

#undef CONTEXT
#define CONTEXT		CONTEXTPTR
#undef FRAME
#define FRAME		FRAMEPTR
#undef FONT
#define FONT		FONTPTR
#undef _GFX_PROTOS
#include "gfxlib.h"

#endif /* _GFXINTRN_H */

