#include "reslib.h"
#include "resinst.h"
#include "starcon.h"
#include "commglue.h"
#include "strings.h"

static LOCDATA	spathi_desc =
{
    NULL_PTR,				/* init_encounter_func */
    NULL_PTR,				/* uninit_encounter_func */
    (FRAME)SPATHI_PMAP_ANIM,		/* AlienFrame */
    0,					/* AlienColorMap */
    SPATHI_MUSIC,			/* AlienSong */
    SPATHI_PLAYER_PHRASES,		/* PlayerPhrases */
    8,					/* NumAnimations */
    {
	{
	    1,				/* StartIndex */
	    6,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    6, 0,			/* FrameRate */
	    0, ONE_SECOND,		/* RestartRate */
	    (1 << 1),			/* BlockMask */
	},
	{
	    7,				/* StartIndex */
	    9,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    6, 0,			/* FrameRate */
	    6, 0,			/* RestartRate */
	    (1 << 0),			/* BlockMask */
	},
	{
	    16,				/* StartIndex */
	    4,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    12, 8,			/* FrameRate */
	    12, 8,			/* RestartRate */
	    (1 << 4),			/* BlockMask */
	},
	{
	    20,				/* StartIndex */
	    4,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    8, 8,			/* FrameRate */
	    8, 8,			/* FrameRate */
	    (1 << 5)
	},
	{
	    24,				/* StartIndex */
	    5,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    8, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	    (1 << 2),			/* BlockMask */
	},


	{
	    34,				/* StartIndex */
	    4,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    8, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	    (1 << 3),			/* BlockMask */
	},
	{
	    38,				/* StartIndex */
	    3,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    8, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    41,				/* StartIndex */
	    3,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    8, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	},
    },
#ifdef NEVER
    {
	0,				/* StartIndex */
	0,				/* NumFrames */
	0,				/* AnimFlags */
	0, 0,				/* FrameRate */
	0, 0,				/* RestartRate */
    },
    {
	29,				/* StartIndex */
	5,				/* NumFrames */
	0,				/* AnimFlags */
	8, 0,				/* FrameRate */
	10, 0,				/* RestartRate */
    },
#endif /* NEVER */
};

static void	far
ExitConversation (Response)
RESPONSE_REF	Response;
{
    PUSH_CONTEXT
    {
	SET_GAME_STATE (BATTLE_SEGUE, 0);

	if (PLAYER_SAID (Response, bye_ally_space))
	    NPCPhrase (GOODBYE_ALLY_SPACE);
	else if (PLAYER_SAID (Response, bye_friendly_space))
	    NPCPhrase (GOODBYE_FRIENDLY_SPACE);
	else if (PLAYER_SAID (Response, part_in_peace))
	    NPCPhrase (KEEP_IT_SECRET);
	else if (PLAYER_SAID (Response, we_sorry_space))
	    NPCPhrase (APOLOGIZE_AT_HOMEWORLD);
	else if (PLAYER_SAID (Response, give_info_space))
	    NPCPhrase (HERES_SOME_INFO);
	else if (PLAYER_SAID (Response, bye_angry_space))
	    NPCPhrase (GOODBYE_ANGRY_SPACE);
	else if (PLAYER_SAID (Response, attack_you_now))
	{
	    NPCPhrase (YIPES);

	    SET_GAME_STATE (BATTLE_SEGUE, 1);
	}
	else if (PLAYER_SAID (Response, we_fight_again_space))
	{
	    NPCPhrase (OK_FIGHT_AGAIN_SPACE);

	    SET_GAME_STATE (BATTLE_SEGUE, 1);
	}
	else if (PLAYER_SAID (Response, die_slugboy)
		|| PLAYER_SAID (Response, we_fight_1)
		|| PLAYER_SAID (Response, we_fight_2)
		|| PLAYER_SAID (Response, pay_for_crimes)
		|| PLAYER_SAID (Response, tell_me_coordinates)
		|| PLAYER_SAID (Response, changed_mind))
	{
	    if (PLAYER_SAID (Response, tell_me_coordinates))
		NPCPhrase (FAKE_COORDINATES);
	    NPCPhrase (OK_WE_FIGHT_AT_PLUTO);

	    SET_GAME_STATE (BATTLE_SEGUE, 1);
	}
	else if (PLAYER_SAID (Response, join_us))
	{
	    if (ActivateStarShip (SPATHI_SHIP, FEASIBILITY_STUDY) == 0)
		NPCPhrase (TOO_SCARY);
	    else
	    {
		NPCPhrase (WILL_JOIN);

		AlienTalkSegue ((COUNT)~0);
		ActivateStarShip (SPATHI_SHIP, 1);
	    }
	}
    }
    POP_CONTEXT
}

