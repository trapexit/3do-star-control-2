#include "reslib.h"
#include "resinst.h"
#include "starcon.h"
#include "commglue.h"
#include "strings.h"

static LOCDATA	vux_desc =
{
    NULL_PTR,				/* init_encounter_func */
    NULL_PTR,				/* uninit_encounter_func */
    (FRAME)VUX_PMAP_ANIM,		/* AlienFrame */
    VUX_COLOR_MAP,			/* AlienColorMap */
    VUX_MUSIC,				/* AlienSong */
    VUX_PLAYER_PHRASES,			/* PlayerPhrases */
    17,					/* NumAnimations */
    {
	{
	    12,				/* StartIndex */
	    3,				/* NumFrames */
	    RANDOM_ANIM,		/* AnimFlags */
	    4, 4,			/* FrameRate */
	    4, 4,			/* RestartRate */
	},
	{
	    15,				/* StartIndex */
	    5,				/* NumFrames */
	    RANDOM_ANIM,		/* AnimFlags */
	    4, 4,			/* FrameRate */
	    4, 4,			/* RestartRate */
	},
	{
	    20,				/* StartIndex */
	    14,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    4, 0,			/* FrameRate */
	    4, 0,			/* RestartRate */
	},
	{
	    34,				/* StartIndex */
	    7,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    4, 4,			/* FrameRate */
	    4, 4,			/* RestartRate */
	},
	{
	    41,				/* StartIndex */
	    6,				/* NumFrames */
	    RANDOM_ANIM,		/* AnimFlags */
	    4, 4,			/* FrameRate */
	    4, 4,			/* RestartRate */
	},
	{
	    47,				/* StartIndex */
	    11,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    4, 4,			/* FrameRate */
	    4, 4,			/* RestartRate */
	},
	{
	    58,				/* StartIndex */
	    3,				/* NumFrames */
	    RANDOM_ANIM,		/* AnimFlags */
	    4, 4,			/* FrameRate */
	    4, 4,			/* RestartRate */
	},
	{
	    61,				/* StartIndex */
	    4,				/* NumFrames */
	    RANDOM_ANIM,		/* AnimFlags */
	    4, 4,			/* FrameRate */
	    4, 4,			/* RestartRate */
	},
	{
	    65,				/* StartIndex */
	    4,				/* NumFrames */
	    RANDOM_ANIM,		/* AnimFlags */
	    4, 4,			/* FrameRate */
	    4, 4,			/* RestartRate */
	},
	{
	    69,				/* StartIndex */
	    2,				/* NumFrames */
	    RANDOM_ANIM,		/* AnimFlags */
	    4, 4,			/* FrameRate */
	    4, 4,			/* RestartRate */
	},
	{
	    71,				/* StartIndex */
	    3,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    6, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    74,				/* StartIndex */
	    6,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    8, 8,			/* FrameRate */
	    8, 8,			/* FrameRate */
	},
	{
	    80,				/* StartIndex */
	    5,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    8, 8,			/* FrameRate */
	    8, 8,			/* FrameRate */
	    (1 << 14),			/* BlockMask */
	},
	{
	    85,				/* StartIndex */
	    5,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    8, 8,			/* FrameRate */
	    8, 8,			/* FrameRate */
	},
	{
	    90,				/* StartIndex */
	    5,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    8, 8,			/* FrameRate */
	    8, 8,			/* FrameRate */
	    (1 << 12),			/* BlockMask */
	},
	{
	    95,				/* StartIndex */
	    4,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    8, 8,			/* FrameRate */
	    ONE_SECOND * 5, ONE_SECOND * 5,/* RestartRate */
	},
	{
	    99,				/* StartIndex */
	    4,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    8, 8,			/* FrameRate */
	    ONE_SECOND * 5, ONE_SECOND * 5,/* RestartRate */
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
	8, 0,				/* FrameRate */
	10, 0,				/* RestartRate */
    },
};

PROC(STATIC
void far CombatIsInevitable, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	BYTE	NumVisits;

	SET_GAME_STATE (BATTLE_SEGUE, 1);

	if (PLAYER_SAID (R, ok_take_beast))
	{
	    NPCPhrase (FOOL_AIEE0);
	    NPCPhrase (FOOL_AIEE1);

	    AlienTalkSegue (1);
	    XFormPLUT (GetColorMapAddress (
		    SetAbsColorMapIndex (CommData.AlienColorMap, 1)
		    ), ONE_SECOND / 4);
	    AlienTalkSegue ((COUNT)~0);

	    SET_GAME_STATE (VUX_BEAST_ON_SHIP, 0);
	    SET_GAME_STATE (ZEX_IS_DEAD, 1);
	    SET_GAME_STATE (BATTLE_SEGUE, 0);
	}
	else if (PLAYER_SAID (R, try_any_way))
	{
	    NPCPhrase (NOPE);

	    SET_GAME_STATE (VUX_STACK_1, 4);
	}
	else if (PLAYER_SAID (R, kill_you_squids_1)
		|| PLAYER_SAID (R, kill_you_squids_2)
		|| PLAYER_SAID (R, kill_you_squids_3)
		|| PLAYER_SAID (R, kill_you_squids_4))
	{
	    NPCPhrase (WE_FIGHT);

	    NumVisits = GET_GAME_STATE (VUX_STACK_2) + 1;
	    if (NumVisits <= 3)
	    {
		SET_GAME_STATE (VUX_STACK_2, NumVisits);
	    }
	}
	else if (PLAYER_SAID (R, cant_we_be_friends_1)
		|| PLAYER_SAID (R, cant_we_be_friends_2)
		|| PLAYER_SAID (R, cant_we_be_friends_3)
		|| PLAYER_SAID (R, cant_we_be_friends_4))
	{
	    NumVisits = GET_GAME_STATE (VUX_STACK_3);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (NEVER_UGLY_HUMANS_1);
		    break;
		case 1:
		    NPCPhrase (NEVER_UGLY_HUMANS_2);
		    break;
		case 2:
		    NPCPhrase (NEVER_UGLY_HUMANS_3);
		    break;
		case 3:
		    NPCPhrase (NEVER_UGLY_HUMANS_4);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (VUX_STACK_3, NumVisits);
	}
	else if (PLAYER_SAID (R, bye_hostile_space))
	{
	    NumVisits = GET_GAME_STATE (VUX_STACK_4);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (BYE_AND_DIE_HSTL_SPACE_1);
		    break;
		case 1:
		    NPCPhrase (BYE_AND_DIE_HSTL_SPACE_2);
		    break;
		case 2:
		    NPCPhrase (BYE_AND_DIE_HSTL_SPACE_3);
		    break;
		case 3:
		    NPCPhrase (BYE_AND_DIE_HSTL_SPACE_4);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (VUX_STACK_4, NumVisits);
	}
	else if (PLAYER_SAID (R, bye_zex))
	{
	    NPCPhrase (GOODBYE_ZEX);

	    SET_GAME_STATE (BATTLE_SEGUE, 0);
	}
	else
	{
	    NumVisits = GET_GAME_STATE (VUX_STACK_1);
	    switch (NumVisits++)
	    {
		case 4:
		    NPCPhrase (NOT_ACCEPTED_1);
		    break;
		case 5:
		    NPCPhrase (NOT_ACCEPTED_2);
		    break;
		case 6:
		    NPCPhrase (NOT_ACCEPTED_3);
		    break;
		case 7:
		    NPCPhrase (NOT_ACCEPTED_4);
		    break;
		case 8:
		    NPCPhrase (NOT_ACCEPTED_5);
		    break;
		case 9:
		    NPCPhrase (NOT_ACCEPTED_6);
		    break;
		case 10:
		    NPCPhrase (NOT_ACCEPTED_7);
		    break;
		case 11:
		    NPCPhrase (NOT_ACCEPTED_8);
		    break;
		case 12:
		    NPCPhrase (NOT_ACCEPTED_9);
		    break;
		case 13:
		    NPCPhrase (TRUTH);
		    break;
	    }
	    SET_GAME_STATE (VUX_STACK_1, NumVisits);
	}
    }
    POP_CONTEXT
}

