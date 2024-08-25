#include "reslib.h"
#include "resinst.h"
#include "starcon.h"
#include "commglue.h"
#include "strings.h"

static LOCDATA	mycon_desc =
{
    NULL_PTR,				/* init_encounter_func */
    NULL_PTR,				/* uninit_encounter_func */
    (FRAME)MYCON_PMAP_ANIM,		/* AlienFrame */
    0,					/* AlienColorMap */
    MYCON_MUSIC,			/* AlienSong */
    MYCON_PLAYER_PHRASES,		/* PlayerPhrases */
    5,					/* NumAnimations */
    {
	{
	    12,				/* StartIndex */
	    6,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    9, 0,			/* FrameRate */
	    9, 0,			/* RestartRate */
	    (1 << 1),			/* BlockMask */
	},
	{
	    18,				/* StartIndex */
	    4,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    9, 0,			/* FrameRate */
	    9, 0,			/* RestartRate */
	    (1 << 0),			/* BlockMask */
	},
	{
	    22,				/* StartIndex */
	    6,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    9, 0,			/* FrameRate */
	    9, 0,			/* RestartRate */
	},
	{
	    28,				/* StartIndex */
	    5,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    9, 0,			/* FrameRate */
	    9, 0,			/* RestartRate */
	},
	{
	    33,				/* StartIndex */
	    4,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    9, 0,			/* FrameRate */
	    9, 0,			/* RestartRate */
	},
    },
    {
	0,				/* StartIndex */
	0,				/* NumFrames */
	0,				/* AnimFlags */
	0, 0,				/* FrameRate */
	0, 0,				/* RestartRate */
    },
    {
	1,				/* StartIndex */
	11,				/* NumFrames */
	0,				/* AnimFlags */
	0,				/* AnimFlags */
	8, 0,				/* FrameRate */
	10, 0,				/* RestartRate */
    },
};

static BYTE	MadeChoice = 0;

PROC(STATIC
void near DoRamble, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    BYTE	Counter;

    Counter = GET_GAME_STATE (MYCON_RAMBLE);
    switch (Counter++)
    {
	case 0:
	    NPCPhrase (RAMBLE_1);
	    break;
	case 1:
	    NPCPhrase (RAMBLE_2);
	    break;
	case 2:
	    NPCPhrase (RAMBLE_3);
	    break;
	case 3:
	    NPCPhrase (RAMBLE_4);
	    break;
	case 4:
	    NPCPhrase (RAMBLE_5);
	    break;
	case 5:
	    NPCPhrase (RAMBLE_6);
	    break;
	case 6:
	    NPCPhrase (RAMBLE_7);
	    break;
	case 7:
	    NPCPhrase (RAMBLE_8);
	    break;
	case 8:
	    NPCPhrase (RAMBLE_9);
	    break;
	case 9:
	    NPCPhrase (RAMBLE_10);
	    break;
	case 10:
	    NPCPhrase (RAMBLE_11);
	    break;
	case 11:
	    NPCPhrase (RAMBLE_12);
	    break;
	case 12:
	    NPCPhrase (RAMBLE_13);
	    break;
	case 13:
	    NPCPhrase (RAMBLE_14);
	    break;
	case 14:
	    NPCPhrase (RAMBLE_15);
	    break;
	case 15:
	    NPCPhrase (RAMBLE_16);
	    break;
	case 16:
	    NPCPhrase (RAMBLE_17);
	    break;
	case 17:
	    NPCPhrase (RAMBLE_18);
	    break;
	case 18:
	    NPCPhrase (RAMBLE_19);
	    break;
	case 19:
	    NPCPhrase (RAMBLE_20);
	    break;
	case 20:
	    NPCPhrase (RAMBLE_21);
	    break;
	case 21:
	    NPCPhrase (RAMBLE_22);
	    break;
	case 22:
	    NPCPhrase (RAMBLE_23);
	    break;
	case 23:
	    NPCPhrase (RAMBLE_24);
	    break;
	case 24:
	    NPCPhrase (RAMBLE_25);
	    if (GET_GAME_STATE (KNOW_ABOUT_SHATTERED) < 2)
	    {
		SET_GAME_STATE (KNOW_ABOUT_SHATTERED, 2);
	    }
	    break;
	case 25:
	    NPCPhrase (RAMBLE_26);
	    break;
	case 26:
	    NPCPhrase (RAMBLE_27);
	    break;
	case 27:
	    NPCPhrase (RAMBLE_28);
	    break;
	case 28:
	    NPCPhrase (RAMBLE_29);
	    break;
	case 29:
	    NPCPhrase (RAMBLE_30);
	    break;
	case 30:
	    NPCPhrase (RAMBLE_31);
	    break;
	case 31:
	    NPCPhrase (RAMBLE_32);
	    Counter = 0;
	    break;
    }
    SET_GAME_STATE (MYCON_RAMBLE, Counter);

    if (!(GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 7)))
    {
	if (!PLAYER_SAID (R, come_in_peace)
		&& !PLAYER_SAID (R, gonna_die))
	{
	    Counter = (GET_GAME_STATE (MYCON_INSULTS) + 1) & 7;
	    SET_GAME_STATE (MYCON_INSULTS, Counter);
	    MadeChoice = 1;
	}
    }
    else if (!PLAYER_SAID (R, lets_be_friends)
	    && !PLAYER_SAID (R, came_to_homeworld)
	    && !PLAYER_SAID (R, submit_to_us))
    {
	Counter = (GET_GAME_STATE (MYCON_INFO) + 1) & 15;
	SET_GAME_STATE (MYCON_INFO, Counter);
	MadeChoice = 1;
    }
}

