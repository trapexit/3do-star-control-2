#include "starcon.h"

SIZE	cur_player;

PROC(
INPUT_STATE computer_intelligence, (InputRef, InputState),
    ARG		(INPUT_REF	InputRef)
    ARG_END	(INPUT_STATE	InputState)
)
{
    if (InputState || LOBYTE (GLOBAL (CurrentActivity)) == IN_LAST_BATTLE)
	return (InputState);

    if (CyborgDescPtr)
    {
	if (PlayerControl[cur_player] & CYBORG_CONTROL)
	    InputState = tactical_intelligence ();
	else
	    InputState = GetInputState (JoystickInput[cur_player]);
    }
    else if (!(PlayerControl[cur_player] & PSYTRON_CONTROL))
	InputState = 0;
    else
    {
	switch (LOBYTE (GLOBAL (CurrentActivity)))
	{
	    case SUPER_MELEE:
	    {
		SleepTask (GetTimeCounter () + (ONE_SECOND >> 1));
		InputState = DEVICE_BUTTON1;	/* pick a random ship */
		break;
	    }
	}
    }

    return (InputState);
}

