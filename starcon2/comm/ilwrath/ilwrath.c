#include "reslib.h"
#include "resinst.h"
#include "starcon.h"
#include "commglue.h"
#include "strings.h"

static LOCDATA	ilwrath_desc =
{
    NULL_PTR,				/* init_encounter_func */
    NULL_PTR,				/* uninit_encounter_func */
    (FRAME)ILWRATH_PMAP_ANIM,		/* AlienFrame */
    0,					/* AlienColorMap */
    ILWRATH_MUSIC,			/* AlienSong */
    ILWRATH_PLAYER_PHRASES,		/* PlayerPhrases */
    4,					/* NumAnimations */
    {
	{
	    6,				/* StartIndex */
	    5,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    6, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    11,				/* StartIndex */
	    5,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    6, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    16,				/* StartIndex */
	    5,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    6, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    21,				/* StartIndex */
	    6,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    6, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
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
	5,				/* NumFrames */
	0,				/* AnimFlags */
	8, 0,				/* FrameRate */
	10, 0,				/* RestartRate */
    },
};

static void	far
CombatIsInevitable (Response)
RESPONSE_REF	Response;
{
    PUSH_CONTEXT
    {
	SET_GAME_STATE (BATTLE_SEGUE, 1);

	if (PLAYER_SAID (Response, you_are_weak))
	    NPCPhrase (STRENGTH_NOT_ALL);
	else if (PLAYER_SAID (Response, slay_by_thousands))
	    NPCPhrase (NO_SLAY_BY_THOUSANDS);
	else if (PLAYER_SAID (Response, ease_up))
	    NPCPhrase (NO_EASE_UP);
	else if (PLAYER_SAID (Response, bye_space))
	    NPCPhrase (GOODBYE_AND_DIE_SPACE);
	else if (PLAYER_SAID (Response, bye_homeworld))
	    NPCPhrase (GOODBYE_AND_DIE_HOMEWORLD);
	else if (PLAYER_SAID (Response, want_peace))
	    NPCPhrase (NO_PEACE);
	else if (PLAYER_SAID (Response, want_alliance))
	    NPCPhrase (NO_ALLIANCE);
	else if (PLAYER_SAID (Response, but_evil_is_defined))
	    NPCPhrase (DONT_CONFUSE_US);
	else if (PLAYER_SAID (Response, bye_gods))
	{
	    NPCPhrase (GOODBYE_GODS);

	    SET_GAME_STATE (BATTLE_SEGUE, 0);
	}
	if (PLAYER_SAID (Response, whats_up))
	{
	    NPCPhrase (GENERAL_INFO);
	    Response (bye, CombatIsInevitable);
	}
	else if (PLAYER_SAID (Response, whats_up_space_1)
		|| PLAYER_SAID (Response, whats_up_space_2)
		|| PLAYER_SAID (Response, whats_up_space_3)
		|| PLAYER_SAID (Response, whats_up_space_4)
		|| PLAYER_SAID (Response, whats_up_space_5))
	{
	    BYTE	NumVisits;

	    NumVisits = GET_GAME_STATE (ILWRATH_INFO);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (GENERAL_INFO_SPACE_1);
		    break;
		case 1:
		    NPCPhrase (GENERAL_INFO_SPACE_2);
		    break;
		case 2:
		    NPCPhrase (GENERAL_INFO_SPACE_3);
		    break;
		case 3:
		    NPCPhrase (GENERAL_INFO_SPACE_4);
		    break;
		case 4:
		    NPCPhrase (GENERAL_INFO_SPACE_5);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (ILWRATH_INFO, NumVisits);
	}
	else
	{
	    if (PLAYER_SAID (Response, bye))
		NPCPhrase (GOODBYE_AND_DIE);
	    else if (PLAYER_SAID (Response, where_you_come_from))
		NPCPhrase (CAME_FROM);
	    if (PLAYER_SAID (Response, it_will_be_a_pleasure))
		NPCPhrase (WHO_BLASTS_WHO);
	    if (PLAYER_SAID (Response, surrender))
		NPCPhrase (NO_SURRENDER);
	    if (PLAYER_SAID (Response, be_reasonable))
		NPCPhrase (NOT_REASONABLE);
	}
    }
    POP_CONTEXT
}

PROC_LOCAL(
void far IlwrathHome, (R),
    ARG_END	(RESPONSE_REF	R)
);

PROC(STATIC
void far IlwrathGods, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	BYTE		GodsLeft;

	GodsLeft = FALSE;
	if (PLAYER_SAID (R, want_info_on_gods))
	    NPCPhrase (SO_MUCH_TO_KNOW);
	else if (PLAYER_SAID (R, when_start_worship))
	{
	    NPCPhrase (LONG_AGO);

	    DISABLE_PHRASE (when_start_worship);
	}
	else if (PLAYER_SAID (R, any_good_gods))
	{
	    NPCPhrase (KILLED_GOOD_GODS);

	    DISABLE_PHRASE (any_good_gods);
	}
	else if (PLAYER_SAID (R, how_talk_with_gods))
	{
	    NPCPhrase (CHANNEL_44);

	    DISABLE_PHRASE (how_talk_with_gods);
	}
	else if (PLAYER_SAID (R, why_44))
	{
	    NPCPhrase (BECAUSE_44);

	    DISABLE_PHRASE (why_44);
	}

	if (PHRASE_ENABLED (when_start_worship))
	{
	    Response (when_start_worship, IlwrathGods);
	    GodsLeft = TRUE;
	}
	if (PHRASE_ENABLED (any_good_gods))
	{
	    Response (any_good_gods, IlwrathGods);
	    GodsLeft = TRUE;
	}
	if (PHRASE_ENABLED (how_talk_with_gods))
	{
	    Response (how_talk_with_gods, IlwrathGods);
	    GodsLeft = TRUE;
	}
	else if (PHRASE_ENABLED (why_44))
	{
	    Response (why_44, IlwrathGods);
	    GodsLeft = TRUE;
	}
	Response (enough_gods, IlwrathHome);

	if (!GodsLeft)
	    DISABLE_PHRASE (want_info_on_gods);
    }
    POP_CONTEXT
}

