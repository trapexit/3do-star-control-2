#include "starcon.h"
#include "melee.h"

#define NUM_MELEE_COLS_ORIG 6

PROC(
HSTARSHIP GetMeleeStarShip, (LastStarShipPtr, which_player),
    ARG		(STARSHIPPTR	LastStarShipPtr)
    ARG_END	(COUNT		which_player)
)
{
#define PICK_X_OFFS	57
#define PICK_Y_OFFS	24
#define PICK_SIDE_OFFS	100
    COUNT	ships_left, row, col;
    DWORD	NewTime, OldTime, LastTime;
    INPUT_STATE	OldInputState;
    HSTARSHIP	hBattleShip, hNextShip;
    STARSHIPPTR	StarShipPtr;
    RECT	flash_rect;
    TEXT	t;
    char	buf[10];
    STAMP	s;
    CONTEXT	OldContext;

    if (!(GLOBAL (CurrentActivity) & IN_BATTLE))
	return (0);

    s.frame = SetAbsFrameIndex (PickMeleeFrame, which_player);

    OldContext = SetContext (OffScreenContext);
    SetContextFGFrame (s.frame);
    if (LastStarShipPtr == 0 || LastStarShipPtr->special_counter == 0)
    {
	COUNT	cur_bucks;

	cur_bucks = 0;
	for (hBattleShip = GetHeadLink (&race_q[which_player]);
		hBattleShip != 0; hBattleShip = hNextShip)
	{
	    StarShipPtr = LockStarShip (&race_q[which_player], hBattleShip);
	    if (StarShipPtr == LastStarShipPtr)
	    {
		extern FRAME	status;

		LastStarShipPtr->RaceResIndex = 0;

		col = LastStarShipPtr->ShipFacing;
		s.origin.x = 3
			+ ((ICON_WIDTH + 2) * (col % NUM_MELEE_COLS_ORIG));
		s.origin.y = 9
			+ ((ICON_HEIGHT + 2) * (col / NUM_MELEE_COLS_ORIG));
		s.frame = SetAbsFrameIndex (status, 3);
		DrawStamp (&s);
		s.frame = SetAbsFrameIndex (PickMeleeFrame, which_player);
	    }
	    else if (StarShipPtr->RaceResIndex)
	    {
		cur_bucks += StarShipPtr->special_counter;
	    }
	    hNextShip = _GetSuccLink (StarShipPtr);
	    UnlockStarShip (&race_q[which_player], hBattleShip);
	}

	GetFrameRect (s.frame, &flash_rect);
	flash_rect.extent.width -= 4;
	t.baseline.x = flash_rect.extent.width;
	flash_rect.corner.x = flash_rect.extent.width - (6 * 3);
	flash_rect.corner.y = 2;
	flash_rect.extent.width = (6 * 3);
	flash_rect.extent.height = 7 - 2;
	SetContextForeGroundColor (PICK_BG_COLOR);
	DrawFilledRectangle (&flash_rect);

	sprintf (buf, "%d", cur_bucks);
	t.baseline.y = 7;
	t.align = ALIGN_RIGHT;
	t.lpStr = (LPBYTE)buf;
	t.CharCount = (COUNT)~0;
	SetContextFont (TinyFont);
	SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x13, 0x00, 0x00), 0x2C));
	DrawText (&t);
    }

    SetContext (SpaceContext);

    s.origin.x = PICK_X_OFFS - 3;
    s.origin.y = PICK_Y_OFFS - 9
	    + ((1 - which_player) * PICK_SIDE_OFFS);

    DrawStamp (&s);

    if (LOBYTE (battle_counter) == 0
	    || HIBYTE (battle_counter) == 0)
    {
	DWORD		TimeOut;
	INPUT_STATE	PressState, ButtonState;

	s.origin.y = PICK_Y_OFFS - 9
		+ (which_player * PICK_SIDE_OFFS);
	s.frame = SetAbsFrameIndex (PickMeleeFrame, 1 - which_player);
	DrawStamp (&s);

	TimeOut = GetTimeCounter () + (ONE_SECOND * 4);
	ClearSemaphore (&GraphicsSem);

	PressState = AnyButtonPress (TRUE);
	do
	{
	    ButtonState = AnyButtonPress (TRUE);
	    if (PressState)
	    {
		PressState = ButtonState;
		ButtonState = 0;
	    }
	} while (!ButtonState
		&& (!(PlayerControl[0] & PlayerControl[1] & PSYTRON_CONTROL)
		|| TaskSwitch () < TimeOut));

	if (ButtonState)
	    ButtonState = GetInputState (NormalInput);
	if (ButtonState & DEVICE_EXIT)
	    ConfirmExit ();

	SetSemaphore (&GraphicsSem);
	SetContext (OldContext);

	return (0);
    }

    if (LastStarShipPtr == 0 && which_player)
    {
	BYTE	fade_buf[] = {FadeAllToColor};
			
	SleepTask (XFormColorMap (
		(COLORMAPPTR)fade_buf, ONE_SECOND / 2
		) + 2);
	FlushColorXForms ();
    }

    row = 0;
    col = NUM_MELEE_COLS_ORIG;
    if (which_player == 0)
	ships_left = LOBYTE (battle_counter);
    else
	ships_left = HIBYTE (battle_counter);

    flash_rect.extent.width = (ICON_WIDTH + 2);
    flash_rect.extent.height = (ICON_HEIGHT + 2);

    NewTime = OldTime = LastTime = GetTimeCounter ();
    OldInputState = 0;
    goto ChangeSelection;
    for (;;)
    {
	INPUT_STATE	InputState;

	NewTime = SleepTask (GetTimeCounter () + 1);
	
	InputState = GetInputState (PlayerInput[which_player]);
	if (InputState)
	    LastTime = NewTime;
	else if (!(PlayerControl[1 - which_player] & PSYTRON_CONTROL)
		&& NewTime - LastTime >= ONE_SECOND * 3)
	    InputState = GetInputState (PlayerInput[1 - which_player]);
	if (InputState & DEVICE_EXIT)
	{
	    if (ConfirmExit ())
	    {
		hBattleShip = 0;
		break;
	    }
	    continue;
	}

	if (InputState == OldInputState
		&& NewTime - OldTime < (DWORD)MENU_REPEAT_DELAY)
	    InputState = 0;
	else
	{
	    OldInputState = InputState;
	    OldTime = NewTime;
	}

	if (InputState & DEVICE_BUTTON1)
	{
	    if (hBattleShip || (col == NUM_MELEE_COLS_ORIG && ConfirmExit ()))
	    {
		GLOBAL (CurrentActivity) &= ~CHECK_ABORT;
		break;
	    }
	}
	else
	{
	    COUNT	new_row, new_col;

	    new_row = row;
	    new_col = col;
	    if (GetInputXComponent (InputState) < 0)
	    {
		if (new_col-- == 0)
		    new_col = NUM_MELEE_COLS_ORIG;
	    }
	    else if (GetInputXComponent (InputState) > 0)
	    {
		if (new_col++ == NUM_MELEE_COLS_ORIG)
		    new_col = 0;
	    }
	    if (GetInputYComponent (InputState) < 0)
	    {
		if (new_row-- == 0)
		    new_row = NUM_MELEE_ROWS - 1;
	    }
	    else if (GetInputYComponent (InputState) > 0)
	    {
		if (++new_row == NUM_MELEE_ROWS)
		    new_row = 0;
	    }

	    if (new_row != row || new_col != col)
	    {
		COUNT	ship_index;

		row = new_row;
		col = new_col;

		PlaySoundEffect (MenuSounds, 0, 0);
		SetSemaphore (&GraphicsSem);
ChangeSelection:
		flash_rect.corner.x = PICK_X_OFFS
			+ ((ICON_WIDTH + 2) * col);
		flash_rect.corner.y = PICK_Y_OFFS
			+ ((ICON_HEIGHT + 2) * row)
			+ ((1 - which_player) * PICK_SIDE_OFFS);
		SetFlashRect (&flash_rect, (FRAME)0);

		hBattleShip = GetHeadLink (&race_q[which_player]);
		if (col == NUM_MELEE_COLS_ORIG)
		{
		    if (row)
			hBattleShip = 0;
		    else
		    {
			ship_index = (COUNT)random () % ships_left;
			for (hBattleShip = GetHeadLink (&race_q[which_player]);
				hBattleShip != 0; hBattleShip = hNextShip)
			{
			    StarShipPtr = LockStarShip (&race_q[which_player], hBattleShip);
			    if (StarShipPtr->RaceResIndex && ship_index-- == 0)
			    {
				UnlockStarShip (&race_q[which_player], hBattleShip);
				break;
			    }
			    hNextShip = _GetSuccLink (StarShipPtr);
			    UnlockStarShip (&race_q[which_player], hBattleShip);
			}
		    }
		}
		else
		{
		    ship_index = (row * NUM_MELEE_COLS_ORIG) + col;
		    for (hBattleShip = GetHeadLink (&race_q[which_player]);
			    hBattleShip != 0; hBattleShip = hNextShip)
		    {
			StarShipPtr = LockStarShip (&race_q[which_player], hBattleShip);
			if (StarShipPtr->ShipFacing == ship_index)
			{
			    hNextShip = hBattleShip;
			    if (StarShipPtr->RaceResIndex == 0)
				hBattleShip = 0;
			    UnlockStarShip (&race_q[which_player], hNextShip);
			    break;
			}
			hNextShip = _GetSuccLink (StarShipPtr);
			UnlockStarShip (&race_q[which_player], hBattleShip);
		    }
		}
		ClearSemaphore (&GraphicsSem);
	    }
	}
    }

    SetSemaphore (&GraphicsSem);
    SetFlashRect (NULL_PTR, (FRAME)0);
    SetContext (OldContext);
    
    if (hBattleShip == 0)
	GLOBAL (CurrentActivity) &= ~IN_BATTLE;
    else
    {
	StarShipPtr = LockStarShip (&race_q[which_player], hBattleShip);
	OwnStarShip (StarShipPtr,
		1 << which_player, StarShipPtr->captains_name_index);
	StarShipPtr->captains_name_index = 0;
	UnlockStarShip (&race_q[which_player], hBattleShip);

	PlaySoundEffect (SetAbsSoundIndex (MenuSounds, 1), 0, 0);

	while (ChannelPlaying (0))
	    ;
    }

    return (hBattleShip);
}


