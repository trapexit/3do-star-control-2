#include "starcon.h"

PROC(
void spawn_planet, (),
    ARG_VOID
)
{
    HELEMENT	hPlanetElement;
    
    if (hPlanetElement = AllocElement ())
    {
	POINT		loc;
	ELEMENTPTR	PlanetElementPtr;
	extern FRAME	planet[];

	LockElement (hPlanetElement, &PlanetElementPtr);
	PlanetElementPtr->hit_points = 200;
	PlanetElementPtr->state_flags = APPEARING;
	PlanetElementPtr->life_span = NORMAL_LIFE + 1;
	SetPrimType (&DisplayArray[PlanetElementPtr->PrimIndex], NO_PRIM);
	PlanetElementPtr->current.image.farray = planet;
	PlanetElementPtr->current.image.frame =
		SetAbsFrameIndex (PlanetElementPtr->current.image.farray[0], 1);
	PlanetElementPtr->collision_func = collision;
	PlanetElementPtr->postprocess_func =
		(PROC_PARAMETER(
		void (far *), (ElementPtr),
		    ARG_END	(struct element far	*ElementPtr)
		))CalculateGravity;
	ZeroVelocityComponents (&PlanetElementPtr->velocity);
	do
	{
	    PlanetElementPtr->current.location.x =
		    WRAP_X (DISPLAY_ALIGN_X (random ()));
	    PlanetElementPtr->current.location.y =
		    WRAP_Y (DISPLAY_ALIGN_Y (random ()));
	} while (CalculateGravity (PlanetElementPtr)
		|| TimeSpaceMatterConflict (PlanetElementPtr));
	loc = PlanetElementPtr->current.location;
	PlanetElementPtr->mass_points = PlanetElementPtr->hit_points;
	UnlockElement (hPlanetElement);

	PutElement (hPlanetElement);
	if (hPlanetElement = AllocElement ())
	{
	    LockElement (hPlanetElement, &PlanetElementPtr);
	    PlanetElementPtr->hit_points = 1;
	    PlanetElementPtr->mass_points = PlanetElementPtr->hit_points;
	    PlanetElementPtr->state_flags = APPEARING | NONSOLID;
	    PlanetElementPtr->life_span = NORMAL_LIFE + 1;
	    SetPrimType (&DisplayArray[PlanetElementPtr->PrimIndex], STAMP_PRIM);
	    PlanetElementPtr->current.image.farray = planet;
	    PlanetElementPtr->current.image.frame =
		    PlanetElementPtr->current.image.farray[0];
	    ZeroVelocityComponents (&PlanetElementPtr->velocity);
	    PlanetElementPtr->current.location = loc;
	    UnlockElement (hPlanetElement);

	    PutElement (hPlanetElement);
	}
    }
}

extern FRAME		asteroid[];

PROC(STATIC
void spawn_rubble, (AsteroidElementPtr),
    ARG_END	(LPELEMENT	AsteroidElementPtr)
)
{
    HELEMENT	hRubbleElement;
    
    hRubbleElement = AllocElement ();
    if (hRubbleElement)
    {
	ELEMENTPTR	RubbleElementPtr;

	PutElement (hRubbleElement);
	LockElement (hRubbleElement, &RubbleElementPtr);
	RubbleElementPtr->state_flags = APPEARING | FINITE_LIFE | NONSOLID;
	RubbleElementPtr->life_span = 5;
	RubbleElementPtr->turn_wait = RubbleElementPtr->next_turn = 0;
	SetPrimType (&DisplayArray[RubbleElementPtr->PrimIndex], STAMP_PRIM);
	RubbleElementPtr->current.image.farray = asteroid;
	RubbleElementPtr->current.image.frame =
		SetAbsFrameIndex (asteroid[0], ANGLE_TO_FACING (FULL_CIRCLE));
	RubbleElementPtr->current.location = AsteroidElementPtr->current.location;
	RubbleElementPtr->preprocess_func = animation_preprocess;
	RubbleElementPtr->death_func = spawn_asteroid;
	UnlockElement (hRubbleElement);
    }
}

