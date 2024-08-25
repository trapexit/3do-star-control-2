#include "reslib.h"
#include "resinst.h"
#include "starcon.h"
#include "commglue.h"
#include "strings.h"

static LOCDATA	thradd_desc =
{
    NULL_PTR,				/* init_encounter_func */
    NULL_PTR,				/* uninit_encounter_func */
    (FRAME)THRADD_PMAP_ANIM,		/* AlienFrame */
    0,					/* AlienColorMap */
    THRADD_MUSIC,			/* AlienSong */
    THRADD_PLAYER_PHRASES,		/* PlayerPhrases */
    8,					/* NumAnimations */
    {
	{
	    8,				/* StartIndex */
	    4,				/* NumFrames */
	    RANDOM_ANIM,		/* AnimFlags */
	    8, 8,			/* FrameRate */
	    8, 8,			/* RestartRate */
	    (1 << 4),			/* BlockMask */
	},
	{
	    12,				/* StartIndex */
	    9,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    6, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    21,				/* StartIndex */
	    6,				/* NumFrames */
	    RANDOM_ANIM,		/* AnimFlags */
	    6, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    27,				/* StartIndex */
	    3,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    6, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	    (1 << 4),			/* BlockMask */
	},
	{
	    30,				/* StartIndex */
	    12,				/* NumFrames */
	    CIRCULAR_ANIM
		    | WAIT_TALKING,	/* AnimFlags */
	    10, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND,	/* RestartRate */
	    (1 << 0) | (1 << 3) | (1 << 5),
	},
	{
	    42,				/* StartIndex */
	    5,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    12, 4,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	    (1 << 4) | (1 << 6),	/* BlockMask */
	},
	{
	    47,				/* StartIndex */
	    5,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    12, 4,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	    (1 << 5),			/* BlockMask */
	},
	{
	    52,				/* StartIndex */
	    4,				/* NumFrames */
	    RANDOM_ANIM,		/* AnimFlags */
	    6, 0,			/* FrameRate */
	    6, 0,			/* RestartRate */
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
	7,				/* NumFrames */
	0,				/* AnimFlags */
	8, 0,				/* FrameRate */
	10, 0,				/* RestartRate */
    },
};

PROC(STATIC
int near GetCultureName, (),
    ARG_VOID
)
{
    int	culture;

    switch (GET_GAME_STATE (THRADD_CULTURE))
    {
	case 1:
	    culture = CULTURE;
	    break;
	case 2:
	    culture = FAT_JERKS;
	    break;
	case 3:
	    culture = SLAVE_EMPIRE;
	    break;
    }
    
    return (culture);
}

PROC(STATIC
void near PolitePhrase, (which_phrase),
    ARG_END	(BYTE	which_phrase)
)
{
    switch (which_phrase)
    {
	case 0:
	    NPCPhrase (HELLO_POLITE_1);
	    break;
	case 1:
	    NPCPhrase (HELLO_POLITE_2);
	    break;
	case 2:
	    NPCPhrase (HELLO_POLITE_3);
	    break;
	case 3:
	    NPCPhrase (HELLO_POLITE_4);
	    break;
    }
}

PROC(STATIC
void near RhymePhrase, (which_phrase),
    ARG_END	(BYTE	which_phrase)
)
{
    switch (which_phrase)
    {
	case 0:
	    NPCPhrase (HELLO_RHYME_1);
	    break;
	case 1:
	    NPCPhrase (HELLO_RHYME_2);
	    break;
	case 2:
	    NPCPhrase (HELLO_RHYME_3);
	    break;
	case 3:
	    NPCPhrase (HELLO_RHYME_4);
	    break;
    }
}

PROC(STATIC
void near PigLatinPhrase, (which_phrase),
    ARG_END	(BYTE	which_phrase)
)
{
    switch (which_phrase)
    {
	case 0:
	    NPCPhrase (HELLO_PIG_LATIN_1);
	    break;
	case 1:
	    NPCPhrase (HELLO_PIG_LATIN_2);
	    break;
	case 2:
	    NPCPhrase (HELLO_PIG_LATIN_3);
	    break;
	case 3:
	    NPCPhrase (HELLO_PIG_LATIN_4);
	    break;
    }
}

PROC(STATIC
void near LikeYouPhrase, (which_phrase),
    ARG_END	(BYTE	which_phrase)
)
{
    switch (which_phrase)
    {
	case 0:
	    NPCPhrase (HELLO_LIKE_YOU_1);
	    break;
	case 1:
	    NPCPhrase (HELLO_LIKE_YOU_2);
	    break;
	case 2:
	    NPCPhrase (HELLO_LIKE_YOU_3);
	    break;
	case 3:
	    NPCPhrase (HELLO_LIKE_YOU_4);
	    break;
    }
}

PROC(STATIC
void far ExitConversation, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	SET_GAME_STATE (BATTLE_SEGUE, 1);

	if (PLAYER_SAID (R, bye_hostile_2))
	    NPCPhrase (GOODBYE_HOSTILE_2);
	else if (PLAYER_SAID (R, bye_hostile_1))
	{
	    NPCPhrase (GOODBYE_HOSTILE_1);

	    SET_GAME_STATE (THRADD_HOSTILE_STACK_5, 1);
	}
	else if (PLAYER_SAID (R, submit_1))
	{
	    NPCPhrase (NO_SUBMIT_1);

	    SET_GAME_STATE (THRADD_HOSTILE_STACK_2, 1);
	}
	else if (PLAYER_SAID (R, submit_2))
	    NPCPhrase (NO_SUBMIT_2);
	else if (PLAYER_SAID (R, got_idea))
	{
	    NPCPhrase (GOOD_IDEA);

	    SET_GAME_STATE (BATTLE_SEGUE, 0);
	    AddEvent (RELATIVE_EVENT, 0, 0, 0, ADVANCE_THRADD_MISSION);
	    SET_GAME_STATE (THRADD_STACK_1, 5);
	}
	else if (PLAYER_SAID (R, bye_hostile_helix))
	    NPCPhrase (GOODBYE_HOSTILE_HELIX);
	else if (PLAYER_SAID (R, bye_ally))
	{
	    BYTE	NumVisits;

	    NumVisits = GET_GAME_STATE (THRADD_STACK_1);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (GOODBYE_ALLY_1);
		    break;
		case 1:
		    NPCPhrase (GOODBYE_ALLY_2);
		    break;
		case 2:
		    NPCPhrase (GOODBYE_ALLY_3);
		    break;
		case 3:
		    NPCPhrase (GOODBYE_ALLY_4);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (THRADD_STACK_1, NumVisits);
	    SET_GAME_STATE (BATTLE_SEGUE, 0);
	}
	else if (PLAYER_SAID (R, may_i_land))
	{
	    NPCPhrase (SURE_LAND);

	    SET_GAME_STATE (HELIX_UNPROTECTED, 1);
	    SET_GAME_STATE (BATTLE_SEGUE, 0);
	}
	else if (PLAYER_SAID (R, demand_to_land))
	    NPCPhrase (NO_DEMAND);
	else if (PLAYER_SAID (R, i_need_to_land_lie))
	    NPCPhrase (CAUGHT_LIE);
	else
	{
	    if (PLAYER_SAID (R, contemplative))
	    {
		NPCPhrase (OK_CONTEMPLATIVE);

		SET_GAME_STATE (THRADD_DEMEANOR, 0);
	    }
	    else if (PLAYER_SAID (R, friendly))
	    {
		NPCPhrase (OK_FRIENDLY);

		SET_GAME_STATE (THRADD_DEMEANOR, 1);
	    }
	    else if (PLAYER_SAID (R, wacky))
	    {
		NPCPhrase (OK_WACKY);

		SET_GAME_STATE (THRADD_DEMEANOR, 2);
	    }
	    else if (PLAYER_SAID (R, just_like_us))
	    {
		NPCPhrase (OK_JUST_LIKE_YOU);

		SET_GAME_STATE (THRADD_DEMEANOR, 3);
	    }
	    NPCPhrase (WORK_TO_DO);

	    SET_GAME_STATE (BATTLE_SEGUE, 0);
	}
    }
    POP_CONTEXT
}

