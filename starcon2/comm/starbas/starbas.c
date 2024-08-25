#include "reslib.h"
#include "../comandr/resinst.h"
#include "starcon.h"
#include "commglue.h"
#include "strings.h"

static LOCDATA	commander_desc =
{
    NULL_PTR,				/* init_encounter_func */
    NULL_PTR,				/* uninit_encounter_func */
    (FRAME)COMMANDER_PMAP_ANIM,		/* AlienFrame */
    (COLORMAP)COMMANDER_COLOR_MAP,	/* AlienColorMap */
    COMMANDER_MUSIC,			/* AlienSong */
    STARBASE_PLAYER_PHRASES,		/* PlayerPhrases */
    10,					/* NumAnimations */
    {
	{	/* Blink */
	    1,				/* StartIndex */
	    3,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    8, 0,			/* FrameRate */
	    0, ONE_SECOND * 8,		/* RestartRate */
	},
	{	/* Running light */
	    10,				/* StartIndex */
	    30,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    3, 0,			/* FrameRate */
	    ONE_SECOND * 2, 0,		/* RestartRate */
	},
	{	/* Arc welder 0 */
	    40,				/* StartIndex */
	    7,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    3, 0,			/* FrameRate */
	    0, ONE_SECOND * 8,		/* RestartRate */
	},
	{	/* Arc welder 1 */
	    47,				/* StartIndex */
	    8,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    3, 0,			/* FrameRate */
	    0, ONE_SECOND * 8,		/* RestartRate */
	},
	{	/* Arc welder 2 */
	    55,				/* StartIndex */
	    6,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    3, 0,			/* FrameRate */
	    0, ONE_SECOND * 8,		/* RestartRate */
	},
	{	/* Arc welder 3 */
	    61,				/* StartIndex */
	    6,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    3, 0,			/* FrameRate */
	    0, ONE_SECOND * 8,		/* RestartRate */
	},
	{	/* Arc welder 4 */
	    67,				/* StartIndex */
	    7,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    3, 0,			/* FrameRate */
	    0, ONE_SECOND * 8,		/* RestartRate */
	},
	{	/* Arc welder 5 */
	    74,				/* StartIndex */
	    11,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    3, 0,			/* FrameRate */
	    0, ONE_SECOND * 8,		/* RestartRate */
	},
	{	/* Arc welder 6 */
	    85,				/* StartIndex */
	    10,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    3, 0,			/* FrameRate */
	    0, ONE_SECOND * 8,		/* RestartRate */
	},
	{	/* Flagship picture */
	    95,				/* StartIndex */
	    1,				/* NumFrames */
	    0,				/* AnimFlags */
	    0, 0,			/* FrameRate */
	    0, 0,			/* RestartRate */
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
	4,				/* StartIndex */
	6,				/* NumFrames */
	0,				/* AnimFlags */
	12, 8,				/* FrameRate */
	14, 10,				/* RestartRate */
    },
};

static DWORD	CurBulletinMask;

static void	far
ByeBye (R)
RESPONSE_REF	R;
{
    PUSH_CONTEXT
    {
	BYTE	b0, b1, b2, b3;

	b0 = GET_GAME_STATE (STARBASE_BULLETS0);
	b1 = GET_GAME_STATE (STARBASE_BULLETS1);
	b2 = GET_GAME_STATE (STARBASE_BULLETS2);
	b3 = GET_GAME_STATE (STARBASE_BULLETS3);
	CurBulletinMask |= MAKE_DWORD (
		MAKE_WORD (b0, b1), MAKE_WORD (b2, b3)
		);
	b0 = LOBYTE (LOWORD (CurBulletinMask));
	b1 = HIBYTE (LOWORD (CurBulletinMask));
	b2 = LOBYTE (HIWORD (CurBulletinMask));
	b3 = HIBYTE (HIWORD (CurBulletinMask));
	SET_GAME_STATE (STARBASE_BULLETS0, b0);
	SET_GAME_STATE (STARBASE_BULLETS1, b1);
	SET_GAME_STATE (STARBASE_BULLETS2, b2);
	SET_GAME_STATE (STARBASE_BULLETS3, b3);

	/* if (R == goodbye_starbase_commander) */
	if (GET_GAME_STATE (CHMMR_BOMB_STATE) >= 2)
	    NPCPhrase (AFTER_BOMB_GOODBYE);
	else
	{
	    RESPONSE_REF	pStr0;
	    
	    switch ((BYTE)random () & 7)
	    {
		case 0:
		    pStr0 = NORMAL_GOODBYE_A;
		    break;
		case 1:
		    pStr0 = NORMAL_GOODBYE_B;
		    break;
		case 2:
		    pStr0 = NORMAL_GOODBYE_C;
		    break;
		case 3:
		    pStr0 = NORMAL_GOODBYE_D;
		    break;
		case 4:
		    pStr0 = NORMAL_GOODBYE_E;
		    break;
		case 5:
		    pStr0 = NORMAL_GOODBYE_F;
		    break;
		case 6:
		    pStr0 = NORMAL_GOODBYE_G;
		    break;
		case 7:
		    pStr0 = NORMAL_GOODBYE_H;
		    break;
	    }

	    NPCPhrase (pStr0);
	}
    }
    POP_CONTEXT
}

PROC_LOCAL(
void far NeedInfo, (R),
    ARG_END	(RESPONSE_REF	R)
);
PROC_LOCAL(
void far TellHistory, (R),
    ARG_END	(RESPONSE_REF	R)
);
PROC_LOCAL(
void far AlienRaces, (R),
    ARG_END	(RESPONSE_REF	R)
);

static BYTE	stack0 = 0,
		stack1 = 0,
		stack2 = 0,
		stack3 = 0;

PROC(STATIC
void far AllianceInfo, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
#define ALLIANCE_SHOFIXTI	(1 << 0)
#define ALLIANCE_YEHAT		(1 << 1)
#define ALLIANCE_ARILOU		(1 << 2)
#define ALLIANCE_CHENJESU	(1 << 3)
#define ALLIANCE_MMRNMHRM	(1 << 4)
#define ALLIANCE_SYREEN		(1 << 5)
    PUSH_CONTEXT
    {
	static BYTE	AllianceMask = 0;

	if (PLAYER_SAID (R, what_about_alliance))
	{
	    NPCPhrase (WHICH_ALLIANCE);
	    AllianceMask = 0;
	}
	else if (PLAYER_SAID (R, shofixti))
	{
	    NPCPhrase (ABOUT_SHOFIXTI);
	    AllianceMask |= ALLIANCE_SHOFIXTI;
	}
	else if (PLAYER_SAID (R, yehat))
	{
	    NPCPhrase (ABOUT_YEHAT);
	    AllianceMask |= ALLIANCE_YEHAT;
	}
	else if (PLAYER_SAID (R, arilou))
	{
	    NPCPhrase (ABOUT_ARILOU);
	    AllianceMask |= ALLIANCE_ARILOU;
	}
	else if (PLAYER_SAID (R, chenjesu))
	{
	    NPCPhrase (ABOUT_CHENJESU);
	    AllianceMask |= ALLIANCE_CHENJESU;
	}
	else if (PLAYER_SAID (R, mmrnmhrm))
	{
	    NPCPhrase (ABOUT_MMRNMHRM);
	    AllianceMask |= ALLIANCE_MMRNMHRM;
	}
	else if (PLAYER_SAID (R, syreen))
	{
	    NPCPhrase (ABOUT_SYREEN);
	    AllianceMask |= ALLIANCE_SYREEN;
	}

	if (!(AllianceMask & ALLIANCE_SHOFIXTI))
	    Response (shofixti, AllianceInfo);
	if (!(AllianceMask & ALLIANCE_YEHAT))
	    Response (yehat, AllianceInfo);
	if (!(AllianceMask & ALLIANCE_ARILOU))
	    Response (arilou, AllianceInfo);
	if (!(AllianceMask & ALLIANCE_CHENJESU))
	    Response (chenjesu, AllianceInfo);
	if (!(AllianceMask & ALLIANCE_MMRNMHRM))
	    Response (mmrnmhrm, AllianceInfo);
	if (!(AllianceMask & ALLIANCE_SYREEN))
	    Response (syreen, AllianceInfo);
	Response (enough_alliance, AlienRaces);
    }
    POP_CONTEXT
}

PROC(STATIC
void far HierarchyInfo, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
#define HIERARCHY_MYCON		(1 << 0)
#define HIERARCHY_SPATHI	(1 << 1)
#define HIERARCHY_UMGAH		(1 << 2)
#define HIERARCHY_ANDROSYNTH	(1 << 3)
#define HIERARCHY_ILWRATH	(1 << 4)
#define HIERARCHY_VUX		(1 << 5)
    PUSH_CONTEXT
    {
	static BYTE	HierarchyMask = 0;

	if (PLAYER_SAID (R, what_about_hierarchy))
	{
	    NPCPhrase (WHICH_HIERARCHY);
	    HierarchyMask = 0;
	}
	else if (PLAYER_SAID (R, mycon))
	{
	    NPCPhrase (ABOUT_MYCON);
	    HierarchyMask |= HIERARCHY_MYCON;
	}
	else if (PLAYER_SAID (R, spathi))
	{
	    NPCPhrase (ABOUT_SPATHI);
	    HierarchyMask |= HIERARCHY_SPATHI;
	}
	else if (PLAYER_SAID (R, umgah))
	{
	    NPCPhrase (ABOUT_UMGAH);
	    HierarchyMask |= HIERARCHY_UMGAH;
	}
	else if (PLAYER_SAID (R, androsynth))
	{
	    NPCPhrase (ABOUT_ANDROSYNTH);
	    HierarchyMask |= HIERARCHY_ANDROSYNTH;
	}
	else if (PLAYER_SAID (R, ilwrath))
	{
	    NPCPhrase (ABOUT_ILWRATH);
	    HierarchyMask |= HIERARCHY_ILWRATH;
	}
	else if (PLAYER_SAID (R, vux))
	{
	    NPCPhrase (ABOUT_VUX);
	    HierarchyMask |= HIERARCHY_VUX;
	}

	if (!(HierarchyMask & HIERARCHY_MYCON))
	    Response (mycon, HierarchyInfo);
	if (!(HierarchyMask & HIERARCHY_SPATHI))
	    Response (spathi, HierarchyInfo);
	if (!(HierarchyMask & HIERARCHY_UMGAH))
	    Response (umgah, HierarchyInfo);
	if (!(HierarchyMask & HIERARCHY_ANDROSYNTH))
	    Response (androsynth, HierarchyInfo);
	if (!(HierarchyMask & HIERARCHY_ILWRATH))
	    Response (ilwrath, HierarchyInfo);
	if (!(HierarchyMask & HIERARCHY_VUX))
	    Response (vux, HierarchyInfo);
	Response (enough_hierarchy, AlienRaces);
    }
    POP_CONTEXT
}

PROC(STATIC
void far AlienRaces, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
#define RACES_ALLIANCE	(1 << 0)
#define RACES_HIERARCHY	(1 << 1)
#define RACES_OTHER	(1 << 2)
	static BYTE	RacesMask = 0;

	if (PLAYER_SAID (R, alien_races))
	{
	    NPCPhrase (WHICH_ALIEN);
	    RacesMask = 0;
	}
	else if (PLAYER_SAID (R, enough_alliance))
	{
	    NPCPhrase (OK_ENOUGH_ALLIANCE);
	    RacesMask |= RACES_ALLIANCE;
	}
	else if (PLAYER_SAID (R, enough_hierarchy))
	{
	    NPCPhrase (OK_ENOUGH_HIERARCHY);
	    RacesMask |= RACES_HIERARCHY;
	}
	else if (PLAYER_SAID (R, what_about_other))
	{
	    NPCPhrase (ABOUT_OTHER);
	    RacesMask |= RACES_OTHER;
	}

	if (!(RacesMask & RACES_ALLIANCE))
	{
	    Response (what_about_alliance, AllianceInfo);
	}
	if (!(RacesMask & RACES_HIERARCHY))
	{
	    Response (what_about_hierarchy, HierarchyInfo);
	}
	if (!(RacesMask & RACES_OTHER))
	{
	    Response (what_about_other, AlienRaces);
	}
	Response (enough_aliens, TellHistory);
    }
    POP_CONTEXT
}

