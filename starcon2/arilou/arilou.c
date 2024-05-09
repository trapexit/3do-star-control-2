#include "reslib.h"
#include "resinst.h"
#include "starcon.h"

#define MAX_CREW		6
#define MAX_ENERGY		20
#define ENERGY_REGENERATION	1
#define WEAPON_ENERGY_COST	2
#define SPECIAL_ENERGY_COST	3
#define ENERGY_WAIT		6
#define MAX_THRUST		/* DISPLAY_TO_WORLD (10) */ 40
#define THRUST_INCREMENT	MAX_THRUST
#define TURN_WAIT		0
#define THRUST_WAIT		0
#define WEAPON_WAIT		1
#define SPECIAL_WAIT		2

#define SHIP_MASS		1
#define ARILOU_OFFSET		9
#define LASER_RANGE		DISPLAY_TO_WORLD (100 + ARILOU_OFFSET)

STATIC RACE_DESC	arilou_desc =
{
    {
	/* FIRES_FORE | */ IMMEDIATE_WEAPON,
	16,					/* Super Melee cost */
	250 / SPHERE_RADIUS_INCREMENT,		/* Initial sphere of influence radius */
	MAX_CREW, MAX_CREW,
	MAX_ENERGY, MAX_ENERGY,
	{
	    438, 6372,
	},
	(STRING)ARILOU_RACE_STRINGS,
	(FRAME)ARILOU_ICON_MASK_PMAP_ANIM,
	(FRAME)ARILOU_MICON_MASK_PMAP_ANIM,
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
	    (FRAME)ARILOU_BIG_MASK_PMAP_ANIM,
	    (FRAME)ARILOU_MED_MASK_PMAP_ANIM,
	    (FRAME)ARILOU_SML_MASK_PMAP_ANIM,
	},
	{
	    (FRAME)0,
	    (FRAME)0,
	    (FRAME)0,
	},
	{
	    (FRAME)WARP_BIG_MASK_PMAP_ANIM,
	    (FRAME)WARP_MED_MASK_PMAP_ANIM,
	    (FRAME)WARP_SML_MASK_PMAP_ANIM,
	},
	{
	    (FRAME)ARILOU_CAPTAIN_MASK_PMAP_ANIM,
	    (FRAME)0,
	    (FRAME)0,
	    (FRAME)0,
	    (FRAME)0,
	},
	(SOUND)ARILOU_VICTORY_SONG,
	(SOUND)ARILOU_SHIP_SOUNDS,
    },
    {
	0,
	LASER_RANGE >> 1,
	NULL_PTR,
    },
    NULL_PTR,
    NULL_PTR,
    NULL_PTR,
    0,
};

PROC(STATIC
COUNT far initialize_autoaim_laser, (ShipPtr, LaserArray),
    ARG		(LPELEMENT	ShipPtr)
    ARG_END	(HELEMENT	far LaserArray[])
)
{
    PUSH_CONTEXT
    {
	COUNT		orig_facing;
	SIZE		delta_facing;
	STARSHIPPTR	StarShipPtr;
	LASER_BLOCK	LaserBlock;

	GetElementStarShip (ShipPtr, &StarShipPtr);
	LaserBlock.face = orig_facing = StarShipPtr->ShipFacing;
	if ((delta_facing = TrackShip (ShipPtr, &LaserBlock.face)) > 0)
	    LaserBlock.face = NORMALIZE_FACING (orig_facing + delta_facing);
	ShipPtr->hTarget = 0;

	LaserBlock.cx = ShipPtr->next.location.x;
	LaserBlock.cy = ShipPtr->next.location.y;
	LaserBlock.ex = COSINE (FACING_TO_ANGLE (LaserBlock.face), LASER_RANGE);
	LaserBlock.ey = SINE (FACING_TO_ANGLE (LaserBlock.face), LASER_RANGE);
	LaserBlock.sender = (ShipPtr->state_flags & (GOOD_GUY | BAD_GUY))
		| IGNORE_SIMILAR;
	LaserBlock.pixoffs = ARILOU_OFFSET;
	LaserBlock.color = BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1F, 0xA), 0x0E);
	LaserArray[0] = initialize_laser (&LaserBlock);
    }
    POP_CONTEXT

    return (1);
}

