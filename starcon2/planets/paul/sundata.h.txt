#ifndef	_SUNDATA_H
#define	_SUNDATA_H

/*------------------------------ Global Data ------------------------------ */

#define NUMBER_OF_SUN_SIZES	(SUPER_GIANT_STAR - DWARF_STAR + 1)

#define	DWARF_ENERGY		1
#define	GIANT_ENERGY		5
#define	SUPERGIANT_ENERGY	20

typedef struct
{
    BYTE	StarSize;
    BYTE	StarIntensity;
    UWORD	StarEnergy;

    PLANET_INFO	PlanetInfo;
} SYSTEM_INFO;
typedef SYSTEM_INFO	*PSYSTEM_INFO;
typedef SYSTEM_INFO	near *NPSYSTEM_INFO;
typedef SYSTEM_INFO	far *LPSYSTEM_INFO;

#define SYSTEM_INFOPTR	PSYSTEM_INFO
	
PROC_GLOBAL(
DWORD GenerateMineralDeposits, (SysInfoPtr, pwhich_deposit),
    ARG		(SYSTEM_INFOPTR	SysInfoPtr)
    ARG_END	(PCOUNT		pwhich_deposit)
);
PROC_GLOBAL(
DWORD GenerateLifeForms, (SysInfoPtr, pwhich_life),
    ARG		(SYSTEM_INFOPTR	SysInfoPtr)
    ARG_END	(PCOUNT		pwhich_life)
);

#define DWARF_ELEMENT_DENSITY 		1
#define GIANT_ELEMENT_DENSITY		3
#define SUPERGIANT_ELEMENT_DENSITY	8

#define MAX_ELEMENT_DENSITY		((MAX_ELEMENT_UNITS * SUPERGIANT_ELEMENT_DENSITY) << 1)

PROC_GLOBAL(
DWORD DoPlanetaryAnalysis, (SysInfoPtr, pPlanetDesc),
    ARG		(SYSTEM_INFOPTR	SysInfoPtr)
    ARG_END	(PPLANET_DESC	pPlanetDesc)
);

extern SYSTEM_INFO	CurSysInfo;

#endif /* _SUNDATA_H */

