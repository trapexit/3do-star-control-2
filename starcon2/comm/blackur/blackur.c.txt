#include "reslib.h"
#include "resinst.h"
#include "starcon.h"
#include "commglue.h"
#include "strings.h"

static LOCDATA	blackurq_desc =
{
    NULL_PTR,				/* init_encounter_func */
    NULL_PTR,				/* uninit_encounter_func */
    (FRAME)BLACKURQ_PMAP_ANIM,		/* AlienFrame */
    0,					/* AlienColorMap */
    BLACKURQ_MUSIC,			/* AlienSong */
    BLACKURQ_PLAYER_PHRASES,		/* PlayerPhrases */
    8,					/* NumAnimations */
    {
	{
	    7,				/* StartIndex */
	    6,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    8, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    13,				/* StartIndex */
	    7,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    8, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    20,				/* StartIndex */
	    3,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    8, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    23,				/* StartIndex */
	    3,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    8, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    26,				/* StartIndex */
	    3,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    8, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    29,				/* StartIndex */
	    4,				/* NumFrames */
	    RANDOM_ANIM,		/* AnimFlags */
	    12, 0,			/* FrameRate */
	    12, 0,			/* RestartRate */
	},
	{
	    33,				/* StartIndex */
	    5,				/* NumFrames */
	    CIRCULAR_ANIM
		    | WAIT_TALKING,	/* AnimFlags */
	    12, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    38,				/* StartIndex */
	    4,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    12, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	},
    },
    {
	1,				/* StartIndex */
	2,				/* NumFrames */
	0,				/* AnimFlags */
	20, 0,				/* FrameRate */
	0, 0,				/* RestartRate */
    },
    {
	2,				/* StartIndex */
	5,				/* NumFrames */
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

	if (PLAYER_SAID (R, bye))
	{
	    if (GET_GAME_STATE (KOHR_AH_BYES) == 0)
		NPCPhrase (GOODBYE_AND_DIE_1);
	    else
		NPCPhrase (GOODBYE_AND_DIE_2);

	    SET_GAME_STATE (KOHR_AH_BYES, 1);
	}
	else if (PLAYER_SAID (R, guess_thats_all))
	    NPCPhrase (THEN_DIE);
	else if (PLAYER_SAID (R, what_are_you_hovering_over))
	{
	    NPCPhrase (BONE_PILE);

	    SET_GAME_STATE (KOHR_AH_INFO, 1);
	}
	else if (PLAYER_SAID (R, you_sure_are_creepy))
	{
	    NPCPhrase (YES_CREEPY);

	    SET_GAME_STATE (KOHR_AH_INFO, 2);
	}
	else if (PLAYER_SAID (R, stop_that_gross_blinking))
	{
	    NPCPhrase (DIE_HUMAN);

	    SET_GAME_STATE (KOHR_AH_INFO, 3);
	}
	else if (PLAYER_SAID (R, threat_1)
		|| PLAYER_SAID (R, threat_2)
		|| PLAYER_SAID (R, threat_3)
		|| PLAYER_SAID (R, threat_4))
	{
	    NumVisits = GET_GAME_STATE (KOHR_AH_REASONS);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (RESISTANCE_IS_USELESS_1);
		    break;
		case 1:
		    NPCPhrase (RESISTANCE_IS_USELESS_2);
		    break;
		case 2:
		    NPCPhrase (RESISTANCE_IS_USELESS_3);
		    break;
		case 3:
		    NPCPhrase (RESISTANCE_IS_USELESS_4);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (KOHR_AH_REASONS, NumVisits);
	}
	else if (PLAYER_SAID (R, plead_1)
		|| PLAYER_SAID (R, plead_2)
		|| PLAYER_SAID (R, plead_3)
		|| PLAYER_SAID (R, plead_4))
	{
	    NumVisits = GET_GAME_STATE (KOHR_AH_PLEAD);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (PLEADING_IS_USELESS_1);
		    break;
		case 1:
		    NPCPhrase (PLEADING_IS_USELESS_2);
		    break;
		case 2:
		    NPCPhrase (PLEADING_IS_USELESS_3);
		    break;
		case 3:
		    NPCPhrase (PLEADING_IS_USELESS_4);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (KOHR_AH_PLEAD, NumVisits);
	}
	else if (PLAYER_SAID (R, why_kill_all_1)
		|| PLAYER_SAID (R, why_kill_all_2)
		|| PLAYER_SAID (R, why_kill_all_3)
		|| PLAYER_SAID (R, why_kill_all_4))
	{
	    NumVisits = GET_GAME_STATE (KOHR_AH_REASONS);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (KILL_BECAUSE_1);
		    break;
		case 1:
		    NPCPhrase (KILL_BECAUSE_2);
		    break;
		case 2:
		    NPCPhrase (KILL_BECAUSE_3);
		    break;
		case 3:
		    NPCPhrase (KILL_BECAUSE_4);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (KOHR_AH_REASONS, NumVisits);
	}
	else if (PLAYER_SAID (R, please_dont_kill_1)
		|| PLAYER_SAID (R, please_dont_kill_2)
		|| PLAYER_SAID (R, please_dont_kill_3)
		|| PLAYER_SAID (R, please_dont_kill_4))
	{
	    NumVisits = GET_GAME_STATE (KOHR_AH_PLEAD);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (WILL_KILL_1);
		    break;
		case 1:
		    NPCPhrase (WILL_KILL_2);
		    break;
		case 2:
		    NPCPhrase (WILL_KILL_3);
		    break;
		case 3:
		    NPCPhrase (WILL_KILL_4);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (KOHR_AH_PLEAD, NumVisits);
	}
	else if (PLAYER_SAID (R, bye_frenzy_1)
		|| PLAYER_SAID (R, bye_frenzy_2)
		|| PLAYER_SAID (R, bye_frenzy_3)
		|| PLAYER_SAID (R, bye_frenzy_4))
	{
	    NumVisits = GET_GAME_STATE (KOHR_AH_INFO);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (GOODBYE_AND_DIE_FRENZY_1);
		    break;
		case 1:
		    NPCPhrase (GOODBYE_AND_DIE_FRENZY_2);
		    break;
		case 2:
		    NPCPhrase (GOODBYE_AND_DIE_FRENZY_3);
		    break;
		case 3:
		    NPCPhrase (GOODBYE_AND_DIE_FRENZY_4);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (KOHR_AH_INFO, NumVisits);
	}
    }
    POP_CONTEXT
}

