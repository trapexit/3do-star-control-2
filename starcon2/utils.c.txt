#include "starcon.h"
#include "commglue.h"

PROC(
void DrawStarConBox,
	(pRect, BorderWidth, TopLeftColor, BottomRightColor,
	FillInterior, InteriorColor),
    ARG		(PRECT		pRect)
    ARG		(SIZE		BorderWidth)
    ARG		(COLOR		TopLeftColor)
    ARG		(COLOR		BottomRightColor)
    ARG		(BOOLEAN	FillInterior)
    ARG_END	(COLOR		InteriorColor)
)
{
    RECT	locRect;

    if (BorderWidth == 0)
	BorderWidth = 2;
    else
    {
	SetContextForeGroundColor (TopLeftColor);
	locRect.corner = pRect->corner;
	locRect.extent.width = pRect->extent.width;
	locRect.extent.height = 1;
	DrawFilledRectangle (&locRect);
	if (BorderWidth == 2)
	{
	    ++locRect.corner.x;
	    ++locRect.corner.y;
	    locRect.extent.width -= 2;
	    DrawFilledRectangle (&locRect);
	}

	locRect.corner = pRect->corner;
	locRect.extent.width = 1;
	locRect.extent.height = pRect->extent.height;
	DrawFilledRectangle (&locRect);
	if (BorderWidth == 2)
	{
	    ++locRect.corner.x;
	    ++locRect.corner.y;
	    locRect.extent.height -= 2;
	    DrawFilledRectangle (&locRect);
	}

	SetContextForeGroundColor (BottomRightColor);
	locRect.corner.x = pRect->corner.x + pRect->extent.width - 1;
	locRect.corner.y = pRect->corner.y + 1;
	locRect.extent.height = pRect->extent.height - 1;
	DrawFilledRectangle (&locRect);
	if (BorderWidth == 2)
	{
	    --locRect.corner.x;
	    ++locRect.corner.y;
	    locRect.extent.height -= 2;
	    DrawFilledRectangle (&locRect);
	}

	locRect.corner.x = pRect->corner.x;
	locRect.extent.width = pRect->extent.width;
	locRect.corner.y = pRect->corner.y + pRect->extent.height - 1;
	locRect.extent.height = 1;
	DrawFilledRectangle (&locRect);
	if (BorderWidth == 2)
	{
	    ++locRect.corner.x;
	    --locRect.corner.y;
	    locRect.extent.width -= 2;
	    DrawFilledRectangle (&locRect);
	}
    }

    if (FillInterior)
    {
	SetContextForeGroundColor (InteriorColor);
	locRect.corner.x = pRect->corner.x + BorderWidth;
	locRect.corner.y = pRect->corner.y + BorderWidth;
	locRect.extent.width = pRect->extent.width - (BorderWidth << 1);
	locRect.extent.height = pRect->extent.height - (BorderWidth << 1);
	DrawFilledRectangle (&locRect);
    }
}

PROC(
DWORD SeedRandomNumbers, (),
    ARG_VOID
)
{
    DWORD	cur_time;

    seed_random (cur_time = GetTimeCounter ());

    return (cur_time);
}

PROC(
void WaitForNoInput, (Duration),
    ARG_END	(SIZE	Duration)
)
{
    INPUT_STATE	PressState;

    PressState = AnyButtonPress (FALSE);
    if (Duration < 0)
    {
	if (PressState)
	    return;
	Duration = -Duration;
    }
    else if (!PressState)
	return;
    
    {
	DWORD		TimeOut;
	INPUT_STATE	ButtonState;

	TimeOut = GetTimeCounter () + Duration;
	do
	{
	    ButtonState = AnyButtonPress (FALSE);
	    if (PressState)
	    {
		PressState = ButtonState;
		ButtonState = 0;
	    }
	} while (!ButtonState && TaskSwitch () <= TimeOut);
    }
}

