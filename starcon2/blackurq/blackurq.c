#include "reslib.h"
#include "resinst.h"
#include "starcon.h"

#define MAX_CREW		MAX_CREW_SIZE
#define MAX_ENERGY		MAX_ENERGY_SIZE
#define ENERGY_REGENERATION	1
#define WEAPON_ENERGY_COST	6
#define SPECIAL_ENERGY_COST	(MAX_ENERGY_SIZE / 2)
#define ENERGY_WAIT		4
#define MAX_THRUST		30
#define THRUST_INCREMENT	6
#define TURN_WAIT		4
#define THRUST_WAIT		6
#define WEAPON_WAIT		6
#define SPECIAL_WAIT		9

#define SHIP_MASS		10
#define MISSILE_SPEED		64
#define MISSILE_LIFE		64	/* actually, it's as long as you
					 * hold the button down.
					 */

static RACE_DESC	black_urquan_desc =
{
    {
	FIRES_FORE,
	30,					/* Super Melee cost */
	2666 / SPHERE_RADIUS_INCREMENT,		/* Initial sphere of influence radius */
	MAX_CREW, MAX_CREW,
	MAX_ENERGY, MAX_ENERGY,
	{
	    6000, 6250,
	},
	(STRING)KOHR_AH_RACE_STRINGS,
	(FRAME)KOHR_AH_ICON_MASK_PMAP_ANIM,
	(FRAME)KOHR_AH_MICON_MASK_PMAP_ANIM,
    },
    {
	MAX_THRUST,
	THRUST_INCREMENT,
	ENERGY_REGENERATION,
	WEAPON_ENERGY_COST,
	SPECIAL_ENERGY_COST,
	ENERGY_WAIT,
	TURN_WAIT,
	THRUST_WAIT,
	WEAPON_WAIT,
	SPECIAL_WAIT,
	SHIP_MASS,
    },
    {
	{
	    (FRAME)KOHR_AH_BIG_MASK_PMAP_ANIM,
	    (FRAME)KOHR_AH_MED_MASK_PMAP_ANIM,
	    (FRAME)KOHR_AH_SML_MASK_PMAP_ANIM,
	},
	{
	    (FRAME)BUZZSAW_BIG_MASK_PMAP_ANIM,
	    (FRAME)BUZZSAW_MED_MASK_PMAP_ANIM,
	    (FRAME)BUZZSAW_SML_MASK_PMAP_ANIM,
	},
	{
	    (FRAME)GAS_BIG_MASK_PMAP_ANIM,
	    (FRAME)GAS_MED_MASK_PMAP_ANIM,
	    (FRAME)GAS_SML_MASK_PMAP_ANIM,
	},
	{
	    (FRAME)KOHR_AH_CAPTAIN_MASK_PMAP_ANIM,
	    (FRAME)0,
	    (FRAME)0,
	    (FRAME)0,
	    (FRAME)0,
	},
	(SOUND)KOHR_AH_VICTORY_SONG,
	(SOUND)KOHR_AH_SHIP_SOUNDS,
    },
    {
	0,
	CLOSE_RANGE_WEAPON,
	NULL_PTR,
    },
    NULL_PTR,
    NULL_PTR,
    NULL_PTR,
    0,
};

#define SAW_RATE	0
#define MAX_SAWS	8

static void	far
spin_preprocess (ElementPtr)
LPELEMENT	ElementPtr;
{
    PUSH_CONTEXT
    {
	ELEMENTPTR	ShipPtr;
	STARSHIPPTR	StarShipPtr;

	GetElementStarShip (ElementPtr, &StarShipPtr);
	LockElement (StarShipPtr->hShip, &ShipPtr);
	if (ShipPtr->crew_level
		&& ++StarShipPtr->RaceDescPtr->characteristics.special_wait > MAX_SAWS)
	{
	    ElementPtr->life_span = 1;
	    ElementPtr->state_flags |= DISAPPEARING;
	}
	else
	{
	    ++ElementPtr->life_span;
	    if (ElementPtr->turn_wait)
		--ElementPtr->turn_wait;
	    else
	    {
#define LAST_SPIN_INDEX	1
		if (GetFrameIndex (
			ElementPtr->current.image.frame
			) < LAST_SPIN_INDEX)
		    ElementPtr->next.image.frame =
			    IncFrameIndex (ElementPtr->current.image.frame);
		else
		    ElementPtr->next.image.frame =
			    SetAbsFrameIndex (ElementPtr->current.image.frame, 0);
		ElementPtr->state_flags |= CHANGING;

		ElementPtr->turn_wait = SAW_RATE;
	    }
	}
	UnlockElement (StarShipPtr->hShip);
    }
    POP_CONTEXT
}

