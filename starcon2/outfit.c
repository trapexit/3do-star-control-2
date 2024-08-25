#include "starcon.h"

enum
{
    OUTFIT_FUEL,
    OUTFIT_MODULES,
    OUTFIT_SAVELOAD,
    OUTFIT_EXIT,
    OUTFIT_DOFUEL
};

PROC(STATIC
void near DrawModuleStrings, (pMS, NewModule),
    ARG		(PMENU_STATE	pMS)
    ARG_END	(BYTE		NewModule)
)
{
    RECT	r;
    STAMP	s;
    CONTEXT	OldContext;

    OldContext = SetContext (StatusContext);
    GetContextClipRect (&r);
    s.origin.x = RADAR_X - r.corner.x;
    s.origin.y = RADAR_Y - r.corner.y;
    r.corner.x = s.origin.x - 1;
    r.corner.y = s.origin.y - 11;
    r.extent.width = RADAR_WIDTH + 2;
    r.extent.height = 11;
    BatchGraphics ();
    SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0xA, 0xA, 0xA), 0x08));
    DrawFilledRectangle (&r);
    if (NewModule >= EMPTY_SLOT)
    {
	r.corner = s.origin;
	r.extent.width = RADAR_WIDTH;
	r.extent.height = RADAR_HEIGHT;
	SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x00), 0x00));
	DrawFilledRectangle (&r);
    }
    else if (pMS->CurFrame)
    {
	TEXT	t;
	char	buf[10];

	s.frame = SetAbsFrameIndex (pMS->CurFrame, NewModule);
	DrawStamp (&s);
	t.baseline.x = s.origin.x + RADAR_WIDTH - 2;
	t.baseline.y = s.origin.y + RADAR_HEIGHT - 2;
	t.align = ALIGN_RIGHT;
	t.CharCount = (COUNT)~0;
	t.lpStr = (LPBYTE)buf;
	sprintf (buf, "%u", GLOBAL (ModuleCost[NewModule]) * MODULE_COST_SCALE);
	SetContextFont (TinyFont);
	SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x00, 0x1F, 0x00), 0x02));
	DrawText (&t);
    }
    UnbatchGraphics ();
    SetContext (OldContext);
}

PROC(STATIC
void near RedistributeFuel, (),
    ARG_VOID
)
{
    COUNT	m;
    DWORD	FuelVolume;
    RECT	r;

    if ((FuelVolume = GLOBAL_SIS (FuelOnBoard)) <= FUEL_RESERVE)
	return;

    GLOBAL_SIS (FuelOnBoard) = 0;
    m = FUEL_VOLUME_PER_ROW;

    r.extent.width = 3;
    r.extent.height = 1;
    while (FuelVolume -= m)
    {
	GLOBAL_SIS (FuelOnBoard) += FUEL_VOLUME_PER_ROW;
	GetFTankCapacity (&r.corner);
	DrawPoint (&r.corner);
	r.corner.x += r.extent.width + 1;
	DrawPoint (&r.corner);
	r.corner.x -= r.extent.width;
	SetContextForeGroundColor (SetContextBackGroundColor (BLACK_COLOR));
	DrawFilledRectangle (&r);
	if (FuelVolume < FUEL_VOLUME_PER_ROW)
	    m = (COUNT)FuelVolume;
    }

    FuelVolume = GLOBAL_SIS (FuelOnBoard) + m;

    r.extent.width = 5;
    while ((GLOBAL_SIS (FuelOnBoard) += FUEL_VOLUME_PER_ROW) <
	    GetFTankCapacity (&r.corner))
    {
	SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0xB, 0x00, 0x00), 0x2E));
	DrawFilledRectangle (&r);
    }

    GLOBAL_SIS (FuelOnBoard) = FuelVolume;
}

#define LANDER_X	11
#define LANDER_Y	67
#define LANDER_WIDTH	15

