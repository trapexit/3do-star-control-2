#include "reslib.h"
#include "resinst.h"
#include "starcon.h"

#define MAX_CREW		16
#define MAX_ENERGY		20
#define ENERGY_REGENERATION	1
#define WEAPON_ENERGY_COST	(MAX_ENERGY / 3)
#define SPECIAL_ENERGY_COST	0
#define ENERGY_WAIT		6
#define MAX_THRUST		35
#define THRUST_INCREMENT	5
#define TURN_WAIT		1
#define THRUST_WAIT		0
#define WEAPON_WAIT		4
#define SPECIAL_WAIT		12

#define SHIP_MASS		4
#define ORZ_OFFSET		9
#define MISSILE_SPEED		DISPLAY_TO_WORLD (30)
#define MISSILE_LIFE		12

static RACE_DESC	orz_desc =
{
    {
	FIRES_FORE | SEEKING_SPECIAL,
	23,					/* Super Melee cost */
	333 / SPHERE_RADIUS_INCREMENT,		/* Initial sphere of influence radius */
	MAX_CREW, MAX_CREW,
	MAX_ENERGY, MAX_ENERGY,
	{
	    3608, 2637,
	},
	(STRING)ORZ_RACE_STRINGS,
	(FRAME)ORZ_ICON_MASK_PMAP_ANIM,
	(FRAME)ORZ_MICON_MASK_PMAP_ANIM,
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
	    (FRAME)ORZ_BIG_MASK_PMAP_ANIM,
	    (FRAME)ORZ_MED_MASK_PMAP_ANIM,
	    (FRAME)ORZ_SML_MASK_PMAP_ANIM,
	},
	{
	    (FRAME)HOWITZER_BIG_MASK_PMAP_ANIM,
	    (FRAME)HOWITZER_MED_MASK_PMAP_ANIM,
	    (FRAME)HOWITZER_SML_MASK_PMAP_ANIM,
	},
	{
	    (FRAME)TURRET_BIG_MASK_PMAP_ANIM,
	    (FRAME)TURRET_MED_MASK_PMAP_ANIM,
	    (FRAME)TURRET_SML_MASK_PMAP_ANIM,
	},
	{
	    (FRAME)ORZ_CAPTAIN_MASK_PMAP_ANIM,
	    (FRAME)0,
	    (FRAME)0,
	    (FRAME)0,
	    (FRAME)0,
	},
	(SOUND)ORZ_VICTORY_SONG,
	(SOUND)ORZ_SHIP_SOUNDS,
    },
    {
	0,
	MISSILE_SPEED * MISSILE_LIFE,
	NULL_PTR,
    },
    NULL_PTR,
    NULL_PTR,
    NULL_PTR,
    0,
};

static void	far
howitzer_collision (ElementPtr0, pPt0, ElementPtr1, pPt1)
LPELEMENT	ElementPtr0, ElementPtr1;
PPOINT		pPt0, pPt1;
{
    PUSH_CONTEXT
    {
	if ((ElementPtr0->state_flags & (GOOD_GUY | BAD_GUY))
		!= (ElementPtr1->state_flags & (GOOD_GUY | BAD_GUY)))
	    weapon_collision (ElementPtr0, pPt0, ElementPtr1, pPt1);
    }
    POP_CONTEXT
}

#define TURRET_OFFSET	14
#define TURRET_WAIT	3

static COUNT	far
initialize_turret_missile (ShipPtr, MissileArray)
LPELEMENT	ShipPtr;
HELEMENT	far MissileArray[];
{
#define MISSILE_HITS		2
#define MISSILE_DAMAGE		3
#define MISSILE_OFFSET		1
    PUSH_CONTEXT
    {
	ELEMENTPTR	TurretPtr;
	STARSHIPPTR	StarShipPtr;
	MISSILE_BLOCK	MissileBlock;

	GetElementStarShip (ShipPtr, &StarShipPtr);
	MissileBlock.cx = ShipPtr->next.location.x;
	MissileBlock.cy = ShipPtr->next.location.y;
	MissileBlock.farray = StarShipPtr->RaceDescPtr->ship_data.weapon;

	LockElement (GetSuccElement (ShipPtr), &TurretPtr);
	if (TurretPtr->turn_wait == 0
		&& (StarShipPtr->cur_status_flags & SPECIAL)
		&& (StarShipPtr->cur_status_flags & (LEFT | RIGHT)))
	{
	    if (StarShipPtr->cur_status_flags & RIGHT)
		++TurretPtr->thrust_wait;
	    else
		--TurretPtr->thrust_wait;

	    TurretPtr->turn_wait = TURRET_WAIT + 1;
	}
	MissileBlock.face = MissileBlock.index = 
		NORMALIZE_FACING (StarShipPtr->ShipFacing
		+ TurretPtr->thrust_wait);
	UnlockElement (GetSuccElement (ShipPtr));

	MissileBlock.sender = (ShipPtr->state_flags & (GOOD_GUY | BAD_GUY))
		| IGNORE_SIMILAR;
	MissileBlock.pixoffs = TURRET_OFFSET;
	MissileBlock.speed = MISSILE_SPEED;
	MissileBlock.hit_points = MISSILE_HITS;
	MissileBlock.damage = MISSILE_DAMAGE;
	MissileBlock.life = MISSILE_LIFE;
	MissileBlock.preprocess_func = NULL_PTR;
	MissileBlock.blast_offs = MISSILE_OFFSET;
	MissileArray[0] = initialize_missile (&MissileBlock);

	if (MissileArray[0])
	{
	    ELEMENTPTR	HowitzerPtr;

	    LockElement (MissileArray[0], &HowitzerPtr);
	    HowitzerPtr->collision_func = howitzer_collision;
	    UnlockElement (MissileArray[0]);
	}
    }
    POP_CONTEXT

    return (1);
}

