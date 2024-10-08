#include "reslib.h"
#include "resinst.h"
#include "starcon.h"
#include "commglue.h"
#include "strings.h"

static LOCDATA	orz_desc =
{
    NULL_PTR,				/* init_encounter_func */
    NULL_PTR,				/* uninit_encounter_func */
    (FRAME)ORZ_PMAP_ANIM,		/* AlienFrame */
    (COLORMAP)ORZ_COLOR_MAP,		/* AlienColorMap */
    ORZ_MUSIC,				/* AlienSong */
    ORZ_PLAYER_PHRASES,			/* PlayerPhrases */
    12 /* 13 */,					/* NumAnimations */
    {
	{
	    4,				/* StartIndex */
	    6,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    12, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    10,				/* StartIndex */
	    5,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    12, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    15,				/* StartIndex */
	    2,				/* NumFrames */
	    RANDOM_ANIM,		/* AnimFlags */
	    12, 12,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    17,				/* StartIndex */
	    3,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    12, 12,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    20,				/* StartIndex */
	    2,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    12, 0,			/* FrameRate */
	    12, ONE_SECOND * 3,		/* RestartRate */
	    (1 << 7),			/* BlockMask */
	},
	{
	    22,				/* StartIndex */
	    8,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    12, 0,			/* FrameRate */
	    12, ONE_SECOND * 3,		/* RestartRate */
	    (1 << 6),			/* BlockMask */
	},
	{
	    30,				/* StartIndex */
	    3,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    12, 0,			/* FrameRate */
	    12, ONE_SECOND * 3,		/* RestartRate */
	    (1 << 5),			/* BlockMask */
	},
	{
	    33,				/* StartIndex */
	    3,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    12, 0,			/* FrameRate */
	    12, ONE_SECOND * 3,		/* RestartRate */
	    (1 << 4),			/* BlockMask */
	},
	{
	    36,				/* StartIndex */
	    25,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    2, 8,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    61,				/* StartIndex */
	    15,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    2, 8,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    76,				/* StartIndex */
	    17,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    2, 8,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	    (1 << 12),			/* BlockMask */
	},
	{
	    93,				/* StartIndex */
	    25,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    2, 8,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    118,			/* StartIndex */
	    11,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    2, 8,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	    (1 << 10),			/* BlockMask */
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
	8, 8,				/* FrameRate */
	10, ONE_SECOND * 3 / 8,		/* RestartRate */
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

	if (PLAYER_SAID (R, bye_ally))
	    NPCPhrase (GOODBYE_ALLY);
	else if (PLAYER_SAID (R, bye_neutral))
	    NPCPhrase (GOODBYE_NEUTRAL);
	else if (PLAYER_SAID (R, bye_angry))
	    NPCPhrase (GOODBYE_ANGRY);
	else if (PLAYER_SAID (R, bye_taalo))
	{
	    if (GET_GAME_STATE (ORZ_MANNER) == 1)
		NPCPhrase (ANGRY_TAALO_GOODBYE);
	    else
		NPCPhrase (FRIENDLY_TAALO_GOODBYE);
	}
	else if (PLAYER_SAID (R, hostile_2))
	{
	    NPCPhrase (HOSTILITY_IS_BAD_2);
	    
	    SET_GAME_STATE (BATTLE_SEGUE, 1);
	}
	else if (PLAYER_SAID (R, may_we_land))
	{
	    NPCPhrase (SURE_LAND);

	    SET_GAME_STATE (TAALO_UNPROTECTED, 1);
	}
	else if (PLAYER_SAID (R, yes_alliance)
		|| PLAYER_SAID (R, were_sorry))
	{
	    if (PLAYER_SAID (R, yes_alliance))
		NPCPhrase (GREAT);
	    else
		NPCPhrase (APOLOGY_ACCEPTED);

	    SET_GAME_STATE (ORZ_ANDRO_STATE, 0);
	    SET_GAME_STATE (ORZ_GENERAL_INFO, 0);
	    SET_GAME_STATE (ORZ_PERSONAL_INFO, 0);
	    SET_GAME_STATE (ORZ_MANNER, 3);
	    ActivateStarShip (ORZ_SHIP, 0);
	}
	else if (PLAYER_SAID (R, demand_to_land))
	{
	    NPCPhrase (NO_DEMAND);

	    SET_GAME_STATE (BATTLE_SEGUE, 1);
	}
	else if (PLAYER_SAID (R, about_andro_3)
		|| PLAYER_SAID (R, must_know_about_androsyn))
	{
	    if (PLAYER_SAID (R, about_andro_3))
		NPCPhrase (BLEW_IT);
	    else
		NPCPhrase (KNOW_TOO_MUCH);

	    SET_GAME_STATE (ORZ_VISITS, 0);
	    SET_GAME_STATE (ORZ_MANNER, 2);
	    SET_GAME_STATE (BATTLE_SEGUE, 1);
	    if (PLAYER_SAID (R, about_andro_3))
		ActivateStarShip (ORZ_SHIP, -1);

	    XFormPLUT (GetColorMapAddress (
		    SetAbsColorMapIndex (CommData.AlienColorMap, 1)
		    ), ONE_SECOND / 2);
	}
	else /* insults */
	{
	    BYTE	NumVisits;

	    NumVisits = GET_GAME_STATE (ORZ_PERSONAL_INFO);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (INSULTED_1);
		    break;
		case 1:
		    NPCPhrase (INSULTED_2);
		    break;
		case 2:
		    NPCPhrase (INSULTED_3);
		    SET_GAME_STATE (BATTLE_SEGUE, 1);
		    break;
		case 7:
		    --NumVisits;
		default:
		    NPCPhrase (INSULTED_4);
		    SET_GAME_STATE (BATTLE_SEGUE, 1);
		    break;
	    }
	    SET_GAME_STATE (ORZ_PERSONAL_INFO, NumVisits);
	}
    }
    POP_CONTEXT
}