PROC(STATIC
void far SpathiOnPluto, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	if (PLAYER_SAID (R, hi_there))
	    NPCPhrase (ARE_YOU_SURE);
	else if (PLAYER_SAID (R, dont_kill))
	    NPCPhrase (OK_WONT);
	else if (PLAYER_SAID (R, youre_forgiven))
	    NPCPhrase (THANKS_FOR_FORGIVENESS);
	else if (PLAYER_SAID (R, you_wont_die_yet))
	    NPCPhrase (ETERNAL_GRATITUDE);
	else if (PLAYER_SAID (R, you_may_live))
	    NPCPhrase (GEE_THANKS);
	else if (PLAYER_SAID (R, youve_got_me_all_wrong))
	    NPCPhrase (SORRY_NO_COORDS);
	else if (PLAYER_SAID (R, what_doing_on_pluto_1))
	{
	    NPCPhrase (ABOUT_20_YEARS_AGO);

	    DISABLE_PHRASE (what_doing_on_pluto_1);
	}
	else if (PLAYER_SAID (R, what_doing_on_pluto_2))
	{
	    NPCPhrase (WHEN_URQUAN_ARRIVED);

	    DISABLE_PHRASE (what_doing_on_pluto_2);
	}
	else if (PLAYER_SAID (R, what_doing_on_pluto_3))
	{
	    NPCPhrase (STATIONED_ON_EARTH_MOON);

	    DISABLE_PHRASE (what_doing_on_pluto_3);
	}
	else if (PLAYER_SAID (R, what_about_ilwrath))
	{
	    NPCPhrase (ABOUT_ILWRATH);

	    DISABLE_PHRASE (what_about_ilwrath);
	}
	else if (PLAYER_SAID (R, when_ilwrath))
	{
	    NPCPhrase (THEN_ILWRATH);

	    DISABLE_PHRASE (when_ilwrath);
	}
	else if (PLAYER_SAID (R, what_about_moonbase))
	{
	    NPCPhrase (SET_UP_BASE);

	    DISABLE_PHRASE (what_about_moonbase);
	}
	else if (PLAYER_SAID (R, what_about_other_spathi))
	{
	    NPCPhrase (SPATHI_ARE);

	    DISABLE_PHRASE (what_about_other_spathi);
	}
	else if (PLAYER_SAID (R, what_about_other_spathi))
	{
	    NPCPhrase (THEN_ILWRATH);

	    DISABLE_PHRASE (what_about_other_spathi);
	}
	else if (PLAYER_SAID (R, how_many_crew))
	{
	    NPCPhrase (THOUSANDS);

	    DISABLE_PHRASE (how_many_crew);
	}
	else if (PLAYER_SAID (R, really_thousands))
	{
	    NPCPhrase (JUST_ME);

	    DISABLE_PHRASE (really_thousands);
	}
	else if (PLAYER_SAID (R, full_of_monsters))
	{
	    NPCPhrase (HOW_TRUE);

	    DISABLE_PHRASE (full_of_monsters);
	}
	else if (PLAYER_SAID (R, what_enemy))
	{
	    NPCPhrase (ENEMY_IS);

	    DISABLE_PHRASE (what_enemy);
	}
	else if (PLAYER_SAID (R, why_you_here))
	{
	    NPCPhrase (DREW_SHORT_STRAW);

	    DISABLE_PHRASE (why_you_here);
	}
	else if (PLAYER_SAID (R, where_are_urquan))
	{
	    NPCPhrase (URQUAN_LEFT);

	    DISABLE_PHRASE (where_are_urquan);
	}
	else if (PLAYER_SAID (R, what_about_other_races))
	{
	    NPCPhrase (ABOUT_OTHER_RACES);

	    DISABLE_PHRASE (what_about_other_races);
	}
	else if (PLAYER_SAID (R, what_blaze_of_glory))
	{
	    NPCPhrase (BLAZE_IS);

	    DISABLE_PHRASE (what_blaze_of_glory);
	}
	else if (PLAYER_SAID (R, what_about_yourself))
	{
	    NPCPhrase (ABOUT_MYSELF);

	    DISABLE_PHRASE (what_about_yourself);
	}
	else if (PLAYER_SAID (R, join_us))
	{
	    static BYTE	join_us_refusals;

	    if (join_us_refusals == 0)
	    {
		NPCPhrase (WONT_JOIN_1);
		++join_us_refusals;
	    }
	    else if (join_us_refusals == 1)
	    {
		NPCPhrase (WONT_JOIN_2);
		++join_us_refusals;
	    }
	    else
		NPCPhrase (WONT_JOIN_3);
	}

	if (PHRASE_ENABLED (what_doing_on_pluto_1))
	    Response (what_doing_on_pluto_1, SpathiOnPluto);
	else if (PHRASE_ENABLED (what_doing_on_pluto_2))
	    Response (what_doing_on_pluto_2, SpathiOnPluto);
	else if (PHRASE_ENABLED (what_doing_on_pluto_3))
	    Response (what_doing_on_pluto_3, SpathiOnPluto);
	else
	{
	    if (PHRASE_ENABLED (what_about_ilwrath))
		Response (what_about_ilwrath, SpathiOnPluto);
	    else if (PHRASE_ENABLED (when_ilwrath))
		Response (when_ilwrath, SpathiOnPluto);

	    if (PHRASE_ENABLED (what_about_moonbase))
		Response (what_about_moonbase, SpathiOnPluto);
	    else if (PHRASE_ENABLED (what_about_other_spathi))
		Response (what_about_other_spathi, SpathiOnPluto);
	    else
	    {
		if (PHRASE_ENABLED (how_many_crew))
		    Response (how_many_crew, SpathiOnPluto);
		else if (PHRASE_ENABLED (really_thousands))
		    Response (really_thousands, SpathiOnPluto);
		else if (PHRASE_ENABLED (full_of_monsters))
		    Response (full_of_monsters, SpathiOnPluto);

		if (PHRASE_ENABLED (what_enemy))
		    Response (what_enemy, SpathiOnPluto);
		else if (PHRASE_ENABLED (why_you_here))
		    Response (why_you_here, SpathiOnPluto);
	    }
	}
	if (PHRASE_ENABLED (where_are_urquan))
	    Response (where_are_urquan, SpathiOnPluto);
	else if (PHRASE_ENABLED (what_about_other_races))
	    Response (what_about_other_races, SpathiOnPluto);
	else if (PHRASE_ENABLED (what_blaze_of_glory))
	    Response (what_blaze_of_glory, SpathiOnPluto);
	else if (PHRASE_ENABLED (what_about_yourself))
	    Response (what_about_yourself, SpathiOnPluto);

	if (!PHRASE_ENABLED (full_of_monsters))
	    Response (join_us, ExitConversation);
	else
	    Response (join_us, SpathiOnPluto);
	Response (changed_mind, ExitConversation);
    }
    POP_CONTEXT
}