#define MAX_MARINES	8

PROC(STATIC
BYTE count_marines, (StarShipPtr, FindSpot),
    ARG		(STARSHIPPTR	StarShipPtr)
    ARG_END	(BOOLEAN	FindSpot)
)
{
    BYTE	num_marines, id_use[MAX_MARINES];
    HELEMENT	hElement, hNextElement;

    num_marines = MAX_MARINES;
    while (num_marines--)
	id_use[num_marines] = 0;

    num_marines = 0;
    for (hElement = GetTailElement (); hElement; hElement = hNextElement)
    {
	ELEMENTPTR	ElementPtr;

	LockElement (hElement, &ElementPtr);
	hNextElement = GetPredElement (ElementPtr);
	if (ElementPtr->current.image.farray == StarShipPtr->RaceDescPtr->ship_data.special
		&& ElementPtr->life_span
		&& !(ElementPtr->state_flags & (FINITE_LIFE | DISAPPEARING)))
	{
	    if (ElementPtr->state_flags & NONSOLID)
	    {
		id_use[ElementPtr->turn_wait] = 1;
	    }

	    if (++num_marines == MAX_MARINES)
	    {
		UnlockElement (hElement);
		hNextElement = 0;
	    }
	}
	UnlockElement (hElement);
    }

    if (FindSpot)
    {
	num_marines = 0;
	while (id_use[num_marines])
	    ++num_marines;
    }

    return (num_marines);
}

static void	 far
orz_intelligence (ShipPtr, ObjectsOfConcern, ConcernCounter)
LPELEMENT	ShipPtr;
LPEVALUATE_DESC	ObjectsOfConcern;
COUNT		ConcernCounter;
{
    PUSH_CONTEXT
    {
	ELEMENTPTR	TurretPtr;
	STARSHIPPTR	StarShipPtr;
	LPEVALUATE_DESC	lpEvalDesc;

	LockElement (GetSuccElement (ShipPtr), &TurretPtr);

	++TurretPtr->turn_wait;
	ship_intelligence (ShipPtr, ObjectsOfConcern, ConcernCounter);
	--TurretPtr->turn_wait;

	GetElementStarShip (ShipPtr, &StarShipPtr);
	lpEvalDesc = &ObjectsOfConcern[ENEMY_SHIP_INDEX];
	if (lpEvalDesc->ObjectPtr == 0)
	    StarShipPtr->ship_input_state &= ~SPECIAL;
	else if (StarShipPtr->special_counter != 1)
	{
	    STARSHIPPTR	EnemyStarShipPtr;

	    if (ShipPtr->turn_wait == 0
		    && lpEvalDesc->MoveState == ENTICE
		    && lpEvalDesc->which_turn < 24
		    && (StarShipPtr->cur_status_flags
		    & (SHIP_AT_MAX_SPEED | SHIP_BEYOND_MAX_SPEED))
		    && !(StarShipPtr->ship_input_state & THRUST)
		    && NORMALIZE_ANGLE (
		    GetVelocityTravelAngle (&ShipPtr->velocity)
		    - ARCTAN (
			    lpEvalDesc->ObjectPtr->next.location.x
			    - ShipPtr->next.location.x,
			    lpEvalDesc->ObjectPtr->next.location.y
			    - ShipPtr->next.location.y
		    ) + (QUADRANT - (OCTANT >> 1))) >=
		    ((QUADRANT - (OCTANT >> 1)) << 1))
		StarShipPtr->ship_input_state &= ~(LEFT | RIGHT);

	    StarShipPtr->ship_input_state &= ~SPECIAL;
	    if (ShipPtr->turn_wait == 0
		    && !(StarShipPtr->ship_input_state & (LEFT | RIGHT | WEAPON))
		    && TurretPtr->turn_wait == 0)
	    {
		SIZE	delta_facing;
		COUNT	facing, orig_facing;

		facing = NORMALIZE_FACING (StarShipPtr->ShipFacing
			+ TurretPtr->thrust_wait);
		if ((delta_facing = TrackShip (TurretPtr, &facing)) > 0)
		{
		    StarShipPtr->ship_input_state |= SPECIAL;
		    if (delta_facing == ANGLE_TO_FACING (HALF_CIRCLE))
			delta_facing += (((BYTE)random () & 1) << 1) - 1;

		    if (delta_facing < ANGLE_TO_FACING (HALF_CIRCLE))
			StarShipPtr->ship_input_state |= RIGHT;
		    else
			StarShipPtr->ship_input_state |= LEFT;
		}
	    }

	    GetElementStarShip (lpEvalDesc->ObjectPtr, &EnemyStarShipPtr);
	    if (StarShipPtr->special_counter == 0
		    && !(StarShipPtr->ship_input_state & WEAPON)
		    && StarShipPtr->RaceDescPtr->ship_info.crew_level >
		    (BYTE)(StarShipPtr->RaceDescPtr->ship_info.max_crew >> 2)
		    && !(EnemyStarShipPtr->RaceDescPtr->ship_info.ship_flags
		    & POINT_DEFENSE)
		    && (MANEUVERABILITY (
			    &EnemyStarShipPtr->RaceDescPtr->cyborg_control
			    ) < SLOW_SHIP
		    || lpEvalDesc->which_turn <= 12
		    || count_marines (StarShipPtr, FALSE) < 2))
	    {
		StarShipPtr->ship_input_state |= WEAPON | SPECIAL;
	    }
	}

	UnlockElement (GetSuccElement (ShipPtr));
    }
    POP_CONTEXT
}

