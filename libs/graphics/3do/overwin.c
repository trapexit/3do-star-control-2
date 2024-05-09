#include "gfxintrn.h"
#include "timlib.h"
//#include "vga.h"
#include "defs3do.h"

#define OVERLAY_DRAWABLE	(3 << FTYPE_SHIFT)

PROC(
DRAWABLE CreateOverlayWindow, (CreateFlags, Display, height),
    ARG		(CREATE_FLAGS	CreateFlags)
    ARG		(FRAMEPTR	Display)
    ARG_END	(SIZE		height)
)
{
    DRAWABLE	Drawable;

    if (!DisplayActive ())
	return (0);

/* if (0) */
    if (Drawable = _request_drawable (
	    1, OVERLAY_DRAWABLE,
	    CreateFlags & (GetDisplayFlags () & WANT_MASK),
	    GetDisplayWidth (),
	    height
	    ))
    {
	FRAME	F;

	if ((F = CaptureDrawable (Drawable)) == 0)
	    DestroyDrawable (Drawable);
	else
	{
// 	    ThreeDOCreateOverlayWin ();
//	    AddFrameFlags (F, OVERLAY_WIN);
#ifdef NOTYET
	    _overlay_off = height * SCREEN_WIDTH;
	    _screen_phys_off += _overlay_off;
	    _screen_log_off += _overlay_off;
	    Display->DataOffs += _overlay_off;

	    FindDisplayPage (Display, 0, 0);
	    ScreenOrigin (Display, 0, 0);
	    _vga_batch_flags |= VERT_SYNC;
	    _screen_origin ();
#endif /* NOTYET */

	    ReleaseDrawable (F);
	    return (Drawable);
	}
    }

    return (0);
}

PROC(
void ShowOverlayWin, (WinFrame, TimeInterval),
    ARG		(FRAMEPTR	WinFrame)
    ARG_END	(COUNT		TimeInterval)
)
{
    if (WinFrame && TYPE_GET (WinFrame->TypeIndexAndFlags) == OVERLAY_DRAWABLE)
    {
	COUNT	h, TDelta;
	DWORD	StartTime, CurTime;

	h = GetFrameHeight (WinFrame);

	if (TimeInterval == 0)
	    TimeInterval = 1;

	TDelta = 0;
	StartTime = TaskSwitch ();
	CurTime = StartTime + 1;
	do
	{
	    if ((TDelta += (COUNT)(CurTime - StartTime)) > TimeInterval)
		TDelta = TimeInterval;
	    StartTime = CurTime;

	    _split_screen (SCREEN_HEIGHT - h * TDelta / TimeInterval);
	    CurTime = SleepTask (StartTime + 2);
	} while (TimeInterval > TDelta);
    }
}

PROC(
void HideOverlayWin, (WinFrame, TimeInterval),
    ARG		(FRAMEPTR	WinFrame)
    ARG_END	(COUNT		TimeInterval)
)
{
    if (WinFrame && TYPE_GET (WinFrame->TypeIndexAndFlags) == OVERLAY_DRAWABLE)
    {
	COUNT	h, height, TDelta;
	DWORD	StartTime, CurTime;

	h = height = GetFrameHeight (WinFrame);

	if (TimeInterval == 0)
	    TimeInterval = 1;

	TDelta = 0;
	StartTime = TaskSwitch ();
	CurTime = StartTime + 1;
	do
	{
	    if ((TDelta += (COUNT)(CurTime - StartTime)) > TimeInterval)
		TDelta = TimeInterval;
	    StartTime = CurTime;

	    _split_screen (SCREEN_HEIGHT - (height - h * TDelta / TimeInterval));
	    CurTime = SleepTask (StartTime + 2);
	} while (TimeInterval > TDelta);

	_split_screen (0x3FFF);
    }
}

PROC(
BOOLEAN DestroyOverlayWindow, (Display, Drawable),
    ARG		(FRAME		Display)
    ARG_END	(DRAWABLE	Drawable)
)
{
    if (Display)
    {
#ifdef NOTYET
	_screen_phys_off -= _overlay_off;
	_screen_log_off -= _overlay_off;
	Display->DataOffs -= _overlay_off;
	_overlay_off = 0;

	_split_screen (0x3FFF);
#endif /* NOTYET */
    }

    return (DestroyDrawable (Drawable));
}