PROC(STATIC
void far TaaloWorld, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	BYTE		Manner;

	Manner = GET_GAME_STATE (ORZ_MANNER);
	if (PLAYER_SAID (R, demand_to_land))
	{
	    NPCPhrase (ASK_NICELY);

	    DISABLE_PHRASE (demand_to_land);
	}
	else if (PLAYER_SAID (R, why_you_here))
	{
	    if (Manner != 1)
		NPCPhrase (FRIENDLY_EXPLANATION);
	    else
		NPCPhrase (ANGRY_EXPLANATION);

	    DISABLE_PHRASE (why_you_here);
	}
	else if (PLAYER_SAID (R, what_is_this_place))
	{
	    if (Manner != 1)
		NPCPhrase (FRIENDLY_PLACE);
	    else
		NPCPhrase (ANGRY_PLACE);

	    DISABLE_PHRASE (what_is_this_place);
	}
	else if (PLAYER_SAID (R, may_we_land))
	{
	    NPCPhrase (ALLIES_CAN_VISIT);

	    DISABLE_PHRASE (may_we_land);
	}
	else if (PLAYER_SAID (R, make_alliance))
	{
	    NPCPhrase (CANT_ALLY_HERE);

	    DISABLE_PHRASE (make_alliance);
	}
	else if (PLAYER_SAID (R, why_busy))
	{
	    NPCPhrase (BUSY_BECAUSE);

	    DISABLE_PHRASE (why_busy);
	}

	if (PHRASE_ENABLED (may_we_land))
	{
	    if (Manner == 3 && (ActivateStarShip (
		    ORZ_SHIP, CHECK_ALLIANCE
		    ) & GOOD_GUY))
		Response (may_we_land, ExitConversation);
	    else
	    	Response (may_we_land, TaaloWorld);
	}
	else if (PHRASE_ENABLED (make_alliance))
	    Response (make_alliance, TaaloWorld);
	else if (PHRASE_ENABLED (why_busy))
	    Response (why_busy, TaaloWorld);
	if (PHRASE_ENABLED (demand_to_land))
	{
	    if (Manner == 1)
		Response (demand_to_land, ExitConversation);
	    else
	    	Response (demand_to_land, TaaloWorld);
	}
	if (PHRASE_ENABLED (why_you_here))
	    Response (why_you_here, TaaloWorld);
	if (PHRASE_ENABLED (what_is_this_place))
	    Response (what_is_this_place, TaaloWorld);
	Response (bye_taalo, ExitConversation);
    }
    POP_CONTEXT
}

