#include "reslib.h"
#include "resinst.h"
#include "starcon.h"

#define MAX_CREW		20
#define MAX_ENERGY		40
#define ENERGY_REGENERATION	1
#define WEAPON_ENERGY_COST	1
#define SPECIAL_ENERGY_COST	2
#define ENERGY_WAIT		8
#define MAX_THRUST		/* DISPLAY_TO_WORLD (5) */ 21
#define THRUST_INCREMENT	/* DISPLAY_TO_WORLD (2) */ 7
#define TURN_WAIT		6
#define THRUST_WAIT		4
#define WEAPON_WAIT		0
#define SPECIAL_WAIT		7

#define SHIP_MASS		6
#define VUX_OFFSET		12
#define LASER_RANGE		DISPLAY_TO_WORLD (150 + VUX_OFFSET)

STATIC RACE_DESC	vux_desc =
{
    {
	FIRES_FORE | SEEKING_SPECIAL | IMMEDIATE_WEAPON,
	12,					/* Super Melee cost */
	900 / SPHERE_RADIUS_INCREMENT,		/* Initial sphere of influence radius */
	MAX_CREW, MAX_CREW,
	MAX_ENERGY, MAX_ENERGY,
	{
	    4412, 1558,
	},
	(STRING)VUX_RACE_STRINGS,
	(FRAME)VUX_ICON_MASK_PMAP_ANIM,
	(FRAME)VUX_MICON_MASK_PMAP_ANIM,
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
	    (FRAME)VUX_BIG_MASK_PMAP_ANIM,
	    (FRAME)VUX_MED_MASK_PMAP_ANIM,
	    (FRAME)VUX_SML_MASK_PMAP_ANIM,
	},
	{
	    (FRAME)SLIME_MASK_PMAP_ANIM,
	    (FRAME)0,
	    (FRAME)0,
	},
	{
	    (FRAME)LIMPETS_BIG_MASK_PMAP_ANIM,
	    (FRAME)LIMPETS_MED_MASK_PMAP_ANIM,
	    (FRAME)LIMPETS_SML_MASK_PMAP_ANIM,
	},
	{
	    (FRAME)VUX_CAPTAIN_MASK_PMAP_ANIM,
	    (FRAME)0,
	    (FRAME)0,
	    (FRAME)0,
	    (FRAME)0,
	},
	(SOUND)VUX_VICTORY_SONG,
	(SOUND)VUX_SHIP_SOUNDS,
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

#define LIMPET_SPEED		25

PROC(STATIC
void far limpet_preprocess, (ElementPtr),
    ARG_END	(LPELEMENT	ElementPtr)
)
{
    PUSH_CONTEXT
    {
	COUNT	facing, orig_facing;
	SIZE	delta_facing;

	facing = orig_facing = NORMALIZE_FACING (ANGLE_TO_FACING (
		GetVelocityTravelAngle (&ElementPtr->velocity)
		));
	if ((delta_facing = TrackShip (ElementPtr, &facing)) > 0)
	{
	    facing = orig_facing + delta_facing;
	    SetVelocityVector (&ElementPtr->velocity,
		    LIMPET_SPEED, facing);
	}
	ElementPtr->next.image.frame =
		 IncFrameIndex (ElementPtr->next.image.frame);

	ElementPtr->state_flags |= CHANGING;
    }
    POP_CONTEXT
}

PROC(STATIC
void far limpet_collision, (ElementPtr0, pPt0, ElementPtr1, pPt1),
    ARG		(LPELEMENT	ElementPtr0)
    ARG		(PPOINT		pPt0)
    ARG		(LPELEMENT	ElementPtr1)
    ARG_END	(PPOINT		pPt1)
)
{
    PUSH_CONTEXT
    {
	if (ElementPtr1->state_flags & PLAYER_SHIP)
	{
	    STAMP		s;
	    STARSHIPPTR		StarShipPtr;
	    RACE_DESCPTR	RDPtr;

	    GetElementStarShip (ElementPtr1, &StarShipPtr);
	    RDPtr = StarShipPtr->RaceDescPtr;

	    if (++RDPtr->characteristics.turn_wait == 0)
		--RDPtr->characteristics.turn_wait;
	    if (++RDPtr->characteristics.thrust_wait == 0)
		--RDPtr->characteristics.thrust_wait;
#define MIN_THRUST_INCREMENT	DISPLAY_TO_WORLD (1)
	    if (RDPtr->characteristics.thrust_increment <= MIN_THRUST_INCREMENT)
	    {
		RDPtr->characteristics.max_thrust =
			RDPtr->characteristics.thrust_increment << 1;
	    }
	    else
	    {
		COUNT	num_thrusts;

		num_thrusts = RDPtr->characteristics.max_thrust /
			RDPtr->characteristics.thrust_increment;
		--RDPtr->characteristics.thrust_increment;
		RDPtr->characteristics.max_thrust =
			RDPtr->characteristics.thrust_increment * num_thrusts;
	    }
	    RDPtr->cyborg_control.ManeuverabilityIndex = 0;

	    GetElementStarShip (ElementPtr0, &StarShipPtr);
	    ProcessSound (SetAbsSoundIndex (
			    /* LIMPET_AFFIXES */
		    StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 2));
	    s.frame = SetAbsFrameIndex (
		    StarShipPtr->RaceDescPtr->ship_data.weapon[0], (COUNT)random ()
		    );
	    ModifySilhouette (ElementPtr1, &s, MODIFY_IMAGE);
	}

	ElementPtr0->hit_points = 0;
	ElementPtr0->life_span = 0;
	ElementPtr0->state_flags |= COLLISION | DISAPPEARING;
    }
    POP_CONTEXT
}

PROC(STATIC
void spawn_limpets, (ElementPtr),
    ARG_END	(LPELEMENT	ElementPtr)
)
{
#define LIMPET_OFFSET		8
#define LIMPET_LIFE		80
#define LIMPET_HITS		1
#define LIMPET_DAMAGE		0
    HELEMENT		Limpet;
    STARSHIPPTR		StarShipPtr;
    MISSILE_BLOCK	MissileBlock;

    GetElementStarShip (ElementPtr, &StarShipPtr);
    MissileBlock.farray = StarShipPtr->RaceDescPtr->ship_data.special;
    MissileBlock.face = StarShipPtr->ShipFacing + HALF_CIRCLE;
    MissileBlock.index = 0;
    MissileBlock.sender = (ElementPtr->state_flags & (GOOD_GUY | BAD_GUY))
	    | IGNORE_SIMILAR;
    MissileBlock.pixoffs = LIMPET_OFFSET;
    MissileBlock.speed = LIMPET_SPEED;
    MissileBlock.hit_points = LIMPET_HITS;
    MissileBlock.damage = LIMPET_DAMAGE;
    MissileBlock.life = LIMPET_LIFE;
    MissileBlock.preprocess_func = limpet_preprocess;
    MissileBlock.blast_offs = 0;

    MissileBlock.cx = ElementPtr->next.location.x;
    MissileBlock.cy = ElementPtr->next.location.y;
    if (Limpet = initialize_missile (&MissileBlock))
    {
	ELEMENTPTR	LimpetPtr;

	LockElement (Limpet, &LimpetPtr);
	LimpetPtr->collision_func = limpet_collision;
	SetElementStarShip (LimpetPtr, StarShipPtr);
	UnlockElement (Limpet);

	PutElement (Limpet);
    }
}

PROC(STATIC
COUNT far initialize_horrific_laser, (ShipPtr, LaserArray),
    ARG		(LPELEMENT	ShipPtr)
    ARG_END	(HELEMENT	far LaserArray[])
)
{
    PUSH_CONTEXT
    {
	STARSHIPPTR	StarShipPtr;
	LASER_BLOCK	LaserBlock;

	GetElementStarShip (ShipPtr, &StarShipPtr);
	LaserBlock.face = StarShipPtr->ShipFacing;
	LaserBlock.cx = ShipPtr->next.location.x;
	LaserBlock.cy = ShipPtr->next.location.y;
	LaserBlock.ex = COSINE (FACING_TO_ANGLE (LaserBlock.face), LASER_RANGE);
	LaserBlock.ey = SINE (FACING_TO_ANGLE (LaserBlock.face), LASER_RANGE);
	LaserBlock.sender = (ShipPtr->state_flags & (GOOD_GUY | BAD_GUY))
		| IGNORE_SIMILAR;
	LaserBlock.pixoffs = VUX_OFFSET;
	LaserBlock.color = BUILD_COLOR (MAKE_RGB15 (0xA, 0x1F, 0xA), 0x0A);
	LaserArray[0] = initialize_laser (&LaserBlock);
    }
    POP_CONTEXT

    return (1);
}

PROC(STATIC
void far vux_intelligence, (ShipPtr, ObjectsOfConcern, ConcernCounter),
    ARG		(LPELEMENT		ShipPtr)
    ARG		(LPEVALUATE_DESC	ObjectsOfConcern)
    ARG_END	(COUNT			ConcernCounter)
)
{
    PUSH_CONTEXT
    {
	LPEVALUATE_DESC	lpEvalDesc;
	STARSHIPPTR	StarShipPtr;

	lpEvalDesc = &ObjectsOfConcern[ENEMY_SHIP_INDEX];
 	lpEvalDesc->MoveState = PURSUE;
	if (ObjectsOfConcern[ENEMY_WEAPON_INDEX].ObjectPtr != 0
		&& ObjectsOfConcern[ENEMY_WEAPON_INDEX].MoveState == ENTICE)
	{
	    if ((ObjectsOfConcern[ENEMY_WEAPON_INDEX].ObjectPtr->state_flags
		    & FINITE_LIFE)
		    && !(ObjectsOfConcern[ENEMY_WEAPON_INDEX].ObjectPtr->state_flags
		    & CREW_OBJECT))
		ObjectsOfConcern[ENEMY_WEAPON_INDEX].MoveState = AVOID;
	    else
		ObjectsOfConcern[ENEMY_WEAPON_INDEX].MoveState = PURSUE;
	}

	ship_intelligence (ShipPtr,
		ObjectsOfConcern, ConcernCounter);

	GetElementStarShip (ShipPtr, &StarShipPtr);
	if (StarShipPtr->special_counter == 0
		&& lpEvalDesc->ObjectPtr != 0
		&& lpEvalDesc->which_turn <= 12
		&& (StarShipPtr->ship_input_state & (LEFT | RIGHT))
		&& StarShipPtr->RaceDescPtr->ship_info.energy_level >=
		(BYTE)(StarShipPtr->RaceDescPtr->ship_info.max_energy >> 1))
	    StarShipPtr->ship_input_state |= SPECIAL;
	else
	    StarShipPtr->ship_input_state &= ~SPECIAL;
    }
    POP_CONTEXT
}

PROC(STATIC
void far vux_postprocess, (ElementPtr),
    ARG_END	(LPELEMENT	ElementPtr)
)
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
			    /* LAUNCH_LIMPET */
		    StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 1));
	    spawn_limpets (ElementPtr);

	    StarShipPtr->special_counter =
		    StarShipPtr->RaceDescPtr->characteristics.special_wait;
	}
    }
    POP_CONTEXT
}

