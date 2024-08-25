#include "starcon.h"
#include "lander.h"
#include "lifeform.h"

FRAME			LanderFrame[8];
static SOUND		LanderSounds;
MUSIC_REF		LanderMusic;
#define NUM_ORBIT_THEMES	5
static MUSIC_REF	OrbitMusic[NUM_ORBIT_THEMES];
CONST LIFEFORM_DESC	CreatureData[] =
{
    {SPEED_MOTIONLESS | DANGER_HARMLESS, MAKE_BYTE (1, 1)},
    {SPEED_MOTIONLESS | DANGER_HARMLESS, MAKE_BYTE (6, 1)},
    {SPEED_MOTIONLESS | DANGER_WEAK, MAKE_BYTE (3, 1)},
    {SPEED_MOTIONLESS | DANGER_NORMAL, MAKE_BYTE (5, 3)},
    {SPEED_MOTIONLESS | DANGER_HARMLESS, MAKE_BYTE (2, 10)},
    {BEHAVIOR_UNPREDICTABLE | SPEED_SLOW | DANGER_HARMLESS, MAKE_BYTE (1, 2)},
    {BEHAVIOR_FLEE | AWARENESS_MEDIUM | SPEED_SLOW | DANGER_HARMLESS, MAKE_BYTE (8, 5)},
    {BEHAVIOR_HUNT | AWARENESS_LOW | SPEED_SLOW | DANGER_WEAK, MAKE_BYTE (2, 2)},
    {BEHAVIOR_UNPREDICTABLE | SPEED_SLOW | DANGER_NORMAL, MAKE_BYTE (3, 8)},
    {BEHAVIOR_HUNT | AWARENESS_MEDIUM | SPEED_SLOW | DANGER_MONSTROUS, MAKE_BYTE (10, 15)},
    {BEHAVIOR_HUNT | AWARENESS_MEDIUM | SPEED_MEDIUM | DANGER_WEAK, MAKE_BYTE (3, 3)},
    {BEHAVIOR_FLEE | AWARENESS_MEDIUM | SPEED_MEDIUM | DANGER_HARMLESS, MAKE_BYTE (2, 1)},
    {BEHAVIOR_UNPREDICTABLE | SPEED_MEDIUM | DANGER_WEAK, MAKE_BYTE (2, 2)},
    {BEHAVIOR_HUNT | AWARENESS_HIGH | SPEED_MEDIUM | DANGER_NORMAL, MAKE_BYTE (4, 6)},
    {BEHAVIOR_UNPREDICTABLE | SPEED_MEDIUM | DANGER_MONSTROUS, MAKE_BYTE (9, 12)},
    {BEHAVIOR_HUNT | AWARENESS_HIGH | SPEED_FAST | DANGER_WEAK, MAKE_BYTE (3, 1)},
    {BEHAVIOR_FLEE | AWARENESS_HIGH | SPEED_FAST | DANGER_HARMLESS, MAKE_BYTE (1, 1)},
    {BEHAVIOR_HUNT | AWARENESS_LOW | SPEED_FAST | DANGER_NORMAL, MAKE_BYTE (7, 8)},
    {BEHAVIOR_FLEE | AWARENESS_HIGH | SPEED_FAST | DANGER_WEAK, MAKE_BYTE (15, 2)},
    {BEHAVIOR_FLEE | AWARENESS_LOW | SPEED_FAST | DANGER_WEAK, MAKE_BYTE (1, 1)},
    {BEHAVIOR_UNPREDICTABLE | SPEED_SLOW | DANGER_WEAK, MAKE_BYTE (6, 2)},
    {BEHAVIOR_FLEE | AWARENESS_HIGH | SPEED_SLOW | DANGER_WEAK, MAKE_BYTE (4, 2)},
    {SPEED_MOTIONLESS | DANGER_WEAK, MAKE_BYTE (8, 5)},

    {SPEED_MOTIONLESS | DANGER_MONSTROUS, MAKE_BYTE (1, 1)},
    {BEHAVIOR_UNPREDICTABLE | SPEED_SLOW | DANGER_HARMLESS, MAKE_BYTE (0, 1)},
    {BEHAVIOR_HUNT | AWARENESS_HIGH | SPEED_FAST | DANGER_MONSTROUS, MAKE_BYTE (15, 15)},
};

#define FLASH_WIDTH		9
#define FLASH_HEIGHT		9

extern PRIM_LINKS	DisplayLinks;

#define DAMAGE_CYCLE		6
#define EXPLOSION_LIFE		10

#define SHIELD_BIT		(1 << 7)

#define DEATH_EXPLOSION		0

#define SURFACE_X		SIS_ORG_X
#define SURFACE_Y		SIS_ORG_Y
#define SURFACE_WIDTH		SIS_SCREEN_WIDTH
#define SURFACE_HEIGHT		124

#define REPAIR_LANDER		(1 << 7)
#define REPAIR_TRANSITION	(1 << 6)
#define KILL_CREW		(1 << 5)
#define ADD_AT_END		(1 << 4)
#define REPAIR_COUNT		(0xf)

#define LANDER_SPEED_DENOM	10

static BYTE	lander_flags;

PROC(STATIC
COLOR near DamageColorCycle, (c, i),
    ARG		(COLOR	c)
    ARG_END	(COUNT	i)
)
{
    COLOR	damage_tab[DAMAGE_CYCLE + 1] =
    {
	WHITE_COLOR,
	BUILD_COLOR (MAKE_RGB15 (0x1B, 0x00, 0x00), 0x2A),
	BUILD_COLOR (MAKE_RGB15 (0x1F, 0x7, 0x00), 0x7E),
	BUILD_COLOR (MAKE_RGB15 (0x1F, 0xE, 0x00), 0x7C),
	BUILD_COLOR (MAKE_RGB15 (0x1F, 0x15, 0x00), 0x7A),
	BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1C, 0x00), 0x78),
	BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1F, 0xA), 0x0E),
    };

    if (i)
	c = damage_tab[i];
    else if (c == WHITE_COLOR)
	c = damage_tab[6];
    else if (c == BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1F, 0xA), 0x0E))
	c = damage_tab[5];
    else if (c == BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1C, 0x00), 0x78))
	c = damage_tab[4];
    else if (c == BUILD_COLOR (MAKE_RGB15 (0x1F, 0x15, 0x00), 0x7A))
	c = damage_tab[3];
    else if (c == BUILD_COLOR (MAKE_RGB15 (0x1F, 0xE, 0x00), 0x7C))
	c = damage_tab[2];
    else if (c == BUILD_COLOR (MAKE_RGB15 (0x1F, 0x7, 0x00), 0x7E))
	c = damage_tab[1];
    else
	c = damage_tab[0];

    return (c);
}

#define MAGNIFICATION	(1 << MAG_SHIFT)

PROC(STATIC
BOOLEAN near RepairTopography, (ElementPtr),
    ARG_END	(ELEMENTPTR	ElementPtr)
)
{
    BOOLEAN	CursorIntersect;
    SIZE	delta;
    RECT	r;
    STAMP	s;

    s.origin = ElementPtr->current.location;

    GetFrameRect (ElementPtr->next.image.frame, &r);
    r.corner.x += ElementPtr->current.location.x;
    r.corner.y += ElementPtr->current.location.y;

    delta = (ElementPtr->next.location.x >> MAG_SHIFT) - r.corner.x;
    if (delta >= 0)
	r.extent.width += delta;
    else
    {
	r.corner.x += delta;
	r.extent.width -= delta;
    }
    if ((ElementPtr->current.location.x += delta) < 0)
	ElementPtr->current.location.x += MAP_WIDTH;
    else if (ElementPtr->current.location.x >= MAP_WIDTH)
	ElementPtr->current.location.x -= MAP_WIDTH;

    delta = (ElementPtr->next.location.y >> MAG_SHIFT) - r.corner.y;
    if (delta >= 0)
	r.extent.height += delta;
    else
    {
	r.corner.y += delta;
	r.extent.height -= delta;
    }
    ElementPtr->current.location.y += delta;

#if 0
    if (s.origin.x != ElementPtr->current.location.x
	    || s.origin.y != ElementPtr->current.location.y
	    || (ElementPtr->state_flags & DISAPPEARING))
    {
	CONTEXT		OldContext;
	DRAW_STATE	OldDrawState;

	s.frame = ElementPtr->next.image.frame;

#if 0
	CursorIntersect = (BOOLEAN)(
		pMenuState->flash_rect0.corner.x < r.corner.x
		+ r.extent.width
		&& pMenuState->flash_rect0.corner.y < r.corner.y
		+ r.extent.height
		&& r.corner.x < pMenuState->flash_rect0.corner.x
		+ FLASH_WIDTH
		&& r.corner.y < pMenuState->flash_rect0.corner.y
		+ FLASH_HEIGHT
		);
#endif

	OldContext = SetContext (ScanContext);
	OldDrawState = SetContextDrawState (DEST_MASK | DRAW_ADDITIVE);
#if 0
	DrawStamp (&s);
#endif
	s.origin = ElementPtr->current.location;
	DrawStamp (&s);
	SetContextDrawState (OldDrawState);
	SetContext (OldContext);

	return (CursorIntersect);
    }
#endif

    return (FALSE);
}

PROC_LOCAL(
HELEMENT near AddGroundDisaster, (which_disaster),
    ARG_END	(COUNT	which_disaster)
);