PROC(STATIC
void far SpathiMustGrovel, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	if (PLAYER_SAID (R, identify))
	{
	    NPCPhrase (I_FWIFFO);

	    Response (do_cultural, SpathiMustGrovel);
	    Response (youre_forgiven, SpathiOnPluto);
	    Response (die_slugboy, ExitConversation);
	}
	else if (PLAYER_SAID (R, do_cultural))
	{
	    NPCPhrase (WEZZY_WEZZAH);

	    Response (begin_ritual, SpathiMustGrovel);
	    Response (you_wont_die_yet, SpathiOnPluto);
	    Response (we_fight_2, ExitConversation);
	}
	else if (PLAYER_SAID (R, begin_ritual))
	{
	    NPCPhrase (MUST_DO_RITUAL_AT_HOME);

	    Response (you_may_live, SpathiOnPluto);
	    Response (pay_for_crimes, ExitConversation);
	    Response (what_are_coordinates, SpathiMustGrovel);
	}
	else /* if (R == what_are_coordinates) */
	{
	    NPCPhrase (COORDINATES_ARE);

	    Response (youve_got_me_all_wrong, SpathiOnPluto);
	    Response (tell_me_coordinates, ExitConversation);
	}
    }
    POP_CONTEXT
}

PROC(STATIC
void far SpathiAllies, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	if (R == 0)
	{
	    BYTE	NumVisits;

	    NumVisits = GET_GAME_STATE (SPATHI_VISITS);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (INIT_ALLIED_HELLO_SPACE);
		    break;
		case 1:
		    NPCPhrase (SSQ_ALLIED_HELLO_SPACE);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (SPATHI_VISITS, NumVisits);
	}
	else if (PLAYER_SAID (R, whats_up_space_2))
	{
	    NPCPhrase (GENERAL_INFO_SPACE_2);

	    DISABLE_PHRASE (whats_up_space_2);
	}
	else if (PLAYER_SAID (R, give_us_info_from_space))
	{
	    NPCPhrase (GET_INFO_FROM_SPATHIWA);

	    DISABLE_PHRASE (give_us_info_from_space);
	}
	else if (PLAYER_SAID (R, give_us_resources_space))
	{
	    NPCPhrase (GET_RES_FROM_SPATHIWA);

	    DISABLE_PHRASE (give_us_resources_space);
	}
	else if (PLAYER_SAID (R, what_do_for_fun))
	{
	    NPCPhrase (DO_THIS_FOR_FUN);

	    DISABLE_PHRASE (what_do_for_fun);
	}

	if (PHRASE_ENABLED (whats_up_space_2))
	    Response (whats_up_space_2, SpathiAllies);
	if (PHRASE_ENABLED (give_us_info_from_space))
	    Response (give_us_info_from_space, SpathiAllies);
	if (PHRASE_ENABLED (give_us_resources_space))
	    Response (give_us_resources_space, SpathiAllies);
	if (PHRASE_ENABLED (what_do_for_fun))
	    Response (what_do_for_fun, SpathiAllies);
	Response (bye_ally_space, ExitConversation);
    }
    POP_CONTEXT
}

