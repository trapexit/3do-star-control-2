#include "starcon.h"

PROC(
void DoFMV, (name, loopname),
    ARG		(char	*name)
    ARG_END	(char	*loopname)
)
{
    VidPlay ((MEM_HANDLE)name, loopname);
    while (VidPlaying ())
	;
    VidStop ();
}

PROC(
void Introduction, (),
    ARG_VOID
)
{
    BYTE	xform_buf[1];
    STAMP	s;
    DWORD	TimeOut;
    INPUT_STATE	InputState;

    xform_buf[0] = FadeAllToBlack;
    SleepTask (XFormColorMap ((COLORMAPPTR)xform_buf, 1));
    SetSemaphore (&GraphicsSem);
    SetContext (ScreenContext);
    s.origin.x = s.origin.y = 0;
    s.frame = CaptureDrawable (LoadGraphic (TITLE_ANIM));
    DrawStamp (&s);
    DestroyDrawable (ReleaseDrawable (s.frame));
    ClearSemaphore (&GraphicsSem);

    FlushInput ();

    xform_buf[0] = FadeAllToColor;
    TimeOut = XFormColorMap ((COLORMAPPTR)xform_buf, ONE_SECOND / 2);
    LoadMasterShipList ();
    SleepTask (TimeOut);
    
    GLOBAL (CurrentActivity) |= CHECK_ABORT;
    TimeOut += ONE_SECOND * 3;
    while (!(InputState = AnyButtonPress (FALSE)) && TaskSwitch () <= TimeOut)
	;
    GLOBAL (CurrentActivity) &= ~CHECK_ABORT;
    xform_buf[0] = FadeAllToBlack;
    SleepTask (XFormColorMap ((COLORMAPPTR)xform_buf, ONE_SECOND / 2));

    if (InputState == 0)
	DoFMV ("intro", NULL);
    
    InitGameKernel ();
}

PROC(
void Victory, (),
    ARG_VOID
)
{
    BYTE	xform_buf[1];

    xform_buf[0] = FadeAllToBlack;
    SleepTask (XFormColorMap ((COLORMAPPTR)xform_buf, ONE_SECOND / 2));

    DoFMV ("victory", NULL);
	
    InitGameKernel ();
}