PROC(
void object_animation, (ElementPtr),
    ARG_END	(LPELEMENT	ElementPtr)
)
{
    COUNT	frame_index, angle;
    PPRIMITIVE	pPrim;

    pPrim = &DisplayArray[ElementPtr->PrimIndex];
    if (GetPrimType (pPrim) == STAMPFILL_PRIM
	    && !((ElementPtr->state_flags & FINITE_LIFE)
	    && ElementPtr->mass_points == EARTHQUAKE_DISASTER))
    {
	COLOR	c;

	if ((c = DamageColorCycle (
		GetPrimColor (pPrim), 0
		)) == WHITE_COLOR)
	{
	    SetPrimType (pPrim, STAMP_PRIM);
	    if (ElementPtr->hit_points == 0)
	    {
		ZeroVelocityComponents (&ElementPtr->velocity);
		pPrim->Object.Stamp.frame =
			SetAbsFrameIndex (pPrim->Object.Stamp.frame, 0);

		PlaySound (SetAbsSoundIndex (LanderSounds, LIFEFORM_CANNED),
			GAME_SOUND_PRIORITY);
	    }
	}

	SetPrimColor (pPrim, c);
    }

    frame_index = GetFrameIndex (pPrim->Object.Stamp.frame) + 1;
    if (LONIBBLE (ElementPtr->turn_wait))
	--ElementPtr->turn_wait;
    else
    {
	ElementPtr->turn_wait += HINIBBLE (ElementPtr->turn_wait);

	pPrim->Object.Stamp.frame =
		IncFrameIndex (pPrim->Object.Stamp.frame);
	if (ElementPtr->state_flags & FINITE_LIFE)
	{
	    if (ElementPtr->mass_points == DEATH_EXPLOSION)
	    {
		if (++pMenuState->CurState >= EXPLOSION_LIFE)
		    pPrim->Object.Stamp.frame =
			    DecFrameIndex (pPrim->Object.Stamp.frame);
	    }
	    else if (ElementPtr->mass_points == EARTHQUAKE_DISASTER)
	    {
		SIZE	s;

		if (frame_index >= 13)
		    s = 0;
		else
		    s = (14 - frame_index) >> 1;
		SetPrimColor (pPrim, BUILD_COLOR (0x8000 | MAKE_RGB15 (0x1F, 0x1F, 0x1F), s));
		if (frame_index == 13)
		    PlaySound (SetAbsSoundIndex (LanderSounds, EARTHQUAKE_DISASTER),
			    GAME_SOUND_PRIORITY);
	    }
	    
	    if (ElementPtr->mass_points == LAVASPOT_DISASTER
		    && frame_index == 5
		    && (BYTE)random () < 256 * 90 / 100)
	    {
		HELEMENT	hLavaElement;

		if (hLavaElement = AddGroundDisaster (LAVASPOT_DISASTER))
		{
		    ELEMENTPTR	LavaElementPtr;

		    angle = FACING_TO_ANGLE (ElementPtr->hit_points);
		    LockElement (hLavaElement, &LavaElementPtr);
		    LavaElementPtr->next.location = ElementPtr->next.location;
		    LavaElementPtr->next.location.x += COSINE (angle, 4);
		    LavaElementPtr->next.location.y += SINE (angle, 4);
		    if (LavaElementPtr->next.location.y < 0)
			LavaElementPtr->next.location.y = 0;
		    else if (LavaElementPtr->next.location.y >= (MAP_HEIGHT << MAG_SHIFT))
			LavaElementPtr->next.location.y = (MAP_HEIGHT << MAG_SHIFT) - 1;
		    if (LavaElementPtr->next.location.x < 0)
			LavaElementPtr->next.location.x += MAP_WIDTH << MAG_SHIFT;
		    else
			LavaElementPtr->next.location.x %= MAP_WIDTH << MAG_SHIFT;
		    LavaElementPtr->hit_points = NORMALIZE_FACING (
			    ElementPtr->hit_points + ((COUNT)random () % 3) - 1
			    );
		    UnlockElement (hLavaElement);
		}
	    }
	}
	else if (!(frame_index & 3) && ElementPtr->hit_points)
	{
	    BYTE	index;
	    COUNT	speed;

	    index = ElementPtr->mass_points & ~CREATURE_AWARE;
	    if (speed = CreatureData[index].Attributes & SPEED_MASK)
	    {
		SIZE	dx, dy;
		COUNT	old_angle;

		dx = pSolarSysState->MenuState.first_item.x
			- ElementPtr->next.location.x;
		if (dx < 0 && dx < -(MAP_WIDTH << (MAG_SHIFT - 1)))
		    dx += MAP_WIDTH << MAG_SHIFT;
		else if (dx > (MAP_WIDTH << (MAG_SHIFT - 1)))
		    dx -= MAP_WIDTH << MAG_SHIFT;
		dy = pSolarSysState->MenuState.first_item.y
			- ElementPtr->next.location.y;
		angle = ARCTAN (dx, dy);
		if (dx < 0)
		    dx = -dx;
		if (dy < 0)
		    dy = -dy;

		if (dx >= SURFACE_WIDTH || dy >= SURFACE_WIDTH
			|| dx * dx + dy * dy >= SURFACE_WIDTH * SURFACE_WIDTH)
		    ElementPtr->mass_points &= ~CREATURE_AWARE;
		else if (!(ElementPtr->mass_points & CREATURE_AWARE))
		{
		    BYTE	DetectPercent;

		    DetectPercent = (((BYTE)(CreatureData[index].Attributes
			    & AWARENESS_MASK) >> AWARENESS_SHIFT) + 1)
			    * (30 / 6);
		    if (((BYTE)random () % 100) < DetectPercent)
		    {
			ElementPtr->thrust_wait = 0;
			ElementPtr->mass_points |= CREATURE_AWARE;
		    }
		}

		if (ElementPtr->next.location.y == 0
			|| ElementPtr->next.location.y ==
			(MAP_HEIGHT << MAG_SHIFT) - 1)
		    ElementPtr->thrust_wait = 0;

		old_angle = GetVelocityTravelAngle (&ElementPtr->velocity);
		if (ElementPtr->thrust_wait)
		{
		    --ElementPtr->thrust_wait;
		    angle = old_angle;
		}
		else if (!(ElementPtr->mass_points & CREATURE_AWARE)
			|| (CreatureData[index].Attributes
			& BEHAVIOR_MASK) == BEHAVIOR_UNPREDICTABLE)
		{
		    COUNT	rand_val;

		    rand_val = (COUNT)random ();
		    angle = NORMALIZE_ANGLE (LOBYTE (rand_val));
		    ElementPtr->thrust_wait =
			    (HIBYTE (rand_val) >> 2) + 10;
		}
		else if ((CreatureData[index].Attributes
			& BEHAVIOR_MASK) == BEHAVIOR_FLEE)
		{
		    if (ElementPtr->next.location.y == 0
			    || ElementPtr->next.location.y ==
			    (MAP_HEIGHT << MAG_SHIFT) - 1)
		    {
			if (angle & (HALF_CIRCLE - 1))
			    angle = HALF_CIRCLE - angle;
			else if (old_angle == QUADRANT
				|| old_angle == (FULL_CIRCLE - QUADRANT))
			    angle = old_angle;
			else
			    angle = (((COUNT)random () & 1)
					* HALF_CIRCLE) - QUADRANT;
			ElementPtr->thrust_wait = 5;
		    }
		    angle = NORMALIZE_ANGLE (angle + HALF_CIRCLE);
		}

		switch (speed)
		{
		    case SPEED_SLOW:
			speed = WORLD_TO_VELOCITY (2 * 1) >> 2;
			break;
		    case SPEED_MEDIUM:
			speed = WORLD_TO_VELOCITY (2 * 1) >> 1;
			break;
		    case SPEED_FAST:
			speed = WORLD_TO_VELOCITY (2 * 1) * 9 / 10;
			break;
		}

		SetVelocityComponents (
			&ElementPtr->velocity,
			COSINE (angle, speed),
			SINE (angle, speed)
			);
	    }
	}
    }

    if ((ElementPtr->state_flags & FINITE_LIFE)
	    && ElementPtr->mass_points == DEATH_EXPLOSION
	    && GetSuccLink (DisplayLinks) != ElementPtr->PrimIndex)
	lander_flags |= ADD_AT_END;
}

#define NUM_CREW_COLS	6
#define NUM_CREW_ROWS	2

PROC(STATIC
void near DeltaLanderCrew, (crew_delta, which_disaster),
    ARG		(SIZE	crew_delta)
    ARG_END	(COUNT	which_disaster)
)
{
    STAMP	s;
    CONTEXT	OldContext;

    if (crew_delta > 0)
    {
	crew_delta = HIBYTE (pMenuState->delta_item);
	pMenuState->delta_item += MAKE_WORD (0, 1);

	s.frame = SetAbsFrameIndex (LanderFrame[0], 55);
    }
    else /* if (crew_delta < 0) */
    {
	BYTE	ShieldFlags;

	ShieldFlags = GET_GAME_STATE (LANDER_SHIELDS);
	ShieldFlags &= 1 << which_disaster;
	crew_delta = HIBYTE (pMenuState->delta_item);
	if (ShieldFlags && (BYTE)random () < 256 * 95 / 100)
	    pMenuState->delta_item = SHIELD_BIT;
	else
	{
	    ShieldFlags = 0;
	    if (--crew_delta == 0)
		pMenuState->CurState = 0;
	    pMenuState->delta_item = 0;
	}

	pMenuState->delta_item |= MAKE_WORD (DAMAGE_CYCLE, crew_delta);
	if (ShieldFlags)
	    return;

	s.frame = SetAbsFrameIndex (LanderFrame[0], 56);

	PlaySound (SetAbsSoundIndex (LanderSounds, LANDER_INJURED),
		GAME_SOUND_PRIORITY);
    }

    s.origin.x = 11 + (6 * (crew_delta % NUM_CREW_COLS));
    s.origin.y = 35 - (6 * (crew_delta / NUM_CREW_COLS));

    OldContext = SetContext (RadarContext);
    DrawStamp (&s);
    SetContext (OldContext);
}

PROC(STATIC
void near FillLanderHold, (pPSD, scan, NumRetrieved),
    ARG		(PPLANETSIDE_DESC	pPSD)
    ARG		(COUNT			scan)
    ARG_END	(COUNT			NumRetrieved)
)
{
    COUNT	start_count;
    STAMP	s;
    CONTEXT	OldContext;

    PlaySound (SetAbsSoundIndex (LanderSounds, LANDER_PICKUP),
	    GAME_SOUND_PRIORITY);

    if (scan == BIOLOGICAL_SCAN)
    {
	start_count = pPSD->BiologicalLevel;

	s.frame = SetAbsFrameIndex (LanderFrame[0], 41);

	pPSD->BiologicalLevel += NumRetrieved;
    }
    else
    {
	start_count = pPSD->ElementLevel;
	pPSD->ElementLevel += NumRetrieved;
	if (GET_GAME_STATE (IMPROVED_LANDER_CARGO))
	{
	    start_count >>= 1;
	    NumRetrieved = (pPSD->ElementLevel >> 1) - start_count;
	}

	s.frame = SetAbsFrameIndex (LanderFrame[0], 43);
    }

    s.origin.x = 0;
    s.origin.y = -start_count;
    if (!(start_count & 1))
	s.frame = IncFrameIndex (s.frame);

    OldContext = SetContext (RadarContext);
    while (NumRetrieved--)
    {
	if (start_count++ & 1)
	    s.frame = IncFrameIndex (s.frame);
	else
	    s.frame = DecFrameIndex (s.frame);
	DrawStamp (&s);
	--s.origin.y;
    }
    SetContext (OldContext);
}

