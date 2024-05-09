#include "starcon.h"
#include "scan.h"

PROC_GLOBAL(
void rotate_planet_task, (),
    ARG_VOID
);

PROC(
void DrawScannedObjects, (Reversed),
    ARG_END	(BOOLEAN	Reversed)
)
{
    HELEMENT	hElement, hNextElement;

    for (hElement = Reversed ? GetTailElement () : GetHeadElement ();
	    hElement; hElement = hNextElement)
    {
	ELEMENTPTR	ElementPtr;

	LockElement (hElement, &ElementPtr);
	hNextElement = Reversed ?
		GetPredElement (ElementPtr) :
		GetSuccElement (ElementPtr);

	if (ElementPtr->state_flags & APPEARING)
	{
	    STAMP	s;

	    s.origin = ElementPtr->current.location;
	    s.frame = ElementPtr->next.image.frame;
	    DrawStamp (&s);
	}

	UnlockElement (hElement);
    }
}

PROC(
void LoadPlanet, (IsDefined),
    ARG_END	(BOOLEAN	IsDefined)
)
{
    STAMP	s;

    SetSemaphore (&GraphicsSem);

    BatchGraphics ();
    ScreenOrigin (Screen, 0, 0);
    if (!(LastActivity & CHECK_LOAD))
	DrawStarBackGround (TRUE);

    if (pSolarSysState->MenuState.flash_task == 0)
    {
	PPLANET_DESC	pPlanetDesc;
	PROC_GLOBAL(
	void GeneratePlanetSide, (),
	    ARG_VOID
	);

	StopMusic ();

	TaskContext = CaptureContext (CreateContext ());

	pPlanetDesc = pSolarSysState->pOrbitalDesc;

	if (pPlanetDesc->data_index & PLANET_SHIELDED)
	    ; /* pSolarSysState->PlanetSideFrame[2] = CaptureDrawable (
		    LoadGraphic (PLANET_SHIELDED_MASK_PMAP_ANIM)
		    ); */
//	else if (pSolarSysState->SysInfo.PlanetInfo.AtmoDensity != GAS_GIANT_ATMOSPHERE)
//	    LoadLanderData ();

	{
	    PROC_GLOBAL(
	    void GeneratePlanetMask, (pPlanetDesc),
		ARG_END	(PPLANET_DESC	pPlanetDesc)
	    );

	    GeneratePlanetMask (pPlanetDesc);
	}

	if (pPlanetDesc->pPrevDesc != &pSolarSysState->SunDesc[0])
	    pPlanetDesc = pPlanetDesc->pPrevDesc;

	GeneratePlanetSide ();
    }

    if (LastActivity & CHECK_LOAD)
    {
	if (LOBYTE (LastActivity) == 0)
	{
	    ClearSemaphore (&GraphicsSem);
	    DrawSISFrame ();
	    SetSemaphore (&GraphicsSem);
	}
	else
	{
	    ClearSISRect (DRAW_SIS_DISPLAY);
	    RepairSISBorder ();
	}
	DrawSISMessage (NULL_PTR);
	DrawSISTitle ((PBYTE)GLOBAL_SIS (PlanetName));

	DrawStarBackGround (TRUE);
    }

    SetContext (SpaceContext);
    SetContextDrawState (DEST_PIXMAP | DRAW_REPLACE);

    BatchGraphics ();
    DrawPlanet (SIS_SCREEN_WIDTH - MAP_WIDTH, SIS_SCREEN_HEIGHT - MAP_HEIGHT, 0, 0);
    UnbatchGraphics ();

    if (s.frame = pSolarSysState->PlanetSideFrame[2])
    {
	s.origin.x = SIS_SCREEN_WIDTH >> 1;
	s.origin.y = ((116 - SIS_ORG_Y) >> 1) + 2;
	DrawStamp (&s);
    }

    if (!(LastActivity & CHECK_LOAD))
    {
	RECT	r;

	r.corner.x = SIS_ORG_X;
	r.corner.y = SIS_ORG_Y;
	r.extent.width = SIS_SCREEN_WIDTH;
	r.extent.height = SIS_SCREEN_HEIGHT;
	ScreenTransition (3, &r);
	UnbatchGraphics ();
	LoadIntoExtraScreen (&r);
    }
    
    ClearSemaphore (&GraphicsSem);

    if (!PLRPlaying ((MUSIC_REF)~0))
    {
	extern MUSIC_REF	LanderMusic;
	    
	PlayMusic (LanderMusic, TRUE, 1);
	if (pSolarSysState->MenuState.flash_task == 0)
	{
	    pSolarSysState->MenuState.flash_task =
		    AddTask (rotate_planet_task, 4096);

	    while (pSolarSysState->MenuState.Initialized == 2)
		TaskSwitch ();
	}
    }

    if (LastActivity & CHECK_LOAD)
    {
	RECT	r;

	r.corner.x = SIS_ORG_X;
	r.corner.y = SIS_ORG_Y;
	r.extent.width = SIS_SCREEN_WIDTH;
	r.extent.height = SIS_SCREEN_HEIGHT;
	SetSemaphore (&GraphicsSem);    
	ScreenTransition (3, &r);
	UnbatchGraphics ();
	LoadIntoExtraScreen (&r);
	ClearSemaphore (&GraphicsSem);
    }
}

PROC(
void FreePlanet, (),
    ARG_VOID
)
{
    COUNT	i;

    SetSemaphore (&GraphicsSem);

    if (pSolarSysState->MenuState.flash_task)
    {
	DeleteTask (pSolarSysState->MenuState.flash_task);
	pSolarSysState->MenuState.flash_task = 0;
    }

    StopMusic ();

    for (i = 0; i < sizeof (pSolarSysState->PlanetSideFrame)
	    / sizeof (pSolarSysState->PlanetSideFrame[0]); ++i)
    {
	DestroyDrawable (ReleaseDrawable (pSolarSysState->PlanetSideFrame[i]));
	pSolarSysState->PlanetSideFrame[i] = 0;
    }

//    FreeLanderData ();

    UnlockResourceData (pSolarSysState->hTopoData);
    FreeResourceData (pSolarSysState->hTopoData);
    pSolarSysState->hTopoData = 0;

    DestroyContext (ReleaseContext (TaskContext));
    TaskContext = 0;

    DestroyStringTable (ReleaseStringTable (
	    pSolarSysState->SysInfo.PlanetInfo.DiscoveryString
	    ));
    pSolarSysState->SysInfo.PlanetInfo.DiscoveryString = 0;
    DestroyFont (ReleaseFont (
	    pSolarSysState->SysInfo.PlanetInfo.LanderFont
	    ));
    pSolarSysState->SysInfo.PlanetInfo.LanderFont = 0;

    ClearSemaphore (&GraphicsSem);
}

