#include "reslib.h"
#include "resinst.h"
#include "starcon.h"
#include "commglue.h"
#include "strings.h"

static LOCDATA	supox_desc =
{
    NULL_PTR,				/* init_encounter_func */
    NULL_PTR,				/* uninit_encounter_func */
    (FRAME)SUPOX_PMAP_ANIM,		/* AlienFrame */
    0,					/* AlienColorMap */
    SUPOX_MUSIC,			/* AlienSong */
    SUPOX_PLAYER_PHRASES,		/* PlayerPhrases */
    4,					/* NumAnimations */
    {
	{
	    4,				/* StartIndex */
	    5,				/* NumFrames */
	    YOYO_ANIM
		    | WAIT_TALKING,	/* AnimFlags */
	    12, 12,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    9,				/* StartIndex */
	    10,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    8, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    19,				/* StartIndex */
	    10,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    8, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    29,				/* StartIndex */
	    13,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    8, 0,			/* FrameRate */
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
	3,				/* NumFrames */
	0,				/* AnimFlags */
	8, 0,				/* FrameRate */
	10, 0,				/* RestartRate */
    },
};

PROC(STATIC
void far ExitConversation, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	SET_GAME_STATE (BATTLE_SEGUE, 0);

	if (PLAYER_SAID (R, bye_neutral))
	    NPCPhrase (GOODBYE_NEUTRAL);
	else if (PLAYER_SAID (R, what_do_i_do_now))
	    NPCPhrase (FIX_IT);
	else if (PLAYER_SAID (R, thanks_now_we_eat_you))
	{
	    NPCPhrase (HIDEOUS_MONSTERS);

	    SET_GAME_STATE (SUPOX_HOSTILE, 1);
	    SET_GAME_STATE (SUPOX_HOME_VISITS, 0);
	    SET_GAME_STATE (SUPOX_VISITS, 0);
	}
	else if (PLAYER_SAID (R, bye_after_space))
	    NPCPhrase (GOODBYE_AFTER_SPACE);
	else if (PLAYER_SAID (R, bye_before_space))
	    NPCPhrase (GOODBYE_BEFORE_SPACE);
	else if (PLAYER_SAID (R, bye_allied_homeworld))
	    NPCPhrase (GOODBYE_ALLIED_HOMEWORLD);
	else if (PLAYER_SAID (R, can_you_help))
	{
	    NPCPhrase (HOW_HELP);
	    if (ActivateStarShip (SUPOX_SHIP, FEASIBILITY_STUDY) == 0)
		NPCPhrase (DONT_NEED);
	    else
	    {
		NPCPhrase (HAVE_4_SHIPS);

		AlienTalkSegue ((COUNT)~0);
		ActivateStarShip (SUPOX_SHIP, 4);
	    }
	}
    }
    POP_CONTEXT
}

PROC_LOCAL(
void far AlliedHome, (R),
    ARG_END	(RESPONSE_REF	R)
);