PROC(STATIC
void far IlwrathInfo, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	BYTE		InfoLeft;

	InfoLeft = FALSE;
	if (PLAYER_SAID (R, want_info_on_ilwrath))
	    NPCPhrase (WHAT_ABOUT_ILWRATH);
	else if (PLAYER_SAID (R, what_about_physio))
	{
	    NPCPhrase (ABOUT_PHYSIO);

	    DISABLE_PHRASE (what_about_physio);
	}
	else if (PLAYER_SAID (R, what_about_history))
	{
	    NPCPhrase (ABOUT_HISTORY);

	    DISABLE_PHRASE (what_about_history);
	}
	else if (PLAYER_SAID (R, what_about_culture))
	{
	    NPCPhrase (ABOUT_CULTURE);

	    DISABLE_PHRASE (what_about_culture);
	}
	else if (PLAYER_SAID (R, what_about_urquan))
	{
	    NPCPhrase (URQUAN_TOO_NICE);

	    DISABLE_PHRASE (what_about_urquan);
	}
	else if (PLAYER_SAID (R, are_you_evil))
	{
	    NPCPhrase (OF_COURSE_WERE_EVIL);

	    DISABLE_PHRASE (are_you_evil);
	}

	if (PHRASE_ENABLED (what_about_physio))
	{
	    Response (what_about_physio, IlwrathInfo);
	    InfoLeft = TRUE;
	}
	if (PHRASE_ENABLED (what_about_history))
	{
	    Response (what_about_history, IlwrathInfo);
	    InfoLeft = TRUE;
	}
	if (PHRASE_ENABLED (what_about_culture))
	{
	    Response (what_about_culture, IlwrathInfo);
	    InfoLeft = TRUE;
	}
	if (PHRASE_ENABLED (what_about_urquan))
	{
	    Response (what_about_urquan, IlwrathInfo);
	    InfoLeft = TRUE;
	}
	if (PHRASE_ENABLED (are_you_evil))
	{
	    Response (are_you_evil, IlwrathInfo);
	    InfoLeft = TRUE;
	}
	else
	{
	    Response (but_evil_is_defined, CombatIsInevitable);
	    InfoLeft = TRUE;
	}
	Response (enough_ilwrath, IlwrathHome);

	if (!InfoLeft)
	    DISABLE_PHRASE (want_info_on_ilwrath);
    }
    POP_CONTEXT
}

PROC(STATIC
void far IlwrathHome, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	if (R == 0)
	{
	    BYTE	NumVisits;

	    NumVisits = GET_GAME_STATE (ILWRATH_HOME_VISITS);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (INIT_HOME_HELLO);
		    break;
		case 1:
		    NPCPhrase (SUBSEQUENT_HOME_HELLO);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (ILWRATH_HOME_VISITS, NumVisits);
	}
	else if (PLAYER_SAID (R, enough_gods))
	    NPCPhrase (OK_ENOUGH_GODS);
	else if (PLAYER_SAID (R, enough_ilwrath))
	    NPCPhrase (OK_ENOUGH_ILWRATH);

	if (PHRASE_ENABLED (want_info_on_gods))
	{
	    Response (want_info_on_gods, IlwrathGods);
	}
	if (PHRASE_ENABLED (want_info_on_ilwrath))
	{
	    Response (want_info_on_ilwrath, IlwrathInfo);
	}
	Response (want_peace, CombatIsInevitable);
	Response (want_alliance, CombatIsInevitable);
	Response (bye_homeworld, CombatIsInevitable);
    }
    POP_CONTEXT
}

