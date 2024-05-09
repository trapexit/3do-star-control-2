#include "starcon.h"

typedef struct
{
    PROC_PARAMETER(
    BOOLEAN (*InputFunc), (InputState, pInputState),
	ARG	(INPUT_STATE	InputState)
	ARG_END	(PVOID		pInputState)
    );
    COUNT	MenuRepeatDelay;
} INPUT_STATE_DESC;
typedef INPUT_STATE_DESC	*PINPUT_STATE_DESC;

INPUT_STATE	OldInputState;

PROC(
void DoInput, (pInputState),
    ARG_END	(PVOID	pInputState)
)
{
    DWORD	NewTime;
    INPUT_STATE	InputState;

    NewTime = GetTimeCounter ();
    ((PINPUT_STATE_DESC)pInputState)->MenuRepeatDelay = MENU_REPEAT_DELAY;
    do
    {
	NewTime = TaskSwitch ();

	InputState = GetInputState (NormalInput);

#if DEMO_MODE || CREATE_JOURNAL
	if (ArrowInput != DemoInput)
#endif
	{
	    static DWORD	OldTime;

	    if (InputState == OldInputState
		    && NewTime - OldTime <
		    (DWORD)((PINPUT_STATE_DESC)pInputState)->MenuRepeatDelay)
		InputState = 0;
	    else
	    {
#define ACCELERATION_INCREMENT	10
		if (InputState != OldInputState)
		    ((PINPUT_STATE_DESC)pInputState)->MenuRepeatDelay =
			    MENU_REPEAT_DELAY;
		else if (((PINPUT_STATE_DESC)pInputState)->MenuRepeatDelay
			<= ACCELERATION_INCREMENT)
		    ((PINPUT_STATE_DESC)pInputState)->MenuRepeatDelay =
			    ACCELERATION_INCREMENT;
		else
		    ((PINPUT_STATE_DESC)pInputState)->MenuRepeatDelay -=
			    ACCELERATION_INCREMENT;

		if (OldInputState != (INPUT_STATE)~0L)
		    OldInputState = InputState;
		else
		{
		    OldInputState = InputState;
		    InputState = 0;
		}

		OldTime = NewTime;
	    }

#if CREATE_JOURNAL
	    JournalInput (InputState);
#endif /* CREATE_JOURNAL */
	}

	if (InputState & DEVICE_EXIT)
	    InputState = ConfirmExit ();
	    
	if (MenuSounds
		&& (pSolarSysState == 0
			/* see if in menu */
		|| pSolarSysState->MenuState.CurState
		|| pSolarSysState->MenuState.Initialized > 2)
		&& ((InputState & DEVICE_BUTTON1)
		|| GetInputXComponent (InputState)
		|| GetInputYComponent (InputState))
#ifdef NEVER
		&& !PLRPlaying ((MUSIC_REF)~0)
#endif /* NEVER */
		)
	{
	    SOUND	S;

	    S = MenuSounds;
	    if (InputState & DEVICE_BUTTON1)
		S = SetAbsSoundIndex (S, 1);

	    PlaySoundEffect (S, 0, 0);
	}

    } while ((*((PINPUT_STATE_DESC)pInputState)->InputFunc)
	    (InputState, pInputState));

    OldInputState = (INPUT_STATE)~0L;
}

PROC(
INPUT_STATE game_input, (InputRef, InputState),
    ARG		(INPUT_REF	InputRef)
    ARG_END	(INPUT_STATE	InputState)
)
{
    if (LastActivity & (CHECK_LOAD | CHECK_RESTART))
	InputState = 0;
    else if (InputState == 0)
    {
	if ((InputState = GetInputState (JoystickInput[0])) == 0
		&& JoystickInput[1] != JoystickInput[0])
	    InputState = GetInputState (JoystickInput[1]);
    }

    return (InputState);
}