PROC(STATIC
void far CombatIsInevitable, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	SET_GAME_STATE (BATTLE_SEGUE, 1);

	if (PLAYER_SAID (R, bye_space))
	    NPCPhrase (BYE_AND_DIE_SPACE);
	else if (PLAYER_SAID (R, bye_homeworld))
	    NPCPhrase (BYE_AND_DIE_HOMEWORLD);
	else if (PLAYER_SAID (R, like_to_land))
	    NPCPhrase (NEVER_LET_LAND);
	else if (PLAYER_SAID (R, bye_sun_device))
	{
	    NPCPhrase (GOODBYE_SUN_DEVICE);

	    SET_GAME_STATE (BATTLE_SEGUE, 0);
	}
	else
	{
	    DoRamble (R);
	    if (!(GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 7)))
		NPCPhrase (BYE_AND_DIE_SPACE);
	    else
		NPCPhrase (BYE_AND_DIE_HOMEWORLD);
	}
    }
    POP_CONTEXT
}

PROC(STATIC
void far SunDevice, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	if (PLAYER_SAID (R, whats_up_sun_device))
	{
	    NPCPhrase (GENERAL_INFO_SUN_DEVICE);

	    DISABLE_PHRASE (whats_up_sun_device);
	}
	else if (PLAYER_SAID (R, how_goes_implanting))
	{
	    NPCPhrase (UNFORSEEN_DELAYS);

	    DISABLE_PHRASE (how_goes_implanting);
	}
	else if (PLAYER_SAID (R, i_have_a_cunning_plan))
	{
	    NPCPhrase (WONT_FALL_FOR_TRICK);

	    SET_GAME_STATE (NO_TRICK_AT_SUN, 1);
	}

	if (PHRASE_ENABLED (whats_up_sun_device))
	    Response (whats_up_sun_device, SunDevice);
	if (GET_GAME_STATE (MYCON_FELL_FOR_AMBUSH))
	{
	    if (PHRASE_ENABLED (how_goes_implanting) && GET_GAME_STATE (MYCON_FELL_FOR_AMBUSH))
		Response (how_goes_implanting, SunDevice);
	    Response (like_to_land, CombatIsInevitable);
	}
	else if (GET_GAME_STATE (MYCON_AMBUSH)
		&& !GET_GAME_STATE (NO_TRICK_AT_SUN))
	    Response (i_have_a_cunning_plan, SunDevice);
	Response (bye_sun_device, CombatIsInevitable);
    }
    POP_CONTEXT
}