PROC(STATIC
void far arilou_intelligence, (ShipPtr, ObjectsOfConcern, ConcernCounter),
    ARG		(LPELEMENT		ShipPtr)
    ARG		(LPEVALUATE_DESC	ObjectsOfConcern)
    ARG_END	(COUNT			ConcernCounter)
)
{
    PUSH_CONTEXT
    {
	STARSHIPPTR	StarShipPtr;

	GetElementStarShip (ShipPtr, &StarShipPtr);
	StarShipPtr->ship_input_state |= THRUST;

 	ObjectsOfConcern[ENEMY_SHIP_INDEX].MoveState = ENTICE;
	ship_intelligence (ShipPtr, ObjectsOfConcern, ConcernCounter);

	if (StarShipPtr->special_counter == 0)
	{
	    LPEVALUATE_DESC	lpEvalDesc;

	    StarShipPtr->ship_input_state &= ~SPECIAL;

	    lpEvalDesc = &ObjectsOfConcern[ENEMY_WEAPON_INDEX];
	    if (lpEvalDesc->ObjectPtr && lpEvalDesc->which_turn <= 6)
	    {
		BOOLEAN		IsTrackingWeapon;
		STARSHIPPTR	EnemyStarShipPtr;

		GetElementStarShip (lpEvalDesc->ObjectPtr, &EnemyStarShipPtr);
		if (((EnemyStarShipPtr->RaceDescPtr->ship_info.ship_flags
			& SEEKING_WEAPON) &&
			lpEvalDesc->ObjectPtr->next.image.farray ==
			EnemyStarShipPtr->RaceDescPtr->ship_data.weapon) ||
			((EnemyStarShipPtr->RaceDescPtr->ship_info.ship_flags
			& SEEKING_SPECIAL) &&
			lpEvalDesc->ObjectPtr->next.image.farray ==
			EnemyStarShipPtr->RaceDescPtr->ship_data.special))
		    IsTrackingWeapon = TRUE;
		else
		    IsTrackingWeapon = FALSE;

		if (((lpEvalDesc->ObjectPtr->state_flags & PLAYER_SHIP)	/* means IMMEDIATE WEAPON */
			|| (IsTrackingWeapon && (lpEvalDesc->which_turn == 1
			|| (lpEvalDesc->ObjectPtr->state_flags & CREW_OBJECT)))	/* FIGHTERS!!! */
			|| PlotIntercept (lpEvalDesc->ObjectPtr, ShipPtr, 3, 0))
			&& !(random () & 3))
		{
		    StarShipPtr->ship_input_state &= ~(LEFT | RIGHT | THRUST | WEAPON);
		    StarShipPtr->ship_input_state |= SPECIAL;
		}
	    }
	}
	if (StarShipPtr->RaceDescPtr->ship_info.energy_level <= SPECIAL_ENERGY_COST << 1)
	    StarShipPtr->ship_input_state &= ~WEAPON;
    }
    POP_CONTEXT
}