PROC(STATIC
void near CheckObjectCollision, (index),
    ARG_END	(COUNT	index)
)
{
    INTERSECT_CONTROL	LanderControl;
    MEM_HANDLE		LanderHandle;
    PPRIMITIVE		pPrim, pLanderPrim;
    PPLANETSIDE_DESC	pPSD;

    if (index != END_OF_LIST)
    {
	pLanderPrim = &DisplayArray[index];
	LanderControl.IntersectStamp = pLanderPrim->Object.Stamp;
	index = GetPredLink (GetPrimLinks (pLanderPrim));
    }
    else
    {
	pLanderPrim = 0;
	LanderControl.IntersectStamp.origin.x = SURFACE_WIDTH >> 1;
	LanderControl.IntersectStamp.origin.y = SURFACE_HEIGHT >> 1;
	LanderControl.IntersectStamp.frame = LanderFrame[0];
	index = GetSuccLink (DisplayLinks);
    }

    pPSD = (PPLANETSIDE_DESC)pMenuState->ModuleFrame;
    LanderControl.EndPoint = LanderControl.IntersectStamp.origin;
    LanderHandle = GetFrameHandle (LanderControl.IntersectStamp.frame);

    for (; index != END_OF_LIST; index = GetPredLink (GetPrimLinks (pPrim)))
    {
	INTERSECT_CONTROL	ElementControl;

	pPrim = &DisplayArray[index];
	ElementControl.IntersectStamp = pPrim->Object.Stamp;
	ElementControl.EndPoint =
		ElementControl.IntersectStamp.origin;

	if (GetFrameHandle (
		ElementControl.IntersectStamp.frame
		) == LanderHandle)
	    CheckObjectCollision (index);
	else if (DrawablesIntersect (&LanderControl,
		&ElementControl, MAX_TIME_VALUE))
	{
	    HELEMENT		hElement, hNextElement;

	    for (hElement = GetHeadElement ();
		    hElement; hElement = hNextElement)
	    {
		ELEMENTPTR	ElementPtr;

		LockElement (hElement, &ElementPtr);
		hNextElement = GetSuccElement (ElementPtr);

		if (&DisplayArray[ElementPtr->PrimIndex] == pLanderPrim)
		{
		    ElementPtr->state_flags |= DISAPPEARING;
		    UnlockElement (hElement);

		    continue;
		}
		else if (&DisplayArray[ElementPtr->PrimIndex] == pPrim
			&& (ElementPtr->state_flags & BAD_GUY))
		{
		    COUNT	scan, NumRetrieved;
		    SIZE	which_node;

		    scan = LOBYTE (ElementPtr->life_span);
		    if (pLanderPrim == 0)
		    {
			if (HIBYTE (pMenuState->delta_item) == 0
				|| pPSD->InTransit)
			    break;

				/* if a natural disaster */
			if (ElementPtr->state_flags & FINITE_LIFE)
			{
			    scan = ElementPtr->mass_points;
			    switch (scan)
			    {
				case EARTHQUAKE_DISASTER:
				case LAVASPOT_DISASTER:
				    if ((BYTE)random () < (256 >> 2))
					DeltaLanderCrew (-1, scan);
				    break;
			    }

			    UnlockElement (hElement);
			    continue;
			}
			else if (scan == ENERGY_SCAN)
			{
			    if (ElementPtr->mass_points == 1)
			    {
				DWORD	TimeIn;

					/* ran into Spathi on Pluto */
				TimeIn = GetTimeCounter ();
				which_node = 8;
				do
				{
				    DeltaLanderCrew (-1, LANDER_INJURED);
				    TimeIn = SleepTask (TimeIn + 6);
				} while (HIBYTE (pMenuState->delta_item) && --which_node);
			    }

			    if (HIBYTE (pMenuState->delta_item)
				    && pSolarSysState->SysInfo.PlanetInfo.DiscoveryString
				    && CurStarDescPtr->Index != ANDROSYNTH_DEFINED)
			    {
				UnbatchGraphics ();
			        DoDiscoveryReport (pPSD->OldMenuSounds);
				BatchGraphics ();
			    }
			    if (ElementPtr->mass_points == 0)
			    {
				DestroyStringTable (ReleaseStringTable (
					pSolarSysState->SysInfo.PlanetInfo.DiscoveryString
					));
				pSolarSysState->SysInfo.PlanetInfo.DiscoveryString = 0;
				UnlockElement (hElement);
				continue;
			    }
			}
			else if (scan == BIOLOGICAL_SCAN
				&& ElementPtr->hit_points)
			{
			    BYTE	danger_vals[] =
			    {
				0, 6, 13, 26
			    };

			    if (((COUNT)random () & 127) < danger_vals[
				    (CreatureData[
					    ElementPtr->mass_points
					    & ~CREATURE_AWARE
					    ].Attributes & DANGER_MASK)
				    >> DANGER_SHIFT
				    ])
			    {
				PlaySound (SetAbsSoundIndex (
					LanderSounds, BIOLOGICAL_DISASTER
					), GAME_SOUND_PRIORITY);
				DeltaLanderCrew (-1, BIOLOGICAL_DISASTER);
			    }
			    UnlockElement (hElement);
			    continue;
			}

			NumRetrieved = ElementPtr->mass_points;
		    }
			/* if a natural disaster */
		    else if (ElementPtr->state_flags & FINITE_LIFE)
		    {
			UnlockElement (hElement);
			continue;
		    }
		    else
		    {
			BYTE	value;

			if (scan == ENERGY_SCAN)
			{
			    UnlockElement (hElement);
			    break;
			}
			else if (scan == BIOLOGICAL_SCAN
				&& (value = LONIBBLE (CreatureData[
				ElementPtr->mass_points
				& ~CREATURE_AWARE
				].ValueAndHitPoints)))
			{
			    if (ElementPtr->hit_points)
			    {
				if (--ElementPtr->hit_points == 0)
				{
				    ElementPtr->mass_points = value;
				    DisplayArray[
					    ElementPtr->PrimIndex
					    ].Object.Stamp.frame =
					    pSolarSysState->PlanetSideFrame[0];
				}
				else if (CreatureData[
					ElementPtr->mass_points
					& ~CREATURE_AWARE
					].Attributes & SPEED_MASK)
				{
				    COUNT	angle;

				    angle = FACING_TO_ANGLE (GetFrameIndex (
					    LanderControl.IntersectStamp.frame
					    ) - ANGLE_TO_FACING (FULL_CIRCLE));
				    DeltaVelocityComponents (
					    &ElementPtr->velocity,
					    COSINE (angle, WORLD_TO_VELOCITY (1)),
					    SINE (angle, WORLD_TO_VELOCITY (1))
					    );
				    ElementPtr->thrust_wait = 0;
				    ElementPtr->mass_points |= CREATURE_AWARE;
				}

				SetPrimType (pPrim, STAMPFILL_PRIM);
				SetPrimColor (pPrim, WHITE_COLOR);

				PlaySound (SetAbsSoundIndex (
					LanderSounds, LANDER_HITS
					), GAME_SOUND_PRIORITY);
			    }
			    UnlockElement (hElement);
			    break;
			}

			NumRetrieved = 0;
		    }

		    if (NumRetrieved)
		    {
			switch (scan)
			{
			    case ENERGY_SCAN:
				break;
			    case MINERAL_SCAN:
				if (pPSD->ElementLevel < pPSD->MaxElementLevel)
				{
				    if (pPSD->ElementLevel
					    + NumRetrieved > pPSD->MaxElementLevel)
					NumRetrieved = (COUNT)(
						pPSD->MaxElementLevel
						- pPSD->ElementLevel
						);
				    FillLanderHold (pPSD, scan, NumRetrieved);
				    if (scan == MINERAL_SCAN)
				    {
#define ELEMENT_BASE	161
					BYTE		EType;
					char		ch;
					LPSTR		lpStr;
					extern PSTR	far *ClusterNames;

					EType = ElementPtr->turn_wait;
					pPSD->ElementAmounts[
						ElementCategory (EType)
						] += NumRetrieved;

					pPSD->NumFrames = NUM_TEXT_FRAMES;
					sprintf (pPSD->AmountBuf, "%u", NumRetrieved);
					SET_SEGMENT (&lpStr,
						GET_SEGMENT (&ClusterNames));
					SET_OFFSET (&lpStr, ClusterNames[
						EType + ELEMENT_BASE
						]);

					pPSD->MineralText[0].baseline.x =
						(SURFACE_WIDTH >> 1)
						+ (ElementControl.EndPoint.x
						- LanderControl.EndPoint.x);
					pPSD->MineralText[0].baseline.y =
						(SURFACE_HEIGHT >> 1)
						+ (ElementControl.EndPoint.y
						- LanderControl.EndPoint.y);
					pPSD->MineralText[0].CharCount = (COUNT)~0;
					pPSD->MineralText[1].lpStr = (LPBYTE)lpStr;
					while ((ch = *lpStr++) && ch != ' ')
					    ;
					if (ch == '\0')
					{
					    pPSD->MineralText[1].CharCount = (COUNT)~0;
					    pPSD->MineralText[2].CharCount = 0;
					}
					else
					{
// BVT - cast to COUNT to use - operator.
					    pPSD->MineralText[1].CharCount =
						    (COUNT)lpStr - (COUNT)pPSD->MineralText[1].lpStr -1;
					    pPSD->MineralText[2].lpStr = (LPBYTE)lpStr;
					    pPSD->MineralText[2].CharCount = (COUNT)~0;
					}
				    }
				    break;
				}
				PlaySound (SetAbsSoundIndex (
					LanderSounds, LANDER_FULL
					), GAME_SOUND_PRIORITY);
				continue;
			    case BIOLOGICAL_SCAN:
				if (pPSD->BiologicalLevel < MAX_SCROUNGED)
				{
				    if (pPSD->BiologicalLevel
					    + NumRetrieved > MAX_SCROUNGED)
					NumRetrieved = (COUNT)(
						MAX_SCROUNGED
						- pPSD->BiologicalLevel
						);
				    FillLanderHold (pPSD, scan, NumRetrieved);
				    break;
				}
				PlaySound (SetAbsSoundIndex (
					LanderSounds, LANDER_FULL
					), GAME_SOUND_PRIORITY);
				continue;
			}
		    }

		    which_node = HIBYTE (ElementPtr->life_span) - 1;
		    pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask[scan] |=
			    (1L << which_node);
		    pSolarSysState->CurNode = (COUNT)~0;
		    (*pSolarSysState->GenFunc) ((BYTE)(scan + GENERATE_MINERAL));

		    if (!(pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask[scan] &
			    (1L << which_node)))
		    {
			if (DestroyStringTable (ReleaseStringTable (
				pSolarSysState->SysInfo.PlanetInfo.DiscoveryString
				)))
			    pSolarSysState->SysInfo.PlanetInfo.DiscoveryString = 0;
		    }
		    else
		    {
			if (NumRetrieved && scan == ENERGY_SCAN)
			    pPSD->InTransit = TRUE;

			SET_GAME_STATE (PLANETARY_CHANGE, 1);
			ElementPtr->state_flags |= DISAPPEARING;
		    }
		    UnlockElement (hElement);
		}
		else
		    UnlockElement (hElement);
	    }
	}
    }
}

