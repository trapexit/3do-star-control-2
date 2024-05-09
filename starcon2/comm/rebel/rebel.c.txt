#include "reslib.h"
#include "../yehat/resinst.h"
#include "starcon.h"
#include "commglue.h"
#include "strings.h"

static LOCDATA	yehat_desc =
{
    NULL_PTR,				/* init_encounter_func */
    NULL_PTR,				/* uninit_encounter_func */
    (FRAME)YEHAT_PMAP_ANIM,		/* AlienFrame */
    0,					/* AlienColorMap */
    YEHAT_MUSIC,			/* AlienSong */
    REBEL_PLAYER_PHRASES,		/* PlayerPhrases */
    15,					/* NumAnimations */
    {
	{
	    4,				/* StartIndex */
	    3,				/* NumFrames */
	    YOYO_ANIM
		    | WAIT_TALKING,	/* AnimFlags */
	    12, 12,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	    (1 << 6),
	},
	{
	    7,				/* StartIndex */
	    3,				/* NumFrames */
	    YOYO_ANIM
		    | WAIT_TALKING,	/* AnimFlags */
	    12, 12,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	    (1 << 6),
	},
	{
	    10,				/* StartIndex */
	    3,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    6, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	    (1 << 4) | (1 << 14),
	},
	{
	    13,				/* StartIndex */
	    3,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    6, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	    (1 << 5),
	},
	{
	    16,				/* StartIndex */
	    5,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    8, 8,			/* FrameRate */
	    ONE_SECOND * 10, ONE_SECOND * 3,/* RestartRate */
	    (1 << 2) | (1 << 14),
	},
	{
	    21,				/* StartIndex */
	    5,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    8, 8,			/* FrameRate */
	    ONE_SECOND * 10, ONE_SECOND * 3,/* RestartRate */
	    (1 << 3),
	},
	{
	    26,				/* StartIndex */
	    2,				/* NumFrames */
	    YOYO_ANIM
		    | WAIT_TALKING,	/* AnimFlags */
	    8, 8,			/* FrameRate */
	    ONE_SECOND * 10, ONE_SECOND * 3,/* RestartRate */
	    (1 << 0) | (1 << 1),
	},
	{
	    28,				/* StartIndex */
	    2,				/* NumFrames */
	    YOYO_ANIM
		    | WAIT_TALKING,	/* AnimFlags */
	    8, 8,			/* FrameRate */
	    ONE_SECOND * 10, ONE_SECOND * 3,/* RestartRate */
	    (1 << 0) | (1 << 1),
	},
	{
	    30,				/* StartIndex */
	    3,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    4, 4,			/* FrameRate */
	    4, 4,			/* RestartRate */
	},
	{
	    33,				/* StartIndex */
	    3,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    4, 4,			/* FrameRate */
	    4, 4,			/* RestartRate */
	},
	{
	    36,				/* StartIndex */
	    3,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    4, 4,			/* FrameRate */
	    4, 4,			/* RestartRate */
	},
	{
	    39,				/* StartIndex */
	    3,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    4, 4,			/* FrameRate */
	    4, 4,			/* RestartRate */
	},
	{
	    42,				/* StartIndex */
	    3,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    4, 4,			/* FrameRate */
	    4, 4,			/* RestartRate */
	},
	{
	    45,				/* StartIndex */
	    3,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    4, 4,			/* FrameRate */
	    4, 4,			/* RestartRate */
	},
	{
	    48,				/* StartIndex */
	    4,				/* NumFrames */
	    YOYO_ANIM
		    | WAIT_TALKING,	/* AnimFlags */
	    4, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	    (1 << 2) | (1 << 4),
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
void near PrepareShip, (),
    ARG_VOID
)
{
    BYTE	mi, di, yi;

    mi = (GLOBAL (GameClock.month_index) % 12) + 1;
    SET_GAME_STATE (YEHAT_SHIP_MONTH, mi);
    if ((di = GLOBAL (GameClock.day_index)) > 28)
	di = 28;
    SET_GAME_STATE (YEHAT_SHIP_DAY, di);
    yi = (BYTE)(GLOBAL (GameClock.year_index) - START_YEAR);
    if (mi == 1)
	++yi;
    SET_GAME_STATE (YEHAT_SHIP_YEAR, yi);
}

PROC(STATIC
void far ExitConversation, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	if (PLAYER_SAID (R, bye_rebel))
	    NPCPhrase (GOODBYE_REBEL);
    }
    POP_CONTEXT
}

PROC_LOCAL(
void far Rebels, (R),
    ARG_END	(RESPONSE_REF	R)
);

PROC(STATIC
void far RebelInfo, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	BYTE		InfoLeft;
	PTR_DESC	PtrDesc;

	InfoLeft = FALSE;
	if (PLAYER_SAID (R, give_info_rebels))
	    NPCPhrase (WHAT_INFO);
	else if (PLAYER_SAID (R, what_about_urquan))
	{
	    NPCPhrase (ABOUT_URQUAN);

	    DISABLE_PHRASE (what_about_urquan);
	}
	else if (PLAYER_SAID (R, what_about_royalty))
	{
	    NPCPhrase (ABOUT_ROYALTY);

	    DISABLE_PHRASE (what_about_royalty);
	}
	else if (PLAYER_SAID (R, what_about_war))
	{
	    NPCPhrase (ABOUT_WAR);

	    DISABLE_PHRASE (what_about_war);
	}
	else if (PLAYER_SAID (R, what_about_vux))
	{
	    NPCPhrase (ABOUT_VUX);

	    DISABLE_PHRASE (what_about_vux);
	}
	else if (PLAYER_SAID (R, what_about_clue))
	{
	    NPCPhrase (ABOUT_CLUE);

	    DISABLE_PHRASE (what_about_clue);
	}

	if (PHRASE_ENABLED (what_about_urquan))
	{
	    Response (what_about_urquan, RebelInfo);
	    InfoLeft = TRUE;
	}
	if (PHRASE_ENABLED (what_about_royalty))
	{
	    Response (what_about_royalty, RebelInfo);
	    InfoLeft = TRUE;
	}
	if (PHRASE_ENABLED (what_about_war))
	{
	    Response (what_about_war, RebelInfo);
	    InfoLeft = TRUE;
	}
	if (PHRASE_ENABLED (what_about_vux))
	{
	    Response (what_about_vux, RebelInfo);
	    InfoLeft = TRUE;
	}
	if (PHRASE_ENABLED (what_about_clue))
	{
	    Response (what_about_clue, RebelInfo);
	    InfoLeft = TRUE;
	}
	Response (enough_info, Rebels);

	if (!InfoLeft)
	{
	    DISABLE_PHRASE (give_info_rebels);
	}
    }
    POP_CONTEXT
}