PROC(STATIC
void far SpathiFriendly, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	if (R == 0)
	{
	    BYTE	NumVisits;

	    NumVisits = GET_GAME_STATE (SPATHI_VISITS);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (INIT_FRIENDLY_HELLO_SPACE);
		    break;
		case 1:
		    NPCPhrase (SSQ_FRIENDLY_HELLO_SPACE);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (SPATHI_VISITS, NumVisits);
	}
	else if (PLAYER_SAID (R, since_friendly_give_stuff))
	{
	    NPCPhrase (GIVE_ADVICE);

	    DISABLE_PHRASE (since_friendly_give_stuff);
	}
	else if (PLAYER_SAID (R, whats_up_space_1))
	{
	    NPCPhrase (GENERAL_INFO_SPACE_1);

	    DISABLE_PHRASE (whats_up_space_1);
	}

	if (PHRASE_ENABLED (whats_up_space_1))
	    Response (whats_up_space_1, SpathiFriendly);
	if (PHRASE_ENABLED (since_friendly_give_stuff))
	    Response (since_friendly_give_stuff, SpathiFriendly);
	Response (bye_friendly_space, ExitConversation);
    }
    POP_CONTEXT
}

PROC_LOCAL(
void far SpathiNeutral, (R),
    ARG_END	(RESPONSE_REF	R)
);

