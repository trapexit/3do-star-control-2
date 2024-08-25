#include "starcon.h"

PROC(STATIC
BOOLEAN	DoSelectAction, (InputState, pMS),
    ARG		(INPUT_STATE	InputState)
    ARG_END	(PMENU_STATE	pMS)
)
{
    if (GLOBAL (CurrentActivity) & CHECK_ABORT)
    {
	pMS->CurState = ATTACK + 1;
	return (FALSE);
    }

    if (!pMS->Initialized)
    {
	pMS->Initialized = TRUE;
	pMS->InputFunc = DoSelectAction;
    }
    else if (InputState & DEVICE_BUTTON1)
    {
	switch (pMS->CurState)
	{
	    case HAIL:
	    case ATTACK:
		if (LOBYTE (GLOBAL (CurrentActivity)) == IN_LAST_BATTLE)
		    pMS->CurState = HAIL;
		return (FALSE);
	    case ATTACK + 1:
		if (pMS->Initialized = GameOptions ())
		{
		    DrawMenuStateStrings (PM_CONVERSE, pMS->CurState);
		    SetSemaphore (&GraphicsSem);
		    SetFlashRect ((PRECT)~0L, (FRAME)0);
		    ClearSemaphore (&GraphicsSem);
		}
		return ((BOOLEAN)pMS->Initialized);
	}
    }
    else
    {
	BYTE	NewState;

	NewState = pMS->CurState;
	if (GetInputXComponent (InputState) < 0
		|| GetInputYComponent (InputState) < 0)
	{
	    if (NewState-- == HAIL)
		NewState = ATTACK + 1;
	}
	else if (GetInputXComponent (InputState) > 0
		|| GetInputYComponent (InputState) > 0)
	{
	    if (NewState++ == ATTACK + 1)
		NewState = HAIL;
	}

	if (NewState != pMS->CurState)
	{
	    DrawMenuStateStrings (PM_CONVERSE, NewState);
	    pMS->CurState = NewState;
	}
    }

    return (TRUE);
}

PROC(
void BuildBattle, (which_player),
    ARG_END	(COUNT	which_player)
)
{
    PQUEUE	pQueue;
    HSTARSHIP	hStarShip, hNextShip;
    HSTARSHIP	hBuiltShip;
    STARSHIPPTR	BuiltShipPtr;

    battle_counter = 0;
    if (GetHeadLink (&GLOBAL (npc_built_ship_q)) == 0)
    {
	SET_GAME_STATE (BATTLE_SEGUE, 0);
	return;
    }

    if (which_player == 0)
	pQueue = &GLOBAL (built_ship_q);
    else
    {
        switch (LOBYTE (GLOBAL (CurrentActivity)))
        {
	    case IN_LAST_BATTLE:
	        load_gravity_well (NUMBER_OF_PLANET_TYPES);
	        break;
	    case IN_HYPERSPACE:
	        load_gravity_well ((BYTE)((COUNT)random ()
		        % NUMBER_OF_PLANET_TYPES));
	        break;
	    default:
	        SET_GAME_STATE (ESCAPE_COUNTER, 110);
	        load_gravity_well (GET_GAME_STATE (BATTLE_PLANET));
	        break;
        }
	pQueue = &GLOBAL (npc_built_ship_q);
    }

    ReinitQueue (&race_q[which_player]);
    for (hStarShip = GetHeadLink (pQueue);
	    hStarShip != 0; hStarShip = hNextShip)
    {
	SHIP_FRAGMENTPTR	FragPtr;

	FragPtr = (SHIP_FRAGMENTPTR)LockStarShip (pQueue, hStarShip);
	hNextShip = _GetSuccLink (FragPtr);

	if (hBuiltShip = Build (&race_q[which_player],
		GET_RACE_ID (FragPtr) == SAMATRA_SHIP ?
		SAMATRA_RES_INDEX : FragPtr->RaceResIndex,
		1 << which_player,
		StarShipCaptain (FragPtr)))
	{
	    BuiltShipPtr = LockStarShip (
		    &race_q[which_player],
		    hBuiltShip
		    );
	    BuiltShipPtr->captains_name_index =
		    StarShipCaptain (BuiltShipPtr);
	    BuiltShipPtr->cur_status_flags =
		    StarShipPlayer (BuiltShipPtr);
	    if (FragPtr->ShipInfo.crew_level != (BYTE)~0)
		BuiltShipPtr->special_counter = FragPtr->ShipInfo.crew_level;
	    else	/* if infinite ships */
		BuiltShipPtr->special_counter = FragPtr->ShipInfo.max_crew;
	    BuiltShipPtr->RaceDescPtr =
		    (RACE_DESCPTR)&FragPtr->ShipInfo;

	    UnlockStarShip (
		    &race_q[which_player],
		    hBuiltShip
		    );
	}

	UnlockStarShip (pQueue, hStarShip);
    }

    if (which_player == 0
	    && (hBuiltShip = Build (&race_q[0],
	    SIS_RES_INDEX, GOOD_GUY, 0)))
    {
	BuiltShipPtr = LockStarShip (
		&race_q[0],
		hBuiltShip
		);
	BuiltShipPtr->captains_name_index =
		StarShipCaptain (BuiltShipPtr);
	BuiltShipPtr->cur_status_flags =
		StarShipPlayer (BuiltShipPtr);
	if (GLOBAL_SIS (CrewEnlisted) >= MAX_CREW_SIZE - 1)
	    BuiltShipPtr->special_counter = MAX_CREW_SIZE;
	else
	    BuiltShipPtr->special_counter = (BYTE)(
		    GLOBAL_SIS (CrewEnlisted) + 1
		    );
	BuiltShipPtr->energy_counter = MAX_ENERGY_SIZE;
	UnlockStarShip (
		&race_q[0],
		hBuiltShip
		);
    }
}