PROC(STATIC
void far ThraddAllies, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	BYTE		NumVisits;

	if (PLAYER_SAID (R, why_you_here_ally))
	{
	    NPCPhrase (GUARDING_HELIX_ALLY);

	    DISABLE_PHRASE (why_you_here_ally);
	}
	else if (PLAYER_SAID (R, whats_helix_ally))
	{
	    NPCPhrase (HELIX_IS_ALLY);

	    DISABLE_PHRASE (whats_helix_ally);
	}
	else if (PLAYER_SAID (R, whats_up_ally))
	{
	    NumVisits = GET_GAME_STATE (THRADD_INFO);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (GENERAL_INFO_ALLY_1);
		    break;
		case 1:
		    NPCPhrase (GENERAL_INFO_ALLY_2);
		    break;
		case 2:
		    NPCPhrase (GENERAL_INFO_ALLY_3);
		    break;
		case 3:
		    NPCPhrase (GENERAL_INFO_ALLY_4);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (THRADD_INFO, NumVisits);

	    DISABLE_PHRASE (whats_up_ally);
	}
	else if (PLAYER_SAID (R, how_goes_culture))
	{
	    NumVisits = GET_GAME_STATE (THRADD_DEMEANOR);
	    switch (NumVisits & ((1 << 2) - 1))
	    {
		case 0:
		    if (!(NumVisits & ~((1 << 2) - 1)))
			NPCPhrase (CONTEMP_GOES_1);
		    else
			NPCPhrase (CONTEMP_GOES_2);
		    break;
		case 1:
		    if (!(NumVisits & ~((1 << 2) - 1)))
			NPCPhrase (FRIENDLY_GOES_1);
		    else
			NPCPhrase (FRIENDLY_GOES_2);
		    break;
		case 2:
		    if (!(NumVisits & ~((1 << 2) - 1)))
			NPCPhrase (WACKY_GOES_1);
		    else
			NPCPhrase (WACKY_GOES_2);
		    break;
		case 3:
		    if (!(NumVisits & ~((1 << 2) - 1)))
			NPCPhrase (LIKE_YOU_GOES_1);
		    else
			NPCPhrase (LIKE_YOU_GOES_2);
		    break;
	    }
	    NumVisits |= 1 << 2;
	    SET_GAME_STATE (THRADD_DEMEANOR, NumVisits);

	    DISABLE_PHRASE (how_goes_culture);
	}

	if (GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 6))
	{
	    if (PHRASE_ENABLED (why_you_here_ally))
		Response (why_you_here_ally, ThraddAllies);
	    else
	    {
		if (PHRASE_ENABLED (whats_helix_ally))
		    Response (whats_helix_ally, ThraddAllies);
		Response (may_i_land, ExitConversation);
	    }
	}
	if (PHRASE_ENABLED (whats_up_ally))
	    Response (whats_up_ally, ThraddAllies);
	if (PHRASE_ENABLED (how_goes_culture))
	    Response (how_goes_culture, ThraddAllies);
	Response (bye_ally, ExitConversation);
    }
    POP_CONTEXT
}

