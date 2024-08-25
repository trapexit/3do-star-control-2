#include "starcon.h"

PROC(STATIC
void far flash_ship_task, (),
    ARG_VOID
)
{
    DWORD	TimeIn;
    COLOR	c;

    c = BUILD_COLOR (MAKE_RGB15 (0x1F, 0x19, 0x19), 0x24);
    TimeIn = GetTimeCounter ();
    for (;;)
    {
	STAMP			s;
	SHIP_FRAGMENTPTR	StarShipPtr;
	COLOR			OldColor;
	CONTEXT			OldContext;

	SetSemaphore (&GraphicsSem);
	s.origin = pMenuState->first_item;
	StarShipPtr = (SHIP_FRAGMENTPTR)LockStarShip (
		&GLOBAL (built_ship_q),
		(HSTARSHIP)pMenuState->CurFrame
		);
	s.frame = StarShipPtr->ShipInfo.icons;
	UnlockStarShip (
		&GLOBAL (built_ship_q),
		(HSTARSHIP)pMenuState->CurFrame
		);
	OldContext = SetContext (StatusContext);
	if (c == BUILD_COLOR (MAKE_RGB15 (0x1F, 0x19, 0x19), 0x24))
	    c = BUILD_COLOR (MAKE_RGB15 (0xB, 0x00, 0x00), 0x2E);
	else
	    c -= BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x14), 0x01);
	OldColor = SetContextForeGroundColor (c);
	DrawFilledStamp (&s);
	SetContextForeGroundColor (OldColor);
	SetContext (OldContext);
	ClearSemaphore (&GraphicsSem);
	TimeIn = SleepTask (TimeIn + 8);
    }
}

PROC(STATIC
HSTARSHIP near MatchSupportShip, (pMS),
    ARG_END	(PMENU_STATE	pMS)
)
{
    PPOINT	pship_pos;
    HSTARSHIP	hStarShip, hNextShip;

    for (hStarShip = GetHeadLink (&GLOBAL (built_ship_q)),
	    pship_pos = (PPOINT)pMS->flash_frame0;
	    hStarShip; hStarShip = hNextShip, ++pship_pos)
    {
	SHIP_FRAGMENTPTR	StarShipPtr;

	StarShipPtr = (SHIP_FRAGMENTPTR)LockStarShip (
		&GLOBAL (built_ship_q),
		hStarShip
		);

	if (pship_pos->x == pMS->first_item.x
		&& pship_pos->y == pMS->first_item.y)
	{
	    UnlockStarShip (
		    &GLOBAL (built_ship_q),
		    hStarShip
		    );

	    return (hStarShip);
	}

	hNextShip = _GetSuccLink (StarShipPtr);
	UnlockStarShip (
		&GLOBAL (built_ship_q),
		hStarShip
		);
    }

    return (0);
}

PROC(STATIC
void near DeltaSupportCrew, (crew_delta),
    ARG_END	(SIZE	crew_delta)
)
{
    char		buf[10];
    HSTARSHIP		hTemplate;
    SHIP_FRAGMENTPTR	StarShipPtr, TemplatePtr;

    StarShipPtr = (SHIP_FRAGMENTPTR)LockStarShip (
	    &GLOBAL (built_ship_q),
	    (HSTARSHIP)pMenuState->CurFrame
	    );
    hTemplate = GetStarShipFromIndex (
	    &GLOBAL (avail_race_q), 
	    GET_RACE_ID (StarShipPtr)
	    );
    TemplatePtr = (SHIP_FRAGMENTPTR)LockStarShip (
	    &GLOBAL (avail_race_q), hTemplate
	    );

    StarShipPtr->ShipInfo.crew_level += crew_delta;

    if (StarShipPtr->ShipInfo.crew_level == 0)
	StarShipPtr->ShipInfo.crew_level = 1;
    else if (StarShipPtr->ShipInfo.crew_level >
	    TemplatePtr->RaceDescPtr->ship_info.crew_level)
	StarShipPtr->ShipInfo.crew_level =
		TemplatePtr->RaceDescPtr->ship_info.crew_level;
    else
    {
	if (StarShipPtr->ShipInfo.crew_level ==
		TemplatePtr->RaceDescPtr->ship_info.crew_level)
	    sprintf (buf, "%u", StarShipPtr->ShipInfo.crew_level);
	else
	    sprintf (buf, "%u/%u",
		    StarShipPtr->ShipInfo.crew_level,
		    TemplatePtr->RaceDescPtr->ship_info.crew_level);

	DrawStatusMessage ((LPSTR)buf);
	DeltaSISGauges (-crew_delta, 0, 0);
	if (crew_delta)
	{
	    RECT	r;

	    r.corner.x = 2;
	    r.corner.y = 130;
	    r.extent.width = STATUS_MESSAGE_WIDTH;
	    r.extent.height = STATUS_MESSAGE_HEIGHT;
	    SetContext (StatusContext);
	    SetFlashRect (&r, (FRAME)0);
	}
    }

    UnlockStarShip (
	    &GLOBAL (avail_race_q), hTemplate
	    );
    UnlockStarShip (
	    &GLOBAL (built_ship_q),
	    (HSTARSHIP)pMenuState->CurFrame
	    );
}

