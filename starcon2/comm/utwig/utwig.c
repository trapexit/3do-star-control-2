#include "reslib.h"
#include "resinst.h"
#include "starcon.h"
#include "commglue.h"
#include "strings.h"

static LOCDATA	utwig_desc =
{
    NULL_PTR,				/* init_encounter_func */
    NULL_PTR,				/* uninit_encounter_func */
    (FRAME)UTWIG_PMAP_ANIM,		/* AlienFrame */
    0,					/* AlienColorMap */
    UTWIG_MUSIC,			/* AlienSong */
    UTWIG_PLAYER_PHRASES,		/* PlayerPhrases */
    16,					/* NumAnimations */
    {
	{
	    4,				/* StartIndex */
	    3,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    8, 4,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    7,				/* StartIndex */
	    4,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    8, 4,			/* FrameRate */
	    ONE_SECOND * 10, ONE_SECOND * 3,	/* RestartRate */
	    (1 << 2),			/* BlockMask */
	},
	{
	    11,				/* StartIndex */
	    2,				/* NumFrames */
	    YOYO_ANIM
		    | WAIT_TALKING,	/* AnimFlags */
	    16, 4,			/* FrameRate */
	    ONE_SECOND * 10, ONE_SECOND * 3,	/* RestartRate */
	    (1 << 1),			/* BlockMask */
	},
	{
	    13,				/* StartIndex */
	    5,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    10, 0,			/* FrameRate */
	    10, 0,			/* RestartRate */
	},
	{
	    18,				/* StartIndex */
	    2,				/* NumFrames */
	    RANDOM_ANIM,		/* AnimFlags */
	    16, 4,			/* FrameRate */
	    ONE_SECOND * 10, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    20,				/* StartIndex */
	    2,				/* NumFrames */
	    RANDOM_ANIM,		/* AnimFlags */
	    16, 4,			/* FrameRate */
	    ONE_SECOND * 10, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    22,				/* StartIndex */
	    3,				/* NumFrames */
	    RANDOM_ANIM,		/* AnimFlags */
	    16, 4,			/* FrameRate */
	    ONE_SECOND * 10, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    25,				/* StartIndex */
	    2,				/* NumFrames */
	    RANDOM_ANIM,		/* AnimFlags */
	    16, 4,			/* FrameRate */
	    ONE_SECOND * 10, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    27,				/* StartIndex */
	    3,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    16, 4,			/* FrameRate */
	    ONE_SECOND * 10, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    30,				/* StartIndex */
	    2,				/* NumFrames */
	    RANDOM_ANIM,		/* AnimFlags */
	    16, 4,			/* FrameRate */
	    ONE_SECOND * 10, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    32,				/* StartIndex */
	    2,				/* NumFrames */
	    RANDOM_ANIM,		/* AnimFlags */
	    16, 4,			/* FrameRate */
	    ONE_SECOND * 10, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    34,				/* StartIndex */
	    2,				/* NumFrames */
	    RANDOM_ANIM,		/* AnimFlags */
	    16, 4,			/* FrameRate */
	    ONE_SECOND * 10, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    36,				/* StartIndex */
	    2,				/* NumFrames */
	    RANDOM_ANIM,		/* AnimFlags */
	    16, 4,			/* FrameRate */
	    ONE_SECOND * 10, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    38,				/* StartIndex */
	    2,				/* NumFrames */
	    RANDOM_ANIM,		/* AnimFlags */
	    16, 4,			/* FrameRate */
	    ONE_SECOND * 10, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    40,				/* StartIndex */
	    2,				/* NumFrames */
	    RANDOM_ANIM,		/* AnimFlags */
	    16, 4,			/* FrameRate */
	    ONE_SECOND * 10, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    42,				/* StartIndex */
	    3,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    16, 4,			/* FrameRate */
	    ONE_SECOND * 10, ONE_SECOND * 3,	/* RestartRate */
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
	6, 6,				/* FrameRate */
	14, ONE_SECOND / 2,		/* RestartRate */
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
	else if (PLAYER_SAID (R, bye_after_space))
	    NPCPhrase (GOODBYE_AFTER_SPACE);
	else if (PLAYER_SAID (R, bye_before_space))
	    NPCPhrase (GOODBYE_BEFORE_SPACE);
	else if (PLAYER_SAID (R, bye_allied_homeworld))
	    NPCPhrase (GOODBYE_ALLIED_HOMEWORLD);
	else if (PLAYER_SAID (R, bye_bomb))
	    NPCPhrase (GOODBYE_BOMB);
	else if (PLAYER_SAID (R, demand_bomb))
	{
	    NPCPhrase (GUARDS_FIGHT);

	    SET_GAME_STATE (BATTLE_SEGUE, 1);
	}
	else if (PLAYER_SAID (R, got_ultron)
		|| PLAYER_SAID (R, hey_wait_got_ultron))
	{
	    if (GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 6))
	    {
	        NPCPhrase (NO_ULTRON_AT_BOMB);

		SET_GAME_STATE (REFUSED_ULTRON_AT_BOMB, 1);
	    }
	    else
	    {
		if (PLAYER_SAID (R, got_ultron))
		    NPCPhrase (DONT_WANT_TO_LOOK);
		else
		    NPCPhrase (TAUNT_US_BUT_WE_LOOK);
		if (GET_GAME_STATE (ULTRON_CONDITION) < 4)
		{
		    switch (GET_GAME_STATE (UTWIG_INFO))
		    {
			case 0:
			    if (PLAYER_SAID (R, got_ultron))
				NPCPhrase (SICK_TRICK_1);
			    else
			    {
				NPCPhrase (TRICKED_US_1);

				SET_GAME_STATE (BATTLE_SEGUE, 1);
			    }
			    break;
			case 1:
			    if (PLAYER_SAID (R, got_ultron))
				NPCPhrase (SICK_TRICK_2);
			    else
			    {
				NPCPhrase (TRICKED_US_2);

				SET_GAME_STATE (BATTLE_SEGUE, 1);
			    }
			    break;
		    }
		    SET_GAME_STATE (UTWIG_INFO, 1);
		}
		else
		{
		    NPCPhrase (HAPPY_DAYS);
		    if (GET_GAME_STATE (KOHR_AH_FRENZY))
			NPCPhrase (TOO_LATE);
		    else
		    {
			NPCPhrase (OK_ATTACK_KOHRAH);

			AddEvent (RELATIVE_EVENT, 0, 0, 0, ADVANCE_UTWIG_SUPOX_MISSION);
		    }

		    SET_GAME_STATE (UTWIG_HAVE_ULTRON, 1);
		    SET_GAME_STATE (ULTRON_CONDITION, 5);

		    SET_GAME_STATE (UTWIG_VISITS, 0);
		    SET_GAME_STATE (SUPOX_VISITS, 0);
		    SET_GAME_STATE (UTWIG_HOME_VISITS, 0);
		    SET_GAME_STATE (SUPOX_HOME_VISITS, 0);
		    SET_GAME_STATE (BOMB_VISITS, 0);

		    SET_GAME_STATE (SUPOX_INFO, 0);
		    SET_GAME_STATE (UTWIG_INFO, 0);
		    SET_GAME_STATE (SUPOX_WAR_NEWS, 0);
		    SET_GAME_STATE (UTWIG_WAR_NEWS, 0);
		    SET_GAME_STATE (SUPOX_HOSTILE, 0);
		    SET_GAME_STATE (UTWIG_HOSTILE, 0);

		    ActivateStarShip (UTWIG_SHIP, 0);
		    ActivateStarShip (SUPOX_SHIP, 0);
		}
	    }
	}
	else if (PLAYER_SAID (R, can_you_help))
	{
	    NPCPhrase (HOW_HELP);
	    if (ActivateStarShip (UTWIG_SHIP, FEASIBILITY_STUDY) == 0)
		NPCPhrase (DONT_NEED);
	    else
	    {
		NPCPhrase (HAVE_4_SHIPS);

		AlienTalkSegue ((COUNT)~0);
		ActivateStarShip (UTWIG_SHIP, 4);
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

	News = GET_GAME_STATE (UTWIG_WAR_NEWS);
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

		SET_GAME_STATE (UTWIG_WAR_NEWS, 1);
	    }
	    else switch (GET_GAME_STATE (UTWIG_WAR_NEWS))
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
	SET_GAME_STATE (UTWIG_WAR_NEWS, News);

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
	    NumVisits = GET_GAME_STATE (UTWIG_INFO);
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
	    SET_GAME_STATE (UTWIG_INFO, NumVisits);

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
	    NumVisits = GET_GAME_STATE (UTWIG_INFO);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (GEN_INFO_AFTER_SPACE_1);
		    break;
		case 1:
		    NPCPhrase (GEN_INFO_AFTER_SPACE_2);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (UTWIG_INFO, NumVisits);

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
void far NeutralUtwig, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	BYTE		i, LastStack;
	RESPONSE_REF	pStr[4];

	LastStack = 0;
	pStr[0] = pStr[1] = pStr[2] = pStr[3] = 0;
	if (PLAYER_SAID (R, we_are_vindicator0))
	{
	    NPCPhrase (WOULD_BE_HAPPY_BUT);

	    SET_GAME_STATE (UTWIG_STACK1, 1);
	}
	else if (PLAYER_SAID (R, why_sad))
	{
	    NPCPhrase (ULTRON_BROKE);

	    SET_GAME_STATE (UTWIG_STACK1, 2);
	}
	else if (PLAYER_SAID (R, what_ultron))
	{
	    NPCPhrase (GLORIOUS_ULTRON);

	    SET_GAME_STATE (UTWIG_STACK1, 3);
	}
	else if (PLAYER_SAID (R, dont_be_babies))
	{
	    NPCPhrase (MOCK_OUR_PAIN);

	    SET_GAME_STATE (BATTLE_SEGUE, 1);
	    SET_GAME_STATE (UTWIG_STACK1, 4);
	    SET_GAME_STATE (UTWIG_HOSTILE, 1);
	    SET_GAME_STATE (UTWIG_INFO, 0);
	    SET_GAME_STATE (UTWIG_HOME_VISITS, 0);
	    SET_GAME_STATE (UTWIG_VISITS, 0);
	    SET_GAME_STATE (BOMB_VISITS, 0);
	    goto ExitUtwig;
	}
	else if (PLAYER_SAID (R, real_sorry_about_ultron))
	{
	    NPCPhrase (APPRECIATE_SYMPATHY);

	    SET_GAME_STATE (UTWIG_STACK1, 4);
	    goto ExitUtwig;
	}
	else if (PLAYER_SAID (R, what_about_you_1))
	{
	    NPCPhrase (ABOUT_US_1);

	    LastStack = 2;
	    SET_GAME_STATE (UTWIG_WAR_NEWS, 1);
	}
	else if (PLAYER_SAID (R, what_about_you_2))
	{
	    NPCPhrase (ABOUT_US_2);

	    LastStack = 2;
	    SET_GAME_STATE (UTWIG_WAR_NEWS, 2);
	}
	else if (PLAYER_SAID (R, what_about_you_3))
	{
	    NPCPhrase (ABOUT_US_3);
	    
	    SET_GAME_STATE (UTWIG_WAR_NEWS, 3);
	}
	else if (PLAYER_SAID (R, what_about_urquan_1))
	{
	    NPCPhrase (ABOUT_URQUAN_1);

	    LastStack = 3;
	    SET_GAME_STATE (UTWIG_STACK2, 1);
	}
	else if (PLAYER_SAID (R, what_about_urquan_2))
	{
	    NPCPhrase (ABOUT_URQUAN_2);

	    SET_GAME_STATE (UTWIG_STACK2, 2);
	}

	switch (GET_GAME_STATE (UTWIG_STACK1))
	{
	    case 0:
		{
		    char	buf[80];

		    GetAllianceName (buf, name_1);
		    construct_response (
			    shared_phrase_buf,
			    we_are_vindicator0,
			    GLOBAL_SIS (CommanderName),
			    we_are_vindicator1,
			    buf,
			    we_are_vindicator2,
			    0
			    );
		}
		pStr[0] = we_are_vindicator0;
		break;
	    case 1:
		pStr[0] = why_sad;
		break;
	    case 2:
		pStr[0] = what_ultron;
		break;
	    case 3:
		pStr[0] = dont_be_babies;
		pStr[1] = real_sorry_about_ultron;
		break;
	}
	switch (GET_GAME_STATE (UTWIG_WAR_NEWS))
	{
	    case 0:
		pStr[2] = what_about_you_1;
		break;
	    case 1:
		pStr[2] = what_about_you_2;
		break;
	    case 2:
		pStr[2] = what_about_you_3;
		break;
	}
	switch (GET_GAME_STATE (UTWIG_STACK2))
	{
	    case 0:
		pStr[2] = what_about_urquan_1;
		break;
	    case 1:
		pStr[2] = what_about_urquan_2;
		break;
	}

	if (pStr[LastStack])
	{
	    if (pStr[LastStack] != we_are_vindicator0)
		Response (pStr[LastStack], NeutralUtwig);
	    else
		DoResponsePhrase (pStr[LastStack], NeutralUtwig, shared_phrase_buf);
	}
	for (i = 0; i < 4; ++i)
	{
	    if (i != LastStack && pStr[i])
	    {
		if (pStr[i] != we_are_vindicator0)
		    Response (pStr[i], NeutralUtwig);
		else
		    DoResponsePhrase (pStr[i], NeutralUtwig, shared_phrase_buf);
	    }
	}
	if (GET_GAME_STATE (ULTRON_CONDITION))
	    Response (got_ultron, ExitConversation);
	Response (bye_neutral, ExitConversation);
ExitUtwig:
	;
    }
    POP_CONTEXT
}

PROC(STATIC
void far BombWorld, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	BYTE		LastStack;
	RESPONSE_REF		pStr[2];

	LastStack = 0;
	pStr[0] = pStr[1] = 0;
	if (PLAYER_SAID (R, why_you_here))
	{
	    NPCPhrase (WE_GUARD_BOMB);

	    SET_GAME_STATE (BOMB_STACK1, 1);
	}
	else if (PLAYER_SAID (R, what_about_bomb))
	{
	    NPCPhrase (ABOUT_BOMB);

	    SET_GAME_STATE (BOMB_STACK1, 2);
	}
	else if (PLAYER_SAID (R, give_us_bomb_or_die))
	{
	    NPCPhrase (GUARDS_WARN);

	    SET_GAME_STATE (BOMB_STACK1, 3);
	}
	else if (PLAYER_SAID (R, demand_bomb))
	{
	    NPCPhrase (GUARDS_FIGHT);

	    SET_GAME_STATE (BATTLE_SEGUE, 1);
	    SET_GAME_STATE (UTWIG_HOSTILE, 1);
	    SET_GAME_STATE (UTWIG_INFO, 0);
	    SET_GAME_STATE (UTWIG_HOME_VISITS, 0);
	    SET_GAME_STATE (UTWIG_VISITS, 0);
	    SET_GAME_STATE (BOMB_VISITS, 0);
	    goto ExitBomb;
	}
	else if (PLAYER_SAID (R, may_we_have_bomb))
	{
	    NPCPhrase (NO_BOMB);

	    LastStack = 1;
	    SET_GAME_STATE (BOMB_STACK2, 1);
	}
	else if (PLAYER_SAID (R, please))
	{
	    NPCPhrase (SORRY_NO_BOMB);

	    SET_GAME_STATE (BOMB_STACK2, 2);
	}
	else if (PLAYER_SAID (R, whats_up_bomb))
	{
	    if (GET_GAME_STATE (BOMB_INFO))
		NPCPhrase (GENERAL_INFO_BOMB_2);
	    else
	    {
		NPCPhrase (GENERAL_INFO_BOMB_1);

		SET_GAME_STATE (BOMB_INFO, 1);
	    }

	    DISABLE_PHRASE (whats_up_bomb);
	}

	switch (GET_GAME_STATE (BOMB_STACK2))
	{
	    case 0:
		pStr[1] = may_we_have_bomb;
		break;
	    case 1:
		pStr[1] = please;
		break;
	}
	switch (GET_GAME_STATE (BOMB_STACK1))
	{
	    case 0:
		pStr[0] = why_you_here;
		pStr[1] = 0;
		break;
	    case 1:
		pStr[0] = what_about_bomb;
		pStr[1] = 0;
		break;
	    case 2:
		pStr[0] = give_us_bomb_or_die;
		break;
	    case 3:
		pStr[0] = demand_bomb;
		break;
	}

	if (pStr[LastStack])
	    Response (pStr[LastStack], BombWorld);
	LastStack ^= 1;
	if (pStr[LastStack])
	    Response (pStr[LastStack], BombWorld);
	if (PHRASE_ENABLED (whats_up_bomb))
	    Response (whats_up_bomb, BombWorld);
	if (GET_GAME_STATE (ULTRON_CONDITION)
		&& !GET_GAME_STATE (REFUSED_ULTRON_AT_BOMB))
	    Response (got_ultron, ExitConversation);
	Response (bye_bomb, ExitConversation);
ExitBomb:
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

	if (GET_GAME_STATE (UTWIG_HOSTILE))
	{
	    if (GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 6))
	    {
		NumVisits = GET_GAME_STATE (BOMB_VISITS);
		switch (NumVisits++)
		{
		    case 0:
			NPCPhrase (HOSTILE_BOMB_HELLO_1);
			break;
		    case 1:
			NPCPhrase (HOSTILE_BOMB_HELLO_2);
			--NumVisits;
			break;
		}
		SET_GAME_STATE (BOMB_VISITS, NumVisits);
	    }
	    else if (GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 7))
	    {
		NumVisits = GET_GAME_STATE (UTWIG_HOME_VISITS);
		switch (NumVisits++)
		{
		    case 0:
			NPCPhrase (HOSTILE_HOMEWORLD_HELLO_1);
			break;
		    case 1:
			NPCPhrase (HOSTILE_HOMEWORLD_HELLO_2);
			--NumVisits;
			break;
		}
		SET_GAME_STATE (UTWIG_HOME_VISITS, NumVisits);
	    }
	    else
	    {
		NumVisits = GET_GAME_STATE (UTWIG_VISITS);
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
		SET_GAME_STATE (UTWIG_VISITS, NumVisits);
	    }

	    if (!GET_GAME_STATE (ULTRON_CONDITION)
		    || (GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 6)))
	    {
		SET_GAME_STATE (BATTLE_SEGUE, 1);
	    }
	    else
	    {
		Response (hey_wait_got_ultron, ExitConversation);
	    }
	}
	else if (ActivateStarShip (UTWIG_SHIP, CHECK_ALLIANCE) & GOOD_GUY)
	{
	    if (GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 7))
	    {
		NumVisits = GET_GAME_STATE (UTWIG_HOME_VISITS);
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
		SET_GAME_STATE (UTWIG_HOME_VISITS, NumVisits);

		AlliedHome ((RESPONSE_REF)0);
	    }
	    else
	    {
		NumVisits = GET_GAME_STATE (UTWIG_SUPOX_MISSION);
		if (NumVisits == 1)
		{
		    NumVisits = GET_GAME_STATE (UTWIG_VISITS);
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
		    SET_GAME_STATE (UTWIG_VISITS, NumVisits);

		    BeforeKohrAh ((RESPONSE_REF)0);
		}
		else if (NumVisits < 5)
		{
		    NumVisits = GET_GAME_STATE (UTWIG_VISITS);
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
		    SET_GAME_STATE (UTWIG_VISITS, NumVisits);
		}
		else
		{
		    NumVisits = GET_GAME_STATE (UTWIG_VISITS);
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
		    SET_GAME_STATE (UTWIG_VISITS, NumVisits);

		    AfterKohrAh ((RESPONSE_REF)0);
		}
	    }
	}
	else if (GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 6))
	{
	    NumVisits = GET_GAME_STATE (BOMB_VISITS);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (BOMB_WORLD_HELLO_1);
		    break;
		case 1:
		    NPCPhrase (BOMB_WORLD_HELLO_2);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (BOMB_VISITS, NumVisits);

	    BombWorld ((RESPONSE_REF)0);
	}
	else
	{
	    if (GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 7))
	    {
		NumVisits = GET_GAME_STATE (UTWIG_HOME_VISITS);
		switch (NumVisits++)
		{
		    case 0:
			NPCPhrase (NEUTRAL_HOMEWORLD_HELLO_1);
			break;
		    case 1:
			NPCPhrase (NEUTRAL_HOMEWORLD_HELLO_2);
			break;
		    case 2:
			NPCPhrase (NEUTRAL_HOMEWORLD_HELLO_3);
			break;
		    case 3:
			NPCPhrase (NEUTRAL_HOMEWORLD_HELLO_4);
			--NumVisits;
			break;
		}
		SET_GAME_STATE (UTWIG_HOME_VISITS, NumVisits);
	    }
	    else
	    {
		NumVisits = GET_GAME_STATE (UTWIG_VISITS);
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
		SET_GAME_STATE (UTWIG_VISITS, NumVisits);
	    }

	    NeutralUtwig ((RESPONSE_REF)0);
	}
ExitIntro:
	;
    }
    POP_CONTEXT
}

PROC(STATIC
COUNT far uninit_utwig, (),
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
LOCDATAPTR far init_utwig_comm, (),
    ARG_VOID
)
{
    LOCDATAPTR	retval;

    PUSH_CONTEXT
    {
	utwig_desc.init_encounter_func = Intro;
	utwig_desc.uninit_encounter_func = uninit_utwig;

strcpy(aiff_folder, "comm/utwig/utwig");

	if (GET_GAME_STATE (UTWIG_HAVE_ULTRON)
		|| LOBYTE (GLOBAL (CurrentActivity)) == WON_LAST_BATTLE)
	{
	    SET_GAME_STATE (BATTLE_SEGUE, 0);
	}
	else
	{
	    SET_GAME_STATE (BATTLE_SEGUE, 1);
	}
	retval = &utwig_desc;
    }
    POP_CONTEXT

    return (retval);
}
