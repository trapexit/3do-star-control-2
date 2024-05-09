#include "starcon.h"

FRAME		stars_in_space,
		asteroid[NUM_VIEWS],
		blast[NUM_VIEWS],
		explosion[NUM_VIEWS];

PROC(
BOOLEAN load_animation, (pixarray, big_res, med_res, sml_res),
    ARG		(LPFRAME	pixarray)
    ARG		(DWORD		big_res)
    ARG		(DWORD		med_res)
    ARG_END	(DWORD		sml_res)
)
{
    DRAWABLE	d;

    if ((d = LoadGraphic (big_res)) == (DRAWABLE)NULL_PTR)
	return (FALSE);
    pixarray[0] = CaptureDrawable (d);

    if (med_res != 0L && (d = LoadGraphic (med_res)) == (DRAWABLE)NULL_PTR)
	return (FALSE);
    pixarray[1] = CaptureDrawable (d);

    if (sml_res != 0L && (d = LoadGraphic (sml_res)) == (DRAWABLE)NULL_PTR)
	return (FALSE);
    pixarray[2] = CaptureDrawable (d);

    return (TRUE);
}

PROC(
BOOLEAN free_image, (pixarray),
    ARG_END	(LPFRAME	pixarray)
)
{
    BOOLEAN	retval;
    COUNT	i;

    retval = TRUE;
    for (i = 0; i < NUM_VIEWS; ++i)
    {
	if (pixarray[i] != (FRAME)NULL_PTR)
	{
	    if (!DestroyDrawable (ReleaseDrawable (pixarray[i])))
		retval = FALSE;
	    pixarray[i] = (FRAME)NULL_PTR;
	}
    }

    return (retval);
}

static BYTE	space_ini_cnt;

PROC(
BOOLEAN InitSpace, (),
    ARG_VOID
)
{
    if (space_ini_cnt++ == 0
	    && LOBYTE (GLOBAL (CurrentActivity)) <= IN_ENCOUNTER)
    {
	if ((stars_in_space = CaptureDrawable (
		LoadGraphic (STAR_MASK_PMAP_ANIM)
		)) == 0)
	    return (FALSE);

	if (!load_animation (explosion,
		BOOM_BIG_MASK_PMAP_ANIM,
		BOOM_MED_MASK_PMAP_ANIM,
		BOOM_SML_MASK_PMAP_ANIM))
	    return (FALSE);

	if (!load_animation (blast,
		BLAST_BIG_MASK_PMAP_ANIM,
		BLAST_MED_MASK_PMAP_ANIM,
		BLAST_SML_MASK_PMAP_ANIM))
	    return (FALSE);

	if (!load_animation (asteroid,
		ASTEROID_BIG_MASK_PMAP_ANIM,
		ASTEROID_MED_MASK_PMAP_ANIM,
		ASTEROID_SML_MASK_PMAP_ANIM))
	    return (FALSE);
    }

    return (TRUE);
}

PROC(
void UninitSpace, (),
    ARG_VOID
)
{
    if (space_ini_cnt && --space_ini_cnt == 0)
    {
	free_image (blast);
	free_image (explosion);
	free_image (asteroid);

	DestroyDrawable (ReleaseDrawable (stars_in_space));
	stars_in_space = 0;
    }
}

