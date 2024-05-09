#ifndef _RACES_H
#define _RACES_H

#define RACES_PER_PLAYER	7
#define MAX_SHIPS_PER_SIDE	14

		/* heat of battle specific flags */
#define LEFT			(1 << 0)
#define RIGHT			(1 << 1)
#define THRUST			(1 << 2)
#define WEAPON			(1 << 3)
#define SPECIAL			(1 << 4)
#define LOW_ON_ENERGY		(1 << 5)
#define SHIP_BEYOND_MAX_SPEED	(1 << 6)
#define SHIP_AT_MAX_SPEED	(1 << 7)
#define SHIP_IN_GRAVITY_WELL	(1 << 8)
#define PLAY_VICTORY_DITTY	(1 << 9)

		/* ship specific flags */
/*
#define GOOD_GUY		(1 << 0)
#define BAD_GUY			(1 << 1)
*/
#define SEEKING_WEAPON		(1 << 2)
#define SEEKING_SPECIAL		(1 << 3)
#define POINT_DEFENSE		(1 << 4)
#define IMMEDIATE_WEAPON	(1 << 5)
#define CREW_IMMUNE		(1 << 6)
#define FIRES_FORE		(1 << 7)
#define FIRES_RIGHT		(1 << 8)
#define FIRES_AFT		(1 << 9)
#define FIRES_LEFT		(1 << 10)
#define SHIELD_DEFENSE		(1 << 11)
#define DONT_CHASE		(1 << 12)

#define MAX_SHAPES_ACTIVE	2	/* SHIP_SHAPE_ACTIVE &&
					 * SPECIAL_SHAPE_ACTIVE
					 */

typedef struct captain_stuff
{
    FRAME	background, turn, thrust, weapon, special;
} CAPTAIN_STUFF;
typedef CAPTAIN_STUFF	*PCAPTAIN_STUFF;
typedef CAPTAIN_STUFF	near *NPCAPTAIN_STUFF;
typedef CAPTAIN_STUFF	far *LPCAPTAIN_STUFF;

#define CAPTAIN_STUFFPTR	LPCAPTAIN_STUFF

typedef struct
{
    COUNT	ManeuverabilityIndex;
    COUNT	WeaponRange;
    PROC_PARAMETER(
    void (far *intelligence_func), (ShipPtr, ObjectsOfConcern, ConcernCounter),
	ARG	(LPVOID	ShipPtr)
	ARG	(LPVOID	ObjectsOfConcern)
	ARG_END	(COUNT	ConcernCounter)
    );
} INTEL_STUFF;

typedef struct
{
    COUNT		max_thrust,
			thrust_increment;
    BYTE		energy_regeneration,
			weapon_energy_cost,
			special_energy_cost,
			energy_wait,
			turn_wait,
			thrust_wait,
			weapon_wait,
			special_wait,
			ship_mass;
} CHARACTERISTIC_STUFF;
typedef CHARACTERISTIC_STUFF	*PCHARACTERISTIC_STUFF;
typedef CHARACTERISTIC_STUFF	near *NPCHARACTERISTIC_STUFF;
typedef CHARACTERISTIC_STUFF	far *LPCHARACTERISTIC_STUFF;

typedef struct
{
    UWORD		ship_flags;
    BYTE		var1, var2;
    BYTE		crew_level, max_crew;
    BYTE		energy_level, max_energy;
    POINT		loc;

    STRING		race_strings;
    FRAME		icons, melee_icon;
} SHIP_INFO;
typedef SHIP_INFO	*PSHIP_INFO;
typedef SHIP_INFO	near *NPSHIP_INFO;
typedef SHIP_INFO	far *LPSHIP_INFO;

#define SHIP_INFOPTR	LPSHIP_INFO

#define ship_cost		var1
#define group_counter		ship_flags

enum
{
    IN_ORBIT = 0,
    EXPLORE,
    FLEE,
    ON_STATION,

    IGNORE_FLAGSHIP = 1 << 2,
    REFORM_GROUP = 1 << 3
};
#define MAX_REVOLUTIONS	5