PROC(STATIC
void far TrickMycon, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	if (PLAYER_SAID (R, i_have_a_cunning_plan))
	{
	    NPCPhrase (TELL_US_ABOUT_WORLD);

	    DISABLE_PHRASE (i_have_a_cunning_plan);
	}
	else if (PLAYER_SAID (R, clue_1))
	{
	    NPCPhrase (RESPONSE_1);

	    DISABLE_PHRASE (clue_1);
	}
	else if (PLAYER_SAID (R, clue_2))
	{
	    NPCPhrase (RESPONSE_2);

	    DISABLE_PHRASE (clue_2);
	}
	else if (PLAYER_SAID (R, clue_3))
	{
	    NPCPhrase (RESPONSE_3);

	    DISABLE_PHRASE (clue_3);
	}

	if (PHRASE_ENABLED (clue_1))
	    Response (clue_1, TrickMycon);
	if (PHRASE_ENABLED (clue_2))
	    Response (clue_2, TrickMycon);
	if (PHRASE_ENABLED (clue_3))
	    Response (clue_3, TrickMycon);

	if (PHRASE_ENABLED (clue_1) == 0
		&& PHRASE_ENABLED (clue_2) == 0
		&& PHRASE_ENABLED (clue_3) == 0)
	{
	    NPCPhrase (WE_GO_TO_IMPLANT);

	    SET_GAME_STATE (BATTLE_SEGUE, 0);
	    SET_GAME_STATE (MYCON_FELL_FOR_AMBUSH, 1);
	    AddEvent (RELATIVE_EVENT, 0, 0, 0, ADVANCE_MYCON_MISSION);
	}
    }
    POP_CONTEXT
}

PROC(STATIC
void far NormalMycon, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
RESPONSE_FUNC RespFunc;

	if (PLAYER_SAID (R, what_about_shattered))
	{
	    NPCPhrase (ABOUT_SHATTERED);

	    SET_GAME_STATE (KNOW_ABOUT_SHATTERED, 2);
	}
	else if (R)
	{
	    DoRamble (R);

	    DISABLE_PHRASE (RESPONSE_TO_REF (R));
	}

	if ((BYTE)random () < 256 * 30 / 100)
	    RespFunc = (RESPONSE_FUNC)CombatIsInevitable;
	else
	    RespFunc = (RESPONSE_FUNC)NormalMycon;
	if (!(GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 7)))
	{
	    if (PHRASE_ENABLED (come_in_peace))
		Response (come_in_peace, RespFunc);
	    if (PHRASE_ENABLED (gonna_die))
		Response (gonna_die, RespFunc);
	    if (!MadeChoice) switch (GET_GAME_STATE (MYCON_INSULTS))
	    {
		case 0:
		    Response (insult_1, RespFunc);
		    break;
		case 1:
		    Response (insult_2, RespFunc);
		    break;
		case 2:
		    Response (insult_3, RespFunc);
		    break;
		case 3:
		    Response (insult_4, RespFunc);
		    break;
		case 4:
		    Response (insult_5, RespFunc);
		    break;
		case 5:
		    Response (insult_6, RespFunc);
		    break;
		case 6:
		    Response (insult_7, RespFunc);
		    break;
		case 7:
		    Response (insult_8, RespFunc);
		    break;
	    }
	    Response (bye_space, CombatIsInevitable);
	}
	else
	{
	    if (!MadeChoice) switch (GET_GAME_STATE (MYCON_INFO))
	    {
		case 0:
		    Response (question_1, RespFunc);
		    break;
		case 1:
		    Response (question_2, RespFunc);
		    break;
		case 2:
		    Response (question_3, RespFunc);
		    break;
		case 3:
		    Response (question_4, RespFunc);
		    break;
		case 4:
		    Response (question_5, RespFunc);
		    break;
		case 5:
		    Response (question_6, RespFunc);
		    break;
		case 6:
		    Response (question_7, RespFunc);
		    break;
		case 7:
		    Response (question_8, RespFunc);
		    break;
		case 8:
		    Response (question_9, RespFunc);
		    break;
		case 9:
		    Response (question_10, RespFunc);
		    break;
		case 10:
		    Response (question_11, RespFunc);
		    break;
		case 11:
		    Response (question_12, RespFunc);
		    break;
		case 12:
		    Response (question_13, RespFunc);
		    break;
		case 13:
		    Response (question_14, RespFunc);
		    break;
		case 14:
		    Response (question_15, RespFunc);
		    break;
		case 15:
		    Response (question_16, RespFunc);
		    break;
	    }
	    if (PHRASE_ENABLED (lets_be_friends))
		Response (lets_be_friends, RespFunc);
	    if (PHRASE_ENABLED (came_to_homeworld))
		Response (came_to_homeworld, RespFunc);
	    if (PHRASE_ENABLED (submit_to_us))
		Response (submit_to_us, RespFunc);
	    if (!GET_GAME_STATE (MYCON_FELL_FOR_AMBUSH))
	    {
		if (GET_GAME_STATE (KNOW_ABOUT_SHATTERED) == 1)
		    Response (what_about_shattered, NormalMycon);
		if (GET_GAME_STATE (MYCON_AMBUSH))
		{
		    Response (i_have_a_cunning_plan, TrickMycon);
		}
	    }
	    Response (bye_homeworld, CombatIsInevitable);
	}
    }
    POP_CONTEXT
}