PROC(STATIC
void asteroid_preprocess, (ElementPtr),
    ARG_END	(LPELEMENT	ElementPtr)
)
{
    if (ElementPtr->turn_wait > 0)
	--ElementPtr->turn_wait;
    else
    {
	COUNT	frame_index;

	frame_index = GetFrameIndex (ElementPtr->current.image.frame);
	if (ElementPtr->thrust_wait & (1 << 7))
	    --frame_index;
	else
	    ++frame_index;
	ElementPtr->next.image.frame =
		SetAbsFrameIndex (ElementPtr->current.image.frame,
		NORMALIZE_FACING (frame_index));
	ElementPtr->state_flags |= CHANGING;

	ElementPtr->turn_wait = ElementPtr->thrust_wait & ((1 << 7) - 1);
    }
}

PROC(
void spawn_asteroid, (ElementPtr),
    ARG_END	(LPELEMENT	ElementPtr)
)
{
    HELEMENT	hAsteroidElement;

    if ((hAsteroidElement = AllocElement ()) == 0)
    {
	if (ElementPtr != 0)
	{
	    ElementPtr->state_flags &= ~DISAPPEARING;
	    SetPrimType (&DisplayArray[ElementPtr->PrimIndex], NO_PRIM);
	    ElementPtr->life_span = 1;
	}
    }
    else
    {
	ELEMENTPTR	AsteroidElementPtr;
	COUNT		val;

	LockElement (hAsteroidElement, &AsteroidElementPtr);
	AsteroidElementPtr->hit_points = 1;
	AsteroidElementPtr->mass_points = 3;
	AsteroidElementPtr->state_flags = APPEARING;
	AsteroidElementPtr->life_span = NORMAL_LIFE;
	SetPrimType (&DisplayArray[AsteroidElementPtr->PrimIndex], STAMP_PRIM);
	if ((val = (COUNT)random ()) & (1 << 0))
	{
	    if (!(val & (1 << 1)))
		AsteroidElementPtr->current.location.x = 0;
	    else
		AsteroidElementPtr->current.location.x = LOG_SPACE_WIDTH;
	    AsteroidElementPtr->current.location.y =
		    WRAP_Y (DISPLAY_ALIGN_Y (random ()));
	}
	else
	{
	    AsteroidElementPtr->current.location.x =
		    WRAP_X (DISPLAY_ALIGN_X (random ()));
	    if (!(val & (1 << 1)))
		AsteroidElementPtr->current.location.y = 0;
	    else
		AsteroidElementPtr->current.location.y = LOG_SPACE_HEIGHT;
	}

	SetVelocityVector (&AsteroidElementPtr->velocity,
		DISPLAY_TO_WORLD (((SIZE)random () & 7) + 4),
		(COUNT)random ());
	AsteroidElementPtr->current.image.farray = asteroid;
	AsteroidElementPtr->current.image.frame =
		SetAbsFrameIndex (asteroid[0],
		NORMALIZE_FACING (random ()));
	AsteroidElementPtr->turn_wait =
		AsteroidElementPtr->thrust_wait =
		(BYTE)random () & (BYTE)((1 << 2) - 1);
	AsteroidElementPtr->thrust_wait |= (BYTE)random () & (BYTE)(1 << 7);
	AsteroidElementPtr->preprocess_func = asteroid_preprocess;
	AsteroidElementPtr->death_func = spawn_rubble;
	AsteroidElementPtr->collision_func = collision;
	UnlockElement (hAsteroidElement);

	PutElement (hAsteroidElement);
    }
}