PROC(
COUNT InitEncounter, (),
    ARG_VOID
)
{
    COUNT		i;
    MEM_HANDLE		hOldIndex;
    FRAME		SegueFrame;
    STAMP		s;
    TEXT		t;
    extern FRAME	planet[];
MUSIC_REF	MR;

    SetSemaphore (&GraphicsSem);

    hOldIndex = SetResourceIndex (hResIndex);

    SetContext (SpaceContext);
    SetContextDrawState (DEST_PIXMAP | DRAW_REPLACE);
    SetContextFont (TinyFont);

MR = LoadMusicInstance (REDALERT_MUSIC);
PlayMusic (MR, FALSE, 1);
    SegueFrame = CaptureDrawable (LoadGraphic (SEGUE_PMAP_ANIM));
ClearSemaphore (&GraphicsSem);
while (PLRPlaying (MR))
    TaskSwitch ();
StopMusic ();
DestroyMusic (MR);
SetSemaphore (&GraphicsSem);
    s.origin.x = s.origin.y = 0;
    
    BatchGraphics ();
    
    SetContextBackGroundColor (BLACK_COLOR);
    ClearDrawable ();
    s.frame = SegueFrame;
    DrawStamp (&s);

//    t.baseline.x = SIS_SCREEN_WIDTH >> 1;
    t.baseline.x = (SIS_SCREEN_WIDTH >> 1) + 1;
    t.baseline.y = 10;
    t.align = ALIGN_CENTER;

    SetContextFont (MicroFont);
    SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x14), 0x01));
    if (LOBYTE (GLOBAL (CurrentActivity)) == IN_HYPERSPACE)
    {
	t.lpStr = (LPBYTE)"ENCOUNTER IN";
	t.CharCount = (COUNT)~0;
	DrawText (&t);
	t.baseline.y += 12;
	t.lpStr = (LPBYTE)"Deep Space";
	t.CharCount = (COUNT)~0;
	DrawText (&t);
    }
    else
    {
	char	buf[40];

	t.lpStr = (LPBYTE)"ENCOUNTER AT";
	t.CharCount = (COUNT)~0;
	DrawText (&t);
	t.baseline.y += 12;
	GetClusterName (CurStarDescPtr, buf);
	t.lpStr = (LPBYTE)buf;
	t.CharCount = (COUNT)~0;
	DrawText (&t);
	t.baseline.y += 12;
	t.lpStr = (LPBYTE)GLOBAL_SIS (PlanetName);
	t.CharCount = (COUNT)~0;
	DrawText (&t);
    }

    s.origin.x = SIS_SCREEN_WIDTH >> 1;
    s.origin.y = SIS_SCREEN_HEIGHT >> 1;
    s.frame = planet[0];
    DrawStamp (&s);

    if (LOBYTE (GLOBAL (CurrentActivity)) != IN_LAST_BATTLE)
    {
#define NUM_DISPLAY_PTS	(sizeof (display_pt) / sizeof (display_pt[0]))
	HSTARSHIP	hStarShip, hNextShip;
	POINT		display_pt[] =
	{
	    {10, 51},
	    {-10, 51},
	    {33, 40},
	    {-33, 40},
	    {49, 18},
	    {-49, 18},
	    {52, -6},
	    {-52, -6},
	    {44, -27},
	    {-44, -27},
	};

	for (hStarShip = GetHeadLink (&race_q[1]), i = 0;
		hStarShip && i < 60; hStarShip = hNextShip, ++i)
	{
	    RECT		r;
	    STARSHIPPTR		StarShipPtr;

	    StarShipPtr = LockStarShip (&race_q[1], hStarShip);
	    if (StarShipPtr->RaceDescPtr->ship_info.crew_level != (BYTE)~0)
		hNextShip = _GetSuccLink (StarShipPtr);
	    else	/* if infinite ships */
		hNextShip = hStarShip;

	    s.origin = display_pt[i % NUM_DISPLAY_PTS];
	    if (i >= NUM_DISPLAY_PTS)
	    {
		COUNT	angle, radius;

		radius = square_root ((long)s.origin.x * s.origin.x
			+ (long)s.origin.y * s.origin.y)
			+ ((i / NUM_DISPLAY_PTS) * 18);

		angle = ARCTAN (s.origin.x, s.origin.y);
		s.origin.x = COSINE (angle, radius);
		s.origin.y = SINE (angle, radius);
	    }
	    s.frame = SetAbsFrameIndex (
		    StarShipPtr->RaceDescPtr->ship_info.icons, 0
		    );
	    GetFrameRect (s.frame, &r);
	    s.origin.x += (SIS_SCREEN_WIDTH >> 1) - (r.extent.width >> 1);
	    s.origin.y += (SIS_SCREEN_HEIGHT >> 1) - (r.extent.height >> 1);
	    DrawStamp (&s);

	    UnlockStarShip (&race_q[1], hStarShip);
	}
    }

    UnbatchGraphics ();
    DestroyDrawable (ReleaseDrawable (SegueFrame));

    SetResourceIndex (hOldIndex);

    ClearSemaphore (&GraphicsSem);

    {
	MENU_STATE	MenuState;

	MenuState.InputFunc = DoSelectAction;
	MenuState.Initialized = FALSE;

	DrawMenuStateStrings (PM_CONVERSE, MenuState.CurState = HAIL);
	SetSemaphore (&GraphicsSem);
	SetFlashRect ((PRECT)~0L, (FRAME)0);
	ClearSemaphore (&GraphicsSem);

	DoInput ((PVOID)&MenuState);

	SetSemaphore (&GraphicsSem);
	SetFlashRect (NULL_PTR, (FRAME)0);
	ClearSemaphore (&GraphicsSem);

	return (MenuState.CurState);
    }
}