#define TRACK_WAIT	4

static void	far
buzztrack_preprocess (ElementPtr)
LPELEMENT	ElementPtr;
{
    PUSH_CONTEXT
    {
	if (ElementPtr->thrust_wait)
	    --ElementPtr->thrust_wait;
	else
	{
	    COUNT	facing;

	    if (ElementPtr->hTarget == 0
		    && TrackShip (ElementPtr, &facing) < 0)
	    {
		ZeroVelocityComponents (&ElementPtr->velocity);
	    }
	    else
	    {
#define ACTIVATE_RANGE	SPACE_WIDTH
		SIZE		delta_x, delta_y;
		ELEMENTPTR	eptr;

		LockElement (ElementPtr->hTarget, &eptr);
		delta_x = eptr->current.location.x
			- ElementPtr->current.location.x;
		delta_y = eptr->current.location.y
			- ElementPtr->current.location.y;
		UnlockElement (ElementPtr->hTarget);
		delta_x = WRAP_DELTA_X (delta_x);
		delta_y = WRAP_DELTA_Y (delta_y);
		facing = NORMALIZE_FACING (
			ANGLE_TO_FACING (ARCTAN (delta_x, delta_y))
			);

		if (delta_x < 0)
		    delta_x = -delta_x;
		if (delta_y < 0)
		    delta_y = -delta_y;
		delta_x = WORLD_TO_DISPLAY (delta_x);
		delta_y = WORLD_TO_DISPLAY (delta_y);
		if (delta_x >= ACTIVATE_RANGE
			|| delta_y >= ACTIVATE_RANGE
			|| (DWORD)((UWORD)delta_x * delta_x)
			+ (DWORD)((UWORD)delta_y * delta_y) >=
			(DWORD)ACTIVATE_RANGE * ACTIVATE_RANGE)
		{
		    ZeroVelocityComponents (&ElementPtr->velocity);
		}
		else
		{
		    ElementPtr->thrust_wait = TRACK_WAIT;
		    SetVelocityVector (&ElementPtr->velocity,
			    DISPLAY_TO_WORLD (2), facing);
		}
	    }
	}

	spin_preprocess (ElementPtr);
    }
    POP_CONTEXT
}

static void	far
decelerate_preprocess (ElementPtr)
LPELEMENT	ElementPtr;
{
    PUSH_CONTEXT
    {
	SIZE	dx, dy;

	GetCurrentVelocityComponents (&ElementPtr->velocity, &dx, &dy);
	dx /= 2;
	dy /= 2;
	SetVelocityComponents (&ElementPtr->velocity, dx, dy);
	if (dx == 0 && dy == 0)
	{
	    ElementPtr->preprocess_func = buzztrack_preprocess;
	}

	spin_preprocess (ElementPtr);
    }
    POP_CONTEXT
}

static void	far
splinter_preprocess (ElementPtr)
LPELEMENT	ElementPtr;
{
    PUSH_CONTEXT
    {
	ElementPtr->next.image.frame =
		IncFrameIndex (ElementPtr->current.image.frame);
	ElementPtr->state_flags |= CHANGING;
    }
    POP_CONTEXT
}

static void	far
buzzsaw_collision (ElementPtr0, pPt0, ElementPtr1, pPt1)
LPELEMENT	ElementPtr0, ElementPtr1;
PPOINT		pPt0, pPt1;
{
    PUSH_CONTEXT
    {
	weapon_collision (ElementPtr0, pPt0, ElementPtr1, pPt1);

	if (ElementPtr0->state_flags & DISAPPEARING)
	{
	    ElementPtr0->state_flags &= ~DISAPPEARING;
	    ElementPtr0->state_flags |= NONSOLID | CHANGING;
	    ElementPtr0->life_span = 5;
	    ElementPtr0->next.image.frame =
		    SetAbsFrameIndex (ElementPtr0->current.image.frame, 2);

	    ElementPtr0->preprocess_func = splinter_preprocess;
	}
    }
    POP_CONTEXT
}

static void	far
buzzsaw_preprocess (ElementPtr)
LPELEMENT	ElementPtr;
{
    PUSH_CONTEXT
    {
	STARSHIPPTR	StarShipPtr;

	GetElementStarShip (ElementPtr, &StarShipPtr);
	if (!(StarShipPtr->cur_status_flags & WEAPON))
	{
	    ElementPtr->life_span >>= 1;
	    ElementPtr->preprocess_func = decelerate_preprocess;
	}

	spin_preprocess (ElementPtr);
    }
    POP_CONTEXT
}