PROC(
void do_damage, (ElementPtr, damage),
    ARG		(ELEMENTPTR	ElementPtr)
    ARG_END	(SIZE		damage)
)
{
    if (ElementPtr->state_flags & PLAYER_SHIP)
    {
	if (!DeltaCrew (ElementPtr, -damage))
	{
	    ElementPtr->life_span = 0;
	    ElementPtr->state_flags |= NONSOLID;
	}
    }
    else if (!GRAVITY_MASS (ElementPtr->mass_points))
    {
	if ((BYTE)damage < ElementPtr->hit_points)
	    ElementPtr->hit_points -= (BYTE)damage;
	else
	{
	    ElementPtr->hit_points = 0;
	    ElementPtr->life_span = 0;
	    ElementPtr->state_flags |= NONSOLID;
	}
    }
}

PROC(
void crew_preprocess, (ElementPtr),
    ARG_END	(LPELEMENT	ElementPtr)
)
{
    HELEMENT	hTarget;

    SetPrimColor (&DisplayArray[ElementPtr->PrimIndex],
	    GetPrimColor (&DisplayArray[ElementPtr->PrimIndex])
	    ^ BUILD_COLOR (MAKE_RGB15 (0xA, 0xA, 0xA), 0x08));
    ElementPtr->state_flags |= CHANGING;

    if ((hTarget = ElementPtr->hTarget) == 0)
    {
	STARSHIPPTR	StarShipPtr;

	GetElementStarShip (ElementPtr, &StarShipPtr);
	if (StarShipPtr && StarShipPtr->RaceDescPtr->ship_info.crew_level)
	    ElementPtr->hTarget = StarShipPtr->hShip;
	else
	{
	    COUNT	facing;

	    facing = 0;
	    TrackShip ((ELEMENTPTR)ElementPtr, &facing);
	}
    }

    if (hTarget)
    {
#define CREW_DELTA	SCALED_ONE
	SIZE		delta;
	ELEMENTPTR	ShipPtr;

	LockElement (hTarget, &ShipPtr);
	delta = ShipPtr->current.location.x
		- ElementPtr->current.location.x;
	delta = WRAP_DELTA_X (delta);
	if (delta > 0)
	    ElementPtr->next.location.x += CREW_DELTA;
	else if (delta < 0)
	    ElementPtr->next.location.x -= CREW_DELTA;

	delta = ShipPtr->current.location.y -
		ElementPtr->current.location.y;
	delta = WRAP_DELTA_Y (delta);
	if (delta > 0)
	    ElementPtr->next.location.y += CREW_DELTA;
	else if (delta < 0)
	    ElementPtr->next.location.y -= CREW_DELTA;
	UnlockElement (hTarget);
    }
}

PROC(
void crew_collision, (ElementPtr0, pPt0, ElementPtr1, pPt1),
    ARG		(LPELEMENT	ElementPtr0)
    ARG		(PPOINT		pPt0)
    ARG		(LPELEMENT	ElementPtr1)
    ARG_END	(PPOINT		pPt1)
)
{
    if ((ElementPtr1->state_flags & PLAYER_SHIP)
	    && ElementPtr1->life_span >= NORMAL_LIFE
	    && ElementPtr0->hit_points > 0)
    {
	STARSHIPPTR		StarShipPtr;

	GetElementStarShip (ElementPtr1, &StarShipPtr);
	if (!(StarShipPtr->RaceDescPtr->ship_info.ship_flags & CREW_IMMUNE))
	{
	    ProcessSound (SetAbsSoundIndex (GameSounds, GRAB_CREW));
	    DeltaCrew ((ELEMENTPTR)ElementPtr1, 1);
	}
    }

    ElementPtr0->hit_points = 0;
    ElementPtr0->life_span = 0;
    ElementPtr0->state_flags |= COLLISION | DISAPPEARING | NONSOLID;
}