PROC(STATIC
void far OrzAllied, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	BYTE		NumVisits;

	if (PLAYER_SAID (R, whats_up_ally))
	{
	    NumVisits = GET_GAME_STATE (ORZ_GENERAL_INFO);
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
	    SET_GAME_STATE (ORZ_GENERAL_INFO, NumVisits);

	    DISABLE_PHRASE (whats_up_ally);
	}
	else if (PLAYER_SAID (R, more_about_you))
	{
	    NumVisits = GET_GAME_STATE (ORZ_PERSONAL_INFO);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (ABOUT_US_1);
		    break;
		case 1:
		    NPCPhrase (ABOUT_US_2);
		    break;
		case 2:
		    NPCPhrase (ABOUT_US_3);
		    break;
		case 3:
		    NPCPhrase (ABOUT_US_4);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (ORZ_PERSONAL_INFO, NumVisits);

	    DISABLE_PHRASE (more_about_you);
	}
	else if (PLAYER_SAID (R, about_andro_1))
	{
	    NPCPhrase (FORGET_ANDRO_1);

	    SET_GAME_STATE (ORZ_ANDRO_STATE, 1);
	}
	else if (PLAYER_SAID (R, about_andro_2))
	{
	    NPCPhrase (FORGET_ANDRO_2);

	    SET_GAME_STATE (ORZ_ANDRO_STATE, 2);
	}

	if (GET_GAME_STATE (ORZ_ANDRO_STATE) == 0)
	    Response (about_andro_1, OrzAllied);
	else if (GET_GAME_STATE (ORZ_ANDRO_STATE) == 1)
	    Response (about_andro_2, OrzAllied);
	else
	{
	    Response (about_andro_3, ExitConversation);
	}
	if (PHRASE_ENABLED (whats_up_ally))
	    Response (whats_up_ally, OrzAllied);
	if (PHRASE_ENABLED (more_about_you))
	    Response (more_about_you, OrzAllied);
	Response (bye_ally, ExitConversation);
    }
    POP_CONTEXT
}

PROC_LOCAL(
void far OrzNeutral, (R),
    ARG_END	(RESPONSE_REF	R)
);

PROC(STATIC
void far WhereAndrosyn, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	NPCPhrase (DISEMBLE_ABOUT_ANDROSYN);
	DISABLE_PHRASE (where_androsyn);

	Response (must_know_about_androsyn, ExitConversation);
	Response (dont_really_care, OrzNeutral);
    }
    POP_CONTEXT
}

PROC(STATIC
void far OfferAlliance, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	if (PLAYER_SAID (R, seem_like_nice_guys))
	    NPCPhrase (ARE_NICE_WANT_ALLY);
	else if (PLAYER_SAID (R, talk_about_alliance))
	    NPCPhrase (OK_TALK_ALLIANCE);
	else if (PLAYER_SAID (R, why_so_trusting))
	{
	    NPCPhrase (TRUSTING_BECAUSE);

	    SET_GAME_STATE (ORZ_STACK1, 1);
	}

	Response (no_alliance, OrzNeutral);
	Response (decide_later, OrzNeutral);
	if (GET_GAME_STATE (ORZ_STACK1) == 0)
	{
	    Response (why_so_trusting, OfferAlliance);
	}
	Response (yes_alliance, ExitConversation);
    }
    POP_CONTEXT
}

