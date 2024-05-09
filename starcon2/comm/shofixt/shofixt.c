#include "reslib.h"
#include "resinst.h"
#include "starcon.h"
#include "commglue.h"
#include "strings.h"

static LOCDATA	shofixti_desc =
{
    NULL_PTR,				/* init_encounter_func */
    NULL_PTR,				/* uninit_encounter_func */
    (FRAME)SHOFIXTI_PMAP_ANIM,		/* AlienFrame */
    0,					/* AlienColorMap */
    SHOFIXTI_MUSIC,			/* AlienSong */
    SHOFIXTI_PLAYER_PHRASES,		/* PlayerPhrases */
    11,					/* NumAnimations */
    {
	{
	    5,				/* StartIndex */
	    15,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    4, 0,			/* FrameRate */
	    4, 0,			/* RestartRate */
	},
	{
	    20,				/* StartIndex */
	    3,				/* NumFrames */
	    RANDOM_ANIM,		/* AnimFlags */
	    12, 0,			/* FrameRate */
	    (ONE_SECOND >> 1), (ONE_SECOND >> 1) * 3,	/* RestartRate */
	},
	{
	    23,				/* StartIndex */
	    3,				/* NumFrames */
	    RANDOM_ANIM,		/* AnimFlags */
	    12, 0,			/* FrameRate */
	    (ONE_SECOND >> 1), (ONE_SECOND >> 1) * 3,	/* RestartRate */
	},
	{
	    26,				/* StartIndex */
	    3,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    4, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    29,				/* StartIndex */
	    4,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    8, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	},

	{
	    33,				/* StartIndex */
	    6,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    6, 4,			/* FrameRate */
	    6, 4,			/* RestartRate */
	},
	{
	    39,				/* StartIndex */
	    7,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    6, 4,			/* FrameRate */
	    6, 4,			/* RestartRate */
	    (1 << 7),			/* BlockMask */
	},
	{
	    46,				/* StartIndex */
	    6,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    6, 4,			/* FrameRate */
	    6, 4,			/* RestartRate */
	    (1 << 6),			/* BlockMask */
	},
	{
	    52,				/* StartIndex */
	    4,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    6, 4,			/* FrameRate */
	    6, 4,			/* RestartRate */
	},
	{
	    56,				/* StartIndex */
	    7,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    6, 4,			/* FrameRate */
	    6, 4,			/* RestartRate */
	    (1 << 10),			/* BlockMask */
	},
	{
	    63,				/* StartIndex */
	    6,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    6, 4,			/* FrameRate */
	    6, 4,			/* RestartRate */
	    (1 << 9),			/* BlockMask */
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
	6, 0,				/* FrameRate */
	8, 0,				/* RestartRate */
    },
};

static RESPONSE_REF	shofixti_name;

PROC(STATIC
void near GetShofixtiName, (),
    ARG_VOID
)
{
    if (GET_GAME_STATE (SHOFIXTI_KIA))
	shofixti_name = katana;
    else
	shofixti_name = tanaka;
}

PROC(STATIC
void far ExitConversation, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	SET_GAME_STATE (BATTLE_SEGUE, 1);

	if (PLAYER_SAID (R, bye0))
	{
	    NPCPhrase (GOODBYE);

	    SET_GAME_STATE (BATTLE_SEGUE, 0);
	}
	else if (PLAYER_SAID (R, go_ahead))
	{
	    NPCPhrase (ON_SECOND_THOUGHT);

	    SET_GAME_STATE (BATTLE_SEGUE, 0);
	}
	else if (PLAYER_SAID (R, need_you_for_duty))
	{
	    NPCPhrase (OK_WILL_BE_SENTRY);

	    SET_GAME_STATE (BATTLE_SEGUE, 0);
	}
	else if (PLAYER_SAID (R, females)
		|| PLAYER_SAID (R, nubiles)
		|| PLAYER_SAID (R, rat_babes))
	{
	    NPCPhrase (LEAPING_HAPPINESS);

	    SET_GAME_STATE (SHOFIXTI_RECRUITED, 1);
	    SET_GAME_STATE (MAIDENS_ON_SHIP, 0);
	    SET_GAME_STATE (BATTLE_SEGUE, 0);

	    AddEvent (RELATIVE_EVENT, 2, 0, 0, SHOFIXTI_RETURN_EVENT);
	}
	else if (PLAYER_SAID (R, dont_attack))
	{
	    NPCPhrase (TYPICAL_PLOY);

	    SET_GAME_STATE (SHOFIXTI_STACK1, 1);
	}
	else if (PLAYER_SAID (R, hey_stop))
	{
	    NPCPhrase (ONLY_STOP);

	    SET_GAME_STATE (SHOFIXTI_STACK1, 2);
	}
	else if (PLAYER_SAID (R, look_you_are))
	{
	    NPCPhrase (TOO_BAD);

	    SET_GAME_STATE (SHOFIXTI_STACK1, 3);
	}
	else if (PLAYER_SAID (R, no_one_insults))
	{
	    NPCPhrase (YOU_LIMP);

	    SET_GAME_STATE (SHOFIXTI_STACK2, 1);
	}
	else if (PLAYER_SAID (R, mighty_words))
	{
	    NPCPhrase (HANG_YOUR);

	    SET_GAME_STATE (SHOFIXTI_STACK2, 2);
	}
	else if (PLAYER_SAID (R, dont_know))
	{
	    NPCPhrase (NEVER);

	    SET_GAME_STATE (SHOFIXTI_STACK3, 1);
	}
	else if (PLAYER_SAID (R, look0))
	{
	    NPCPhrase (FOR_YOU);

	    SET_GAME_STATE (SHOFIXTI_STACK3, 2);
	}
	else if (PLAYER_SAID (R, no_bloodshed))
	{
	    NPCPhrase (YES_BLOODSHED);

	    SET_GAME_STATE (SHOFIXTI_STACK3, 3);
	}
	else if (PLAYER_SAID (R, dont_want_to_fight))
	{
	    BYTE	NumVisits;

	    NumVisits = GET_GAME_STATE (SHOFIXTI_STACK4);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (MUST_FIGHT_YOU_URQUAN_1);
		    break;
		case 1:
		    NPCPhrase (MUST_FIGHT_YOU_URQUAN_2);
		    break;
		case 2:
		    NPCPhrase (MUST_FIGHT_YOU_URQUAN_3);
		    break;
		case 3:
		    NPCPhrase (MUST_FIGHT_YOU_URQUAN_4);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (SHOFIXTI_STACK4, NumVisits);
	}
    }
    POP_CONTEXT
}

