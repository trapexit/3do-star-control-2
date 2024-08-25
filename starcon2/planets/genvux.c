#include "starcon.h"
#include "lander.h"
#include "lifeform.h"

PROC(
void GenerateVUX, (control),
    ARG_END	(BYTE	control)
)
{
    DWORD	rand_val;

    switch (control)
    {
	case GENERATE_ENERGY:
	    if (pSolarSysState->pOrbitalDesc == &pSolarSysState->PlanetDesc[0]
		    && CurStarDescPtr->Index != VUX_BEAST_DEFINED)
	    {
		if (CurStarDescPtr->Index == MAIDENS_DEFINED
			&& !GET_GAME_STATE (SHOFIXTI_MAIDENS))
		{
		    pSolarSysState->SysInfo.PlanetInfo.CurPt.x = MAP_WIDTH / 3;
		    pSolarSysState->SysInfo.PlanetInfo.CurPt.y = MAP_HEIGHT * 5 / 8;
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
			    SET_GAME_STATE (SHOFIXTI_MAIDENS, 1);
			    SET_GAME_STATE (MAIDENS_ON_SHIP, 1);
			}
		    }
		    break;
		}
		else if (CurStarDescPtr->Index == VUX_DEFINED)
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
	    }
	    pSolarSysState->CurNode = 0;
	    break;
	case GENERATE_PLANETS:
	{
	    COUNT	angle;

	    GenerateRandomIP (GENERATE_PLANETS);

	    if (CurStarDescPtr->Index == MAIDENS_DEFINED)
	    {
		GenerateRandomIP (GENERATE_PLANETS);
		pSolarSysState->PlanetDesc[0].data_index = REDUX_WORLD;
		pSolarSysState->PlanetDesc[0].radius = EARTH_RADIUS * 212L / 100;
		angle = ARCTAN (
			pSolarSysState->PlanetDesc[0].location.x,
			pSolarSysState->PlanetDesc[0].location.y
			);
		pSolarSysState->PlanetDesc[0].location.x =
			COSINE (angle, pSolarSysState->PlanetDesc[0].radius);
		pSolarSysState->PlanetDesc[0].location.y =
			SINE (angle, pSolarSysState->PlanetDesc[0].radius);
	    }
	    else
	    {
		if (CurStarDescPtr->Index == VUX_DEFINED)
		{
		    pSolarSysState->PlanetDesc[0].data_index = REDUX_WORLD;
		    pSolarSysState->PlanetDesc[0].NumPlanets = 1;
		    pSolarSysState->PlanetDesc[0].radius = EARTH_RADIUS * 42L / 100;
		    angle = HALF_CIRCLE + OCTANT;
		}
		else /* if (CurStarDescPtr->Index == VUX_BEAST_DEFINED) */
		{
		    MEMMOVE (&pSolarSysState->PlanetDesc[1],
			    &pSolarSysState->PlanetDesc[0],
			    sizeof (pSolarSysState->PlanetDesc[0])
			    * pSolarSysState->SunDesc[0].NumPlanets);
		    ++pSolarSysState->SunDesc[0].NumPlanets;

		    angle = HALF_CIRCLE - OCTANT;
		    pSolarSysState->PlanetDesc[0].data_index = WATER_WORLD;
		    pSolarSysState->PlanetDesc[0].radius = EARTH_RADIUS * 110L / 100;
		    pSolarSysState->PlanetDesc[0].NumPlanets = 0;
		}
		pSolarSysState->PlanetDesc[0].location.x =
			COSINE (angle, pSolarSysState->PlanetDesc[0].radius);
		pSolarSysState->PlanetDesc[0].location.y =
			SINE (angle, pSolarSysState->PlanetDesc[0].radius);
		pSolarSysState->PlanetDesc[0].rand_seed = MAKE_DWORD (
			pSolarSysState->PlanetDesc[0].location.x,
			pSolarSysState->PlanetDesc[0].location.y
			);
	    }
	    break;
	}
	case GENERATE_ORBITAL:
	{
	    if ((pSolarSysState->pOrbitalDesc == &pSolarSysState->PlanetDesc[0]
		    && (CurStarDescPtr->Index == VUX_DEFINED
		    || (CurStarDescPtr->Index == MAIDENS_DEFINED
		    && !GET_GAME_STATE (ZEX_IS_DEAD))))
		    && ActivateStarShip (VUX_SHIP, SPHERE_TRACKING))
	    {
		NotifyOthers (VUX_SHIP, (BYTE)~0);
		PutGroupInfo (0L, (BYTE)~0);
		ReinitQueue (&GLOBAL (npc_built_ship_q));

		CloneShipFragment (VUX_SHIP,
			&GLOBAL (npc_built_ship_q), (BYTE)~0);
		if (CurStarDescPtr->Index == VUX_DEFINED)
		{
		    SET_GAME_STATE (GLOBAL_FLAGS_AND_DATA, 1 << 7);
		}
		else
		{
		    SET_GAME_STATE (GLOBAL_FLAGS_AND_DATA, 1 << 6);
		}

		pSolarSysState->MenuState.Initialized += 2;
		GLOBAL (CurrentActivity) |= START_INTERPLANETARY;
		InitCommunication (VUX_CONVERSATION);
		pSolarSysState->MenuState.Initialized -= 2;

		if (GLOBAL (CurrentActivity) & (CHECK_ABORT | CHECK_LOAD))
		    break;
		else
		{
		    GLOBAL (CurrentActivity) &= ~START_INTERPLANETARY;
		    ReinitQueue (&GLOBAL (npc_built_ship_q));
		    GetGroupInfo (0L, 0);

		    if (CurStarDescPtr->Index == VUX_DEFINED
			    || !GET_GAME_STATE (ZEX_IS_DEAD))
			break;

		    SetSemaphore (&GraphicsSem);
		    RepairSISBorder ();
		    ClearSemaphore (&GraphicsSem);
		}
	    }

	    if (pSolarSysState->pOrbitalDesc == &pSolarSysState->PlanetDesc[0])
	    {
		if (CurStarDescPtr->Index == MAIDENS_DEFINED)
		{
		    if (!GET_GAME_STATE (SHOFIXTI_MAIDENS))
		    {
			pSolarSysState->SysInfo.PlanetInfo.LanderFont =
				CaptureFont (
					LoadGraphic (LANDER_FONT)
					);
			pSolarSysState->PlanetSideFrame[1] =
				CaptureDrawable (
					LoadGraphic (MAIDENS_MASK_PMAP_ANIM)
					);
			pSolarSysState->SysInfo.PlanetInfo.DiscoveryString =
				CaptureStringTable (
					LoadStringTable (MAIDENS_STRTAB)
					);
		    }
		}
		else if (CurStarDescPtr->Index == VUX_BEAST_DEFINED)
		{
		    if (!GET_GAME_STATE (VUX_BEAST))
		    {
			pSolarSysState->SysInfo.PlanetInfo.LanderFont =
				CaptureFont (
					LoadGraphic (LANDER_FONT)
					);
			pSolarSysState->PlanetSideFrame[1] = 0;
			pSolarSysState->SysInfo.PlanetInfo.DiscoveryString =
				CaptureStringTable (
					LoadStringTable (BEAST_STRTAB)
					);
		    }
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
				    LoadStringTable (RUINS_STRTAB)
				    );
		}
	    }
	    GenerateRandomIP (GENERATE_ORBITAL);
	    if (pSolarSysState->pOrbitalDesc == &pSolarSysState->PlanetDesc[0])
	    {
		pSolarSysState->SysInfo.PlanetInfo.Weather = 2;
		pSolarSysState->SysInfo.PlanetInfo.Tectonics = 0;
	    }
	    break;
	}
	case GENERATE_LIFE:
	    if (CurStarDescPtr->Index == MAIDENS_DEFINED
		    && pSolarSysState->pOrbitalDesc == &pSolarSysState->PlanetDesc[0])
	    {
		COUNT	i;
		DWORD	old_rand;

		old_rand = seed_random (pSolarSysState->SysInfo.PlanetInfo.ScanSeed[BIOLOGICAL_SCAN]);

		i = 0;
		do
		{
		    rand_val = random ();
		    pSolarSysState->SysInfo.PlanetInfo.CurPt.x =
			    (LOBYTE (LOWORD (rand_val)) % (MAP_WIDTH - (8 << 1))) + 8;
		    pSolarSysState->SysInfo.PlanetInfo.CurPt.y =
			    (HIBYTE (LOWORD (rand_val)) % (MAP_HEIGHT - (8 << 1))) + 8;
		    if (i < 4)
			pSolarSysState->SysInfo.PlanetInfo.CurType = 9;
		    else if (i < 8)
			pSolarSysState->SysInfo.PlanetInfo.CurType = 14;
		    else /* if (i < 12) */
			pSolarSysState->SysInfo.PlanetInfo.CurType = 18;
		    if (i >= pSolarSysState->CurNode
			    && !(pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask[BIOLOGICAL_SCAN]
			    & (1L << i)))
			break;
		} while (++i < 12);
		pSolarSysState->CurNode = i;

		seed_random (old_rand);
		break;
	    }
	    else if (CurStarDescPtr->Index == VUX_BEAST_DEFINED
		    && pSolarSysState->pOrbitalDesc == &pSolarSysState->PlanetDesc[0])
	    {
		COUNT	i;
		DWORD	old_rand;

		old_rand = seed_random (pSolarSysState->SysInfo.PlanetInfo.ScanSeed[BIOLOGICAL_SCAN]);

		i = 0;
		do
		{
		    rand_val = random ();
		    pSolarSysState->SysInfo.PlanetInfo.CurPt.x =
			    (LOBYTE (LOWORD (rand_val)) % (MAP_WIDTH - (8 << 1))) + 8;
		    pSolarSysState->SysInfo.PlanetInfo.CurPt.y =
			    (HIBYTE (LOWORD (rand_val)) % (MAP_HEIGHT - (8 << 1))) + 8;
		    if (i == 0)
			pSolarSysState->SysInfo.PlanetInfo.CurType = NUM_CREATURE_TYPES + 2;
		    else if (i <= 5)
			    /* {SPEED_MOTIONLESS | DANGER_NORMAL, MAKE_BYTE (5, 3)}, */
			pSolarSysState->SysInfo.PlanetInfo.CurType = 3;
		    else /* if (i <= 10) */
			    /* {BEHAVIOR_UNPREDICTABLE | SPEED_SLOW | DANGER_NORMAL, MAKE_BYTE (3, 8)}, */
			pSolarSysState->SysInfo.PlanetInfo.CurType = 8;
		    if (i >= pSolarSysState->CurNode
			    && !(pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask[BIOLOGICAL_SCAN]
			    & (1L << i)))
			break;
		    else if (i == 0
			    && (pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask[BIOLOGICAL_SCAN]
			    & (1L << i))
			    && !GET_GAME_STATE (VUX_BEAST))
		    {
			PPLANETSIDE_DESC	pPSD;

			pPSD = (PPLANETSIDE_DESC)pMenuState->ModuleFrame;
			UnbatchGraphics ();
		        DoDiscoveryReport (pPSD->OldMenuSounds);
			BatchGraphics ();
			pPSD->InTransit = TRUE;

			SET_GAME_STATE (VUX_BEAST, 1);
			SET_GAME_STATE (VUX_BEAST_ON_SHIP, 1);
		    }
		} while (++i < 11);
		pSolarSysState->CurNode = i;

		seed_random (old_rand);
		break;
	    }
	default:
	    GenerateRandomIP (control);
	    break;
    }
}

