#include "reslib.h"
#include "resinst.h"
#include "starcon.h"

#define MAX_CREW		12
#define MAX_ENERGY		16
#define ENERGY_REGENERATION	1
#define WEAPON_ENERGY_COST	1
#define SPECIAL_ENERGY_COST	1
#define ENERGY_WAIT		4
#define MAX_THRUST		40
#define THRUST_INCREMENT	8
#define TURN_WAIT		1
#define THRUST_WAIT		0
#define WEAPON_WAIT		2
#define SPECIAL_WAIT		0

#define SHIP_MASS		4
#define MISSILE_SPEED		DISPLAY_TO_WORLD (30)
#define MISSILE_LIFE		10

static RACE_DESC	supox_desc =
{
    {
	FIRES_FORE,
	16,					/* Super Melee cost */
	333 / SPHERE_RADIUS_INCREMENT,		/* Initial sphere of influence radius */
	MAX_CREW, MAX_CREW,
	MAX_ENERGY, MAX_ENERGY,
	{
	    7468, 9246,
	},
	(STRING)SUPOX_RACE_STRINGS,
	(FRAME)SUPOX_ICON_MASK_PMAP_ANIM,
	(FRAME)SUPOX_MICON_MASK_PMAP_ANIM,
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
	    (FRAME)SUPOX_BIG_MASK_PMAP_ANIM,
	    (FRAME)SUPOX_MED_MASK_PMAP_ANIM,
	    (FRAME)SUPOX_SML_MASK_PMAP_ANIM,
	},
	{
	    (FRAME)GOB_BIG_MASK_PMAP_ANIM,
	    (FRAME)GOB_MED_MASK_PMAP_ANIM,
	    (FRAME)GOB_SML_MASK_PMAP_ANIM,
	},
	{
	    (FRAME)0,
	    (FRAME)0,
	    (FRAME)0,
	},
	{
	    (FRAME)SUPOX_CAPTAIN_MASK_PMAP_ANIM,
	    (FRAME)0,
	    (FRAME)0,
	    (FRAME)0,
	    (FRAME)0,
	},
	(SOUND)SUPOX_VICTORY_SONG,
	(SOUND)SUPOX_SHIP_SOUNDS,
    },
    {
	0,
	(MISSILE_SPEED * MISSILE_LIFE) >> 1,
	NULL_PTR,
    },
    NULL_PTR,
    NULL_PTR,
    NULL_PTR,
    0,
};

static void	 far
supox_intelligence (ShipPtr, ObjectsOfConcern, ConcernCounter)
LPELEMENT	ShipPtr;
LPEVALUATE_DESC	ObjectsOfConcern;
COUNT		ConcernCounter;
{
    PUSH_CONTEXT
    {
	STARSHIPPTR	StarShipPtr;
	LPEVALUATE_DESC	lpEvalDesc;

	GetElementStarShip (ShipPtr, &StarShipPtr);

	lpEvalDesc = &ObjectsOfConcern[ENEMY_SHIP_INDEX];
	if (StarShipPtr->special_counter || lpEvalDesc->ObjectPtr == 0)
	    StarShipPtr->ship_input_state &= ~SPECIAL;
	else
	{
	    BOOLEAN	LinedUp;
	    COUNT	direction_angle;
	    SIZE	delta_x, delta_y;

	    delta_x = lpEvalDesc->ObjectPtr->next.location.x
		    - ShipPtr->next.location.x;
	    delta_y = lpEvalDesc->ObjectPtr->next.location.y
		    - ShipPtr->next.location.y;
	    direction_angle = ARCTAN (delta_x, delta_y);

	    LinedUp = (BOOLEAN)(NORMALIZE_ANGLE (NORMALIZE_ANGLE (direction_angle
		    - FACING_TO_ANGLE (StarShipPtr->ShipFacing))
		    + QUADRANT) <= HALF_CIRCLE);

	    if (!LinedUp
		    || lpEvalDesc->which_turn > 20
		    || NORMALIZE_ANGLE (
		    lpEvalDesc->facing
		    - (FACING_TO_ANGLE (StarShipPtr->ShipFacing)
		    + HALF_CIRCLE) + OCTANT
		    ) > QUADRANT)
		StarShipPtr->ship_input_state &= ~SPECIAL;
	    else if (LinedUp && lpEvalDesc->which_turn <= 12)
		StarShipPtr->ship_input_state |= SPECIAL;

	    if (StarShipPtr->ship_input_state & SPECIAL)
		lpEvalDesc->MoveState = PURSUE;
	}

	ship_intelligence (ShipPtr,
		ObjectsOfConcern, ConcernCounter);

	if (StarShipPtr->ship_input_state & SPECIAL)
	    StarShipPtr->ship_input_state |= THRUST | WEAPON;

	lpEvalDesc = &ObjectsOfConcern[ENEMY_WEAPON_INDEX];
	if (StarShipPtr->special_counter == 0
		&& lpEvalDesc->ObjectPtr
		&& lpEvalDesc->MoveState == AVOID
		&& ShipPtr->turn_wait == 0)
	{
	    StarShipPtr->ship_input_state &= ~THRUST;
	    StarShipPtr->ship_input_state |= SPECIAL;
	    if (!(StarShipPtr->cur_status_flags & (LEFT | RIGHT)))
		StarShipPtr->ship_input_state |= 1 << ((BYTE)random () & 1);
	    else
		StarShipPtr->ship_input_state |=
			StarShipPtr->cur_status_flags & (LEFT | RIGHT);
	}
    }
    POP_CONTEXT
}

