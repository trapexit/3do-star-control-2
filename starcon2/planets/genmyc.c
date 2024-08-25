#include "starcon.h"

PROC(
void GenerateMycon, (control),
    ARG_END	(BYTE	control)
)
{
    switch (control)
    {
	case GENERATE_LIFE:
	    pSolarSysState->CurNode = 0;
	    break;
	case GENERATE_ENERGY:
	    if (CurStarDescPtr->Index != MYCON_DEFINED
		    && pSolarSysState->pOrbitalDesc == &pSolarSysState->PlanetDesc[0])
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
		if (CurStarDescPtr->Index == SUN_DEVICE_DEFINED)
		{
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
			    SET_GAME_STATE (SUN_DEVICE, 1);
			    SET_GAME_STATE (SUN_DEVICE_ON_SHIP, 1);
			    SET_GAME_STATE (MYCON_VISITS, 0);
			}
		    }
		}
		else
		{
		    if (!(pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask[ENERGY_SCAN]
			    & (1L << 0))
			    && pSolarSysState->CurNode == (COUNT)~0)
			pSolarSysState->CurNode = 1;
		    else
		    {
			pSolarSysState->CurNode = 0;
			if ((pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask[ENERGY_SCAN]
				& (1L << 0))
				&& pSolarSysState->SysInfo.PlanetInfo.DiscoveryString)
			{
			    switch (CurStarDescPtr->Index)
			    {
				case EGG_CASE0_DEFINED:
				    SET_GAME_STATE (EGG_CASE0_ON_SHIP, 1);
				    break;
				case EGG_CASE1_DEFINED:
				    SET_GAME_STATE (EGG_CASE1_ON_SHIP, 1);
				    break;
				case EGG_CASE2_DEFINED:
				    SET_GAME_STATE (EGG_CASE2_ON_SHIP, 1);
				    break;
			    }
			}
		    }
		}

		seed_random (old_rand);
		break;
	    }
	    pSolarSysState->CurNode = 0;
	    break;
	case GENERATE_PLANETS:
	{
	    COUNT	angle;

	    GenerateRandomIP (GENERATE_PLANETS);
	    pSolarSysState->PlanetDesc[0].data_index = CRACKED_WORLD;
	    pSolarSysState->PlanetDesc[0].radius = EARTH_RADIUS * 80L / 100;
	    if (pSolarSysState->PlanetDesc[0].NumPlanets > 2)
		pSolarSysState->PlanetDesc[0].NumPlanets = 2;
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
		if ((CurStarDescPtr->Index == MYCON_DEFINED
			|| CurStarDescPtr->Index == SUN_DEVICE_DEFINED)
			&& ActivateStarShip (MYCON_SHIP, SPHERE_TRACKING))
		{
		    if (CurStarDescPtr->Index == MYCON_DEFINED
			    || !GET_GAME_STATE (SUN_DEVICE_UNGUARDED))
		    {
			NotifyOthers (MYCON_SHIP, (BYTE)~0);
			PutGroupInfo (0L, (BYTE)~0);
			ReinitQueue (&GLOBAL (npc_built_ship_q));

			if (CurStarDescPtr->Index == MYCON_DEFINED
				|| !GET_GAME_STATE (MYCON_FELL_FOR_AMBUSH))
			    CloneShipFragment (MYCON_SHIP,
				    &GLOBAL (npc_built_ship_q), (BYTE)~0);
			else
			{
			    COUNT	i;

			    for (i = 0; i < 5; ++i)
				CloneShipFragment (MYCON_SHIP,
					&GLOBAL (npc_built_ship_q), 0);
			}

			pSolarSysState->MenuState.Initialized += 2;
			GLOBAL (CurrentActivity) |= START_INTERPLANETARY;
			if (CurStarDescPtr->Index == MYCON_DEFINED)
			{
			    SET_GAME_STATE (GLOBAL_FLAGS_AND_DATA, 1 << 7);
			}
			else
			{
			    SET_GAME_STATE (GLOBAL_FLAGS_AND_DATA, 1 << 6);
			}
			InitCommunication (MYCON_CONVERSATION);
			pSolarSysState->MenuState.Initialized -= 2;

			if (GLOBAL (CurrentActivity) & (CHECK_ABORT | CHECK_LOAD))
			    break;

			{
			    BOOLEAN	MyconSurvivors;

			    MyconSurvivors = GetHeadLink (
				    &GLOBAL (npc_built_ship_q)
				    ) != 0;

			    GLOBAL (CurrentActivity) &= ~START_INTERPLANETARY;
			    ReinitQueue (&GLOBAL (npc_built_ship_q));
			    GetGroupInfo (0L, 0);

			    if (MyconSurvivors)
				break;

			    SET_GAME_STATE (SUN_DEVICE_UNGUARDED, 1);
			    SetSemaphore (&GraphicsSem);
			    RepairSISBorder ();
			    ClearSemaphore (&GraphicsSem);
			}
		    }
		}

		switch (CurStarDescPtr->Index)
		{
		    case SUN_DEVICE_DEFINED:
			if (!GET_GAME_STATE (SUN_DEVICE))
			{
			    pSolarSysState->SysInfo.PlanetInfo.LanderFont =
				    CaptureFont (
					    LoadGraphic (LANDER_FONT)
					    );
			    pSolarSysState->PlanetSideFrame[1] =
				    CaptureDrawable (
					    LoadGraphic (SUN_DEVICE_MASK_PMAP_ANIM)
					    );
			    pSolarSysState->SysInfo.PlanetInfo.DiscoveryString =
				    CaptureStringTable (
					    LoadStringTable (SUN_DEVICE_STRTAB)
					    );
			}
			break;
		    case EGG_CASE0_DEFINED:
		    case EGG_CASE1_DEFINED:
		    case EGG_CASE2_DEFINED:
			if (GET_GAME_STATE (KNOW_ABOUT_SHATTERED) == 0)
			{
			    SET_GAME_STATE (KNOW_ABOUT_SHATTERED, 1);
			}
			if (!(pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask[ENERGY_SCAN]
				& (1L << 0)))
			{
			    pSolarSysState->SysInfo.PlanetInfo.LanderFont =
				    CaptureFont (
					    LoadGraphic (LANDER_FONT)
					    );
			    pSolarSysState->PlanetSideFrame[1] =
				    CaptureDrawable (
					    LoadGraphic (EGG_CASE_MASK_PMAP_ANIM)
					    );
			    pSolarSysState->SysInfo.PlanetInfo.DiscoveryString =
				    CaptureStringTable (
					    LoadStringTable (EGG_CASE_STRTAB)
					    );
			}
			break;
		}
	    }
	default:
	    GenerateRandomIP (control);
	    break;
    }
}