PROC(STATIC
void far ThraddDemeanor, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	if (PLAYER_SAID (R, you_decide))
	{
	    NPCPhrase (OK_CULTURE_20);

	    SET_GAME_STATE (THRADD_CULTURE, 1);
	}
	else if (PLAYER_SAID (R, fat))
	{
	    NPCPhrase (OK_FAT);

	    SET_GAME_STATE (THRADD_CULTURE, 2);
	}
	else if (PLAYER_SAID (R, the_slave_empire))
	{
	    SET_GAME_STATE (THRADD_CULTURE, 3);

	    NPCPhrase (OK_SLAVE);
	}

	NPCPhrase (HOW_SHOULD_WE_ACT);
	Response (contemplative, ExitConversation);
	Response (friendly, ExitConversation);
	Response (wacky, ExitConversation);
	Response (just_like_us, ExitConversation);
    }
    POP_CONTEXT
}

PROC(STATIC
void far ThraddCulture, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	if (PLAYER_SAID (R, be_polite))
	{
	    NPCPhrase (OK_POLITE);

	    SET_GAME_STATE (THRADD_INTRO, 0);
	}
	else if (PLAYER_SAID (R, use_rhymes))
	{
	    NPCPhrase (OK_RHYMES);

	    SET_GAME_STATE (THRADD_INTRO, 1);
	}
	else if (PLAYER_SAID (R, speak_pig_latin))
	{
	    NPCPhrase (OK_PIG_LATIN);

	    SET_GAME_STATE (THRADD_INTRO, 2);
	}
	else if (PLAYER_SAID (R, just_the_way_we_do))
	{
	    NPCPhrase (OK_WAY_YOU_DO);

	    SET_GAME_STATE (THRADD_INTRO, 3);
	}
	NPCPhrase (WHAT_NAME_FOR_CULTURE);

	Response (you_decide, ThraddDemeanor);
	Response (fat, ThraddDemeanor);
	Response (the_slave_empire, ThraddDemeanor);
    }
    POP_CONTEXT
}