PROC_LOCAL(
void far GodsSpeak, (R),
    ARG_END	(RESPONSE_REF	R)
);

PROC(STATIC
void far GodsOrder, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	BYTE		OrdersLeft;

	OrdersLeft = FALSE;
	if (PLAYER_SAID (R, other_divine_orders))
	    NPCPhrase (WHAT_ORDERS);
	else if (PLAYER_SAID (R, say_warship))
	{
	    NPCPhrase (OK_WARSHIP);

	    DISABLE_PHRASE (say_warship);
	}
	else if (PLAYER_SAID (R, say_dwe))
	{
	    NPCPhrase (OK_DWE);

	    DISABLE_PHRASE (say_dwe);
	}
	else if (PLAYER_SAID (R, say_youboo))
	{
	    NPCPhrase (OK_YOUBOO);

	    DISABLE_PHRASE (say_youboo);
	}
	else if (PLAYER_SAID (R, say_dillrat))
	{
	    NPCPhrase (OK_DILRAT);

	    DISABLE_PHRASE (say_dillrat);
	}

	if (PHRASE_ENABLED (say_warship))
	{
	    Response (say_warship, GodsOrder);
	    OrdersLeft = TRUE;
	}
	if (PHRASE_ENABLED (say_dwe))
	{
	    Response (say_dwe, GodsOrder);
	    OrdersLeft = TRUE;
	}
	if (PHRASE_ENABLED (say_youboo))
	{
	    Response (say_youboo, GodsOrder);
	    OrdersLeft = TRUE;
	}
	if (PHRASE_ENABLED (say_dillrat))
	{
	    Response (say_dillrat, GodsOrder);
	    OrdersLeft = TRUE;
	}
	Response (enough_orders, GodsSpeak);

	if (!OrdersLeft)
	    DISABLE_PHRASE (other_divine_orders);
    }
    POP_CONTEXT
}

PROC(STATIC
void far GodsSpeak, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	BYTE		NumVisits;

	if (R == 0)
	{
	    if (GET_GAME_STATE (ILWRATH_FIGHT_THRADDASH))
		NPCPhrase (GLORIOUS_WORSHIP);
	    else if (GET_GAME_STATE (ILWRATH_DECEIVED))
		NPCPhrase (ON_WAY);
	    else
	    {
		NumVisits = GET_GAME_STATE (ILWRATH_GODS_SPOKEN);
		switch (NumVisits++)
		{
		    case 0:
			NPCPhrase (ILWRATH_BELIEVE);
			break;
		    case 1:
			NPCPhrase (GODS_RETURN_1);
			break;
		    case 2:
			NPCPhrase (GODS_RETURN_2);
			break;
		    case 3:
			NPCPhrase (GODS_RETURN_3);
			--NumVisits;
			break;
		}
		SET_GAME_STATE (ILWRATH_GODS_SPOKEN, NumVisits);
	    }
	}
	else if (PLAYER_SAID (R, go_kill_thraddash))
	{
	    NPCPhrase (OK_KILL_THRADDASH);

	    SET_GAME_STATE (ILWRATH_DECEIVED, 1);
	    AddEvent (RELATIVE_EVENT, 0, 0, 0, ADVANCE_ILWRATH_MISSION);
	}
	else if (PLAYER_SAID (R, worship_us))
	{
	    NumVisits = GET_GAME_STATE (ILWRATH_WORSHIP);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (WE_WORSHIP_1);
		    break;
		case 1:
		    NPCPhrase (WE_WORSHIP_2);
		    break;
		case 2:
		    NPCPhrase (WE_WORSHIP_3);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (ILWRATH_WORSHIP, NumVisits);

	    DISABLE_PHRASE (worship_us);
	}
	else if (PLAYER_SAID (R, enough_orders))
	    NPCPhrase (NEVER_ENOUGH);

	if (!GET_GAME_STATE (ILWRATH_DECEIVED))
	    Response (go_kill_thraddash, GodsSpeak);
	if (PHRASE_ENABLED (worship_us))
	    Response (worship_us, GodsSpeak);
	if (PHRASE_ENABLED (other_divine_orders))
	{
	    Response (other_divine_orders, GodsOrder);
	}
	Response (bye_gods, CombatIsInevitable);
    }
    POP_CONTEXT
}

