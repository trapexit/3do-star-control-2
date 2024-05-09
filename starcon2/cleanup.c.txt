#include "starcon.h"

PROC(
void UninitJoySticks, (),
    ARG_VOID
)
{
    COUNT	which_player;

    for (which_player = 0; which_player < NUM_PLAYERS; ++which_player)
	DestroyInputDevice (ReleaseInputDevice (JoystickInput[which_player]));
    UninitInput ();
}

PROC(
void FreeKernel, (),
    ARG_VOID
)
{
    UninitKernel (TRUE);
    UninitContexts ();

    UNINIT_INSTANCES ();
    UninitResourceSystem ();

#if DEMO_MODE
    DestroyInputDevice (ReleaseInputDevice (DemoInput));
#endif /* DEMO_MODE */
    DestroyInputDevice (ReleaseInputDevice (ComputerInput));
    DestroyInputDevice (ReleaseInputDevice (NormalInput));

    UninitJoySticks ();

    DestroyDrawable (ReleaseDrawable (Screen));
    DestroyContext (ReleaseContext (ScreenContext));

    UninitVideo ();
    UninitSound ();
    UninitGraphics ();
}

PROC(
void UninitContexts, (),
    ARG_VOID
)
{
    UninitQueue (&disp_q);

    DestroyContext (ReleaseContext (OffScreenContext));
    DestroyContext (ReleaseContext (SpaceContext));
    DestroyContext (ReleaseContext (StatusContext));
}

PROC(
void UninitKernel, (ships),
    ARG_END	(BOOLEAN	ships)
)
{
    extern FRAME	status;

    UninitSpace ();

    DestroySound (ReleaseSound (MenuSounds));
    DestroyFont (ReleaseFont (MicroFont));
    DestroyDrawable (ReleaseDrawable (status));
    DestroyDrawable (ReleaseDrawable (ActivityFrame));
    DestroyFont (ReleaseFont (TinyFont));
    DestroyFont (ReleaseFont (StarConFont));

    UninitQueue (&race_q[0]);
    UninitQueue (&race_q[1]);

    if (ships)
	FreeMasterShipList ();
    
    ActivityFrame = 0;
}

void UninitGameKernel ()
{
    if (ActivityFrame)
    {
	FreeSC2Data ();
	FreeLanderData ();
	FreeIPData ();
	FreeHyperData ();

	UninitKernel (FALSE);
	UninitContexts ();
    }
}

