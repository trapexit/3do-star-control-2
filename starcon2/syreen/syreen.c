#include "reslib.h"
#include "resinst.h"
#include "starcon.h"

#define SYREEN_MAX_CREW_SIZE	MAX_CREW_SIZE
#define MAX_CREW		12
#define MAX_ENERGY		16
#define ENERGY_REGENERATION	1
#define WEAPON_ENERGY_COST	1
#define SPECIAL_ENERGY_COST	5
#define ENERGY_WAIT		6
#define MAX_THRUST		/* DISPLAY_TO_WORLD (8) */ 36
#define THRUST_INCREMENT	/* DISPLAY_TO_WORLD (2) */ 9
#define TURN_WAIT		1
#define THRUST_WAIT		1
#define WEAPON_WAIT		8
#define SPECIAL_WAIT		20

#define SHIP_MASS		2
#define MISSILE_SPEED		DISPLAY_TO_WORLD (30)
#define MISSILE_LIFE		10

static RACE_DESC	syreen_desc =
{
    {
	FIRES_FORE,
	13,					/* Super Melee cost */
	0 / SPHERE_RADIUS_INCREMENT,		/* Initial sphere of influence radius */
	MAX_CREW, SYREEN_MAX_CREW_SIZE,
	MAX_ENERGY, MAX_ENERGY,
	{
	    0, 0,
	},
	(STRING)SYREEN_RACE_STRINGS,
	(FRAME)SYREEN_ICON_MASK_PMAP_ANIM,
	(FRAME)SYREEN_MICON_MASK_PMAP_ANIM,
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
	    (FRAME)SYREEN_BIG_MASK_PMAP_ANIM,
	    (FRAME)SYREEN_MED_MASK_PMAP_ANIM,
	    (FRAME)SYREEN_SML_MASK_PMAP_ANIM,
	},
	{
	    (FRAME)DAGGER_BIG_MASK_PMAP_ANIM,
	    (FRAME)DAGGER_MED_MASK_PMAP_ANIM,
	    (FRAME)DAGGER_SML_MASK_PMAP_ANIM,
	},
	{
	    (FRAME)0,
	    (FRAME)0,
	    (FRAME)0,
	},
	{
	    (FRAME)SYREEN_CAPTAIN_MASK_PMAP_ANIM,
	    (FRAME)0,
	    (FRAME)0,
	    (FRAME)0,
	    (FRAME)0,
	},
	(SOUND)SYREEN_VICTORY_SONG,
	(SOUND)SYREEN_SHIP_SOUNDS,
    },
    {
	0,
	(MISSILE_SPEED * MISSILE_LIFE * 2 / 3),
	NULL_PTR,
    },
    NULL_PTR,
    NULL_PTR,
    NULL_PTR,
    0,
};

static COUNT	far
initialize_dagger (ShipPtr, DaggerArray)
LPELEMENT	ShipPtr;
HELEMENT	far DaggerArray[];
{
#define SYREEN_OFFSET		30
#define MISSILE_HITS		1
#define MISSILE_DAMAGE		2
#define MISSILE_OFFSET		3
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
	MissileBlock.pixoffs = SYREEN_OFFSET;
	MissileBlock.speed = MISSILE_SPEED;
	MissileBlock.hit_points = MISSILE_HITS;
	MissileBlock.damage = MISSILE_DAMAGE;
	MissileBlock.life = MISSILE_LIFE;
	MissileBlock.preprocess_func = NULL_PTR;
	MissileBlock.blast_offs = MISSILE_OFFSET;
	DaggerArray[0] = initialize_missile (&MissileBlock);
    }
    POP_CONTEXT

    return (1);
}

