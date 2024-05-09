#include "starcon.h"

PROC(
void RenderTopography, (Reconstruct),
    ARG_END	(BOOLEAN	Reconstruct)
)
{
    CONTEXT	OldContext;
	    
    OldContext = SetContext (SpaceContext);
    BatchGraphics ();
    DrawPlanet (SIS_SCREEN_WIDTH - MAP_WIDTH, SIS_SCREEN_HEIGHT - MAP_HEIGHT, 0);
    UnbatchGraphics ();
    SetContext (OldContext);
}

PROC(
void GeneratePlanetMask, (pPlanetDesc),
    ARG_END	(PPLANET_DESC	pPlanetDesc)
)
{
#define WRAP_EXTRA	20
    COUNT		facing;
    int			StarOffset, PlanetOffset, MoonOffset;
    COLORMAP		CMap;
    PLANDATAPTR		PlanDataPtr;
    STRING		XlatRef;
    LPBYTE		lpTopoData;
    extern FRAME	LanderFrame[];

    PlanDataPtr = &PlanData[
	    pPlanetDesc->data_index & ~PLANET_SHIELDED
	    ];
    if (pSolarSysState->hTopoData == 0)
    {
	pSolarSysState->hTopoData = AllocResourceData (
		((MAP_WIDTH + WRAP_EXTRA + 3) & ~3) * MAP_HEIGHT * 2, 0
		);
	LockResourceData (pSolarSysState->hTopoData, &lpTopoData);
    }

    CMap = CaptureColorMap (
	    LoadColorMap (PlanDataPtr->CMapInstance)
	    );
    XlatRef = CaptureStringTable (
	    LoadStringTable (PlanDataPtr->XlatTabInstance)
	    );
    if (pSolarSysState->SysInfo.PlanetInfo.SurfaceTemperature > HOT_THRESHOLD)
    {
	CMap = SetAbsColorMapIndex (CMap, 2);
	XlatRef = SetAbsStringTableIndex (XlatRef, 2);
    }
    else if (pSolarSysState->SysInfo.PlanetInfo.SurfaceTemperature > COLD_THRESHOLD)
    {
	CMap = SetAbsColorMapIndex (CMap, 1);
	XlatRef = SetAbsStringTableIndex (XlatRef, 1);
    }

    StarOffset = CurStarDescPtr - star_array;
    if (pSolarSysState->pOrbitalDesc->pPrevDesc == pSolarSysState->SunDesc)
    {
	PlanetOffset = pSolarSysState->pOrbitalDesc - pSolarSysState->PlanetDesc;
	MoonOffset = 0;
	facing = NORMALIZE_FACING (ANGLE_TO_FACING (
		ARCTAN (pPlanetDesc->location.x, pPlanetDesc->location.y)
		));

    }
    else
    {
	PlanetOffset = pSolarSysState->pOrbitalDesc->pPrevDesc
		- pSolarSysState->PlanetDesc;
	MoonOffset = pSolarSysState->pOrbitalDesc - pSolarSysState->MoonDesc + 1;
	facing = NORMALIZE_FACING (ANGLE_TO_FACING (ARCTAN (
		pSolarSysState->pOrbitalDesc->pPrevDesc->location.x,
		pSolarSysState->pOrbitalDesc->pPrevDesc->location.y
		)));
    }

    StarOffset = BuildPlanet (StarOffset, PlanetOffset, MoonOffset,
	    GetStringAddress (XlatRef),
	    GetColorMapAddress (CMap),
	    lpTopoData, pPlanetDesc->data_index & PLANET_SHIELDED,
	    SetAbsFrameIndex (LanderFrame[7], facing),
	    (CurStarDescPtr->Index == SOL_DEFINED
		    && PlanetOffset == 2 /* EARTH */ && MoonOffset == 0
		    ? SetAbsFrameIndex (LanderFrame[7],
		    	ANGLE_TO_FACING (FULL_CIRCLE) + 3)
		    : 0),
	    LastActivity & CHECK_LOAD);
#ifndef DEBUG
if (StarOffset == 0)
{
    printf ("unable to build planet\n");
}
#endif /* DEBUG */

    DestroyStringTable (ReleaseStringTable (XlatRef));
    DestroyColorMap (ReleaseColorMap (CMap));

    SetPlanetMusic (pPlanetDesc->data_index & ~PLANET_SHIELDED);
}

PROC(
void rotate_planet_task, (),
    ARG_VOID
)
{
    SIZE		i;
    DWORD		TimeIn;
    PSOLARSYS_STATE	pSS;
    BOOLEAN		repair, zooming;

    repair = FALSE;
    zooming = TRUE;

    pSS = pSolarSysState;
    while (((PSOLARSYS_STATE VOLATILE)pSS)->MenuState.Initialized < 2)
	TaskSwitch ();

    SetPlanetTilt ((pSS->SysInfo.PlanetInfo.AxialTilt << 8) / 360);
	    
    i = 1 - ((pSS->SysInfo.PlanetInfo.AxialTilt & 1) << 1);
    TimeIn = GetTimeCounter ();
    for (;;)
    {
	BYTE	view_index;
	COORD	x;

	if (i > 0)
	    x = 0;
	else
	    x = MAP_WIDTH - 1;
	view_index = MAP_WIDTH;
	do
	{
	    CONTEXT	OldContext;
	    
	    SetSemaphore (&GraphicsSem);
	    if (((PSOLARSYS_STATE VOLATILE)pSS)->MenuState.Initialized <= 3
		    && !(GLOBAL (CurrentActivity) & CHECK_ABORT))
	    {
		OldContext = SetContext (SpaceContext);
		BatchGraphics ();
		ScreenOrigin (Screen, 0, 0);
		if (repair)
		{
		    RECT	r;
    
		    r.corner.x = 0;
		    r.corner.y = 0;
		    r.extent.width = SIS_SCREEN_WIDTH;
		    r.extent.height = SIS_SCREEN_HEIGHT - MAP_HEIGHT;
		    RepairBackRect (&r);
		}
		repair = RotatePlanet (x, SIS_SCREEN_WIDTH >> 1, (148 - SIS_ORG_Y) >> 1, 0);

		UnbatchGraphics ();
		SetContext (OldContext);
		if (!repair && zooming)
		{
		    zooming = FALSE;
		    ++pSS->MenuState.Initialized;
	        }
		x += i;
	    }
	    ClearSemaphore (&GraphicsSem);

	    TimeIn = SleepTask (TimeIn + (ONE_SECOND * 5 / MAP_WIDTH));
	} while (--view_index);
    }
}