PROC(STATIC
void far Rebels, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	BYTE		NumVisits;

	if (PLAYER_SAID (R, how_goes_revolution))
	{
	    NumVisits = GET_GAME_STATE (YEHAT_REBEL_INFO);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (REBEL_REVOLUTION_1);
		    break;
		case 1:
		    NPCPhrase (REBEL_REVOLUTION_2);
		    break;
		case 2:
		    NPCPhrase (REBEL_REVOLUTION_3);
		    break;
		case 3:
		    NPCPhrase (REBEL_REVOLUTION_4);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (YEHAT_REBEL_INFO, NumVisits);

	    DISABLE_PHRASE (how_goes_revolution);
	}
	else if (PLAYER_SAID (R, any_ships))
	{
	    if (GET_GAME_STATE (YEHAT_SHIP_MONTH)
		    && ((NumVisits = (GLOBAL (GameClock.year_index) - START_YEAR)
		    - GET_GAME_STATE (YEHAT_SHIP_YEAR)) < 0
		    || (NumVisits == 0
		    && (NumVisits = GLOBAL (GameClock.month_index) -
		    GET_GAME_STATE (YEHAT_SHIP_MONTH)) < 0
		    || (NumVisits == 0
		    && GLOBAL (GameClock.day_index) <
		    GET_GAME_STATE (YEHAT_SHIP_DAY)))))
		NPCPhrase (NO_SHIPS_YET);
	    else if ((NumVisits = ActivateStarShip (YEHAT_SHIP, FEASIBILITY_STUDY)) == 0)
		NPCPhrase (NO_ROOM);
	    else
	    {
#define NUM_YEHAT_SHIPS	4
		if (NumVisits < NUM_YEHAT_SHIPS)
		    NPCPhrase (HAVE_FEW_SHIPS);
		else
		{
		    NumVisits = NUM_YEHAT_SHIPS;
		    NPCPhrase (HAVE_ALL_SHIPS);
		}

		AlienTalkSegue ((COUNT)~0);
		ActivateStarShip (YEHAT_SHIP, NumVisits);
		PrepareShip ();
	    }

	    DISABLE_PHRASE (any_ships);
	}
	else if (PLAYER_SAID (R, what_about_pkunk_rebel))
	{
	    if (GET_GAME_STATE (YEHAT_ABSORBED_PKUNK))
		NPCPhrase (PKUNK_ABSORBED_REBEL);
	    else
		NPCPhrase (HATE_PKUNK_REBEL);

	    SET_GAME_STATE (YEHAT_REBEL_TOLD_PKUNK, 1);
	}
	else if (PLAYER_SAID (R, enough_info))
	    NPCPhrase (OK_ENOUGH_INFO);

	if (PHRASE_ENABLED (how_goes_revolution))
	    Response (how_goes_revolution, Rebels);
	if (!GET_GAME_STATE (YEHAT_REBEL_TOLD_PKUNK)
		&& GET_GAME_STATE (PKUNK_VISITS)
		&& GET_GAME_STATE (PKUNK_HOME_VISITS))
	    Response (what_about_pkunk_rebel, Rebels);
	if (PHRASE_ENABLED (any_ships))
	    Response (any_ships, Rebels);
	if (PHRASE_ENABLED (give_info_rebels))
	{
	    Response (give_info_rebels, RebelInfo);
	}
	Response (bye_rebel,  ExitConversation);
    }
    POP_CONTEXT
}

