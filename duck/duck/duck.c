#include "DuckTypes.h"
#include "Portfolio.h"
#include "Init3DO.h"				// setupScreen
#include "hardware.h"
#include "mem.h"
#include "DuckPlayer.h"
#include "audio.h"
#include "event.h"

#define CD_BASED	"/cd-rom"
#define MAC_BASED	"$boot"
#define BASE_DIR	MAC_BASED

#define AUDIO_22k	23
#define AUDIO_44k	44
#define AUDIO_RATE	AUDIO_22k

#define ALL_FRAMES	0
#define FRAME_COUNT	ALL_FRAMES

//#define DEBUG

#define NUM_CONTROLLERS		2L

static uint32
ButtonPress ()
{
    int32		i;
    ControlPadEventData	cp;

    for (i = 1; i <= NUM_CONTROLLERS; i++)
	if (GetControlPad (i, 0, &cp) > 0 && cp.cped_ButtonBits)
	    return (1UL);

    return (0UL);
}

int
main (int argc, char **argv)
{
    extern ScreenContext	TheScreen;

    if (ChangeDirectory (BASE_DIR) < 0
	    || OpenMathFolio ()
	    || OpenAudioFolio ()
	    || InitEventUtility (NUM_CONTROLLERS, 0L, 0L))
	goto DuckDone;
	
    TheScreen.sc_nScreens = 4;
    if (OpenGraphics (&TheScreen, 4))
    {
 	if (OpenSPORT ())
	{
	    player	plr;
#ifdef DEBUG
extern uint32	gdbg;
	    
gdbg = 1;

#endif /* DEBUG */
	    if (playerOpen (&plr, *(argv + 1), AUDIO_RATE))
	    {
		playerStart (&plr, 0);
		while (playerDo (&plr, ButtonPress () ? JOYFIREC : 0))
		    ;
		playerStop ();
		playerClose (&plr);
	    }
	}
    }
    
DuckDone:
    KillEventUtility ();
    CloseAudioFolio ();
    
    exit (0);
}


