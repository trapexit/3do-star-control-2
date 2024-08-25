#include "reslib.h"
#include "resinst.h"
#include "starcon.h"

#define MAX_CREW		6
#define MAX_ENERGY		4
#define ENERGY_REGENERATION	1
#define WEAPON_ENERGY_COST	1
#define SPECIAL_ENERGY_COST	0
#define ENERGY_WAIT		9
#define MAX_THRUST		35
#define THRUST_INCREMENT	5
#define TURN_WAIT		1
#define THRUST_WAIT		0
#define WEAPON_WAIT		3
#define SPECIAL_WAIT		0

#define SHIP_MASS		1
#define MISSILE_SPEED		DISPLAY_TO_WORLD (24)
#define MISSILE_LIFE		10

static RACE_DESC	shofixti_desc =
{
    {
	FIRES_FORE,
	5,					/* Super Melee cost */
	0 / SPHERE_RADIUS_INCREMENT,		/* Initial sphere of influence radius */
	MAX_CREW, MAX_CREW,
	MAX_ENERGY, MAX_ENERGY,
	{
	    0, 0,
	},
	(STRING)SHOFIXTI_RACE_STRINGS,
	(FRAME)SHOFIXTI_ICON_MASK_PMAP_ANIM,
	(FRAME)SHOFIXTI_MICON_MASK_PMAP_ANIM,
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
	    (FRAME)SHOFIXTI_BIG_MASK_PMAP_ANIM,
	    (FRAME)SHOFIXTI_MED_MASK_PMAP_ANIM,
	    (FRAME)SHOFIXTI_SML_MASK_PMAP_ANIM,
	},
	{
	    (FRAME)DART_BIG_MASK_PMAP_ANIM,
	    (FRAME)DART_MED_MASK_PMAP_ANIM,
	    (FRAME)DART_SML_MASK_PMAP_ANIM,
	},
	{
	    (FRAME)DESTRUCT_BIG_MASK_ANIM,
	    (FRAME)DESTRUCT_MED_MASK_ANIM,
	    (FRAME)DESTRUCT_SML_MASK_ANIM,
	},
	{
	    (FRAME)SHOFIXTI_CAPTAIN_MASK_PMAP_ANIM,
	    (FRAME)0,
	    (FRAME)0,
	    (FRAME)0,
	    (FRAME)0,
	},
	(SOUND)SHOFIXTI_VICTORY_SONG,
	(SOUND)SHOFIXTI_SHIP_SOUNDS,
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

static COUNT	far
initialize_standard_missile (ShipPtr, MissileArray)
LPELEMENT	ShipPtr;
HELEMENT	far MissileArray[];
{
#define SHOFIXTI_OFFSET	15
#define MISSILE_HITS		1
#define MISSILE_DAMAGE		1
#define MISSILE_OFFSET		1
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
	MissileBlock.pixoffs = SHOFIXTI_OFFSET;
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
destruct_preprocess (ElementPtr)
LPELEMENT	ElementPtr;
{
#define DESTRUCT_SWITCH	((NUM_EXPLOSION_FRAMES * 3) - 3)
    PUSH_CONTEXT
    {
	LPPRIMITIVE	lpPrim;

	lpPrim = &(GLOBAL (DisplayArray))[ElementPtr->PrimIndex];
	ElementPtr->state_flags |= CHANGING;
	if (ElementPtr->life_span > DESTRUCT_SWITCH)
	{
	    SetPrimType (lpPrim, STAMPFILL_PRIM);
	    if (ElementPtr->life_span == DESTRUCT_SWITCH + 2)
		SetPrimColor (lpPrim, BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1F, 0xA), 0x0E));
	    else
		SetPrimColor (lpPrim, BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1F, 0x1F), 0x0F));
	}
	else if (ElementPtr->life_span < DESTRUCT_SWITCH)
	{
	    ElementPtr->next.image.frame =
		    IncFrameIndex (ElementPtr->current.image.frame);
	    if (GetPrimColor (lpPrim) == BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1F, 0x1F), 0x0F))
		SetPrimColor (lpPrim, BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1F, 0xA), 0x0E));
	    else if (GetPrimColor (lpPrim) == BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1F, 0xA), 0x0E))
		SetPrimColor (lpPrim, BUILD_COLOR (MAKE_RGB15 (0x1F, 0xA, 0xA), 0x0C));
	    else if (GetPrimColor (lpPrim) == BUILD_COLOR (MAKE_RGB15 (0x1F, 0xA, 0xA), 0x0C))
		SetPrimColor (lpPrim, BUILD_COLOR (MAKE_RGB15 (0x14, 0xA, 0x00), 0x06));
	    else if (GetPrimColor (lpPrim) == BUILD_COLOR (MAKE_RGB15 (0x14, 0xA, 0x00), 0x06))
		SetPrimColor (lpPrim, BUILD_COLOR (MAKE_RGB15 (0x14, 0x00, 0x00), 0x04));
	}
	else
	{
	    HELEMENT	hDestruct;

	    SetPrimType (lpPrim, NO_PRIM);
	    ElementPtr->preprocess_func = NULL_PTR;

	    if (hDestruct = AllocElement ())
	    {
		ELEMENTPTR	DestructPtr;
		STARSHIPPTR	StarShipPtr;

		GetElementStarShip (ElementPtr, &StarShipPtr);

		PutElement (hDestruct);
		LockElement (hDestruct, &DestructPtr);
		SetElementStarShip (DestructPtr, StarShipPtr);
		DestructPtr->hit_points = DestructPtr->mass_points = 0;
		DestructPtr->state_flags = APPEARING | FINITE_LIFE | NONSOLID;
		DestructPtr->life_span = (NUM_EXPLOSION_FRAMES - 3) - 1;
		SetPrimType (
			&(GLOBAL (DisplayArray))[DestructPtr->PrimIndex],
			STAMPFILL_PRIM
			);
		SetPrimColor (
			&(GLOBAL (DisplayArray))[DestructPtr->PrimIndex],
			BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1F, 0x1F), 0x0F)
			);
		DestructPtr->current.image.farray = StarShipPtr->RaceDescPtr->ship_data.special;
		DestructPtr->current.image.frame = StarShipPtr->RaceDescPtr->ship_data.special[0];
		DestructPtr->current.location = ElementPtr->current.location;
		{
		    DestructPtr->preprocess_func = destruct_preprocess;
		}
		DestructPtr->postprocess_func =
			DestructPtr->death_func = NULL_PTR;
		ZeroVelocityComponents (&DestructPtr->velocity);
		UnlockElement (hDestruct);
	    }
	}
    }
    POP_CONTEXT
}

