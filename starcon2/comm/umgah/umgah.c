#include "reslib.h"
#include "resinst.h"
#include "starcon.h"
#include "commglue.h"
#include "strings.h"

static LOCDATA	umgah_desc =
{
    NULL_PTR,				/* init_encounter_func */
    NULL_PTR,				/* uninit_encounter_func */
    (FRAME)UMGAH_PMAP_ANIM,		/* AlienFrame */
    0,					/* AlienColorMap */
    UMGAH_MUSIC,			/* AlienSong */
    UMGAH_PLAYER_PHRASES,		/* PlayerPhrases */
    16,					/* NumAnimations */
    {
	{
	    5,				/* StartIndex */
	    3,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    6, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	    (1 << 5),
	},
	{
	    8,				/* StartIndex */
	    3,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    6, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	    (1 << 6),
	},
	{
	    11,				/* StartIndex */
	    2,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    6, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	    (1 << 7),
	},
	{
	    13,				/* StartIndex */
	    2,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    6, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	    (1 << 8),
	},
	{
	    15,				/* StartIndex */
	    2,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    6, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	    (1 << 9),
	},
	{
	    17,				/* StartIndex */
	    3,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    14, 0,			/* FrameRate */
	    ONE_SECOND * 3, ONE_SECOND * 3,	/* RestartRate */
	    (1 << 0),
	},
	{
	    20,				/* StartIndex */
	    3,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    14, 0,			/* FrameRate */
	    ONE_SECOND * 3, ONE_SECOND * 3,	/* RestartRate */
	    (1 << 1),
	},
	{
	    23,				/* StartIndex */
	    2,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    14, 0,			/* FrameRate */
	    ONE_SECOND * 3, ONE_SECOND * 3,	/* RestartRate */
	    (1 << 2),
	},
	{
	    25,				/* StartIndex */
	    2,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    14, 0,			/* FrameRate */
	    ONE_SECOND * 3, ONE_SECOND * 3,	/* RestartRate */
	    (1 << 3),
	},
	{
	    27,				/* StartIndex */
	    2,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    14, 0,			/* FrameRate */
	    ONE_SECOND * 3, ONE_SECOND * 3,	/* RestartRate */
	    (1 << 4),
	},
	{
	    29,				/* StartIndex */
	    3,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    12, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    32,				/* StartIndex */
	    3,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    12, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    35,				/* StartIndex */
	    5,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    12, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    40,				/* StartIndex */
	    6,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    12, 0,			/* FrameRate */
	    12, 0,			/* RestartRate */
	},
	{
	    46,				/* StartIndex */
	    2,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    24, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	    (1 << 15),			/* BlockMask */
	},
	{
	    48,				/* StartIndex */
	    2,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    24, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	    (1 << 14),			/* BlockMask */
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
	4,				/* NumFrames */
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
	SET_GAME_STATE (BATTLE_SEGUE, 1);

	if (PLAYER_SAID (R, bye_zombie))
	{
	    NPCPhrase (GOODBYE_ZOMBIE);

	    SET_GAME_STATE (BATTLE_SEGUE, 0);
	}
	else if (PLAYER_SAID (R, bye_pre_zombie))
	    NPCPhrase (GOODBYE_PRE_ZOMBIE);
	else if (PLAYER_SAID (R, can_we_be_friends))
	{
	    NPCPhrase (SURE_FRIENDS);

	    SET_GAME_STATE (UMGAH_MENTIONED_TRICKS, 1);
	}
	else if (PLAYER_SAID (R, evil_blobbies_give_up))
	{
	    NPCPhrase (NOT_EVIL_BLOBBIES);

	    SET_GAME_STATE (UMGAH_EVIL_BLOBBIES, 1);
	}
	else if (PLAYER_SAID (R, evil_blobbies_must_die))
	    NPCPhrase (OH_NO_WE_WONT);
	else if (PLAYER_SAID (R, threat))
	    NPCPhrase (NO_THREAT);
	else if (PLAYER_SAID (R, dont_believe))
	{
	    NPCPhrase (THEN_DIE);

	    SET_GAME_STATE (KNOW_UMGAH_ZOMBIES, 1);
	    SET_GAME_STATE (UMGAH_VISITS, 0);
	}
	else if (PLAYER_SAID (R, bye_unknown))
	{
	    NPCPhrase (GOODBYE_UNKNOWN);

	    SET_GAME_STATE (BATTLE_SEGUE, 0);
	}
	else if (PLAYER_SAID (R, bye_post_zombie))
	{
	    NPCPhrase (FUNNY_IDEA);

	    AlienTalkSegue ((COUNT)~0);
	    ActivateStarShip (UMGAH_SHIP, 4);
	    SET_GAME_STATE (UMGAH_HOSTILE, 1);
	}
    }
    POP_CONTEXT
}

PROC(STATIC
void far Zombies, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	if (GET_GAME_STATE (MET_NORMAL_UMGAH))
	{
	    if (PLAYER_SAID (R, whats_up_zombies))
	    {
		NPCPhrase (GENERAL_INFO_ZOMBIE);

		DISABLE_PHRASE (whats_up_zombies);
	    }
	    else if (PLAYER_SAID (R, how_goes_tpet))
	    {
		NPCPhrase (WHAT_TPET);

		DISABLE_PHRASE (how_goes_tpet);
	    }
	    else if (PLAYER_SAID (R, you_told_us))
	    {
		NPCPhrase (SADLY_IT_DIED);

		DISABLE_PHRASE (you_told_us);
	    }

	    if (PHRASE_ENABLED (whats_up_zombies) && PHRASE_ENABLED (how_goes_tpet))
		Response (whats_up_zombies, Zombies);
	    if (PHRASE_ENABLED (how_goes_tpet))
		Response (how_goes_tpet, Zombies);
	    else if (PHRASE_ENABLED (you_told_us))
		Response (you_told_us, Zombies);
	    else
	    {
		Response (dont_believe, CombatIsInevitable);
	    }
	    if (PHRASE_ENABLED (whats_up_zombies) && !PHRASE_ENABLED (how_goes_tpet))
		Response (whats_up_zombies, Zombies);
	    Response (threat, CombatIsInevitable);
	    Response (bye_unknown, CombatIsInevitable);
	}
	else
	{
	    BYTE		i, LastStack;
	    RESPONSE_REF	pStr[4];

	    LastStack = 0;
	    pStr[0] = pStr[1] = pStr[2] = pStr[3] = 0;
	    if (PLAYER_SAID (R, evil_blobbies))
	    {
		NPCPhrase (YES_VERY_EVIL);

		DISABLE_PHRASE (evil_blobbies);
		LastStack = 0;
	    }
	    else if (PLAYER_SAID (R, we_vindicator0))
	    {
		NPCPhrase (GOOD_FOR_YOU_1);

		DISABLE_PHRASE (we_vindicator0);
		LastStack = 1;
	    }
	    else if (PLAYER_SAID (R, come_in_peace))
	    {
		NPCPhrase (GOOD_FOR_YOU_2);

		DISABLE_PHRASE (come_in_peace);
		LastStack = 1;
	    }
	    else if (PLAYER_SAID (R, know_any_jokes))
	    {
		NPCPhrase (JOKE_1);

		DISABLE_PHRASE (know_any_jokes);
		LastStack = 2;
	    }
	    else if (PLAYER_SAID (R, better_joke))
	    {
		NPCPhrase (JOKE_2);

		DISABLE_PHRASE (better_joke);
		LastStack = 2;
	    }
	    else if (PLAYER_SAID (R, not_very_funny))
	    {
		NPCPhrase (YES_WE_ARE);

		DISABLE_PHRASE (not_very_funny);
		LastStack = 2;
	    }
	    else if (PLAYER_SAID (R, what_about_tpet))
	    {
		NPCPhrase (WHAT_TPET);

		DISABLE_PHRASE (what_about_tpet);
		LastStack = 3;
	    }
	    else if (PLAYER_SAID (R, give_up_or_die))
	    {
		NPCPhrase (NOT_GIVE_UP);

		SET_GAME_STATE (BATTLE_SEGUE, 1);
		goto ExitZombies;
	    }
	    else if (PLAYER_SAID (R, arilou_told_us))
	    {
		NPCPhrase (THEN_DIE);

		SET_GAME_STATE (BATTLE_SEGUE, 1);
		SET_GAME_STATE (KNOW_UMGAH_ZOMBIES, 1);
		SET_GAME_STATE (UMGAH_VISITS, 0);
		goto ExitZombies;
	    }

	    if (PHRASE_ENABLED (evil_blobbies))
		pStr[0] = evil_blobbies;
	    else
		pStr[0] = give_up_or_die;

	    if (PHRASE_ENABLED (we_vindicator0))
	    {
		construct_response (shared_phrase_buf,
			we_vindicator0,
	    		GLOBAL_SIS (CommanderName),
			we_vindicator1,
			GLOBAL_SIS (ShipName),
			we_vindicator2,
			0);
		pStr[1] = we_vindicator0;
	    }
	    else if (PHRASE_ENABLED (come_in_peace))
		pStr[1] = come_in_peace;

	    if (PHRASE_ENABLED (know_any_jokes))
		pStr[2] = know_any_jokes;
	    else if (PHRASE_ENABLED (better_joke))
		pStr[2] = better_joke;
	    else if (PHRASE_ENABLED (not_very_funny))
		pStr[2] = not_very_funny;

	    if (PHRASE_ENABLED (what_about_tpet))
		pStr[3] = what_about_tpet;
	    else
		pStr[3] = arilou_told_us;

	    if (pStr[LastStack])
	    {
		if (pStr[LastStack] != we_vindicator0)
		    Response (pStr[LastStack], Zombies);
		else
		    DoResponsePhrase (pStr[LastStack], Zombies, shared_phrase_buf);
	    }
	    for (i = 0; i < 4; ++i)
	    {
		if (i != LastStack && pStr[i])
		{
		    if (pStr[i] != we_vindicator0)
			Response (pStr[i], Zombies);
		    else
			DoResponsePhrase (pStr[i], Zombies, shared_phrase_buf);
		}
	    }
	    Response (bye_zombie, CombatIsInevitable);
ExitZombies:
	    ;
	}
    }
    POP_CONTEXT
}

