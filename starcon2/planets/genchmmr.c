#include "starcon.h"
#include "lander.h"

PROC(
void GenerateChmmr, (control),
    ARG_END	(BYTE	control)
)
{
    switch (control)
    {
	case GENERATE_MOONS:
	    GenerateRandomIP (GENERATE_MOONS);
	    if (pSolarSysState->pBaseDesc == &pSolarSysState->PlanetDesc[1])
	    {
		COUNT	angle;
		DWORD	rand_val;

		pSolarSysState->MoonDesc[0].data_index = (BYTE)~0;
		pSolarSysState->MoonDesc[0].radius = MIN_MOON_RADIUS;
		rand_val = random ();
		angle = NORMALIZE_ANGLE (LOWORD (rand_val));
		pSolarSysState->MoonDesc[0].location.x =
			COSINE (angle, pSolarSysState->MoonDesc[0].radius);
		pSolarSysState->MoonDesc[0].location.y =
			SINE (angle, pSolarSysState->MoonDesc[0].radius);
	    }
	    break;
	case GENERATE_PLANETS:
	    GenerateRandomIP (GENERATE_PLANETS);
	    pSolarSysState->PlanetDesc[1].data_index = SAPPHIRE_WORLD;
	    if (!GET_GAME_STATE (CHMMR_UNLEASHED))
		pSolarSysState->PlanetDesc[1].data_index |= PLANET_SHIELDED;
	    pSolarSysState->PlanetDesc[1].NumPlanets = 1;
	    break;
	case GENERATE_ORBITAL:
	    if (pSolarSysState->pOrbitalDesc == &pSolarSysState->PlanetDesc[1])
	    {
		if (GET_GAME_STATE (CHMMR_UNLEASHED))
		{
		    pSolarSysState->MenuState.Initialized += 2;
		    SET_GAME_STATE (GLOBAL_FLAGS_AND_DATA, 1 << 7);
		    InitCommunication (CHMMR_CONVERSATION);
		    pSolarSysState->MenuState.Initialized -= 2;

		    if (GET_GAME_STATE (CHMMR_BOMB_STATE) == 2)
		    {
			GLOBAL (CurrentActivity) |= END_INTERPLANETARY;
		    }
		    break;
		}
		else if (GET_GAME_STATE (SUN_DEVICE_ON_SHIP)
			&& !GET_GAME_STATE (ILWRATH_DECEIVED)
			&& ActivateStarShip (ILWRATH_SHIP, SPHERE_TRACKING))
		{
		    PutGroupInfo (0L, (BYTE)~0);
		    ReinitQueue (&GLOBAL (npc_built_ship_q));

		    CloneShipFragment (ILWRATH_SHIP,
			    &GLOBAL (npc_built_ship_q), (BYTE)~0);

		    pSolarSysState->MenuState.Initialized += 2;
		    SET_GAME_STATE (GLOBAL_FLAGS_AND_DATA, 1 << 6);
		    GLOBAL (CurrentActivity) |= START_INTERPLANETARY;
		    InitCommunication (ILWRATH_CONVERSATION);
		    pSolarSysState->MenuState.Initialized -= 2;

		    if (!(GLOBAL (CurrentActivity) & (CHECK_ABORT | CHECK_LOAD)))
		    {
			GLOBAL (CurrentActivity) &= ~START_INTERPLANETARY;
			ReinitQueue (&GLOBAL (npc_built_ship_q));
			GetGroupInfo (0L, 0);
		    }
		    break;
		}
	    }
		/* Starbase */
	    else if (pSolarSysState->pOrbitalDesc->pPrevDesc == &pSolarSysState->PlanetDesc[1]
		    && pSolarSysState->pOrbitalDesc == &pSolarSysState->MoonDesc[0])
	    {
		RECT	r;

		SetSemaphore (&GraphicsSem);

		pSolarSysState->SysInfo.PlanetInfo.LanderFont =
			CaptureFont (
				LoadGraphic (LANDER_FONT)
				);
		pSolarSysState->SysInfo.PlanetInfo.DiscoveryString =
			CaptureStringTable (
				LoadStringTable (CHMMR_BASE_STRTAB)
				);

		ScanContext = CaptureContext (CreateContext ());
		SetContext (ScanContext);
		SetContextFGFrame (Screen);
		r.corner.x = (SIS_ORG_X + SIS_SCREEN_WIDTH) - MAP_WIDTH;
		r.corner.y = (SIS_ORG_Y + SIS_SCREEN_HEIGHT) - MAP_HEIGHT;
		r.extent.width = MAP_WIDTH;
		r.extent.height = MAP_HEIGHT;
		SetContextClipRect (&r);

		DoDiscoveryReport (MenuSounds);

		SetContext (SpaceContext);
		DestroyContext (ReleaseContext (ScanContext));
		ScanContext = 0;

		DestroyStringTable (ReleaseStringTable (
			pSolarSysState->SysInfo.PlanetInfo.DiscoveryString
			));
		pSolarSysState->SysInfo.PlanetInfo.DiscoveryString = 0;
		DestroyFont (ReleaseFont (
			pSolarSysState->SysInfo.PlanetInfo.LanderFont
			));
		pSolarSysState->SysInfo.PlanetInfo.LanderFont = 0;

		ClearSemaphore (&GraphicsSem);
		break;
	    }
	default:
	    GenerateRandomIP (control);
	    break;
    }
}

