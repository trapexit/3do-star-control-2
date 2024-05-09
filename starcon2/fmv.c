#include "starcon.h"

PROC(STATIC
void DoFMV, (name, loopname, uninit),
    ARG		(char		*name)
    ARG		(char		*loopname)
    ARG_END	(BOOLEAN	uninit)
)
{
    VidPlay ((MEM_HANDLE)name, loopname, uninit);
    while (VidPlaying ())
	;
    VidStop ();
}

PROC(
void DoShipSpin, (index, hMusic),
    ARG		(COUNT		index)
    ARG_END	(MUSIC_REF	hMusic)
)
{
    char	buf[30];
    BYTE	clut_buf[1];
    RECT	old_r, r;

    SetGraphicUseOtherExtra (1);
    LoadIntoExtraScreen (0);
    clut_buf[0] = FadeAllToBlack;
    SleepTask (XFormColorMap ((COLORMAPPTR)clut_buf, ONE_SECOND / 4));
    FlushColorXForms ();
    
    if (hMusic)
	StopMusic ();

    FreeHyperData ();
    
    sprintf (buf, "ship%02d", index);
    DoFMV (buf, "spin", FALSE);

    GetContextClipRect (&old_r);
    r.corner.x = r.corner.y = 0;
    r.extent.width = SCREEN_WIDTH;
    r.extent.height = SCREEN_HEIGHT;
    SetContextClipRect (&r);
    DrawFromExtraScreen (0, 0);
    SetGraphicUseOtherExtra (0);
    SetContextClipRect (&old_r);

    if (hMusic)
	PlayMusic (hMusic, TRUE, 1);
	
    clut_buf[0] = FadeAllToColor;
    SleepTask (XFormColorMap ((COLORMAPPTR)clut_buf, ONE_SECOND / 4));
    FlushColorXForms ();
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
    {
	static BOOLEAN	play_intro = TRUE;
	
	DoFMV (play_intro ? "intro" : "drumall", NULL, TRUE);
	play_intro = !play_intro;
    }
    
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

    DoFMV ("victory", NULL, TRUE);
	
    xform_buf[0] = FadeAllToBlack;
    XFormColorMap ((COLORMAPPTR)xform_buf, 0);
    
    InitGameKernel ();
}

PROC(
void Logo, (),
    ARG_VOID
)
{
    DoFMV ("logo", NULL, FALSE);
}