PROC(STATIC
void far GiveMaidens, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	if (PLAYER_SAID (R, important_duty))
	{
	    NPCPhrase (WHAT_DUTY);

	    Response (procreating_wildly, GiveMaidens);
	    Response (replenishing_your_species, GiveMaidens);
	    Response (hope_you_have, GiveMaidens);
	}
	else
	{
	    NPCPhrase (SOUNDS_GREAT_BUT_HOW);

	    Response (females, ExitConversation);
	    Response (nubiles, ExitConversation);
	    Response (rat_babes, ExitConversation);
	}
    }
    POP_CONTEXT
}

PROC(STATIC
void far ConsoleShofixti, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	if (PLAYER_SAID (R, dont_do_it))
	{
	    NPCPhrase (YES_I_DO_IT);
	    DISABLE_PHRASE (dont_do_it);
	}
	else
	    NPCPhrase (VERY_SAD_KILL_SELF);

	if (GET_GAME_STATE (MAIDENS_ON_SHIP))
	{
	    Response (important_duty, GiveMaidens);
	}
	if (PHRASE_ENABLED (dont_do_it))
	{
	    Response (dont_do_it, ConsoleShofixti);
	}
	Response (need_you_for_duty, ExitConversation);
	Response (go_ahead, ExitConversation);
    }
    POP_CONTEXT
}

PROC(STATIC
void far ExplainDefeat, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	if (PLAYER_SAID (R, i_am_nice))
	    NPCPhrase (MUST_UNDERSTAND);
	else if (PLAYER_SAID (R, i_am_guy))
	    NPCPhrase (NICE_BUT_WHAT_IS_DONKEY);
	else /* if (PLAYER_SAID (R, i_am_captain0)) */
	    NPCPhrase (SO_SORRY);
	NPCPhrase (IS_DEFEAT_TRUE);

	Response (yes_and_no, ConsoleShofixti);
	Response (clobbered, ConsoleShofixti);
	Response (butt_blasted, ConsoleShofixti);
    }
    POP_CONTEXT
}

