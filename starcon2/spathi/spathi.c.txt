#include "reslib.h"
#include "resinst.h"
#include "starcon.h"

#define MAX_CREW		30
#define MAX_ENERGY		10
#define ENERGY_REGENERATION	1
#define WEAPON_ENERGY_COST	2
#define SPECIAL_ENERGY_COST	3
#define ENERGY_WAIT		10
#define MAX_THRUST		48
#define THRUST_INCREMENT	12
#define TURN_WAIT		1
#define THRUST_WAIT		1
#define WEAPON_WAIT		0
#define SPECIAL_WAIT		7

#define SHIP_MASS		5
#define MISSILE_SPEED		DISPLAY_TO_WORLD (30)
#define MISSILE_LIFE		10
#define MISSILE_RANGE		(MISSILE_SPEED * MISSILE_LIFE)

static RACE_DESC	spathi_desc =
{
    {
	FIRES_FORE | FIRES_AFT | SEEKING_SPECIAL | DONT_CHASE,
	18,					/* Super Melee cost */
	1000 / SPHERE_RADIUS_INCREMENT,		/* Initial sphere of influence radius */
	MAX_CREW, MAX_CREW,
	MAX_ENERGY, MAX_ENERGY,
	{
	    2549, 3600,
	},
	(STRING)SPATHI_RACE_STRINGS,
	(FRAME)SPATHI_ICON_MASK_PMAP_ANIM,
	(FRAME)SPATHI_MICON_MASK_PMAP_ANIM,
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
	    (FRAME)SPATHI_BIG_MASK_PMAP_ANIM,
	    (FRAME)SPATHI_MED_MASK_PMAP_ANIM,
	    (FRAME)SPATHI_SML_MASK_PMAP_ANIM,
	},
	{
	    (FRAME)MISSILE_BIG_MASK_PMAP_ANIM,
	    (FRAME)MISSILE_MED_MASK_PMAP_ANIM,
	    (FRAME)MISSILE_SML_MASK_PMAP_ANIM,
	},
	{
	    (FRAME)DISCRIM_BIG_MASK_PMAP_ANIM,
	    (FRAME)DISCRIM_MED_MASK_PMAP_ANIM,
	    (FRAME)DISCRIM_SML_MASK_PMAP_ANIM,
	},
	{
	    (FRAME)SPATHI_CAPTAIN_MASK_PMAP_ANIM,
	    (FRAME)0,
	    (FRAME)0,
	    (FRAME)0,
	    (FRAME)0,
	},
	(SOUND)SPATHI_VICTORY_SONG,
	(SOUND)SPATHI_SHIP_SOUNDS,
    },
    {
	0,
	MISSILE_RANGE,
	NULL_PTR,
    },
    NULL_PTR,
    NULL_PTR,
    NULL_PTR,
    0,
};

#define DISCRIMINATOR_SPEED	DISPLAY_TO_WORLD (8)
#define TRACK_WAIT		1

static void	far
butt_missile_preprocess (ElementPtr)
LPELEMENT	ElementPtr;
{
    PUSH_CONTEXT
    {
	if (ElementPtr->turn_wait > 0)
	    --ElementPtr->turn_wait;
	else
	{
	    COUNT	facing;

	    facing = GetFrameIndex (ElementPtr->next.image.frame);
	    if (TrackShip (ElementPtr, &facing) > 0)
	    {
		ElementPtr->next.image.frame =
			SetAbsFrameIndex (ElementPtr->next.image.frame,
			facing);
		ElementPtr->state_flags |= CHANGING;
	
		SetVelocityVector (&ElementPtr->velocity,
			DISCRIMINATOR_SPEED, facing);
	    }

	    ElementPtr->turn_wait = TRACK_WAIT;
	}
    }
    POP_CONTEXT
}

static void
spawn_butt_missile (ShipPtr)
LPELEMENT	ShipPtr;
{
#define SPATHI_REAR_OFFSET		20
#define DISCRIMINATOR_LIFE		30
#define DISCRIMINATOR_HITS		1
#define DISCRIMINATOR_DAMAGE		2
#define DISCRIMINATOR_OFFSET		4
    HELEMENT		ButtMissile;
    STARSHIPPTR		StarShipPtr;
    MISSILE_BLOCK	ButtMissileBlock;

    GetElementStarShip (ShipPtr, &StarShipPtr);
    ButtMissileBlock.cx = ShipPtr->next.location.x;
    ButtMissileBlock.cy = ShipPtr->next.location.y;
    ButtMissileBlock.farray = StarShipPtr->RaceDescPtr->ship_data.special;
    ButtMissileBlock.face = ButtMissileBlock.index =
	    NORMALIZE_FACING (StarShipPtr->ShipFacing
	    + ANGLE_TO_FACING (HALF_CIRCLE));
    ButtMissileBlock.sender = ShipPtr->state_flags & (GOOD_GUY | BAD_GUY);
    ButtMissileBlock.pixoffs = SPATHI_REAR_OFFSET;
    ButtMissileBlock.speed = DISCRIMINATOR_SPEED;
    ButtMissileBlock.hit_points = DISCRIMINATOR_HITS;
    ButtMissileBlock.damage = DISCRIMINATOR_DAMAGE;
    ButtMissileBlock.life = DISCRIMINATOR_LIFE;
    ButtMissileBlock.preprocess_func = butt_missile_preprocess;
    ButtMissileBlock.blast_offs = DISCRIMINATOR_OFFSET;
    if (ButtMissile = initialize_missile (&ButtMissileBlock))
    {
	ELEMENTPTR	ButtPtr;

	LockElement (ButtMissile, &ButtPtr);
	ButtPtr->turn_wait = TRACK_WAIT;
	SetElementStarShip (ButtPtr, StarShipPtr);
	UnlockElement (ButtMissile);
	PutElement (ButtMissile);
    }
}