#define START_ION_COLOR		BUILD_COLOR (MAKE_RGB15 (0x1F, 0x15, 0x00), 0x7A)

PROC(
void far ion_preprocess, (ElementPtr),
    ARG_END	(LPELEMENT	ElementPtr)
)
{
    PUSH_CONTEXT
    {
    	COLOR color_tab[] =
    	      { BUILD_COLOR (MAKE_RGB15 (0x1F, 0x00, 0x00), 0x2a), 
	  	BUILD_COLOR (MAKE_RGB15 (0x1B, 0x00, 0x00), 0x2b), 
	  	BUILD_COLOR (MAKE_RGB15 (0x17, 0x00, 0x00), 0x2c), 
	  	BUILD_COLOR (MAKE_RGB15 (0x13, 0x00, 0x00), 0x2d), 
	  	BUILD_COLOR (MAKE_RGB15 (0xF, 0x00, 0x00),  0x2e), 
	  	BUILD_COLOR (MAKE_RGB15 (0xB, 0x00, 0x00),  0x2f), 
	  	BUILD_COLOR (MAKE_RGB15 (0x1F, 0x15, 0x00), 0x7a), 
	  	BUILD_COLOR (MAKE_RGB15 (0x1F, 0x11, 0x00), 0x7b), 
	  	BUILD_COLOR (MAKE_RGB15 (0x1F, 0xE, 0x00),  0x7c), 
	  	BUILD_COLOR (MAKE_RGB15 (0x1F, 0xA, 0x00),  0x7d), 
	  	BUILD_COLOR (MAKE_RGB15 (0x1F, 0x7, 0x00),  0x7e), 
	  	BUILD_COLOR (MAKE_RGB15 (0x1F, 0x3, 0x00),   0x7f), };
#define NUM_TAB_COLORS	(sizeof (color_tab) / sizeof (color_tab[0]))
		
	COUNT color_index = 0;
	COLOR	Color;

	Color = COLOR_256 (GetPrimColor (&(GLOBAL (DisplayArray))[
		ElementPtr->PrimIndex
		]));
	if (Color != 0x2D)
	{
	    ElementPtr->life_span = ElementPtr->thrust_wait;

	    Color += 2;
	    if (Color > 0x7F)
		Color = 0x2B;
	    else if (Color == 0x7E)
		Color = 0x7F;
	    if (Color <= 0x2f && Color >= 0x2a)
	    	color_index = Color - 0x2a;
	    else /* color is between 0x7a and 0x7f */
	    	color_index = (Color - 0x7a) + (NUM_TAB_COLORS >> 1);
	    SetPrimColor (&(GLOBAL (DisplayArray))[
		    ElementPtr->PrimIndex
		    ], color_tab[color_index]);

	    ElementPtr->state_flags &= ~DISAPPEARING;
	    ElementPtr->state_flags |= CHANGING;
	}
    }
    POP_CONTEXT
}

PROC_LOCAL(
void far marine_preprocess, (ElementPtr),
    ARG_END	(LPELEMENT	ElementPtr)
);

#define MARINE_WAIT	12

