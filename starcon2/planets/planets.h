#ifndef _PLANETS_H
#define _PLANETS_H

#define END_INTERPLANETARY	START_INTERPLANETARY

enum
{
    SCAN = 0,
    STARMAP,
    EQUIP_DEVICE,
    CARGO,
    ROSTER
};

enum
{
    MINERAL_SCAN = 0,
    ENERGY_SCAN,
    BIOLOGICAL_SCAN,

    NUM_SCAN_TYPES,
    EXIT_SCAN = NUM_SCAN_TYPES,
    AUTO_SCAN,
    DISPATCH_SHUTTLE
};

#define MAP_WIDTH	210
#define MAP_HEIGHT	67

enum
{
    GENERATE_PLANETS = 0,
    GENERATE_MOONS,
    GENERATE_ORBITAL,

    INIT_NPCS,
    REINIT_NPCS,
    UNINIT_NPCS,

    GENERATE_MINERAL,
    GENERATE_ENERGY,
    GENERATE_LIFE,

    GENERATE_NAME
};

enum
{
    BIOLOGICAL_DISASTER = 0,
    EARTHQUAKE_DISASTER,
    LIGHTNING_DISASTER,
    LAVASPOT_DISASTER,

	/* additional lander sounds */
    LANDER_INJURED,
    LANDER_SHOOTS,
    LANDER_HITS,
    LIFEFORM_CANNED,
    LANDER_PICKUP,
    LANDER_FULL,
    LANDER_DEPARTS,
    LANDER_RETURNS,
    LANDER_DESTROYED
};
#define MAX_SCROUNGED		50	/* max lander can hold */

#define SCALE_RADIUS(r)		((r) << 6)
#define UNSCALE_RADIUS(r)	((r) >> 6)
#define MAX_ZOOM_RADIUS		SCALE_RADIUS(128)
#define MIN_ZOOM_RADIUS		(MAX_ZOOM_RADIUS>>3)
#define EARTH_RADIUS		SCALE_RADIUS(8)

#define MIN_PLANET_RADIUS	SCALE_RADIUS (4)
#define MAX_PLANET_RADIUS	SCALE_RADIUS (124)

#define DISPLAY_FACTOR		((SIS_SCREEN_WIDTH >> 1) - 8)

#define NUM_SCANDOT_TRANSITIONS	4

#define MIN_MOON_RADIUS	35
#define MOON_DELTA	20

#define PLANET_SHIELDED		(1 << 7)

typedef struct planet_desc
{
    DWORD		rand_seed;

    BYTE		data_index;
    BYTE		NumPlanets;
    SIZE		radius;
    POINT		location;

    COLOR		temp_color;
    COUNT		NextIndex;
    STAMP		image;

    struct planet_desc	*pPrevDesc;
} PLANET_DESC;
typedef PLANET_DESC	*PPLANET_DESC;
typedef PLANET_DESC	near *NPPLANET_DESC;
typedef PLANET_DESC	far *LPPLANET_DESC;

typedef struct
{
    POINT	star_pt;
    BYTE	Type, Index;
    BYTE	Prefix, Postfix;
} STAR_DESC;
typedef STAR_DESC		*PSTAR_DESC;
typedef STAR_DESC		near *NPSTAR_DESC;
typedef STAR_DESC		FARDATA *LPSTAR_DESC;

#define STAR_DESCPTR		LPSTAR_DESC

#define MAX_SUNS		1
#define MAX_PLANETS		16
#define MAX_MOONS		4

#include "paul/elemdata.h"
#include "paul/plandata.h"
#include "paul/sundata.h"

typedef PROC_PARAMETER(
	void (*PLAN_GEN_FUNC), (control),
	    ARG_END	(BYTE	control)
	);

typedef struct solarsys_state
{
    MENU_STATE		MenuState;

    COUNT		WaitIntersect;
    PLANET_DESC		SunDesc[MAX_SUNS],
			PlanetDesc[MAX_PLANETS],
			MoonDesc[MAX_MOONS];
    PPLANET_DESC	pBaseDesc, pOrbitalDesc;
    SIZE		FirstPlanetIndex, LastPlanetIndex;

    BYTE		turn_counter, turn_wait;
    BYTE		thrust_counter, max_ship_speed;

    MEM_HANDLE		hTopoData;

    SYSTEM_INFO		SysInfo;

    COUNT		CurNode;
    PLAN_GEN_FUNC	GenFunc;

    FRAME		PlanetSideFrame[6];
} SOLARSYS_STATE;
typedef SOLARSYS_STATE	*PSOLARSYS_STATE;

extern PSOLARSYS_STATE	pSolarSysState;

PROC_GLOBAL(
void  LoadPlanet, (IsDefined),
    ARG_END	(BOOLEAN	IsDefined)
);
PROC_GLOBAL(
void  FreePlanet, (),
    ARG_VOID
);

PROC_GLOBAL(
void ExploreSolarSys, (),
    ARG_VOID
);
PROC_GLOBAL(
void DrawStarBackGround, (ForPlanet),
    ARG_END	(BOOLEAN	ForPlanet)
);
PROC_GLOBAL(
void  XFormIPLoc, (pIn, pOut, ToDisplay),
    ARG		(PPOINT		pIn)
    ARG		(PPOINT		pOut)
    ARG_END	(BOOLEAN	ToDisplay)
);
PROC_GLOBAL(
void GenerateRandomIP, (control),
    ARG_END	(BYTE		control)
);
PROC_GLOBAL(
PLAN_GEN_FUNC GenerateIP, (Index),
    ARG_END	(BYTE		Index)
);
PROC_GLOBAL(
void DrawSystem, (radius, IsInnerSystem),
    ARG		(SIZE		radius)
    ARG_END	(BOOLEAN	IsInnerSystem)
);
PROC_GLOBAL(
void DrawOval, (pRect, num_off_pixels),
    ARG		(PRECT	pRect)
    ARG_END	(BYTE	num_off_pixels)
);
PROC_GLOBAL(
void DrawFilledOval, (pRect),
    ARG_END	(PRECT	pRect)
);
PROC_GLOBAL(
void DoMissions, (),
    ARG_VOID
);
PROC_GLOBAL(
void FillOrbits, (NumPlanets, pBaseDesc, TypesDefined),
    ARG		(BYTE		NumPlanets)
    ARG		(PPLANET_DESC	pBaseDesc)
    ARG_END	(BOOLEAN	TypesDefined)
);
PROC_GLOBAL(
void ScanSystem, (),
    ARG_VOID
);
PROC_GLOBAL(
void ChangeSolarSys, (),
    ARG_VOID
);
PROC_GLOBAL(
BOOLEAN DoFlagshipCommands, (InputState, pSS),
    ARG		(INPUT_STATE	InputState)
    ARG_END	(PMENU_STATE	pMS)
);
PROC_GLOBAL(
void ZoomSystem, (),
    ARG_VOID
);
PROC_GLOBAL(
void LoadSolarSys, (),
    ARG_VOID
);
PROC_GLOBAL(
void InitLander, (LanderFlags),
    ARG_END	(BYTE	LanderFlags)
);

#endif /* _PLANETS_H */