PROC(STATIC
void far NormalUmgah, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	if (PLAYER_SAID (R, whats_up_pre_zombie))
	{
	    NPCPhrase (GENERAL_INFO_PRE_ZOMBIE);

	    DISABLE_PHRASE (whats_up_pre_zombie);
	}
	else if (PLAYER_SAID (R, want_to_defeat_urquan))
	{
	    NPCPhrase (FINE_BY_US);

	    DISABLE_PHRASE (want_to_defeat_urquan);
	}

	if (!GET_GAME_STATE (UMGAH_EVIL_BLOBBIES))
	    Response (evil_blobbies_give_up, CombatIsInevitable);
	else
	    Response (evil_blobbies_must_die, CombatIsInevitable);
	if (PHRASE_ENABLED (whats_up_pre_zombie))
	    Response (whats_up_pre_zombie, NormalUmgah);
	if (PHRASE_ENABLED (want_to_defeat_urquan))
	    Response (want_to_defeat_urquan, NormalUmgah);
	switch (GET_GAME_STATE (UMGAH_MENTIONED_TRICKS))
	{
	    case 0:
		Response (can_we_be_friends, CombatIsInevitable);
		break;
	}
	Response (bye_pre_zombie, CombatIsInevitable);
    }
    POP_CONTEXT
}