static void	far
intruder_preprocess (ElementPtr)
LPELEMENT	ElementPtr;
{
    PUSH_CONTEXT
    {
	HELEMENT	hElement, hNextElement;
	ELEMENTPTR	ShipPtr;
	STARSHIPPTR	StarShipPtr;

	GetElementStarShip (ElementPtr, &StarShipPtr);
	LockElement (StarShipPtr->hShip, &ShipPtr);
	if (ShipPtr->crew_level == 0
		&& ShipPtr->life_span == 1
		&& (ShipPtr->state_flags & (FINITE_LIFE | NONSOLID)) ==
		(FINITE_LIFE | NONSOLID))
	{
	    ElementPtr->life_span = 0;
	    ElementPtr->state_flags |= DISAPPEARING;
	}
	UnlockElement (StarShipPtr->hShip);

	if (ElementPtr->thrust_wait)
	    --ElementPtr->thrust_wait;

	for (hElement = GetHeadElement (); hElement; hElement = hNextElement)
	{
	    LockElement (hElement, &ShipPtr);
	    if ((ShipPtr->state_flags & PLAYER_SHIP)
		    && (ShipPtr->state_flags & (GOOD_GUY | BAD_GUY))
		    != (ElementPtr->state_flags & (GOOD_GUY | BAD_GUY)))
	    {
		STAMP	s;

		if (ElementPtr->thrust_wait == MARINE_WAIT)
		{
		    --ElementPtr->thrust_wait;

		    s.origin.x = 16 + (ElementPtr->turn_wait & 3) * 9;
		    s.origin.y = 14 + (ElementPtr->turn_wait >> 2) * 11;
		    s.frame = SetAbsFrameIndex (ElementPtr->next.image.farray[0],
			    GetFrameCount (ElementPtr->next.image.farray[0]) - 2);
		    ModifySilhouette (ShipPtr, &s, 0);
		}

		ElementPtr->next.location = ShipPtr->next.location;

		if (ShipPtr->crew_level == 0
			|| ElementPtr->life_span == 0)
		{
		    UnlockElement (hElement);
		    hElement = 0;
LeftShip:
		    s.origin.x = 16 + (ElementPtr->turn_wait & 3) * 9;
		    s.origin.y = 14 + (ElementPtr->turn_wait >> 2) * 11;
		    s.frame = ElementPtr->next.image.frame;
		    ModifySilhouette (ShipPtr, &s, MODIFY_SWAP);
		}
		else if (ElementPtr->thrust_wait == 0)
		{
		    BYTE	randval;

		    ElementPtr->thrust_wait = MARINE_WAIT;

		    randval = (BYTE)random ();
		    if (randval < (0x0100 / 16))
		    {
			ElementPtr->life_span = 0;
			ElementPtr->state_flags |= DISAPPEARING;

			ProcessSound (SetAbsSoundIndex (
				StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 4));
			goto LeftShip;
		    }
		    else if (randval < (0x0100 / 2 + 0x0100 / 16))
		    {
			if (!DeltaCrew (ShipPtr, -1))
			    ShipPtr->life_span = 0;

			++ElementPtr->thrust_wait;
			s.origin.x = 16 + (ElementPtr->turn_wait & 3) * 9;
			s.origin.y = 14 + (ElementPtr->turn_wait >> 2) * 11;
			s.frame = SetAbsFrameIndex (ElementPtr->next.image.farray[0],
				GetFrameCount (ElementPtr->next.image.farray[0]) - 1);
			ModifySilhouette (ShipPtr, &s, 0);
			ProcessSound (SetAbsSoundIndex (
				StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 3));
		    }
		}

		UnlockElement (hElement);
		break;
	    }
	    hNextElement = GetSuccElement (ShipPtr);
	    UnlockElement (hElement);
	}

	if (hElement == 0 && ElementPtr->life_span)
	{
	    ElementPtr->state_flags &= ~NONSOLID;
	    ElementPtr->state_flags |= CHANGING | CREW_OBJECT;
	    SetPrimType (&(GLOBAL (DisplayArray))[
		    ElementPtr->PrimIndex
		    ], STAMP_PRIM);

	    ElementPtr->current.image.frame =
		    ElementPtr->next.image.frame =
			    SetAbsFrameIndex (
			    StarShipPtr->RaceDescPtr->ship_data.special[0], 21
			    );
	    ElementPtr->thrust_wait = 0;
	    ElementPtr->turn_wait =
		    MAKE_BYTE (0, NORMALIZE_FACING ((BYTE)random ()));
	    ElementPtr->preprocess_func = marine_preprocess;
	}
    }
    POP_CONTEXT
}

