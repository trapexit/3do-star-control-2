#include "starcon.h"

PROC(
void Credits, (),
    ARG_VOID
)
{
#define NUM_CREDITS	20
    COUNT		i;
    RES_TYPE		rt;
    RES_INSTANCE	ri;
    RES_PACKAGE		rp;
    RECT		r;
    FRAME		f[NUM_CREDITS];
    STAMP		s;
    DWORD		TimeIn;
    MUSIC_REF		hMusic; 
    BYTE	fade_buf[1];
    
FreeSC2Data ();
FreeLanderData ();
FreeIPData ();
FreeHyperData ();
    rt = GET_TYPE (CREDIT00_ANIM);
    ri = GET_INSTANCE (CREDIT00_ANIM);
    rp = GET_PACKAGE (CREDIT00_ANIM);
    for (i = 0; i < NUM_CREDITS; ++i, ++rp, ++ri)
    {
	f[i] = CaptureDrawable (LoadGraphic (
		MAKE_RESOURCE (rp, rt, ri)
		));
    }
    
    if (hMusic = LoadMusicInstance (CREDITS_MUSIC))
	PlayMusic (hMusic, TRUE, 1);

    SetSemaphore (&GraphicsSem);
    SetContext (ScreenContext);
    GetContextClipRect (&r);
    s.origin.x = s.origin.y = 0;
    
    BatchGraphics ();
    FindDisplayPage (Screen, 0, 0);
    s.frame = f[0];
    DrawStamp (&s);
    ScreenOrigin (Screen, 0, 0);
    UnbatchGraphics ();
    
    fade_buf[0] = FadeAllToColor;
    SleepTask (XFormColorMap ((COLORMAPPTR)fade_buf, ONE_SECOND / 2));
    
    TimeIn = GetTimeCounter ();
    for (i = 1; i < NUM_CREDITS; ++i)
    {
	BatchGraphics ();
	FindDisplayPage (Screen, 0, 0);
	s.frame = f[0];
	DrawStamp (&s);
	s.frame = f[i];
	DrawStamp (&s);
	ScreenOrigin (Screen, 0, 0);
	ScreenTransition (3, &r);
	UnbatchGraphics ();
	DestroyDrawable (ReleaseDrawable (f[i]));
	
	TimeIn = SleepTask (TimeIn + ONE_SECOND * 5);
    }
    
    DestroyDrawable (ReleaseDrawable (f[0]));
    ClearSemaphore (&GraphicsSem);

    while (!AnyButtonPress (FALSE));
    while (AnyButtonPress (FALSE));
    
    fade_buf[0] = FadeAllToBlack;
    SleepTask (XFormColorMap ((COLORMAPPTR)fade_buf, ONE_SECOND / 2));
    FlushColorXForms ();
    
    if (hMusic)
    {
	StopMusic ();
	DestroyMusic (hMusic);
    }
}