static void	far
buzzsaw_postprocess (ElementPtr)
LPELEMENT	ElementPtr;
{
    PUSH_CONTEXT
    {
	HELEMENT	hElement;

	ElementPtr->postprocess_func = 0;
	if (hElement = AllocElement ())
	{
	    COUNT	PI;
	    ELEMENTPTR	ListElementPtr;
	    STARSHIPPTR	StarShipPtr;

	    LockElement (hElement, &ListElementPtr);
	    PI = ListElementPtr->PrimIndex;
	    *ListElementPtr = *ElementPtr;
	    ListElementPtr->PrimIndex = PI;
	    (GLOBAL (DisplayArray))[PI] =
		    (GLOBAL (DisplayArray))[ElementPtr->PrimIndex];
	    ListElementPtr->current = ListElementPtr->next;
	    InitIntersectStartPoint (ListElementPtr);
	    InitIntersectEndPoint (ListElementPtr);
	    ListElementPtr->state_flags = (ListElementPtr->state_flags
		    & ~(PRE_PROCESS | CHANGING | APPEARING))
		    | POST_PROCESS;
	    UnlockElement (hElement);

	    GetElementStarShip (ElementPtr, &StarShipPtr);
	    LockElement (StarShipPtr->hShip, &ListElementPtr);
	    InsertElement (hElement, GetSuccElement (ListElementPtr));
	    UnlockElement (StarShipPtr->hShip);

	    ElementPtr->life_span = 0;
	}
    }
    POP_CONTEXT
}

static COUNT	far
initialize_buzzsaw (ShipPtr, SawArray)
LPELEMENT	ShipPtr;
HELEMENT	far SawArray[];
{
    PUSH_CONTEXT
    {
#define MISSILE_HITS	10
#define MISSILE_DAMAGE	4
#define MISSILE_OFFSET	9
#define KOHR_AH_OFFSET	28
	STARSHIPPTR	StarShipPtr;
	MISSILE_BLOCK	MissileBlock;

	GetElementStarShip (ShipPtr, &StarShipPtr);
	MissileBlock.cx = ShipPtr->next.location.x;
	MissileBlock.cy = ShipPtr->next.location.y;
	MissileBlock.farray = StarShipPtr->RaceDescPtr->ship_data.weapon;
	MissileBlock.face = StarShipPtr->ShipFacing;
	MissileBlock.index = 0;
	MissileBlock.sender = (ShipPtr->state_flags & (GOOD_GUY | BAD_GUY))
		| IGNORE_SIMILAR;
	MissileBlock.pixoffs = KOHR_AH_OFFSET;
	MissileBlock.speed = MISSILE_SPEED;
	MissileBlock.hit_points = MISSILE_HITS;
	MissileBlock.damage = MISSILE_DAMAGE;
	MissileBlock.life = MISSILE_LIFE;
	MissileBlock.preprocess_func = buzzsaw_preprocess;
	MissileBlock.blast_offs = MISSILE_OFFSET;
	SawArray[0] = initialize_missile (&MissileBlock);

	if (SawArray[0])
	{
	    ELEMENTPTR	SawPtr;

	    LockElement (SawArray[0], &SawPtr);
	    SawPtr->turn_wait = SAW_RATE;
	    SawPtr->thrust_wait = 0;
	    SawPtr->postprocess_func = buzzsaw_postprocess;
	    SawPtr->collision_func = buzzsaw_collision;
	    UnlockElement (SawArray[0]);
	}
    }
    POP_CONTEXT

    return (1);
}