PROC(STATIC
void far RealizeMistake, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	NPCPhrase (DGRUNTI);
	SET_GAME_STATE (SHOFIXTI_STACK1, 0);
	SET_GAME_STATE (SHOFIXTI_STACK3, 0);
	SET_GAME_STATE (SHOFIXTI_STACK2, 3);

	{
	    char	buf[80];

	    GetAllianceName (buf, name_1);
	    construct_response (
		    shared_phrase_buf,
		    i_am_captain0,
		    GLOBAL_SIS (CommanderName),
		    i_am_captain1,
		    buf,
		    i_am_captain2,
		    GLOBAL_SIS (ShipName),
		    i_am_captain3,
		    0
		    );
	}
	DoResponsePhrase (i_am_captain0, ExplainDefeat, shared_phrase_buf);
	Response (i_am_nice, ExplainDefeat);
	Response (i_am_guy, ExplainDefeat);
    }
    POP_CONTEXT
}

PROC(STATIC
void far Hostile, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	switch (GET_GAME_STATE (SHOFIXTI_STACK1))
	{
	    case 0:
		Response (dont_attack, ExitConversation);
		break;
	    case 1:
		Response (hey_stop, ExitConversation);
		break;
	    case 2:
		Response (look_you_are, ExitConversation);
		break;
	}
	switch (GET_GAME_STATE (SHOFIXTI_STACK2))
	{
	    case 0:
		Response (no_one_insults, ExitConversation);
		break;
	    case 1:
		Response (mighty_words, ExitConversation);
		break;
	    case 2:
		Response (donkey_breath, RealizeMistake);
		break;
	}
	switch (GET_GAME_STATE (SHOFIXTI_STACK3))
	{
	    case 0:
		Response (dont_know, ExitConversation);
		break;
	    case 1:
	    {
		construct_response (
			shared_phrase_buf,
			look0,
			"",
			shofixti_name,
			"",
			look1,
			0
			);
		DoResponsePhrase (look0, ExitConversation, shared_phrase_buf);
		break;
	    }
	    case 2:
		Response (look_you_are, ExitConversation);
		break;
	}
	Response (dont_want_to_fight, ExitConversation);
    }
    POP_CONTEXT
}

PROC(STATIC
void far Friendly, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	BYTE		i, LastStack;
	struct
	{
	    RESPONSE_REF	pStr;
	    char		*c_buf;
	} Resp[3];
	static char	buf0[80], buf1[80];
	
	LastStack = 0;
	MEMSET (Resp, 0, sizeof (Resp));
	if (PLAYER_SAID (R, report0))
	{
	    NPCPhrase (NOTHING_NEW);

	    DISABLE_PHRASE (report0);
	}
	else if (PLAYER_SAID (R, why_here0))
	{
	    NPCPhrase (I_GUARD);

	    LastStack = 1;
	    SET_GAME_STATE (SHOFIXTI_STACK1, 1);
	}
	else if (PLAYER_SAID (R, what_happened))
	{
	    NPCPhrase (MET_VUX);

	    LastStack = 1;
	    SET_GAME_STATE (SHOFIXTI_STACK1, 2);
	}
	else if (PLAYER_SAID (R, glory_device))
	{
	    NPCPhrase (SWITCH_BROKE);

	    SET_GAME_STATE (SHOFIXTI_STACK1, 3);
	}
	else if (PLAYER_SAID (R, where_world))
	{
	    NPCPhrase (BLEW_IT_UP);

	    LastStack = 2;
	    SET_GAME_STATE (SHOFIXTI_STACK3, 1);
	}
	else if (PLAYER_SAID (R, how_survive))
	{
	    NPCPhrase (NOT_HERE);

	    SET_GAME_STATE (SHOFIXTI_STACK3, 2);
	}

	if (PHRASE_ENABLED (report0))
	{
	    construct_response (
		    buf0,
		    report0,
		    "",
		    shofixti_name,
		    "",
		    report1,
		    0
		    );
	    Resp[0].pStr = report0;
	    Resp[0].c_buf = buf0;
	}

	switch (GET_GAME_STATE (SHOFIXTI_STACK1))
	{
	    case 0:
		construct_response (
			buf1,
			why_here0,
			"",
			shofixti_name,
			"",
			why_here1,
			0
			);
		Resp[1].pStr = why_here0;
		Resp[1].c_buf = buf1;
		break;
	    case 1:
		Resp[1].pStr = what_happened;
		break;
	    case 2:
		Resp[1].pStr = glory_device;
		break;
	}

	switch (GET_GAME_STATE (SHOFIXTI_STACK3))
	{
	    case 0:
	    	Resp[2].pStr = where_world;
		break;
	    case 1:
	    	Resp[2].pStr = how_survive;
		break;
	}

	if (Resp[LastStack].pStr)
	    DoResponsePhrase (Resp[LastStack].pStr, Friendly, Resp[LastStack].c_buf);
	for (i = 0; i < 3; ++i)
	{
	    if (i != LastStack && Resp[i].pStr)
		DoResponsePhrase (Resp[i].pStr, Friendly, Resp[i].c_buf);
	}
	if (GET_GAME_STATE (MAIDENS_ON_SHIP))
	{
	    Response (important_duty, GiveMaidens);
	}

	construct_response (
		shared_phrase_buf,
		bye0,
		"",
		shofixti_name,
		"",
		bye1,
		0
		);
	DoResponsePhrase (bye0, ExitConversation, shared_phrase_buf);
    }
    POP_CONTEXT
}