PROC(STATIC
void far UmgahReward, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	if (PLAYER_SAID (R, what_before_tpet))
	{
	    NPCPhrase (TRKD_SPATHI_AND_ILWRATH);

	    DISABLE_PHRASE (what_before_tpet);
	}
	else if (PLAYER_SAID (R, where_caster))
	{
	    NPCPhrase (SPATHI_TOOK_THEM);

	    DISABLE_PHRASE (where_caster);
	}
	else if (PLAYER_SAID (R, owe_me_big_time))
	{
	    NPCPhrase (GIVE_LIFEDATA);

	    GLOBAL_SIS (TotalBioMass) += 1000 / BIO_CREDIT_VALUE;
	    DISABLE_PHRASE (owe_me_big_time);
	    DISABLE_PHRASE (our_largesse);
	}
	else if (PLAYER_SAID (R, our_largesse))
	{
	    NPCPhrase (THANKS);

	    GLOBAL_SIS (TotalBioMass) += 1000 / BIO_CREDIT_VALUE;
	    DISABLE_PHRASE (our_largesse);
	    DISABLE_PHRASE (owe_me_big_time);
	}
	else if (PLAYER_SAID (R, what_do_with_tpet))
	{
	    NPCPhrase (TRICK_URQUAN);

	    DISABLE_PHRASE (what_do_with_tpet);
	}
	else if (PLAYER_SAID (R, any_jokes))
	{
	    NPCPhrase (SURE);

	    DISABLE_PHRASE (any_jokes);
	}
	else if (PLAYER_SAID (R, so_what_for_now))
	{
	    NPCPhrase (DO_THIS_NOW);

	    DISABLE_PHRASE (so_what_for_now);
	}

	if (!GET_GAME_STATE (MET_NORMAL_UMGAH))
	{
	    if (PHRASE_ENABLED (what_before_tpet))
		Response (what_before_tpet, UmgahReward);
	    else if (PHRASE_ENABLED (where_caster))
		Response (where_caster, UmgahReward);
	}
	if (PHRASE_ENABLED (owe_me_big_time))
	{
	    Response (owe_me_big_time, UmgahReward);
	    Response (our_largesse, UmgahReward);
	}
	if (PHRASE_ENABLED (what_do_with_tpet))
	    Response (what_do_with_tpet, UmgahReward);
	else if (PHRASE_ENABLED (any_jokes) && GET_GAME_STATE (UMGAH_MENTIONED_TRICKS) < 2)
	    Response (any_jokes, UmgahReward);
	if (PHRASE_ENABLED (so_what_for_now))
	    Response (so_what_for_now, UmgahReward);
	Response (bye_post_zombie, CombatIsInevitable);
    }
    POP_CONTEXT
}