PROC(STATIC
void far WarInfo, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
#define WAR_STARTED	(1 << 0)
#define WAR_WAS_LIKE	(1 << 1)
#define WAR_LOST	(1 << 2)
#define WAR_AFTERMATH	(1 << 3)
	static BYTE	WarMask = 0;

	if (PLAYER_SAID (R, the_war))
	{
	    NPCPhrase (WHICH_WAR);
	    WarMask = 0;
	}
	else if (PLAYER_SAID (R, what_started_war))
	{
	    NPCPhrase (URQUAN_STARTED_WAR);
	    WarMask |= WAR_STARTED;
	}
	else if (PLAYER_SAID (R, what_was_war_like))
	{
	    NPCPhrase (WAR_WAS_LIKE_SO);
	    WarMask |= WAR_WAS_LIKE;
	}
	else if (PLAYER_SAID (R, why_lose_war))
	{
	    NPCPhrase (LOST_WAR_BECAUSE);
	    WarMask |= WAR_LOST;
	}
	else if (PLAYER_SAID (R, what_after_war))
	{
	    NPCPhrase (AFTER_WAR);
	    WarMask |= WAR_AFTERMATH;
	}

	if (!(WarMask & WAR_STARTED))
	    Response (what_started_war, WarInfo);
	if (!(WarMask & WAR_WAS_LIKE))
	    Response (what_was_war_like, WarInfo);
	if (!(WarMask & WAR_LOST))
	    Response (why_lose_war, WarInfo);
	if (!(WarMask & WAR_AFTERMATH))
	    Response (what_after_war, WarInfo);
	Response (enough_war, TellHistory);
    }
    POP_CONTEXT
}

PROC(STATIC
void far AncientHistory, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
#define ANCIENT_PRECURSORS	(1 << 0)
#define ANCIENT_RACES		(1 << 1)
#define ANCIENT_EARTH		(1 << 2)
	static BYTE	AncientMask = 0;

	if (PLAYER_SAID (R, ancient_history))
	{
	    NPCPhrase (WHICH_ANCIENT);
	    AncientMask = 0;
	}
	else if (PLAYER_SAID (R, precursors))
	{
	    NPCPhrase (ABOUT_PRECURSORS);
	    AncientMask |= ANCIENT_PRECURSORS;
	}
	else if (PLAYER_SAID (R, old_races))
	{
	    NPCPhrase (ABOUT_OLD_RACES);
	    AncientMask |= ANCIENT_RACES;
	}
	else if (PLAYER_SAID (R, aliens_on_earth))
	{
	    NPCPhrase (ABOUT_ALIENS_ON_EARTH);
	    AncientMask |= ANCIENT_EARTH;
	}

	if (!(AncientMask & ANCIENT_PRECURSORS))
	    Response (precursors, AncientHistory);
	if (!(AncientMask & ANCIENT_RACES))
	    Response (old_races, AncientHistory);
	if (!(AncientMask & ANCIENT_EARTH))
	    Response (aliens_on_earth, AncientHistory);
	Response (enough_ancient, TellHistory);
    }
    POP_CONTEXT
}

