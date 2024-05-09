#include "starcon.h"
#include "commglue.h"

PROC(
INPUT_STATE ConfirmExit, (),
    ARG_VOID
)
{
    INPUT_STATE	InputState;
    BOOLEAN	ClockActive;

    if (ClockActive = (BOOLEAN)(
	    LOBYTE (GLOBAL (CurrentActivity)) != SUPER_MELEE
	    && GameClockRunning ()
	    ))
	SuspendGameClock ();
    else if (CommData.PlayerPhrases && PlayingTrack ())
	PauseTrack ();

    SetSemaphore (&GraphicsSem);
    if (GLOBAL (CurrentActivity) & CHECK_ABORT)
	InputState = DEVICE_EXIT;
    else
    {
	RECT		r;
	STAMP		s;
	FRAME		F;
	CONTEXT		oldContext;
	DRAW_STATE	oldDrawState;

	oldContext = SetContext (ScreenContext);
	oldDrawState = SetContextDrawState (DEST_PIXMAP | DRAW_REPLACE);

	s.frame = SetAbsFrameIndex (ActivityFrame, 1);
	GetFrameRect (s.frame, &r);
	r.corner.x = (SCREEN_WIDTH - r.extent.width) >> 1;
	r.corner.y = (SCREEN_HEIGHT - r.extent.height) >> 1;
	s.origin = r.corner;
	F = CaptureDrawable (LoadDisplayPixmap (&r, (FRAME)0));
	DrawStamp (&s);

	{
	    INPUT_STATE	PressState;

	    GLOBAL (CurrentActivity) |= CHECK_ABORT;

	    PressState = GetInputState (NormalInput);
	    do
	    {
	        TaskSwitch ();
	        InputState = GetInputState (NormalInput);
		if (PressState)
		{
		    PressState = InputState;
		    InputState = 0;
		}
	    } while (!InputState);
	}

	s.frame = F;
	DrawStamp (&s);
	DestroyDrawable (ReleaseDrawable (s.frame));

	if (InputState & DEVICE_BUTTON2)
	    InputState = DEVICE_EXIT;
	else
	{
	    InputState = 0;
	    GLOBAL (CurrentActivity) &= ~CHECK_ABORT;
	    WaitForNoInput (ONE_SECOND / 4);
	    FlushInput ();
	}

	SetContextDrawState (oldDrawState);
	SetContext (oldContext);
    }
    ClearSemaphore (&GraphicsSem);

    if (ClockActive)
	ResumeGameClock ();
    else if (CommData.PlayerPhrases && PlayingTrack ())
	ResumeTrack ();

    return (InputState);
}


