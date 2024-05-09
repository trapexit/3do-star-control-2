#include "starcon.h"

PROC(
void InitPlanetInfo, (),
    ARG_VOID
)
{
    LPVOID	fp;

    if (fp = OpenRamFile (STARINFO_FILE, STAR_BUFSIZE))
    {
	DWORD		offset;
	STAR_DESCPTR	pSD;

	offset = 0;
	pSD = &star_array[0];
	do
	{
	    WriteRamFile (&offset, sizeof (offset), 1, fp);
	    ++pSD;
	} while (pSD->star_pt.x <= MAX_X_UNIVERSE
		&& pSD->star_pt.y <= MAX_Y_UNIVERSE);

	CloseRamFile (fp);
    }
}

PROC(
void UninitPlanetInfo, (),
    ARG_VOID
)
{
    DeleteRamFile (STARINFO_FILE);
}

PROC(
void GetPlanetInfo, (),
    ARG_VOID
)
{
    LPVOID	fp;

    pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask[BIOLOGICAL_SCAN] =
	    pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask[MINERAL_SCAN] =
	    pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask[ENERGY_SCAN] = 0;
    if (fp = OpenRamFile (STARINFO_FILE, 0))
    {
	COUNT	star_index, planet_index, moon_index;
	DWORD	offset;

	star_index = (COUNT)(CurStarDescPtr - star_array);
	planet_index = (COUNT)(pSolarSysState->pBaseDesc->pPrevDesc
		- pSolarSysState->PlanetDesc);
	if (pSolarSysState->pOrbitalDesc->pPrevDesc == pSolarSysState->SunDesc)
	    moon_index = 0;
	else
	    moon_index = (COUNT)(pSolarSysState->pOrbitalDesc
		    - pSolarSysState->MoonDesc + 1);

	SeekRamFile (fp, star_index * sizeof (offset), SEEK_SET);
	ReadRamFile (&offset, sizeof (offset), 1, fp);

	if (offset)
	{
	     COUNT	i;

	     for (i = 0; i < planet_index; ++i)
		offset += sizeof (
			pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask
			) * (pSolarSysState->PlanetDesc[i].NumPlanets + 1);
		
	     offset += sizeof (
		    pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask
		    ) * moon_index;

	     SeekRamFile (fp, offset, SEEK_SET);
	     ReadRamFile (pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask,
			sizeof (
			pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask
			), 1, fp);
	}

	CloseRamFile (fp);
    }
}

PROC(
void PutPlanetInfo, (),
    ARG_VOID
)
{
    LPVOID	fp;

    if (fp = OpenRamFile (STARINFO_FILE, 0))
    {
	COUNT	i;
	COUNT	star_index, planet_index, moon_index;
	DWORD	offset;

	star_index = (COUNT)(CurStarDescPtr - star_array);
	planet_index = (COUNT)(pSolarSysState->pBaseDesc->pPrevDesc
		- pSolarSysState->PlanetDesc);
	if (pSolarSysState->pOrbitalDesc->pPrevDesc == pSolarSysState->SunDesc)
	    moon_index = 0;
	else
	    moon_index = (COUNT)(pSolarSysState->pOrbitalDesc
		    - pSolarSysState->MoonDesc + 1);

	SeekRamFile (fp, star_index * sizeof (offset), SEEK_SET);
	ReadRamFile (&offset, sizeof (offset), 1, fp);

	if (offset == 0)
	{
	    DWORD	ScanRetrieveMask[NUM_SCAN_TYPES] =
	    {
		0, 0, 0,
	    };

	    offset = LengthRamFile (fp);

	    SeekRamFile (fp, star_index * sizeof (offset), SEEK_SET);
	    WriteRamFile (&offset, sizeof (offset), 1, fp);

	    SeekRamFile (fp, offset, SEEK_SET);
	    for (i = 0; i < pSolarSysState->SunDesc[0].NumPlanets; ++i)
	    {
		COUNT	j;

		WriteRamFile (ScanRetrieveMask, sizeof (ScanRetrieveMask), 1, fp);
		for (j = 0; j < pSolarSysState->PlanetDesc[i].NumPlanets; ++j)
		    WriteRamFile (ScanRetrieveMask, sizeof (ScanRetrieveMask), 1, fp);
	    }
	}

	for (i = 0; i < planet_index; ++i)
	    offset += sizeof (
		    pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask
		    ) * (pSolarSysState->PlanetDesc[i].NumPlanets + 1);
		
	offset += sizeof (
		pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask
		) * moon_index;

	SeekRamFile (fp, offset, SEEK_SET);
	WriteRamFile (pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask,
		sizeof (
		pSolarSysState->SysInfo.PlanetInfo.ScanRetrieveMask
		), 1, fp);

	CloseRamFile (fp);
    }
}