PROC(STATIC
void far TellHistory, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	RESPONSE_REF		pstack[3];

	if (PLAYER_SAID (R, history))
	{
	    NPCPhrase (WHICH_HISTORY);
	    stack0 = stack1 = stack2 = 0;
	}
	else if (PLAYER_SAID (R, enough_aliens))
	{
	    NPCPhrase (OK_ENOUGH_ALIENS);

	    stack0 = 1;
	}
	else if (PLAYER_SAID (R, enough_war))
	{
	    NPCPhrase (OK_ENOUGH_WAR);

	    stack1 = 1;
	}
	else if (PLAYER_SAID (R, enough_ancient))
	{
	    NPCPhrase (OK_ENOUGH_ANCIENT);

	    stack2 = 1;
	}

	switch (stack0)
	{
	    case 0:
		pstack[0] = alien_races;
		break;
	    case 1:
		pstack[0] = 0;
		break;
	}
	switch (stack1)
	{
	    case 0:
		pstack[1] = the_war;
		break;
	    case 1:
		pstack[1] = 0;
		break;
	}
	switch (stack2)
	{
	    case 0:
		pstack[2] = ancient_history;
		break;
	    case 1:
		pstack[2] = 0;
		break;
	}

	if (pstack[0])
	{
	    Response (pstack[0], AlienRaces);
	}
	if (pstack[1])
	{
	    Response (pstack[1], WarInfo);
	}
	if (pstack[2])
	{
	    Response (pstack[2], AncientHistory);
	}
	Response (enough_history, NeedInfo);
    }
    POP_CONTEXT
}

PROC(STATIC
void far DefeatUrquan, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
#define HOW_FIND_URQUAN			(1 << 0)
#define HOW_FIGHT_URQUAN		(1 << 1)
#define HOW_ALLY_AGAINST_URQUAN		(1 << 2)
#define HOW_STRONG_AGAINST_URQUAN	(1 << 3)
	static BYTE	DefeatMask = 0;
	PROC_LOCAL(
	void far TellMission, (R),
	    ARG_END	(RESPONSE_REF	R)
	);

	if (PLAYER_SAID (R, how_defeat))
	{
	    NPCPhrase (DEFEAT_LIKE_SO);
	    DefeatMask = 0;
	}
	else if (PLAYER_SAID (R, how_find_urquan))
	{
	    NPCPhrase (FIND_URQUAN);
	    DefeatMask |= HOW_FIND_URQUAN;
	}
	else if (PLAYER_SAID (R, how_fight_urquan))
	{
	    NPCPhrase (FIGHT_URQUAN);
	    DefeatMask |= HOW_FIGHT_URQUAN;
	}
	else if (PLAYER_SAID (R, how_ally))
	{
	    NPCPhrase (ALLY_LIKE_SO);
	    DefeatMask |= HOW_ALLY_AGAINST_URQUAN;
	}
	else if (PLAYER_SAID (R, how_get_strong))
	{
	    NPCPhrase (STRONG_LIKE_SO);
	    DefeatMask |= HOW_STRONG_AGAINST_URQUAN;
	}

	if (!(DefeatMask & HOW_FIND_URQUAN))
	    Response (how_find_urquan, DefeatUrquan);
	if (!(DefeatMask & HOW_FIGHT_URQUAN))
	    Response (how_fight_urquan, DefeatUrquan);
	if (!(DefeatMask & HOW_ALLY_AGAINST_URQUAN))
	    Response (how_ally, DefeatUrquan);
	if (!(DefeatMask & HOW_STRONG_AGAINST_URQUAN))
	    Response (how_get_strong, DefeatUrquan);
	Response (enough_defeat, TellMission);
    }
    POP_CONTEXT
}

PROC(STATIC
void near AnalyzeCondition, (),
    ARG_VOID
)
{
    BYTE	i;
    BYTE	num_thrusters = 0,
		num_jets = 0,
		num_guns = 0,
		num_bays = 0,
		num_batts = 0,
		num_track = 0,
		num_defense = 0;
    BOOLEAN	HasMinimum;

    for (i = 0; i < NUM_DRIVE_SLOTS; ++i)
    {
	if (GLOBAL_SIS (DriveSlots[i]) < EMPTY_SLOT)
	    ++num_thrusters;
    }
    for (i = 0; i < NUM_JET_SLOTS; ++i)
    {
	if (GLOBAL_SIS (JetSlots[i]) < EMPTY_SLOT)
	    ++num_jets;
    }
    for (i = 0; i < NUM_MODULE_SLOTS; ++i)
    {
	BYTE	which_piece;

	switch (which_piece = GLOBAL_SIS (ModuleSlots[i]))
	{
	    case STORAGE_BAY:
		++num_bays;
		break;
	    case DYNAMO_UNIT:
	    case SHIVA_FURNACE:
		num_batts += 1 + (which_piece - DYNAMO_UNIT);
		break;
	    case GUN_WEAPON:
	    case BLASTER_WEAPON:
	    case CANNON_WEAPON:
		num_guns += 1 + (which_piece - GUN_WEAPON);
		break;
	    case TRACKING_SYSTEM:
		++num_track;
		break;
	    case ANTIMISSILE_DEFENSE:
		++num_defense;
		break;
	}
    }
    if (num_track && num_guns)
	num_guns += 2;

    HasMinimum = (num_thrusters >= 7 && num_jets >= 5
	    && GLOBAL_SIS (CrewEnlisted) >= CREW_POD_CAPACITY
	    && GLOBAL_SIS (FuelOnBoard) >= FUEL_TANK_CAPACITY
	    && num_bays >= 1 && GLOBAL_SIS (NumLanders)
	    && num_batts >= 1 && num_guns >= 2);
    NPCPhrase (LETS_SEE);
    if (!HasMinimum && GET_GAME_STATE (CHMMR_BOMB_STATE) < 2)
    {
	NPCPhrase (IMPROVE_1);
	if (num_thrusters < 7)
	    NPCPhrase (NEED_THRUSTERS_1);
	if (num_jets < 5)
	    NPCPhrase (NEED_TURN_1);
	if (num_guns < 2)
	    NPCPhrase (NEED_GUNS_1);
	if (GLOBAL_SIS (CrewEnlisted) < CREW_POD_CAPACITY)
	    NPCPhrase (NEED_CREW_1);
	if (GLOBAL_SIS (FuelOnBoard) < FUEL_TANK_CAPACITY)
	    NPCPhrase (NEED_FUEL_1);
	if (num_bays < 1)
	    NPCPhrase (NEED_STORAGE_1);
	if (GLOBAL_SIS (NumLanders) == 0)
	    NPCPhrase (NEED_LANDERS_1);
	if (num_batts < 1)
	    NPCPhrase (NEED_DYNAMOS_1);

	if (GLOBAL_SIS (ResUnits) >= 3000)
	    NPCPhrase (IMPROVE_FLAGSHIP_WITH_RU);
	else
	    NPCPhrase (GO_GET_MINERALS);
    }
    else
    {
	BYTE	num_aliens = 0;
	COUNT	FleetStrength;
	BOOLEAN	HasMaximum;

	FleetStrength = ActivateStarShip (0, ESCORT_WORTH);
	for (i = 0; i < NUM_AVAILABLE_RACES; ++i)
	{
	    if (i != HUMAN_SHIP
		    && (ActivateStarShip (i, CHECK_ALLIANCE) & GOOD_GUY))
		++num_aliens;
	}

	HasMaximum = (num_thrusters == NUM_DRIVE_SLOTS
		&& num_jets == NUM_JET_SLOTS
		&& GLOBAL_SIS (CrewEnlisted) >= CREW_POD_CAPACITY * 3
		&& GLOBAL_SIS (FuelOnBoard) >= FUEL_TANK_CAPACITY * 3
		&& GLOBAL_SIS (NumLanders) >= 3
		&& num_batts >= 4 && num_guns >= 7 && num_defense >= 2);
	if (!HasMaximum && GET_GAME_STATE (CHMMR_BOMB_STATE) < 2)
	    NPCPhrase (GOT_OK_FLAGSHIP);
	else
	    NPCPhrase (GOT_AWESOME_FLAGSHIP);

	if (GET_GAME_STATE (CHMMR_BOMB_STATE) >= 2)
	{
	    NPCPhrase (CHMMR_IMPROVED_BOMB);
	    if (FleetStrength < 20000)
		NPCPhrase (MUST_ACQ_AWESOME_FLEET);
	    else
	    {
		NPCPhrase (GOT_AWESOME_FLEET);
		if (!GET_GAME_STATE (TALKING_PET_ON_SHIP))
		    NPCPhrase (MUST_ELIM_URQUAN_GUARDS);
		else
		    NPCPhrase (GO_DESTROY_SAMATRA);
	    }
	}
	else if (num_aliens < 2)
	    NPCPhrase (GO_ALLY_WITH_ALIENS);
	else
	{
	    NPCPhrase (MADE_SOME_ALLIES);
	    if (FleetStrength < 6000)
	    {
		if (GLOBAL_SIS (ResUnits) >= 3000)
		    NPCPhrase (BUY_COMBAT_SHIPS);
		else
		    NPCPhrase (GET_SHIPS_BY);
	    }
	    else
	    {
		NPCPhrase (GOT_OK_FLEET);
		if (!HasMaximum)
		{
		    NPCPhrase (IMPROVE_2);
		    if (num_thrusters < NUM_DRIVE_SLOTS)
			NPCPhrase (NEED_THRUSTERS_2);
		    if (num_jets < NUM_JET_SLOTS)
			NPCPhrase (NEED_TURN_2);
		    if (num_guns < 7)
			NPCPhrase (NEED_GUNS_2);
		    if (GLOBAL_SIS (CrewEnlisted) < CREW_POD_CAPACITY * 3)
			NPCPhrase (NEED_CREW_2);
		    if (GLOBAL_SIS (FuelOnBoard) < FUEL_TANK_CAPACITY * 3)
			NPCPhrase (NEED_FUEL_2);
		    if (GLOBAL_SIS (NumLanders) < 3)
			NPCPhrase (NEED_LANDERS_2);
		    if (num_batts < 4)
			NPCPhrase (NEED_DYNAMOS_2);
		    if (num_defense < 2)
			NPCPhrase (NEED_POINT);
		}
		else if (!GET_GAME_STATE (AWARE_OF_SAMATRA))
		    NPCPhrase (GO_LEARN_ABOUT_URQUAN);
		else
		{
		    NPCPhrase (KNOW_ABOUT_SAMATRA);
		    if (!GET_GAME_STATE (UTWIG_BOMB))
			NPCPhrase (FIND_WAY_TO_DEST_SAMATRA);
		    else if (GET_GAME_STATE (UTWIG_BOMB_ON_SHIP))
			NPCPhrase (MUST_INCR_BOMB_STRENGTH);
		}
	    }
	}
    }
}