PROC(STATIC
void near DisplayLanders, (pMS),
    ARG_END	(PMENU_STATE	pMS)
)
{
    STAMP	s;

    s.frame = pMS->ModuleFrame;
    if (GET_GAME_STATE (CHMMR_BOMB_STATE) == 3)
    {
	s.origin.x = s.origin.y = 0;
	s.frame = DecFrameIndex (s.frame);
	DrawStamp (&s);
    }
    else
    {
	COUNT	i;

	s.origin.x = LANDER_X;
	s.origin.y = LANDER_Y;
	for (i = 0; i < GLOBAL_SIS (NumLanders); ++i)
	{
	    DrawStamp (&s);
	    s.origin.x += LANDER_WIDTH;
	}

	SetContextForeGroundColor (BLACK_COLOR);
	for (; i < MAX_LANDERS; ++i)
	{
	    DrawFilledStamp (&s);
	    s.origin.x += LANDER_WIDTH;
	}
    }
}

PROC(STATIC
BOOLEAN DoInstallModule, (InputState, pMS),
    ARG		(INPUT_STATE	InputState)
    ARG_END	(PMENU_STATE	pMS)
)
{
    BYTE	NewState, new_slot_piece, old_slot_piece;
    SIZE	FirstItem, LastItem;
    STAMP	s;

    if (GLOBAL (CurrentActivity) & CHECK_ABORT)
    {
	pMS->InputFunc = DoOutfit;
	return (TRUE);
    }

    FirstItem = 0;
    switch (NewState = pMS->CurState)
    {
	case PLANET_LANDER:
	case EMPTY_SLOT + 3:
	    old_slot_piece = pMS->delta_item < GLOBAL_SIS (NumLanders)
		    ? PLANET_LANDER : (EMPTY_SLOT + 3);
	    LastItem = MAX_LANDERS - 1;
	    break;
	case FUSION_THRUSTER:
	case EMPTY_SLOT + 0:
	    old_slot_piece =
		    GLOBAL_SIS (DriveSlots[pMS->delta_item]);
	    LastItem = NUM_DRIVE_SLOTS - 1;
	    break;
	case TURNING_JETS:
	case EMPTY_SLOT + 1:
	    old_slot_piece =
		    GLOBAL_SIS (JetSlots[pMS->delta_item]);
	    LastItem = NUM_JET_SLOTS - 1;
	    break;
	default:
	    old_slot_piece =
		    GLOBAL_SIS (ModuleSlots[pMS->delta_item]);
	    if (GET_GAME_STATE (CHMMR_BOMB_STATE) == 3)
		FirstItem = NUM_BOMB_MODULES;
	    LastItem = NUM_MODULE_SLOTS - 1;
	    break;
    }
    
    if (NewState < CREW_POD)
	FirstItem = LastItem = NewState;
    else if (NewState < EMPTY_SLOT)
	FirstItem = CREW_POD, LastItem = NUM_PURCHASE_MODULES - 1;

    if (!pMS->Initialized)
    {
	new_slot_piece = old_slot_piece;
	pMS->Initialized = TRUE;

	pMS->InputFunc = DoInstallModule;

	SetSemaphore (&GraphicsSem);

	SetContext (SpaceContext);
	SetContextDrawState (DEST_PIXMAP | DRAW_REPLACE);

	SetFlashRect (NULL_PTR, (FRAME)0);
	goto InitFlash;
    }
    else if (InputState & (DEVICE_BUTTON1 | DEVICE_BUTTON2))
    {
	new_slot_piece = pMS->CurState;
	if (InputState & DEVICE_BUTTON1)
	{
	    if (new_slot_piece < EMPTY_SLOT)
	    {
		if (GLOBAL_SIS (ResUnits) <
			(DWORD)(GLOBAL (ModuleCost[new_slot_piece])
			* MODULE_COST_SCALE))
		    return (TRUE);
	    }
	    else if (new_slot_piece == EMPTY_SLOT + 2)
	    {
		if (old_slot_piece == CREW_POD)
		{
		    if (GLOBAL_SIS (CrewEnlisted) > CREW_POD_CAPACITY
			    * (CountSISPieces (CREW_POD) - 1))
			return (TRUE);
		}
		else if (old_slot_piece == FUEL_TANK
			|| old_slot_piece == HIGHEFF_FUELSYS)
		{
		    DWORD	volume;

		    volume = (DWORD)CountSISPieces (FUEL_TANK)
			    * FUEL_TANK_CAPACITY
			    + (DWORD)CountSISPieces (HIGHEFF_FUELSYS)
			    * HEFUEL_TANK_CAPACITY;
		    volume -= (old_slot_piece == FUEL_TANK
			    ? FUEL_TANK_CAPACITY : HEFUEL_TANK_CAPACITY);
		    if (GLOBAL_SIS (FuelOnBoard) > volume + FUEL_RESERVE)
			return (TRUE);
		}
		else if (old_slot_piece == STORAGE_BAY)
		{
		    if (GLOBAL_SIS (TotalElementMass) > STORAGE_BAY_CAPACITY
			    * (CountSISPieces (STORAGE_BAY) - 1))
			return (TRUE);
		}
	    }
	}

	SetSemaphore (&GraphicsSem);
	SetContext (SpaceContext);

	SetFlashRect (NULL_PTR, (FRAME)0);

	if (InputState & DEVICE_BUTTON1)
	{
	    if (new_slot_piece >= EMPTY_SLOT && old_slot_piece >= EMPTY_SLOT)
	    {
		new_slot_piece -= EMPTY_SLOT - 1;
		if (new_slot_piece > CREW_POD)
		    new_slot_piece = PLANET_LANDER;
	    }
	    else
	    {
		switch (pMS->CurState)
		{
		    case PLANET_LANDER:
			++GLOBAL_SIS (NumLanders);
			break;
		    case EMPTY_SLOT + 3:
			--GLOBAL_SIS (NumLanders);
			break;
		    case FUSION_THRUSTER:
		    case EMPTY_SLOT + 0:
			GLOBAL_SIS (DriveSlots[pMS->delta_item]) =
				new_slot_piece;
			break;
		    case TURNING_JETS:
		    case EMPTY_SLOT + 1:
			GLOBAL_SIS (JetSlots[pMS->delta_item]) =
				new_slot_piece;
			break;
		    default:
			GLOBAL_SIS (ModuleSlots[pMS->delta_item]) =
				new_slot_piece;
			break;
		}

		if (new_slot_piece < EMPTY_SLOT)
		    DeltaSISGauges (UNDEFINED_DELTA, UNDEFINED_DELTA,
			    -(GLOBAL (ModuleCost[new_slot_piece])
			    * MODULE_COST_SCALE));
		else /* if (old_slot_piece < EMPTY_SLOT) */
		    DeltaSISGauges (UNDEFINED_DELTA, UNDEFINED_DELTA,
			    GLOBAL (ModuleCost[old_slot_piece])
			    * MODULE_COST_SCALE);

		if (WRAP_VAL (pMS->CurState, EMPTY_SLOT + 3) == PLANET_LANDER)
		    DisplayLanders (pMS);
		else
		{
		    DrawShipPiece (pMS, new_slot_piece, pMS->delta_item, FALSE);

		    if (new_slot_piece > TURNING_JETS
			    && old_slot_piece > TURNING_JETS)
			RedistributeFuel ();
		}
	    }

	    InputState &= ~DEVICE_BUTTON2;
	}

	if (pMS->CurState < EMPTY_SLOT)
	{
	    pMS->CurState += EMPTY_SLOT - 1;
	    if (pMS->CurState < EMPTY_SLOT)
		pMS->CurState = EMPTY_SLOT + 3;
	    else if (pMS->CurState > EMPTY_SLOT + 2)
		pMS->CurState = EMPTY_SLOT + 2;
	    if (InputState & DEVICE_BUTTON2)
		new_slot_piece = pMS->CurState;
	    goto InitFlash;
	}
	else if (!(InputState & DEVICE_BUTTON2))
	{
	    pMS->CurState = new_slot_piece;
	    goto InitFlash;
	}
	else
	{
	    SetContext (StatusContext);
	    ClearSemaphore (&GraphicsSem);
	    DrawMenuStateStrings (PM_FUEL, pMS->CurState = OUTFIT_MODULES);
	    SetSemaphore (&GraphicsSem);
	    SetFlashRect ((PRECT)~0L, (FRAME)0);

	    pMS->InputFunc = DoOutfit;
	    ClearSISRect (DRAW_SIS_DISPLAY);
	}
	ClearSemaphore (&GraphicsSem);
    }
    else if (InputState)
    {
	SIZE	NewItem;

	NewItem = NewState < EMPTY_SLOT ? pMS->CurState : pMS->delta_item;
	do
	{
	    SBYTE	dy;

	    dy = GetInputYComponent (InputState);
	    if (NewState >= EMPTY_SLOT && dy)
	    {
		if (dy < 0)
		{
		    if (NewState-- == EMPTY_SLOT)
			NewState = EMPTY_SLOT + 3;
		}
		else
		{
		    if (NewState++ == EMPTY_SLOT + 3)
			NewState = EMPTY_SLOT;
		}
		NewItem = 0;
		if (GET_GAME_STATE (CHMMR_BOMB_STATE) == 3)
		{
		    if (NewState == EMPTY_SLOT + 3)
			NewState = dy < 0 ? EMPTY_SLOT + 2 : EMPTY_SLOT;
		    if (NewState == EMPTY_SLOT + 2)
			NewItem = NUM_BOMB_MODULES;
		}
		pMS->delta_item = NewItem;
	    }
	    else if (GetInputXComponent (InputState) < 0 || dy < 0)
	    {
		if (NewItem-- == FirstItem)
		    NewItem = LastItem;
	    }
	    else if (GetInputXComponent (InputState) > 0 || dy > 0)
	    {
		if (NewItem++ == LastItem)
		    NewItem = FirstItem;
	    }
	} while (NewState < EMPTY_SLOT
		&& (GLOBAL (ModuleCost[NewItem]) == 0
		|| (NewItem >= GUN_WEAPON && NewItem <= CANNON_WEAPON
		&& pMS->delta_item > 0 && pMS->delta_item < 13)));

	SetSemaphore (&GraphicsSem);
	if (NewState < EMPTY_SLOT)
	{
	    if (NewItem != pMS->CurState)
	    {
		DrawModuleStrings (pMS, pMS->CurState = NewItem);
		SetFlashRect ((PRECT)~0L, (FRAME)0);
	    }
	}
	else if (NewItem != pMS->delta_item || NewState != pMS->CurState)
	{
	    SIZE	w;
		
	    switch (NewState)
	    {
		case PLANET_LANDER:
		case EMPTY_SLOT + 3:
		    new_slot_piece = NewItem < GLOBAL_SIS (NumLanders)
			    ? PLANET_LANDER : (EMPTY_SLOT + 3);
		    break;
		case FUSION_THRUSTER:
		case EMPTY_SLOT + 0:
		    new_slot_piece = GLOBAL_SIS (DriveSlots[NewItem]);
		    break;
		case TURNING_JETS:
		case EMPTY_SLOT + 1:
		    new_slot_piece = GLOBAL_SIS (JetSlots[NewItem]);
		    break;
		default:
		    new_slot_piece = GLOBAL_SIS (ModuleSlots[NewItem]);
		    break;
	    }

	    SetContext (SpaceContext);

	    if (NewState == pMS->CurState)
	    {
		if (NewState == PLANET_LANDER || NewState == EMPTY_SLOT + 3)
		    w = LANDER_WIDTH;
		else
		    w = SHIP_PIECE_OFFSET;

		w *= (NewItem - pMS->delta_item);
		pMS->flash_rect0.corner.x += w;
		pMS->delta_item = NewItem;
	    }
	    else
	    {
		pMS->CurState = NewState;
InitFlash:
		w = SHIP_PIECE_OFFSET;
		switch (pMS->CurState)
		{
		    case PLANET_LANDER:
		    case EMPTY_SLOT + 3:
			pMS->flash_rect0.corner.x = LANDER_X - 1;
			pMS->flash_rect0.corner.y = LANDER_Y - 1;
			pMS->flash_rect0.extent.width = 11 + 2;
			pMS->flash_rect0.extent.height = 13 + 2;

			w = LANDER_WIDTH;
			break;
		    case FUSION_THRUSTER:
		    case EMPTY_SLOT + 0:
			pMS->flash_rect0.corner.x = DRIVE_TOP_X - 1;
			pMS->flash_rect0.corner.y = DRIVE_TOP_Y - 1;
			pMS->flash_rect0.extent.width = 8;
			pMS->flash_rect0.extent.height = 6;

			break;
		    case TURNING_JETS:
		    case EMPTY_SLOT + 1:
			pMS->flash_rect0.corner.x = JET_TOP_X - 1;
			pMS->flash_rect0.corner.y = JET_TOP_Y - 1;
			pMS->flash_rect0.extent.width = 9;
			pMS->flash_rect0.extent.height = 10;

			break;
		    default:
			pMS->flash_rect0.corner.x = MODULE_TOP_X - 1;
			pMS->flash_rect0.corner.y = MODULE_TOP_Y - 1;
			pMS->flash_rect0.extent.width = 15;
			pMS->flash_rect0.extent.height = 34;

			break;
		}

		w *= pMS->delta_item;
		pMS->flash_rect0.corner.x += w;
	    }

	    DrawModuleStrings (pMS, new_slot_piece);
	    if (pMS->CurState < EMPTY_SLOT)
		SetFlashRect ((PRECT)~0L, (FRAME)0);
	    else
		SetFlashRect (&pMS->flash_rect0, (FRAME)0);
	}
	ClearSemaphore (&GraphicsSem);
    }

    return (TRUE);
}

