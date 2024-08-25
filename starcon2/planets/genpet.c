#include "starcon.h"

PROC(STATIC
void near ZapToUrquanEncounter, (),
    ARG_VOID
)
{
    HENCOUNTER		hEncounter;

    if ((hEncounter = AllocEncounter ()) || (hEncounter = GetHeadEncounter ()))
    {
	SIZE				dx, dy;
	ENCOUNTERPTR			EncounterPtr;
	HSTARSHIP			hStarShip;
	EXTENDED_SHIP_FRAGMENTPTR	TemplatePtr;

	LockEncounter (hEncounter, &EncounterPtr);

	if (hEncounter == GetHeadEncounter ())
	    RemoveEncounter (hEncounter);
	MEMSET (EncounterPtr, 0, sizeof (*EncounterPtr));

	InsertEncounter (hEncounter, GetHeadEncounter ());

	hStarShip = GetStarShipFromIndex (&GLOBAL (avail_race_q), URQUAN_SHIP);
	TemplatePtr = (EXTENDED_SHIP_FRAGMENTPTR)LockStarShip (
		&GLOBAL (avail_race_q),
		hStarShip
		);
	EncounterPtr->origin = TemplatePtr->ShipInfo.loc;
	EncounterPtr->radius = TemplatePtr->ShipInfo.actual_strength;
	EncounterPtr->SD.Type = URQUAN_SHIP;
	EncounterPtr->SD.Index = MAKE_BYTE (1, 0) | ONE_SHOT_ENCOUNTER;
	EncounterPtr->SD.ShipList[0] =
		((SHIP_FRAGMENTPTR)TemplatePtr)->ShipInfo;
	EncounterPtr->SD.ShipList[0].var1 = URQUAN_SHIP;
	EncounterPtr->SD.star_pt.x = 5288;
	EncounterPtr->SD.star_pt.y = 4892;
	EncounterPtr->log_x = UNIVERSE_TO_LOGX (EncounterPtr->SD.star_pt.x);
	EncounterPtr->log_y = UNIVERSE_TO_LOGY (EncounterPtr->SD.star_pt.y);
	GLOBAL_SIS (log_x) = EncounterPtr->log_x;
	GLOBAL_SIS (log_y) = EncounterPtr->log_y;
	UnlockStarShip (
		&GLOBAL (avail_race_q),
		hStarShip
		);

	{
#define LOST_DAYS	15
	    COUNT	i;
	    BYTE	black_buf[] = {FadeAllToBlack};

	    SleepTask (XFormColorMap ((COLORMAPPTR)black_buf, ONE_SECOND * 2));
	    for (i = 0; i < LOST_DAYS; ++i)
	    {
		while (ClockTick () > 0)
		    ;

		ResumeGameClock ();
		SleepTask (GetTimeCounter () + 2);
		SuspendGameClock ();
	    }
	}

	GLOBAL (CurrentActivity) = MAKE_WORD (IN_HYPERSPACE, 0) | START_ENCOUNTER;

	dx = CurStarDescPtr->star_pt.x - EncounterPtr->SD.star_pt.x;
	dy = CurStarDescPtr->star_pt.y - EncounterPtr->SD.star_pt.y;
	dx = (SIZE)square_root ((long)dx * dx + (long)dy * dy)
		+ (FUEL_TANK_SCALE >> 1);

	SetSemaphore (&GraphicsSem);
	DeltaSISGauges (0, -dx, 0);
	if (GLOBAL_SIS (FuelOnBoard) < 5 * FUEL_TANK_SCALE)
	{
	    dx = ((5 + ((COUNT)random () % 5)) * FUEL_TANK_SCALE)
		    - (SIZE)GLOBAL_SIS (FuelOnBoard);
	    DeltaSISGauges (0, dx, 0);
	}
	DrawSISMessage (NULL_PTR);
	DrawHyperCoords (EncounterPtr->SD.star_pt);
	ClearSemaphore (&GraphicsSem);

	UnlockEncounter (hEncounter);
    }
}