static COUNT	far
initialize_horn (ShipPtr, HornArray)
LPELEMENT	ShipPtr;
HELEMENT	far HornArray[];
{
#define MISSILE_HITS		1
#define MISSILE_DAMAGE		1
#define MISSILE_OFFSET		2
#define SUPOX_OFFSET		23
    PUSH_CONTEXT
    {
	STARSHIPPTR	StarShipPtr;
	MISSILE_BLOCK	MissileBlock;

	GetElementStarShip (ShipPtr, &StarShipPtr);
	MissileBlock.cx = ShipPtr->next.location.x;
	MissileBlock.cy = ShipPtr->next.location.y;
	MissileBlock.farray = StarShipPtr->RaceDescPtr->ship_data.weapon;
	MissileBlock.face = MissileBlock.index = StarShipPtr->ShipFacing;
	MissileBlock.sender = (ShipPtr->state_flags & (GOOD_GUY | BAD_GUY))
		| IGNORE_SIMILAR;
	MissileBlock.pixoffs = SUPOX_OFFSET;
	MissileBlock.speed = MISSILE_SPEED;
	MissileBlock.hit_points = MISSILE_HITS;
	MissileBlock.damage = MISSILE_DAMAGE;
	MissileBlock.life = MISSILE_LIFE;
	MissileBlock.preprocess_func = NULL_PTR;
	MissileBlock.blast_offs = MISSILE_OFFSET;
	HornArray[0] = initialize_missile (&MissileBlock);
    }
    POP_CONTEXT
    return (1);
}

static void	far
supox_preprocess (ElementPtr)
LPELEMENT	ElementPtr;
{
    PUSH_CONTEXT
    {
	STARSHIPPTR	StarShipPtr;

	GetElementStarShip (ElementPtr, &StarShipPtr);
	if ((StarShipPtr->cur_status_flags & SPECIAL)
/*
		&& DeltaEnergy (ElementPtr, -SPECIAL_ENERGY_COST)
*/
		)
	{
	    SIZE	add_facing;

	    add_facing = 0;
	    if (StarShipPtr->cur_status_flags & THRUST)
	    {
		if (ElementPtr->thrust_wait == 0)
		    ++ElementPtr->thrust_wait;

		add_facing = ANGLE_TO_FACING (HALF_CIRCLE);
	    }
	    if (StarShipPtr->cur_status_flags & LEFT)
	    {
		if (ElementPtr->turn_wait == 0)
		    ++ElementPtr->turn_wait;

		if (add_facing)
		    add_facing += ANGLE_TO_FACING (OCTANT);
		else
		    add_facing = -ANGLE_TO_FACING (QUADRANT);
	    }
	    else if (StarShipPtr->cur_status_flags & RIGHT)
	    {
		if (ElementPtr->turn_wait == 0)
		    ++ElementPtr->turn_wait;

		if (add_facing)
		    add_facing -= ANGLE_TO_FACING (OCTANT);
		else
		    add_facing = ANGLE_TO_FACING (QUADRANT);
	    }

	    if (add_facing)
	    {
		COUNT	facing;
		UWORD	thrust_status;

		facing = StarShipPtr->ShipFacing;
		StarShipPtr->ShipFacing = NORMALIZE_FACING (
			facing + add_facing
			);
		thrust_status = inertial_thrust (ElementPtr);
		StarShipPtr->cur_status_flags &=
			~(SHIP_AT_MAX_SPEED
			| SHIP_BEYOND_MAX_SPEED
			| SHIP_IN_GRAVITY_WELL);
		StarShipPtr->cur_status_flags |= thrust_status;
		StarShipPtr->ShipFacing = facing;
	    }
	}
    }
    POP_CONTEXT
}

RACE_DESCPTR	far
init_supox ()
{
    RACE_DESCPTR	RaceDescPtr;

    PUSH_CONTEXT
    {
	supox_desc.preprocess_func = supox_preprocess;
	supox_desc.init_weapon_func = initialize_horn;
	supox_desc.cyborg_control.intelligence_func =
		(PROC_PARAMETER(
		void (far *), (ShipPtr, ObjectsOfConcern, ConcernCounter),
		    ARG		(LPVOID		ShipPtr)
		    ARG		(LPVOID		ObjectsOfConcern)
		    ARG_END	(COUNT		ConcernCounter)
		)) supox_intelligence;

	RaceDescPtr = &supox_desc;
    }
    POP_CONTEXT

    return (RaceDescPtr);
}