PROC(STATIC
void far Intro, (),
    ARG_VOID
)
{
    PUSH_CONTEXT
    {
	if (LOBYTE (GLOBAL (CurrentActivity)) == WON_LAST_BATTLE)
	{
//	    NPCPhrase (OUT_TAKES);

	    SET_GAME_STATE (BATTLE_SEGUE, 0);
	    goto ExitIntro;
	}

	GetShofixtiName ();

	if (GET_GAME_STATE (SHOFIXTI_STACK2) > 2)
	{
	    NPCPhrase (FRIENDLY_HELLO);

	    Friendly ((RESPONSE_REF)0);
	}
	else
	{
	    BYTE	NumVisits;

	    NumVisits = GET_GAME_STATE (SHOFIXTI_VISITS);
	    if (GET_GAME_STATE (SHOFIXTI_KIA))
	    {
		switch (NumVisits++)
		{
		    case 0:
			NPCPhrase (HOSTILE_KATANA_1);
			break;
		    case 1:
			NPCPhrase (HOSTILE_KATANA_2);
			break;
		    case 2:
			NPCPhrase (HOSTILE_KATANA_3);
			break;
		    case 3:
			NPCPhrase (HOSTILE_KATANA_4);
			--NumVisits;
			break;
		}
	    }
	    else
	    {
		switch (NumVisits++)
		{
		    case 0:
			NPCPhrase (HOSTILE_TANAKA_1);
			break;
		    case 1:
			NPCPhrase (HOSTILE_TANAKA_2);
			break;
		    case 2:
			NPCPhrase (HOSTILE_TANAKA_3);
			break;
		    case 3:
			NPCPhrase (HOSTILE_TANAKA_4);
			break;
		    case 4:
			NPCPhrase (HOSTILE_TANAKA_5);
			break;
		    case 5:
			NPCPhrase (HOSTILE_TANAKA_6);
			break;
		    case 6:
			NPCPhrase (HOSTILE_TANAKA_7);
			break;
		    case 7:
			NPCPhrase (HOSTILE_TANAKA_8);
			--NumVisits;
			break;
		}
	    }
	    SET_GAME_STATE (SHOFIXTI_VISITS, NumVisits);

	    Hostile ((RESPONSE_REF)0);
	}
ExitIntro:
	;
    }
    POP_CONTEXT
}

PROC(STATIC
COUNT uninit_shofixti, (),
    ARG_VOID
)
{
    PUSH_CONTEXT
    {
    }
    POP_CONTEXT
}

PROC(
LOCDATAPTR far init_shofixti_comm, (),
    ARG_VOID
)
{
    LOCDATAPTR	retval;

    PUSH_CONTEXT
    {
	shofixti_desc.init_encounter_func = Intro;
	shofixti_desc.uninit_encounter_func = uninit_shofixti;

strcpy(aiff_folder, "comm/shofixt/shofi");

	SET_GAME_STATE (BATTLE_SEGUE, 0);

	retval = &shofixti_desc;
    }
    POP_CONTEXT

    return (retval);
}