PROC(STATIC
void far OrzNeutral, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	BYTE		i, LastStack;
	RESPONSE_REF		pStr[3];

	LastStack = 0;
	pStr[0] = pStr[1] = pStr[2] = 0;
	if (PLAYER_SAID (R, hostile_1))
	{
	    NPCPhrase (HOSTILITY_IS_BAD_1);

	    DISABLE_PHRASE (hostile_1);
	    LastStack = 2;
	}
	else if (PLAYER_SAID (R, we_are_vindicator0))
	{
	    NPCPhrase (NICE_TO_MEET_YOU);

	    SET_GAME_STATE (ORZ_STACK0, 1);
	    LastStack = 1;
	}
	else if (PLAYER_SAID (R, who_you))
	{
	    NPCPhrase (WE_ARE_ORZ);

	    SET_GAME_STATE (ORZ_ANDRO_STATE, 1);
	}
	else if (PLAYER_SAID (R, why_here))
	{
	    NPCPhrase (HERE_BECAUSE);

	    SET_GAME_STATE (ORZ_ANDRO_STATE, 2);
	}
	else if (PLAYER_SAID (R, no_alliance))
	{
	    NPCPhrase (MAYBE_LATER);

	    DISABLE_PHRASE (talk_about_alliance);
	    SET_GAME_STATE (REFUSED_ORZ_ALLIANCE, 1);
	}
	else if (PLAYER_SAID (R, decide_later))
	{
	    NPCPhrase (OK_LATER);

	    DISABLE_PHRASE (talk_about_alliance);
	    SET_GAME_STATE (REFUSED_ORZ_ALLIANCE, 1);
	}
	else if (PLAYER_SAID (R, dont_really_care))
	    NPCPhrase (YOU_ARE_OUR_FRIENDS);
	else if (PLAYER_SAID (R, where_androsyn))
	{
	    WhereAndrosyn (R);

	    goto ExitNeutral;
	}
	else if (PLAYER_SAID (R, talk_about_alliance)
		|| PLAYER_SAID (R, seem_like_nice_guys))
	{
	    OfferAlliance (R);

	    goto ExitNeutral;
	}
	else if (PLAYER_SAID (R, hostile_2))
	{
	    ExitConversation (R);

	    goto ExitNeutral;
	}

	if (GET_GAME_STATE (ORZ_ANDRO_STATE) == 0)
	    pStr[0] = who_you;
	else if (GET_GAME_STATE (ORZ_ANDRO_STATE) == 1)
	    pStr[0] = why_here;
	else if (PHRASE_ENABLED (where_androsyn) && GET_GAME_STATE (ORZ_ANDRO_STATE) == 2)
	    pStr[0] = where_androsyn;
	if (GET_GAME_STATE (REFUSED_ORZ_ALLIANCE))
	{
	    if (PHRASE_ENABLED (talk_about_alliance))
		pStr[1] = talk_about_alliance;
	}
	else if (GET_GAME_STATE (ORZ_STACK0) == 0)
	{
	    construct_response (shared_phrase_buf,
		    we_are_vindicator0,
	    	    GLOBAL_SIS (CommanderName),
		    we_are_vindicator1,
		    GLOBAL_SIS (ShipName),
		    we_are_vindicator2,
		    0);
	    pStr[1] = we_are_vindicator0;
	}
	else
	    pStr[1] = seem_like_nice_guys;
	if (PHRASE_ENABLED (hostile_1))
	    pStr[2] = hostile_1;
	else
	    pStr[2] = hostile_2;

	if (pStr[LastStack])
	{
	    if (pStr[LastStack] != we_are_vindicator0)
		Response (pStr[LastStack], OrzNeutral);
	    else
		DoResponsePhrase (pStr[LastStack], OrzNeutral, shared_phrase_buf);
	}
	for (i = 0; i < 3; ++i)
	{
	    if (i != LastStack && pStr[i])
	    {
		if (pStr[i] != we_are_vindicator0)
		    Response (pStr[i], OrzNeutral);
		else
		    DoResponsePhrase (pStr[i], OrzNeutral, shared_phrase_buf);
	    }
	}
	Response (bye_neutral, ExitConversation);
ExitNeutral:
	;
    }
    POP_CONTEXT
}

