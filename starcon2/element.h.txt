#ifndef _ELEMENT_H
#define _ELEMENT_H

#define BATTLE_FRAME_RATE	5

#define SHIP_INFO_HEIGHT	65
#define CAPTAIN_XOFFS		4
#define CAPTAIN_YOFFS		(SHIP_INFO_HEIGHT + 4)
#define SHIP_STATUS_HEIGHT	(STATUS_HEIGHT >> 1)
#define BAD_GUY_YOFFS		0
#define GOOD_GUY_YOFFS		SHIP_STATUS_HEIGHT
#define STARCON_TEXT_HEIGHT	7
#define TINY_TEXT_HEIGHT	9

#define NORMAL_LIFE		1

typedef QUEUE_HANDLE	HELEMENT;

#define GOOD_GUY		(1 << 0)
#define BAD_GUY			(1 << 1)
#define PLAYER_SHIP		(1 << 2)

#define APPEARING		(1 << 3)
#define DISAPPEARING		(1 << 4)
#define CHANGING		(1 << 5)

#define NONSOLID		(1 << 6)
#define COLLISION		(1 << 7)
#define IGNORE_SIMILAR		(1 << 8)
#define DEFY_PHYSICS		(1 << 9)

#define FINITE_LIFE		(1 << 10)

#define PRE_PROCESS		(1 << 11)
#define POST_PROCESS		(1 << 12)

#define IGNORE_VELOCITY		(1 << 13)
#define CREW_OBJECT		(1 << 14)
#define BACKGROUND_OBJECT	(1 << 15)

#define HYPERJUMP_LIFE		15

#define NUM_EXPLOSION_FRAMES	12

#define GAME_SOUND_PRIORITY	2

typedef enum
{
    VIEW_STABLE,
    VIEW_SCROLL,
    VIEW_CHANGE
} VIEW_STATE;

typedef UWORD	ELEMENT_FLAGS;

#define NO_PRIM	NUM_PRIMS

typedef struct state
{
    POINT	location;
    struct
    {
	FRAME	frame;
	LPFRAME	farray;
    } image;
} STATE;
typedef STATE	*PSTATE;
typedef STATE	near *NPSTATE;
typedef STATE	far *LPSTATE;

typedef struct element
{
    HELEMENT		pred, succ;

    PROC_PARAMETER(
    void (far *preprocess_func), (ElementPtr),
	ARG_END	(struct element far	*ElementPtr)
    );
    PROC_PARAMETER(
    void (far *postprocess_func), (ElementPtr),
	ARG_END	(struct element far	*ElementPtr)
    );
    PROC_PARAMETER(
    void (far *collision_func), (ElementPtr0, pPt0, ElementPtr1, pPt1),
	ARG	(struct element far	*ElementPtr0)
	ARG	(PPOINT			pPt0)
	ARG	(struct element far	*ElementPtr1)
	ARG_END	(PPOINT			pPt1)
    );
    PROC_PARAMETER(
    void (far *death_func), (ElementPtr),
	ARG_END	(struct element far	*ElementPtr)
    );

    ELEMENT_FLAGS	state_flags;
    COUNT		life_span;
    BYTE		crew_level, mass_points;
    BYTE		turn_wait, thrust_wait;
    VELOCITY_DESC	velocity;
    INTERSECT_CONTROL	IntersectControl;
    COUNT		PrimIndex;
    STATE		current, next;

    PVOID		pParent;
    HELEMENT		hTarget;
} ELEMENT;
typedef ELEMENT	*PELEMENT;
typedef ELEMENT	near *NPELEMENT;
typedef ELEMENT	far *LPELEMENT;

#define MAX_DISPLAY_PRIMS	280
extern COUNT			DisplayFreeList;
extern PRIMITIVE		DisplayArray[MAX_DISPLAY_PRIMS];

#define AllocDisplayPrim()	DisplayFreeList; \
				DisplayFreeList = GetSuccLink (GetPrimLinks (&DisplayArray[DisplayFreeList]))