#define GET_RACE_ID(s)		((s)->ShipInfo.var1)
#define SET_RACE_ID(s,v)	((s)->ShipInfo.var1 = (v))
#define GET_GROUP_LOC(s)	LONIBBLE ((s)->ShipInfo.var2)
#define SET_GROUP_LOC(s,v)	((s)->ShipInfo.var2 = MAKE_BYTE ( \
					(v), HINIBBLE ((s)->ShipInfo.var2) \
					))
#define GET_GROUP_MISSION(s)	HINIBBLE ((s)->ShipInfo.var2)
#define SET_GROUP_MISSION(s,v)	((s)->ShipInfo.var2 = MAKE_BYTE ( \
					LONIBBLE ((s)->ShipInfo.var2), (v) \
					))
#define GET_GROUP_DEST(s)	LONIBBLE ((s)->ShipInfo.energy_level)
#define SET_GROUP_DEST(s,v)	((s)->ShipInfo.energy_level = MAKE_BYTE ( \
					(v), HINIBBLE ((s)->ShipInfo.energy_level) \
					))
#define GET_ORBIT_LOC(s)	HINIBBLE ((s)->ShipInfo.energy_level)
#define SET_ORBIT_LOC(s,v)	((s)->ShipInfo.energy_level = MAKE_BYTE ( \
					LONIBBLE ((s)->ShipInfo.energy_level), (v) \
					))
#define GET_GROUP_ID(s)		((s)->ShipInfo.max_energy)
#define SET_GROUP_ID(s,v)	((s)->ShipInfo.max_energy = (v))

#define STATION_RADIUS		1600
#define ORBIT_RADIUS		2400

typedef struct
{
    UWORD		ship_flags;
    BYTE		days_left, growth_fract;
    BYTE		crew_level, max_crew;
    BYTE		energy_level, max_energy;
    POINT		loc;

    STRING		race_strings;
    FRAME		icons, melee_icon;

    COUNT		actual_strength, known_strength;
    POINT		known_loc;

    BYTE		growth_err_term, func_index;
    POINT		dest_loc;
} EXTENDED_SHIP_INFO;
typedef EXTENDED_SHIP_INFO	*PEXTENDED_SHIP_INFO;
typedef EXTENDED_SHIP_INFO	near *NPEXTENDED_SHIP_INFO;
typedef EXTENDED_SHIP_INFO	far *LPEXTENDED_SHIP_INFO;

typedef struct
{
    FRAME		ship[NUM_VIEWS];
    FRAME		weapon[NUM_VIEWS];
    FRAME		special[NUM_VIEWS];
    CAPTAIN_STUFF	captain_control;
    DWORD		victory_ditty;
    SOUND		ship_sounds;
} DATA_STUFF;
typedef DATA_STUFF	*PDATA_STUFF;
typedef DATA_STUFF	near *NPDATA_STUFF;
typedef DATA_STUFF	far *LPDATA_STUFF;

#define DATA_STUFFPTR	LPDATA_STUFF

typedef struct race_desc
{
    SHIP_INFO			ship_info;
    CHARACTERISTIC_STUFF	characteristics;
    DATA_STUFF			ship_data;
    INTEL_STUFF			cyborg_control;

    PROC_PARAMETER(
    void (far *preprocess_func), (ElementPtr),
	ARG_END	(LPELEMENT	ElementPtr)
    );
    PROC_PARAMETER(
    void (far *postprocess_func), (ElementPtr),
	ARG_END	(LPELEMENT	ElementPtr)
    );
    PROC_PARAMETER(
    COUNT (far *init_weapon_func), (ElementPtr, Weapon),
	ARG	(LPELEMENT	ElementPtr)
	ARG_END	(HELEMENT	far Weapon[])
    );

    LPVOID			CodeRef;
} RACE_DESC;
typedef RACE_DESC	*PRACE_DESC;
typedef RACE_DESC	near *NPRACE_DESC;
typedef RACE_DESC	far *LPRACE_DESC;

#define RACE_DESCPTR	LPRACE_DESC

typedef QUEUE_HANDLE	HSTARSHIP;