PROC(STATIC
void far TellMission, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	RESPONSE_REF		pstack[4];

	if (PLAYER_SAID (R, our_mission))
	{
	    NPCPhrase (WHICH_MISSION);
	    stack0 = stack1 = stack2 = stack3 = 0;
	}
	else if (PLAYER_SAID (R, where_get_minerals))
	{
	    NPCPhrase (GET_MINERALS);

	    stack0 = 1;
	}
	else if (PLAYER_SAID (R, what_about_aliens))
	{
	    NPCPhrase (ABOUT_ALIENS);

	    stack1 = 1;
	}
	else if (PLAYER_SAID (R, what_do_now))
	{
	    AnalyzeCondition ();

	    stack2 = 1;
	}
	else if (PLAYER_SAID (R, what_about_urquan))
	{
	    NPCPhrase (MUST_DEFEAT);

	    stack3 = 1;
	}
	else if (PLAYER_SAID (R, enough_defeat))
	{
	    NPCPhrase (OK_ENOUGH_DEFEAT);

	    stack3 = 2;
	}

	switch (stack0)
	{
	    case 0:
		pstack[0] = where_get_minerals;
		break;
	    default:
		pstack[0] = 0;
		break;
	}
	switch (stack1)
	{
	    case 0:
		pstack[1] = what_about_aliens;
		break;
	    default:
		pstack[1] = 0;
		break;
	}
	switch (stack2)
	{
	    case 0:
		pstack[2] = what_do_now;
		break;
	    default:
		pstack[2] = 0;
		break;
	}
	switch (stack3)
	{
	    case 0:
		pstack[3] = what_about_urquan;
		break;
	    case 1:
		pstack[3] = how_defeat;
		break;
	    default:
		pstack[3] = 0;
		break;
	}

	if (pstack[0])
	    Response (pstack[0], TellMission);
	if (pstack[1])
	    Response (pstack[1], TellMission);
	if (pstack[2])
	    Response (pstack[2], TellMission);
	if (pstack[3])
	{
	    if (stack3 == 1)
		Response (pstack[3], DefeatUrquan);
	    else
		Response (pstack[3], TellMission);
	}

	Response (enough_mission, NeedInfo);
    }
    POP_CONTEXT
}

PROC(STATIC
void far TellStarBase, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	RESPONSE_REF	pstack[4];
	static char	buf0[80];

	if (PLAYER_SAID (R, starbase_functions))
	{
	    NPCPhrase (WHICH_FUNCTION);
	    stack0 = stack1 = stack2 = stack3 = 0;
	}
	else if (PLAYER_SAID (R, tell_me_about_fuel0))
	{
	    NPCPhrase (ABOUT_FUEL);

	    stack1 = 1;
	}
	else if (PLAYER_SAID (R, tell_me_about_crew))
	{
	    NPCPhrase (ABOUT_CREW);

	    stack2 = 2;
	}
	else if (PLAYER_SAID (R, tell_me_about_modules0))
	{
	    NPCPhrase (ABOUT_MODULES);

	    stack0 = 1;
	}
	else if (PLAYER_SAID (R, tell_me_about_ships))
	{
	    NPCPhrase (ABOUT_SHIPS);

	    stack2 = 1;
	}
	else if (PLAYER_SAID (R, tell_me_about_ru))
	{
	    NPCPhrase (ABOUT_RU);

	    stack3 = 1;
	}
	else if (PLAYER_SAID (R, tell_me_about_minerals))
	{
	    NPCPhrase (ABOUT_MINERALS);

	    stack3 = 2;
	}
	else if (PLAYER_SAID (R, tell_me_about_life))
	{
	    NPCPhrase (ABOUT_LIFE);

	    stack3 = 3;
	}

	switch (stack0)
	{
	    case 0:
		construct_response (
			buf0,
			tell_me_about_modules0,
			GLOBAL_SIS (ShipName),
			tell_me_about_modules1,
			0
			);
		pstack[0] = tell_me_about_modules0;
		break;
	    default:
		pstack[0] = 0;
		break;
	}
	switch (stack1)
	{
	    case 0:
		construct_response (
			shared_phrase_buf,
			tell_me_about_fuel0,
			GLOBAL_SIS (ShipName),
			tell_me_about_fuel1,
			0
			);
		pstack[1] = tell_me_about_fuel0;
		break;
	    default:
		pstack[1] = 0;
		break;
	}
	switch (stack2)
	{
	    case 0:
		pstack[2] = tell_me_about_ships;
		break;
	    case 1:
		pstack[2] = tell_me_about_crew;
		break;
	    default:
		pstack[2] = 0;
		break;
	}
	switch (stack3)
	{
	    case 0:
		pstack[3] = tell_me_about_ru;
		break;
	    case 1:
		pstack[3] = tell_me_about_minerals;
		break;
	    case 2:
		pstack[3] = tell_me_about_life;
		break;
	    default:
		pstack[3] = 0;
		break;
	}

	if (pstack[0])
	    DoResponsePhrase (pstack[0], TellStarBase, buf0);
	if (pstack[1])
	    DoResponsePhrase (pstack[1], TellStarBase, shared_phrase_buf);
	if (pstack[2])
	    Response (pstack[2], TellStarBase);
	if (pstack[3])
	    Response (pstack[3], TellStarBase);

	Response (enough_starbase, NeedInfo);
    }
    POP_CONTEXT
}

PROC_LOCAL(
void far NormalStarbase, (R),
    ARG_END	(RESPONSE_REF	R)
);

PROC(STATIC
void far NeedInfo, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	if (PLAYER_SAID (R, need_info))
	    NPCPhrase (WHAT_KIND_OF_INFO);
	else if (PLAYER_SAID (R, enough_starbase))
	    NPCPhrase (OK_ENOUGH_STARBASE);
	else if (PLAYER_SAID (R, enough_history))
	    NPCPhrase (OK_ENOUGH_HISTORY);
	else if (PLAYER_SAID (R, enough_mission))
	    NPCPhrase (OK_ENOUGH_MISSION);

	Response (starbase_functions, TellStarBase);
	Response (history, TellHistory);
	Response (our_mission, TellMission);
	Response (no_need_info, NormalStarbase);
    }
    POP_CONTEXT
}