PROC(STATIC
void near DrawFadeText, (str1, str2, fade_in, pRect),
    ARG		(PSTR		str1)
    ARG		(PSTR		str2)
    ARG		(BOOLEAN	fade_in)
    ARG_END	(PRECT		pRect)
)
{
    SIZE	i;
    DWORD	TimeIn;
    TEXT	t1, t2;
    COLOR	fade_cycle[] =
    {
	BUILD_COLOR (MAKE_RGB15 (0xA, 0xA, 0xA), 0x1D),
	BUILD_COLOR (MAKE_RGB15 (0x9, 0x9, 0x9), 0x1E),
	BUILD_COLOR (MAKE_RGB15 (0x8, 0x8, 0x8), 0x1F),
	BUILD_COLOR (MAKE_RGB15 (0x6, 0x6, 0x6), 0x20),
	BUILD_COLOR (MAKE_RGB15 (0x5, 0x5, 0x5), 0x21),
	BUILD_COLOR (MAKE_RGB15 (0x4, 0x4, 0x4), 0x22),
	BUILD_COLOR (MAKE_RGB15 (0x3, 0x3, 0x3), 0x23),
    };
#define NUM_FADES	(sizeof (fade_cycle) / sizeof (fade_cycle[0]))

    t1.baseline.x = pRect->corner.x + 100;
    t1.baseline.y = pRect->corner.y + 45;
    t1.align = ALIGN_CENTER;
    t1.lpStr = (LPBYTE)str1;
    t1.CharCount = (COUNT)~0;
    t2 = t1;
    t2.baseline.y += 11;
    t2.lpStr = (LPBYTE)str2;

    FlushInput ();
    TimeIn = GetTimeCounter ();
    if (fade_in)
    {
	for (i = 0; i < NUM_FADES; ++i)
	{
	    ClearSemaphore (&GraphicsSem);
	    if (AnyButtonPress (TRUE))
		i = NUM_FADES - 1;
	    SetSemaphore (&GraphicsSem);

	    SetContextForeGroundColor (fade_cycle[i]);
	    DrawText (&t1);
	    DrawText (&t2);
	    TimeIn = SleepTask (TimeIn + (ONE_SECOND / 20));
	}
    }
    else
    {
	for (i = NUM_FADES - 1; i >= 0; --i)
	{
	    ClearSemaphore (&GraphicsSem);
	    if (AnyButtonPress (TRUE))
		i = 0;
	    SetSemaphore (&GraphicsSem);

	    SetContextForeGroundColor (fade_cycle[i]);
	    DrawText (&t1);
	    DrawText (&t2);
	    TimeIn = SleepTask (TimeIn + (ONE_SECOND / 20));
	}
	SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0xA, 0xA, 0xA), 0x08));
	DrawText (&t1);
	DrawText (&t2);
    }
}

