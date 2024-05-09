#include "starcon.h"

PROC(STATIC
void near GenerateTrap, (control),
    ARG_END	(BYTE	control)
)
{
    switch (control)
    {
	case GENERATE_PLANETS:
	{
	    COUNT	angle;

	    GenerateRandomIP (GENERATE_PLANETS);
	    pSolarSysState->PlanetDesc[0].data_index = TELLURIC_WORLD;
	    pSolarSysState->PlanetDesc[0].NumPlanets = 1;
	    pSolarSysState->PlanetDesc[0].radius = EARTH_RADIUS * 203L / 100;
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
	    GenerateRandomIP (GENERATE_ORBITAL);
	    if (pSolarSysState->pOrbitalDesc == &pSolarSysState->PlanetDesc[0])
	    {
		pSolarSysState->SysInfo.PlanetInfo.AtmoDensity = EARTH_ATMOSPHERE * 2;
		pSolarSysState->SysInfo.PlanetInfo.SurfaceTemperature = 35;
		pSolarSysState->SysInfo.PlanetInfo.Weather = 3;
		pSolarSysState->SysInfo.PlanetInfo.Tectonics = 1;
	    }
	    break;
	default:
	    GenerateRandomIP (control);
	    break;
    }
}

PROC(
void GenerateSyreen, (control),
    ARG_END	(BYTE	control)
)
{
    if (CurStarDescPtr->Index == MYCON_TRAP_DEFINED)
    {
	GenerateTrap (control);
	return;
    }

    switch (control)
    {
	case GENERATE_MOONS:
	    GenerateRandomIP (GENERATE_MOONS);
	    if (pSolarSysState->pBaseDesc == &pSolarSysState->PlanetDesc[0])
	    {
		pSolarSysState->MoonDesc[0].data_index = (BYTE)~0;
		pSolarSysState->MoonDesc[0].radius = MIN_MOON_RADIUS;
		pSolarSysState->MoonDesc[0].location.x =
			COSINE (QUADRANT, pSolarSysState->MoonDesc[0].radius);
		pSolarSysState->MoonDesc[0].location.y =
			SINE (QUADRANT, pSolarSysState->MoonDesc[0].radius);
	    }
	    break;
	case GENERATE_PLANETS:
	{
	    GenerateRandomIP (GENERATE_PLANETS);
	    pSolarSysState->PlanetDesc[0].data_index =
		    WATER_WORLD | PLANET_SHIELDED;
	    pSolarSysState->PlanetDesc[0].NumPlanets = 1;
	    break;
	}
	case GENERATE_ORBITAL:
	    if (pSolarSysState->pOrbitalDesc == &pSolarSysState->PlanetDesc[0])
	    {
		GenerateRandomIP (GENERATE_ORBITAL);
		pSolarSysState->SysInfo.PlanetInfo.SurfaceTemperature = 19;
		pSolarSysState->SysInfo.PlanetInfo.Tectonics = 0;
		pSolarSysState->SysInfo.PlanetInfo.Weather = 0;
		pSolarSysState->SysInfo.PlanetInfo.AtmoDensity =
			EARTH_ATMOSPHERE * 9 / 10;
		break;
	    }
		/* Starbase */
	    else if (pSolarSysState->pOrbitalDesc->pPrevDesc == &pSolarSysState->PlanetDesc[0]
		    && pSolarSysState->pOrbitalDesc == &pSolarSysState->MoonDesc[0])
	    {
		pSolarSysState->MenuState.Initialized += 2;
		InitCommunication (SYREEN_CONVERSATION);
		pSolarSysState->MenuState.Initialized -= 2;
		break;
	    }
	default:
	    GenerateRandomIP (control);
	    break;
    }
}