PROC(STATIC
BOOLEAN near DiscussDevices, (TalkAbout),
    ARG_END	(BOOLEAN	TalkAbout)
)
{
    COUNT	i, VuxBeastIndex, PhraseIndex;
    BOOLEAN	Undiscussed;

    if (TalkAbout)
	NPCPhrase (DEVICE_HEAD);
    PhraseIndex = 2;

    VuxBeastIndex = 0;
    Undiscussed = FALSE;
    for (i = 0; i < NUM_DEVICES; ++i)
    {
	RESPONSE_REF	pStr;

	pStr = 0;
	switch (i)
	{
	    case ROSY_SPHERE_DEVICE:
		if (GET_GAME_STATE (ROSY_SPHERE_ON_SHIP)
			&& !GET_GAME_STATE (DISCUSSED_ROSY_SPHERE))
		{
		    pStr = ABOUT_SPHERE;
		    SET_GAME_STATE (DISCUSSED_ROSY_SPHERE, TalkAbout);
		}
		break;
	    case ARTIFACT_2_DEVICE:
		if (GET_GAME_STATE (ARTIFACT_2_ON_SHIP)
			&& !GET_GAME_STATE (DISCUSSED_ARTIFACT_2))
		{
		    pStr = ABOUT_ARTIFACT_2;
		    SET_GAME_STATE (DISCUSSED_ARTIFACT_2, TalkAbout);
		}
		break;
	    case ARTIFACT_3_DEVICE:
		if (GET_GAME_STATE (ARTIFACT_3_ON_SHIP)
			&& !GET_GAME_STATE (DISCUSSED_ARTIFACT_3))
		{
		    pStr = ABOUT_ARTIFACT_3;
		    SET_GAME_STATE (DISCUSSED_ARTIFACT_3, TalkAbout);
		}
		break;
	    case SUN_EFFICIENCY_DEVICE:
		if (GET_GAME_STATE (SUN_DEVICE_ON_SHIP)
			&& !GET_GAME_STATE (DISCUSSED_SUN_EFFICIENCY))
		{
		    pStr = ABOUT_SUN;
		    SET_GAME_STATE (DISCUSSED_SUN_EFFICIENCY, TalkAbout);
		}
		break;
	    case UTWIG_BOMB_DEVICE:
		if (GET_GAME_STATE (UTWIG_BOMB_ON_SHIP)
			&& !GET_GAME_STATE (DISCUSSED_UTWIG_BOMB))
		{
		    pStr = ABOUT_BOMB;
		    SET_GAME_STATE (DISCUSSED_UTWIG_BOMB, TalkAbout);
		}
		break;
	    case ULTRON_0_DEVICE:
		if (GET_GAME_STATE (ULTRON_CONDITION) == 1
			&& !GET_GAME_STATE (DISCUSSED_ULTRON))
		{
		    pStr = ABOUT_ULTRON_0;
		    SET_GAME_STATE (DISCUSSED_ULTRON, TalkAbout);
		}
		break;
	    case ULTRON_1_DEVICE:
		if (GET_GAME_STATE (ULTRON_CONDITION) == 2
			&& !GET_GAME_STATE (DISCUSSED_ULTRON))
		{
		    pStr = ABOUT_ULTRON_1;
		    SET_GAME_STATE (DISCUSSED_ULTRON, TalkAbout);
		}
		break;
	    case ULTRON_2_DEVICE:
		if (GET_GAME_STATE (ULTRON_CONDITION) == 3
			&& !GET_GAME_STATE (DISCUSSED_ULTRON))
		{
		    pStr = ABOUT_ULTRON_2;
		    SET_GAME_STATE (DISCUSSED_ULTRON, TalkAbout);
		}
		break;
	    case ULTRON_3_DEVICE:
		if (GET_GAME_STATE (ULTRON_CONDITION) == 4
			&& !GET_GAME_STATE (DISCUSSED_ULTRON))
		{
		    pStr = ABOUT_ULTRON_3;
		    SET_GAME_STATE (DISCUSSED_ULTRON, TalkAbout);
		}
		break;
	    case MAIDENS_DEVICE:
		if (GET_GAME_STATE (MAIDENS_ON_SHIP)
			&& !GET_GAME_STATE (DISCUSSED_MAIDENS))
		{
		    pStr = ABOUT_MAIDENS;
		    SET_GAME_STATE (DISCUSSED_MAIDENS, TalkAbout);
		}
		break;
	    case TALKING_PET_DEVICE:
		if (GET_GAME_STATE (TALKING_PET_ON_SHIP)
			&& !GET_GAME_STATE (DISCUSSED_TALKING_PET))
		{
		    pStr = ABOUT_TALKPET;
		    SET_GAME_STATE (DISCUSSED_TALKING_PET, TalkAbout);
		}
		break;
	    case AQUA_HELIX_DEVICE:
		if (GET_GAME_STATE (AQUA_HELIX_ON_SHIP)
			&& !GET_GAME_STATE (DISCUSSED_AQUA_HELIX))
		{
		    pStr = ABOUT_HELIX;
		    SET_GAME_STATE (DISCUSSED_AQUA_HELIX, TalkAbout);
		}
		break;
	    case CLEAR_SPINDLE_DEVICE:
		if (GET_GAME_STATE (CLEAR_SPINDLE_ON_SHIP)
			&& !GET_GAME_STATE (DISCUSSED_CLEAR_SPINDLE))
		{
		    pStr = ABOUT_SPINDLE;
		    SET_GAME_STATE (DISCUSSED_CLEAR_SPINDLE, TalkAbout);
		}
		break;
	    case UMGAH_HYPERWAVE_DEVICE:
		if (GET_GAME_STATE (UMGAH_BROADCASTERS_ON_SHIP)
			&& !GET_GAME_STATE (DISCUSSED_UMGAH_HYPERWAVE))
		{
		    pStr = ABOUT_UCASTER;
		    SET_GAME_STATE (DISCUSSED_UMGAH_HYPERWAVE, TalkAbout);
		}
		break;
#ifdef NEVER
	    case DATA_PLATE_1_DEVICE:
		if (GET_GAME_STATE (DATA_PLATE_1_ON_SHIP)
			&& !GET_GAME_STATE (DISCUSSED_DATA_PLATE_1))
		{
		    pStr = (PSTR)ABOUT_DATAPLATE_1;
		    SET_GAME_STATE (DISCUSSED_DATA_PLATE_1, TalkAbout);
		}
		break;
	    case DATA_PLATE_2_DEVICE:
		if (GET_GAME_STATE (DATA_PLATE_2_ON_SHIP)
			&& !GET_GAME_STATE (DISCUSSED_DATA_PLATE_2))
		{
		    pStr = (PSTR)ABOUT_DATAPLATE_2;
		    SET_GAME_STATE (DISCUSSED_DATA_PLATE_2, TalkAbout);
		}
		break;
	    case DATA_PLATE_3_DEVICE:
		if (GET_GAME_STATE (DATA_PLATE_3_ON_SHIP)
			&& !GET_GAME_STATE (DISCUSSED_DATA_PLATE_3))
		{
		    pStr = (PSTR)ABOUT_DATAPLATE_3;
		    SET_GAME_STATE (DISCUSSED_DATA_PLATE_3, TalkAbout);
		}
		break;
#endif /* NEVER */
	    case TAALO_PROTECTOR_DEVICE:
		if (GET_GAME_STATE (TAALO_PROTECTOR_ON_SHIP)
			&& !GET_GAME_STATE (DISCUSSED_TAALO_PROTECTOR))
		{
		    pStr = ABOUT_SHIELD;
		    SET_GAME_STATE (DISCUSSED_TAALO_PROTECTOR, TalkAbout);
		}
		break;
	    case EGG_CASING0_DEVICE:
		if (GET_GAME_STATE (EGG_CASE0_ON_SHIP)
			&& !GET_GAME_STATE (DISCUSSED_EGG_CASING0))
		{
		    pStr = ABOUT_EGGCASE_0;
		    SET_GAME_STATE (DISCUSSED_EGG_CASING0, TalkAbout);
		    SET_GAME_STATE (DISCUSSED_EGG_CASING1, TalkAbout);
		    SET_GAME_STATE (DISCUSSED_EGG_CASING2, TalkAbout);
		}
		break;
	    case EGG_CASING1_DEVICE:
		if (GET_GAME_STATE (EGG_CASE1_ON_SHIP)
			&& !GET_GAME_STATE (DISCUSSED_EGG_CASING1))
		{
		    pStr = ABOUT_EGGCASE_0;
		    SET_GAME_STATE (DISCUSSED_EGG_CASING0, TalkAbout);
		    SET_GAME_STATE (DISCUSSED_EGG_CASING1, TalkAbout);
		    SET_GAME_STATE (DISCUSSED_EGG_CASING2, TalkAbout);
		}
		break;
	    case EGG_CASING2_DEVICE:
		if (GET_GAME_STATE (EGG_CASE2_ON_SHIP)
			&& !GET_GAME_STATE (DISCUSSED_EGG_CASING2))
		{
		    pStr = ABOUT_EGGCASE_0;
		    SET_GAME_STATE (DISCUSSED_EGG_CASING0, TalkAbout);
		    SET_GAME_STATE (DISCUSSED_EGG_CASING1, TalkAbout);
		    SET_GAME_STATE (DISCUSSED_EGG_CASING2, TalkAbout);
		}
		break;
	    case SYREEN_SHUTTLE_DEVICE:
		if (GET_GAME_STATE (SYREEN_SHUTTLE_ON_SHIP)
			&& !GET_GAME_STATE (DISCUSSED_SYREEN_SHUTTLE))
		{
		    pStr = ABOUT_SHUTTLE;
		    SET_GAME_STATE (DISCUSSED_SYREEN_SHUTTLE, TalkAbout);
		}
		break;
	    case VUX_BEAST_DEVICE:
		if (GET_GAME_STATE (VUX_BEAST_ON_SHIP)
			&& !GET_GAME_STATE (DISCUSSED_VUX_BEAST))
		{
		    pStr = ABOUT_VUXBEAST0;
		    SET_GAME_STATE (DISCUSSED_VUX_BEAST, TalkAbout);
		}
		break;
	    case DESTRUCT_CODE_DEVICE:
		if (GET_GAME_STATE (DESTRUCT_CODE_ON_SHIP)
			&& !GET_GAME_STATE (DISCUSSED_DESTRUCT_CODE))
		{
		    pStr = ABOUT_DESTRUCT;
		    SET_GAME_STATE (DISCUSSED_DESTRUCT_CODE, TalkAbout);
		}
		break;
	    case PORTAL_SPAWNER_DEVICE:
		if (GET_GAME_STATE (PORTAL_SPAWNER_ON_SHIP)
			&& !GET_GAME_STATE (DISCUSSED_PORTAL_SPAWNER))
		{
		    pStr = ABOUT_PORTAL;
		    SET_GAME_STATE (DISCUSSED_PORTAL_SPAWNER, TalkAbout);
		}
		break;
	    case URQUAN_WARP_DEVICE:
		if (GET_GAME_STATE (PORTAL_KEY_ON_SHIP)
			&& !GET_GAME_STATE (DISCUSSED_URQUAN_WARP))
		{
		    pStr = ABOUT_WARPPOD;
		    SET_GAME_STATE (DISCUSSED_URQUAN_WARP, TalkAbout);
		}
		break;
	    case BURVIX_HYPERWAVE_DEVICE:
		if (GET_GAME_STATE (BURV_BROADCASTERS_ON_SHIP)
			&& !GET_GAME_STATE (DISCUSSED_BURVIX_HYPERWAVE))
		{
		    pStr = ABOUT_BCASTER;
		    SET_GAME_STATE (DISCUSSED_BURVIX_HYPERWAVE, TalkAbout);
		}
		break;
	}

	if (pStr)
	{
	    if (TalkAbout)
	    {
		if (PhraseIndex > 2)
		    NPCPhrase (BETWEEN_DEVICES);
		NPCPhrase (pStr);
		if (pStr == ABOUT_VUXBEAST0)
		{
		    VuxBeastIndex = ++PhraseIndex;
		    NPCPhrase (ABOUT_VUXBEAST1);
		}
	    }
	    PhraseIndex += 2;
	}
    }

    if (TalkAbout)
    {
	NPCPhrase (DEVICE_TAIL);

	if (VuxBeastIndex)
	{
	    for (i = 1; i < VuxBeastIndex; ++i)
		AlienTalkSegue (i);
	    i = CommData.AlienTalkDesc.NumFrames;
	    CommData.AlienTalkDesc.NumFrames = 0;
	    AlienTalkSegue (VuxBeastIndex);
	    CommData.AlienTalkDesc.NumFrames = i;
	    AlienTalkSegue ((COUNT)~0);
	}
    }

    return (PhraseIndex > 2);
}

