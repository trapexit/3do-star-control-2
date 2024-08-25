#include "starcon.h"
#include "lander.h"

PROC(
void GeneratePkunk, (control),
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
		    if (!GET_GAME_STATE (CLEAR_SPINDLE))
			pSolarSysState->SysInfo.PlanetInfo.CurType = 0;
		    else
			pSolarSysState->SysInfo.PlanetInfo.CurType = 1;
		    pSolarSysState->SysInfo.PlanetInfo.CurDensity = 0;
		    if (pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask[ENERGY_SCAN]
			    & (1L << i))
		    {
			pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask[ENERGY_SCAN]
				&= ~(1L << i);

			if (!GET_GAME_STATE (CLEAR_SPINDLE))
			{
			    ((PPLANETSIDE_DESC)pMenuState->ModuleFrame)->InTransit = TRUE;

			    SET_GAME_STATE (CLEAR_SPINDLE, 1);
			    SET_GAME_STATE (CLEAR_SPINDLE_ON_SHIP, 1);
			}
		    }
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
	    pSolarSysState->PlanetDesc[0].data_index = WATER_WORLD;
	    pSolarSysState->PlanetDesc[0].NumPlanets = 1;
	    pSolarSysState->PlanetDesc[0].radius = EARTH_RADIUS * 104L / 100;
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
	    if (pSolarSysState->pOrbitalDesc == &pSolarSysState->PlanetDesc[0])
	    {
		if (ActivateStarShip (PKUNK_SHIP, SPHERE_TRACKING))
		{
		    NotifyOthers (PKUNK_SHIP, (BYTE)~0);
		    PutGroupInfo (0L, (BYTE)~0);
		    ReinitQueue (&GLOBAL (npc_built_ship_q));

		    CloneShipFragment (PKUNK_SHIP,
			    &GLOBAL (npc_built_ship_q), (BYTE)~0);

		    pSolarSysState->MenuState.Initialized += 2;
		    GLOBAL (CurrentActivity) |= START_INTERPLANETARY;
		    SET_GAME_STATE (GLOBAL_FLAGS_AND_DATA, 1 << 7);
		    InitCommunication (PKUNK_CONVERSATION);
		    pSolarSysState->MenuState.Initialized -= 2;

		    if (!(GLOBAL (CurrentActivity) & (CHECK_ABORT | CHECK_LOAD)))
		    {
			GLOBAL (CurrentActivity) &= ~START_INTERPLANETARY;
			ReinitQueue (&GLOBAL (npc_built_ship_q));
			GetGroupInfo (0L, 0);
		    }
		    break;
		}
		else
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
				    LoadStringTable (PKUNK_RUINS_STRTAB)
				    );
		    if (GET_GAME_STATE (CLEAR_SPINDLE))
			pSolarSysState->SysInfo.PlanetInfo.DiscoveryString =
				SetAbsStringTableIndex (
				pSolarSysState->SysInfo.PlanetInfo.DiscoveryString,
				1
				);
		}
	    }
	default:
	    GenerateRandomIP (control);
	    break;
    }
}