static void	far
Intro ()
{
    PUSH_CONTEXT
    {
	BYTE	NumVisits;

	SET_GAME_STATE (BATTLE_SEGUE, 0);
	if (LOBYTE (GLOBAL (CurrentActivity)) == IN_LAST_BATTLE)
	{
	    NPCPhrase (YEHAT_CAVALRY);
	    AlienTalkSegue ((COUNT)~0);

	    if ((NumVisits = (BYTE)ActivateStarShip (
		    YEHAT_REBEL_SHIP, FEASIBILITY_STUDY
		    )) > 8)
		NumVisits = 8;
	    ActivateStarShip (YEHAT_REBEL_SHIP, NumVisits - (NumVisits >> 1));
	    ActivateStarShip (PKUNK_SHIP, NumVisits >> 1);
	}
	else
	{
	    NumVisits = GET_GAME_STATE (YEHAT_REBEL_VISITS);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (REBEL_HELLO_1);
		    break;
		case 1:
		    NPCPhrase (REBEL_HELLO_2);
		    break;
		case 2:
		    NPCPhrase (REBEL_HELLO_3);
		    break;
		case 3:
		    NPCPhrase (REBEL_HELLO_4);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (YEHAT_REBEL_VISITS, NumVisits);

	    Rebels ((RESPONSE_REF)0);
	}
    }
    POP_CONTEXT
}

PROC(STATIC
COUNT far uninit_yehat, (),
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
LOCDATAPTR far init_rebel_yehat_comm, (),
    ARG_VOID
)
{
    LOCDATAPTR	retval;

    PUSH_CONTEXT
    {
	yehat_desc.init_encounter_func = Intro;
	yehat_desc.uninit_encounter_func = uninit_yehat;

strcpy(aiff_folder, "comm/rebel/rebel");

	SET_GAME_STATE (BATTLE_SEGUE, 0);
	retval = &yehat_desc;
    }
    POP_CONTEXT

    return (retval);
}