PROC(STATIC
BOOLEAN near CheckTiming, (month_index, day_index),
    ARG		(COUNT	month_index)
    ARG_END	(COUNT	day_index)
)
{
    COUNT	mi, year_index;
    BYTE	days_in_month[12] =
    {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
    };

    mi = GET_GAME_STATE (STARBASE_MONTH);
    year_index = START_YEAR;

    day_index += GET_GAME_STATE (STARBASE_DAY);
    while (day_index > days_in_month[mi - 1])
    {
	day_index -= days_in_month[mi - 1];
	if (++mi > 12)
	{
	    mi = 1;
	    ++year_index;
	}
    }

    month_index += mi;
    year_index += (month_index - 1) / 12;
    month_index = ((month_index - 1) % 12) + 1;

    return (year_index < GLOBAL (GameClock.year_index)
	    || (year_index == GLOBAL (GameClock.year_index)
	    && (month_index < GLOBAL (GameClock.month_index)
	    || (month_index == GLOBAL (GameClock.month_index)
	    && day_index < GLOBAL (GameClock.day_index)))));
}

PROC(STATIC
void near CheckBulletins, (Repeat),
    ARG_END	(BOOLEAN	Repeat)
)
{
    RESPONSE_REF	pIntro;
    BYTE	b0, b1, b2, b3;
    DWORD	BulletinMask;

    if (Repeat)
	BulletinMask = CurBulletinMask ^ 0xFFFFFFFFL;
    else
    {
	b0 = GET_GAME_STATE (STARBASE_BULLETS0);
	b1 = GET_GAME_STATE (STARBASE_BULLETS1);
	b2 = GET_GAME_STATE (STARBASE_BULLETS2);
	b3 = GET_GAME_STATE (STARBASE_BULLETS3);
	BulletinMask = MAKE_DWORD (MAKE_WORD (b0, b1), MAKE_WORD (b2, b3));
    }

    pIntro = 0;
    for (b0 = 0; b0 < 32; ++b0)
    {
	if (!(BulletinMask & (1L << b0)))
	{
	    RESPONSE_REF	pStr;

	    pStr = 0;
	    switch (b0)
	    {
		case 0:
		    if (ActivateStarShip (SPATHI_SHIP, CHECK_ALLIANCE)
			    & GOOD_GUY)
		    {
			pStr = STARBASE_BULLETIN_1;
		    }
		    break;
		case 1:
		    if (ActivateStarShip (ZOQFOTPIK_SHIP, CHECK_ALLIANCE)
			    & GOOD_GUY)
		    {
			pStr = STARBASE_BULLETIN_2;
		    }
		    break;
		case 2:
		    if (ActivateStarShip (SUPOX_SHIP, CHECK_ALLIANCE)
			    & GOOD_GUY)
		    {
			pStr = STARBASE_BULLETIN_3;
		    }
		    break;
		case 3:
		    if (ActivateStarShip (UTWIG_SHIP, CHECK_ALLIANCE)
			    & GOOD_GUY)
		    {
			pStr = STARBASE_BULLETIN_4;
		    }
		    break;
		case 4:
		    if (ActivateStarShip (ORZ_SHIP, CHECK_ALLIANCE)
			    & GOOD_GUY)
		    {
			pStr = STARBASE_BULLETIN_5;
		    }
		    break;
		case 5:
		    if (GET_GAME_STATE (ARILOU_MANNER) == 2)
			BulletinMask |= 1L << b0;
		    else if (GET_GAME_STATE (PORTAL_SPAWNER)
			    && (Repeat || ActivateStarShip (
				    ARILOU_SHIP, FEASIBILITY_STUDY
				    )))
		    {
#define NUM_GIFT_ARILOUS	3
			pStr = STARBASE_BULLETIN_6;
			if (!Repeat)
			    ActivateStarShip (ARILOU_SHIP, NUM_GIFT_ARILOUS);
		    }
		    break;
		case 6:
		    if (GET_GAME_STATE (ZOQFOT_DISTRESS) == 1)
		    {
			pStr = STARBASE_BULLETIN_7;
		    }
		    break;
		case 7:
		    if (GET_GAME_STATE (MET_MELNORME))
			BulletinMask |= 1L << b0;
		    else if (CheckTiming (3, 0))
		    {
			pStr = STARBASE_BULLETIN_8;
		    }
		    break;
		case 8:
		    if (GET_GAME_STATE (MET_MELNORME))
			BulletinMask |= 1L << b0;
		    else if (CheckTiming (6, 0))
		    {
			pStr = STARBASE_BULLETIN_9;
		    }
		    break;
		case 9:
		    if (GET_GAME_STATE (FOUND_PLUTO_SPATHI))
			BulletinMask |= 1L << b0;
		    else if (CheckTiming (0, 7))
		    {
			pStr = STARBASE_BULLETIN_10;
		    }
		    break;
		case 10:
		    if (GET_GAME_STATE (SPATHI_SHIELDED_SELVES))
		    {
			pStr = STARBASE_BULLETIN_11;
		    }
		    break;
		case 11:
		    if (GET_GAME_STATE (ZOQFOT_HOME_VISITS)
			    || GET_GAME_STATE (ZOQFOT_GRPOFFS0)
			    || GET_GAME_STATE (ZOQFOT_GRPOFFS1)
			    || GET_GAME_STATE (ZOQFOT_GRPOFFS2)
			    || GET_GAME_STATE (ZOQFOT_GRPOFFS3))
			BulletinMask |= 1L << b0;
		    else if (CheckTiming (0, 42))
		    {
			pStr = STARBASE_BULLETIN_12;
		    }
		    break;
		case 12:
		    if (ActivateStarShip (CHMMR_SHIP, CHECK_ALLIANCE)
			    & GOOD_GUY)
		    {
			pStr = STARBASE_BULLETIN_13;
		    }
		    break;
		case 13:
		    if (ActivateStarShip (SHOFIXTI_SHIP, CHECK_ALLIANCE)
			    & GOOD_GUY)
		    {
			pStr = STARBASE_BULLETIN_14;
		    }
		    break;
		case 14:
		    if (GET_GAME_STATE (PKUNK_MISSION))
		    {
			pStr = STARBASE_BULLETIN_15;
		    }
		    break;
		case 15:
		    if (GET_GAME_STATE (DESTRUCT_CODE_ON_SHIP))
			BulletinMask |= 1L << b0;
		    else if (CheckTiming (7, 0))
		    {
			pStr = STARBASE_BULLETIN_16;
		    }
		    break;
		case 16:
		    break;
		case 17:
		    if (GET_GAME_STATE (YEHAT_ABSORBED_PKUNK))
		    {
			pStr = STARBASE_BULLETIN_18;
		    }
		    break;
		case 18:
		    if (GET_GAME_STATE (CHMMR_BOMB_STATE) == 2)
		    {
			pStr = STARBASE_BULLETIN_19;
		    }
		    break;
		case 19:
		    break;
		case 20:
		    break;
		case 21:
		    if (GET_GAME_STATE (ZOQFOT_DISTRESS) == 2)
		    {
			pStr = STARBASE_BULLETIN_22;
		    }
		    break;
		case 22:
		    break;
		case 23:
		    break;
		case 24:
		    break;
		case 25:
		    break;
		case 26:
		{
		    COUNT	crew_sold;

		    crew_sold = MAKE_WORD (
			    GET_GAME_STATE (CREW_SOLD_TO_DRUUGE0),
			    GET_GAME_STATE (CREW_SOLD_TO_DRUUGE1)
			    );
		    if (crew_sold > 100)
			BulletinMask |= 1L << b0;
		    else if (crew_sold)
		    {
			pStr = STARBASE_BULLETIN_27;
		    }
		    break;
		}
		case 27:
		{
		    COUNT	crew_sold;

		    crew_sold = MAKE_WORD (
			    GET_GAME_STATE (CREW_SOLD_TO_DRUUGE0),
			    GET_GAME_STATE (CREW_SOLD_TO_DRUUGE1)
			    );
		    if (crew_sold > 250)
			BulletinMask |= 1L << b0;
		    else if (crew_sold > 100)
		    {
			pStr = STARBASE_BULLETIN_28;
		    }
		    break;
		}
		case 28:
		{
		    COUNT	crew_bought;

		    crew_bought = MAKE_WORD (
			    GET_GAME_STATE (CREW_PURCHASED0),
			    GET_GAME_STATE (CREW_PURCHASED1)
			    );
		    if (crew_bought >= CREW_EXPENSE_THRESHOLD)
		    {
			pStr = STARBASE_BULLETIN_29;
		    }
		    break;
		}
		case 29:
		    if (MAKE_WORD (
			    GET_GAME_STATE (CREW_SOLD_TO_DRUUGE0),
			    GET_GAME_STATE (CREW_SOLD_TO_DRUUGE1)
			    ) > 250)
		    {
			pStr = STARBASE_BULLETIN_30;
		    }
		    break;
		case 30:
		    break;
		case 31:
		    break;
	    }

	    if (pStr)
	    {
		if (pIntro)
		    NPCPhrase (BETWEEN_BULLETINS);
		else if (Repeat)
		    pIntro = BEFORE_WE_GO_ON_1;
		else
		{
		    switch ((BYTE)random () % 7)
		    {
			case 0:
			    pIntro = BEFORE_WE_GO_ON_1;
			    break;
			case 1:
			    pIntro = BEFORE_WE_GO_ON_2;
			    break;
			case 2:
			    pIntro = BEFORE_WE_GO_ON_3;
			    break;
			case 3:
			    pIntro = BEFORE_WE_GO_ON_4;
			    break;
			case 4:
			    pIntro = BEFORE_WE_GO_ON_5;
			    break;
			case 5:
			    pIntro = BEFORE_WE_GO_ON_6;
			    break;
			default:
			    pIntro = BEFORE_WE_GO_ON_7;
			    break;
		    }

		    NPCPhrase (pIntro);
		}

		NPCPhrase (pStr);
		CurBulletinMask |= 1L << b0;
	    }
	}
    }

    if (pIntro == 0 && GET_GAME_STATE (STARBASE_VISITED))
	NPCPhrase (RETURN_HELLO);
    else if (!Repeat)
    {
	b0 = LOBYTE (LOWORD (BulletinMask));
	b1 = HIBYTE (LOWORD (BulletinMask));
	b2 = LOBYTE (HIWORD (BulletinMask));
	b3 = HIBYTE (HIWORD (BulletinMask));
	SET_GAME_STATE (STARBASE_BULLETS0, b0);
	SET_GAME_STATE (STARBASE_BULLETS1, b1);
	SET_GAME_STATE (STARBASE_BULLETS2, b2);
	SET_GAME_STATE (STARBASE_BULLETS3, b3);
    }
}