static void	far
self_destruct (ElementPtr)
LPELEMENT	ElementPtr;
{
    PUSH_CONTEXT
    {
	STARSHIPPTR	StarShipPtr;

	GetElementStarShip (ElementPtr, &StarShipPtr);
	if (ElementPtr->state_flags & PLAYER_SHIP)
	{
	    HELEMENT	hDestruct;
	    
	    if (hDestruct = AllocElement ())
	    {
		ELEMENTPTR	DestructPtr;

		LockElement (hDestruct, &DestructPtr);
		DestructPtr->state_flags = APPEARING | NONSOLID | FINITE_LIFE |
			(ElementPtr->state_flags & (GOOD_GUY | BAD_GUY));
		DestructPtr->next.location = ElementPtr->next.location;
		DestructPtr->life_span = 0;
		DestructPtr->pParent = ElementPtr->pParent;
		DestructPtr->hTarget = 0;

		DestructPtr->death_func = self_destruct;

		UnlockElement (hDestruct);

		PutElement (hDestruct);
	    }

	    ElementPtr->state_flags |= NONSOLID;
	    ElementPtr->life_span = 0;

	    ElementPtr->preprocess_func = destruct_preprocess;
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

		if (CollidingElement (ObjPtr))
		{
#define DESTRUCT_RANGE		180
		    SIZE	delta_x, delta_y;
		    DWORD	dist;

		    if ((delta_x = ObjPtr->next.location.x
			    - ElementPtr->next.location.x) < 0)
			delta_x = -delta_x;
		    if ((delta_y = ObjPtr->next.location.y
			    - ElementPtr->next.location.y) < 0)
			delta_y = -delta_y;
		    delta_x = WORLD_TO_DISPLAY (delta_x);
		    delta_y = WORLD_TO_DISPLAY (delta_y);
		    if (delta_x <= DESTRUCT_RANGE && delta_y <= DESTRUCT_RANGE
			    && (dist = (DWORD)(delta_x * delta_x)
			    + (DWORD)(delta_y * delta_y)) <=
			    (DWORD)(DESTRUCT_RANGE * DESTRUCT_RANGE))
		    {
#define MAX_DESTRUCTION	(DESTRUCT_RANGE / 10)
			SIZE	destruction;

			destruction = ((MAX_DESTRUCTION
				* (DESTRUCT_RANGE - square_root (dist)))
				/ DESTRUCT_RANGE) + 1;

			if (ObjPtr->state_flags & PLAYER_SHIP)
			{
			    if (!DeltaCrew (ObjPtr, -destruction))
				ObjPtr->life_span = 0;
			}
			else if (!GRAVITY_MASS (ObjPtr->mass_points))
			{
			    if ((BYTE)destruction < ObjPtr->hit_points)
				ObjPtr->hit_points -= (BYTE)destruction;
			    else
			    {
				ObjPtr->hit_points = 0;
				ObjPtr->life_span = 0;
			    }
			}
		    }
		}

		UnlockElement (hElement);
	    }
	}
    }
    POP_CONTEXT
}