#define SHIP_TOGGLE	((BYTE)(1 << 7))

PROC(STATIC
void RosterCleanup, (pMS),
    ARG_END	(PMENU_STATE	pMS)
)
{
    if (pMS->flash_task)
    {
	DeleteTask (pMS->flash_task);
	pMS->flash_task = 0;
    }

    if (pMS->CurFrame)
    {
	STAMP			s;
	SHIP_FRAGMENTPTR	StarShipPtr;

	SetContext (StatusContext);
	s.origin = pMS->first_item;
	StarShipPtr = (SHIP_FRAGMENTPTR)LockStarShip (
		&GLOBAL (built_ship_q),
		(HSTARSHIP)pMS->CurFrame
		);
	s.frame = StarShipPtr->ShipInfo.icons;
	UnlockStarShip (
		&GLOBAL (built_ship_q),
		(HSTARSHIP)pMS->CurFrame
		);
	if (!(pMS->CurState & SHIP_TOGGLE))
	    DrawStamp (&s);
	else
	{
	    SetContextForeGroundColor (WHITE_COLOR);
	    DrawFilledStamp (&s);
	}
    }
}

PROC(STATIC
BOOLEAN	DoModifyRoster, (InputState, pMS),
    ARG		(INPUT_STATE	InputState)
    ARG_END	(PMENU_STATE	pMS)
)
{
    BYTE		NewState;
    SBYTE		sx, sy;
    RECT		r;
    STAMP		s;
    SHIP_FRAGMENTPTR	StarShipPtr;

    if (GLOBAL (CurrentActivity) & CHECK_ABORT)
    {
	pMS->CurFrame = 0;
	SetSemaphore (&GraphicsSem);
	RosterCleanup (pMS);
	ClearSemaphore (&GraphicsSem);

	return (FALSE);
    }

    if (!pMS->Initialized)
    {
	pMS->InputFunc = DoModifyRoster;
	pMS->Initialized = TRUE;

	pMS->CurState = NewState = 0;
	SetSemaphore (&GraphicsSem);
	SetContext (StatusContext);
	goto SelectSupport;
    }
    else if ((InputState & DEVICE_BUTTON2)
	    && !(pMS->CurState & SHIP_TOGGLE))
    {
	SetSemaphore (&GraphicsSem);
	SetFlashRect (NULL_PTR, (FRAME)0);
	RosterCleanup (pMS);
	pMS->CurFrame = 0;
	DrawStatusMessage (NULL_PTR);
	ClearSemaphore (&GraphicsSem);

	return (FALSE);
    }
    else if (InputState & (DEVICE_BUTTON1 | DEVICE_BUTTON2))
    {
	SetSemaphore (&GraphicsSem);
	pMS->CurState ^= SHIP_TOGGLE;
	if (!(pMS->CurState & SHIP_TOGGLE))
	    SetFlashRect (NULL_PTR, (FRAME)0);
	else
	{
	    RosterCleanup (pMS);

	    r.corner.x = 2;
	    r.corner.y = 130;
	    r.extent.width = STATUS_MESSAGE_WIDTH;
	    r.extent.height = STATUS_MESSAGE_HEIGHT;
	    SetContext (StatusContext);
	    SetFlashRect (&r, (FRAME)0);
	}
	ClearSemaphore (&GraphicsSem);
    }
    else if (pMS->CurState & SHIP_TOGGLE)
    {
	if ((sy = GetInputYComponent (InputState)) < 0)
	{
	    if (GLOBAL_SIS (CrewEnlisted))
	    {
		SetSemaphore (&GraphicsSem);
		DeltaSupportCrew (1);
		ClearSemaphore (&GraphicsSem);
	    }
	}
	else if (sy > 0)
	{
	    if (GLOBAL_SIS (CrewEnlisted)
		    < GetCPodCapacity (NULL_PTR))
	    {
		SetSemaphore (&GraphicsSem);
		DeltaSupportCrew (-1);
		ClearSemaphore (&GraphicsSem);
	    }
	}
    }
    else
    {
	PPOINT	pship_pos;

	NewState = pMS->CurState;
	sx = (SBYTE)((pMS->delta_item + 1) >> 1);
	if (GetInputXComponent (InputState))
	{
	    pship_pos = (PPOINT)pMS->flash_frame1;
	    if (NewState == (BYTE)(sx - 1))
		NewState = (BYTE)(pMS->delta_item - 1);
	    else if (NewState >= (BYTE)sx)
	    {
		NewState -= sx;
		if (pship_pos[NewState].y < pship_pos[pMS->CurState].y)
		    ++NewState;
	    }
	    else
	    {
		NewState += sx;
		if (NewState != (BYTE)sx
			&& pship_pos[NewState].y > pship_pos[pMS->CurState].y)
		    --NewState;
	    }
	}
	else if ((sy = GetInputYComponent (InputState)) > 0)
	{
	    if (++NewState == (BYTE)pMS->delta_item)
		NewState = (BYTE)(sx - 1);
	    else if (NewState == (BYTE)sx)
		NewState = 0;
	}
	else if (sy < 0)
	{
	    if (NewState == 0)
		NewState += sx - 1;
	    else if (NewState == (BYTE)sx)
		NewState = (BYTE)(pMS->delta_item - 1);
	    else
		--NewState;
	}

	if (NewState != pMS->CurState)
	{
	    SetSemaphore (&GraphicsSem);
	    SetContext (StatusContext);
	    s.origin = pMS->first_item;
	    StarShipPtr = (SHIP_FRAGMENTPTR)LockStarShip (
		    &GLOBAL (built_ship_q),
		    (HSTARSHIP)pMS->CurFrame
		    );
	    s.frame = StarShipPtr->ShipInfo.icons;
	    UnlockStarShip (
		    &GLOBAL (built_ship_q),
		    (HSTARSHIP)pMS->CurFrame
		    );
	    DrawStamp (&s);
SelectSupport:
	    pship_pos = (PPOINT)pMS->flash_frame1;
	    pMS->first_item = pship_pos[NewState];
	    pMS->CurFrame = (FRAME)MatchSupportShip (pMS);

	    DeltaSupportCrew (0);
	    ClearSemaphore (&GraphicsSem);

	    pMS->CurState = NewState;
	}

	if (pMS->flash_task == 0)
	    pMS->flash_task = AddTask (flash_ship_task, 2048);
    }

    return (TRUE);
}