PROC(STATIC
void far NormalStarbase, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	if (PLAYER_SAID (R, no_need_info))
	    NPCPhrase (OK_NO_NEED_INFO);
	else if (PLAYER_SAID (R, new_devices))
	    DiscussDevices (TRUE);
	else if (PLAYER_SAID (R, repeat_bulletins))
	    CheckBulletins (TRUE);
	else if (R == 0)
	{
	    if (GET_GAME_STATE (MOONBASE_ON_SHIP))
	    {
		NPCPhrase (STARBASE_IS_READY);

		SetSemaphore (&GraphicsSem);
		DeltaSISGauges (0, 0, 2500);
		ClearSemaphore (&GraphicsSem);
		SET_GAME_STATE (STARBASE_MONTH,
			GLOBAL (GameClock.month_index));
		SET_GAME_STATE (STARBASE_DAY,
			GLOBAL (GameClock.day_index));
	    }
	    else if (GET_GAME_STATE (STARBASE_VISITED))
	    {
		CheckBulletins (FALSE);
	    }
	    else
	    {
		RESPONSE_REF	pStr;

		switch ((BYTE)random () & 7)
		{
		    case 0:
			pStr = NORMAL_HELLO_A;
			break;
		    case 1:
			pStr = NORMAL_HELLO_B;
			break;
		    case 2:
			pStr = NORMAL_HELLO_C;
			break;
		    case 3:
			pStr = NORMAL_HELLO_D;
			break;
		    case 4:
			pStr = NORMAL_HELLO_E;
			break;
		    case 5:
			pStr = NORMAL_HELLO_F;
			break;
		    case 6:
			pStr = NORMAL_HELLO_G;
			break;
		    case 7:
			pStr = NORMAL_HELLO_H;
			break;
		}
		NPCPhrase (pStr);
		CheckBulletins (FALSE);
	    }

	    SET_GAME_STATE (STARBASE_VISITED, 1);
	}

	if (GLOBAL_SIS (TotalElementMass))
	{
	    PROC_LOCAL(
	    void far SellMinerals, (R),
		ARG_END	(RESPONSE_REF	R)
	    );

	    Response (have_minerals, SellMinerals);
	}
	if (DiscussDevices (FALSE))
	    Response (new_devices, NormalStarbase);
	if (CurBulletinMask)
	    Response (repeat_bulletins, NormalStarbase);
	Response (need_info, NeedInfo);
	Response (goodbye_starbase_commander, ByeBye);
    }
    POP_CONTEXT
}

