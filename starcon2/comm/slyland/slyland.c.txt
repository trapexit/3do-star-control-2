#include "reslib.h"
#include "resinst.h"
#include "starcon.h"
#include "commglue.h"
#include "strings.h"

static LOCDATA	slylandro_desc =
{
    NULL_PTR,				/* init_encounter_func */
    NULL_PTR,				/* uninit_encounter_func */
    (FRAME)SLYLAND_PMAP_ANIM,		/* AlienFrame */
    0,					/* AlienColorMap */
    SLYLAND_MUSIC,			/* AlienSong */
    SLYLAND_PLAYER_PHRASES,		/* PlayerPhrases */
    6,					/* NumAnimations */
    {
	{
	    1,				/* StartIndex */
	    9,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    3, 0,			/* FrameRate */
	    0, ONE_SECOND * 3,		/* RestartRate */
	    (1 << 3)			/* BlockMask */
	},
	{
	    10,				/* StartIndex */
	    8,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    3, 0,			/* FrameRate */
	    0, ONE_SECOND * 3,		/* RestartRate */
	    (1 << 4)			/* BlockMask */
	},
	{
	    18,				/* StartIndex */
	    8,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    3, 0,			/* FrameRate */
	    0, ONE_SECOND * 3,		/* RestartRate */
	    (1 << 5)			/* BlockMask */
	},
	{
	    26,				/* StartIndex */
	    8,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    3, 0,			/* FrameRate */
	    0, ONE_SECOND * 3,		/* RestartRate */
	    (1 << 0)			/* BlockMask */
	},
	{
	    34,				/* StartIndex */
	    8,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    3, 0,			/* FrameRate */
	    0, ONE_SECOND * 3,		/* RestartRate */
	    (1 << 1)			/* BlockMask */
	},
	{
	    42,				/* StartIndex */
	    9,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    3, 0,			/* FrameRate */
	    0, ONE_SECOND * 3,		/* RestartRate */
	    (1 << 2)			/* BlockMask */
	},
    },
};

static RESPONSE_REF	threat = 0,
			something_wrong = 0,
			we_are_us = 0,
			why_attack = 0,
			bye = 0;