PROC(STATIC
void far AlliedHome, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	BYTE		NumVisits, News;

	News = GET_GAME_STATE (SUPOX_WAR_NEWS);
	NumVisits = GET_GAME_STATE (UTWIG_SUPOX_MISSION);
	if (PLAYER_SAID (R, how_went_war))
	{
	    NPCPhrase (ABOUT_BATTLE);

	    News |= (1 << 0);
	}
	else if (PLAYER_SAID (R, how_goes_war))
	{
	    if (NumVisits == 1)
	    {
		NPCPhrase (FLEET_ON_WAY);

		SET_GAME_STATE (SUPOX_WAR_NEWS, 1);
	    }
	    else switch (GET_GAME_STATE (SUPOX_WAR_NEWS))
	    {
		case 0:
		    NPCPhrase (BATTLE_HAPPENS_1);
		    News = 1;
		    break;
		case 1:
		    NPCPhrase (BATTLE_HAPPENS_2);
		    News = 2;
		    break;
	    }

	    DISABLE_PHRASE (how_goes_war);
	}
	else if (PLAYER_SAID (R, learn_new_info))
	{
	    if (NumVisits < 5)
		NPCPhrase (NO_NEW_INFO);
	    else
	    {
		NPCPhrase (SAMATRA);

		News |= (1 << 1);
	    }

	    DISABLE_PHRASE (learn_new_info);
	}
	else if (PLAYER_SAID (R, what_now_homeworld))
	{
	    if (NumVisits < 5)
		NPCPhrase (UP_TO_YOU);
	    else
		NPCPhrase (HOPE_KILL_EACH_OTHER);

	    DISABLE_PHRASE (what_now_homeworld);
	}
	else if (PLAYER_SAID (R, how_is_ultron))
	{
	    NPCPhrase (ULTRON_IS_GREAT);

	    DISABLE_PHRASE (how_is_ultron);
	}
	SET_GAME_STATE (SUPOX_WAR_NEWS, News);

	if (NumVisits >= 5)
	{
	    if (!(News & (1 << 0)))
		Response (how_went_war, AlliedHome);
	}
	else if (PHRASE_ENABLED (how_goes_war)
		&& ((NumVisits == 1 && News == 0)
		|| (NumVisits && News < 2)))
	    Response (how_goes_war, AlliedHome);
	if (PHRASE_ENABLED (learn_new_info))
	    Response (learn_new_info, AlliedHome);
	if (PHRASE_ENABLED (what_now_homeworld))
	    Response (what_now_homeworld, AlliedHome);
	if (PHRASE_ENABLED (how_is_ultron))
	    Response (how_is_ultron, AlliedHome);
	if (NumVisits == 0)
	    Response (can_you_help, ExitConversation);
	Response (bye_allied_homeworld, ExitConversation);
    }
    POP_CONTEXT
}

PROC(STATIC
void far BeforeKohrAh, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	BYTE		NumVisits;

	if (PLAYER_SAID (R, whats_up_before_space))
	{
	    NumVisits = GET_GAME_STATE (SUPOX_INFO);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (GEN_INFO_BEFORE_SPACE_1);
		    break;
		case 1:
		    NPCPhrase (GEN_INFO_BEFORE_SPACE_2);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (SUPOX_INFO, NumVisits);

	    DISABLE_PHRASE (whats_up_before_space);
	}
	else if (PLAYER_SAID (R, what_now_before_space))
	{
	    NPCPhrase (DO_THIS_BEFORE_SPACE);

	    DISABLE_PHRASE (what_now_before_space);
	}

	if (PHRASE_ENABLED (whats_up_before_space))
	    Response (whats_up_before_space, BeforeKohrAh);
	if (PHRASE_ENABLED (what_now_before_space))
	    Response (what_now_before_space, BeforeKohrAh);
	Response (bye_before_space, ExitConversation);
    }
    POP_CONTEXT
}

PROC(STATIC
void far AfterKohrAh, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	BYTE		NumVisits;

	if (PLAYER_SAID (R, whats_up_after_space))
	{
	    NumVisits = GET_GAME_STATE (SUPOX_INFO);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (GENERAL_INFO_AFTER_SPACE_1);
		    break;
		case 1:
		    NPCPhrase (GENERAL_INFO_AFTER_SPACE_2);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (SUPOX_INFO, NumVisits);

	    DISABLE_PHRASE (whats_up_after_space);
	}
	else if (PLAYER_SAID (R, what_now_after_space))
	{
	    NPCPhrase (DO_THIS_AFTER_SPACE);

	    DISABLE_PHRASE (what_now_after_space);
	}

	if (PHRASE_ENABLED (whats_up_after_space))
	    Response (whats_up_after_space, AfterKohrAh);
	if (PHRASE_ENABLED (what_now_after_space))
	    Response (what_now_after_space, AfterKohrAh);
	Response (bye_after_space, ExitConversation);
    }
    POP_CONTEXT
}