typedef struct
{
    HSTARSHIP			pred, succ;

    DWORD			RaceResIndex;
    RACE_DESCPTR		RaceDescPtr;
    BYTE			captains_name_index;

    BYTE			weapon_counter,
				special_counter,
				energy_counter;

    BYTE			ship_input_state;
    UWORD			cur_status_flags, old_status_flags;

    HELEMENT			hShip;
    COUNT			ShipFacing;
    FRAME			silhouette;
} STARSHIP;
typedef STARSHIP	*PSTARSHIP;
typedef STARSHIP	near *NPSTARSHIP;
typedef STARSHIP	far *LPSTARSHIP;

#define STARSHIPPTR	PSTARSHIP

typedef struct
{
    HSTARSHIP			pred, succ;

    DWORD			RaceResIndex;
    RACE_DESCPTR		RaceDescPtr;
    SHIP_INFO			ShipInfo;
} SHIP_FRAGMENT;
typedef SHIP_FRAGMENT	*PSHIP_FRAGMENT;
typedef SHIP_FRAGMENT	near *NPSHIP_FRAGMENT;
typedef SHIP_FRAGMENT	far *LPSHIP_FRAGMENT;

#define SHIP_FRAGMENTPTR	PSHIP_FRAGMENT

typedef struct
{
    HSTARSHIP			pred, succ;

    DWORD			RaceResIndex;
    RACE_DESCPTR		RaceDescPtr;
    EXTENDED_SHIP_INFO		ShipInfo;
} EXTENDED_SHIP_FRAGMENT;
typedef EXTENDED_SHIP_FRAGMENT	*PEXTENDED_SHIP_FRAGMENT;
typedef EXTENDED_SHIP_FRAGMENT	near *NPEXTENDED_SHIP_FRAGMENT;
typedef EXTENDED_SHIP_FRAGMENT	far *LPEXTENDED_SHIP_FRAGMENT;

#define EXTENDED_SHIP_FRAGMENTPTR	PEXTENDED_SHIP_FRAGMENT

#define AllocStarShip(pq)	AllocLink (pq)
#define LockStarShip(pq,h)	(STARSHIPPTR)LockLink (pq, h)
#define UnlockStarShip(pq,h)	UnlockLink (pq, h)
#define FreeStarShip(pq,h)	FreeLink (pq, h)

enum
{
    ARILOU_SHIP,
    CHMMR_SHIP,
    HUMAN_SHIP,
    ORZ_SHIP,
    PKUNK_SHIP,
    SHOFIXTI_SHIP,
    SPATHI_SHIP,
    SUPOX_SHIP,
    THRADDASH_SHIP,
    UTWIG_SHIP,
    VUX_SHIP,
    YEHAT_SHIP,
    MELNORME_SHIP,
    DRUUGE_SHIP,
    ILWRATH_SHIP,
    MYCON_SHIP,
    SLYLANDRO_SHIP,
    UMGAH_SHIP,
    URQUAN_SHIP,
    ZOQFOTPIK_SHIP,

    SYREEN_SHIP,
    BLACK_URQUAN_SHIP,
    YEHAT_REBEL_SHIP,
    URQUAN_PROBE_SHIP,
    SAMATRA_SHIP = URQUAN_PROBE_SHIP,

    NUM_AVAILABLE_RACES
};

#define YEHAT_REBEL_CONVERSATION	(~0L)