PROC(STATIC
void lightning_process, (ElementPtr),
    ARG_END	(LPELEMENT	ElementPtr)
)
{
    PPRIMITIVE	pPrim;

    pPrim = &DisplayArray[ElementPtr->PrimIndex];
    if (LONIBBLE (ElementPtr->turn_wait))
	--ElementPtr->turn_wait;
    else
    {
	COUNT	num_frames;

	num_frames = GetFrameCount (pPrim->Object.Stamp.frame) - 7;
	if (GetFrameIndex (pPrim->Object.Stamp.frame) >= num_frames)
	    pPrim->Object.Stamp.frame =
		    IncFrameIndex (pPrim->Object.Stamp.frame);
	else
	{
	    SIZE	s;
	    
	    s = 7 - ((SIZE)ElementPtr->crew_level - (SIZE)ElementPtr->life_span);
	    if (s < 0)
		s = 0;
	    SetPrimColor (pPrim, BUILD_COLOR (0x8000 | MAKE_RGB15 (0x1F, 0x1F, 0x1F), s));

	    if (ElementPtr->mass_points == LIGHTNING_DISASTER)
	    {
		if (HIBYTE (pMenuState->delta_item)
			&& (BYTE)random () < (256 / 10)
			&& !(
			(PPLANETSIDE_DESC)pMenuState->ModuleFrame
			)->InTransit)
		    lander_flags |= KILL_CREW;

		ElementPtr->next.location =
			pSolarSysState->MenuState.first_item;
	    }

	    pPrim->Object.Stamp.frame =
		     SetAbsFrameIndex (pPrim->Object.Stamp.frame,
		     (COUNT)random () % num_frames);
	}

	ElementPtr->turn_wait += HINIBBLE (ElementPtr->turn_wait);
    }

    if (GetSuccLink (DisplayLinks) != ElementPtr->PrimIndex)
	lander_flags |= ADD_AT_END;
}

PROC(STATIC
void near AddLightning, (),
    ARG_VOID
)
{
    HELEMENT	hLightningElement;

    if (hLightningElement = AllocElement ())
    {
	DWORD		rand_val;
	ELEMENTPTR	LightningElementPtr;

	LockElement (hLightningElement, &LightningElementPtr);

	LightningElementPtr->state_flags = FINITE_LIFE | BAD_GUY;
	LightningElementPtr->preprocess_func = lightning_process;
	if ((BYTE)random () >= (256 >> 2))
	    LightningElementPtr->mass_points = 0;
	else
	    LightningElementPtr->mass_points = LIGHTNING_DISASTER;

	rand_val = random ();
	LightningElementPtr->life_span = 10 + (HIWORD (rand_val) % 10) + 1;
	LightningElementPtr->next.location.x = (
		pSolarSysState->MenuState.first_item.x
		+ ((MAP_WIDTH << MAG_SHIFT) - ((SURFACE_WIDTH >> 1) - 6))
		+ (LOBYTE (rand_val) % (SURFACE_WIDTH - 12))
		) % (MAP_WIDTH << MAG_SHIFT);
	LightningElementPtr->next.location.y = (
		pSolarSysState->MenuState.first_item.y
		+ ((MAP_HEIGHT << MAG_SHIFT) - ((SURFACE_HEIGHT >> 1) - 6))
		+ (HIBYTE (rand_val) % (SURFACE_HEIGHT - 12))
		) % (MAP_HEIGHT << MAG_SHIFT);

	LightningElementPtr->crew_level = LightningElementPtr->life_span;
	
	SetPrimType (&DisplayArray[LightningElementPtr->PrimIndex], STAMPFILL_PRIM);
	SetPrimColor (&DisplayArray[LightningElementPtr->PrimIndex], WHITE_COLOR);
	DisplayArray[LightningElementPtr->PrimIndex].Object.Stamp.frame =
		LanderFrame[2];

	UnlockElement (hLightningElement);

	PutElement (hLightningElement);

	PlaySound (SetAbsSoundIndex (LanderSounds, LIGHTNING_DISASTER),
		GAME_SOUND_PRIORITY);
    }
}

PROC(STATIC
HELEMENT near AddGroundDisaster, (which_disaster),
    ARG_END	(COUNT	which_disaster)
)
{
    HELEMENT	hGroundDisasterElement;

    if (hGroundDisasterElement = AllocElement ())
    {
	DWORD		rand_val;
	ELEMENTPTR	GroundDisasterElementPtr;
	PPRIMITIVE	pPrim;

	LockElement (hGroundDisasterElement, &GroundDisasterElementPtr);

	pPrim = &DisplayArray[GroundDisasterElementPtr->PrimIndex];
	GroundDisasterElementPtr->mass_points = which_disaster;
	GroundDisasterElementPtr->state_flags = FINITE_LIFE | BAD_GUY;
	GroundDisasterElementPtr->preprocess_func = object_animation;

	rand_val = random ();
	GroundDisasterElementPtr->next.location.x = (
		pSolarSysState->MenuState.first_item.x
		+ ((MAP_WIDTH << MAG_SHIFT) - (SURFACE_WIDTH * 3 / 8))
		+ (LOWORD (rand_val) % (SURFACE_WIDTH * 3 / 4))
		) % (MAP_WIDTH << MAG_SHIFT);
	GroundDisasterElementPtr->next.location.y = (
		pSolarSysState->MenuState.first_item.y
		+ ((MAP_HEIGHT << MAG_SHIFT) - (SURFACE_HEIGHT * 3 / 8))
		+ (HIWORD (rand_val) % (SURFACE_HEIGHT * 3 / 4))
		) % (MAP_HEIGHT << MAG_SHIFT);


	if (which_disaster == EARTHQUAKE_DISASTER)
	{
	    SetPrimType (pPrim, STAMPFILL_PRIM);
	    pPrim->Object.Stamp.frame = LanderFrame[1];
	    GroundDisasterElementPtr->turn_wait = MAKE_BYTE (2, 2);
	}
	else
	{
	    SetPrimType (pPrim, STAMP_PRIM);
	    GroundDisasterElementPtr->hit_points =
		    NORMALIZE_FACING (random ());
	    pPrim->Object.Stamp.frame = LanderFrame[3];
	    GroundDisasterElementPtr->turn_wait = MAKE_BYTE (0, 0);
	}
	GroundDisasterElementPtr->life_span =
		GetFrameCount (pPrim->Object.Stamp.frame)
		* (LONIBBLE (GroundDisasterElementPtr->turn_wait) + 1) - 1;

	UnlockElement (hGroundDisasterElement);

	PutElement (hGroundDisasterElement);
    }

    return (hGroundDisasterElement);
}

PROC(STATIC
void near BuildObjectList, (),
    ARG_VOID
)
{
    DWORD		rand_val;
    POINT		org;
    HELEMENT		hElement, hNextElement;
    PPLANETSIDE_DESC	pPSD;

    DisplayLinks = MakeLinks (END_OF_LIST, END_OF_LIST);
    
    lander_flags &= ~KILL_CREW;

    pPSD = (PPLANETSIDE_DESC)pMenuState->ModuleFrame;
    rand_val = random ();
    if (LOBYTE (HIWORD (rand_val)) < pPSD->FireChance)
    {
	AddGroundDisaster (LAVASPOT_DISASTER);
	PlaySound (SetAbsSoundIndex (LanderSounds, LAVASPOT_DISASTER),
		GAME_SOUND_PRIORITY);
    }

    if (HIBYTE (LOWORD (rand_val)) < pPSD->TectonicsChance)
	AddGroundDisaster (EARTHQUAKE_DISASTER);

    if (LOBYTE (LOWORD (rand_val)) < pPSD->WeatherChance)
	AddLightning ();

    org = pSolarSysState->MenuState.first_item;
    for (hElement = GetHeadElement ();
	    hElement; hElement = hNextElement)
    {
	SIZE		dx, dy;
	ELEMENTPTR	ElementPtr;

	LockElement (hElement, &ElementPtr);

	if (ElementPtr->life_span == 0
		|| (ElementPtr->state_flags & DISAPPEARING))
	{
	    hNextElement = GetSuccElement (ElementPtr);
	    UnlockElement (hElement);

	    if (ElementPtr->state_flags & APPEARING)
	    {
		ElementPtr->current.location.x |= 0x8000;
		ElementPtr->current.location.y |= 0x8000;
	    }
	    else
	    {
		RemoveElement (hElement);
		FreeElement (hElement);
	    }
	    
	    continue;
	}
	else if (ElementPtr->state_flags & FINITE_LIFE)
	    --ElementPtr->life_span;

	lander_flags &= ~ADD_AT_END;

	if (ElementPtr->preprocess_func)
	    (*ElementPtr->preprocess_func) (ElementPtr);

	GetNextVelocityComponents (
		&ElementPtr->velocity, &dx, &dy, 1
		);
	if (dx || dy)
	{
	    ElementPtr->next.location.x += dx;
	    ElementPtr->next.location.y += dy;
		/* if not lander's shot */
	    if (ElementPtr->state_flags != (FINITE_LIFE | GOOD_GUY))
	    {
		if (ElementPtr->next.location.y < 0)
		    ElementPtr->next.location.y = 0;
		else if (ElementPtr->next.location.y >= (MAP_HEIGHT << MAG_SHIFT))
		    ElementPtr->next.location.y = (MAP_HEIGHT << MAG_SHIFT) - 1;
	    }
	    if (ElementPtr->state_flags & APPEARING)
		ElementPtr->current.location.x |= 0x8000;
	    if (ElementPtr->next.location.x < 0)
		ElementPtr->next.location.x += MAP_WIDTH << MAG_SHIFT;
	    else
		ElementPtr->next.location.x %= MAP_WIDTH << MAG_SHIFT;
	}

	{
	    PPRIMITIVE	pPrim;

	    pPrim = &DisplayArray[ElementPtr->PrimIndex];
	    pPrim->Object.Stamp.origin.x =
		    ElementPtr->next.location.x
		    - org.x + (SURFACE_WIDTH >> 1);
	    if (pPrim->Object.Stamp.origin.x >=
		    (MAP_WIDTH << MAG_SHIFT) - (SURFACE_WIDTH * 3 / 2))
		pPrim->Object.Stamp.origin.x -= MAP_WIDTH << MAG_SHIFT;
	    else if (pPrim->Object.Stamp.origin.x <=
		    -((MAP_WIDTH << MAG_SHIFT) - (SURFACE_WIDTH * 3 / 2)))
		pPrim->Object.Stamp.origin.x += MAP_WIDTH << MAG_SHIFT;

	    pPrim->Object.Stamp.origin.y =
		    ElementPtr->next.location.y
		    - org.y + (SURFACE_HEIGHT >> 1);

	    if (lander_flags & ADD_AT_END)
		InsertPrim (&DisplayLinks, ElementPtr->PrimIndex, END_OF_LIST);
	    else
		InsertPrim (&DisplayLinks, ElementPtr->PrimIndex, GetPredLink (DisplayLinks));
	}

	hNextElement = GetSuccElement (ElementPtr);
	UnlockElement (hElement);
    }
}