PROC(
COUNT UninitEncounter, (),
    ARG_VOID
)
{
    COUNT	ships_killed;

    ships_killed = 0;

    SetSemaphore (&GraphicsSem);
    free_gravity_well ();

    if ((GLOBAL (CurrentActivity) & (CHECK_ABORT | CHECK_LOAD))
	    || GLOBAL_SIS (CrewEnlisted) == (COUNT)~0
	    || LOBYTE (GLOBAL (CurrentActivity)) == IN_LAST_BATTLE
	    || LOBYTE (GLOBAL (CurrentActivity)) == WON_LAST_BATTLE)
	goto ExitUninitEncounter;

    if (GET_GAME_STATE (BATTLE_SEGUE) == 0)
    {
	ReinitQueue (&race_q[0]);
	ReinitQueue (&race_q[1]);
    }
    else
    {
	BOOLEAN			Sleepy;
	DWORD			Time;
	SIZE			VictoryState;
	COUNT			RecycleAmount;
	SIZE			i;
	RECT			r, scavenge_r;
	TEXT			t;
	STAMP			ship_s;
	PSTR			str1, str2;
	char			buf[20];
	HSTARSHIP		hStarShip;
	SHIP_FRAGMENTPTR	FragPtr;
	COLOR			fade_ship_cycle[] =
	{
	    BUILD_COLOR (MAKE_RGB15 (0x7, 0x00, 0x00), 0x2F),
	    BUILD_COLOR (MAKE_RGB15 (0xF, 0x00, 0x00), 0x2D),
	    BUILD_COLOR (MAKE_RGB15 (0x17, 0x00, 0x00), 0x2B),
	    BUILD_COLOR (MAKE_RGB15 (0x1F, 0xA, 0xA), 0x27),
	    BUILD_COLOR (MAKE_RGB15 (0x1F, 0x14, 0x14), 0x25),
	    BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1F, 0x1F), 0x0F),
	    BUILD_COLOR (MAKE_RGB15 (0x1F, 0x14, 0x14), 0x25),
	    BUILD_COLOR (MAKE_RGB15 (0x1F, 0xA, 0xA), 0x27),
	    BUILD_COLOR (MAKE_RGB15 (0x1B, 0x00, 0x00), 0x2A),
	    BUILD_COLOR (MAKE_RGB15 (0x17, 0x00, 0x00), 0x2B),
	};