static void	far
spawn_crew (ElementPtr)
LPELEMENT	ElementPtr;
{
    PUSH_CONTEXT
    {
	if (ElementPtr->state_flags & PLAYER_SHIP)
	{
	    HELEMENT	hCrew;

	    if ((hCrew = AllocElement ()) != 0)
	    {
		ELEMENTPTR	CrewPtr;

		LockElement (hCrew, &CrewPtr);
		CrewPtr->next.location = ElementPtr->next.location;
		CrewPtr->state_flags = APPEARING | NONSOLID | FINITE_LIFE
			| (ElementPtr->state_flags & (GOOD_GUY | BAD_GUY));
		CrewPtr->life_span = 0;
		{
		    CrewPtr->death_func = spawn_crew;
		}
		CrewPtr->pParent = ElementPtr->pParent;
		CrewPtr->hTarget = 0;
		UnlockElement (hCrew);

		PutElement (hCrew);
	    }
	}
	else
	{
	    HELEMENT	hElement, hNextElement;

	    for (hElement = GetHeadElement ();
		    hElement != 0; hElement = hNextElement)
	    {
		ELEMENTPTR	ObjPtr;

		LockElement (hElement, &ObjPtr);
		hNextElement = GetSuccElement (ObjPtr);

		if ((ObjPtr->state_flags & PLAYER_SHIP)
			&& (ObjPtr->state_flags & (GOOD_GUY | BAD_GUY)) !=
			(ElementPtr->state_flags & (GOOD_GUY | BAD_GUY))
			&& ObjPtr->crew_level > 1)
		{
		    SIZE	dx, dy;
		    DWORD	d_squared;

		    if ((dx = ObjPtr->next.location.x
			    - ElementPtr->next.location.x) < 0)
			dx = -dx;
		    if ((dy = ObjPtr->next.location.y
			    - ElementPtr->next.location.y) < 0)
			dy = -dy;
		    dx = WORLD_TO_DISPLAY (dx);
		    dy = WORLD_TO_DISPLAY (dy);
		    if (dx <= SPACE_HEIGHT && dy <= SPACE_HEIGHT
			    && (d_squared = (DWORD)((UWORD)dx * (UWORD)dx)
			    + (DWORD)((UWORD)dy * (UWORD)dy)) <=
			    (DWORD)((UWORD)SPACE_HEIGHT * (UWORD)SPACE_HEIGHT))
		    {
#define MAX_ABANDONERS	8
			COUNT	crew_loss;

			crew_loss = ((MAX_ABANDONERS
				* (SPACE_HEIGHT - square_root (d_squared)))
				/ SPACE_HEIGHT) + 1;
			if (crew_loss >= ObjPtr->crew_level)
			    crew_loss = ObjPtr->crew_level - 1;

			AbandonShip (ObjPtr, ElementPtr, crew_loss);
		    }
		}

		UnlockElement (hElement);
	    }
	}
    }
    POP_CONTEXT
}

static void	 far
syreen_intelligence (ShipPtr, ObjectsOfConcern, ConcernCounter)
LPELEMENT	ShipPtr;
LPEVALUATE_DESC	ObjectsOfConcern;
COUNT		ConcernCounter;
{
    PUSH_CONTEXT
    {
	LPEVALUATE_DESC	lpEvalDesc;

	ship_intelligence (ShipPtr,
		ObjectsOfConcern, ConcernCounter);

	lpEvalDesc = &ObjectsOfConcern[ENEMY_SHIP_INDEX];
	if (lpEvalDesc->ObjectPtr != NULL_PTR)
	{
	    STARSHIPPTR	StarShipPtr, EnemyStarShipPtr;

	    GetElementStarShip (ShipPtr, &StarShipPtr);
	    GetElementStarShip (lpEvalDesc->ObjectPtr, &EnemyStarShipPtr);
	    if (!(EnemyStarShipPtr->RaceDescPtr->ship_info.ship_flags & CREW_IMMUNE)
		    && StarShipPtr->special_counter == 0
		    && lpEvalDesc->ObjectPtr->crew_level > 1
		    && lpEvalDesc->which_turn <= 14)
		StarShipPtr->ship_input_state |= SPECIAL;
	    else
		StarShipPtr->ship_input_state &= ~SPECIAL;
	}
    }
    POP_CONTEXT
}

static void	far
syreen_postprocess (ElementPtr)
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
			    /* SYREEN_SONG */
		    StarShipPtr->RaceDescPtr->ship_data.ship_sounds, 1));
	    spawn_crew (ElementPtr);

	    StarShipPtr->special_counter =
		    StarShipPtr->RaceDescPtr->characteristics.special_wait;
	}
    }
    POP_CONTEXT
}

RACE_DESCPTR	far
init_syreen ()
{
    RACE_DESCPTR	RaceDescPtr;

    PUSH_CONTEXT
    {
	syreen_desc.postprocess_func = syreen_postprocess;
	syreen_desc.init_weapon_func = initialize_dagger;
	syreen_desc.cyborg_control.intelligence_func =
		(PROC_PARAMETER(
		void (far *), (ShipPtr, ObjectsOfConcern, ConcernCounter),
		    ARG		(LPVOID		ShipPtr)
		    ARG		(LPVOID		ObjectsOfConcern)
		    ARG_END	(COUNT		ConcernCounter)
		)) syreen_intelligence;

	RaceDescPtr = &syreen_desc;
    }
    POP_CONTEXT

    return (RaceDescPtr);
}