PROC(STATIC
void far arilou_preprocess, (ElementPtr),
    ARG_END	(LPELEMENT	ElementPtr)
)
{
    PUSH_CONTEXT
    {
	STARSHIPPTR	StarShipPtr;

	GetElementStarShip (ElementPtr, &StarShipPtr);
	if (!(ElementPtr->state_flags & NONSOLID))
	{
	    if (ElementPtr->thrust_wait == 0)
	    {
		ZeroVelocityComponents (&ElementPtr->velocity);
		StarShipPtr->cur_status_flags &= ~SHIP_AT_MAX_SPEED;
	    }

	    if ((StarShipPtr->cur_status_flags & SPECIAL)
		    && StarShipPtr->special_counter == 0
		    && DeltaEnergy (ElementPtr, -SPECIAL_ENERGY_COST))
	    {
#define HYPER_LIFE		5
		ZeroVelocityComponents (&ElementPtr->velocity);
		StarShipPtr->cur_status_flags &=
			~(SHIP_AT_MAX_SPEED | LEFT | RIGHT | THRUST | WEAPON);

		ElementPtr->state_flags |= NONSOLID | FINITE_LIFE | CHANGING;
		ElementPtr->life_span = HYPER_LIFE;

		ElementPtr->next.image.farray =
			StarShipPtr->RaceDescPtr->ship_data.special;
		ElementPtr->next.image.frame =
			StarShipPtr->RaceDescPtr->ship_data.special[0];

		ProcessSound (SetAbsSoundIndex (
				/* HYPERJUMP */
			StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 1));
		StarShipPtr->special_counter =
			StarShipPtr->RaceDescPtr->characteristics.special_wait;
	    }
	}
	else if (ElementPtr->next.image.farray == StarShipPtr->RaceDescPtr->ship_data.special)
	{
	    COUNT	life_span;

	    StarShipPtr->cur_status_flags =
		    (StarShipPtr->cur_status_flags
		    & ~(LEFT | RIGHT | THRUST | WEAPON | SPECIAL))
		    | (StarShipPtr->old_status_flags
		    & (LEFT | RIGHT | THRUST | WEAPON | SPECIAL));
	    ++StarShipPtr->weapon_counter;
	    ++StarShipPtr->special_counter;
	    ++StarShipPtr->energy_counter;
	    ++ElementPtr->turn_wait;
	    ++ElementPtr->thrust_wait;

	    if ((life_span = ElementPtr->life_span) == NORMAL_LIFE)
	    {
		ElementPtr->state_flags &= ~(NONSOLID | FINITE_LIFE);
		ElementPtr->state_flags |= APPEARING;
		ElementPtr->current.image.farray =
			ElementPtr->next.image.farray =
			StarShipPtr->RaceDescPtr->ship_data.ship;
		ElementPtr->current.image.frame =
			ElementPtr->next.image.frame =
			SetAbsFrameIndex (StarShipPtr->RaceDescPtr->ship_data.ship[0],
			StarShipPtr->ShipFacing);
		InitIntersectStartPoint (ElementPtr);
	    }
	    else
	    {
		--life_span;
		if (life_span != 2)
		{
		    if (life_span < 2)
			ElementPtr->next.image.frame =
				DecFrameIndex (ElementPtr->next.image.frame);
		    else
			ElementPtr->next.image.frame =
				IncFrameIndex (ElementPtr->next.image.frame);
		}
		else
		{
		    ElementPtr->next.location.x =
			    WRAP_X (DISPLAY_ALIGN_X (random ()));
		    ElementPtr->next.location.y =
			    WRAP_Y (DISPLAY_ALIGN_Y (random ()));
		}
	    }

	    ElementPtr->state_flags |= CHANGING;
	}
    }
    POP_CONTEXT
}

PROC(
RACE_DESCPTR far init_arilou, (),
    ARG_VOID
)
{
    RACE_DESCPTR	RaceDescPtr;

    PUSH_CONTEXT
    {
	arilou_desc.preprocess_func = arilou_preprocess;
	arilou_desc.init_weapon_func = initialize_autoaim_laser;
	arilou_desc.cyborg_control.intelligence_func = 
		(PROC_PARAMETER(
		void (far *), (ShipPtr, ObjectsOfConcern, ConcernCounter),
		    ARG		(PVOID		ShipPtr)
		    ARG		(PVOID		ObjectsOfConcern)
		    ARG_END	(COUNT		ConcernCounter)
		)) arilou_intelligence;

	RaceDescPtr = &arilou_desc;
    }
    POP_CONTEXT

    return (RaceDescPtr);
}

