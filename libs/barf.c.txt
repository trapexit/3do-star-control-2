#include <stdio.h>
#include <stdlib.h>
#include "gfxlib.h"
#include "timlib.h"
#include "sndlib.h"
#include "inplib.h"
#include "vidlib.h"
#include "declib.h"
#include "reslib.h"
#include "graphics/drawable.h"
#include "graphics/3DO/defs3do.h"

static char	**anifiles;

#define COLORMAP_FILE	"lbm/scclrtab.ct"

INPUT_REF	ArrowInput, JoystickInput;
FRAME		Screen;

#define SAFE_X	16
#define SAFE_Y	16

#define screen_width	320
#define screen_height	240

void UninitGameKernel () {}

PROC(
void CDECL main, (argc, argv),
    ARG		(int	argc)
    ARG_END	(char	*argv[])
)
{
    CONTEXT	Context;

    if (!InitGraphics (argc, argv, 0))
	return;
    
    InitSound (argc, argv);
    InitInput (SK_F1, 0, 0);

    ++argv;
    --argc;
    
    if (argc == 0)
	exit (0);

    ArrowInput = CaptureInputDevice (CreateJoystickKeyboardDevice (
	    SK_LF_ARROW, SK_RT_ARROW, SK_UP_ARROW, SK_DN_ARROW,
	    '\n', ' '
	    ));
    JoystickInput = CaptureInputDevice (CreateJoystickDevice (0));

    if (Context = CaptureContext (CreateContext ()))
    {
	SIZE	width, height;
	COUNT	num_frames;
	STAMP	s;
	char	*anif, *cmf;
	int	i;
	BOOLEAN	render_all;
	int	page_ct;

	page_ct = 0;	
	{
	    render_all = FALSE;
	    cmf = COLORMAP_FILE;
	}
	
	s.frame = 0;
	SetContext (Context);

	if (Screen = CaptureDrawable (
		CreateDisplay (WANT_PIXMAP /* | WANT_MASK */,
		&width, &height)
		))
	{
	
	    COLORMAP_REF	ColorRef;

	    if (ColorRef = LoadColorMapFile (cmf))
	    {
		COLORMAP	CM;

		CM = CaptureColorMap (ColorRef);
		SetColorMap (GetColorMapAddress (CM));
		ReleaseColorMap (CM);
		DestroyColorMap (ColorRef);
	    }

NextFile:

	if (render_all)
	    anif = anifiles[i];
	else
	    anif = *argv;
	    
printf ("Showing ani '%s'\n", anif);

	if (s.frame = CaptureDrawable (
		LoadGraphic ((int)anif)
		))
	{
	    SIZE		h;

	    num_frames = GetFrameCount (s.frame);

#undef VIEW_WIDTH
#undef VIEW_HEIGHT
#define VIEW_WIDTH	292
#define VIEW_HEIGHT	216
#define XOFF		10
#define YOFF		10
	    SetContextFGFrame (Screen);
	    SetFrameHot (Screen, MAKE_HOT_SPOT (
		    (VIEW_WIDTH - width) / 2, (VIEW_HEIGHT - height) / 2
		    ));

	    SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x1f, 0x1f, 0x1f), 0x01));
	    SetContextBackGroundColor (BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x00), 0xFF));

	    h = 0;
	    s.origin.x = XOFF;
	    s.origin.y = YOFF;

	    BatchGraphics ();
ScreenOrigin (Screen, 0, 0);
	    ClearDrawable ();
	    do
	    {
		RECT	r;

		r.corner.x = r.corner.y = 0;
		GetFrameRect (s.frame, &r);
{
HOT_SPOT	hot;

printf ("frame %ld: %ld, %ld -- ", GetFrameCount (s.frame) - num_frames, r.corner.x, r.corner.y);
printf ("%ld,%ld  ", r.extent.width, r.extent.height); 
hot = GetFrameHot (s.frame);
printf ("hot is %d,%d\n", GET_HOT_X (hot), GET_HOT_Y (hot));
}
		SetFrameHot (s.frame, MAKE_HOT_SPOT (0, 0));
		r.corner.x = s.origin.x - 1;
		r.corner.y = s.origin.y - 1;
		r.extent.width += 2;
		r.extent.height += 2;
		if (r.corner.x + r.extent.width > VIEW_WIDTH)
		{
		    s.origin.x = XOFF;
		    s.origin.y += h - 1;
		    if (s.origin.y + r.extent.height > VIEW_HEIGHT)
		    {
			UnbatchGraphics ();
			FlushInput ();
			while (!AnyButtonPress (TRUE))
			    TaskSwitch ();
			while (AnyButtonPress (TRUE))
			    TaskSwitch ();
			s.origin.x = s.origin.y = 10;//-16;
			BatchGraphics ();
ScreenOrigin (Screen, 0, 0);
			ClearDrawable ();
		    }
		    r.corner.x = s.origin.x - 1;
		    r.corner.y = s.origin.y - 1;
		    h = 0;
		}
		DrawRectangle (&r);
		DrawStamp (&s);
		s.origin.x += r.extent.width - 1;
		if (r.extent.height > h)
		    h = r.extent.height;
		s.frame = IncFrameIndex (s.frame);
	    } while (--num_frames);

ScreenTransition (3, 0);
	    UnbatchGraphics ();
#if 1
{
RECT	r;

r.corner.x = r.corner.y = 50;
r.extent.width = r.extent.height = 100;
while (AnyButtonPress (TRUE));
while (!AnyButtonPress (TRUE));
BatchGraphics ();
ScreenTransition (3, 0);
UnbatchGraphics ();
ScreenTransition (3, &r);
#if 0
while (AnyButtonPress (TRUE));
while (!AnyButtonPress (TRUE));
BatchGraphics ();
ScreenTransition (2, 0);
UnbatchGraphics ();
ScreenTransition (2, 0);
while (AnyButtonPress (TRUE));
while (!AnyButtonPress (TRUE));
#endif
BatchGraphics ();
ScreenTransition (4, 0);
UnbatchGraphics ();
ScreenTransition (4, &r);
while (AnyButtonPress (TRUE));
while (!AnyButtonPress (TRUE));
}
#endif
	    FlushInput ();

	    while (!AnyButtonPress (TRUE))
		TaskSwitch ();

	    DestroyDrawable (ReleaseDrawable (s.frame));
	}
	
	if (render_all)
	{
	    if (anifiles[++i])
		goto NextFile;
	}
	else
	{
	    ++argv;
	    --argc;
	    if (argc)
		goto NextFile;
	}
	}

	DestroyDrawable (ReleaseDrawable (Screen));

	DestroyContext (ReleaseContext (Context));
    }
    
    DestroyInputDevice (ReleaseInputDevice (JoystickInput));
    DestroyInputDevice (ReleaseInputDevice (ArrowInput));

    UninitInput ();
    UninitSound ();
    UninitGraphics ();

    exit (0);
}