#define RACE_COMMUNICATION	\
	ARILOU_CONVERSATION,	/* ARILOU_SHIP */ \
	CHMMR_CONVERSATION,	/* CHMMR_SHIP */ \
	0L,	/* HUMAN_SHIP */ \
	ORZ_CONVERSATION,	/* ORZ_SHIP */ \
	PKUNK_CONVERSATION,	/* PKUNK_SHIP */ \
	SHOFIXTI_CONVERSATION,	/* SHOFIXTI_SHIP */ \
	SPATHI_CONVERSATION,	/* SPATHI_SHIP */ \
	SUPOX_CONVERSATION,	/* SUPOX_SHIP */ \
	THRADD_CONVERSATION,	/* THRADDASH_SHIP */ \
	UTWIG_CONVERSATION,	/* UTWIG_SHIP */ \
	VUX_CONVERSATION,	/* VUX_SHIP */ \
	YEHAT_CONVERSATION,	/* YEHAT_SHIP */ \
	MELNORME_CONVERSATION,	/* MELNORME_SHIP */ \
	DRUUGE_CONVERSATION,	/* DRUUGE_SHIP */ \
	ILWRATH_CONVERSATION,	/* ILWRATH_SHIP */ \
	MYCON_CONVERSATION,	/* MYCON_SHIP */ \
	SLYLANDRO_CONVERSATION,	/* SLYLANDRO_SHIP */ \
	UMGAH_CONVERSATION,	/* UMGAH_SHIP */ \
	URQUAN_CONVERSATION,	/* URQUAN_SHIP */ \
	ZOQFOTPIK_CONVERSATION,	/* ZOQFOTPIK_SHIP */ \
	0L,	/* SYREEN_SHIP */ \
	BLACKURQ_CONVERSATION,	/* BLACK_URQUAN_SHIP */ \
	YEHAT_REBEL_CONVERSATION,/* YEHAT_REBEL_SHIP */ \
	0L,			/* URQUAN_PROBE_SHIP */

#define RACE_SHIP_COST	\
	1600,	/* ARILOU_SHIP */ \
	3000,	/* CHMMR_SHIP */ \
	1100,	/* HUMAN_SHIP */ \
	2300,	/* ORZ_SHIP */ \
	2000,	/* PKUNK_SHIP */ \
	500,	/* SHOFIXTI_SHIP */ \
	1800,	/* SPATHI_SHIP */ \
	1600,	/* SUPOX_SHIP */ \
	1000,	/* THRADDASH_SHIP */ \
	2200,	/* UTWIG_SHIP */ \
	1200,	/* VUX_SHIP */ \
	2300,	/* YEHAT_SHIP */ \
	3600,	/* MELNORME_SHIP */ \
	1700,	/* DRUUGE_SHIP */ \
	1000,	/* ILWRATH_SHIP */ \
	2100,	/* MYCON_SHIP */ \
	4400,	/* SLYLANDRO_SHIP */ \
	700,	/* UMGAH_SHIP */ \
	3000,	/* URQUAN_SHIP */ \
	600,	/* ZOQFOTPIK_SHIP */ \
	1300,	/* SYREEN_SHIP */ \
	3000,	/* BLACK_URQUAN_SHIP */ \
	2300,	/* YEHAT_REBEL_SHIP */

#define LOG_TO_IP(s)		((s) << 1)
#define RACE_IP_SPEED	\
	LOG_TO_IP (40),		/* ARILOU_SHIP */ \
	LOG_TO_IP (27),		/* CHMMR_SHIP */ \
	LOG_TO_IP (24),		/* HUMAN_SHIP */ \
	LOG_TO_IP (40),		/* ORZ_SHIP */ \
	LOG_TO_IP (40),		/* PKUNK_SHIP */ \
	LOG_TO_IP (35),		/* SHOFIXTI_SHIP */ \
	LOG_TO_IP (48),		/* SPATHI_SHIP */ \
	LOG_TO_IP (40),		/* SUPOX_SHIP */ \
	LOG_TO_IP (28),		/* THRADDASH_SHIP */ \
	LOG_TO_IP (30),		/* UTWIG_SHIP */ \
	LOG_TO_IP (21),		/* VUX_SHIP */ \
	LOG_TO_IP (30),		/* YEHAT_SHIP */ \
	LOG_TO_IP (40),		/* MELNORME_SHIP */ \
	LOG_TO_IP (20),		/* DRUUGE_SHIP */ \
	LOG_TO_IP (25),		/* ILWRATH_SHIP */ \
	LOG_TO_IP (27),		/* MYCON_SHIP */ \
	LOG_TO_IP (60),		/* SLYLANDRO_SHIP */ \
	LOG_TO_IP (18),		/* UMGAH_SHIP */ \
	LOG_TO_IP (30),		/* URQUAN_SHIP */ \
	LOG_TO_IP (40),		/* ZOQFOTPIK_SHIP */ \
	LOG_TO_IP (36),		/* SYREEN_SHIP */ \
	LOG_TO_IP (30),		/* BLACK_URQUAN_SHIP */ \
	LOG_TO_IP (30),		/* YEHAT_REBEL_SHIP */ \
	LOG_TO_IP (90),		/* URQUAN_PROBE_SHIP */