PROC(STATIC
void far SpathiBefriend, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	BYTE		InfoLeft, LastStack;
	RESPONSE_REF		pStr[2];

	InfoLeft = FALSE;
	LastStack = 0;
	pStr[0] = pStr[1] = 0;
	if (PLAYER_SAID (R, come_in_peace))
	    NPCPhrase (AGAINST_NATURE);
	else if (PLAYER_SAID (R, looking_for_a_few_good_squids))
	{
	    NPCPhrase (URQUAN_SLAVES);

	    DISABLE_PHRASE (looking_for_a_few_good_squids);
	}
	else if (PLAYER_SAID (R, why_slaves))
	{
	    NPCPhrase (UMGAH_TRICK);

	    DISABLE_PHRASE (why_slaves);
	}
	else if (PLAYER_SAID (R, tell_us_about_you))
	{
	    NPCPhrase (ABOUT_US);

	    DISABLE_PHRASE (tell_us_about_you);
	    LastStack = 1;
	}
	else if (PLAYER_SAID (R, what_you_really_want))
	{
	    NPCPhrase (WANT_THIS);

	    DISABLE_PHRASE (what_you_really_want);
	}
	else if (PLAYER_SAID (R, how_about_alliance))
	{
	    NPCPhrase (SURE);

	    DISABLE_PHRASE (how_about_alliance);
	}

	if (PHRASE_ENABLED (looking_for_a_few_good_squids))
	    pStr[0] = looking_for_a_few_good_squids;
	else if (PHRASE_ENABLED (why_slaves))
	    pStr[0] = why_slaves;
	if (PHRASE_ENABLED (tell_us_about_you))
	    pStr[1] = tell_us_about_you;
	else if (PHRASE_ENABLED (what_you_really_want))
	    pStr[1] = what_you_really_want;
	if (pStr[LastStack])
	{
	    InfoLeft = TRUE;
	    Response (pStr[LastStack], SpathiBefriend);
	}
	LastStack ^= 1;
	if (pStr[LastStack])
	{
	    InfoLeft = TRUE;
	    Response (pStr[LastStack], SpathiBefriend);
	}
	if (PHRASE_ENABLED (how_about_alliance))
	{
	    InfoLeft = TRUE;
	    Response (how_about_alliance, SpathiBefriend);
	}

	if (!InfoLeft)
	{
	    SET_GAME_STATE (SPATHI_STACK1, 1);
	    SpathiNeutral (R);
	}
    }
    POP_CONTEXT
}

PROC(STATIC
void far SpathiAntagonize, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	if (PLAYER_SAID (R, prepare_to_die))
	{
	    NPCPhrase (ALWAYS_PREPARED);

	    SET_GAME_STATE (SPATHI_STACK2, 1);
	}
	else if (PLAYER_SAID (R, heard_youre_cowards))
	{
	    NPCPhrase (DARN_TOOTIN);

	    DISABLE_PHRASE (heard_youre_cowards);
	}
	else if (PLAYER_SAID (R, wanna_fight))
	{
	    NPCPhrase (YES_WE_DO);

	    DISABLE_PHRASE (wanna_fight);
	}
	else if (PLAYER_SAID (R, so_lets_fight))
	{
	    NPCPhrase (OK_LETS_FIGHT);

	    DISABLE_PHRASE (so_lets_fight);
	}
	else if (PLAYER_SAID (R, so_lets_fight_already))
	{
	    NPCPhrase (DONT_REALLY_WANT_TO_FIGHT);

	    DISABLE_PHRASE (so_lets_fight_already);
	}

	if (PHRASE_ENABLED (wanna_fight))
	    Response (wanna_fight, SpathiAntagonize);
	else if (PHRASE_ENABLED (so_lets_fight))
	    Response (so_lets_fight, SpathiAntagonize);
	else if (PHRASE_ENABLED (so_lets_fight_already))
	    Response (so_lets_fight_already, SpathiAntagonize);
	if (PHRASE_ENABLED (heard_youre_cowards))
	    Response (heard_youre_cowards, SpathiAntagonize);
	Response (attack_you_now, ExitConversation);
    }
    POP_CONTEXT
}

PROC(STATIC
void far SpathiNeutral, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	if (R == 0)
	{
	    BYTE	NumVisits;

	    NumVisits = GET_GAME_STATE (SPATHI_VISITS);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (INIT_NEUTRAL_HELLO_SPACE);
		    break;
		case 1:
		    NPCPhrase (SSQ_NEUTRAL_HELLO_SPACE);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (SPATHI_VISITS, NumVisits);
	}
	else if (PLAYER_SAID (R, look_weird))
	{
	    NPCPhrase (YOU_LOOK_WEIRD);

	    SET_GAME_STATE (SPATHI_STACK0, 1);
	}
	else if (PLAYER_SAID (R, no_look_really_weird))
	{
	    NPCPhrase (NO_YOU_LOOK_REALLY_WEIRD);

	    SET_GAME_STATE (SPATHI_STACK0, 2);
	}

	switch (GET_GAME_STATE (SPATHI_STACK0))
	{
	    case 0:
		Response (look_weird, SpathiNeutral);
		break;
	    case 1:
		Response (no_look_really_weird, SpathiNeutral);
		break;
	}
	if (GET_GAME_STATE (SPATHI_STACK1) == 0)
	{
	    Response (come_in_peace, SpathiBefriend);
	}
	if (GET_GAME_STATE (SPATHI_STACK2) == 0)
	{
	    Response (prepare_to_die, SpathiAntagonize);
	}
	else
	{
	    Response (attack_you_now, ExitConversation);
	}
	Response (part_in_peace, ExitConversation);
    }
    POP_CONTEXT
}