#define FreeDisplayPrim(p)	SetPrimLinks (&DisplayArray[p], END_OF_LIST, DisplayFreeList); \
				DisplayFreeList = (p)

#ifdef QUEUE_TABLE
#define STATEPTR	PSTATE
#define ELEMENTPTR	PELEMENT
#else /* !QUEUE_TABLE */
#define STATEPTR	LPSTATE
#define ELEMENTPTR	LPELEMENT
#endif /* QUEUE_TABLE */

#define GetElementStarShip(e,psd)	*(psd) = (PVOID)(e)->pParent
#define SetElementStarShip(e,psd)	((e)->pParent = (PVOID)(psd))

#define blast_offset		thrust_wait
#define hit_points		crew_level
#define next_turn		thrust_wait
#define MAX_CREW_SIZE		42
#define MAX_ENERGY_SIZE		42
#define MAX_SHIP_MASS		10
#define GRAVITY_MASS(m)		((m) > MAX_SHIP_MASS * 10)
#define GRAVITY_THRESHOLD	(COUNT)255

#define WHICH_SIDE(f)		(((f) & BAD_GUY) >> 1)
#define OBJECT_CLOAKED(eptr)	\
	(GetPrimType (&GLOBAL (DisplayArray[(eptr)->PrimIndex])) >= NUM_PRIMS \
	|| (GetPrimType (&GLOBAL (DisplayArray[(eptr)->PrimIndex])) == STAMPFILL_PRIM \
	&& GetPrimColor (&GLOBAL (DisplayArray[(eptr)->PrimIndex])) == BLACK_COLOR))
#define UNDEFINED_LEVEL	0

PROC_GLOBAL(
HELEMENT AllocElement, (),
    ARG_VOID
);
PROC_GLOBAL(
void FreeElement, (hElement),
    ARG_END	(HELEMENT	hElement)
);
#define PutElement(h)		PutQueue (&disp_q, h)
#define InsertElement(h,i)	InsertQueue (&disp_q, h, i)
#define GetHeadElement()	GetHeadLink (&disp_q)
#define GetTailElement()	GetTailLink (&disp_q)
#define LockElement(h,eptr)	*(eptr) = (ELEMENTPTR)LockLink (&disp_q, h)
#define UnlockElement(h)	UnlockLink (&disp_q, h)
#define RemoveElement(h)	RemoveQueue (&disp_q, h)
#define GetPredElement(l)	_GetPredLink (l)
#define GetSuccElement(l)	_GetSuccLink (l)

PROC_GLOBAL(
void RedrawQueue, (clear),
    ARG_END	(BOOLEAN	clear)
);
PROC_GLOBAL(
BOOLEAN	DeltaEnergy, (ElementPtr, energy_delta),
    ARG		(ELEMENTPTR	ElementPtr)
    ARG_END	(SIZE		energy_delta)
);
PROC_GLOBAL(
BOOLEAN	DeltaCrew, (ElementPtr, crew_delta),
    ARG		(ELEMENTPTR	ElementPtr)
    ARG_END	(SIZE		crew_delta)
);

PROC_GLOBAL(
void PlaySound, (S, Priority),
    ARG		(SOUND	S)
    ARG_END	(BYTE	Priority)
);

PROC_GLOBAL(
void ProcessSound, (Sound),
    ARG_END	(SOUND	Sound)
);

PROC_GLOBAL(
void PreProcessStatus, (ShipPtr),
    ARG_END	(LPELEMENT	ShipPtr)
);
PROC_GLOBAL(
void PostProcessStatus, (ShipPtr),
    ARG_END	(LPELEMENT	ShipPtr)
);