#define LOG_TO_HYPER(s)		(WORLD_TO_VELOCITY (s) >> 1)
#define RACE_HYPER_SPEED	\
	LOG_TO_HYPER (40),	/* ARILOU_SHIP */ \
	LOG_TO_HYPER (27),	/* CHMMR_SHIP */ \
	LOG_TO_HYPER (24),	/* HUMAN_SHIP */ \
	LOG_TO_HYPER (40),	/* ORZ_SHIP */ \
	LOG_TO_HYPER (40),	/* PKUNK_SHIP */ \
	LOG_TO_HYPER (35),	/* SHOFIXTI_SHIP */ \
	LOG_TO_HYPER (48),	/* SPATHI_SHIP */ \
	LOG_TO_HYPER (40),	/* SUPOX_SHIP */ \
	LOG_TO_HYPER (50),	/* THRADDASH_SHIP */ \
	LOG_TO_HYPER (30),	/* UTWIG_SHIP */ \
	LOG_TO_HYPER (21),	/* VUX_SHIP */ \
	LOG_TO_HYPER (30),	/* YEHAT_SHIP */ \
	LOG_TO_HYPER (40),	/* MELNORME_SHIP */ \
	LOG_TO_HYPER (20),	/* DRUUGE_SHIP */ \
	LOG_TO_HYPER (25),	/* ILWRATH_SHIP */ \
	LOG_TO_HYPER (27),	/* MYCON_SHIP */ \
	LOG_TO_HYPER (60),	/* SLYLANDRO_SHIP */ \
	LOG_TO_HYPER (18),	/* UMGAH_SHIP */ \
	LOG_TO_HYPER (30),	/* URQUAN_SHIP */ \
	LOG_TO_HYPER (40),	/* ZOQFOTPIK_SHIP */ \
	LOG_TO_HYPER (36),	/* SYREEN_SHIP */ \
	LOG_TO_HYPER (30),	/* BLACK_URQUAN_SHIP */ \
	LOG_TO_HYPER (30),	/* YEHAT_REBEL_SHIP */

#define RACE_HYPERSPACE_PERCENT	\
	20,	/* ARILOU_SHIP */ \
	0,	/* CHMMR_SHIP */ \
	0,	/* HUMAN_SHIP */ \
	20,	/* ORZ_SHIP */ \
	40,	/* PKUNK_SHIP */ \
	0,	/* SHOFIXTI_SHIP */ \
	20,	/* SPATHI_SHIP */ \
	40,	/* SUPOX_SHIP */ \
	60,	/* THRADDASH_SHIP */ \
	40,	/* UTWIG_SHIP */ \
	40,	/* VUX_SHIP */ \
	60,	/* YEHAT_SHIP */ \
	0,	/* MELNORME_SHIP */ \
	30,	/* DRUUGE_SHIP */ \
	60,	/* ILWRATH_SHIP */ \
	40,	/* MYCON_SHIP */ \
	2,	/* SLYLANDRO_SHIP */ \
	30,	/* UMGAH_SHIP */ \
	70,	/* URQUAN_SHIP */ \
	0,	/* ZOQFOTPIK_SHIP */ \
	0,	/* SYREEN_SHIP */ \
	70,	/* BLACK_URQUAN_SHIP */ \
	60,	/* YEHAT_REBEL_SHIP */ \
	0,	/* URQUAN_PROBE_SHIP */