PROC(STATIC
void near RepairScan, (),
    ARG_VOID
)
{
    CONTEXT	OldContext;
    HELEMENT	hElement, hNextElement;
    PROC_GLOBAL(
    void DrawScannedObjects, (Reversed),
	ARG_END	(BOOLEAN	Reversed)
    );

    OldContext = SetContext (ScanContext);

    for (hElement = GetHeadElement ();
	    hElement; hElement = hNextElement)
    {
	ELEMENTPTR	ElementPtr;

	LockElement (hElement, &ElementPtr);
	hNextElement = GetSuccElement (ElementPtr);
	if (ElementPtr->current.location.x & 0x8000)
	{
	    BOOLEAN	remove;
	    
	    ElementPtr->current.location.x &= ~0x8000;
	    if (ElementPtr->current.location.y & 0x8000)
	    {
		remove = TRUE;
		ElementPtr->current.location.y &= ~0x8000;
	    }
	    else
		remove = FALSE;
	    RepairTopography (ElementPtr);
	    if (remove)
	    {
		UnlockElement (hElement);
		RemoveElement (hElement);
		FreeElement (hElement);
		continue;
	    }
	}
	
	UnlockElement (hElement);
    }

    BatchGraphics ();
#if 1
    DrawPlanet (0, 0, 0, 0);
#endif
    DrawScannedObjects (TRUE);
    {
#define FLASH_INDEX		105
	STAMP		s;
	extern FRAME	misc_data;
	
	s.origin = pMenuState->flash_rect0.corner;
	s.frame = SetAbsFrameIndex (misc_data, FLASH_INDEX);
	DrawStamp (&s);
    }
    UnbatchGraphics ();

    SetContext (OldContext);
}

PROC(STATIC
void near ScrollPlanetSide, (dx, dy, CountDown),
    ARG		(SIZE	dx)
    ARG		(SIZE	dy)
    ARG_END	(SIZE	CountDown)
)
{
    POINT		old_pt, new_pt;
    DRAW_STATE		OldDrawState;
    STAMP		lander_s, shadow_s, shield_s;
    CONTEXT		OldContext;
    signed char		lander_pos[] = { -10, 1, 12, 22, 31, 39, 45, 50, 54, 57, 59 };

    old_pt.x = pSolarSysState->MenuState.first_item.x - (SURFACE_WIDTH >> 1);
    old_pt.y = pSolarSysState->MenuState.first_item.y - (SURFACE_HEIGHT >> 1);

    new_pt.y = old_pt.y + dy;
    if (new_pt.y < -(SURFACE_HEIGHT >> 1))
    {
	new_pt.y = -(SURFACE_HEIGHT >> 1);
	dy = new_pt.y - old_pt.y;
	dx = 0;
	ZeroVelocityComponents (&GLOBAL (velocity));
    }
    else if (new_pt.y > (MAP_HEIGHT << MAG_SHIFT) - (SURFACE_HEIGHT >> 1))
    {
	new_pt.y = (MAP_HEIGHT << MAG_SHIFT) - (SURFACE_HEIGHT >> 1);
	dy = new_pt.y - old_pt.y;
	dx = 0;
	ZeroVelocityComponents (&GLOBAL (velocity));
    }

    new_pt.x = old_pt.x + dx;
    if (new_pt.x < 0)
	new_pt.x += MAP_WIDTH << MAG_SHIFT;
    else if (new_pt.x >= MAP_WIDTH << MAG_SHIFT)
	new_pt.x -= MAP_WIDTH << MAG_SHIFT;

    new_pt.x = pSolarSysState->MenuState.first_item.x + dx;
    if (new_pt.x < 0)
	new_pt.x += MAP_WIDTH << MAG_SHIFT;
    else if (new_pt.x >= MAP_WIDTH << MAG_SHIFT)
	new_pt.x -= MAP_WIDTH << MAG_SHIFT;
    new_pt.y = pSolarSysState->MenuState.first_item.y + dy;

    pSolarSysState->MenuState.first_item = new_pt;

    {
	DWORD	TimeIn;

	TimeIn = SleepTask (MAKE_DWORD (
		pSolarSysState->MenuState.flash_rect1.corner.x,
		pSolarSysState->MenuState.flash_rect1.corner.y
		)) + 2;
	pSolarSysState->MenuState.flash_rect1.corner.x = LOWORD (TimeIn);
	pSolarSysState->MenuState.flash_rect1.corner.y = HIWORD (TimeIn);
    }

    SetSemaphore (&GraphicsSem);
    OldContext = SetContext (SpaceContext);

    OldDrawState = SetContextDrawState (DEST_PIXMAP | DRAW_SUBTRACTIVE); // to BUILD_FRAME
    BatchGraphics ();
    SetContextDrawState (OldDrawState);

    {
	RECT	r;

	GetContextClipRect (&r);
	DrawFromExtraScreen (r.corner.x + dx, r.corner.y + dy);

	if (dx || dy)
	{
	    BatchZoomedPlanet ();
	    ScrollZoomedPlanet (-dx, -dy, SURFACE_X, SURFACE_Y, GetScreenBitmap ());
	    UnbatchZoomedPlanet ();

	    FlushGraphics (TRUE);
	    
	    SetGraphicGrabOther (1);
	    LoadIntoExtraScreen (&r);
	    SetGraphicGrabOther (0);
	}
    }

    BuildObjectList ();
    
    DrawBatch (DisplayArray, DisplayLinks, 0);

    if (pMenuState->delta_item || pMenuState->CurState < 3)
    {
	BYTE	damage_index;

	lander_s.origin.x = SURFACE_WIDTH >> 1;
	lander_s.frame = LanderFrame[0];
	if (CountDown < 0)
	    lander_s.origin.y = SURFACE_HEIGHT >> 1;
	else
	    lander_s.origin.y = lander_pos[CountDown];

	if (CountDown >= 0)
	{
	    shadow_s.origin.x = lander_s.origin.y + (SURFACE_WIDTH >> 1) - (SURFACE_HEIGHT >> 1);//2;
	    shadow_s.origin.y = lander_s.origin.y;
	    shadow_s.frame = lander_s.frame;
	    SetContextForeGroundColor (BLACK_COLOR);
	    DrawFilledStamp (&shadow_s);
        }

	if ((damage_index = LOBYTE (pMenuState->delta_item)) == 0)
	    DrawStamp (&lander_s);
	else
	{
	    --pMenuState->delta_item;
	    if (!(damage_index & SHIELD_BIT))
	    {
		SetContextForeGroundColor (DamageColorCycle (0, damage_index));
		DrawFilledStamp (&lander_s);
	    }
	    else
	    {
		if ((damage_index =
			(BYTE)((damage_index & ~SHIELD_BIT) - 1)) == 0)
		    pMenuState->delta_item &= ~SHIELD_BIT;
		else
		{
		    shield_s.origin = lander_s.origin;
		    shield_s.frame = SetEquFrameIndex (
			    LanderFrame[4],
			    lander_s.frame
			    );

		    SetContextForeGroundColor (
			    BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1F, 0x1F) | 0x8000, damage_index)
			    );
		    DrawFilledStamp (&shield_s);
		}
		DrawStamp (&lander_s);
	    }
	}
    }
    
    if (CountDown < 0
	    && HIBYTE (pMenuState->delta_item)
	    && GetPredLink (DisplayLinks) != END_OF_LIST)
	CheckObjectCollision (END_OF_LIST);

    {
	PPLANETSIDE_DESC	pPSD;

	pPSD = (PPLANETSIDE_DESC)pMenuState->ModuleFrame;
	if (pPSD->NumFrames)
	{
	    --pPSD->NumFrames;
	    SetContextForeGroundColor (pPSD->ColorCycle[pPSD->NumFrames >> 1]);

	    pPSD->MineralText[0].baseline.x -= dx;
	    pPSD->MineralText[0].baseline.y -= dy;
	    DrawText (&pPSD->MineralText[0]);
	    pPSD->MineralText[1].baseline.x = pPSD->MineralText[0].baseline.x;
	    pPSD->MineralText[1].baseline.y = pPSD->MineralText[0].baseline.y + 7;
	    DrawText (&pPSD->MineralText[1]);
	    pPSD->MineralText[2].baseline.x = pPSD->MineralText[1].baseline.x;
	    pPSD->MineralText[2].baseline.y = pPSD->MineralText[1].baseline.y + 7;
	    DrawText (&pPSD->MineralText[2]);
	}
    }

    pMenuState->flash_rect0.corner.x = new_pt.x >> MAG_SHIFT;
    pMenuState->flash_rect0.corner.y = new_pt.y >> MAG_SHIFT;
    RepairScan ();

    if (lander_flags & KILL_CREW)
	DeltaLanderCrew (-1, LIGHTNING_DISASTER);

    UnbatchGraphics ();

#ifdef NEVER    
    if (CountDown == 0)
    {
	RECT	r;

	SetContext (ScanContext);
	GetContextClipRect (&r);
	r.corner.x += pMenuState->flash_rect0.corner.x - (FLASH_WIDTH >> 1);
	r.corner.y += pMenuState->flash_rect0.corner.y - (FLASH_HEIGHT >> 1);
	r.extent.width = FLASH_WIDTH;
	r.extent.height = FLASH_HEIGHT;
	LoadDisplayPixmap (&r, pMenuState->flash_frame0);
    }
#endif /* NEVER */

    SetContext (OldContext);
    ClearSemaphore (&GraphicsSem);
}

