#include "starcon.h"
#include "coderes.h"

ACTIVITY	LastActivity;
BYTE		PlayerControl[NUM_PLAYERS];

MEM_HANDLE	hResIndex;
CONTEXT		ScreenContext, SpaceContext, StatusContext, OffScreenContext,
		TaskContext;
SIZE		screen_width, screen_height;
FRAME		Screen;
FONT		StarConFont, MicroFont, TinyFont;
INPUT_REF	ComputerInput, NormalInput,
		JoystickInput[NUM_PLAYERS],
		PlayerInput[NUM_PLAYERS];
QUEUE		race_q[NUM_PLAYERS];
SOUND		MenuSounds, GameSounds;
FRAME		ActivityFrame, status, flagship_status, misc_data;
SEMAPHORE	GraphicsSem;

PROC(
void InitJoySticks, (),
    ARG_VOID
)
{
    INPUT_DEVICE	InputDevice[NUM_PLAYERS];
    PROC_GLOBAL(
    BOOLEAN PauseGame, (),
	ARG_VOID
    );

    InitInput (SK_F1, SK_F10, PauseGame);

    InputDevice[1] = CreateJoystickDevice (1);
    JoystickInput[0] = CaptureInputDevice (InputDevice[1]);
    JoystickInput[1] = CaptureInputDevice (InputDevice[1]);
    if (InputDevice[0] = CreateJoystickDevice (0))
    {
	ReleaseInputDevice (JoystickInput[0]);

	JoystickInput[0] = CaptureInputDevice (InputDevice[0]);
	if (JoystickInput[1] == NULL_PTR)
	    JoystickInput[1] = CaptureInputDevice (InputDevice[0]);
    }
}

PROC(
BOOLEAN LoadKernel, (argc, argv),
    ARG		(int	argc)
    ARG_END	(char	*argv[])
)
{
    INPUT_DEVICE	InputDevice[NUM_PLAYERS];

#define MIN_K_REQUIRED	(580000L / 1024)
    if (!InitGraphics (argc, argv, MIN_K_REQUIRED))
	return (FALSE);
    InitSound (argc, argv);
    InitVideo (TRUE);
    if ((ScreenContext = CaptureContext (
	    CreateContext ()
	    )) == 0
	    || (Screen = CaptureDrawable (
	    CreateDisplay (WANT_MASK | WANT_PIXMAP,
	    &screen_width, &screen_height)
	    )) == 0)
	return (FALSE);
    SetContext (ScreenContext);
    SetContextFGFrame (Screen);
    SetFrameHot (Screen, MAKE_HOT_SPOT (0, 0));

    hResIndex = InitResourceSystem (
	    "starcon", RES_INDEX,
#ifdef FROM_FLOPPY
	    StarConDiskError
#else /* !FROM_FLOPPY */
	    NULL_PTR
#endif /* FROM_FLOPPY */
	    );
    INIT_INSTANCES ();

    {
	COLORMAP	ColorMapTab;

	ColorMapTab = CaptureColorMap (LoadColorMap (STARCON_COLOR_MAP));
	SetColorMap (GetColorMapAddress (ColorMapTab));
	DestroyColorMap (ReleaseColorMap (ColorMapTab));
    }

    InitJoySticks ();

    InputDevice[0] = CreateInternalDevice (game_input);
    NormalInput = CaptureInputDevice (InputDevice[0]);
    InputDevice[0] = CreateInternalDevice (computer_intelligence);
    ComputerInput = CaptureInputDevice (InputDevice[0]);

#if DEMO_MODE
    InputDevice[0] = CreateInternalDevice (demo_input);
    DemoInput = CaptureInputDevice (InputDevice[0]);
#endif /* DEMO_MODE */

    GLOBAL (CurrentActivity) = (ACTIVITY)~0;
    return (TRUE);
}

PROC(
BOOLEAN InitContexts, (),
    ARG_VOID
)
{
    RECT	r;
    if ((StatusContext = CaptureContext (
	    CreateContext ()
	    )) == 0)
	return (FALSE);

    SetContext (StatusContext);
    SetContextFGFrame (Screen);
    r.corner.x = SPACE_WIDTH + SAFE_X;
    r.corner.y = SAFE_Y;
    r.extent.width = STATUS_WIDTH;
    r.extent.height = STATUS_HEIGHT;
    SetContextClipRect (&r);
    
    if ((SpaceContext = CaptureContext (
	    CreateContext ()
	    )) == 0)
	return (FALSE);
	
    if ((OffScreenContext = CaptureContext (
	    CreateContext ()
	    )) == 0)
	return (FALSE);

    if (!InitQueue (&disp_q, 100, sizeof (ELEMENT)))
	return (FALSE);

    return (TRUE);
}

PROC(
BOOLEAN InitKernel, (),
    ARG_VOID
)
{
    COUNT	counter;

    for (counter = 0; counter < NUM_PLAYERS; ++counter)
    {
	InitQueue (&race_q[counter], MAX_SHIPS_PER_SIDE, sizeof (STARSHIP));
    }

    if ((StarConFont = CaptureFont (
	    LoadGraphic (STARCON_FONT)
	    )) == 0)
	return (FALSE);

    if ((TinyFont = CaptureFont (
	    LoadGraphic (TINY_FONT)
	    )) == 0)
	return (FALSE);
    if ((ActivityFrame = CaptureDrawable (
	    LoadGraphic (ACTIVITY_ANIM)
	    )) == 0)
	return (FALSE);

    if ((status = CaptureDrawable (
	    LoadGraphic (STATUS_MASK_PMAP_ANIM)
	    )) == 0)
	return (FALSE);

    if ((MicroFont = CaptureFont (
	    LoadGraphic (MICRO_FONT)
	    )) == 0)
	return (FALSE);

    if ((MenuSounds = CaptureSound (
	    LoadSound (MENU_SOUNDS)
	    )) == 0)
	return (FALSE);

    InitSpace ();

    return (TRUE);
}

PROC(
BOOLEAN InitGameKernel, (),
    ARG_VOID
)
{
    if (ActivityFrame == 0)
    {
	InitKernel ();
	InitContexts ();
    }
}

PROC(
void SetPlayerInput, (),
    ARG_VOID
)
{
    COUNT	which_player;

    for (which_player = 0; which_player < NUM_PLAYERS; ++which_player)
    {
	if (!(PlayerControl[which_player] & HUMAN_CONTROL))
	    PlayerInput[which_player] = ComputerInput;
	else if (LOBYTE (GLOBAL (CurrentActivity)) != SUPER_MELEE)
	{
	    if (which_player == 0)
		PlayerInput[which_player] = NormalInput;
	    else
	    {
		PlayerInput[which_player] = ComputerInput;
		PlayerControl[which_player] =
			COMPUTER_CONTROL | AWESOME_RATING | JOYSTICK_CONTROL;
	    }
	}
	else if (PlayerControl[1 - which_player] & HUMAN_CONTROL)
	    PlayerInput[which_player] = JoystickInput[which_player];
	else
	    PlayerInput[which_player] = NormalInput;
    }
}