PROC(
void GenerateTalkingPet, (control),
    ARG_END	(BYTE	control)
)
{
    switch (control)
    {
	case GENERATE_ENERGY:
	    if (pSolarSysState->pOrbitalDesc == &pSolarSysState->PlanetDesc[0])
	    {
		COUNT	i, which_node;
		DWORD	rand_val, old_rand;

		old_rand = seed_random (
			pSolarSysState->SysInfo.PlanetInfo.ScanSeed[ENERGY_SCAN]
			);

		which_node = i = 0;
		do
		{
		    rand_val = random ();
		    pSolarSysState->SysInfo.PlanetInfo.CurPt.x =
			    (LOBYTE (LOWORD (rand_val)) % (MAP_WIDTH - (8 << 1))) + 8;
		    pSolarSysState->SysInfo.PlanetInfo.CurPt.y =
			    (HIBYTE (LOWORD (rand_val)) % (MAP_HEIGHT - (8 << 1))) + 8;
		    pSolarSysState->SysInfo.PlanetInfo.CurType = 1;
		    pSolarSysState->SysInfo.PlanetInfo.CurDensity = 0;
		    if (which_node >= pSolarSysState->CurNode
			    && !(pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask[ENERGY_SCAN]
			    & (1L << i)))
			break;
		    ++which_node;
		} while (++i < 16);
		pSolarSysState->CurNode = which_node;

		seed_random (old_rand);
		break;
	    }
	    pSolarSysState->CurNode = 0;
	    break;
	case GENERATE_PLANETS:
	{
	    COUNT	angle;

	    GenerateRandomIP (GENERATE_PLANETS);
	    pSolarSysState->PlanetDesc[0].data_index = TELLURIC_WORLD;
	    pSolarSysState->PlanetDesc[0].radius = EARTH_RADIUS * 204L / 100;
	    angle = ARCTAN (
		    pSolarSysState->PlanetDesc[0].location.x,
		    pSolarSysState->PlanetDesc[0].location.y
		    );
	    pSolarSysState->PlanetDesc[0].location.x =
		    COSINE (angle, pSolarSysState->PlanetDesc[0].radius);
	    pSolarSysState->PlanetDesc[0].location.y =
		    SINE (angle, pSolarSysState->PlanetDesc[0].radius);
	    break;
	}
	case GENERATE_ORBITAL:
	    if (pSolarSysState->pOrbitalDesc == &pSolarSysState->PlanetDesc[0]
		    && (GET_GAME_STATE (UMGAH_ZOMBIE_BLOBBIES)
		    || !GET_GAME_STATE (TALKING_PET)
		    || ActivateStarShip (UMGAH_SHIP, SPHERE_TRACKING)))
	    {
		NotifyOthers (UMGAH_SHIP, (BYTE)~0);
		PutGroupInfo (0L, (BYTE)~0);
		ReinitQueue (&GLOBAL (npc_built_ship_q));

		if (ActivateStarShip (UMGAH_SHIP, SPHERE_TRACKING))
		{
		    pSolarSysState->MenuState.Initialized += 2;
		    GLOBAL (CurrentActivity) |= START_INTERPLANETARY;
		    SET_GAME_STATE (GLOBAL_FLAGS_AND_DATA, 1 << 7);
		    if (!GET_GAME_STATE (UMGAH_ZOMBIE_BLOBBIES))
		    {
			CloneShipFragment (UMGAH_SHIP,
				&GLOBAL (npc_built_ship_q), (BYTE)~0);
			InitCommunication (UMGAH_CONVERSATION);
		    }
		    else
		    {
			COUNT	i;

			for (i = 0; i < 10; ++i)
			    CloneShipFragment (UMGAH_SHIP,
				    &GLOBAL (npc_built_ship_q), 0);
			InitCommunication (TALKING_PET_CONVERSATION);
		    }
		    pSolarSysState->MenuState.Initialized -= 2;
		}

		if (!(GLOBAL (CurrentActivity) & (CHECK_ABORT | CHECK_LOAD)))
		{
		    BOOLEAN	UmgahSurvivors;

		    UmgahSurvivors = GetHeadLink (
			    &GLOBAL (npc_built_ship_q)
			    ) != 0;
		    GLOBAL (CurrentActivity) &= ~START_INTERPLANETARY;

		    if (GET_GAME_STATE (PLAYER_HYPNOTIZED))
			ZapToUrquanEncounter ();
		    else if (GET_GAME_STATE (UMGAH_ZOMBIE_BLOBBIES)
			    && !UmgahSurvivors)
			InitCommunication (TALKING_PET_CONVERSATION);

		    ReinitQueue (&GLOBAL (npc_built_ship_q));
		    GetGroupInfo (0L, 0);
		}
		break;
	    }

	    if (pSolarSysState->pOrbitalDesc == &pSolarSysState->PlanetDesc[0])
	    {
		pSolarSysState->SysInfo.PlanetInfo.LanderFont =
			CaptureFont (
				LoadGraphic (LANDER_FONT)
				);
		pSolarSysState->PlanetSideFrame[1] =
			CaptureDrawable (
			LoadGraphic (RUINS_MASK_PMAP_ANIM)
			);
		pSolarSysState->SysInfo.PlanetInfo.DiscoveryString =
			CaptureStringTable (
				LoadStringTable (RUINS_STRTAB)
				);
	    }

	    GenerateRandomIP (GENERATE_ORBITAL);
	    if (pSolarSysState->pOrbitalDesc == &pSolarSysState->PlanetDesc[0])
		pSolarSysState->SysInfo.PlanetInfo.Weather = 0;
	    break;
	default:
	    GenerateRandomIP (control);
	    break;
    }
}