#define NUM_SHIP_FADES	(sizeof (fade_ship_cycle) / sizeof (fade_ship_cycle[0]))
	COUNT			race_bounty[] =
	{
	    RACE_SHIP_COST
	};

	SET_GAME_STATE (BATTLE_SEGUE, 0);
	SET_GAME_STATE (BOMB_CARRIER, 0);

	VictoryState = (
		HIBYTE (battle_counter) || !LOBYTE (battle_counter)
		|| GET_GAME_STATE (URQUAN_PROTECTING_SAMATRA)
		) ? 0 : 1;

	hStarShip = GetHeadLink (&GLOBAL (npc_built_ship_q));
	FragPtr = (SHIP_FRAGMENTPTR)LockStarShip (&GLOBAL (npc_built_ship_q), hStarShip);
	battle_counter = GET_RACE_ID (FragPtr) + 1;
	if (GetStarShipFromIndex (
		&GLOBAL (avail_race_q), 
		(COUNT)(battle_counter - 1)
		) == 0)
	{
	    VictoryState = -1;
	    InitSISContexts ();
	}
	UnlockStarShip (&GLOBAL (npc_built_ship_q), hStarShip);

	Sleepy = TRUE;
	for (i = 0; i < NUM_SIDES; ++i)
	{
	    PQUEUE	pQueue;
	    HSTARSHIP	hNextShip;

	    if (i == 0)
		pQueue = &GLOBAL (built_ship_q);
	    else
	    {
		if (VictoryState < 0)
		    VictoryState = 0;
		else
		{
		    ClearSemaphore (&GraphicsSem);
		    DrawSISFrame ();
		    SetSemaphore (&GraphicsSem);

		    DrawSISMessage (NULL_PTR);
		    if (LOBYTE (GLOBAL (CurrentActivity)) == IN_HYPERSPACE)
			DrawHyperCoords (
				GLOBAL (ShipStamp.origin)
				);
		    else if (HIWORD (GLOBAL (ShipStamp.frame)) == 0)
			DrawHyperCoords (CurStarDescPtr->star_pt);
		    else
			DrawSISTitle((PBYTE)GLOBAL_SIS (PlanetName));

		    SetContext (SpaceContext);
		    if (VictoryState)
			DrawArmadaPickShip (TRUE, &scavenge_r);
		}
		pQueue = &GLOBAL (npc_built_ship_q);
	    }
		
	    ReinitQueue (&race_q[(NUM_SIDES - 1) - i]);

	    for (hStarShip = GetHeadLink (pQueue); hStarShip; hStarShip = hNextShip)
	    {
		FragPtr = (SHIP_FRAGMENTPTR)LockStarShip (pQueue, hStarShip);
		hNextShip = _GetSuccLink (FragPtr);

		if (FragPtr->ShipInfo.crew_level == 0
			|| (VictoryState && i == NUM_SIDES - 1))
		{
		    if (i == NUM_SIDES - 1)
		    {
			++ships_killed;
			if (VictoryState)
			{
#define MAX_DEAD_DISPLAYED	5
			    COUNT	j;

			    if (ships_killed == 1)
			    {
				RecycleAmount = 0;

				DrawStatusMessage ((LPSTR)~0);
				
				ship_s.origin.x = scavenge_r.corner.x + 32;
				ship_s.origin.y = scavenge_r.corner.y + 56;
				ship_s.frame = IncFrameIndex (FragPtr->ShipInfo.icons);
				DrawStamp (&ship_s);
				SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x8, 0x8, 0x8), 0x1F));
				SetContextFont (TinyFont);

				GetStringContents (
					FragPtr->ShipInfo.race_strings, (STRINGPTR)buf, FALSE
					);
				strupr (buf);

				t.baseline.x = scavenge_r.corner.x + 100;
				t.baseline.y = scavenge_r.corner.y + 68;
				t.align = ALIGN_CENTER;
				t.lpStr = (LPBYTE)buf;
				t.CharCount = (COUNT)~0;
				DrawText (&t);
				t.baseline.y += 6;
				t.lpStr = (LPBYTE)"BATTLE GROUP";
				t.CharCount = (COUNT)~0;
				DrawText (&t);

				ship_s.frame = FragPtr->ShipInfo.icons;

				SetContextFont (MicroFont);
				DrawFadeText (
					str1 = (PSTR)"Enemy Ships",
					str2 = (PSTR)"Destroyed",
					TRUE,
					&scavenge_r
					);
			    }

			    r.corner.y = scavenge_r.corner.y + 9;
			    r.extent.height = 22;

			    SetContextForeGroundColor (BLACK_COLOR);

			    r.extent.width = 34;
			    r.corner.x = scavenge_r.corner.x + scavenge_r.extent.width
				    - (10 + r.extent.width);
			    DrawFilledRectangle (&r);

				    /* collect bounty ResUnits */
			    j = race_bounty[battle_counter - 1] >> 3;
			    RecycleAmount += j;
			    sprintf (buf, "%u", RecycleAmount);
			    t.baseline.x = r.corner.x + r.extent.width - 1;
			    t.baseline.y = r.corner.y + 14;
			    t.align = ALIGN_RIGHT;
			    t.lpStr = (LPBYTE)buf;
			    t.CharCount = (COUNT)~0;
			    SetContextForeGroundColor (
				    BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x18), 0x50)
				    );
			    DrawText (&t);
			    DeltaSISGauges (0, 0, j);

			    if ((VictoryState++ - 1) % MAX_DEAD_DISPLAYED)
				ship_s.origin.x += 17;
			    else
			    {
				SetContextForeGroundColor (BLACK_COLOR);

				r.corner.x = scavenge_r.corner.x + 10;
				r.extent.width = 104;
				DrawFilledRectangle (&r);

				ship_s.origin.x = r.corner.x + 2;
				ship_s.origin.y = scavenge_r.corner.y + 12;
			    }

			    if (Sleepy)
			    {
				Time = GetTimeCounter ();
				for (j = 0; j < NUM_SHIP_FADES; ++j)
				{
				    ClearSemaphore (&GraphicsSem);
				    Sleepy = (BOOLEAN)!AnyButtonPress (TRUE);
				    SetSemaphore (&GraphicsSem);
				    if (!Sleepy)
					break;

				    SetContextForeGroundColor (fade_ship_cycle[j]);
				    DrawFilledStamp (&ship_s);

				    Time = SleepTask (Time + (ONE_SECOND / 15));
				}
			    }
			    DrawStamp (&ship_s);
			}
		    }

		    UnlockStarShip (pQueue, hStarShip);
		    RemoveQueue (pQueue, hStarShip);
		    FreeStarShip (pQueue, hStarShip);

		    continue;
		}

		UnlockStarShip (pQueue, hStarShip);
	    }
	}

	if (VictoryState)
	{
	    INPUT_STATE	InputState;

#ifdef NEVER
	    DestroyDrawable (ReleaseDrawable (s.frame));
#endif /* NEVER */

	    FlushInput ();
	    Time = GetTimeCounter () + (ONE_SECOND * 3);
	    ClearSemaphore (&GraphicsSem);
	    while (!(InputState = AnyButtonPress (TRUE)) && GetTimeCounter () < Time)
		;
	    SetSemaphore (&GraphicsSem);
	    if (GetInputASCII (InputState) != 0x1B)	/* Escape key */
	    {
		DrawFadeText (str1, str2, FALSE, &scavenge_r);
		if (GetInputASCII (InputState) != 0x1B)	/* Escape key */
		{
		    SetContextForeGroundColor (BLACK_COLOR);
		    r.corner.x = scavenge_r.corner.x + 10;
		    r.extent.width = 132;
		    DrawFilledRectangle (&r);
		    sprintf (buf, "%u RU", RecycleAmount);
		    t.baseline.x = r.corner.x + (r.extent.width >> 1);
		    t.baseline.y = r.corner.y + 14;
		    t.align = ALIGN_CENTER;
		    t.lpStr = (LPBYTE)buf;
		    t.CharCount = (COUNT)~0;
		    SetContextForeGroundColor (
			    BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x18), 0x50)
			    );
		    DrawText (&t);

		    DrawFadeText (
			    str1 = (PSTR)"Debris",
			    str2 = (PSTR)"Scavenged",
			    TRUE,
			    &scavenge_r
			    );
		    Time = GetTimeCounter () + ONE_SECOND * 2;
		    ClearSemaphore (&GraphicsSem);
		    while (!(InputState = AnyButtonPress (TRUE))
			    && GetTimeCounter () < Time)
			;
		    SetSemaphore (&GraphicsSem);
		    if (GetInputASCII (InputState) != 0x1B)	/* Escape key */
			DrawFadeText (str1, str2, FALSE, &scavenge_r);
		}
	    }

	    DrawStatusMessage (NULL_PTR);
	}

	if (ships_killed && (battle_counter - 1) == THRADDASH_SHIP
		&& !GET_GAME_STATE (THRADD_MANNER))
	{
	    if ((ships_killed += GET_GAME_STATE (THRADDASH_BODY_COUNT)) >
		    THRADDASH_BODY_THRESHOLD)
		ships_killed = THRADDASH_BODY_THRESHOLD;
	    SET_GAME_STATE (THRADDASH_BODY_COUNT, ships_killed);
	}
    }