PROC_GLOBAL(
void load_gravity_well, (selector),
    ARG_END	(BYTE	selector)
);
PROC_GLOBAL(
void free_gravity_well, (),
    ARG_VOID
);
PROC_GLOBAL(
void spawn_planet, (),
    ARG_VOID
);
PROC_GLOBAL(
void spawn_asteroid, (ElementPtr),
    ARG_END	(LPELEMENT	ElementPtr)
);
PROC_GLOBAL(
void animation_preprocess, (ElementPtr),
    ARG_END	(LPELEMENT	ElementPtr)
);
PROC_GLOBAL(
void do_damage, (ElementPtr, damage),
    ARG		(ELEMENTPTR	ElementPtr)
    ARG_END	(SIZE		damage)
);
PROC_GLOBAL(
void collision, (ElementPtr0, pPt0, ElementPtr1, pPt1),
    ARG		(LPELEMENT	ElementPtr0)
    ARG		(PPOINT		pPt0)
    ARG		(LPELEMENT	ElementPtr1)
    ARG_END	(PPOINT		pPt1)
);
PROC_GLOBAL(
void crew_preprocess, (ElementPtr),
    ARG_END	(LPELEMENT	ElementPtr)
);
PROC_GLOBAL(
void crew_collision, (ElementPtr0, pPt0, ElementPtr1, pPt1),
    ARG		(LPELEMENT	ElementPtr0)
    ARG		(PPOINT		pPt0)
    ARG		(LPELEMENT	ElementPtr1)
    ARG_END	(PPOINT		pPt1)
);
PROC_GLOBAL(
void AbandonShip, (ShipPtr, TargetPtr, crew_loss),
    ARG		(ELEMENTPTR	ShipPtr)
    ARG		(ELEMENTPTR	TargetPtr)
    ARG_END	(COUNT		crew_loss)
);
PROC_GLOBAL(
BOOLEAN TimeSpaceMatterConflict, (ElementPtr),
    ARG_END	(ELEMENTPTR	ElementPtr)
);
PROC_GLOBAL(
COUNT PlotIntercept, (ElementPtr0, ElementPtr1, max_turns, margin_of_error),
    ARG		(ELEMENTPTR	ElementPtr0)
    ARG		(ELEMENTPTR	ElementPtr1)
    ARG		(COUNT		max_turns)
    ARG_END	(COUNT		margin_of_error)
);
PROC_GLOBAL(
BOOLEAN LoadKernel, (argc, argv),
    ARG		(int	argc)
    ARG_END	(char	*argv[])
);
PROC_GLOBAL(
void FreeKernel, (),
    ARG_VOID
);

PROC_GLOBAL(
void InitDisplayList, (),
    ARG_VOID
);

PROC_GLOBAL(
void InitGalaxy, (),
    ARG_VOID
);
PROC_GLOBAL(
void MoveGalaxy, (view_state, dx, dy),
    ARG		(VIEW_STATE	view_state)
    ARG		(register SIZE	dx)
    ARG_END	(register SIZE	dy)
);
PROC_GLOBAL(
void ship_preprocess, (ElementPtr),
    ARG_END	(LPELEMENT	ElementPtr)
);
PROC_GLOBAL(
void ship_postprocess, (ElementPtr),
    ARG_END	(register LPELEMENT	ElementPtr)
);
PROC_GLOBAL(
void ship_death, (ShipPtr),
    ARG_END	(LPELEMENT	ShipPtr)
);
PROC_GLOBAL(
BOOLEAN hyper_transition, (ElementPtr),
    ARG_END	(LPELEMENT	ElementPtr)
);

PROC_GLOBAL(
BOOLEAN CalculateGravity, (ElementPtr),
    ARG_END	(LPELEMENT	ElementPtr)
);
PROC_GLOBAL(
UWORD inertial_thrust, (ElementPtr),
    ARG_END	(ELEMENTPTR	ElementPtr)
);
PROC_GLOBAL(
void SetUpElement, (ElementPtr),
    ARG_END	(register ELEMENTPTR	ElementPtr)
);

PROC_GLOBAL(
void BattleSong, (DoPlay),
    ARG_END	(BOOLEAN	DoPlay)
);
PROC_GLOBAL(
void FreeBattleSong, (),
    ARG_VOID
);

#endif /* _ELEMENT_H */

