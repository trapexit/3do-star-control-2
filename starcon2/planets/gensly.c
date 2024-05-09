#include "starcon.h"

PROC(
void GenerateSlylandro, (control),
    ARG_END	(BYTE	control)
)
{
    switch (control)
    {
	case GENERATE_PLANETS:
	    GenerateRandomIP (GENERATE_PLANETS);
	    pSolarSysState->PlanetDesc[3].data_index = RED_GAS_GIANT;
	    pSolarSysState->PlanetDesc[3].NumPlanets = 1;
	    break;
	case GENERATE_ORBITAL:
	    if (pSolarSysState->pOrbitalDesc == &pSolarSysState->PlanetDesc[3])
	    {
		pSolarSysState->MenuState.Initialized += 2;
		InitCommunication (SLYLANDRO_HOME_CONVERSATION);
		pSolarSysState->MenuState.Initialized -= 2;
		break;
	    }
	default:
	    GenerateRandomIP (control);
	    break;
    }
}