static void	far
Intro ()
{
    PUSH_CONTEXT
    {
	BYTE		Manner;

	Manner = GET_GAME_STATE (SPATHI_MANNER);
	if (GET_GAME_STATE (FOUND_PLUTO_SPATHI) == 1)
	{
	    NPCPhrase (SORRY_ABOUT_THAT);

		/* if already know password from Melnorme,
		 * but haven't visited Spathiwa yet . . .
		 */
	    if (GET_GAME_STATE (SPATHI_HOME_VISITS) == 7)
	    {
		SET_GAME_STATE (KNOW_SPATHI_PASSWORD, 0);
		SET_GAME_STATE (SPATHI_HOME_VISITS, 0);
	    }

	    Response (identify, SpathiMustGrovel);
	    Response (hi_there, SpathiOnPluto);
	    Response (dont_kill, SpathiOnPluto);
	    Response (we_fight_1, ExitConversation);
	}
	else if (Manner == 2)
	{
	    NPCPhrase (HATE_YOU_FOREVER_SPACE);
	    SET_GAME_STATE (BATTLE_SEGUE, 1);
	}
	else if (Manner == 1)
	{
	    BYTE	NumVisits;

	    NumVisits = GET_GAME_STATE (SPATHI_VISITS);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (INIT_ANGRY_HELLO_SPACE);
		    break;
		case 1:
		    NPCPhrase (SSQ_ANGRY_HELLO_SPACE);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (SPATHI_VISITS, NumVisits);

	    Response (give_info_space, ExitConversation);
	    Response (we_sorry_space,ExitConversation);
	    Response (we_fight_again_space, ExitConversation);
	    Response (bye_angry_space, ExitConversation);
	}
	else if (ActivateStarShip (SPATHI_SHIP, CHECK_ALLIANCE) & GOOD_GUY)
	{
	    SpathiAllies ((RESPONSE_REF)0);
	}
	else if (GET_GAME_STATE (SPATHI_QUEST))
	{
	    SpathiFriendly ((RESPONSE_REF)0);
	}
	else
	{
	    SpathiNeutral ((RESPONSE_REF)0);
	}
    }
    POP_CONTEXT
}

PROC(STATIC
COUNT far uninit_spathi, (),
    ARG_VOID
)
{
    PUSH_CONTEXT
    {
	BYTE	Manner;

	if (GET_GAME_STATE (FOUND_PLUTO_SPATHI) == 1)
	{
	    SET_GAME_STATE (FOUND_PLUTO_SPATHI, 2);
	}
	else if (GET_GAME_STATE (BATTLE_SEGUE) == 1
		&& (Manner = GET_GAME_STATE (SPATHI_MANNER)) != 2)
	{
	    SET_GAME_STATE (SPATHI_MANNER, 1);
	    if (Manner != 1)
	    {
		SET_GAME_STATE (SPATHI_VISITS, 0);
		/* if don't know about Spathi via Melnorme . . . */
		if (GET_GAME_STATE (SPATHI_HOME_VISITS) != 7)
		{
		    SET_GAME_STATE (SPATHI_HOME_VISITS, 0);
		}
	    }
	}
    }
    POP_CONTEXT

    return (0);
}

PROC(
LOCDATAPTR far init_spathi_comm, (),
    ARG_VOID
)
{
    LOCDATAPTR	retval;

    PUSH_CONTEXT
    {
	spathi_desc.init_encounter_func = Intro;
	spathi_desc.uninit_encounter_func = uninit_spathi;

strcpy(aiff_folder, "comm/spathi/spath");

	if (GET_GAME_STATE (FOUND_PLUTO_SPATHI) == 1
		|| GET_GAME_STATE (SPATHI_MANNER) == 3
		|| LOBYTE (GLOBAL (CurrentActivity)) == WON_LAST_BATTLE)
	{
	    SET_GAME_STATE (BATTLE_SEGUE, 0);
	}
	else
	{
	    SET_GAME_STATE (BATTLE_SEGUE, 1);
	}
	retval = &spathi_desc;
    }
    POP_CONTEXT

    return (retval);
}
