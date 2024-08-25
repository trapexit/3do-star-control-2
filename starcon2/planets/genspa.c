#include "starcon.h"
#include "lifeform.h"

PROC(
void GenerateSpathi, (control),
    ARG_END	(BYTE	control)
)
{
    COUNT	i, angle;
    DWORD	rand_val;

    switch (control)
    {
	case GENERATE_ENERGY:
	    if (pSolarSysState->pOrbitalDesc == &pSolarSysState->MoonDesc[0]
		    && !GET_GAME_STATE (UMGAH_BROADCASTERS))
	    {
		DWORD	rand_val, old_rand;

		old_rand = seed_random (
			pSolarSysState->SysInfo.PlanetInfo.ScanSeed[ENERGY_SCAN]
			);

		rand_val = random ();
		pSolarSysState->SysInfo.PlanetInfo.CurPt.x =
			(LOBYTE (LOWORD (rand_val)) % (MAP_WIDTH - (8 << 1))) + 8;
		pSolarSysState->SysInfo.PlanetInfo.CurPt.y =
			(HIBYTE (LOWORD (rand_val)) % (MAP_HEIGHT - (8 << 1))) + 8;
		pSolarSysState->SysInfo.PlanetInfo.CurDensity = 0;
		pSolarSysState->SysInfo.PlanetInfo.CurType = 0;
		if (!(pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask[ENERGY_SCAN]
			& (1L << 0))
			&& pSolarSysState->CurNode == (COUNT)~0)
		    pSolarSysState->CurNode = 1;
		else
		{
		    pSolarSysState->CurNode = 0;
		    if (pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask[ENERGY_SCAN]
			    & (1L << 0))
		    {
			SET_GAME_STATE (UMGAH_BROADCASTERS, 1);
			SET_GAME_STATE (UMGAH_BROADCASTERS_ON_SHIP, 1);
		    }
		}

		seed_random (old_rand);
		break;
	    }
	    pSolarSysState->CurNode = 0;
	    break;
	case GENERATE_MOONS:
	    GenerateRandomIP (GENERATE_MOONS);
	    if (pSolarSysState->pBaseDesc == &pSolarSysState->PlanetDesc[0])
	    {
#ifdef NOTYET
		strcpy (GLOBAL_SIS (PlanetName), "Spathiwa");
#endif /* NOTYET */

		pSolarSysState->MoonDesc[0].data_index = PELLUCID_WORLD;
		pSolarSysState->MoonDesc[0].radius = MIN_MOON_RADIUS + MOON_DELTA;
		angle = NORMALIZE_ANGLE (LOWORD (random ()));
		pSolarSysState->MoonDesc[0].location.x =
			COSINE (angle, pSolarSysState->MoonDesc[0].radius);
		pSolarSysState->MoonDesc[0].location.y =
			SINE (angle, pSolarSysState->MoonDesc[0].radius);
	    }
	    break;
	case GENERATE_PLANETS:
	{
	    PPLANET_DESC	pMinPlanet;

	    pMinPlanet = &pSolarSysState->PlanetDesc[0];
	    FillOrbits (pSolarSysState->SunDesc[0].NumPlanets = 1, pMinPlanet, FALSE);

	    pMinPlanet->radius = EARTH_RADIUS * 1150L / 100;
	    angle = ARCTAN (pMinPlanet->location.x, pMinPlanet->location.y);
	    pMinPlanet->location.x =
		    COSINE (angle, pMinPlanet->radius);
	    pMinPlanet->location.y =
		    SINE (angle, pMinPlanet->radius);
	    pMinPlanet->data_index = WATER_WORLD;
	    if (GET_GAME_STATE (SPATHI_SHIELDED_SELVES))
		pMinPlanet->data_index |= PLANET_SHIELDED;
	    pMinPlanet->NumPlanets = 1;
	    break;
	}
	case GENERATE_LIFE:
			/* visiting Spathiwa */
	    if (pSolarSysState->pOrbitalDesc == &pSolarSysState->PlanetDesc[0]
		    && !GET_GAME_STATE (SPATHI_SHIELDED_SELVES))
	    {
		COUNT	which_node;
		DWORD	old_rand;

		old_rand = seed_random (pSolarSysState->SysInfo.PlanetInfo.ScanSeed[BIOLOGICAL_SCAN]);

		which_node = i = 0;
		do
		{
		    rand_val = random ();
		    pSolarSysState->SysInfo.PlanetInfo.CurPt.x =
			    (LOBYTE (LOWORD (rand_val)) % (MAP_WIDTH - (8 << 1))) + 8;
		    pSolarSysState->SysInfo.PlanetInfo.CurPt.y =
			    (HIBYTE (LOWORD (rand_val)) % (MAP_HEIGHT - (8 << 1))) + 8;
		    pSolarSysState->SysInfo.PlanetInfo.CurType = NUM_CREATURE_TYPES;
		    if (which_node >= pSolarSysState->CurNode
			    && !(pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask[BIOLOGICAL_SCAN]
			    & (1L << i)))
			break;
		    ++which_node;
		} while (++i < 32);
		pSolarSysState->CurNode = which_node;
		if (pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask[BIOLOGICAL_SCAN])
		{
		    SET_GAME_STATE (SPATHI_CREATURES_EXAMINED, 1);
		    if (pSolarSysState->SysInfo.
			    PlanetInfo.ScanRetrieveMask[BIOLOGICAL_SCAN] == 0xFFFFFFFF)
			SET_GAME_STATE (SPATHI_CREATURES_ELIMINATED, 1);
		}

		seed_random (old_rand);
		break;
	    }
	    pSolarSysState->CurNode = 0;
	    break;
	case GENERATE_ORBITAL:
	    if (pSolarSysState->pOrbitalDesc->pPrevDesc == &pSolarSysState->PlanetDesc[0]
		    && pSolarSysState->pOrbitalDesc == &pSolarSysState->MoonDesc[0])
	    {
		if (!GET_GAME_STATE (SPATHI_SHIELDED_SELVES)
			&& ActivateStarShip (SPATHI_SHIP, SPHERE_TRACKING))
		{
		    NotifyOthers (SPATHI_SHIP, (BYTE)~0);
		    PutGroupInfo (0L, (BYTE)~0);
		    ReinitQueue (&GLOBAL (npc_built_ship_q));

		    CloneShipFragment (SPATHI_SHIP,
			    &GLOBAL (npc_built_ship_q), (BYTE)~0);

		    pSolarSysState->MenuState.Initialized += 2;
		    SET_GAME_STATE (GLOBAL_FLAGS_AND_DATA, 1 << 7);
		    GLOBAL (CurrentActivity) |= START_INTERPLANETARY;
		    InitCommunication (SPATHI_CONVERSATION);
		    pSolarSysState->MenuState.Initialized -= 2;

		    if (!(GLOBAL (CurrentActivity) & (CHECK_ABORT | CHECK_LOAD)))
		    {
			GLOBAL (CurrentActivity) &= ~START_INTERPLANETARY;
			ReinitQueue (&GLOBAL (npc_built_ship_q));
			GetGroupInfo (0L, 0);
		    }
		    break;
		}
		rand_val = DoPlanetaryAnalysis (
			&pSolarSysState->SysInfo, pSolarSysState->pOrbitalDesc
			);

		pSolarSysState->SysInfo.PlanetInfo.ScanSeed[BIOLOGICAL_SCAN] = rand_val;
		i = (COUNT)~0;
		rand_val = GenerateLifeForms (&pSolarSysState->SysInfo, &i);

		pSolarSysState->SysInfo.PlanetInfo.ScanSeed[MINERAL_SCAN] = rand_val;
		i = (COUNT)~0;
		GenerateMineralDeposits (&pSolarSysState->SysInfo, &i);

		pSolarSysState->SysInfo.PlanetInfo.ScanSeed[ENERGY_SCAN] = rand_val;

		pSolarSysState->SysInfo.PlanetInfo.Weather = 0;
		pSolarSysState->SysInfo.PlanetInfo.Tectonics = 0;
		pSolarSysState->SysInfo.PlanetInfo.SurfaceTemperature = 28;
		if (!GET_GAME_STATE (UMGAH_BROADCASTERS))
		{
		    pSolarSysState->SysInfo.PlanetInfo.LanderFont =
			    CaptureFont (
				    LoadGraphic (LANDER_FONT)
				    );
		    pSolarSysState->PlanetSideFrame[1] =
			    CaptureDrawable (
			    LoadGraphic (UMGAH_BCS_MASK_PMAP_ANIM)
			    );
		    pSolarSysState->SysInfo.PlanetInfo.DiscoveryString =
			    CaptureStringTable (
				    LoadStringTable (UMGAH_BCS_STRTAB)
				    );
		}
		LoadPlanet (FALSE);
		break;
	    }
	    else if (pSolarSysState->pOrbitalDesc == &pSolarSysState->PlanetDesc[0])
	    {
			/* visiting Spathiwa */
		rand_val = DoPlanetaryAnalysis (
			&pSolarSysState->SysInfo, pSolarSysState->pOrbitalDesc
			);

		pSolarSysState->SysInfo.PlanetInfo.ScanSeed[MINERAL_SCAN] = rand_val;
		i = (COUNT)~0;
		rand_val = GenerateMineralDeposits (&pSolarSysState->SysInfo, &i);

		pSolarSysState->SysInfo.PlanetInfo.ScanSeed[BIOLOGICAL_SCAN] = rand_val;

		pSolarSysState->SysInfo.PlanetInfo.PlanetRadius = 120;
		pSolarSysState->SysInfo.PlanetInfo.SurfaceGravity =
			CalcGravity (pSolarSysState->SysInfo.PlanetInfo.PlanetDensity,
			pSolarSysState->SysInfo.PlanetInfo.PlanetRadius);
		pSolarSysState->SysInfo.PlanetInfo.Weather = 0;
		pSolarSysState->SysInfo.PlanetInfo.Tectonics = 0;
		pSolarSysState->SysInfo.PlanetInfo.SurfaceTemperature = 31;

		LoadPlanet (FALSE);
		break;
	    }
	default:
	    GenerateRandomIP (control);
	    break;
    }
}