PROC(STATIC
void far Frenzy, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	switch (GET_GAME_STATE (KOHR_AH_REASONS))
	{
	    case 0:
		Response (why_kill_all_1, CombatIsInevitable);
		break;
	    case 1:
		Response (why_kill_all_2, CombatIsInevitable);
		break;
	    case 2:
		Response (why_kill_all_3, CombatIsInevitable);
		break;
	    case 3:
		Response (why_kill_all_4, CombatIsInevitable);
		break;
	}
	switch (GET_GAME_STATE (KOHR_AH_PLEAD))
	{
	    case 0:
		Response (please_dont_kill_1, CombatIsInevitable);
		break;
	    case 1:
		Response (please_dont_kill_2, CombatIsInevitable);
		break;
	    case 2:
		Response (please_dont_kill_3, CombatIsInevitable);
		break;
	    case 3:
		Response (please_dont_kill_4, CombatIsInevitable);
		break;
	}
	switch (GET_GAME_STATE (KOHR_AH_INFO))
	{
	    case 0:
		Response (bye_frenzy_1, CombatIsInevitable);
		break;
	    case 1:
		Response (bye_frenzy_2, CombatIsInevitable);
		break;
	    case 2:
		Response (bye_frenzy_3, CombatIsInevitable);
		break;
	    case 3:
		Response (bye_frenzy_4, CombatIsInevitable);
		break;
	}
    }
    POP_CONTEXT
}

PROC(STATIC
void far KohrAhStory, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	PTR_DESC	PtrDesc;

	if (PLAYER_SAID (R, key_phrase))
	{
	    NPCPhrase (RESPONSE_TO_KEY_PHRASE);

	    SET_GAME_STATE (KNOW_KOHR_AH_STORY, 2);
	}
	else if (PLAYER_SAID (R, why_do_you_destroy))
	{
	    NPCPhrase (WE_WERE_SLAVES);

	    DISABLE_PHRASE (why_do_you_destroy);
	}
	else if (PLAYER_SAID (R, relationship_with_urquan))
	{
	    NPCPhrase (WE_ARE_URQUAN_TOO);

	    DISABLE_PHRASE (relationship_with_urquan);
	}
	else if (PLAYER_SAID (R, what_about_culture))
	{
	    NPCPhrase (BONE_GARDENS);

	    DISABLE_PHRASE (what_about_culture);
	}
	else if (PLAYER_SAID (R, how_leave_me_alone))
	{
	    NPCPhrase (YOU_DIE);

	    DISABLE_PHRASE (how_leave_me_alone);
	}

	if (PHRASE_ENABLED (why_do_you_destroy))
	    Response (why_do_you_destroy, KohrAhStory);
	if (PHRASE_ENABLED (relationship_with_urquan))
	    Response (relationship_with_urquan, KohrAhStory);
	if (PHRASE_ENABLED (what_about_culture))
	    Response (what_about_culture, KohrAhStory);
	if (PHRASE_ENABLED (how_leave_me_alone))
	    Response (how_leave_me_alone, KohrAhStory);
	Response (guess_thats_all, CombatIsInevitable);
    }
    POP_CONTEXT
}