PROC(STATIC
void far OrzAngry, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	if (PLAYER_SAID (R, whats_up_angry))
	{
	    BYTE	NumVisits;

	    NumVisits = GET_GAME_STATE (ORZ_GENERAL_INFO);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (GENERAL_INFO_ANGRY_1);
		    break;
		case 1:
		    NPCPhrase (GENERAL_INFO_ANGRY_2);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (ORZ_GENERAL_INFO, NumVisits);

	    DISABLE_PHRASE (whats_up_angry);
	}

	if (PHRASE_ENABLED (whats_up_angry))
	{
	    Response (whats_up_angry, OrzAngry);
	}
	Response (were_sorry, ExitConversation);
	switch (GET_GAME_STATE (ORZ_PERSONAL_INFO))
	{
	    case 0:
		Response (insult_1, ExitConversation);
		break;
	    case 1:
		Response (insult_2, ExitConversation);
		break;
	    case 2:
		Response (insult_3, ExitConversation);
		break;
	    case 3:
		Response (insult_4, ExitConversation);
		break;
	    case 4:
		Response (insult_5, ExitConversation);
		break;
	    case 5:
		Response (insult_6, ExitConversation);
		break;
	    case 6:
		Response (insult_7, ExitConversation);
		break;
	    case 7:
		Response (insult_8, ExitConversation);
		break;
	}
	Response (bye_angry, ExitConversation);
    }
    POP_CONTEXT
}

static void	far
Intro ()
{
    PUSH_CONTEXT
    {
	BYTE	NumVisits, Manner;

	if (LOBYTE (GLOBAL (CurrentActivity)) == WON_LAST_BATTLE)
	{
//	    NPCPhrase (OUT_TAKES);

	    SET_GAME_STATE (BATTLE_SEGUE, 0);
	    goto ExitIntro;
	}

	if (!GET_GAME_STATE (MET_ORZ_BEFORE))
	    NPCPhrase (INIT_HELLO);

	Manner = GET_GAME_STATE (ORZ_MANNER);
	if (Manner == 2)
	{
	    CommData.AlienColorMap =
		    SetAbsColorMapIndex (CommData.AlienColorMap, 1);

	    NumVisits = GET_GAME_STATE (ORZ_VISITS);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (HOSTILE_HELLO_1);
		    break;
		case 1:
		    NPCPhrase (HOSTILE_HELLO_2);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (ORZ_VISITS, NumVisits);

	    SET_GAME_STATE (BATTLE_SEGUE, 1);
	}
	else if (GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 6))
	{
	    NumVisits = GET_GAME_STATE (TAALO_VISITS);
	    if (Manner == 0)
	    {
		switch (NumVisits++)
		{
		    case 0:
			NPCPhrase (FRDLY_ALLI_TAALO_HELLO_1);
			break;
		    case 1:
			NPCPhrase (FRDLY_ALLI_TAALO_HELLO_2);
			--NumVisits;
			break;
		}
	    }
	    else
	    {
		switch (NumVisits++)
		{
		    case 0:
			NPCPhrase (ANGRY_TAALO_HELLO_1);
			break;
		    case 1:
			NPCPhrase (ANGRY_TAALO_HELLO_2);
			--NumVisits;
			break;
		}
	    }
	    SET_GAME_STATE (TAALO_VISITS, NumVisits);

	    TaaloWorld ((RESPONSE_REF)0);
	}
	else if (Manner == 3 && (ActivateStarShip (
		ORZ_SHIP, CHECK_ALLIANCE
		) & GOOD_GUY))
	{
	    if (GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 7))
	    {
		NumVisits = GET_GAME_STATE (ORZ_HOME_VISITS);
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
		SET_GAME_STATE (ORZ_HOME_VISITS, NumVisits);
	    }
	    else
	    {
		NumVisits = GET_GAME_STATE (ORZ_VISITS);
		switch (NumVisits++)
		{
		    case 0:
			NPCPhrase (ALLIED_SPACE_HELLO_1);
			break;
		    case 1:
			NPCPhrase (ALLIED_SPACE_HELLO_2);
			break;
		    case 2:
			NPCPhrase (ALLIED_SPACE_HELLO_3);
			break;
		    case 3:
			NPCPhrase (ALLIED_SPACE_HELLO_4);
			--NumVisits;
			break;
		}
		SET_GAME_STATE (ORZ_VISITS, NumVisits);
	    }

	    OrzAllied ((RESPONSE_REF)0);
	}
	else if (Manner != 1)
	{
	    if (GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 7))
	    {
		NumVisits = GET_GAME_STATE (ORZ_HOME_VISITS);
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
		SET_GAME_STATE (ORZ_HOME_VISITS, NumVisits);
	    }
	    else
	    {
		NumVisits = GET_GAME_STATE (ORZ_VISITS);
		switch (NumVisits++)
		{
		    case 0:
			NPCPhrase (NEUTRAL_SPACE_HELLO_1);
			break;
		    case 1:
			NPCPhrase (NEUTRAL_SPACE_HELLO_2);
			break;
		    case 2:
			NPCPhrase (NEUTRAL_SPACE_HELLO_3);
			break;
		    case 3:
			NPCPhrase (NEUTRAL_SPACE_HELLO_4);
			--NumVisits;
			break;
		}
		SET_GAME_STATE (ORZ_VISITS, NumVisits);
	    }

	    OrzNeutral ((RESPONSE_REF)0);
	}
	else
	{
	    if (GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 7))
	    {
		NumVisits = GET_GAME_STATE (ORZ_HOME_VISITS);
		switch (NumVisits++)
		{
		    case 0:
			NPCPhrase (ANGRY_HOMEWORLD_HELLO_1);
			break;
		    case 1:
			NPCPhrase (ANGRY_HOMEWORLD_HELLO_2);
			--NumVisits;
			break;
		}
		SET_GAME_STATE (ORZ_HOME_VISITS, NumVisits);
	    }
	    else
	    {
		NumVisits = GET_GAME_STATE (ORZ_VISITS);
		switch (NumVisits++)
		{
		    case 0:
			NPCPhrase (ANGRY_SPACE_HELLO_1);
			break;
		    case 1:
			NPCPhrase (ANGRY_SPACE_HELLO_2);
			--NumVisits;
			break;
		}
		SET_GAME_STATE (ORZ_VISITS, NumVisits);
	    }

	    OrzAngry ((RESPONSE_REF)0);
	}

	if (!GET_GAME_STATE (MET_ORZ_BEFORE))
	{
	    COUNT	N;

	    SET_GAME_STATE (MET_ORZ_BEFORE, 1);

	    N = CommData.AlienTalkDesc.NumFrames;
	    CommData.AlienTalkDesc.NumFrames = 0;
	    AlienTalkSegue (1);
	    CommData.AlienTalkDesc.NumFrames = N;
	}