static void	 far
spathi_intelligence (ShipPtr, ObjectsOfConcern, ConcernCounter)
LPELEMENT	ShipPtr;
LPEVALUATE_DESC	ObjectsOfConcern;
COUNT		ConcernCounter;
{
    PUSH_CONTEXT
    {
	STARSHIPPTR	StarShipPtr;
	LPEVALUATE_DESC	lpEvalDesc;

	ship_intelligence (ShipPtr, ObjectsOfConcern, ConcernCounter);

	GetElementStarShip (ShipPtr, &StarShipPtr);
	StarShipPtr->ship_input_state &= ~SPECIAL;

	lpEvalDesc = &ObjectsOfConcern[ENEMY_SHIP_INDEX];
	if (StarShipPtr->special_counter == 0
		&& lpEvalDesc->ObjectPtr
		&& lpEvalDesc->which_turn <= 24)
	{
	    COUNT	travel_facing, direction_facing;
	    SIZE	delta_x, delta_y;

	    travel_facing = NORMALIZE_FACING (
		    ANGLE_TO_FACING (GetVelocityTravelAngle (&ShipPtr->velocity)
		    + HALF_CIRCLE)
		    );
	    delta_x = lpEvalDesc->ObjectPtr->current.location.x
		    - ShipPtr->current.location.x;
	    delta_y = lpEvalDesc->ObjectPtr->current.location.y
		    - ShipPtr->current.location.y;
	    direction_facing = NORMALIZE_FACING (
		    ANGLE_TO_FACING (ARCTAN (delta_x, delta_y))
		    );

	    if (NORMALIZE_FACING (direction_facing
		    - (StarShipPtr->ShipFacing + ANGLE_TO_FACING (HALF_CIRCLE))
		    + ANGLE_TO_FACING (QUADRANT))
		    <= ANGLE_TO_FACING (HALF_CIRCLE)
		    && (lpEvalDesc->which_turn <= 8
		    || NORMALIZE_FACING (direction_facing
		    + ANGLE_TO_FACING (HALF_CIRCLE)
		    - ANGLE_TO_FACING (GetVelocityTravelAngle (
			    &lpEvalDesc->ObjectPtr->velocity
			    ))
		    + ANGLE_TO_FACING (QUADRANT))
		    <= ANGLE_TO_FACING (HALF_CIRCLE))
		    && (!(StarShipPtr->cur_status_flags &
		    (SHIP_BEYOND_MAX_SPEED | SHIP_IN_GRAVITY_WELL))
		    || NORMALIZE_FACING (direction_facing
		    - travel_facing + ANGLE_TO_FACING (QUADRANT))
		    <= ANGLE_TO_FACING (HALF_CIRCLE)))
		StarShipPtr->ship_input_state |= SPECIAL;
	}
    }
    POP_CONTEXT
}

static COUNT	far
initialize_standard_missile (ShipPtr, MissileArray)
LPELEMENT	ShipPtr;
HELEMENT	far MissileArray[];
{
#define SPATHI_FORWARD_OFFSET		16
#define MISSILE_HITS			1
#define MISSILE_DAMAGE			1
#define MISSILE_OFFSET			1
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
	MissileBlock.pixoffs = SPATHI_FORWARD_OFFSET;
	MissileBlock.speed = MISSILE_SPEED;
	MissileBlock.hit_points = MISSILE_HITS;
	MissileBlock.damage = MISSILE_DAMAGE;
	MissileBlock.life = MISSILE_LIFE;
	MissileBlock.preprocess_func = NULL_PTR;
	MissileBlock.blast_offs = MISSILE_OFFSET;
	MissileArray[0] = initialize_missile (&MissileBlock);
    }
    POP_CONTEXT

    return (1);
}

static void	far
spathi_postprocess (ElementPtr)
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
	    ProcessSound (SetAbsSoundIndex (
			/* LAUNCH_BUTT_MISSILE */
		    StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 1));
	    spawn_butt_missile (ElementPtr);

	    StarShipPtr->special_counter =
		    StarShipPtr->RaceDescPtr->characteristics.special_wait;
	}
    }
    POP_CONTEXT
}

RACE_DESCPTR	far
init_spathi ()
{
    RACE_DESCPTR	RaceDescPtr;

    PUSH_CONTEXT
    {
	spathi_desc.postprocess_func = spathi_postprocess;
	spathi_desc.init_weapon_func = initialize_standard_missile;
	spathi_desc.cyborg_control.intelligence_func =
		(PROC_PARAMETER(
		void (far *), (ShipPtr, ObjectsOfConcern, ConcernCounter),
		    ARG		(LPVOID		ShipPtr)
		    ARG		(LPVOID		ObjectsOfConcern)
		    ARG_END	(COUNT		ConcernCounter)
		)) spathi_intelligence;

	RaceDescPtr = &spathi_desc;
    }
    POP_CONTEXT

    return (RaceDescPtr);
}