static void	far
marine_preprocess (ElementPtr)
LPELEMENT	ElementPtr;
{
    PUSH_CONTEXT
    {
	ELEMENTPTR	ShipPtr;
	STARSHIPPTR	StarShipPtr;

	GetElementStarShip (ElementPtr, &StarShipPtr);
	LockElement (StarShipPtr->hShip, &ShipPtr);
	if (ShipPtr->crew_level == 0
		&& ShipPtr->life_span == 1
		&& (ShipPtr->state_flags & (FINITE_LIFE | NONSOLID)) ==
		(FINITE_LIFE | NONSOLID))
	{
	    ElementPtr->life_span = 0;
	    ElementPtr->state_flags |= DISAPPEARING | NONSOLID;
	    ElementPtr->turn_wait = 1;
	}
	UnlockElement (StarShipPtr->hShip);

	if (LONIBBLE (ElementPtr->turn_wait))
	    --ElementPtr->turn_wait;
	else
	{
	    COUNT	facing, pfacing;
	    SIZE	delta_x, delta_y, delta_facing;
	    HELEMENT	hObject, hNextObject, hTarget;
	    ELEMENTPTR	ObjectPtr;

	    ElementPtr->thrust_wait &= ~(SHIP_IN_GRAVITY_WELL >> 6);

	    hTarget = 0;
	    for (hObject = GetHeadElement ();
		    hObject; hObject = hNextObject)
	    {
		LockElement (hObject, &ObjectPtr);
		hNextObject = GetSuccElement (ObjectPtr);
		if (GRAVITY_MASS (ObjectPtr->mass_points))
		{
		    delta_x = ObjectPtr->current.location.x
			    - ElementPtr->current.location.x;
		    delta_x = WRAP_DELTA_X (delta_x);

		    delta_y = ObjectPtr->current.location.y
			    - ElementPtr->current.location.y;
		    delta_y = WRAP_DELTA_Y (delta_y);
		    if ((long)delta_x * delta_x + (long)delta_y * delta_y <=
			    (long)DISPLAY_TO_WORLD (GRAVITY_THRESHOLD)
			    * DISPLAY_TO_WORLD (GRAVITY_THRESHOLD))
		    {
			pfacing = ANGLE_TO_FACING (ARCTAN (delta_x, delta_y));
			delta_facing = NORMALIZE_FACING (
				pfacing - ANGLE_TO_FACING (
					GetVelocityTravelAngle (&ElementPtr->velocity)
					) + ANGLE_TO_FACING (OCTANT)
				);
			if (delta_facing <= ANGLE_TO_FACING (QUADRANT))
			{
			    hTarget = hObject;
			    hNextObject = 0;
			}

			ElementPtr->thrust_wait |= (SHIP_IN_GRAVITY_WELL >> 6);
		    }
		}
		else if ((ObjectPtr->state_flags & PLAYER_SHIP)
			&& ObjectPtr->crew_level
			&& !OBJECT_CLOAKED (ObjectPtr))
		{
		    if ((ObjectPtr->state_flags & (GOOD_GUY | BAD_GUY)) !=
			    (ElementPtr->state_flags & (GOOD_GUY | BAD_GUY)))
		    {
			if (ElementPtr->state_flags & IGNORE_SIMILAR)
			    hTarget = hObject;
		    }
		    else if (hTarget == 0)
			hTarget = hObject;
		}
		UnlockElement (hObject);
	    }

	    facing = HINIBBLE (ElementPtr->turn_wait);
	    if (hTarget == 0)
		delta_facing = -1;
	    else
	    {
		LockElement (hTarget, &ObjectPtr);
		delta_x = ObjectPtr->current.location.x
			- ElementPtr->current.location.x;
		delta_x = WRAP_DELTA_X (delta_x);
		delta_y = ObjectPtr->current.location.y
			- ElementPtr->current.location.y;
		delta_y = WRAP_DELTA_Y (delta_y);
		if (GRAVITY_MASS (ObjectPtr->mass_points))
		{
		    delta_facing = NORMALIZE_FACING (pfacing - facing
			    + ANGLE_TO_FACING (OCTANT));

		    if (delta_facing > ANGLE_TO_FACING (QUADRANT))
			delta_facing = 0;
		    else
		    {
			if (delta_facing == ANGLE_TO_FACING (OCTANT))
			    facing += (((SIZE)random () & 1) << 1) - 1;
			else if (delta_facing < ANGLE_TO_FACING (OCTANT))
			    ++facing;
			else
			    --facing;
		    }
		}
		else
		{
		    COUNT		num_frames;
		    VELOCITY_DESC	ShipVelocity;

		    if ((ObjectPtr->state_flags & (GOOD_GUY | BAD_GUY)) ==
			    (ElementPtr->state_flags & (GOOD_GUY | BAD_GUY))
			    && (ElementPtr->state_flags & IGNORE_SIMILAR))
		    {
			ElementPtr->next.image.frame =
				SetAbsFrameIndex (
				StarShipPtr->RaceDescPtr->ship_data.special[0], 21
				);
			ElementPtr->state_flags &= ~IGNORE_SIMILAR;
			ElementPtr->state_flags |= CHANGING;
		    }

		    if ((num_frames = WORLD_TO_TURN (
			    square_root ((long)delta_x * delta_x
			    + (long)delta_y * delta_y)
			    )) == 0)
			num_frames = 1;

		    ShipVelocity = ObjectPtr->velocity;
		    GetNextVelocityComponents (&ShipVelocity,
			    &delta_x, &delta_y, num_frames);

		    delta_x = (ObjectPtr->current.location.x + delta_x)
			    - ElementPtr->current.location.x;
		    delta_y = (ObjectPtr->current.location.y + delta_y)
			    - ElementPtr->current.location.y;

		    delta_facing = NORMALIZE_FACING (
			    ANGLE_TO_FACING (ARCTAN (delta_x, delta_y)) - facing
			    );

		    if (delta_facing > 0)
		    {
			if (delta_facing == ANGLE_TO_FACING (HALF_CIRCLE))
			    facing += (((BYTE)random () & 1) << 1) - 1;
			else if (delta_facing < ANGLE_TO_FACING (HALF_CIRCLE))
			    ++facing;
			else
			    --facing;
		    }
		}
		UnlockElement (hTarget);
	    }

	    ElementPtr->turn_wait = MAKE_BYTE (0, NORMALIZE_FACING (facing));
	    if (delta_facing == 0
		     || ((ElementPtr->thrust_wait & (SHIP_BEYOND_MAX_SPEED >> 6))
		     && !(ElementPtr->thrust_wait & (SHIP_IN_GRAVITY_WELL >> 6))))
	    {
		UWORD		thrust_status;
		COUNT		OldFacing;
		UWORD		OldStatus;
		COUNT		OldIncrement, OldThrust;
		STARSHIPPTR	StarShipPtr;

		GetElementStarShip (ElementPtr, &StarShipPtr);

		OldFacing = StarShipPtr->ShipFacing;
		OldStatus = StarShipPtr->cur_status_flags;
		OldIncrement = StarShipPtr->RaceDescPtr->characteristics.thrust_increment;
		OldThrust = StarShipPtr->RaceDescPtr->characteristics.max_thrust;

		StarShipPtr->ShipFacing = facing;
		StarShipPtr->cur_status_flags = ElementPtr->thrust_wait << 6;
		StarShipPtr->RaceDescPtr->characteristics.thrust_increment = 8;
		StarShipPtr->RaceDescPtr->characteristics.max_thrust = 32;

		thrust_status = inertial_thrust (ElementPtr);

		StarShipPtr->RaceDescPtr->characteristics.max_thrust = OldThrust;
		StarShipPtr->RaceDescPtr->characteristics.thrust_increment = OldIncrement;
		StarShipPtr->cur_status_flags = OldStatus;
		StarShipPtr->ShipFacing = OldFacing;

		if ((ElementPtr->thrust_wait & (SHIP_IN_GRAVITY_WELL >> 6))
			|| delta_facing
			|| !(thrust_status
			& (SHIP_AT_MAX_SPEED | SHIP_BEYOND_MAX_SPEED)))
		{
		    HELEMENT	hIonElement;

		    if (hIonElement = AllocElement ())
		    {
#define ION_LIFE		1
			COUNT	angle;
			ELEMENTPTR	IonElementPtr;

			angle = FACING_TO_ANGLE (facing) + HALF_CIRCLE;

			InsertElement (hIonElement, GetHeadElement ());
			LockElement (hIonElement, &IonElementPtr);
			IonElementPtr->state_flags = APPEARING | FINITE_LIFE | NONSOLID;
			IonElementPtr->life_span = IonElementPtr->thrust_wait = ION_LIFE;
			SetPrimType (&(GLOBAL (DisplayArray))[
				IonElementPtr->PrimIndex
				], POINT_PRIM);
			SetPrimColor (&(GLOBAL (DisplayArray))[
				IonElementPtr->PrimIndex
				], START_ION_COLOR);
			IonElementPtr->current.location = ElementPtr->current.location;
			IonElementPtr->current.location.x +=
				(COORD)COSINE (angle, DISPLAY_TO_WORLD (2));
			IonElementPtr->current.location.y +=
				(COORD)SINE (angle, DISPLAY_TO_WORLD (2));
			IonElementPtr->death_func = ion_preprocess;

			SetElementStarShip (IonElementPtr, StarShipPtr);

			{
				/* normally done during preprocess, but because
				 * object is being inserted at head rather than
				 * appended after tail it may never get preprocessed.
				 */
			    IonElementPtr->next = IonElementPtr->current;
			    --IonElementPtr->life_span;
			    IonElementPtr->state_flags |= PRE_PROCESS;
			}

			UnlockElement (hIonElement);
		    }
		}

		ElementPtr->thrust_wait = (BYTE)(thrust_status >> 6);
	    }
	}
    }
    POP_CONTEXT
}