static void	 far
shofixti_intelligence (ShipPtr, ObjectsOfConcern, ConcernCounter)
LPELEMENT	ShipPtr;
LPEVALUATE_DESC	ObjectsOfConcern;
COUNT		ConcernCounter;
{
    PUSH_CONTEXT
    {
	STARSHIPPTR	StarShipPtr;

	ship_intelligence (ShipPtr,
		ObjectsOfConcern, ConcernCounter);

	GetElementStarShip (ShipPtr, &StarShipPtr);
	if (StarShipPtr->special_counter == 0)
	{
	    if (StarShipPtr->ship_input_state & SPECIAL)
		StarShipPtr->ship_input_state &= ~SPECIAL;
	    else
	    {
		register LPEVALUATE_DESC	lpWeaponEvalDesc, lpShipEvalDesc;

		lpWeaponEvalDesc = &ObjectsOfConcern[ENEMY_WEAPON_INDEX];
		lpShipEvalDesc = &ObjectsOfConcern[ENEMY_SHIP_INDEX];
		if (StarShipPtr->RaceDescPtr->ship_data.special[0]
			&& (GetFrameCount (StarShipPtr->RaceDescPtr->ship_data.captain_control.special)
			- GetFrameIndex (StarShipPtr->RaceDescPtr->ship_data.captain_control.special) > 5
			|| (lpShipEvalDesc->ObjectPtr != NULL_PTR
			&& lpShipEvalDesc->which_turn <= 4)
			|| (lpWeaponEvalDesc->ObjectPtr != NULL_PTR
				    /* means IMMEDIATE WEAPON */
			&& (((lpWeaponEvalDesc->ObjectPtr->state_flags & PLAYER_SHIP)
			&& ShipPtr->crew_level == 1)
			|| (PlotIntercept (lpWeaponEvalDesc->ObjectPtr, ShipPtr, 2, 0)
			&& lpWeaponEvalDesc->ObjectPtr->mass_points >= ShipPtr->crew_level
			&& (random () & 1))))))
		    StarShipPtr->ship_input_state |= SPECIAL;
	    }
	}
    }
    POP_CONTEXT
}