PROC(STATIC
void far ThraddWorship, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	SET_GAME_STATE (THRADD_VISITS, 0);
	SET_GAME_STATE (THRADD_MANNER, 1);
	SET_GAME_STATE (THRADD_STACK_1, 0);
	ActivateStarShip (THRADDASH_SHIP, 0);

	Response (be_polite, ThraddCulture);
	Response (speak_pig_latin, ThraddCulture);
	Response (use_rhymes, ThraddCulture);
	Response (just_the_way_we_do, ThraddCulture);
    }
    POP_CONTEXT
}

PROC(STATIC
void far HelixWorld, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	if (PLAYER_SAID (R, why_you_here_hostile))
	{
	    NPCPhrase (NONE_OF_YOUR_CONCERN);

	    SET_GAME_STATE (THRADD_CULTURE, 1);
	}
	else if (PLAYER_SAID (R, what_about_this_world))
	{
	    NPCPhrase (BLUE_HELIX);

	    SET_GAME_STATE (THRADD_INTRO, 1);
	}
	else if (PLAYER_SAID (R, whats_helix_hostile))
	{
	    NPCPhrase (HELIX_IS_HOSTILE);

	    SET_GAME_STATE (THRADD_INTRO, 2);
	}
	else if (PLAYER_SAID (R, i_need_to_land_lie))
	{
	    NPCPhrase (CAUGHT_LIE);

	    SET_GAME_STATE (THRADD_DEMEANOR, 1);
	}

	if (!GET_GAME_STATE (THRADD_CULTURE))
	    Response (why_you_here_hostile, HelixWorld);
	else
	{
	    Response (demand_to_land, ExitConversation);
	}
	switch (GET_GAME_STATE (THRADD_INTRO))
	{
	    case 0:
		Response (what_about_this_world, HelixWorld);
		break;
	    case 1:
		Response (whats_helix_hostile, HelixWorld);
		break;
	}
	if (!GET_GAME_STATE (THRADD_DEMEANOR))
	{
	    Response (i_need_to_land_lie, ExitConversation);
	}
	Response (bye_hostile_helix, ExitConversation);
    }
    POP_CONTEXT
}