static void	far
marine_collision (ElementPtr0, pPt0, ElementPtr1, pPt1)
LPELEMENT	ElementPtr0, ElementPtr1;
PPOINT		pPt0, pPt1;
{
    PUSH_CONTEXT
    {
        if (ElementPtr0->life_span
		&& !(ElementPtr0->state_flags & (NONSOLID | COLLISION))
		&& !(ElementPtr1->state_flags & FINITE_LIFE))
        {
	    if ((ElementPtr0->state_flags & (GOOD_GUY | BAD_GUY))
		    != (ElementPtr1->state_flags & (GOOD_GUY | BAD_GUY)))
	    {
		ElementPtr0->turn_wait =
			MAKE_BYTE (5, HINIBBLE (ElementPtr0->turn_wait));
		ElementPtr0->thrust_wait &=
			~((SHIP_AT_MAX_SPEED | SHIP_BEYOND_MAX_SPEED) >> 6);
		ElementPtr0->state_flags |= COLLISION;
	    }

	    if (GRAVITY_MASS (ElementPtr1->mass_points))
	    {
		ElementPtr0->state_flags |= NONSOLID | FINITE_LIFE;
		ElementPtr0->hit_points = 0;
		ElementPtr0->life_span = 0;
	    }
	    else if ((ElementPtr1->state_flags & PLAYER_SHIP)
		    && ((ElementPtr1->state_flags & FINITE_LIFE)
		    || ElementPtr1->life_span == NORMAL_LIFE))
	    {
		ElementPtr1->state_flags &= ~COLLISION;

		if (!(ElementPtr0->state_flags & COLLISION))
		{
		    DeltaCrew (ElementPtr1, 1);

		    ElementPtr0->state_flags |=
			    DISAPPEARING | NONSOLID | FINITE_LIFE;
		    ElementPtr0->hit_points = 0;
		    ElementPtr0->life_span = 0;
		}
		else if ((ElementPtr0->state_flags & IGNORE_SIMILAR)
			&& ElementPtr1->crew_level
#ifdef NEVER
			&& (BYTE)random () <= (0x0100 / 3)
#endif /* NEVER */
			)
		{
		    STAMP	s;
		    STARSHIPPTR	StarShipPtr;

		    GetElementStarShip (ElementPtr0, &StarShipPtr);
		    if (!DeltaCrew (ElementPtr1, -1))
			ElementPtr1->life_span = 0;
		    else
		    {
			ElementPtr0->turn_wait = count_marines (StarShipPtr, TRUE);
			ElementPtr0->thrust_wait = MARINE_WAIT;
			ElementPtr0->next.image.frame = SetAbsFrameIndex (
				ElementPtr0->next.image.farray[0],
				22 + ElementPtr0->turn_wait
				);
			ElementPtr0->state_flags |= NONSOLID;
			ElementPtr0->state_flags &= ~CREW_OBJECT;
			SetPrimType (&(GLOBAL (DisplayArray))[
				ElementPtr0->PrimIndex
				], NO_PRIM);
			ElementPtr0->preprocess_func = intruder_preprocess;

			s.origin.x = 16 + (ElementPtr0->turn_wait & 3) * 9;
			s.origin.y = 14 + (ElementPtr0->turn_wait >> 2) * 11;
			s.frame = ElementPtr0->next.image.frame;
			ModifySilhouette (ElementPtr1, &s, 0);
		    }

		    ProcessSound (SetAbsSoundIndex (
			    StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 2));
		}

		ElementPtr0->state_flags &= ~COLLISION;
	    }
        }
    }
    POP_CONTEXT
}

static void	far
animate (ElementPtr)
LPELEMENT	ElementPtr;
{
    PUSH_CONTEXT
    {
	if (ElementPtr->turn_wait > 0)
	    --ElementPtr->turn_wait;
	else
	{
	    ElementPtr->next.image.frame =
		    IncFrameIndex (ElementPtr->current.image.frame);
	    ElementPtr->state_flags |= CHANGING;

	    ElementPtr->turn_wait = ElementPtr->next_turn;
	}
    }
    POP_CONTEXT
}