PROC(
SIZE InitShips, (),
    ARG_VOID
)
{
    SIZE	num_ships;

    InitSpace ();

    SetContext (StatusContext);
    SetContextDrawState (DEST_PIXMAP | DRAW_REPLACE);
    SetContext (SpaceContext);

    InitDisplayList ();
    InitGalaxy ();

    num_ships = NUM_SIDES;
    if (LOBYTE (GLOBAL (CurrentActivity)) == IN_HYPERSPACE)
    {
	ReinitQueue (&race_q[0]);
	ReinitQueue (&race_q[1]);

	Build (&race_q[0], SIS_RES_INDEX, GOOD_GUY, 0);

	LoadHyperspace ();

	--num_ships;
    }
    else
    {
	COUNT	i;
	RECT	r;

	SetContextFGFrame (Screen);
	r.corner.x = SAFE_X;
	r.corner.y = SAFE_Y;
	r.extent.width = SPACE_WIDTH;
	r.extent.height = SPACE_HEIGHT;
	SetContextClipRect (&r);

	SetContextBackGroundColor (BLACK_COLOR);
	{
	    CONTEXT	OldContext;

	    OldContext = SetContext (ScreenContext);

	    SetContextDrawState (DEST_PIXMAP | DRAW_REPLACE);
	    SetContextBackGroundColor (BLACK_COLOR);
	    ClearDrawable ();

	    SetContext (OldContext);
	}

	if (LOBYTE (GLOBAL (CurrentActivity)) == IN_LAST_BATTLE)
	    free_gravity_well ();
	else
	{
#define NUM_ASTEROIDS	5
	    for (i = 0; i < NUM_ASTEROIDS; ++i)
		spawn_asteroid (NULL_PTR);
#define NUM_PLANETS	1
	    for (i = 0; i < NUM_PLANETS; ++i)
		spawn_planet ();
	}
    }

    FlushInput ();

    return (num_ships);
}

PROC(
void UninitShips, (),
    ARG_VOID
)
{
    BYTE		crew_retrieved;
    SIZE		i;
    HELEMENT		hElement, hNextElement;
    STARSHIPPTR		SPtr[NUM_PLAYERS];

    StopSound ();

    UninitSpace ();

    for (i = 0; i < NUM_PLAYERS; ++i)
	SPtr[i] = 0;

    crew_retrieved = 0;
    for (hElement = GetHeadElement ();
	    hElement != 0; hElement = hNextElement)
    {
	ELEMENTPTR	ElementPtr;
	PROC_GLOBAL(
	void new_ship, (ElementPtr),
	    ARG_END	(LPELEMENT	ElementPtr)
	);

	LockElement (hElement, &ElementPtr);
	hNextElement = GetSuccElement (ElementPtr);
	if (ElementPtr->state_flags & CREW_OBJECT)
	    ++crew_retrieved;
	else if ((ElementPtr->state_flags & PLAYER_SHIP)
		|| ElementPtr->death_func == new_ship)
	{
	    STARSHIPPTR		StarShipPtr;

	    GetElementStarShip (ElementPtr, &StarShipPtr);
	    StarShipPtr->ShipFacing =
		    StarShipPtr->RaceDescPtr->ship_info.var2;
	    StarShipPtr->special_counter =
		    StarShipPtr->RaceDescPtr->ship_info.crew_level;
	    SPtr[WHICH_SIDE (ElementPtr->state_flags)] = StarShipPtr;
	    free_ship (StarShipPtr, TRUE);
	}
	UnlockElement (hElement);
    }

    GLOBAL (CurrentActivity) &= ~IN_BATTLE;

    if (LOBYTE (GLOBAL (CurrentActivity)) == IN_LAST_BATTLE)
    {
    }
    else if (LOBYTE (GLOBAL (CurrentActivity)) <= IN_ENCOUNTER
	    && !(GLOBAL (CurrentActivity) & CHECK_ABORT))
    {
	for (i = NUM_PLAYERS - 1; i >= 0; --i)
	{
	    if (SPtr[i])
	    {
		if (SPtr[i]->special_counter)
		    SPtr[i]->special_counter += crew_retrieved;

		GetEncounterStarShip (SPtr[i], i);
	    }
	}
    }

    if (LOBYTE (GLOBAL (CurrentActivity)) != IN_ENCOUNTER)
    {
	ReinitQueue (&race_q[0]);
	ReinitQueue (&race_q[1]);

	if (LOBYTE (GLOBAL (CurrentActivity)) == IN_HYPERSPACE)
	    FreeHyperspace ();
    }
}

PROC(
void InitGlobData, (),
    ARG_VOID
)
{
    COUNT	i;

    i = GLOBAL (glob_flags);
    MEMSET ((PBYTE)&GlobData, 0, sizeof (GlobData));
    GLOBAL (glob_flags) = (BYTE)i;

    GLOBAL (DisplayArray) = DisplayArray;
}