static void	 far
black_urquan_intelligence (ShipPtr, ObjectsOfConcern, ConcernCounter)
LPELEMENT	ShipPtr;
LPEVALUATE_DESC	ObjectsOfConcern;
COUNT		ConcernCounter;
{
    PUSH_CONTEXT
    {
	LPEVALUATE_DESC	lpEvalDesc;
	STARSHIPPTR	StarShipPtr;

	lpEvalDesc = &ObjectsOfConcern[ENEMY_WEAPON_INDEX];
	if (lpEvalDesc->ObjectPtr
		&& lpEvalDesc->MoveState == ENTICE
		&& (lpEvalDesc->ObjectPtr->state_flags & CREW_OBJECT)
		&& lpEvalDesc->which_turn <= 8)
	    lpEvalDesc->MoveState = PURSUE;

	ship_intelligence (ShipPtr,
		ObjectsOfConcern, ConcernCounter);

	GetElementStarShip (ShipPtr, &StarShipPtr);
	StarShipPtr->ship_input_state &= ~SPECIAL;

	if (StarShipPtr->special_counter == 0
		&& StarShipPtr->RaceDescPtr->ship_info.energy_level >= SPECIAL_ENERGY_COST
		&& lpEvalDesc->ObjectPtr
		&& lpEvalDesc->which_turn <= 8)
	    StarShipPtr->ship_input_state |= SPECIAL;

	lpEvalDesc = &ObjectsOfConcern[ENEMY_SHIP_INDEX];
	if (lpEvalDesc->ObjectPtr)
	{
#define FRAGMENT_LIFE		10
#define FRAGMENT_SPEED		MISSILE_SPEED
#define FRAGMENT_RANGE		(FRAGMENT_LIFE * FRAGMENT_SPEED)
	    HELEMENT	h, hNext;
	    ELEMENTPTR	BuzzSawPtr;

	    h = (StarShipPtr->old_status_flags & WEAPON) ?
		    GetSuccElement (ShipPtr) : (HELEMENT)0;
	    for (; h; h = hNext)
	    {
		LockElement (h, &BuzzSawPtr);
		hNext = GetSuccElement (BuzzSawPtr);
		if (!(BuzzSawPtr->state_flags & NONSOLID)
			&& BuzzSawPtr->next.image.farray == StarShipPtr->RaceDescPtr->ship_data.weapon
			&& BuzzSawPtr->life_span > MISSILE_LIFE * 3 / 4
			&& (BuzzSawPtr->state_flags & (GOOD_GUY | BAD_GUY)) ==
			(ShipPtr->state_flags & (GOOD_GUY | BAD_GUY)))
		{
		    {
			COUNT	which_turn;

			if (!PlotIntercept (BuzzSawPtr,
				lpEvalDesc->ObjectPtr, BuzzSawPtr->life_span,
				FRAGMENT_RANGE / 2))
			    StarShipPtr->ship_input_state &= ~WEAPON;
			else if (StarShipPtr->weapon_counter == 0)
			    StarShipPtr->ship_input_state |= WEAPON;

			UnlockElement (h);
			break;
		    }
		    hNext = 0;
		}
		UnlockElement (h);
	    }

	    if (h == 0)
	    {
		if (StarShipPtr->old_status_flags & WEAPON)
		    StarShipPtr->ship_input_state &= ~WEAPON;
		else if (StarShipPtr->weapon_counter == 0
			&& ship_weapons (ShipPtr, lpEvalDesc->ObjectPtr, FRAGMENT_RANGE / 2))
		    StarShipPtr->ship_input_state |= WEAPON;

		if (StarShipPtr->special_counter == 0
			&& !(StarShipPtr->ship_input_state & WEAPON)
			&& lpEvalDesc->which_turn <= 8
			&& (StarShipPtr->ship_input_state & (LEFT | RIGHT))
			&& StarShipPtr->RaceDescPtr->ship_info.energy_level >=
			SPECIAL_ENERGY_COST)
		    StarShipPtr->ship_input_state |= SPECIAL;
	    }
	}
    }
    POP_CONTEXT
}

#define GAS_RATE		2

static void	far
gas_cloud_preprocess (ElementPtr)
LPELEMENT	ElementPtr;
{
    PUSH_CONTEXT
    {
	if (ElementPtr->turn_wait)
	    --ElementPtr->turn_wait;
	else
	{
	    ElementPtr->next.image.frame =
		    IncFrameIndex (ElementPtr->current.image.frame);
	    ElementPtr->state_flags |= CHANGING;

	    ElementPtr->turn_wait = GAS_RATE;
	}
    }
    POP_CONTEXT
}

#define GAS_DAMAGE	3

static void	far
gas_cloud_collision (ElementPtr0, pPt0, ElementPtr1, pPt1)
LPELEMENT	ElementPtr0, ElementPtr1;
PPOINT		pPt0, pPt1;
{
    PUSH_CONTEXT
    {
	if (ElementPtr1->state_flags & PLAYER_SHIP)
	    ElementPtr0->mass_points = GAS_DAMAGE;
	else
	    ElementPtr0->mass_points = 50;

	weapon_collision (ElementPtr0, pPt0, ElementPtr1, pPt1);
    }
    POP_CONTEXT
}

