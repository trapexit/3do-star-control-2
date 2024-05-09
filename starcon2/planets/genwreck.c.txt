#include "starcon.h"
#include "lander.h"

PROC(
void GenerateUrquanWreck, (control),
    ARG_END	(BYTE	control)
)
{
    switch (control)
    {
	case GENERATE_ENERGY:
	    if (pSolarSysState->pOrbitalDesc == &pSolarSysState->PlanetDesc[6])
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
		if (!GET_GAME_STATE (PORTAL_KEY))
		    pSolarSysState->SysInfo.PlanetInfo.CurType = 0;
		else
		    pSolarSysState->SysInfo.PlanetInfo.CurType = 1;
		pSolarSysState->CurNode = 1;
		if (pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask[ENERGY_SCAN]
			& (1L << 0))
		{
		    pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask[ENERGY_SCAN]
			    &= ~(1L << 0);

		    if (!GET_GAME_STATE (PORTAL_KEY))
		    {
			((PPLANETSIDE_DESC)pMenuState->ModuleFrame)->InTransit = TRUE;

			SET_GAME_STATE (PORTAL_KEY, 1);
			SET_GAME_STATE (PORTAL_KEY_ON_SHIP, 1);
		    }
		}

		seed_random (old_rand);
		break;
	    }
	    pSolarSysState->CurNode = 0;
	    break;
	case GENERATE_ORBITAL:
	    if (pSolarSysState->pOrbitalDesc == &pSolarSysState->PlanetDesc[6])
	    {
		pSolarSysState->SysInfo.PlanetInfo.LanderFont =
			CaptureFont (
				LoadGraphic (LANDER_FONT)
				);
		pSolarSysState->PlanetSideFrame[1] =
			CaptureDrawable (
				LoadGraphic (WRECK_MASK_PMAP_ANIM)
				);
		pSolarSysState->SysInfo.PlanetInfo.DiscoveryString =
			CaptureStringTable (
				LoadStringTable (WRECK_STRTAB)
				);
		if (GET_GAME_STATE (PORTAL_KEY))
		    pSolarSysState->SysInfo.PlanetInfo.DiscoveryString =
			    SetAbsStringTableIndex (
			    pSolarSysState->SysInfo.PlanetInfo.DiscoveryString,
			    1
			    );
	    }
	default:
	    GenerateRandomIP (control);
	    break;
    }
}