PROC(STATIC
void far vux_preprocess, (ElementPtr),
    ARG_END	(LPELEMENT	ElementPtr)
)
{
    PUSH_CONTEXT
    {
	if (ElementPtr->state_flags & APPEARING)
	{
	    COUNT	facing;
	    STARSHIPPTR	StarShipPtr;

	    GetElementStarShip (ElementPtr, &StarShipPtr);
	    facing = StarShipPtr->ShipFacing;
	    if (LOBYTE (GLOBAL (CurrentActivity)) != IN_ENCOUNTER
		    && TrackShip (ElementPtr, &facing) >= 0)
	    {
		ELEMENTPTR	OtherShipPtr;

		LockElement (ElementPtr->hTarget, &OtherShipPtr);

		do
		{
#define MAXX_ENTRY_DIST	DISPLAY_TO_WORLD (SPACE_HEIGHT << 1)
#define MAXY_ENTRY_DIST	DISPLAY_TO_WORLD (SPACE_HEIGHT << 1)
		    SIZE	dx, dy;

		    ElementPtr->current.location.x =
			    (OtherShipPtr->current.location.x -
			    (MAXX_ENTRY_DIST >> 1)) +
			    ((COUNT)random () % MAXX_ENTRY_DIST);
		    ElementPtr->current.location.y =
			    (OtherShipPtr->current.location.y -
			    (MAXY_ENTRY_DIST >> 1)) +
			    ((COUNT)random () % MAXY_ENTRY_DIST);
		    dx = OtherShipPtr->current.location.x -
			    ElementPtr->current.location.x;
		    dy = OtherShipPtr->current.location.y -
			    ElementPtr->current.location.y;
		    facing = NORMALIZE_FACING (
			    ANGLE_TO_FACING (ARCTAN (dx, dy))
			    );
		    ElementPtr->current.image.frame =
			    SetAbsFrameIndex (ElementPtr->current.image.frame,
			    facing);

		    ElementPtr->current.location.x =
			    WRAP_X (DISPLAY_ALIGN (ElementPtr->current.location.x));
		    ElementPtr->current.location.y =
			    WRAP_Y (DISPLAY_ALIGN (ElementPtr->current.location.y));
		} while (CalculateGravity (ElementPtr)
			|| TimeSpaceMatterConflict (ElementPtr));

		UnlockElement (ElementPtr->hTarget);
		ElementPtr->hTarget = 0;

		ElementPtr->next = ElementPtr->current;
		InitIntersectStartPoint (ElementPtr);
		InitIntersectEndPoint (ElementPtr);
		InitIntersectFrame (ElementPtr);

		StarShipPtr->ShipFacing = facing;
	    }

	    StarShipPtr->RaceDescPtr->preprocess_func = 0;
	}
    }
    POP_CONTEXT
}

PROC(
RACE_DESCPTR far init_vux, (),
    ARG_VOID
)
{
    RACE_DESCPTR	RaceDescPtr;

    PUSH_CONTEXT
    {
	vux_desc.preprocess_func = vux_preprocess;
	vux_desc.postprocess_func = vux_postprocess;
	vux_desc.init_weapon_func = initialize_horrific_laser;
	vux_desc.cyborg_control.intelligence_func =
		(PROC_PARAMETER(
		void (far *), (ShipPtr, ObjectsOfConcern, ConcernCounter),
		    ARG		(LPVOID		ShipPtr)
		    ARG		(LPVOID		ObjectsOfConcern)
		    ARG_END	(COUNT		ConcernCounter)
		)) vux_intelligence;

	RaceDescPtr = &vux_desc;
    }
    POP_CONTEXT

    return (RaceDescPtr);
}