PROC(STATIC
void far DieHuman, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	switch (GET_GAME_STATE (KOHR_AH_REASONS))
	{
	    case 0:
		Response (threat_1, CombatIsInevitable);
		break;
	    case 1:
		Response (threat_2, CombatIsInevitable);
		break;
	    case 2:
		Response (threat_3, CombatIsInevitable);
		break;
	    case 3:
		Response (threat_4, CombatIsInevitable);
		break;
	}
	if (GET_GAME_STATE (KNOW_KOHR_AH_STORY) == 1)
	{
	    Response (key_phrase, KohrAhStory);
	}
	switch (GET_GAME_STATE (KOHR_AH_INFO))
	{
	    case 0:
		Response (what_are_you_hovering_over, CombatIsInevitable);
		break;
	    case 1:
		Response (you_sure_are_creepy, CombatIsInevitable);
		break;
	    case 2:
		Response (stop_that_gross_blinking, CombatIsInevitable);
		break;
	}
	switch (GET_GAME_STATE (KOHR_AH_PLEAD))
	{
	    case 0:
		Response (plead_1, CombatIsInevitable);
		break;
	    case 1:
		Response (plead_2, CombatIsInevitable);
		break;
	    case 2:
		Response (plead_3, CombatIsInevitable);
		break;
	    case 3:
		Response (plead_4, CombatIsInevitable);
		break;
	}
	Response (bye, CombatIsInevitable);
    }
    POP_CONTEXT
}

static void	far
Intro ()
{
    PUSH_CONTEXT
    {
	BYTE	b0, b1, b2, b3;

	if (LOBYTE (GLOBAL (CurrentActivity)) == WON_LAST_BATTLE)
	{
//	    NPCPhrase (OUT_TAKES);

	    SET_GAME_STATE (BATTLE_SEGUE, 0);
	    goto ExitIntro;
	}

	if (GET_GAME_STATE (KOHR_AH_KILLED_ALL))
	{
	    NPCPhrase (GAME_OVER_DUDE);

	    SET_GAME_STATE (BATTLE_SEGUE, 0);
	    goto ExitIntro;
	}

	if (!GET_GAME_STATE (KOHR_AH_SENSES_EVIL)
		&& GET_GAME_STATE (TALKING_PET_ON_SHIP))
	{
	    NPCPhrase (SENSE_EVIL);
	    SET_GAME_STATE (KOHR_AH_SENSES_EVIL, 1);
	}

	b0 = GET_GAME_STATE (SAMATRA_GRPOFFS0);
	b1 = GET_GAME_STATE (SAMATRA_GRPOFFS1);
	b2 = GET_GAME_STATE (SAMATRA_GRPOFFS2);
	b3 = GET_GAME_STATE (SAMATRA_GRPOFFS3);
	if (LOBYTE (GLOBAL (CurrentActivity)) == IN_INTERPLANETARY
		&& GLOBAL (BattleGroupRef)
		&& GLOBAL (BattleGroupRef) == MAKE_DWORD (
			MAKE_WORD (b0, b1), MAKE_WORD (b2, b3)
			))
	{
	    NPCPhrase (HELLO_SAMATRA);

	    SET_GAME_STATE (BATTLE_SEGUE, 1);
	}
	else
	{
	    BYTE	NumVisits;

	    NumVisits = GET_GAME_STATE (KOHR_AH_VISITS);
	    if (GET_GAME_STATE (KOHR_AH_FRENZY))
	    {
		switch (NumVisits++)
		{
		    case 0:
			NPCPhrase (WE_KILL_ALL_1);
			break;
		    case 1:
			NPCPhrase (WE_KILL_ALL_2);
			break;
		    case 2:
			NPCPhrase (WE_KILL_ALL_3);
			break;
		    case 3:
			NPCPhrase (WE_KILL_ALL_4);
			--NumVisits;
			break;
		}

		Frenzy ((RESPONSE_REF)0);
	    }
	    else
	    {
		switch (NumVisits++)
		{
		    case 0:
			NPCPhrase (HELLO_AND_DIE_1);
			break;
		    case 1:
			NPCPhrase (HELLO_AND_DIE_2);
			break;
		    case 2:
			NPCPhrase (HELLO_AND_DIE_3);
			break;
		    case 3:
			NPCPhrase (HELLO_AND_DIE_4);
			--NumVisits;
			break;
		}

		DieHuman ((RESPONSE_REF)0);
	    }
	    SET_GAME_STATE (KOHR_AH_VISITS, NumVisits);
	}
ExitIntro:
	;
    }
    POP_CONTEXT
}

PROC(STATIC
COUNT far uninit_blackurq, (),
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
LOCDATAPTR far init_blackurq_comm, (),
    ARG_VOID
)
{
    LOCDATAPTR	retval;

    PUSH_CONTEXT
    {
	blackurq_desc.init_encounter_func = Intro;
	blackurq_desc.uninit_encounter_func = uninit_blackurq;

strcpy(aiff_folder, "comm/blackur/black");

	if (!GET_GAME_STATE (KOHR_AH_KILLED_ALL)
		&& LOBYTE (GLOBAL (CurrentActivity)) != WON_LAST_BATTLE)
	{
	    SET_GAME_STATE (BATTLE_SEGUE, 1);
	}
	else
	{
	    SET_GAME_STATE (BATTLE_SEGUE, 0);
	}
	retval = &blackurq_desc;
    }
    POP_CONTEXT

    return (retval);
}