static void	far
shofixti_postprocess (ElementPtr)
LPELEMENT	ElementPtr;
{
    PUSH_CONTEXT
    {
	STARSHIPPTR	StarShipPtr;

	GetElementStarShip (ElementPtr, &StarShipPtr);
	if ((StarShipPtr->cur_status_flags
		^ StarShipPtr->old_status_flags) & SPECIAL)
	{
	    StarShipPtr->RaceDescPtr->ship_data.captain_control.special =
		    IncFrameIndex (StarShipPtr->RaceDescPtr->ship_data.captain_control.special);
	    if (GetFrameCount (StarShipPtr->RaceDescPtr->ship_data.captain_control.special)
		    - GetFrameIndex (StarShipPtr->RaceDescPtr->ship_data.captain_control.special) == 3)
		self_destruct (ElementPtr);
	}
    }
    POP_CONTEXT
}

RACE_DESCPTR	far
init_shofixti ()
{
    RACE_DESCPTR	RaceDescPtr;

    PUSH_CONTEXT
    {
	static RACE_DESC	new_shofixti_desc;

	shofixti_desc.postprocess_func = shofixti_postprocess;
	shofixti_desc.init_weapon_func = initialize_standard_missile;
	shofixti_desc.cyborg_control.intelligence_func =
		(PROC_PARAMETER(
		void (far *), (ShipPtr, ObjectsOfConcern, ConcernCounter),
		    ARG		(LPVOID		ShipPtr)
		    ARG		(LPVOID		ObjectsOfConcern)
		    ARG_END	(COUNT		ConcernCounter)
		)) shofixti_intelligence;

	new_shofixti_desc = shofixti_desc;
	if (LOBYTE (GLOBAL (CurrentActivity)) == IN_ENCOUNTER
		&& !GET_GAME_STATE (SHOFIXTI_RECRUITED))
	{
#define NUM_LIMPETS	3
	    COUNT	i;

	    new_shofixti_desc.ship_data.ship[0] = (FRAME)OLDSHOF_BIG_MASK_PMAP_ANIM;
	    new_shofixti_desc.ship_data.ship[1] = (FRAME)OLDSHOF_MED_MASK_PMAP_ANIM;
	    new_shofixti_desc.ship_data.ship[2] = (FRAME)OLDSHOF_SML_MASK_PMAP_ANIM;
	    new_shofixti_desc.ship_data.special[0] =
		    new_shofixti_desc.ship_data.special[1] =
		    new_shofixti_desc.ship_data.special[2] = (FRAME)0;
	    new_shofixti_desc.ship_data.captain_control.background =
		    (FRAME)OLDSHOF_CAPTAIN_MASK_PMAP_ANIM;

		    /* weapon doesn't work as well */
	    new_shofixti_desc.characteristics.weapon_wait = 10;
		    /* simulate VUX limpets */
	    for (i = 0; i < NUM_LIMPETS; ++i)
	    {
		if (++new_shofixti_desc.characteristics.turn_wait == 0)
		    --new_shofixti_desc.characteristics.turn_wait;
		if (++new_shofixti_desc.characteristics.thrust_wait == 0)
		    --new_shofixti_desc.characteristics.thrust_wait;
#define MIN_THRUST_INCREMENT	DISPLAY_TO_WORLD (1)
		if (new_shofixti_desc.characteristics.thrust_increment <= MIN_THRUST_INCREMENT)
		{
		    new_shofixti_desc.characteristics.max_thrust =
			    new_shofixti_desc.characteristics.thrust_increment << 1;
		}
		else
		{
		    COUNT	num_thrusts;

		    num_thrusts = new_shofixti_desc.characteristics.max_thrust /
			    new_shofixti_desc.characteristics.thrust_increment;
		    --new_shofixti_desc.characteristics.thrust_increment;
		    new_shofixti_desc.characteristics.max_thrust =
			    new_shofixti_desc.characteristics.thrust_increment * num_thrusts;
		}
	    }
	}

	RaceDescPtr = &new_shofixti_desc;
    }
    POP_CONTEXT

    return (RaceDescPtr);
}