PROC(
void AbandonShip, (ShipPtr, TargetPtr, crew_loss),
    ARG		(ELEMENTPTR	ShipPtr)
    ARG		(ELEMENTPTR	TargetPtr)
    ARG_END	(COUNT		crew_loss)
)
{
    SIZE		dx, dy;
    COUNT		direction;
    RECT		r;
    STARSHIPPTR		StarShipPtr;
    HELEMENT		hCrew;
    INTERSECT_CONTROL	ShipIntersect;

    GetElementStarShip (ShipPtr, &StarShipPtr);
    if (StarShipPtr->RaceDescPtr->ship_info.ship_flags & CREW_IMMUNE)
	return;

    ShipIntersect = ShipPtr->IntersectControl;
    GetFrameRect (ShipIntersect.IntersectStamp.frame, &r);

    if ((direction = GetVelocityTravelAngle (
	    &ShipPtr->velocity)) == FULL_CIRCLE)
	dx = dy = 0;
    else
    {
#define MORE_THAN_ENOUGH	100
	direction += HALF_CIRCLE;
	dx = COSINE (direction, MORE_THAN_ENOUGH);
	dy = SINE (direction, MORE_THAN_ENOUGH);
    }

    while (crew_loss-- && (hCrew = AllocElement ()))
    {
#define CREW_LIFE	300
	ELEMENTPTR	CrewPtr;

	DeltaCrew (ShipPtr, -1);

	PutElement (hCrew);
	LockElement (hCrew, &CrewPtr);
	CrewPtr->hit_points = 1;
	CrewPtr->state_flags =
		APPEARING | FINITE_LIFE | CREW_OBJECT;
	CrewPtr->life_span = CREW_LIFE;
	SetPrimType (&DisplayArray[CrewPtr->PrimIndex], POINT_PRIM);
	SetPrimColor (&DisplayArray[CrewPtr->PrimIndex],
		BUILD_COLOR (MAKE_RGB15 (0x00, 0x14, 0x00), 0x02));
	CrewPtr->current.image.frame = DecFrameIndex (stars_in_space);
	CrewPtr->current.image.farray = &stars_in_space;
	CrewPtr->preprocess_func = crew_preprocess;
	CrewPtr->collision_func = crew_collision;

	SetElementStarShip (CrewPtr, StarShipPtr);

	GetElementStarShip (TargetPtr, &StarShipPtr);
	CrewPtr->hTarget = StarShipPtr->hShip;

	{
	    SIZE		w, h;
	    INTERSECT_CONTROL	CrewIntersect;

	    ShipIntersect.IntersectStamp.origin =
		    ShipPtr->IntersectControl.EndPoint;

	    w = (SIZE)((COUNT)random () % r.extent.width);
	    h = (SIZE)((COUNT)random () % r.extent.height);
	    CrewIntersect.EndPoint = ShipIntersect.EndPoint;
	    CrewIntersect.IntersectStamp.frame = DecFrameIndex (stars_in_space);
	    if (dx == 0 && dy == 0)
	    {
		CrewIntersect.EndPoint.x += w - (r.extent.width >> 1);
		CrewIntersect.EndPoint.y += h - (r.extent.height >> 1);
		CrewIntersect.IntersectStamp.origin =
			TargetPtr->IntersectControl.EndPoint;
	    }
	    else
	    {
		if (dx == 0)
		    CrewIntersect.EndPoint.x += w - (r.extent.width >> 1);
		else if (dx > 0)
		    CrewIntersect.EndPoint.x += w;
		else
		    CrewIntersect.EndPoint.x -= w;
		if (dy == 0)
		    CrewIntersect.EndPoint.y += h - (r.extent.height >> 1);
		else if (dy > 0)
		    CrewIntersect.EndPoint.y += h;
		else
		    CrewIntersect.EndPoint.y -= h;
		CrewIntersect.IntersectStamp.origin.x =
			CrewIntersect.EndPoint.x + dx;
		CrewIntersect.IntersectStamp.origin.y =
			CrewIntersect.EndPoint.y + dy;
	    }

	    DrawablesIntersect (&CrewIntersect,
		    &ShipIntersect, MAX_TIME_VALUE);

	    CrewPtr->current.location.x =
		    DISPLAY_TO_WORLD (CrewIntersect.EndPoint.x);
	    CrewPtr->current.location.y =
		    DISPLAY_TO_WORLD (CrewIntersect.EndPoint.y);
	}
	UnlockElement (hCrew);
    }
}