PROC(STATIC
void far IlwrathSpace, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	BYTE		NumVisits;

	if (R == 0)
	{
	    NumVisits = GET_GAME_STATE (ILWRATH_VISITS);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (INIT_HELLO_SPACE);
		    break;
		case 1:
		    NPCPhrase (SUBSEQUENT_HELLO_SPACE_1);
		    break;
		case 2:
		    NPCPhrase (SUBSEQUENT_HELLO_SPACE_2);
		    break;
		case 3:
		    NPCPhrase (SUBSEQUENT_HELLO_SPACE_3);
		    break;
		case 4:
		    NPCPhrase (SUBSEQUENT_HELLO_SPACE_4);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (ILWRATH_VISITS, NumVisits);
	}

	NumVisits = GET_GAME_STATE (ILWRATH_INFO);
	switch (NumVisits)
	{
	    case 0:
		Response (whats_up_space_1, CombatIsInevitable);
		break;
	    case 1:
		Response (whats_up_space_2, CombatIsInevitable);
		break;
	    case 2:
		Response (whats_up_space_3, CombatIsInevitable);
		break;
	    case 3:
		Response (whats_up_space_4, CombatIsInevitable);
		break;
	    case 4:
		Response (whats_up_space_5, CombatIsInevitable);
		break;
	}
	Response (you_are_weak, CombatIsInevitable);
	Response (slay_by_thousands, CombatIsInevitable);
	Response (ease_up, CombatIsInevitable);
	Response (bye_space, CombatIsInevitable);
    }
    POP_CONTEXT
}

static void	far
Intro ()
{
    PUSH_CONTEXT
    {
	BYTE		NumVisits;

	if (GET_GAME_STATE (PROBE_ILWRATH_ENCOUNTER))
	{
	    NPCPhrase (SEND_MESSAGE);

	    Response (where_you_come_from, CombatIsInevitable);
	    Response (it_will_be_a_pleasure, CombatIsInevitable);
	    Response (surrender, CombatIsInevitable);
	    Response (be_reasonable, CombatIsInevitable);
	}
	else if (GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 7))
	{
	    IlwrathHome ((RESPONSE_REF)0);
	}
	else if (GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 6))
	{
	    NumVisits = GET_GAME_STATE (ILWRATH_CHMMR_VISITS);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (INIT_CHMMR_HELLO);
		    break;
		case 1:
		    NPCPhrase (SUBSEQUENT_CHMMR_HELLO);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (ILWRATH_CHMMR_VISITS, NumVisits);

	    Response (whats_up, CombatIsInevitable);
	    Response (bye, CombatIsInevitable);
	}
	else if (GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 5))
	{
	    if (GET_GAME_STATE (ILWRATH_FIGHT_THRADDASH))
		NPCPhrase (BIG_FUN);
	    else if (GET_GAME_STATE (ILWRATH_DECEIVED))
		NPCPhrase (FAST_AS_CAN);
	    else
		NPCPhrase (JUST_GRUNTS);

	    SET_GAME_STATE (BATTLE_SEGUE, 0);
	}
	else if (GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 4))
	{
	    GodsSpeak ((RESPONSE_REF)0);
	}
	else
	{
	    SET_GAME_STATE (BATTLE_SEGUE, 0);

	    if (GET_GAME_STATE (ILWRATH_FIGHT_THRADDASH))
		NPCPhrase (HAPPY_FIGHTING_THRADDASH);
	    else if (GET_GAME_STATE (ILWRATH_DECEIVED))
		NPCPhrase (ON_WAY_TO_THRADDASH);
	    else
		IlwrathSpace ((RESPONSE_REF)0);
	}
    }
    POP_CONTEXT
}

PROC(STATIC
COUNT far uninit_ilwrath, (),
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
LOCDATAPTR far init_ilwrath_comm, (),
    ARG_VOID
)
{
    LOCDATAPTR	retval;

    PUSH_CONTEXT
    {
	ilwrath_desc.init_encounter_func = Intro;
	ilwrath_desc.uninit_encounter_func = uninit_ilwrath;

strcpy(aiff_folder, "comm/ilwrath/ilwra");

	if (GET_GAME_STATE (PROBE_ILWRATH_ENCOUNTER)
		|| (GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA)
		& ((1 << 4) | (1 << 5)))
		|| LOBYTE (GLOBAL (CurrentActivity)) == WON_LAST_BATTLE)
	{
	    SET_GAME_STATE (BATTLE_SEGUE, 0);
	}
	else
	{
	    SET_GAME_STATE (BATTLE_SEGUE, 1);
	}
	retval = &ilwrath_desc;
    }
    POP_CONTEXT

    return (retval);
}