PROC(STATIC
void far ThraddHostile, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	if (PLAYER_SAID (R, whats_up_hostile_1))
	{
	    NPCPhrase (GENERAL_INFO_HOSTILE_1);

	    SET_GAME_STATE (THRADD_INFO, 1);
	    DISABLE_PHRASE (whats_up_hostile_2);
	}
	else if (PLAYER_SAID (R, whats_up_hostile_2))
	{
	    BYTE	NumVisits;

	    NumVisits = GET_GAME_STATE (THRADD_INFO);
	    switch (NumVisits++)
	    {
		case 1:
		    NPCPhrase (GENERAL_INFO_HOSTILE_2);
		    break;
		case 2:
		    NPCPhrase (GENERAL_INFO_HOSTILE_3);
		    break;
		case 3:
		    NPCPhrase (GENERAL_INFO_HOSTILE_4);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (THRADD_INFO, NumVisits);

	    DISABLE_PHRASE (whats_up_hostile_2);
	}
	else if (PLAYER_SAID (R, what_about_you_1))
	{
	    NPCPhrase (ABOUT_US_1);

	    SET_GAME_STATE (THRADD_STACK_1, 1);
	}
	else if (PLAYER_SAID (R, what_about_you_2))
	{
	    NPCPhrase (ABOUT_US_2);

	    SET_GAME_STATE (THRADD_STACK_1, 2);
	}
	else if (PLAYER_SAID (R, what_about_urquan_1))
	{
	    NPCPhrase (ABOUT_URQUAN_1);

	    SET_GAME_STATE (THRADD_STACK_1, 3);
	}
	else if (PLAYER_SAID (R, what_about_urquan_2))
	{
	    NPCPhrase (ABOUT_URQUAN_2);

	    SET_GAME_STATE (THRADD_STACK_1, 4);
	}
	else if (PLAYER_SAID (R, be_friends_1))
	{
	    NPCPhrase (NO_FRIENDS_1);

	    SET_GAME_STATE (THRADD_HOSTILE_STACK_3, 1);
	}
	else if (PLAYER_SAID (R, be_friends_2))
	{
	    NPCPhrase (NO_FRIENDS_2);
	    DISABLE_PHRASE (be_friends_2);
	}
	else if (PLAYER_SAID (R, how_impressed_urquan_1))
	{
	    NPCPhrase (IMPRESSED_LIKE_SO_1);

	    SET_GAME_STATE (THRADD_HOSTILE_STACK_4, 1);
	}
	else if (PLAYER_SAID (R, how_impressed_urquan_2))
	{
	    NPCPhrase (IMPRESSED_LIKE_SO_2);

	    SET_GAME_STATE (THRADD_MISSION, 5);
	}

	if (GET_GAME_STATE (THRADD_INFO) == 0)
	    Response (whats_up_hostile_1, ThraddHostile);
	else if (PHRASE_ENABLED (whats_up_hostile_2))
	    Response (whats_up_hostile_2, ThraddHostile);
	switch (GET_GAME_STATE (THRADD_STACK_1))
	{
	    case 0:
		Response (what_about_you_1, ThraddHostile);
		break;
	    case 1:
		Response (what_about_you_2, ThraddHostile);
		break;
	    case 2:
		Response (what_about_urquan_1, ThraddHostile);
		break;
	    case 3:
		Response (what_about_urquan_2, ThraddHostile);
		break;
	    case 4:
		if (!GET_GAME_STATE (KOHR_AH_FRENZY))
		    Response (got_idea, ExitConversation);
		else
		{
		    SET_GAME_STATE (THRADD_STACK_1, 5);
		}
		break;
	}
	if (GET_GAME_STATE (THRADD_HOSTILE_STACK_2) == 0)
	    Response (submit_1, ExitConversation);
	else
	    Response (submit_2, ExitConversation);
	if (GET_GAME_STATE (THRADD_HOSTILE_STACK_3) == 0)
	    Response (be_friends_1, ThraddHostile);
	else if (PHRASE_ENABLED (be_friends_2))
	    Response (be_friends_2, ThraddHostile);
	if (GET_GAME_STATE (THRADD_MISSION) == 4)
	{
	    if (GET_GAME_STATE (THRADD_HOSTILE_STACK_4) == 0)
		Response (how_impressed_urquan_1, ThraddHostile);
	    else
		Response (how_impressed_urquan_2, ThraddHostile);
	}
	if (GET_GAME_STATE (THRADD_HOSTILE_STACK_5) == 0)
	    Response (bye_hostile_1, ExitConversation);
	else
	    Response (bye_hostile_2, ExitConversation);
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

	if (GET_GAME_STATE (AQUA_HELIX))
	{
	    NumVisits = GET_GAME_STATE (HELIX_VISITS);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (DIE_THIEF_1);
		    break;
		case 1:
		    NPCPhrase (DIE_THIEF_2);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (HELIX_VISITS, NumVisits);

	    SET_GAME_STATE (BATTLE_SEGUE, 1);
	}
	else if (GET_GAME_STATE (ILWRATH_FIGHT_THRADDASH))
	{
	    NumVisits = GET_GAME_STATE (THRADD_VISITS);
	    if (GET_GAME_STATE (THRADD_MANNER))
	    {
		switch (NumVisits++)
		{
		    case 0:
			NPCPhrase (HAVING_FUN_WITH_ILWRATH_1);
			break;
		    case 1:
			NPCPhrase (HAVING_FUN_WITH_ILWRATH_2);
			--NumVisits;
			break;
		}
	    }
	    else
	    {
		switch (NumVisits++)
		{
		    case 0:
			NPCPhrase (GO_AWAY_FIGHT_ILWRATH_1);
			break;
		    case 1:
			NPCPhrase (GO_AWAY_FIGHT_ILWRATH_2);
			--NumVisits;
			break;
		}
	    }
	    SET_GAME_STATE (THRADD_VISITS, NumVisits);

	    SET_GAME_STATE (BATTLE_SEGUE, 0);
	}
	else if (GET_GAME_STATE (THRADD_MANNER))
	{
	    RESPONSE_REF	pStr0, pStr1;

	    NumVisits = GET_GAME_STATE (THRADD_VISITS);
	    switch (GET_GAME_STATE (THRADD_INTRO))
	    {
		case 0:
		    PolitePhrase (NumVisits);
		    break;
		case 1:
		    RhymePhrase (NumVisits);
		    break;
		case 2:
		    PigLatinPhrase (NumVisits);
		    break;
		case 3:
		    LikeYouPhrase (NumVisits);
		    break;
	    }
	    if (++NumVisits < 4)
	    {
		SET_GAME_STATE (THRADD_VISITS, NumVisits);
	    }

	    if (GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 6))
	    {
		pStr0 = WELCOME_HELIX0;
		pStr1 = WELCOME_HELIX1;
	    }
	    else if (GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 7))
	    {
		pStr0 = WELCOME_HOMEWORLD0;
		pStr1 = WELCOME_HOMEWORLD1;
	    }
	    else
	    {
		pStr0 = WELCOME_SPACE0;
		pStr1 = WELCOME_SPACE1;
	    }
	    NPCPhrase (pStr0);
	    NPCPhrase (GetCultureName ());
	    NPCPhrase (pStr1);

	    ThraddAllies ((RESPONSE_REF)0);
	}
	else if (GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 6))
	{
	    NumVisits = GET_GAME_STATE (HELIX_VISITS);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (HOSTILE_HELIX_HELLO_1);
		    break;
		case 1:
		    NPCPhrase (HOSTILE_HELIX_HELLO_2);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (HELIX_VISITS, NumVisits);

	    HelixWorld ((RESPONSE_REF)0);
	}
	else if (GET_GAME_STATE (THRADDASH_BODY_COUNT) >= THRADDASH_BODY_THRESHOLD)
	{
	    NPCPhrase (AMAZING_PERFORMANCE);

	    ThraddWorship ((RESPONSE_REF)0);
	}
	else
	{
	    NumVisits = GET_GAME_STATE (THRADDASH_BODY_COUNT);
	    if (NumVisits >= 16
		    && GET_GAME_STATE (THRADD_BODY_LEVEL) == 1)
	    {
		SET_GAME_STATE (THRADD_BODY_LEVEL, 2);
		NPCPhrase (IMPRESSIVE_PERFORMANCE);
	    }
	    else if (NumVisits >= 8
		    && GET_GAME_STATE (THRADD_BODY_LEVEL) == 0)
	    {
		SET_GAME_STATE (THRADD_BODY_LEVEL, 1);
		NPCPhrase (ADEQUATE_PERFORMANCE);
	    }

	    {
		if (GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 7))
		{
		    NumVisits = GET_GAME_STATE (THRADD_HOME_VISITS);
		    switch (NumVisits++)
		    {
			case 0:
			    NPCPhrase (HOSTILE_HWLD_HELLO_1);
			    break;
			case 1:
			    NPCPhrase (HOSTILE_HWLD_HELLO_2);
			    break;
			case 2:
			    NPCPhrase (HOSTILE_HWLD_HELLO_3);
			    break;
			case 3:
			    NPCPhrase (HOSTILE_HWLD_HELLO_4);
			    --NumVisits;
			    break;
		    }
		    SET_GAME_STATE (THRADD_HOME_VISITS, NumVisits);
		}
		else if ((NumVisits = GET_GAME_STATE (THRADD_MISSION)) == 0
			|| NumVisits > 3)
		{
		    NumVisits = GET_GAME_STATE (THRADD_VISITS);
		    switch (NumVisits++)
		    {
			case 0:
			    NPCPhrase (HOSTILE_SPACE_HELLO_1);
			    break;
			case 1:
			    NPCPhrase (HOSTILE_SPACE_HELLO_2);
			    break;
			case 2:
			    NPCPhrase (HOSTILE_SPACE_HELLO_3);
			    break;
			case 3:
			    NPCPhrase (HOSTILE_SPACE_HELLO_4);
			    --NumVisits;
			    break;
		    }
		    SET_GAME_STATE (THRADD_VISITS, NumVisits);
		}
		else
		{
		    switch (NumVisits)
		    {
		        case 1:
			    if (GET_GAME_STATE (THRADD_MISSION_VISITS) == 0)
				NPCPhrase (WE_GO_TO_IMPRESS_URQUAN_1);
			    else
				NPCPhrase (WE_GO_TO_IMPRESS_URQUAN_2);
			    break;
		        case 2:
			    if (GET_GAME_STATE (THRADD_MISSION_VISITS) == 0)
				NPCPhrase (WE_IMPRESSING_URQUAN_1);
			    else
				NPCPhrase (WE_IMPRESSING_URQUAN_2);
			    break;
		        case 3:
			    if (GET_GAME_STATE (THRADD_MISSION_VISITS) == 0)
				NPCPhrase (WE_IMPRESSED_URQUAN_1);
			    else
				NPCPhrase (WE_IMPRESSED_URQUAN_2);
			    break;
		    }
		    SET_GAME_STATE (THRADD_MISSION_VISITS, 1);
		}

		ThraddHostile ((RESPONSE_REF)0);
	    }
	}
ExitIntro:
	;
    }
    POP_CONTEXT
}

PROC(STATIC
COUNT far uninit_thradd, (),
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
LOCDATAPTR far init_thradd_comm, (),
    ARG_VOID
)
{
    LOCDATAPTR	retval;

    PUSH_CONTEXT
    {
	thradd_desc.init_encounter_func = Intro;
	thradd_desc.uninit_encounter_func = uninit_thradd;

strcpy(aiff_folder, "comm/thradd/thrad");

	if (GET_GAME_STATE (THRADD_MANNER)
		|| LOBYTE (GLOBAL (CurrentActivity)) == WON_LAST_BATTLE)
	{
	    SET_GAME_STATE (BATTLE_SEGUE, 0);
	}
	else
	{
	    SET_GAME_STATE (BATTLE_SEGUE, 1);
	}
	retval = &thradd_desc;
    }
    POP_CONTEXT

    return (retval);
}