PROC(STATIC
void far NeutralSupox, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	BYTE		i, LastStack, NumVisits;
	RESPONSE_REF	pStr[3];

	LastStack = 0;
	pStr[0] = pStr[1] = pStr[2] = 0;
	if (PLAYER_SAID (R, i_am0))
	{
	    NPCPhrase (WE_ARE_SUPOX);

	    SET_GAME_STATE (SUPOX_STACK1, 1);
	    DISABLE_PHRASE (i_am0);
	}
	else if (PLAYER_SAID (R, my_ship0))
	{
	    NPCPhrase (OUR_SHIP);

	    SET_GAME_STATE (SUPOX_STACK1, 2);
	    DISABLE_PHRASE (my_ship0);
	}
	else if (PLAYER_SAID (R, from_alliance0))
	{
	    NPCPhrase (FROM_SUPOX);

	    SET_GAME_STATE (SUPOX_STACK1, 3);
	    DISABLE_PHRASE (from_alliance0);
	}
	else if (PLAYER_SAID (R, are_you_copying))
	{
	    NPCPhrase (YEAH_SORRY);

	    SET_GAME_STATE (SUPOX_STACK1, 4);
	}
	else if (PLAYER_SAID (R, why_copy))
	{
	    NPCPhrase (SYMBIOTS);

	    SET_GAME_STATE (SUPOX_STACK1, 5);
	}
	else if (PLAYER_SAID (R, tell_us_of_your_species))
	{
	    NPCPhrase (OUR_SPECIES);

	    LastStack = 1;
	    SET_GAME_STATE (SUPOX_STACK2, 1);
	}
	else if (PLAYER_SAID (R, plants_arent_intelligent))
	{
	    NPCPhrase (PROVES_WERE_SPECIAL);

	    SET_GAME_STATE (SUPOX_STACK2, 2);
	}
	else if (PLAYER_SAID (R, anyone_around_here))
	{
	    NPCPhrase (UTWIG_NEARBY);

	    LastStack = 2;
	    SET_GAME_STATE (SUPOX_WAR_NEWS, 1);
	    ActivateStarShip (UTWIG_SHIP, SPHERE_TRACKING);
	}
	else if (PLAYER_SAID (R, what_relation_to_utwig))
	{
	    NPCPhrase (UTWIG_ALLIES);

	    LastStack = 2;
	    SET_GAME_STATE (SUPOX_WAR_NEWS, 1);
	}
	else if (PLAYER_SAID (R, whats_wrong_with_utwig))
	{
	    NPCPhrase (BROKE_ULTRON);

	    LastStack = 2;
	    SET_GAME_STATE (SUPOX_WAR_NEWS, 2);
	}
	else if (PLAYER_SAID (R, whats_ultron))
	{
	    NPCPhrase (TAKE_ULTRON);

	    SET_GAME_STATE (SUPOX_WAR_NEWS, 0);
	    SET_GAME_STATE (ULTRON_CONDITION, 1);

	    Response (what_do_i_do_now, ExitConversation);
	    Response (thanks_now_we_eat_you, ExitConversation);

	    goto ExitSupox;
	}
	else if (PLAYER_SAID (R, got_fixed_ultron))
	{
	    NPCPhrase (GOOD_GIVE_TO_UTWIG);

	    SET_GAME_STATE (SUPOX_ULTRON_HELP, 1);
	}
	else if (PLAYER_SAID (R, look_i_repaired_lots))
	{
	    NPCPhrase (ALMOST_THERE);

	    SET_GAME_STATE (SUPOX_ULTRON_HELP, 1);
	}
	else if (PLAYER_SAID (R, look_i_slightly_repaired))
	{
	    NPCPhrase (GREAT_DO_MORE);

	    SET_GAME_STATE (SUPOX_ULTRON_HELP, 1);
	}
	else if (PLAYER_SAID (R, where_get_repairs))
	{
	    NPCPhrase (ANCIENT_RHYME);

	    SET_GAME_STATE (SUPOX_ULTRON_HELP, 1);
	}

	switch (GET_GAME_STATE (SUPOX_STACK2))
	{
	    case 0:
		pStr[1] = tell_us_of_your_species;
		break;
	    case 1:
		pStr[1] = plants_arent_intelligent;
		break;
	}
	switch (GET_GAME_STATE (SUPOX_STACK1))
	{
	    case 0:
		construct_response (shared_phrase_buf,
			i_am0,
	    		GLOBAL_SIS (CommanderName),
			i_am1,
			0);
		pStr[0] = i_am0;
		pStr[1] = 0;
		break;
	    case 1:
		construct_response (shared_phrase_buf,
			my_ship0,
			GLOBAL_SIS (ShipName),
			my_ship1,
			0);
		pStr[0] = my_ship0;
		pStr[1] = 0;
		break;
	    case 2:
		{
		    char	buf[80];

		    GetAllianceName (buf, name_1);
		    construct_response (
			    shared_phrase_buf,
			    from_alliance0,
			    buf,
			    from_alliance1,
			    0
			    );
		}
		pStr[0] = from_alliance0;
		pStr[1] = 0;
		break;
	    case 3:
		pStr[0] = are_you_copying;
		break;
	    case 4:
		pStr[0] = why_copy;
		break;
	}
	NumVisits = GET_GAME_STATE (ULTRON_CONDITION);
	if (NumVisits == 0)
	{
	    switch (GET_GAME_STATE (SUPOX_WAR_NEWS))
	    {
		case 0:
		    if (GET_GAME_STATE (UTWIG_VISITS)
			    || GET_GAME_STATE (UTWIG_HOME_VISITS)
			    || GET_GAME_STATE (BOMB_VISITS))
			pStr[2] = what_relation_to_utwig;
		    else
			pStr[2] = anyone_around_here;
		    break;
		case 1:
		    pStr[2] = whats_wrong_with_utwig;
		    break;
		case 2:
		    pStr[2] = whats_ultron;
		    break;
	    }
	}
	if (pStr[LastStack])
	{
	    if (LastStack != 0 || GET_GAME_STATE (SUPOX_STACK1) > 2)
		Response (pStr[LastStack], NeutralSupox);
	    else
		DoResponsePhrase (pStr[LastStack], NeutralSupox, shared_phrase_buf);
	}
	for (i = 0; i < 3; ++i)
	{
	    if (i != LastStack && pStr[i])
	    {
		if (i != 0 || GET_GAME_STATE (SUPOX_STACK1) > 2)
		    Response (pStr[i], NeutralSupox);
		else
		    DoResponsePhrase (pStr[i], NeutralSupox, shared_phrase_buf);
	    }
	}
	if (!GET_GAME_STATE (SUPOX_ULTRON_HELP))
	{
	    switch (NumVisits)
	    {
		case 1:
		    Response (where_get_repairs, NeutralSupox);
		    break;
		case 2:
		    Response (look_i_slightly_repaired, NeutralSupox);
		    break;
		case 3:
		    Response (look_i_repaired_lots, NeutralSupox);
		    break;
		case 4:
		    Response (got_fixed_ultron, NeutralSupox);
		    break;
	    }
	}
	Response (bye_neutral, ExitConversation);
ExitSupox:
	;
    }
    POP_CONTEXT
}