#define RACE_INTERPLANETARY_PERCENT	\
	0,	/* ARILOU_SHIP */ \
	0,	/* CHMMR_SHIP */ \
	0,	/* HUMAN_SHIP */ \
	20,	/* ORZ_SHIP */ \
	20,	/* PKUNK_SHIP */ \
	0,	/* SHOFIXTI_SHIP */ \
	10,	/* SPATHI_SHIP */ \
	20,	/* SUPOX_SHIP */ \
	20,	/* THRADDASH_SHIP */ \
	20,	/* UTWIG_SHIP */ \
	20,	/* VUX_SHIP */ \
	40,	/* YEHAT_SHIP */ \
	0,	/* MELNORME_SHIP */ \
	20,	/* DRUUGE_SHIP */ \
	60,	/* ILWRATH_SHIP */ \
	20,	/* MYCON_SHIP */ \
	5,	/* SLYLANDRO_SHIP */ \
	20,	/* UMGAH_SHIP */ \
	40,	/* URQUAN_SHIP */ \
	0,	/* ZOQFOTPIK_SHIP */ \
	0,	/* SYREEN_SHIP */ \
	40,	/* BLACK_URQUAN_SHIP */ \
	40,	/* YEHAT_REBEL_SHIP */ \
	0,	/* URQUAN_PROBE_SHIP */

#define RACE_ENCOUNTER_MAKEUP	\
	MAKE_BYTE (1, 5),	/* ARILOU_SHIP */ \
	0,			/* CHMMR_SHIP */ \
	0,			/* HUMAN_SHIP */ \
	MAKE_BYTE (1, 5),	/* ORZ_SHIP */ \
	MAKE_BYTE (1, 5),	/* PKUNK_SHIP */ \
	0,			/* SHOFIXTI_SHIP */ \
	MAKE_BYTE (1, 5),	/* SPATHI_SHIP */ \
	MAKE_BYTE (1, 5),	/* SUPOX_SHIP */ \
	MAKE_BYTE (1, 5),	/* THRADDASH_SHIP */ \
	MAKE_BYTE (1, 5),	/* UTWIG_SHIP */ \
	MAKE_BYTE (1, 5),	/* VUX_SHIP */ \
	MAKE_BYTE (1, 5),	/* YEHAT_SHIP */ \
	MAKE_BYTE (1, 1),	/* MELNORME_SHIP */ \
	MAKE_BYTE (1, 5),	/* DRUUGE_SHIP */ \
	MAKE_BYTE (1, 5),	/* ILWRATH_SHIP */ \
	MAKE_BYTE (1, 5),	/* MYCON_SHIP */ \
	MAKE_BYTE (1, 1),	/* SLYLANDRO_SHIP */ \
	MAKE_BYTE (1, 5),	/* UMGAH_SHIP */ \
	MAKE_BYTE (1, 5),	/* URQUAN_SHIP */ \
	MAKE_BYTE (1, 5),	/* ZOQFOTPIK_SHIP */ \
	0,			/* SYREEN_SHIP */ \
	MAKE_BYTE (1, 5),	/* BLACK_URQUAN_SHIP */ \
	MAKE_BYTE (1, 5),	/* YEHAT_REBEL_SHIP */