PROC(STATIC
void near AnimateLaunch, (farray, ShowPlanetSide),
    ARG		(FRAME		farray)
    ARG_END	(BOOLEAN	ShowPlanetSide)
)
{
    RECT	r;
    STAMP	s;
    COUNT	num_frames;
    DWORD	Time;

    Time = GetTimeCounter ();

    s.origin.x = s.origin.y = 0;
    s.frame = DecFrameIndex (farray);

    num_frames = GetFrameCount (s.frame);
    do
    {
	GetFrameRect (s.frame, &r);
	s.frame = IncFrameIndex (s.frame);
	RepairBackRect (&r);
	DrawStamp (&s);

	ClearSemaphore (&GraphicsSem);

#if 0
	if (ShowPlanetSide)
	{
	    do
	    {
		ScrollPlanetSide (0, 0, 0);
	    } while (GetTimeCounter () < Time + (ONE_SECOND / 22));
	}
#endif

	Time = SleepTask (Time + (ONE_SECOND / 22));

	SetSemaphore (&GraphicsSem);
    } while (--num_frames);

    GetFrameRect (s.frame, &r);
    RepairBackRect (&r);
}

PROC(STATIC
void near InitPlanetSide, (),
    ARG_VOID
)
{
    SIZE	num_crew;
    POINT	pt;
    STAMP	s;
    CONTEXT	OldContext;
    DWORD	Time;

    SetSemaphore (&GraphicsSem);
    OldContext = SetContext (RadarContext);
    ClearSemaphore (&GraphicsSem);

    Time = GetTimeCounter ();

    s.origin.x = 0;
    s.origin.y = 0;
    s.frame = SetAbsFrameIndex (LanderFrame[0],
	    (ANGLE_TO_FACING (FULL_CIRCLE) << 1) + 1);

    SetSemaphore (&GraphicsSem);
    DrawStamp (&s);
    ClearSemaphore (&GraphicsSem);

    Time = SleepTask (Time + (ONE_SECOND / 15));

    for (num_crew = 0; num_crew < (NUM_CREW_COLS * NUM_CREW_ROWS)
	    && GLOBAL_SIS (CrewEnlisted); ++num_crew)
    {
	Time = SleepTask (Time + 4);
	SetSemaphore (&GraphicsSem);
	DeltaSISGauges (-1, 0, 0);
	DeltaLanderCrew (1, 0);
	ClearSemaphore (&GraphicsSem);
    }

    Time = SleepTask (Time + (ONE_SECOND / 15));

    if (GET_GAME_STATE (IMPROVED_LANDER_SHOT))
	s.frame = SetAbsFrameIndex (s.frame, 59);
    else
	s.frame = SetAbsFrameIndex (s.frame,
		(ANGLE_TO_FACING (FULL_CIRCLE) << 1) + 2);
    SetSemaphore (&GraphicsSem);
    DrawStamp (&s);
    ClearSemaphore (&GraphicsSem);

    Time = SleepTask (Time + (ONE_SECOND / 15));

    if (GET_GAME_STATE (IMPROVED_LANDER_SPEED))
	s.frame = SetAbsFrameIndex (s.frame, 57);
    else
    {
	s.frame = SetAbsFrameIndex (s.frame,
		(ANGLE_TO_FACING (FULL_CIRCLE) << 1) + 3);
	SetSemaphore (&GraphicsSem);
	DrawStamp (&s);
	ClearSemaphore (&GraphicsSem);

	Time = SleepTask (Time + (ONE_SECOND / 15));

	s.frame = IncFrameIndex (s.frame);
    }
    SetSemaphore (&GraphicsSem);
    DrawStamp (&s);
    ClearSemaphore (&GraphicsSem);

    if (GET_GAME_STATE (IMPROVED_LANDER_CARGO))
    {
	Time = SleepTask (Time + (ONE_SECOND / 15));

	s.frame = SetAbsFrameIndex (s.frame, 58);
	SetSemaphore (&GraphicsSem);
	DrawStamp (&s);
	ClearSemaphore (&GraphicsSem);
    }
    SleepTask (Time + (ONE_SECOND / 15));

    SetSemaphore (&GraphicsSem);
    PlaySound (SetAbsSoundIndex (LanderSounds, LANDER_DEPARTS),
	    GAME_SOUND_PRIORITY + 1);
    SetContext (SpaceContext);
    AnimateLaunch (LanderFrame[5], FALSE);

#define RANDOM_MISS	64
    pt.x = pSolarSysState->MenuState.first_item.x
	    - RANDOM_MISS + (SIZE)(LOWORD (random ()) % (RANDOM_MISS << 1));
    if (pt.x < 0)
	pt.x += (MAP_WIDTH << MAG_SHIFT);
    else if (pt.x >= (MAP_WIDTH << MAG_SHIFT))
	pt.x -= (MAP_WIDTH << MAG_SHIFT);

    pt.y = pSolarSysState->MenuState.first_item.y
	    - RANDOM_MISS + (SIZE)(LOWORD (random ()) % (RANDOM_MISS << 1));
    if (pt.y < 0)
	pt.y = 0;
    else if (pt.y >= (MAP_HEIGHT << MAG_SHIFT))
	pt.y = (MAP_HEIGHT << MAG_SHIFT) - 1;

    pSolarSysState->MenuState.first_item = pt;
    pt.x >>= MAG_SHIFT;
    pt.y >>= MAG_SHIFT;

#ifdef NEVER
    SetContext (ScanContext);
    s.origin = pMenuState->flash_rect0.corner;
    s.origin.x -= (FLASH_WIDTH >> 1);
    s.origin.y -= (FLASH_HEIGHT >> 1);
    s.frame = pMenuState->flash_frame0;
    DrawStamp (&s);
#endif /* NEVER */

    pMenuState->flash_rect0.corner = pt;
#ifdef NEVER
    {
	RECT	r;
	    
	GetContextClipRect (&r);
	r.corner.x += pMenuState->flash_rect0.corner.x - (FLASH_WIDTH >> 1);
	r.corner.y += pMenuState->flash_rect0.corner.y - (FLASH_HEIGHT >> 1);
	r.extent.width = FLASH_WIDTH;
	r.extent.height = FLASH_HEIGHT;

	LoadDisplayPixmap (&r, pMenuState->flash_frame0);
    }
#endif /* NEVER */
    SetContext (SpaceContext);
    SetContextFont (TinyFont);

    pt.x = pSolarSysState->MenuState.first_item.x - (SURFACE_WIDTH >> 1); // was SPACE_
    if (pt.x < 0)
	pt.x += MAP_WIDTH << MAG_SHIFT;
    pt.y = pSolarSysState->MenuState.first_item.y - (SURFACE_HEIGHT >> 1); // was SPACE_

    SetPlanetZoom ();
    
    {
	RECT		r;
	DRAW_STATE	OldDrawState;
    
	r.corner.x = SURFACE_X;
	r.corner.y = SURFACE_Y;
	r.extent.width = SURFACE_WIDTH;
	r.extent.height = SURFACE_HEIGHT;
	SetContextClipRect (&r);
	SetContextClipping (TRUE);

	OldDrawState = SetContextDrawState (DEST_PIXMAP | DRAW_SUBTRACTIVE); // to BUILD_FRAME
	BatchGraphics ();
	SetContextDrawState (OldDrawState);
	
	BatchZoomedPlanet ();
 	RepairZoomedPlanet (pt.x, pt.y, SURFACE_WIDTH, SURFACE_HEIGHT);
	UnbatchZoomedPlanet ();
	
	ScreenTransition (3, &r);	
	UnbatchGraphics ();

	LoadIntoExtraScreen (&r);
    }

    SetContext (OldContext);

    ClearSISRect (CLEAR_SIS_RADAR);

    ClearSemaphore (&GraphicsSem);

    SET_GAME_STATE (PLANETARY_LANDING, 1);
}