static void	far
Intro ()
{
    PUSH_CONTEXT
    {
	BYTE	NumVisits;

	if (GET_GAME_STATE (SUN_DEVICE))
	{
	    NumVisits = GET_GAME_STATE (MYCON_VISITS);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (DIE_THIEF);
		    break;
		case 1:
		    NPCPhrase (DIE_THIEF_AGAIN);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (MYCON_VISITS, NumVisits);

	    SET_GAME_STATE (BATTLE_SEGUE, 1);
	}
	else if (GET_GAME_STATE (MYCON_KNOW_AMBUSH))
	{
	    NPCPhrase (DIE_LIAR);

	    SET_GAME_STATE (BATTLE_SEGUE, 1);
	}
	else if (GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 6))
	{
	    NumVisits = GET_GAME_STATE (MYCON_SUN_VISITS);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (HELLO_SUN_DEVICE_WORLD_1);
		    break;
		case 1:
		    NPCPhrase (HELLO_SUN_DEVICE_WORLD_2);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (MYCON_SUN_VISITS, NumVisits);

	    SunDevice ((RESPONSE_REF)0);
	}
	else if (GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 7))
	{
	    NumVisits = GET_GAME_STATE (MYCON_HOME_VISITS);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (HELLO_HOMEWORLD_1);
		    break;
		case 1:
		    NPCPhrase (HELLO_HOMEWORLD_2);
		    break;
		case 2:
		    NPCPhrase (HELLO_HOMEWORLD_3);
		    break;
		case 3:
		    NPCPhrase (HELLO_HOMEWORLD_4);
		    break;
		case 4:
		    NPCPhrase (HELLO_HOMEWORLD_5);
		    break;
		case 5:
		    NPCPhrase (HELLO_HOMEWORLD_6);
		    break;
		case 6:
		    NPCPhrase (HELLO_HOMEWORLD_7);
		    break;
		case 7:
		    NPCPhrase (HELLO_HOMEWORLD_8);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (MYCON_HOME_VISITS, NumVisits);

	    NormalMycon ((RESPONSE_REF)0);
	}
	else
	{
	    NumVisits = GET_GAME_STATE (MYCON_VISITS);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (HELLO_SPACE_1);
		    break;
		case 1:
		    NPCPhrase (HELLO_SPACE_2);
		    break;
		case 2:
		    NPCPhrase (HELLO_SPACE_3);
		    break;
		case 3:
		    NPCPhrase (HELLO_SPACE_4);
		    break;
		case 4:
		    NPCPhrase (HELLO_SPACE_5);
		    break;
		case 5:
		    NPCPhrase (HELLO_SPACE_6);
		    break;
		case 6:
		    NPCPhrase (HELLO_SPACE_7);
		    break;
		case 7:
		    NPCPhrase (HELLO_SPACE_8);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (MYCON_VISITS, NumVisits);

	    NormalMycon ((RESPONSE_REF)0);
	}
    }
    POP_CONTEXT
}

PROC(STATIC
COUNT far uninit_mycon, (),
    ARG_VOID
)
{
    PUSH_CONTEXT
    {
    }
    POP_CONTEXT

    return (0);
}

PROC(
LOCDATAPTR far init_mycon_comm, (),
    ARG_VOID
)
{
    LOCDATAPTR	retval;

    PUSH_CONTEXT
    {
	mycon_desc.init_encounter_func = Intro;
	mycon_desc.uninit_encounter_func = uninit_mycon;

strcpy(aiff_folder, "comm/mycon/mycon");

	if (LOBYTE (GLOBAL (CurrentActivity)) != WON_LAST_BATTLE)
	{
	    SET_GAME_STATE (BATTLE_SEGUE, 1);
	}
	else
	{
	    SET_GAME_STATE (BATTLE_SEGUE, 0);
	}
	retval = &mycon_desc;
    }
    POP_CONTEXT

    return (retval);
}
