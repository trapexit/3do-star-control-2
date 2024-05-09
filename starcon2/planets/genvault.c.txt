#include "starcon.h"
#include "lander.h"

PROC(
void GenerateShipVault, (control),
    ARG_END	(BYTE	control)
)
{
    switch (control)
    {
	case GENERATE_ENERGY:
	    if (pSolarSysState->pOrbitalDesc->pPrevDesc == &pSolarSysState->PlanetDesc[0]
		    && pSolarSysState->pOrbitalDesc == &pSolarSysState->MoonDesc[0])
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
		if (!GET_GAME_STATE (SHIP_VAULT_UNLOCKED))
		    pSolarSysState->SysInfo.PlanetInfo.CurType = 0;
		else
		    pSolarSysState->SysInfo.PlanetInfo.CurType = 1;
		pSolarSysState->CurNode = 1;
		if (pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask[ENERGY_SCAN]
			& (1L << 0))
		{
		    pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask[ENERGY_SCAN]
			    &= ~(1L << 0);
		    if (GET_GAME_STATE (SYREEN_SHUTTLE_ON_SHIP))
		    {
			((PPLANETSIDE_DESC)pMenuState->ModuleFrame)->InTransit = TRUE;

			SET_GAME_STATE (SHIP_VAULT_UNLOCKED, 1);
			SET_GAME_STATE (SYREEN_SHUTTLE_ON_SHIP, 0);
			SET_GAME_STATE (SYREEN_HOME_VISITS, 0);
		    }
		    else if (!GET_GAME_STATE (KNOW_SYREEN_VAULT))
		    {
			SET_GAME_STATE (KNOW_SYREEN_VAULT, 1);
			SET_GAME_STATE (SYREEN_HOME_VISITS, 0);
		    }
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
		pSolarSysState->SysInfo.PlanetInfo.LanderFont =
			CaptureFont (
				LoadGraphic (LANDER_FONT)
				);
		pSolarSysState->PlanetSideFrame[1] =
			CaptureDrawable (
				LoadGraphic (VAULT_MASK_PMAP_ANIM)
				);
		pSolarSysState->SysInfo.PlanetInfo.DiscoveryString =
			CaptureStringTable (
				LoadStringTable (VAULT_STRTAB)
				);
		if (GET_GAME_STATE (SHIP_VAULT_UNLOCKED))
		    pSolarSysState->SysInfo.PlanetInfo.DiscoveryString =
			    SetAbsStringTableIndex (
			    pSolarSysState->SysInfo.PlanetInfo.DiscoveryString,
			    2
			    );
		else if (GET_GAME_STATE (SYREEN_SHUTTLE_ON_SHIP))
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