ExitIntro:
	;
    }
    POP_CONTEXT
}

PROC(STATIC
COUNT far uninit_orz, (),
    ARG_VOID
)
{
    PUSH_CONTEXT
    {
	BYTE	Manner;

	if (GET_GAME_STATE (BATTLE_SEGUE) == 1
		&& (Manner = GET_GAME_STATE (ORZ_MANNER)) != 2)
	{
	    SET_GAME_STATE (ORZ_MANNER, 1);
	    if (Manner != 1)
	    {
		SET_GAME_STATE (ORZ_VISITS, 0);
		SET_GAME_STATE (ORZ_HOME_VISITS, 0);
		SET_GAME_STATE (TAALO_VISITS, 0);
	    }
	}
    }
    POP_CONTEXT

    return (0);
}

PROC(
LOCDATAPTR far init_orz_comm, (),
    ARG_VOID
)
{
    LOCDATAPTR	retval;

    PUSH_CONTEXT
    {
	orz_desc.init_encounter_func = Intro;
	orz_desc.uninit_encounter_func = uninit_orz;

strcpy(aiff_folder, "comm/orz/orz");

	if (GET_GAME_STATE (ORZ_MANNER) == 3
		|| LOBYTE (GLOBAL (CurrentActivity)) == WON_LAST_BATTLE)
	{
	    SET_GAME_STATE (BATTLE_SEGUE, 0);
	}
	else
	{
	    SET_GAME_STATE (BATTLE_SEGUE, 1);
	}
	retval = &orz_desc;
    }
    POP_CONTEXT

    return (retval);
}