PROC(STATIC
void far Menagerie, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	BYTE		i, LastStack;
	RESPONSE_REF		pStr[3];
	PTR_DESC	PtrDesc;

	if (PLAYER_SAID (R, i_have_beast)
		|| PLAYER_SAID (R, why_trust_1)
		|| PLAYER_SAID (R, why_trust_2)
		|| PLAYER_SAID (R, why_trust_3))
	{
	    if (PLAYER_SAID (R, i_have_beast))
		NPCPhrase (GIVE_BEAST);
	    else if (PLAYER_SAID (R, why_trust_1))
	    {
		NPCPhrase (TRUST_1);

		DISABLE_PHRASE (why_trust_1);
	    }
	    else if (PLAYER_SAID (R, why_trust_2))
	    {
		NPCPhrase (TRUST_2);

		DISABLE_PHRASE (why_trust_2);
	    }
	    else if (PLAYER_SAID (R, why_trust_3))
	    {
		NPCPhrase (TRUST_3);

		DISABLE_PHRASE (why_trust_3);
	    }

	    if (PHRASE_ENABLED (why_trust_1))
		Response (why_trust_1, Menagerie);
	    else if (PHRASE_ENABLED (why_trust_2))
		Response (why_trust_2, Menagerie);
	    else if (PHRASE_ENABLED (why_trust_3))
		Response (why_trust_3, Menagerie);
	    Response (ok_take_beast, CombatIsInevitable);
	}
	else if (PLAYER_SAID (R, kill_you))
	{
	    NPCPhrase (FIGHT_AGAIN);

	    SET_GAME_STATE (BATTLE_SEGUE, 1);
	}
	else if (PLAYER_SAID (R, regardless))
	{
	    NPCPhrase (THEN_FIGHT);

	    SET_GAME_STATE (BATTLE_SEGUE, 1);
	    SET_GAME_STATE (ZEX_STACK_3, 2);
	    SET_GAME_STATE (ZEX_VISITS, 0);
	}
	else
	{
	    LastStack = 0;
	    pStr[0] = pStr[1] = pStr[2] = 0;
	    if (R == 0)
	    {
		BYTE	NumVisits;

		NumVisits = GET_GAME_STATE (ZEX_VISITS);
		if (GET_GAME_STATE (ZEX_STACK_3) >= 2)
		{
		    switch (NumVisits++)
		    {
			case 0:
			    NPCPhrase (FIGHT_OR_TRADE_1);
			    break;
			case 1:
			    NPCPhrase (FIGHT_OR_TRADE_2);
			    --NumVisits;
			    break;
		    }
		}
		else
		{
		    switch (NumVisits++)
		    {
			case 0:
			    NPCPhrase (ZEX_HELLO_1);
			    break;
			case 1:
			    NPCPhrase (ZEX_HELLO_2);
			    break;
			case 2:
			    NPCPhrase (ZEX_HELLO_3);
			    break;
			case 3:
			    NPCPhrase (ZEX_HELLO_4);
			    --NumVisits;
			    break;
		    }
		}
		SET_GAME_STATE (ZEX_VISITS, NumVisits);
	    }
	    else if (PLAYER_SAID (R, what_you_do_here))
	    {
		NPCPhrase (MY_MENAGERIE);

		SET_GAME_STATE (ZEX_STACK_1, 1);
	    }
	    else if (PLAYER_SAID (R, what_about_menagerie))
	    {
		NPCPhrase (NEED_NEW_CREATURE);

		SET_GAME_STATE (ZEX_STACK_1, 2);
	    }
	    else if (PLAYER_SAID (R, what_about_creature))
	    {
		NPCPhrase (ABOUT_CREATURE);

		SET_GAME_STATE (KNOW_ZEX_WANTS_MONSTER, 1);
		SET_GAME_STATE (ZEX_STACK_1, 3);

		R = about_creature_again;
		DISABLE_PHRASE (what_about_creature);
	    }
	    else if (PLAYER_SAID (R, about_creature_again))
	    {
		NPCPhrase (CREATURE_AGAIN);

		DISABLE_PHRASE (about_creature_again);
	    }
	    else if (PLAYER_SAID (R, why_dont_you_attack))
	    {
		NPCPhrase (LIKE_YOU);

		LastStack = 1;
		SET_GAME_STATE (ZEX_STACK_2, 1);
	    }
	    else if (PLAYER_SAID (R, why_like_me))
	    {
		NPCPhrase (LIKE_BECAUSE);

		LastStack = 1;
		SET_GAME_STATE (ZEX_STACK_2, 2);
	    }
	    else if (PLAYER_SAID (R, are_you_a_pervert))
	    {
		NPCPhrase (CALL_ME_WHAT_YOU_WISH);

		SET_GAME_STATE (ZEX_STACK_2, 3);
	    }
	    else if (PLAYER_SAID (R, take_by_force))
	    {
		NPCPhrase (PRECURSOR_DEVICE);

		LastStack = 2;
		SET_GAME_STATE (ZEX_STACK_3, 1);
	    }
	    else if (PLAYER_SAID (R, you_lied))
	    {
		NPCPhrase (YUP_LIED);

		LastStack = 2;
		SET_GAME_STATE (ZEX_STACK_3, 3);
	    }

	    if (GET_GAME_STATE (KNOW_ZEX_WANTS_MONSTER)
		    && GET_GAME_STATE (VUX_BEAST_ON_SHIP))
		pStr[0] = i_have_beast;
	    else
	    {
		switch (GET_GAME_STATE (ZEX_STACK_1))
		{
		    case 0:
			pStr[0] = what_you_do_here;
			break;
		    case 1:
			pStr[0] = what_about_menagerie;
			break;
		    case 2:
			pStr[0] = what_about_creature;
			break;
		    case 3:
			if (PHRASE_ENABLED (about_creature_again))
			    pStr[0] = about_creature_again;
			break;
		}
	    }
	    switch (GET_GAME_STATE (ZEX_STACK_2))
	    {
		case 0:
		    pStr[1] = why_dont_you_attack;
		    break;
		case 1:
		    pStr[1] = why_like_me;
		    break;
		case 2:
		    pStr[1] = are_you_a_pervert;
		    break;
	    }
	    switch (GET_GAME_STATE (ZEX_STACK_3))
	    {
		case 0:
		    pStr[2] = take_by_force;
		    break;
		case 1:
		    pStr[2] = regardless;
		    break;
		case 2:
		    pStr[2] = you_lied;
		    break;
		case 3:
		    pStr[2] = kill_you;
		    break;
	    }

	    if (pStr[LastStack])
		Response (pStr[LastStack], Menagerie);
	    for (i = 0; i < 3; ++i)
	    {
		if (i != LastStack && pStr[i])
		    Response (pStr[i], Menagerie);
	    }
	    Response (bye_zex, CombatIsInevitable);
	}
    }
    POP_CONTEXT
}