PROC(STATIC
void far SellMinerals, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	COUNT		i, total;
	BOOLEAN		Sleepy;
	RESPONSE_REF	pStr;

	total = 0;
	Sleepy = TRUE;
	for (i = 0; i < NUM_ELEMENT_CATEGORIES; ++i)
	{
	    COUNT	amount;
	    DWORD	TimeIn;

	    if (i == 0)
	    {
		DrawCargoStrings ((BYTE)~0, (BYTE)~0);
		TimeIn = SleepTask (GetTimeCounter () + (ONE_SECOND / 2));
		DrawCargoStrings ((BYTE)0, (BYTE)0);
	    }
	    else if (Sleepy)
	    {
		DrawCargoStrings ((BYTE)(i - 1), (BYTE)i);
		TimeIn = GetTimeCounter ();
	    }

	    if (amount = GLOBAL_SIS (ElementAmounts[i]))
	    {
		total += amount * GLOBAL (ElementWorth[i]);
		do
		{
		    if (!Sleepy || AnyButtonPress (TRUE))
		    {
			Sleepy = FALSE;
			GLOBAL_SIS (ElementAmounts[i]) = 0;
			GLOBAL_SIS (TotalElementMass) -= amount;
			SetSemaphore (&GraphicsSem);
			DeltaSISGauges (0, 0, amount * GLOBAL (ElementWorth[i]));
			ClearSemaphore (&GraphicsSem);
			break;
		    }
		    
		    --GLOBAL_SIS (ElementAmounts[i]);
		    --GLOBAL_SIS (TotalElementMass);
		    TimeIn = TaskSwitch ();
		    DrawCargoStrings ((BYTE)i, (BYTE)i);
		    SetSemaphore (&GraphicsSem);
		    ShowRemainingCapacity ();
		    DeltaSISGauges (0, 0, GLOBAL (ElementWorth[i]));
		    ClearSemaphore (&GraphicsSem);
		} while (--amount);
	    }
	    if (Sleepy)
		TimeIn = SleepTask (TimeIn + (ONE_SECOND / 4));
	}
	SleepTask (GetTimeCounter () + (ONE_SECOND / 2));

	SetSemaphore (&GraphicsSem);
	ClearSISRect (DRAW_SIS_DISPLAY);
	ClearSemaphore (&GraphicsSem);
//	DrawStorageBays (FALSE);

	if (total < 1000)
	{
	    total = GET_GAME_STATE (LIGHT_MINERAL_LOAD);
	    switch (total++)
	    {
		case 0:
		    pStr = LIGHT_LOAD_A;
		    break;
		case 1:
		    pStr = LIGHT_LOAD_B;
		    break;
		case 2:
		    pStr = LIGHT_LOAD_C;
		    break;
		case 3:
		    pStr = LIGHT_LOAD_D;
		    break;
		case 4:
		    pStr = LIGHT_LOAD_E;
		    break;
		case 5:
		    pStr = LIGHT_LOAD_F;
		    break;
		case 6:
		    --total;
		    pStr = LIGHT_LOAD_G;
		    break;
	    }
	    SET_GAME_STATE (LIGHT_MINERAL_LOAD, total);
	}
	else if (total < 2500)
	{
	    total = GET_GAME_STATE (MEDIUM_MINERAL_LOAD);
	    switch (total++)
	    {
		case 0:
		    pStr = MEDIUM_LOAD_A;
		    break;
		case 1:
		    pStr = MEDIUM_LOAD_B;
		    break;
		case 2:
		    pStr = MEDIUM_LOAD_C;
		    break;
		case 3:
		    pStr = MEDIUM_LOAD_D;
		    break;
		case 4:
		    pStr = MEDIUM_LOAD_E;
		    break;
		case 5:
		    pStr = MEDIUM_LOAD_F;
		    break;
		case 6:
		    --total;
		    pStr = MEDIUM_LOAD_G;
		    break;
	    }
	    SET_GAME_STATE (MEDIUM_MINERAL_LOAD, total);
	}
	else
	{
	    total = GET_GAME_STATE (HEAVY_MINERAL_LOAD);
	    switch (total++)
	    {
		case 0:
		    pStr = HEAVY_LOAD_A;
		    break;
		case 1:
		    pStr = HEAVY_LOAD_B;
		    break;
		case 2:
		    pStr = HEAVY_LOAD_C;
		    break;
		case 3:
		    pStr = HEAVY_LOAD_D;
		    break;
		case 4:
		    pStr = HEAVY_LOAD_E;
		    break;
		case 5:
		    pStr = HEAVY_LOAD_F;
		    break;
		case 6:
		    --total;
		    pStr = HEAVY_LOAD_G;
		    break;
	    }
	    SET_GAME_STATE (HEAVY_MINERAL_LOAD, total);
	}

	NPCPhrase (pStr);

	NormalStarbase (R);
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
	NormalStarbase (NULL_PTR);
    }
    POP_CONTEXT
}

static COUNT	far
uninit_starbase ()
{
    PUSH_CONTEXT
    {
	SET_GAME_STATE (MOONBASE_ON_SHIP, 0);
	if (GET_GAME_STATE (CHMMR_BOMB_STATE) == 2)
	{
	    SET_GAME_STATE (CHMMR_BOMB_STATE, 3);
	}
    }
    POP_CONTEXT

    return (0);
}

LOCDATAPTR	far
init_starbase_comm ()
{
    LOCDATAPTR	retval;

    PUSH_CONTEXT
    {
	commander_desc.init_encounter_func = Intro;
	commander_desc.uninit_encounter_func = uninit_starbase;

strcpy(aiff_folder, "comm/starbas/starb");

	CurBulletinMask = 0;
	SET_GAME_STATE (BATTLE_SEGUE, 0);
	retval = &commander_desc;
    }
    POP_CONTEXT

    return (retval);
}