static void
spawn_gas_cloud (ElementPtr)
LPELEMENT	ElementPtr;
{
#define GAS_SPEED	16
#define GAS_HITS	100
#define GAS_OFFSET	2
#define NUM_GAS_CLOUDS	16
    SIZE		dx, dy;
    STARSHIPPTR		StarShipPtr;
    MISSILE_BLOCK	MissileBlock;

    GetElementStarShip (ElementPtr, &StarShipPtr);
    MissileBlock.cx = ElementPtr->next.location.x;
    MissileBlock.cy = ElementPtr->next.location.y;
    MissileBlock.farray = StarShipPtr->RaceDescPtr->ship_data.special;
    MissileBlock.index = 0;
    MissileBlock.sender =
	    (ElementPtr->state_flags & (GOOD_GUY | BAD_GUY))
	    | IGNORE_SIMILAR;
    MissileBlock.pixoffs = 20;
    MissileBlock.speed = GAS_SPEED;
    MissileBlock.hit_points = GAS_HITS;
    MissileBlock.damage = GAS_DAMAGE;
    MissileBlock.life =
	    GetFrameCount (MissileBlock.farray[0]) * (GAS_RATE + 1) - 1;
    MissileBlock.preprocess_func = gas_cloud_preprocess;
    MissileBlock.blast_offs = GAS_OFFSET;

    GetCurrentVelocityComponents (&ElementPtr->velocity, &dx, &dy);
    for (MissileBlock.face = 0;
	    MissileBlock.face < ANGLE_TO_FACING (FULL_CIRCLE);
	    MissileBlock.face +=
	    (ANGLE_TO_FACING (FULL_CIRCLE) / NUM_GAS_CLOUDS))
    {
	HELEMENT	hGasCloud;

	if (hGasCloud = initialize_missile (&MissileBlock))
	{
	    ELEMENTPTR	GasCloudPtr;

	    LockElement (hGasCloud, &GasCloudPtr);
	    SetElementStarShip (GasCloudPtr, StarShipPtr);
	    GasCloudPtr->hTarget = 0;
	    GasCloudPtr->turn_wait = GAS_RATE - 1;
	    GasCloudPtr->collision_func = gas_cloud_collision;
	    DeltaVelocityComponents (&GasCloudPtr->velocity, dx, dy);
	    UnlockElement (hGasCloud);
	    PutElement (hGasCloud);
	}
    }
}

static void	far
black_urquan_postprocess (ElementPtr)
LPELEMENT	ElementPtr;
{
    PUSH_CONTEXT
    {
	STARSHIPPTR	StarShipPtr;

	GetElementStarShip (ElementPtr, &StarShipPtr);
	if ((StarShipPtr->cur_status_flags & SPECIAL)
		&& StarShipPtr->special_counter == 0
		&& DeltaEnergy (ElementPtr, -SPECIAL_ENERGY_COST))
	{
	    spawn_gas_cloud (ElementPtr);

	    ProcessSound (SetAbsSoundIndex (
		    StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 1));

	    StarShipPtr->special_counter = SPECIAL_WAIT;
	}
    }
    POP_CONTEXT
}

static void	far
black_urquan_preprocess (ElementPtr)
LPELEMENT	ElementPtr;
{
    PUSH_CONTEXT
    {
	STARSHIPPTR	StarShipPtr;

	GetElementStarShip (ElementPtr, &StarShipPtr);
		/* no spinning disks */
	StarShipPtr->RaceDescPtr->characteristics.special_wait = 0;
	if (StarShipPtr->weapon_counter == 0
		&& (StarShipPtr->cur_status_flags
		& StarShipPtr->old_status_flags & WEAPON))
	    ++StarShipPtr->weapon_counter;
    }
    POP_CONTEXT
}

RACE_DESCPTR	far
init_black_urquan ()
{
    RACE_DESCPTR	RaceDescPtr;

    PUSH_CONTEXT
    {
	black_urquan_desc.preprocess_func = black_urquan_preprocess;
	black_urquan_desc.postprocess_func = black_urquan_postprocess;
	black_urquan_desc.init_weapon_func = initialize_buzzsaw;
	black_urquan_desc.cyborg_control.intelligence_func =
		(PROC_PARAMETER(
		void (far *), (ShipPtr, ObjectsOfConcern, ConcernCounter),
		    ARG		(LPVOID		ShipPtr)
		    ARG		(LPVOID		ObjectsOfConcern)
		    ARG_END	(COUNT		ConcernCounter)
		)) black_urquan_intelligence;

	RaceDescPtr = &black_urquan_desc;
    }
    POP_CONTEXT

    return (RaceDescPtr);
}