static void	far
Intro ()
{
    PUSH_CONTEXT
    {
	BYTE	NumVisits;

	if (LOBYTE (GLOBAL (CurrentActivity)) == WON_LAST_BATTLE)
	{
//	    NPCPhrase (OUT_TAKES);

	    SET_GAME_STATE (BATTLE_SEGUE, 0);
	    goto ExitIntro;
	}

	if (GET_GAME_STATE (SUPOX_HOSTILE))
	{
	    NumVisits = GET_GAME_STATE (SUPOX_VISITS);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (HOSTILE_SPACE_HELLO_1);
		    break;
		case 1:
		    NPCPhrase (HOSTILE_SPACE_HELLO_2);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (SUPOX_VISITS, NumVisits);

	    SET_GAME_STATE (BATTLE_SEGUE, 0);
	}
	else if (ActivateStarShip (SUPOX_SHIP, CHECK_ALLIANCE) & GOOD_GUY)
	{
	    if (GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 7))
	    {
		NumVisits = GET_GAME_STATE (SUPOX_HOME_VISITS);
		switch (NumVisits++)
		{
		    case 0:
			NPCPhrase (ALLIED_HOMEWORLD_HELLO_1);
			break;
		    case 1:
			NPCPhrase (ALLIED_HOMEWORLD_HELLO_2);
			break;
		    case 2:
			NPCPhrase (ALLIED_HOMEWORLD_HELLO_3);
			break;
		    case 3:
			NPCPhrase (ALLIED_HOMEWORLD_HELLO_4);
			--NumVisits;
			break;
		}
		SET_GAME_STATE (SUPOX_HOME_VISITS, NumVisits);

		AlliedHome ((RESPONSE_REF)0);
	    }
	    else
	    {
		NumVisits = GET_GAME_STATE (UTWIG_SUPOX_MISSION);
		if (NumVisits == 1)
		{
		    NumVisits = GET_GAME_STATE (SUPOX_VISITS);
		    switch (NumVisits++)
		    {
			case 0:
			    NPCPhrase (HI_BEFORE_KOHRAH_SPACE_1);
			    break;
			case 1:
			    NPCPhrase (HI_BEFORE_KOHRAH_SPACE_2);
			    --NumVisits;
			    break;
		    }
		    SET_GAME_STATE (SUPOX_VISITS, NumVisits);

		    BeforeKohrAh ((RESPONSE_REF)0);
		}
		else if (NumVisits < 5)
		{
		    NumVisits = GET_GAME_STATE (SUPOX_VISITS);
		    switch (NumVisits++)
		    {
			case 0:
			    NPCPhrase (HI_DURING_KOHRAH_SPACE_1);
			    break;
			case 1:
			    NPCPhrase (HI_DURING_KOHRAH_SPACE_2);
			    --NumVisits;
			    break;
		    }
		    SET_GAME_STATE (SUPOX_VISITS, NumVisits);
		}
		else
		{
		    NumVisits = GET_GAME_STATE (SUPOX_VISITS);
		    switch (NumVisits++)
		    {
			case 0:
			    NPCPhrase (HI_AFTER_KOHRAH_SPACE_1);
			    break;
			case 1:
			    NPCPhrase (HI_AFTER_KOHRAH_SPACE_2);
			    --NumVisits;
			    break;
		    }
		    SET_GAME_STATE (SUPOX_VISITS, NumVisits);

		    AfterKohrAh ((RESPONSE_REF)0);
		}
	    }
	}
	else
	{
	    if (GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 7))
	    {
		NumVisits = GET_GAME_STATE (SUPOX_HOME_VISITS);
		switch (NumVisits++)
		{
		    case 0:
			NPCPhrase (NEUTRAL_HOMEWORLD_HELLO_1);
			break;
		    case 1:
			NPCPhrase (NEUTRAL_HOMEWORLD_HELLO_2);
			--NumVisits;
			break;
		}
		SET_GAME_STATE (SUPOX_HOME_VISITS, NumVisits);
	    }
	    else
	    {
		NumVisits = GET_GAME_STATE (SUPOX_VISITS);
		switch (NumVisits++)
		{
		    case 0:
			NPCPhrase (NEUTRAL_SPACE_HELLO_1);
			break;
		    case 1:
			NPCPhrase (NEUTRAL_SPACE_HELLO_2);
			--NumVisits;
			break;
		}
		SET_GAME_STATE (SUPOX_VISITS, NumVisits);
	    }

	    NeutralSupox ((RESPONSE_REF)0);
	}
ExitIntro:
	;
    }
    POP_CONTEXT
}

PROC(STATIC
COUNT far uninit_supox, (),
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
LOCDATAPTR far init_supox_comm, (),
    ARG_VOID
)
{
    LOCDATAPTR	retval;

    PUSH_CONTEXT
    {
	supox_desc.init_encounter_func = Intro;
	supox_desc.uninit_encounter_func = uninit_supox;

strcpy(aiff_folder, "comm/supox/supox");

	if (!GET_GAME_STATE (SUPOX_HOSTILE)
		|| LOBYTE (GLOBAL (CurrentActivity)) == WON_LAST_BATTLE)
	{
	    SET_GAME_STATE (BATTLE_SEGUE, 0);
	}
	else
	{
	    SET_GAME_STATE (BATTLE_SEGUE, 1);
	}
	retval = &supox_desc;
    }
    POP_CONTEXT

    return (retval);
}