PROC(STATIC
void far CombatIsInevitable, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	if (R == 0)
	{
	    if (GET_GAME_STATE (DESTRUCT_CODE_ON_SHIP))
		Response (destruct_code, CombatIsInevitable);
	    switch (GET_GAME_STATE (SLYLANDRO_PROBE_THREAT))
	    {
		case 0:
		    threat = threat_1;
		    break;
		case 1:
		    threat = threat_2;
		    break;
		case 2:
		    threat = threat_3;
		    break;
		default:
		    threat = threat_4;
		    break;
	    }
	    Response (threat, CombatIsInevitable);
	    switch (GET_GAME_STATE (SLYLANDRO_PROBE_WRONG))
	    {
		case 0:
		    something_wrong = something_wrong_1;
		    break;
		case 1:
		    something_wrong = something_wrong_2;
		    break;
		case 2:
		    something_wrong = something_wrong_3;
		    break;
		default:
		    something_wrong = something_wrong_4;
		    break;
	    }
	    Response (something_wrong, CombatIsInevitable);
	    switch (GET_GAME_STATE (SLYLANDRO_PROBE_ID))
	    {
		case 0:
		    we_are_us = we_are_us_1;
		    break;
		case 1:
		    we_are_us = we_are_us_2;
		    break;
		case 2:
		    we_are_us = we_are_us_3;
		    break;
		default:
		    we_are_us = we_are_us_4;
		    break;
	    }
	    Response (we_are_us, CombatIsInevitable);
	    switch (GET_GAME_STATE (SLYLANDRO_PROBE_INFO))
	    {
		case 0:
		    why_attack = why_attack_1;
		    break;
		case 1:
		    why_attack = why_attack_2;
		    break;
		case 2:
		    why_attack = why_attack_3;
		    break;
		default:
		    why_attack = why_attack_4;
		    break;
	    }
	    Response (why_attack, CombatIsInevitable);
	    switch (GET_GAME_STATE (SLYLANDRO_PROBE_EXIT))
	    {
		case 0:
		    bye = bye_1;
		    break;
		case 1:
		    bye = bye_2;
		    break;
		case 2:
		    bye = bye_3;
		    break;
		default:
		    bye = bye_4;
		    break;
	    }
	    Response (bye, CombatIsInevitable);
	}
	else if (PLAYER_SAID (R, destruct_code))
	{
	    COUNT	race_bounty[] =
	    {
		RACE_SHIP_COST
	    };

	    NPCPhrase (DESTRUCT_SEQUENCE);

	    SetSemaphore (&GraphicsSem);
	    DeltaSISGauges (0, 0, race_bounty[SLYLANDRO_SHIP] >> 3);
	    ClearSemaphore (&GraphicsSem);
	    SET_GAME_STATE (BATTLE_SEGUE, 0);
	}
	else
	{
	    BYTE	NumVisits;

	    if (PLAYER_SAID (R, threat))
	    {
		NumVisits = GET_GAME_STATE (SLYLANDRO_PROBE_THREAT);
		switch (NumVisits++)
		{
		    case 0:
			NPCPhrase (PROGRAMMED_TO_DEFEND_1);
			break;
		    case 1:
			NPCPhrase (PROGRAMMED_TO_DEFEND_2);
			break;
		    case 2:
			NPCPhrase (PROGRAMMED_TO_DEFEND_3);
			break;
		    case 3:
			NPCPhrase (PROGRAMMED_TO_DEFEND_4);
			--NumVisits;
			break;
		}
		SET_GAME_STATE (SLYLANDRO_PROBE_THREAT, NumVisits);
	    }
	    else if (PLAYER_SAID (R, something_wrong))
	    {
		NumVisits = GET_GAME_STATE (SLYLANDRO_PROBE_WRONG);
		switch (NumVisits++)
		{
		    case 0:
			NPCPhrase (NOMINAL_FUNCTION_1);
			break;
		    case 1:
			NPCPhrase (NOMINAL_FUNCTION_2);
			break;
		    case 2:
			NPCPhrase (NOMINAL_FUNCTION_3);
			break;
		    case 3:
			NPCPhrase (NOMINAL_FUNCTION_4);
			--NumVisits;
			break;
		}
		SET_GAME_STATE (SLYLANDRO_PROBE_WRONG, NumVisits);
	    }
	    else if (PLAYER_SAID (R, we_are_us))
	    {
		NumVisits = GET_GAME_STATE (SLYLANDRO_PROBE_ID);
		switch (NumVisits++)
		{
		    case 0:
			NPCPhrase (THIS_IS_PROBE_1);
			break;
		    case 1:
			NPCPhrase (THIS_IS_PROBE_2);
			break;
		    case 2:
			NPCPhrase (THIS_IS_PROBE_3);
			break;
		    case 3:
		    {
//			SIZE		dx, dy;
//			COUNT		adx, ady;
//
//			dx = LOGX_TO_UNIVERSE (GLOBAL_SIS (log_x)) - 333;
//			adx = dx >= 0 ? dx : -dx;
//			dy = 9812 - LOGY_TO_UNIVERSE (GLOBAL_SIS (log_y));
//			ady = dy >= 0 ? dy : -dy;
//			sprintf (THIS_IS_PROBE_4,
//				"%Fs%+04d.%01u,%+04d.%01u%Fs",
//				(char *)alien_text[THIS_IS_PROBE_40],
//				(SIZE)(dy / 10), (COUNT)(ady % 10),
//				(SIZE)(dx / 10), (COUNT)(adx % 10),
//				(char *)alien_text[THIS_IS_PROBE_41]);

			NPCPhrase (THIS_IS_PROBE_4);
			--NumVisits;
			break;
		    }
		}
		SET_GAME_STATE (SLYLANDRO_PROBE_ID, NumVisits);
	    }
	    else if (PLAYER_SAID (R, why_attack))
	    {
		NumVisits = GET_GAME_STATE (SLYLANDRO_PROBE_INFO);
		switch (NumVisits++)
		{
		    case 0:
			NPCPhrase (PEACEFUL_MISSION_1);
			break;
		    case 1:
			NPCPhrase (PEACEFUL_MISSION_2);
			break;
		    case 2:
			NPCPhrase (PEACEFUL_MISSION_3);
			break;
		    case 3:
			NPCPhrase (PEACEFUL_MISSION_4);
			--NumVisits;
			break;
		}
		SET_GAME_STATE (SLYLANDRO_PROBE_INFO, NumVisits);
	    }
	    else if (PLAYER_SAID (R, bye))
	    {
		NumVisits = GET_GAME_STATE (SLYLANDRO_PROBE_EXIT);
		switch (NumVisits++)
		{
		    case 0:
			NPCPhrase (GOODBYE_1);
			break;
		    case 1:
			NPCPhrase (GOODBYE_2);
			break;
		    case 2:
			NPCPhrase (GOODBYE_3);
			break;
		    case 3:
			NPCPhrase (GOODBYE_4);
			--NumVisits;
			break;
		}
		SET_GAME_STATE (SLYLANDRO_PROBE_EXIT, NumVisits);
	    }

	    NPCPhrase (HOSTILE);

	    SET_GAME_STATE (PROBE_EXHIBITED_BUG, 1);
	    SET_GAME_STATE (BATTLE_SEGUE, 1);
	}
    }
    POP_CONTEXT
}

static void	far
Intro ()
{
    PUSH_CONTEXT
    {
	BYTE 	NumVisits;

	NumVisits = GET_GAME_STATE (SLYLANDRO_PROBE_VISITS);
	switch (NumVisits++)
	{
	    case 0:
		NPCPhrase (WE_COME_IN_PEACE_1);
		break;
	    case 1:
		NPCPhrase (WE_COME_IN_PEACE_2);
		break;
	    case 2:
		NPCPhrase (WE_COME_IN_PEACE_3);
		break;
	    case 3:
		NPCPhrase (WE_COME_IN_PEACE_4);
		break;
	    case 4:
		NPCPhrase (WE_COME_IN_PEACE_5);
		break;
	    case 5:
		NPCPhrase (WE_COME_IN_PEACE_6);
		break;
	    case 6:
		NPCPhrase (WE_COME_IN_PEACE_7);
		break;
	    case 7:
		NPCPhrase (WE_COME_IN_PEACE_8);
		--NumVisits;
		break;
	}
	SET_GAME_STATE (SLYLANDRO_PROBE_VISITS, NumVisits);

	CombatIsInevitable ((RESPONSE_REF)0);
    }
    POP_CONTEXT
}

PROC(STATIC
COUNT uninit_slyland, (),
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
LOCDATAPTR init_slyland_comm, (),
    ARG_VOID
)
{
    LOCDATAPTR	retval;

    PUSH_CONTEXT
    {
	slylandro_desc.init_encounter_func = Intro;
	slylandro_desc.uninit_encounter_func = uninit_slyland;

strcpy(aiff_folder, "comm/slyland/slyla");

	SET_GAME_STATE (BATTLE_SEGUE, 1);
	retval = &slylandro_desc;
    }
    POP_CONTEXT

    return (retval);
}