static void	far
turret_postprocess (ElementPtr)
LPELEMENT	ElementPtr;
{
    PUSH_CONTEXT
    {
	if (ElementPtr->life_span == 0)
	{
	    STARSHIPPTR	StarShipPtr;

	    SetPrimType (&(GLOBAL (DisplayArray))[
		    ElementPtr->PrimIndex
		    ], NO_PRIM);

	    GetElementStarShip (ElementPtr, &StarShipPtr);
	    if (StarShipPtr->hShip)
	    {
		COUNT		facing;
		HELEMENT	hTurret, hSpaceMarine;
		ELEMENTPTR	ShipPtr;

		LockElement (StarShipPtr->hShip, &ShipPtr);
		if (hTurret = AllocElement ())
		{
		    ELEMENTPTR	TurretPtr;

		    LockElement (hTurret, &TurretPtr);
		    TurretPtr->state_flags =
			    FINITE_LIFE | NONSOLID | IGNORE_SIMILAR
			    | CHANGING | PRE_PROCESS | POST_PROCESS
			    | (ElementPtr->state_flags & (GOOD_GUY | BAD_GUY));
		    TurretPtr->life_span = 1;
		    TurretPtr->current.image = ElementPtr->current.image;
		    TurretPtr->current.location = ShipPtr->next.location;
		    TurretPtr->turn_wait = ElementPtr->turn_wait;
		    TurretPtr->thrust_wait = ElementPtr->thrust_wait;

		    if (TurretPtr->turn_wait)
			--TurretPtr->turn_wait;
		    else if ((StarShipPtr->cur_status_flags & SPECIAL)
			    && (StarShipPtr->cur_status_flags & (LEFT | RIGHT)))
		    {
			if (StarShipPtr->cur_status_flags & RIGHT)
			    ++TurretPtr->thrust_wait;
			else
			    --TurretPtr->thrust_wait;

			TurretPtr->turn_wait = TURRET_WAIT;
		    }
		    facing = NORMALIZE_FACING (StarShipPtr->ShipFacing
			    + TurretPtr->thrust_wait);
		    StarShipPtr->RaceDescPtr->ship_info.ship_flags &=
			    ~(FIRES_FORE | FIRES_RIGHT | FIRES_AFT | FIRES_LEFT);
		    StarShipPtr->RaceDescPtr->ship_info.ship_flags |= FIRES_FORE
			    << (NORMALIZE_FACING (facing + ANGLE_TO_FACING (OCTANT))
			    / ANGLE_TO_FACING (QUADRANT));
		    TurretPtr->current.image.frame = SetAbsFrameIndex (
			    TurretPtr->current.image.frame, facing
			    );
		    facing = FACING_TO_ANGLE (facing);
		    if (StarShipPtr->cur_status_flags & WEAPON)
		    {
			HELEMENT	hTurretEffect;
			ELEMENTPTR	TurretEffectPtr;

			LockElement (GetTailElement (), &TurretEffectPtr);
			if ((LPELEMENT)TurretEffectPtr != ElementPtr
				&& (TurretEffectPtr->state_flags & (GOOD_GUY | BAD_GUY)) ==
				(ElementPtr->state_flags & (GOOD_GUY | BAD_GUY))
				&& (TurretEffectPtr->state_flags & APPEARING)
				&& GetPrimType (&(GLOBAL (DisplayArray))[
					TurretEffectPtr->PrimIndex
					]) == STAMP_PRIM
				&& (hTurretEffect = AllocElement ())
				)
			{
			    TurretPtr->current.location.x -=
				    COSINE (facing, DISPLAY_TO_WORLD (2));
			    TurretPtr->current.location.y -=
				    SINE (facing, DISPLAY_TO_WORLD (2));

			    LockElement (hTurretEffect, &TurretEffectPtr);
			    TurretEffectPtr->state_flags =
				    FINITE_LIFE | NONSOLID | IGNORE_SIMILAR | APPEARING
				    | (ElementPtr->state_flags & (GOOD_GUY | BAD_GUY));
			    TurretEffectPtr->life_span = 4;

			    TurretEffectPtr->current.location.x =
				    TurretPtr->current.location.x
				    + COSINE (facing,
				    DISPLAY_TO_WORLD (TURRET_OFFSET));
			    TurretEffectPtr->current.location.y =
				    TurretPtr->current.location.y
				    + SINE (facing,
				    DISPLAY_TO_WORLD (TURRET_OFFSET));
			    TurretEffectPtr->current.image.farray =
				    StarShipPtr->RaceDescPtr->ship_data.special;
			    TurretEffectPtr->current.image.frame = SetAbsFrameIndex (
				    StarShipPtr->RaceDescPtr->ship_data.special[0],
				    ANGLE_TO_FACING (FULL_CIRCLE)
				    );

			    TurretEffectPtr->preprocess_func = animate;

			    SetElementStarShip (TurretEffectPtr, StarShipPtr);

			    SetPrimType (&(GLOBAL (DisplayArray))[
				    TurretEffectPtr->PrimIndex
				    ], STAMP_PRIM);

			    UnlockElement (hTurretEffect);
			    PutElement (hTurretEffect);
			}
			UnlockElement (GetTailElement ());
		    }
		    TurretPtr->next = TurretPtr->current;

		    SetPrimType (&(GLOBAL (DisplayArray))[
			    TurretPtr->PrimIndex
			    ],
			    GetPrimType (&(GLOBAL (DisplayArray))[
			    ShipPtr->PrimIndex
			    ]));
		    SetPrimColor (&(GLOBAL (DisplayArray))[
			    TurretPtr->PrimIndex
			    ],
			    GetPrimColor (&(GLOBAL (DisplayArray))[
			    ShipPtr->PrimIndex
			    ]));

		    TurretPtr->postprocess_func =
			    ElementPtr->postprocess_func;

		    SetElementStarShip (TurretPtr, StarShipPtr);

		    UnlockElement (hTurret);
		    InsertElement (hTurret, GetSuccElement (ElementPtr));
		}

		if (StarShipPtr->special_counter == 0
			&& (StarShipPtr->cur_status_flags & SPECIAL)
			&& (StarShipPtr->cur_status_flags & WEAPON)
			&& ShipPtr->crew_level > 1
			&& count_marines (StarShipPtr, FALSE) < MAX_MARINES
			&& TrackShip (ShipPtr, &facing) >= 0
			&& (hSpaceMarine = AllocElement ()))
		{
		    ELEMENTPTR	SpaceMarinePtr;

		    LockElement (hSpaceMarine, &SpaceMarinePtr);
		    SpaceMarinePtr->state_flags =
			    IGNORE_SIMILAR | APPEARING | CREW_OBJECT
			    | (ElementPtr->state_flags & (GOOD_GUY | BAD_GUY));
		    SpaceMarinePtr->life_span = NORMAL_LIFE;
		    SpaceMarinePtr->hit_points = 3;
		    SpaceMarinePtr->mass_points = 1;

		    facing = FACING_TO_ANGLE (StarShipPtr->ShipFacing);
		    SpaceMarinePtr->current.location.x =
			    ShipPtr->current.location.x
			    - COSINE (facing,
			    DISPLAY_TO_WORLD (TURRET_OFFSET));
		    SpaceMarinePtr->current.location.y =
			    ShipPtr->current.location.y
			    - SINE (facing,
			    DISPLAY_TO_WORLD (TURRET_OFFSET));
		    SpaceMarinePtr->current.image.farray =
			    StarShipPtr->RaceDescPtr->ship_data.special;
		    SpaceMarinePtr->current.image.frame = SetAbsFrameIndex (
			    StarShipPtr->RaceDescPtr->ship_data.special[0], 20
			    );

		    SpaceMarinePtr->turn_wait = MAKE_BYTE (0,
			    NORMALIZE_FACING (
			    ANGLE_TO_FACING (facing + HALF_CIRCLE)
			    ));
		    SpaceMarinePtr->preprocess_func = marine_preprocess;
		    SpaceMarinePtr->collision_func = marine_collision;

		    SetElementStarShip (SpaceMarinePtr, StarShipPtr);

		    SetPrimType (&(GLOBAL (DisplayArray))[
			    SpaceMarinePtr->PrimIndex
			    ], STAMP_PRIM);

		    UnlockElement (hSpaceMarine);
		    PutElement (hSpaceMarine);

		    DeltaCrew (ShipPtr, -1);
		    ProcessSound (SetAbsSoundIndex (
			    StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 1));

		    StarShipPtr->special_counter = 
			    StarShipPtr->RaceDescPtr->characteristics.special_wait;
		}

		UnlockElement (StarShipPtr->hShip);
	    }
	}
    }
    POP_CONTEXT
}