#define RACE_COLORS	\
	BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x10), 0x53),	/* ARILOU_SHIP */ \
	BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x00), 0x00),	/* CHMMR_SHIP */ \
	BUILD_COLOR (MAKE_RGB15 (0x00, 0x1, 0x1f), 0x4D),	/* HUMAN_SHIP */ \
	BUILD_COLOR (MAKE_RGB15 (0xE, 0x00, 0xE), 0x36),	/* ORZ_SHIP */ \
	BUILD_COLOR (MAKE_RGB15 (0x00, 0x6, 0x8), 0x62),	/* PKUNK_SHIP */ \
	BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x00), 0x00),	/* SHOFIXTI_SHIP */ \
	BUILD_COLOR (MAKE_RGB15 (0xC, 0x5, 0x00), 0x76),	/* SPATHI_SHIP */ \
	BUILD_COLOR (MAKE_RGB15 (0xC, 0x5, 0x00), 0x76),	/* SUPOX_SHIP */ \
	BUILD_COLOR (MAKE_RGB15 (0x00, 0x6, 0x8), 0x62),	/* THRADDASH_SHIP */ \
	BUILD_COLOR (MAKE_RGB15 (0x00, 0x6, 0x8), 0x62),	/* UTWIG_SHIP */ \
	BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x10), 0x53),	/* VUX_SHIP */ \
	BUILD_COLOR (MAKE_RGB15 (0xA, 0x00, 0x11), 0x3D),	/* YEHAT_SHIP */ \
	BUILD_COLOR (MAKE_RGB15 (0x00, 0x6, 0x8), 0x62),	/* MELNORME_SHIP */ \
	BUILD_COLOR (MAKE_RGB15 (0xF, 0x00, 0x00), 0x2D),	/* DRUUGE_SHIP */ \
	BUILD_COLOR (MAKE_RGB15 (0xE, 0x00, 0xE), 0x36),	/* ILWRATH_SHIP */ \
	BUILD_COLOR (MAKE_RGB15 (0xE, 0x00, 0xE), 0x36),	/* MYCON_SHIP */ \
	BUILD_COLOR (MAKE_RGB15 (0xC, 0x5, 0x00), 0x76),	/* SLYLANDRO_SHIP */ \
	BUILD_COLOR (MAKE_RGB15 (0xA, 0x00, 0x11), 0x3D),	/* UMGAH_SHIP */ \
	BUILD_COLOR (MAKE_RGB15 (0x00, 0x8, 0x00), 0x6E),	/* URQUAN_SHIP */ \
	BUILD_COLOR (MAKE_RGB15 (0xF, 0x00, 0x00), 0x2D),	/* ZOQFOTPIK_SHIP */ \
	BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x00), 0x00),	/* SYREEN_SHIP */ \
	BUILD_COLOR (MAKE_RGB15 (0x6, 0x6, 0x6), 0x20),		/* BLACK_URQUAN_SHIP */ \
	BUILD_COLOR (MAKE_RGB15 (0x14, 0x7, 0x1F), 0x39),	/* YEHAT_REBEL_SHIP */

#define RADAR_SCAN_WIDTH	(UNIT_SCREEN_WIDTH * NUM_RADAR_SCREENS)
#define RADAR_SCAN_HEIGHT	(UNIT_SCREEN_HEIGHT * NUM_RADAR_SCREENS)

#define ARILOU_SPACE_X		438
#define ARILOU_SPACE_Y		6372

#define QUASI_SPACE_X		5000
#define QUASI_SPACE_Y		5000
#define ARILOU_HOME_X		(QUASI_SPACE_X + ((RADAR_SCAN_WIDTH >> 1) * 3))
#define ARILOU_HOME_Y		(QUASI_SPACE_Y + ((RADAR_SCAN_HEIGHT >> 1) * 3))

#define SOL_X			1752
#define SOL_Y			1450

PROC_GLOBAL(
BOOLEAN InitKernel, (),
    ARG_VOID
);
PROC_GLOBAL(
void UninitKernel, (ships),
    ARG_END	(BOOLEAN	ships)
);

PROC_GLOBAL(
void DrawCaptainsWindow, (StarShipPtr),
    ARG_END	(STARSHIPPTR	StarShipPtr)
);
PROC_GLOBAL(
BOOLEAN GetNextStarShip, (LastStarShipPtr, which_side),
    ARG		(STARSHIPPTR	LastStarShipPtr)
    ARG_END	(COUNT		which_side)
);
PROC_GLOBAL(
HSTARSHIP GetEncounterStarShip, (LastStarShipPtr, which_player),
    ARG		(STARSHIPPTR	LastStarShipPtr)
    ARG_END	(COUNT		which_player)
);
PROC_GLOBAL(
void DrawArmadaPickShip, (draw_salvage_frame, pPickRect),
    ARG	 	(BOOLEAN	draw_salvage_frame)
    ARG_END	(PRECT		pPickRect)
);

PROC_GLOBAL(
BOOLEAN load_animation, (pixarray, big_res, med_res, sml_res),
    ARG		(LPFRAME	pixarray)
    ARG		(DWORD		big_res)
    ARG		(DWORD		med_res)
    ARG_END	(DWORD		sml_res)
);
PROC_GLOBAL(
BOOLEAN free_image, (pixarray),
    ARG_END	(LPFRAME	pixarray)
);
PROC_GLOBAL(
void NotifyOthers, (which_race, target_loc),
    ARG		(COUNT	which_race)
    ARG_END	(BYTE	target_loc)
);

#endif /* _RACES_H */

