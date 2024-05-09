#ifndef _ENCOUNT_H
#define _ENCOUNT_H

typedef QUEUE_HANDLE	HENCOUNTER;

#define MAX_HYPER_SHIPS		7
#define ONE_SHOT_ENCOUNTER	(1 << 7)
#define ENCOUNTER_REFORMING	(1 << 6)

typedef struct
{
    POINT	star_pt;
    BYTE	Type, Index;
    SHIP_INFO	ShipList[MAX_HYPER_SHIPS];
} EXTENDED_STAR_DESC;
typedef EXTENDED_STAR_DESC	*PEXTENDED_STAR_DESC;

typedef struct
{
    HENCOUNTER		pred, succ;

    HELEMENT		hElement;

    SIZE		transition_state;
    POINT		origin;
    COUNT		radius;

    EXTENDED_STAR_DESC	SD;

    long		log_x, log_y;
} ENCOUNTER;
typedef ENCOUNTER	*PENCOUNTER;
typedef ENCOUNTER	near *NPENCOUNTER;
typedef ENCOUNTER	far *LPENCOUNTER;

#define AllocEncounter()	AllocLink (&GLOBAL (encounter_q))
#define PutEncounter(h)		PutQueue (&GLOBAL (encounter_q), h)
#define InsertEncounter(h,i)	InsertQueue (&GLOBAL (encounter_q), h, i)
#define GetHeadEncounter()	GetHeadLink (&GLOBAL (encounter_q))
#define GetTailEncounter()	GetTailLink (&GLOBAL (encounter_q))
#define LockEncounter(h,eptr)	*(eptr) = (ENCOUNTERPTR)LockLink (&GLOBAL (encounter_q), h)
#define UnlockEncounter(h)	UnlockLink (&GLOBAL (encounter_q), h)
#define RemoveEncounter(h)	RemoveQueue (&GLOBAL (encounter_q), h)
#define FreeEncounter(h)	FreeLink (&GLOBAL (encounter_q), h)
#define GetPredEncounter(l)	_GetPredLink (l)
#define GetSuccEncounter(l)	_GetSuccLink (l)

#ifdef QUEUE_TABLE
#define ENCOUNTERPTR	PENCOUNTER
#else /* !QUEUE_TABLE */
#define ENCOUNTERPTR	LPENCOUNTER
#endif /* QUEUE_TABLE */

enum
{
    SOL_DEFINED = 1,
    SHOFIXTI_DEFINED,
    MAIDENS_DEFINED,
    START_COLONY_DEFINED,
    SPATHI_DEFINED,
    ZOQFOT_DEFINED,

    MELNORME0_DEFINED,
    MELNORME1_DEFINED,
    MELNORME2_DEFINED,
    MELNORME3_DEFINED,
    MELNORME4_DEFINED,
    MELNORME5_DEFINED,
    MELNORME6_DEFINED,
    MELNORME7_DEFINED,
    MELNORME8_DEFINED,

    TALKING_PET_DEFINED,
    CHMMR_DEFINED,
    SYREEN_DEFINED,
    BURVIXESE_DEFINED,
    SLYLANDRO_DEFINED,
    DRUUGE_DEFINED,
    BOMB_DEFINED,
    AQUA_HELIX_DEFINED,
    SUN_DEVICE_DEFINED,
    TAALO_PROTECTOR_DEFINED,
    SHIP_VAULT_DEFINED,
    URQUAN_WRECK_DEFINED,
    VUX_BEAST_DEFINED,
    SAMATRA_DEFINED,
    ZOQ_SCOUT_DEFINED,
    MYCON_DEFINED,
    EGG_CASE0_DEFINED,
    EGG_CASE1_DEFINED,
    EGG_CASE2_DEFINED,
    PKUNK_DEFINED,
    UTWIG_DEFINED,
    SUPOX_DEFINED,
    YEHAT_DEFINED,
    VUX_DEFINED,
    ORZ_DEFINED,
    THRADD_DEFINED,
    RAINBOW_DEFINED,
    ILWRATH_DEFINED,
    ANDROSYNTH_DEFINED,
    MYCON_TRAP_DEFINED
};

#define UMGAH_DEFINED	TALKING_PET_DEFINED