PROC(
BOOLEAN	DoOutfit, (InputState, pMS),
    ARG		(INPUT_STATE	InputState)
    ARG_END	(PMENU_STATE	pMS)
)
{
    if (GLOBAL (CurrentActivity) & CHECK_ABORT)
	goto ExitOutfit;

    if (!pMS->Initialized)
    {
	pMS->InputFunc = DoOutfit;
	pMS->Initialized = TRUE;

	{
	    COUNT	num_frames;
	    STAMP	s;
	    PROC_GLOBAL(
	    void DrawFlagshipName, (InStatusArea),
		ARG_END	(BOOLEAN	InStatusArea)
	    );

	    pMS->CurFrame = CaptureDrawable (
		    LoadGraphic (MODULES_PMAP_ANIM)
		    );
	    pMS->hMusic = LoadMusicInstance (OUTFIT_MUSIC);
	    pMS->CurState = OUTFIT_FUEL;
	    pMS->ModuleFrame = CaptureDrawable (
		    LoadGraphic (SISMODS_MASK_PMAP_ANIM)
		    );
	    s.origin.x = s.origin.y = 0;
	    s.frame = CaptureDrawable (
		    LoadGraphic (OUTFIT_PMAP_ANIM)
		    );

	    BatchGraphics ();
	    DrawSISFrame ();
	    DrawSISMessage ((LPSTR)starbase_str_array[OUTFIT_STARSHIP]);
	    DrawSISTitle ((PBYTE)"Starbase");

	    SetContext (SpaceContext);
	    SetContextDrawState (DEST_PIXMAP | DRAW_REPLACE);

	    DrawStamp (&s);
	    FlushGraphics (TRUE);
	    DestroyDrawable (ReleaseDrawable (s.frame));

	    for (num_frames = 0; num_frames < NUM_DRIVE_SLOTS; ++num_frames)
	    {
		BYTE	which_piece;

		if ((which_piece =
			GLOBAL_SIS (DriveSlots[num_frames])) < EMPTY_SLOT)
		    DrawShipPiece (pMS, which_piece, num_frames, FALSE);
	    }
	    for (num_frames = 0; num_frames < NUM_JET_SLOTS; ++num_frames)
	    {
		BYTE	which_piece;

		if ((which_piece =
			GLOBAL_SIS (JetSlots[num_frames])) < EMPTY_SLOT)
		    DrawShipPiece (pMS, which_piece, num_frames, FALSE);
	    }
	    for (num_frames = 0; num_frames < NUM_MODULE_SLOTS; ++num_frames)
	    {
		BYTE	which_piece;

		if ((which_piece =
			GLOBAL_SIS (ModuleSlots[num_frames])) < EMPTY_SLOT)
		    DrawShipPiece (pMS, which_piece, num_frames, FALSE);
	    }

	    RedistributeFuel ();
	    DisplayLanders (pMS);
	    if (GET_GAME_STATE (CHMMR_BOMB_STATE) < 3)
	    {
		BYTE	ShieldFlags;
		
		ShieldFlags = GET_GAME_STATE (LANDER_SHIELDS);

		s.frame = SetAbsFrameIndex (pMS->ModuleFrame,
			GetFrameCount (pMS->ModuleFrame) - 5);
		if (ShieldFlags & (1 << EARTHQUAKE_DISASTER))
		    DrawStamp ((LPSTAMP)&s);
		s.frame = IncFrameIndex (s.frame);
		if (ShieldFlags & (1 << BIOLOGICAL_DISASTER))
		    DrawStamp ((LPSTAMP)&s);
		s.frame = IncFrameIndex (s.frame);
		if (ShieldFlags & (1 << LIGHTNING_DISASTER))
		    DrawStamp ((LPSTAMP)&s);
		s.frame = IncFrameIndex (s.frame);
		if (ShieldFlags & (1 << LAVASPOT_DISASTER))
		    DrawStamp ((LPSTAMP)&s);
	    }

	    DrawMenuStateStrings (PM_FUEL, pMS->CurState);
	    DrawFlagshipName (FALSE);

{
    RECT	r;

    r.corner.x = 0;
    r.corner.y = 0;
    r.extent.width = SCREEN_WIDTH;
    r.extent.height = SCREEN_HEIGHT;
    ScreenTransition (3, &r);
}
	    PlayMusic (pMS->hMusic, TRUE, 1);
	    UnbatchGraphics ();
	    
	    SetSemaphore (&GraphicsSem);
	    SetFlashRect ((PRECT)~0L, (FRAME)0);
	    ClearSemaphore (&GraphicsSem);

	    GLOBAL_SIS (FuelOnBoard) =
		    (GLOBAL_SIS (FuelOnBoard)
		    + (FUEL_TANK_SCALE >> 1)) / FUEL_TANK_SCALE;
	    GLOBAL_SIS (FuelOnBoard) *= FUEL_TANK_SCALE;
	}

	SetContext (StatusContext);
	SetContextDrawState (DEST_PIXMAP | DRAW_REPLACE);
    }
    else if ((InputState & DEVICE_BUTTON2)
	    || ((InputState & DEVICE_BUTTON1)
	    && pMS->CurState == OUTFIT_EXIT))
    {
	if (pMS->CurState == OUTFIT_DOFUEL)
	{
	    pMS->CurState = OUTFIT_FUEL;
	    SetSemaphore (&GraphicsSem);
	    SetFlashRect ((PRECT)~0L, (FRAME)0);
	    ClearSemaphore (&GraphicsSem);
	}
	else
	{
ExitOutfit:
	    DestroyDrawable (ReleaseDrawable (pMS->CurFrame));
	    pMS->CurFrame = 0;
	    DestroyDrawable (ReleaseDrawable (pMS->ModuleFrame));
	    pMS->ModuleFrame = 0;

	    return (FALSE);
	}
    }
    else if (InputState & DEVICE_BUTTON1)
    {
	switch (pMS->CurState)
	{
	    case OUTFIT_FUEL:
	    {
		RECT	r;

		pMS->CurState = OUTFIT_DOFUEL;
		SetSemaphore (&GraphicsSem);
		SetContext (StatusContext);
		GetGaugeRect (&r, FALSE);
		SetFlashRect (&r, (FRAME)0);
		ClearSemaphore (&GraphicsSem);
		break;
	    }
	    case OUTFIT_DOFUEL:
		pMS->CurState = OUTFIT_FUEL;
		SetSemaphore (&GraphicsSem);
		SetFlashRect ((PRECT)~0L, (FRAME)0);
		ClearSemaphore (&GraphicsSem);
		break;
	    case OUTFIT_MODULES:
		pMS->CurState = EMPTY_SLOT + 2;
		if (GET_GAME_STATE (CHMMR_BOMB_STATE) != 3)
		    pMS->delta_item = 0;
		else
		    pMS->delta_item = NUM_BOMB_MODULES;
		pMS->first_item.y = 0;
		pMS->Initialized = 0;
		DoInstallModule (InputState, pMS);
		break;
	    case OUTFIT_SAVELOAD:
		if (GameOptions () == 0)
		    goto ExitOutfit;
		DrawMenuStateStrings (PM_FUEL, pMS->CurState);
		SetSemaphore (&GraphicsSem);
		SetFlashRect ((PRECT)~0L, (FRAME)0);
		ClearSemaphore (&GraphicsSem);
		break;
	}
    }
    else
    {
	BYTE	NewState;
	RECT	r;

	r.extent.height = 1;

	NewState = pMS->CurState;
	if (GetInputXComponent (InputState) < 0
		|| GetInputYComponent (InputState) < 0)
	{
	    if (NewState == OUTFIT_DOFUEL)
	    {
		SetSemaphore (&GraphicsSem);
		SetContext (SpaceContext);
		if (GetFTankCapacity (&r.corner) > GLOBAL_SIS (FuelOnBoard)
			&& GLOBAL_SIS (ResUnits) >=
			(DWORD)GLOBAL (FuelCost))
		{
		    if (GLOBAL_SIS (FuelOnBoard) >=
			    FUEL_RESERVE - FUEL_TANK_SCALE)
		    {
			r.extent.width = 3;
			DrawPoint (&r.corner);
			r.corner.x += r.extent.width + 1;
			DrawPoint (&r.corner);
			r.corner.x -= r.extent.width;
			SetContextForeGroundColor (SetContextBackGroundColor (BLACK_COLOR));
			DrawFilledRectangle (&r);
		    }
		    DeltaSISGauges (0, FUEL_TANK_SCALE, -GLOBAL (FuelCost));
		    SetContext (StatusContext);
		    GetGaugeRect (&r, FALSE);
		    SetFlashRect (&r, (FRAME)0);
		}
		ClearSemaphore (&GraphicsSem);
	    }
	    else if (NewState-- == OUTFIT_FUEL)
		NewState = OUTFIT_EXIT;
	}
	else if (GetInputXComponent (InputState) > 0
		|| GetInputYComponent (InputState) > 0)
	{
	    if (NewState == OUTFIT_DOFUEL)
	    {
		SetSemaphore (&GraphicsSem);
		SetContext (SpaceContext);
		if (GLOBAL_SIS (FuelOnBoard))
		{
		    DeltaSISGauges (0, -FUEL_TANK_SCALE,
			    GLOBAL (FuelCost));
		    if (GLOBAL_SIS (FuelOnBoard)
			    % FUEL_VOLUME_PER_ROW == 0)
		    {
			GetFTankCapacity (&r.corner);
			SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0xB, 0x00, 0x00), 0x2E));
			r.extent.width = 5;
			DrawFilledRectangle (&r);
		    }
		}
		SetContext (StatusContext);
		GetGaugeRect (&r, FALSE);
		SetFlashRect (&r, (FRAME)0);
		ClearSemaphore (&GraphicsSem);
	    }
	    else if (NewState++ == OUTFIT_EXIT)
		NewState = OUTFIT_FUEL;
	}

	if (NewState != pMS->CurState)
	{
	    DrawMenuStateStrings (PM_FUEL, NewState);
	    pMS->CurState = NewState;
	}
    }

    return (TRUE);
}