PROC(
BOOLEAN PauseGame, (),
    ARG_VOID
)
{
    RECT	r;
    STAMP	s;
    BOOLEAN	ClockActive;
    CONTEXT	OldContext;
    FRAME	F;
    HOT_SPOT	OldHot;

    if (ActivityFrame == 0
	    || (GLOBAL (CurrentActivity) & (CHECK_ABORT | CHECK_PAUSE))
	    || (LastActivity & (CHECK_LOAD | CHECK_RESTART)))
	return (FALSE);
	
    GLOBAL (CurrentActivity) |= CHECK_PAUSE;

    if (ClockActive = (BOOLEAN)(
	    LOBYTE (GLOBAL (CurrentActivity)) != SUPER_MELEE
	    && GameClockRunning ()
	    ))
	SuspendGameClock ();
    else if (CommData.PlayerPhrases && PlayingTrack ())
	PauseTrack ();

    SetSemaphore (&GraphicsSem);
    OldContext = SetContext (ScreenContext);
    OldHot = SetFrameHot (Screen, MAKE_HOT_SPOT (0, 0));

    GetFrameRect (ActivityFrame, &r);
    r.corner.x = (SCREEN_WIDTH - r.extent.width) >> 1;
    r.corner.y = (SCREEN_HEIGHT - r.extent.height) >> 1;
    s.origin = r.corner;
    s.frame = ActivityFrame;
    F = CaptureDrawable (LoadDisplayPixmap (&r, (FRAME)0));
    DrawStamp (&s);

    {
	BYTE	scan;

	scan = ASCIIToScan (SK_F1);
	while (KeyDown (scan))
	    TaskSwitch ();
    }

    FlushInput ();
    while (KeyHit () != SK_F1)
	TaskSwitch ();

    s.frame = F;
    DrawStamp (&s);
    DestroyDrawable (ReleaseDrawable (s.frame));

    SetFrameHot (Screen, OldHot);
    SetContext (OldContext);

    WaitForNoInput (ONE_SECOND / 4);
    FlushInput ();
    ClearSemaphore (&GraphicsSem);

    if (ClockActive)
	ResumeGameClock ();
    else if (CommData.PlayerPhrases && PlayingTrack ())
	ResumeTrack ();

    TaskSwitch ();
    GLOBAL (CurrentActivity) &= ~CHECK_PAUSE;
    return (TRUE);
}

#ifdef FROM_FLOPPY

PROC(
BOOLEAN	StarConDiskError, (pFileName),
    ARG_END	(PSTR	pFileName)
)
{
    ReportDiskError (pFileName, PACKAGE_ERROR);

    return (FALSE);
}

PROC(
void ReportDiskError, (pFileName, ErrorCondition),
    ARG		(PSTR		pFileName)
    ARG_END	(DISK_ERROR	ErrorCondition)
)
{
    RECT	r;
    STAMP	s;
    TEXT	Text;
    DRAWABLE	Drawable;
    CONTEXT	OldContext;
    DRAW_STATE	OldDrawState;
    COLOR	OldColor;
    FONT	OldFont;

    r.corner.x = 78;
    r.corner.y = 62;
    r.extent.width = 168;
    r.extent.height = 51;

    OldContext = SetContext (ScreenContext);
    Drawable = LoadDisplayPixmap (&r, NULL_PTR);

    OldDrawState = SetContextDrawState (DEST_PIXMAP | DRAW_REPLACE);
    OldColor = SetContextForeGroundColor (BLACK_COLOR);
    DrawStarConBox (&r, 2,
	    BUILD_COLOR (MAKE_RGB15 (0x14, 0x00, 0x00), 0x04),
	    WHITE_COLOR,
	    TRUE,
	    BUILD_COLOR (MAKE_RGB15 (0x1F, 0xA, 0xA), 0x0C));

    Text.align = ALIGN_CENTER;
    Text.baseline.x = SCREEN_WIDTH >> 1;

    SetContextForeGroundColor (WHITE_COLOR);
    OldFont = SetContextFont (StarConFont);
    switch (ErrorCondition)
    {
	case PACKAGE_ERROR:
	    Text.baseline.y = 80;
	    Text.lpStr = "Please insert Disk 2";
	    Text.CharCount = strlen ((char *)Text.lpStr);
	    DrawText (&Text);

	    Text.baseline.y += 11;
	    Text.lpStr = "In any drive";
	    Text.CharCount = strlen ((char *)Text.lpStr);
	    DrawText (&Text);

	    Text.baseline.y += 11;
	    Text.lpStr = "Then press any key";
	    Text.CharCount = strlen ((char *)Text.lpStr);
	    DrawText (&Text);
	    break;
	case LOAD_ERROR:
	case SAVE_ERROR:
	    Text.baseline.y = 74;
	    Text.lpStr = "DISK ERROR";
	    Text.CharCount = strlen ((char *)Text.lpStr);
	    DrawText (&Text);

	    Text.baseline.y += 11;
	    Text.lpStr = "Check disk and try again";
	    Text.CharCount = strlen ((char *)Text.lpStr);
	    DrawText (&Text);

	    Text.baseline.y += 11;
	    Text.lpStr = "or refer to manual";
	    Text.CharCount = strlen ((char *)Text.lpStr);
	    DrawText (&Text);

	    SetContextForeGroundColor (BLACK_COLOR);
	    Text.baseline.y += 11;
	    Text.lpStr = "Press any key";
	    Text.CharCount = strlen ((char *)Text.lpStr);
	    DrawText (&Text);
	    break;
    }
    SetContextFont (OldFont);

    WaitButtonPress (TRUE);

    s.origin = r.corner;
    s.frame = CaptureDrawable (Drawable);
    DrawStamp (&s);
    ReleaseDrawable (s.frame);

    DestroyDrawable (Drawable);

    SetContextForeGroundColor (OldColor);
    SetContextDrawState (OldDrawState);
    SetContext (OldContext);
}

#endif /* FROM_FLOPPY */