PROC(STATIC
void far NormalVux, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	PTR_DESC	PtrDesc;

	if (PLAYER_SAID (R, why_so_mean))
	{
	    NPCPhrase (URQUAN_SLAVES);

	    SET_GAME_STATE (VUX_STACK_1, 1);
	}
	else if (PLAYER_SAID (R, deeper_reason))
	{
	    NPCPhrase (OLD_INSULT);

	    SET_GAME_STATE (VUX_STACK_1, 2);
	}
	else if (PLAYER_SAID (R, if_we_apologize))
	{
	    NPCPhrase (PROBABLY_NOT);

	    SET_GAME_STATE (VUX_STACK_1, 3);
	}
	else if (PLAYER_SAID (R, whats_up_hostile))
	{
	    BYTE	NumVisits;

	    NumVisits = GET_GAME_STATE (VUX_INFO);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (GENERAL_INFO_HOSTILE_1);
		    break;
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
	    SET_GAME_STATE (VUX_INFO, NumVisits);

	    DISABLE_PHRASE (whats_up_hostile);
	}

	switch (GET_GAME_STATE (VUX_STACK_1))
	{
	    case 0:
		Response (why_so_mean, NormalVux);
		break;
	    case 1:
		Response (deeper_reason, NormalVux);
		break;
	    case 2:
		Response (if_we_apologize, NormalVux);
		break;
	    case 3:
		Response (try_any_way, CombatIsInevitable);
		break;
	    case 4:
		Response (apology_1, CombatIsInevitable);
		break;
	    case 5:
		Response (apology_2, CombatIsInevitable);
		break;
	    case 6:
		Response (apology_3, CombatIsInevitable);
		break;
	    case 7:
		Response (apology_4, CombatIsInevitable);
		break;
	    case 8:
		Response (apology_5, CombatIsInevitable);
		break;
	    case 9:
		Response (apology_6, CombatIsInevitable);
		break;
	    case 10:
		Response (apology_7, CombatIsInevitable);
		break;
	    case 11:
		Response (apology_8, CombatIsInevitable);
		break;
	    case 12:
		Response (apology_9, CombatIsInevitable);
		break;
	    case 13:
		Response (apology_10, CombatIsInevitable);
		break;
	}

	switch (GET_GAME_STATE (VUX_STACK_2))
	{
	    case 0:
		Response (kill_you_squids_1, CombatIsInevitable);
		break;
	    case 1:
		Response (kill_you_squids_2, CombatIsInevitable);
		break;
	    case 2:
		Response (kill_you_squids_3, CombatIsInevitable);
		break;
	    case 3:
		Response (kill_you_squids_4, CombatIsInevitable);
		break;
	}

	if (PHRASE_ENABLED (whats_up_hostile))
	{
	    Response (whats_up_hostile, NormalVux);
	}

	if (GET_GAME_STATE (VUX_STACK_1) > 13)
	{
	    switch (GET_GAME_STATE (VUX_STACK_3))
	    {
		case 0:
		    Response (cant_we_be_friends_1, CombatIsInevitable);
		    break;
		case 1:
		    Response (cant_we_be_friends_2, CombatIsInevitable);
		    break;
		case 2:
		    Response (cant_we_be_friends_3, CombatIsInevitable);
		    break;
		case 3:
		    Response (cant_we_be_friends_4, CombatIsInevitable);
		    break;
	    }
	}

	Response (bye_hostile_space, CombatIsInevitable);
    }
    POP_CONTEXT
}

