#include "gfxintrn.h"


/*
	LoadDisplay---Drawable

	Allocate a chunk of memory and read from the display into that
	chunk of memory.


	LoadDisplayPixmapDrawable()
	LoadDisplayMaskDrawable()
	LoadDisplayPixmapMaskDrawable()

*/


PROC(
DRAWABLE LoadDisplayPixmap, (area, frame),
    ARG		(LPRECT		area)
    ARG_END	(FRAME		frame)
)
{
    DRAWABLE	buffer;

    buffer = BUILD_DRAWABLE (
	    GetFrameHandle (frame),
	    GetFrameIndex (frame)
	    );
    if (buffer || (buffer = CreateDrawable (
	    WANT_PIXMAP,
	    area->extent.width,
	    area->extent.height,
	    1))
	)
    {
	frame = CaptureDrawable (buffer);
	ReadDisplay (area, (FRAMEPTR)frame);
	ReleaseDrawable (frame);
    }

    return (buffer);
}


