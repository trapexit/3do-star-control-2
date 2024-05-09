#include "starcon.h"

PROC(
void GenerateBurvixes, (control),
    ARG_END	(BYTE	control)
)
{
    COUNT	i;
    DWORD	rand_val;

    switch (control)
    {
	case GENERATE_ENERGY:
	{
	    DWORD	rand_val, old_rand;

	    if (pSolarSysState->pOrbitalDesc == &pSolarSysState->PlanetDesc[0])
	    {
		COUNT	which_node;

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
	    else if (pSolarSysState->pOrbitalDesc->pPrevDesc == &pSolarSysState->PlanetDesc[0]
		    && pSolarSysState->pOrbitalDesc == &pSolarSysState->MoonDesc[0]
		    && !GET_GAME_STATE (BURVIXESE_BROADCASTERS))
	    {
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
			SET_GAME_STATE (BURVIXESE_BROADCASTERS, 1);
			SET_GAME_STATE (BURV_BROADCASTERS_ON_SHIP, 1);
		    }
		}

		seed_random (old_rand);
		break;
	    }
	    pSolarSysState->CurNode = 0;
	    break;
	}
	case GENERATE_MOONS:
	    GenerateRandomIP (GENERATE_MOONS);
	    if (pSolarSysState->pBaseDesc == &pSolarSysState->PlanetDesc[0])
	    {
		COUNT	angle;

		pSolarSysState->MoonDesc[0].data_index = SELENIC_WORLD;
		pSolarSysState->MoonDesc[0].radius = MIN_MOON_RADIUS
			+ (MAX_MOONS - 1) * MOON_DELTA;
		rand_val = random ();
		angle = NORMALIZE_ANGLE (LOWORD (rand_val));
		pSolarSysState->MoonDesc[0].location.x =
			COSINE (angle, pSolarSysState->MoonDesc[0].radius);
		pSolarSysState->MoonDesc[0].location.y =
			SINE (angle, pSolarSysState->MoonDesc[0].radius);
	    }
	    break;
	case GENERATE_PLANETS:
	{
	    COUNT	angle;

	    GenerateRandomIP (GENERATE_PLANETS);

	    pSolarSysState->PlanetDesc[0].data_index = REDUX_WORLD;
	    pSolarSysState->PlanetDesc[0].NumPlanets = 1;
	    pSolarSysState->PlanetDesc[0].radius = EARTH_RADIUS * 39L / 100;
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
	{
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
	    if (pSolarSysState->pOrbitalDesc == &pSolarSysState->PlanetDesc[0])
	    {
		pSolarSysState->SysInfo.PlanetInfo.LanderFont =
			CaptureFont (LoadGraphic (LANDER_FONT));
		pSolarSysState->PlanetSideFrame[1] =
			CaptureDrawable (
			LoadGraphic (RUINS_MASK_PMAP_ANIM)
			);
		pSolarSysState->SysInfo.PlanetInfo.DiscoveryString =
			CaptureStringTable (
				LoadStringTable (BURV_RUINS_STRTAB)
				);
		pSolarSysState->SysInfo.PlanetInfo.Weather = 0;
		pSolarSysState->SysInfo.PlanetInfo.Tectonics = 0;
	    }
	    else if (pSolarSysState->pOrbitalDesc->pPrevDesc == &pSolarSysState->PlanetDesc[0]
		    && pSolarSysState->pOrbitalDesc == &pSolarSysState->MoonDesc[0]
		    && !GET_GAME_STATE (BURVIXESE_BROADCASTERS))
	    {
		pSolarSysState->SysInfo.PlanetInfo.LanderFont =
			CaptureFont (LoadGraphic (LANDER_FONT));
		pSolarSysState->PlanetSideFrame[1] =
			CaptureDrawable (
			LoadGraphic (BURV_BCS_MASK_PMAP_ANIM)
			);
		pSolarSysState->SysInfo.PlanetInfo.DiscoveryString =
			CaptureStringTable (
				LoadStringTable (BURV_BCS_STRTAB)
				);
	    }
	    LoadPlanet (FALSE);
	    break;
	}
	default:
	    GenerateRandomIP (control);
	    break;
    }
}