static void	far
Intro ()
{
    PUSH_CONTEXT
    {
	BYTE		NumVisits;

	if (GET_GAME_STATE (UMGAH_HOSTILE))
	{
	    NumVisits = GET_GAME_STATE (UMGAH_VISITS);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (HOSTILE_HELLO_1);
		    break;
		case 1:
		    NPCPhrase (HOSTILE_HELLO_2);
		    break;
		case 2:
		    NPCPhrase (HOSTILE_HELLO_3);
		    break;
		case 3:
		    NPCPhrase (HOSTILE_HELLO_4);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (UMGAH_VISITS, NumVisits);

	    SET_GAME_STATE (BATTLE_SEGUE, 1);
	}
	else if (GET_GAME_STATE (UMGAH_ZOMBIE_BLOBBIES))
	{
	    NumVisits = GET_GAME_STATE (UMGAH_VISITS);
	    if (GET_GAME_STATE (TALKING_PET_VISITS))
	    {
		switch (NumVisits++)
		{
		    case 0:
			NPCPhrase (DESTROY_INTERFERER_1);
			break;
		    case 1:
			NPCPhrase (DESTROY_INTERFERER_2);
			break;
		    case 2:
			NPCPhrase (DESTROY_INTERFERER_3);
			break;
		    case 3:
			NPCPhrase (DESTROY_INTERFERER_4);
			--NumVisits;
			break;
		}

		SET_GAME_STATE (BATTLE_SEGUE, 1);
	    }
	    else if (GET_GAME_STATE (KNOW_UMGAH_ZOMBIES))
	    {
		switch (NumVisits++)
		{
		    case 0:
			NPCPhrase (REVEALED_ZOMBIE_HELLO_1);
			break;
		    case 1:
			NPCPhrase (REVEALED_ZOMBIE_HELLO_2);
			break;
		    case 2:
			NPCPhrase (REVEALED_ZOMBIE_HELLO_3);
			break;
		    case 3:
			NPCPhrase (REVEALED_ZOMBIE_HELLO_4);
			--NumVisits;
			break;
		}

		SET_GAME_STATE (BATTLE_SEGUE, 1);
	    }
	    else
	    {
		switch (NumVisits++)
		{
		    case 0:
			NPCPhrase (UNKNOWN_ZOMBIE_HELLO_1);
			break;
		    case 1:
			NPCPhrase (UNKNOWN_ZOMBIE_HELLO_2);
			break;
		    case 2:
			NPCPhrase (UNKNOWN_ZOMBIE_HELLO_3);
			break;
		    case 3:
			NPCPhrase (UNKNOWN_ZOMBIE_HELLO_4);
			--NumVisits;
			break;
		}

		Zombies ((RESPONSE_REF)0);
	    }
	    SET_GAME_STATE (UMGAH_VISITS, NumVisits);
	}
	else if (!GET_GAME_STATE (TALKING_PET))
	{
	    if (GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 7))
	    {
		NumVisits = GET_GAME_STATE (UMGAH_HOME_VISITS);
		switch (NumVisits++)
		{
		    case 0:
			NPCPhrase (HWLD_PRE_ZOMBIE_HELLO_1);
			break;
		    case 1:
			NPCPhrase (HWLD_PRE_ZOMBIE_HELLO_2);
			break;
		    case 2:
			NPCPhrase (HWLD_PRE_ZOMBIE_HELLO_3);
			break;
		    case 3:
			NPCPhrase (HWLD_PRE_ZOMBIE_HELLO_4);
			--NumVisits;
			break;
		}
		SET_GAME_STATE (UMGAH_HOME_VISITS, NumVisits);
	    }
	    else
	    {
		NumVisits = GET_GAME_STATE (UMGAH_VISITS);
		switch (NumVisits++)
		{
		    case 0:
			NPCPhrase (SPACE_PRE_ZOMBIE_HELLO_1);
			break;
		    case 1:
			NPCPhrase (SPACE_PRE_ZOMBIE_HELLO_2);
			break;
		    case 2:
			NPCPhrase (SPACE_PRE_ZOMBIE_HELLO_3);
			break;
		    case 3:
			NPCPhrase (SPACE_PRE_ZOMBIE_HELLO_4);
			--NumVisits;
			break;
		}
		SET_GAME_STATE (UMGAH_VISITS, NumVisits);
	    }

	    NormalUmgah ((RESPONSE_REF)0);
	}
	else
	{
	    if (GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 7))
	    {
		NPCPhrase (POST_ZOMBIE_HWLD_HELLO);

		UmgahReward ((RESPONSE_REF)0);
	    }
	    else
	    {
		NumVisits = GET_GAME_STATE (UMGAH_VISITS);
		switch (NumVisits++)
		{
		    case 0:
			NPCPhrase (REWARD_AT_HOMEWORLD_1);
			break;
		    case 1:
			NPCPhrase (REWARD_AT_HOMEWORLD_2);
			--NumVisits;
			break;
		}
		SET_GAME_STATE (UMGAH_VISITS, NumVisits);

		SET_GAME_STATE (BATTLE_SEGUE, 0);
	    }
	}
    }
    POP_CONTEXT
}

PROC(STATIC
COUNT far uninit_umgah, (),
    ARG_VOID
)
{
    PUSH_CONTEXT
    {
	if (!GET_GAME_STATE (UMGAH_ZOMBIE_BLOBBIES))
	{
	    SET_GAME_STATE (MET_NORMAL_UMGAH, 1);
	}
    }
    POP_CONTEXT

    return (0);
}

PROC(
LOCDATAPTR far init_umgah_comm, (),
    ARG_VOID
)
{
    LOCDATAPTR	retval;

    PUSH_CONTEXT
    {
	umgah_desc.init_encounter_func = Intro;
	umgah_desc.uninit_encounter_func = uninit_umgah;

strcpy(aiff_folder, "comm/umgah/umgah");

	if ((GET_GAME_STATE (TALKING_PET) && !GET_GAME_STATE (UMGAH_HOSTILE))
		|| LOBYTE (GLOBAL (CurrentActivity)) == WON_LAST_BATTLE)
	{
	    SET_GAME_STATE (BATTLE_SEGUE, 0);
	}
	else
	{
	    SET_GAME_STATE (BATTLE_SEGUE, 1);
	}
	retval = &umgah_desc;
    }
    POP_CONTEXT

    return (retval);
}