static void	far
Intro ()
{
    PUSH_CONTEXT
    {
	if (LOBYTE (GLOBAL (CurrentActivity)) == WON_LAST_BATTLE)
	{
//	    NPCPhrase (OUT_TAKES);

	    SET_GAME_STATE (BATTLE_SEGUE, 0);
	    goto ExitIntro;
	}

	if (GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 6))
	{
	    Menagerie ((RESPONSE_REF)0);
	}
	else
	{
	    BYTE	NumVisits;

	    if (GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 7))
	    {
		NumVisits = GET_GAME_STATE (VUX_HOME_VISITS);
		switch (NumVisits++)
		{
		    case 0:
			NPCPhrase (HOMEWORLD_HELLO_1);
			break;
		    case 1:
			NPCPhrase (HOMEWORLD_HELLO_2);
			break;
		    case 2:
			NPCPhrase (HOMEWORLD_HELLO_3);
			break;
		    case 3:
			NPCPhrase (HOMEWORLD_HELLO_4);
			--NumVisits;
			break;
		}
		SET_GAME_STATE (VUX_HOME_VISITS, NumVisits);
	    }
	    else
	    {
		NumVisits = GET_GAME_STATE (VUX_VISITS);
		switch (NumVisits++)
		{
		    case 0:
			NPCPhrase (SPACE_HELLO_1);
			break;
		    case 1:
			NPCPhrase (SPACE_HELLO_2);
			break;
		    case 2:
			NPCPhrase (SPACE_HELLO_3);
			break;
		    case 3:
			NPCPhrase (SPACE_HELLO_4);
			--NumVisits;
			break;
		}
		SET_GAME_STATE (VUX_VISITS, NumVisits);
	    }

	    NormalVux ((RESPONSE_REF)0);
	}
ExitIntro:
	;
    }
    POP_CONTEXT
}

PROC(STATIC
COUNT far uninit_vux, (),
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
LOCDATAPTR far init_vux_comm, (),
    ARG_VOID
)
{
    LOCDATAPTR	retval;

    PUSH_CONTEXT
    {
	vux_desc.init_encounter_func = Intro;
	vux_desc.uninit_encounter_func = uninit_vux;

strcpy(aiff_folder, "comm/vux/vux");

	if ((GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 6))
		|| LOBYTE (GLOBAL (CurrentActivity)) == WON_LAST_BATTLE)
	{
	    SET_GAME_STATE (BATTLE_SEGUE, 0);
	}
	else
	{
	    SET_GAME_STATE (BATTLE_SEGUE, 1);
	}
	retval = &vux_desc;
    }
    POP_CONTEXT

    return (retval);
}