PROC(STATIC
BOOLEAN	DoPlanetSide, (InputState, pMS),
    ARG		(INPUT_STATE	InputState)
    ARG_END	(PMENU_STATE	pMS)
)
{
#define NUM_LANDING_DELTAS	10
#define SHUTTLE_TURN_WAIT	2
#define SHUTTLE_FIRE_WAIT	15
    SIZE	index;

    if (GLOBAL (CurrentActivity) & CHECK_ABORT)
	return (FALSE);

    pMS->MenuRepeatDelay = 0;
    if (!pMS->Initialized)
    {
	pMS->Initialized = TRUE;
	pMS->CurState = 0;

	index = FACING_TO_ANGLE (GetFrameIndex (LanderFrame[0]));
	if (!GET_GAME_STATE (IMPROVED_LANDER_SPEED))
	    SetVelocityComponents (
		    &GLOBAL (velocity),
		    COSINE (index, WORLD_TO_VELOCITY (2 * 8)) / LANDER_SPEED_DENOM,
		    SINE (index, WORLD_TO_VELOCITY (2 * 8)) / LANDER_SPEED_DENOM
		    );
	else
	    SetVelocityComponents (
		    &GLOBAL (velocity),
		    COSINE (index, WORLD_TO_VELOCITY (2 * 14)) / LANDER_SPEED_DENOM,
		    SINE (index, WORLD_TO_VELOCITY (2 * 14)) / LANDER_SPEED_DENOM
		    );
#ifdef FAST_FAST
SetVelocityComponents (
	&GLOBAL (velocity),
	COSINE (index, WORLD_TO_VELOCITY (48)) / LANDER_SPEED_DENOM,
	SINE (index, WORLD_TO_VELOCITY (48)) / LANDER_SPEED_DENOM
	);
#endif
    }
    else if (pMS->delta_item == 0
	    || (HIBYTE (pMS->delta_item) && ((InputState & (DEVICE_BUTTON3 | DEVICE_LEFTSHIFT))
	    || (
	    (PPLANETSIDE_DESC)pMenuState->ModuleFrame
	    )->InTransit)))
    {
	if (pMS->delta_item || pMS->CurState > EXPLOSION_LIFE + 60)
	    return (FALSE);
	else
	{
	    if (pMS->CurState > EXPLOSION_LIFE)
		++pMS->CurState;
	    else if (pMS->CurState == 0)
	    {
		HELEMENT	hExplosionElement;

		++pMS->CurState;
		if (hExplosionElement = AllocElement ())
		{
		    ELEMENTPTR	ExplosionElementPtr;

		    LockElement (hExplosionElement, &ExplosionElementPtr);

		    ExplosionElementPtr->mass_points = DEATH_EXPLOSION;
		    ExplosionElementPtr->state_flags = FINITE_LIFE | GOOD_GUY;
		    ExplosionElementPtr->next.location =
			    pSolarSysState->MenuState.first_item;
		    ExplosionElementPtr->preprocess_func = object_animation;
		    ExplosionElementPtr->turn_wait = MAKE_BYTE (2, 2);
		    ExplosionElementPtr->life_span =
			    EXPLOSION_LIFE
			    * (LONIBBLE (ExplosionElementPtr->turn_wait) + 1);

		    SetPrimType (&DisplayArray[ExplosionElementPtr->PrimIndex], STAMP_PRIM);
		    DisplayArray[ExplosionElementPtr->PrimIndex].Object.Stamp.frame =
			    SetAbsFrameIndex (
			    LanderFrame[0], 46
			    );

		    UnlockElement (hExplosionElement);

		    InsertElement (hExplosionElement, GetHeadElement ());

		    PlaySound (SetAbsSoundIndex (
			    LanderSounds, LANDER_DESTROYED
			    ), GAME_SOUND_PRIORITY + 1);
		}
	    }

	    ScrollPlanetSide (0, 0, -1);
	}
    }
    else
    {
	SIZE	dx, dy;

	if (HIBYTE (pMS->delta_item) == 0)
	    dx = dy = 0;
	else
	{
	    index = GetFrameIndex (LanderFrame[0]);
	    if (LONIBBLE (pMS->CurState))
		pMS->CurState -= MAKE_BYTE (1, 0);
	    else if (dx = GetInputXComponent (InputState))
	    {
		if (dx < 0)
		    --index;
		else
		    ++index;

		index = NORMALIZE_FACING (index);
		LanderFrame[0] =
			SetAbsFrameIndex (LanderFrame[0], index);

		dx = FACING_TO_ANGLE (index);
		if (!GET_GAME_STATE (IMPROVED_LANDER_SPEED))
		    SetVelocityComponents (
			    &GLOBAL (velocity),
			    COSINE (dx, WORLD_TO_VELOCITY (2 * 8)) / LANDER_SPEED_DENOM,
			    SINE (dx, WORLD_TO_VELOCITY (2 * 8)) / LANDER_SPEED_DENOM
			    );
		else
		    SetVelocityComponents (
			    &GLOBAL (velocity),
			    COSINE (dx, WORLD_TO_VELOCITY (2 * 14)) / LANDER_SPEED_DENOM,
			    SINE (dx, WORLD_TO_VELOCITY (2 * 14)) / LANDER_SPEED_DENOM
			    );
#ifdef FAST_FAST
SetVelocityComponents (
	&GLOBAL (velocity),
	COSINE (dx, WORLD_TO_VELOCITY (48)) / LANDER_SPEED_DENOM,
	SINE (dx, WORLD_TO_VELOCITY (48)) / LANDER_SPEED_DENOM
	);
#endif

		pMS->CurState = MAKE_BYTE (
			SHUTTLE_TURN_WAIT, HINIBBLE (pMS->CurState)
			);
	    }

	    if (!(InputState & DEVICE_BUTTON1)
		    && GetInputYComponent (InputState) >= 0)
		dx = dy = 0;
	    else
		GetNextVelocityComponents (
			&GLOBAL (velocity), &dx, &dy, 1
			);

	    if (HINIBBLE (pMS->CurState))
		pMS->CurState -= MAKE_BYTE (0, 1);
	    else if (InputState & (DEVICE_BUTTON2 | DEVICE_RIGHTSHIFT))
	    {
		HELEMENT	hWeaponElement;

		if (hWeaponElement = AllocElement ())
		{
		    SIZE		wdx, wdy;
		    ELEMENTPTR	WeaponElementPtr;

		    LockElement (hWeaponElement, &WeaponElementPtr);

		    WeaponElementPtr->mass_points = 1;
		    WeaponElementPtr->life_span = 12;
		    WeaponElementPtr->state_flags = FINITE_LIFE | GOOD_GUY;
		    WeaponElementPtr->next.location =
			    pSolarSysState->MenuState.first_item;
		    WeaponElementPtr->current.location.x =
			    WeaponElementPtr->next.location.x >> MAG_SHIFT;
		    WeaponElementPtr->current.location.y =
			    WeaponElementPtr->next.location.y >> MAG_SHIFT;

		    SetPrimType (&DisplayArray[WeaponElementPtr->PrimIndex], STAMP_PRIM);
		    DisplayArray[WeaponElementPtr->PrimIndex].Object.Stamp.frame =
			    SetAbsFrameIndex (
			    LanderFrame[0],
			    index + ANGLE_TO_FACING (FULL_CIRCLE)
			    );

		    if (!(InputState & DEVICE_BUTTON1)
			    && GetInputYComponent (InputState) >= 0)
			wdx = wdy = 0;
		    else
			GetCurrentVelocityComponents (
				&GLOBAL (velocity), &wdx, &wdy
				);
		    index = FACING_TO_ANGLE (index);
		    SetVelocityComponents (
			    &WeaponElementPtr->velocity,
			    COSINE (index, WORLD_TO_VELOCITY (2 * 3)) + wdx,
			    SINE (index, WORLD_TO_VELOCITY (2 * 3)) + wdy
			    );

		    UnlockElement (hWeaponElement);

		    InsertElement (hWeaponElement, GetHeadElement ());

		    PlaySound (SetAbsSoundIndex (
			    LanderSounds, LANDER_SHOOTS
			    ), GAME_SOUND_PRIORITY);

		    wdx = SHUTTLE_FIRE_WAIT;
		    if (GET_GAME_STATE (IMPROVED_LANDER_SHOT))
			wdx >>= 1;
		    pMS->CurState = MAKE_BYTE (LONIBBLE (pMS->CurState), wdx);
		}
	    }
	}

	ScrollPlanetSide (dx, dy, -1);
    }

    return (TRUE);
}

PROC(
void FreeLanderData, (),
    ARG_VOID
)
{
    if (LanderFrame[0])
    {
	COUNT	i;

	UninitPlanetStuff ();

	for (i = 0; i < NUM_ORBIT_THEMES; ++i)
	{
	    DestroyMusic (OrbitMusic[i]);
	    OrbitMusic[i] = 0;
	}
	DestroySound (ReleaseSound (LanderSounds));
	LanderSounds = 0;

	for (i = 0; i < sizeof (LanderFrame)
		/ sizeof (LanderFrame[0]); ++i)
	{
	    DestroyDrawable (ReleaseDrawable (LanderFrame[i]));
	    LanderFrame[i] = 0;
	}
    }
}

PROC(
void LoadLanderData, (),
    ARG_VOID
)
{
    if (LanderFrame[0] == 0)
    {
	LanderFrame[0] =
		CaptureDrawable (
		LoadGraphic (LANDER_MASK_PMAP_ANIM)
		);
	LanderFrame[1] =
		CaptureDrawable (
		LoadGraphic (QUAKE_MASK_PMAP_ANIM)
		);
	LanderFrame[2] =
		CaptureDrawable (
		LoadGraphic (LIGHTNING_MASK_ANIM)
		);
	LanderFrame[3] =
		CaptureDrawable (
		LoadGraphic (LAVA_MASK_PMAP_ANIM)
		);
	LanderFrame[4] =
		CaptureDrawable (
		LoadGraphic (LANDER_SHIELD_MASK_ANIM)
		);
	LanderFrame[5] =
		CaptureDrawable (
		LoadGraphic (LANDER_LAUNCH_MASK_PMAP_ANIM)
		);
	LanderFrame[6] =
		CaptureDrawable (
		LoadGraphic (LANDER_RETURN_MASK_PMAP_ANIM)
		);
	LanderSounds = CaptureSound (LoadSound (LANDER_SOUNDS));

	LanderFrame[7] =
		CaptureDrawable (
		LoadGraphic (ORBIT_VIEW_ANIM)
		);
	InitPlanetStuff (
		SetAbsFrameIndex (LanderFrame[7], ANGLE_TO_FACING (FULL_CIRCLE)),
		SetAbsFrameIndex (LanderFrame[7], ANGLE_TO_FACING (FULL_CIRCLE) + 1),
		SetAbsFrameIndex (LanderFrame[7], ANGLE_TO_FACING (FULL_CIRCLE) + 2)
		);

	{
	    COUNT		i;
	    RES_TYPE		rt;
	    RES_INSTANCE	ri;
	    RES_PACKAGE		rp;

	    rt = GET_TYPE (ORBIT1_MUSIC);
	    ri = GET_INSTANCE (ORBIT1_MUSIC);
	    rp = GET_PACKAGE (ORBIT1_MUSIC);
	    for (i = 0; i < NUM_ORBIT_THEMES; ++i, ++ri)
		OrbitMusic[i] = LoadMusicInstance (MAKE_RESOURCE (rp, rt, ri));
	}
    }
}

PROC(
void SetPlanetMusic, (planet_type),
    ARG_END	(BYTE	planet_type)
)
{
    LanderMusic = OrbitMusic[planet_type % NUM_ORBIT_THEMES];
}

PROC(STATIC
void ReturnToOrbit, (pRect),
    ARG_END	(PRECT	pRect)
)
{
    CONTEXT	OldContext;
    
    SetSemaphore (&GraphicsSem);
    OldContext = SetContext (SpaceContext);
    SetContextClipRect (pRect);

    BatchGraphics ();
    ScreenOrigin (Screen, 0, 0);
    DrawStarBackGround (TRUE);
    FlushGraphics (TRUE);
    SetContext (ScanContext);
    DrawPlanet (0, 0, 0, 0);
    DrawScannedObjects (TRUE);
    ScreenTransition (3, pRect);
    UnbatchGraphics ();

    LoadIntoExtraScreen (pRect);
    SetContext (OldContext);
    ClearSemaphore (&GraphicsSem);

    SetPlanetTilt (0);
}