PROC(
BOOLEAN Roster, (),
    ARG_VOID
)
{
    COUNT	num_support_ships;

    if (num_support_ships = CountLinks (&GLOBAL (built_ship_q)))
    {
	SIZE	i, j, k, l;
	POINT	modified_ship_pos[MAX_COMBAT_SHIPS];
	POINT	ship_pos[MAX_COMBAT_SHIPS] =
	{
	    SUPPORT_SHIP_PTS
	};
	MENU_STATE	MenuState;
	PMENU_STATE	pOldMenuState;

	pOldMenuState = pMenuState;
	pMenuState = &MenuState;

	j = 0;
	k = (num_support_ships + 1) >> 1;
	for (i = 0; i < num_support_ships; i += 2)
	{
	    modified_ship_pos[j++] = ship_pos[i];
	    modified_ship_pos[k++] = ship_pos[i + 1];
	}

	k = (num_support_ships + 1) >> 1;
	for (i = 0; i < k; ++i)
	{
	    for (j = k - 1; j > i; --j)
	    {
		if (modified_ship_pos[i].y > modified_ship_pos[j].y)
		{
		    POINT	temp;

		    temp = modified_ship_pos[i];
		    modified_ship_pos[i] = modified_ship_pos[j];
		    modified_ship_pos[j] = temp;
		}
	    }
	}

	l = k;
	k = num_support_ships >> 1;
	for (i = 0; i < k; ++i)
	{
	    for (j = k - 1; j > i; --j)
	    {
		if (modified_ship_pos[i + l].y > modified_ship_pos[j + l].y)
		{
		    POINT	temp;

		    temp = modified_ship_pos[i + l];
		    modified_ship_pos[i + l] = modified_ship_pos[j + l];
		    modified_ship_pos[j + l] = temp;
		}
	    }
	}

	MenuState.InputFunc = DoModifyRoster;
	MenuState.Initialized = FALSE;
	MenuState.CurState = 0;
	MenuState.flash_task = 0;
	MenuState.delta_item = (SIZE)num_support_ships;
	
	MenuState.flash_frame0 = (FRAME)ship_pos;
	MenuState.flash_frame1 = (FRAME)modified_ship_pos;
	DoInput ((PVOID)&MenuState);

	pMenuState = pOldMenuState;
	
	return (TRUE);
    }
    
    return (FALSE);
}