#define TEXT_X_OFFS		1
#define TEXT_Y_OFFS		1
#define SIS_TEXT_WIDTH		(SIS_SCREEN_WIDTH - (TEXT_X_OFFS << 1))

extern STAR_DESCPTR	CurStarDescPtr;
extern STAR_DESCPTR	star_array;

#define NUM_SOLAR_SYSTEMS	502

PROC_GLOBAL(
STAR_DESCPTR FindStar, (pLastStar, puniverse, xbounds, ybounds),
    ARG		(STAR_DESCPTR	pLastStar)
    ARG		(PPOINT		puniverse)
    ARG		(SIZE		xbounds)
    ARG_END	(SIZE		ybounds)
);

PROC_GLOBAL(
void GetClusterName, (pSD, buf),
    ARG		(STAR_DESCPTR	pSD)
    ARG_END	(char		buf[])
);

enum
{
    HAIL = 0,
    ATTACK
};

PROC_GLOBAL(
void BuildBattle, (which_player),
    ARG_END	(COUNT	which_player)
);
PROC_GLOBAL(
COUNT InitEncounter, (),
    ARG_VOID
);
PROC_GLOBAL(
COUNT UninitEncounter, (),
    ARG_VOID
);
PROC_GLOBAL(
COUNT InitCommunication, (which_comm),
    ARG_END	(RESOURCE	which_comm)
);
PROC_GLOBAL(
void RaceCommunication, (),
    ARG_VOID
);

PROC_GLOBAL(
void GenerateSOL, (control),
    ARG_END	(BYTE	control)
);
PROC_GLOBAL(
void GenerateShofixti, (control),
    ARG_END	(BYTE	control)
);
PROC_GLOBAL(
void GenerateColony, (control),
    ARG_END	(BYTE	control)
);
PROC_GLOBAL(
void GenerateSpathi, (control),
    ARG_END	(BYTE	control)
);
PROC_GLOBAL(
void GenerateZoqFotPik, (control),
    ARG_END	(BYTE	control)
);
PROC_GLOBAL(
void GenerateMelnorme, (control),
    ARG_END	(BYTE	control)
);
PROC_GLOBAL(
void GenerateTalkingPet, (control),
    ARG_END	(BYTE	control)
);
PROC_GLOBAL(
void GenerateChmmr, (control),
    ARG_END	(BYTE	control)
);
PROC_GLOBAL(
void GenerateSyreen, (control),
    ARG_END	(BYTE	control)
);
PROC_GLOBAL(
void GenerateBurvixes, (control),
    ARG_END	(BYTE	control)
);
PROC_GLOBAL(
void GenerateSlylandro, (control),
    ARG_END	(BYTE	control)
);
PROC_GLOBAL(
void GenerateDruuge, (control),
    ARG_END	(BYTE	control)
);
PROC_GLOBAL(
void GenerateUtwig, (control),
    ARG_END	(BYTE	control)
);
PROC_GLOBAL(
void GenerateThradd, (control),
    ARG_END	(BYTE	control)
);
PROC_GLOBAL(
void GenerateMycon, (control),
    ARG_END	(BYTE	control)
);
PROC_GLOBAL(
void GenerateOrz, (control),
    ARG_END	(BYTE	control)
);
PROC_GLOBAL(
void GenerateShipVault, (control),
    ARG_END	(BYTE	control)
);
PROC_GLOBAL(
void GenerateUrquanWreck, (control),
    ARG_END	(BYTE	control)
);
PROC_GLOBAL(
void GenerateVUX, (control),
    ARG_END	(BYTE	control)
);
PROC_GLOBAL(
void GenerateSamatra, (control),
    ARG_END	(BYTE	control)
);
PROC_GLOBAL(
void GenerateYehat, (control),
    ARG_END	(BYTE	control)
);
PROC_GLOBAL(
void GeneratePkunk, (control),
    ARG_END	(BYTE	control)
);
PROC_GLOBAL(
void GenerateSupox, (control),
    ARG_END	(BYTE	control)
);
PROC_GLOBAL(
void GenerateRainbow, (control),
    ARG_END	(BYTE	control)
);
PROC_GLOBAL(
void GenerateIlwrath, (control),
    ARG_END	(BYTE	control)
);

#endif /* _ENCOUNT_H */