ExitUninitEncounter:
#ifdef NEVER
    DestroyDrawable (ReleaseDrawable (BackFrame));
    BackFrame = 0;
#endif /* NEVER */
    ClearSemaphore (&GraphicsSem);

    return (ships_killed);
}

PROC(
void EncounterBattle, (),
    ARG_VOID
)
{
    MEM_HANDLE		hLastIndex;
    ACTIVITY		OldActivity;
    extern UWORD	nth_frame;
    PROC_GLOBAL(
    BOOLEAN LoadSC2Data, (),
	ARG_VOID
    );
    PROC_GLOBAL(
    BOOLEAN FreeSC2Data, (),
	ARG_VOID
    );

    SetSemaphore (&GraphicsSem);

    SET_GAME_STATE (BATTLE_SEGUE, 1);

    hLastIndex = SetResourceIndex (hResIndex);

    OldActivity = GLOBAL (CurrentActivity);
    if (LOBYTE (OldActivity) == IN_LAST_BATTLE)
	GLOBAL (CurrentActivity) = MAKE_WORD (IN_LAST_BATTLE, 0);
    else
	GLOBAL (CurrentActivity) = MAKE_WORD (IN_ENCOUNTER, 0);

//    FreeSC2Data ();
//    DestroyFont (ReleaseFont (MicroFont));
    while (SoundPlaying ())
	;
//    DestroySound (ReleaseSound (MenuSounds));

    if (GLOBAL (glob_flags) & CYBORG_ENABLED)
    {
	BYTE	cur_speed;

	cur_speed = (BYTE)(GLOBAL (glob_flags) & COMBAT_SPEED_MASK)
		>> COMBAT_SPEED_SHIFT;
	if (cur_speed == 1)
	    cur_speed = 0;
	else if (cur_speed == 2)
	    ++cur_speed;
	else /* if (cur_speed == 3) */
	    cur_speed = (BYTE)~0;
	nth_frame = MAKE_WORD (1, cur_speed);
	PlayerControl[0] = CYBORG_CONTROL | AWESOME_RATING;
	PlayerInput[0] = ComputerInput;
    }

    GameSounds = CaptureSound (LoadSound (GAME_SOUNDS));

    ClearSemaphore (&GraphicsSem);
    Battle ();
    SetSemaphore (&GraphicsSem);

    DestroySound (ReleaseSound (GameSounds));
    GameSounds = 0;

    if (GLOBAL (CurrentActivity) & CHECK_ABORT)
	GLOBAL_SIS (CrewEnlisted) = (COUNT)~0;

    if (GLOBAL (glob_flags) & CYBORG_ENABLED)
    {
	nth_frame = MAKE_WORD (0, 0);
	PlayerControl[0] = HUMAN_CONTROL | JOYSTICK_CONTROL | STANDARD_RATING,
	PlayerInput[0] = NormalInput;
    }

    SetResourceIndex (hResIndex);
//    MicroFont = CaptureFont (
//	    LoadGraphic (MICRO_FONT)
//	    );
//    MenuSounds = CaptureSound (LoadSound (MENU_SOUNDS));
//    LoadSC2Data ();

    GLOBAL (CurrentActivity) = OldActivity;

    SetResourceIndex (hLastIndex);

    ClearSemaphore (&GraphicsSem);
}