static void	far
orz_preprocess (ElementPtr)
LPELEMENT	ElementPtr;
{
    PUSH_CONTEXT
    {
	STARSHIPPTR	StarShipPtr;

	GetElementStarShip (ElementPtr, &StarShipPtr);
	if (!(ElementPtr->state_flags & APPEARING))
	{
	    if (((StarShipPtr->cur_status_flags
		    | StarShipPtr->old_status_flags) & SPECIAL)
		    && (StarShipPtr->cur_status_flags & (LEFT | RIGHT))
		    && ElementPtr->turn_wait == 0)
	    {
		++ElementPtr->turn_wait;
	    }

	    if ((StarShipPtr->cur_status_flags & SPECIAL)
		    && (StarShipPtr->cur_status_flags & WEAPON)
		    && StarShipPtr->weapon_counter == 0)
	    {
		++StarShipPtr->weapon_counter;
	    }
	}
	else
	{
	    HELEMENT	hTurret;

	    if (hTurret = AllocElement ())
	    {
		ELEMENTPTR	TurretPtr;

		LockElement (hTurret, &TurretPtr);
		TurretPtr->state_flags =
			FINITE_LIFE | NONSOLID | IGNORE_SIMILAR
			| (ElementPtr->state_flags & (GOOD_GUY | BAD_GUY));
		TurretPtr->life_span = 1;
		TurretPtr->current.image.farray = StarShipPtr->RaceDescPtr->ship_data.special;
		TurretPtr->current.image.frame =
			SetAbsFrameIndex (StarShipPtr->RaceDescPtr->ship_data.special[0],
			StarShipPtr->ShipFacing);

		TurretPtr->postprocess_func = turret_postprocess;

		SetElementStarShip (TurretPtr, StarShipPtr);

		UnlockElement (hTurret);
		InsertElement (hTurret, GetSuccElement (ElementPtr));
	    }
	}
    }
    POP_CONTEXT
}

RACE_DESCPTR	far
init_orz ()
{
    RACE_DESCPTR	RaceDescPtr;

    PUSH_CONTEXT
    {
	orz_desc.preprocess_func = orz_preprocess;
	orz_desc.init_weapon_func = initialize_turret_missile;
	orz_desc.cyborg_control.intelligence_func =
		(PROC_PARAMETER(
		void (far *), (ShipPtr, ObjectsOfConcern, ConcernCounter),
		    ARG		(LPVOID		ShipPtr)
		    ARG		(LPVOID		ObjectsOfConcern)
		    ARG_END	(COUNT		ConcernCounter)
		)) orz_intelligence;

	RaceDescPtr = &orz_desc;
    }
    POP_CONTEXT

    return (RaceDescPtr);
}