PROC(
void PlanetSide, (pMS),
    ARG_END	(PMENU_STATE	pMS)
)
{
    SIZE		index;
    DWORD		TimeIn;
    PLANETSIDE_DESC	PSD;
    BYTE		TectonicsChanceTab[] = {0*3, 0*3, 1*3, 2*3, 4*3, 8*3, 16*3, 32*3};
    BYTE		WeatherChanceTab[] = {0*3, 0*3, 1*3, 2*3, 3*3, 6*3, 12*3, 24*3};
    BYTE		FireChanceTab[] = {0*3, 0*3, 1*3, 2*3, 4*3, 12*3, 24*3, 48*3};

    MEMSET ((PPLANETSIDE_DESC)&PSD, 0, sizeof (PSD));
    PSD.InTransit = TRUE;
    PSD.OldMenuSounds = MenuSounds;
    MenuSounds = 0;

    PSD.TectonicsChance =
	    TectonicsChanceTab[pSolarSysState->SysInfo.PlanetInfo.Tectonics];
    PSD.WeatherChance =
	    WeatherChanceTab[pSolarSysState->SysInfo.PlanetInfo.Weather];
    index = pSolarSysState->SysInfo.PlanetInfo.SurfaceTemperature;
    if (index < 50)
	PSD.FireChance = FireChanceTab[0];
    else if (index < 100)
	PSD.FireChance = FireChanceTab[1];
    else if (index < 150)
	PSD.FireChance = FireChanceTab[2];
    else if (index < 250)
	PSD.FireChance = FireChanceTab[3];
    else if (index < 350)
	PSD.FireChance = FireChanceTab[4];
    else if (index < 550)
	PSD.FireChance = FireChanceTab[5];
    else if (index < 800)
	PSD.FireChance = FireChanceTab[6];
    else
	PSD.FireChance = FireChanceTab[7];

    PSD.ElementLevel = GetSBayCapacity (NULL_PTR)
	    - GLOBAL_SIS (TotalElementMass);
    PSD.MaxElementLevel = MAX_SCROUNGED;
    if (GET_GAME_STATE (IMPROVED_LANDER_CARGO))
	PSD.MaxElementLevel <<= 1;
    if (PSD.ElementLevel < PSD.MaxElementLevel)
	PSD.MaxElementLevel = PSD.ElementLevel;
    PSD.ElementLevel = 0;

    PSD.MineralText[0].align = ALIGN_CENTER;
    PSD.MineralText[0].lpStr = (LPBYTE)PSD.AmountBuf;
    PSD.MineralText[1] = PSD.MineralText[0];
    PSD.MineralText[2] = PSD.MineralText[1];

    PSD.ColorCycle[0] = BUILD_COLOR (MAKE_RGB15 (0x1F, 0x3, 0x00), 0x7F);
    PSD.ColorCycle[1] = BUILD_COLOR (MAKE_RGB15 (0x1F, 0xA, 0x00), 0x7D);
    PSD.ColorCycle[2] = BUILD_COLOR (MAKE_RGB15 (0x1F, 0x11, 0x00), 0x7B);
    PSD.ColorCycle[3] = BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1F, 0x00), 0x71);
    for (index = 4; index < (NUM_TEXT_FRAMES >> 1) - 4; ++index)
	PSD.ColorCycle[index] = BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1F, 0x1F), 0x0F);
    PSD.ColorCycle[(NUM_TEXT_FRAMES >> 1) - 4] = BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1F, 0x00), 0x71);
    PSD.ColorCycle[(NUM_TEXT_FRAMES >> 1) - 3] = BUILD_COLOR (MAKE_RGB15 (0x1F, 0x11, 0x00), 0x7B);
    PSD.ColorCycle[(NUM_TEXT_FRAMES >> 1) - 2] = BUILD_COLOR (MAKE_RGB15 (0x1F, 0xA, 0x000), 0x7D);
    PSD.ColorCycle[(NUM_TEXT_FRAMES >> 1) - 1] = BUILD_COLOR (MAKE_RGB15 (0x1F, 0x3, 0x00), 0x7F);
    pMS->ModuleFrame = (FRAME)&PSD;

    index = NORMALIZE_FACING ((COUNT)random ());
    LanderFrame[0] = SetAbsFrameIndex (
	    LanderFrame[0], index
	    );
    pMS->delta_item = 0;
    pSolarSysState->MenuState.Initialized += 4;

    InitPlanetSide ();

    TimeIn = GetTimeCounter ();
    for (index = 0; index <= NUM_LANDING_DELTAS; ++index)
    {
	if (index == 0)
	    TimeIn += ONE_SECOND;
	else
	    TimeIn += 2;

	do
	{
	    ScrollPlanetSide (0, 0, index);
	} while (GetTimeCounter () < TimeIn);
    }
    PSD.InTransit = FALSE;

    TimeIn += 2;
    pSolarSysState->MenuState.flash_rect1.corner.x = LOWORD (TimeIn);
    pSolarSysState->MenuState.flash_rect1.corner.y = HIWORD (TimeIn);

    pMS->Initialized = FALSE;
    pMS->InputFunc = DoPlanetSide;
    DoInput ((PVOID)pMS);

    if (!(GLOBAL (CurrentActivity) & CHECK_ABORT))
    {
	SIZE	crew_left;
	RECT	r;
	CONTEXT	OldContext;

	crew_left = HIBYTE (pMS->delta_item);

	r.corner.x = SIS_ORG_X;
	r.corner.y = SIS_ORG_Y;
	r.extent.width = SIS_SCREEN_WIDTH;
	r.extent.height = SIS_SCREEN_HEIGHT;
	if (crew_left == 0)
	{
#ifdef NEVER
	    ScrollPlanetSide (0, 0, 0);
#endif /* NEVER */

	    --GLOBAL_SIS (NumLanders);
	    SetSemaphore (&GraphicsSem);
	    DrawLanders ();
	    ClearSemaphore (&GraphicsSem);

	    ReturnToOrbit (&r);
	}
	else
	{
	    PSD.InTransit = TRUE;
	    PlaySound (SetAbsSoundIndex (LanderSounds, LANDER_RETURNS),
		    GAME_SOUND_PRIORITY + 1);

	    TimeIn = GetTimeCounter ();
	    for (index = NUM_LANDING_DELTAS; index >= 0; --index)
	    {
		if (index == 0)
		    TimeIn += (ONE_SECOND >> 1);
		else
		    TimeIn += 2;

		do
		{
		    ScrollPlanetSide (0, 0, index);
		} while (GetTimeCounter () < TimeIn);
	    }

	    ReturnToOrbit (&r);

	    pSolarSysState->MenuState.Initialized -= 4;
	    SleepTask (GetTimeCounter () + 1);
	    pSolarSysState->MenuState.Initialized += 4;

	    SetSemaphore (&GraphicsSem);
	    SetContext (SpaceContext);
	    AnimateLaunch (LanderFrame[6], TRUE);
	    DeltaSISGauges (crew_left, 0, 0);

	    if (PSD.ElementLevel)
	    {
		for (index = 0; index < NUM_ELEMENT_CATEGORIES; ++index)
		{
		    GLOBAL_SIS (ElementAmounts[index]) +=
			    PSD.ElementAmounts[index];
		    GLOBAL_SIS (TotalElementMass) +=
			    PSD.ElementAmounts[index];
		}
		DrawStorageBays (FALSE);
	    }

	    ClearSemaphore (&GraphicsSem);

	    GLOBAL_SIS (TotalBioMass) += PSD.BiologicalLevel;
	}
    }

    pMS->ModuleFrame = 0;

    {
	HELEMENT	hElement, hNextElement;

	for (hElement = GetHeadElement ();
		hElement; hElement = hNextElement)
	{
	    ELEMENTPTR	ElementPtr;

	    LockElement (hElement, &ElementPtr);
	    hNextElement = _GetSuccLink (ElementPtr);
	    if (ElementPtr->state_flags & FINITE_LIFE)
	    {
		UnlockElement (hElement);

		RemoveElement (hElement);
		FreeElement (hElement);

		continue;
	    }
	    UnlockElement (hElement);
	}
    }

    ZeroVelocityComponents (&GLOBAL (velocity));
    MenuSounds = PSD.OldMenuSounds;
    
    pSolarSysState->MenuState.Initialized -= 4;
}

PROC(
void InitLander, (LanderFlags),
    ARG_END	(BYTE	LanderFlags)
)
{
    RECT	r;

    SetSemaphore (&GraphicsSem);

    SetContext (RadarContext);
    
    BatchGraphics ();
    
    r.corner.x = 0;
    r.corner.y = 0;
    r.extent.width = RADAR_WIDTH;
    r.extent.height = RADAR_HEIGHT;
    SetContextForeGroundColor (BLACK_COLOR);
    DrawFilledRectangle (&r);
    
    if (GLOBAL_SIS (NumLanders) || LanderFlags)
    {
	BYTE	ShieldFlags, capacity_shift;
	COUNT	free_space;
	STAMP	s;

	s.origin.x = 0;		/* set up powered-down lander */
	s.origin.y = 0;
	s.frame = SetAbsFrameIndex (LanderFrame[0],
		ANGLE_TO_FACING (FULL_CIRCLE) << 1);
	DrawStamp (&s);
	if (LanderFlags == 0)
	{
	    ShieldFlags = GET_GAME_STATE (LANDER_SHIELDS);
	    capacity_shift = GET_GAME_STATE (IMPROVED_LANDER_CARGO);
	}
	else
	{
	    ShieldFlags = LanderFlags &
		    ((1 << EARTHQUAKE_DISASTER)
		    | (1 << BIOLOGICAL_DISASTER)
		    | (1 << LIGHTNING_DISASTER)
		    | (1 << LAVASPOT_DISASTER));
	    s.frame = IncFrameIndex (s.frame);
	    DrawStamp (&s);
	    if (LanderFlags & (1 << (4 + 0)))
		s.frame = SetAbsFrameIndex (s.frame, 57);
	    else
	    {
		s.frame = SetAbsFrameIndex (s.frame,
			(ANGLE_TO_FACING (FULL_CIRCLE) << 1) + 3);
		DrawStamp (&s);
		s.frame = IncFrameIndex (s.frame);
	    }
	    DrawStamp (&s);
	    if (!(LanderFlags & (1 << (4 + 1))))
		capacity_shift = 0;
	    else
	    {
		capacity_shift = 1;
		s.frame = SetAbsFrameIndex (s.frame, 58);
		DrawStamp (&s);
	    }
	    if (LanderFlags & (1 << (4 + 2)))
		s.frame = SetAbsFrameIndex (s.frame, 57);
	    else
		s.frame = SetAbsFrameIndex (s.frame,
			(ANGLE_TO_FACING (FULL_CIRCLE) << 1) + 2);
	    DrawStamp (&s);
	}

	if ((free_space = GetSBayCapacity (NULL_PTR)
		- GLOBAL_SIS (TotalElementMass)) <
		(MAX_SCROUNGED << capacity_shift))
	{
	    r.corner.x = 1;
	    r.extent.width = 4;
	    r.extent.height = MAX_SCROUNGED
		    - (free_space >> capacity_shift) + 1;
	    SetContextForeGroundColor (BLACK_COLOR);
	    DrawFilledRectangle (&r);
	}

	s.frame = SetAbsFrameIndex (s.frame, 37);
	if (ShieldFlags & (1 << EARTHQUAKE_DISASTER))
	    DrawStamp (&s);
	s.frame = IncFrameIndex (s.frame);
	if (ShieldFlags & (1 << BIOLOGICAL_DISASTER))
	    DrawStamp (&s);
	s.frame = IncFrameIndex (s.frame);
	if (ShieldFlags & (1 << LIGHTNING_DISASTER))
	    DrawStamp (&s);
	s.frame = IncFrameIndex (s.frame);
	if (ShieldFlags & (1 << LAVASPOT_DISASTER))
	    DrawStamp (&s);
    }

    UnbatchGraphics ();

    ClearSemaphore (&GraphicsSem);
}
