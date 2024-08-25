#include "starcon.h"

QUEUE	disp_q;
SIZE	battle_counter;

PROC(STATIC
void ProcessInput, (),
    ARG_VOID
)
{
    BOOLEAN	CanRunAway;

    CanRunAway = (BOOLEAN)(
	    (LOBYTE (GLOBAL (CurrentActivity)) == IN_ENCOUNTER
	    || LOBYTE (GLOBAL (CurrentActivity)) == IN_LAST_BATTLE)
	    && GET_GAME_STATE (STARBASE_AVAILABLE)
	    && !GET_GAME_STATE (BOMB_CARRIER)
	    );
    for (cur_player = NUM_SIDES - 1; cur_player >= 0; --cur_player)
    {
	HSTARSHIP	hBattleShip, hNextShip;

	for (hBattleShip = GetHeadLink (&race_q[cur_player]);
		hBattleShip != 0; hBattleShip = hNextShip)
	{
	    INPUT_STATE	InputState;
	    STARSHIPPTR	StarShipPtr;

	    StarShipPtr = LockStarShip (&race_q[cur_player], hBattleShip);
	    hNextShip = _GetSuccLink (StarShipPtr);

	    if (StarShipPtr->hShip)
	    {
		CyborgDescPtr = StarShipPtr;

		InputState = GetInputState (PlayerInput[cur_player]);
#if CREATE_JOURNAL
		JournalInput (InputState);
#endif /* CREATE_JOURNAL */
		if (InputState & DEVICE_EXIT)
		    InputState = ConfirmExit ();

#ifdef TESTING
if ((InputState & DEVICE_BUTTON3)
	&& LOBYTE (GLOBAL (CurrentActivity)) == IN_HYPERSPACE)
{
    COUNT	i;
    
    for (i = ARILOU_SHIP; i <= BLACK_URQUAN_SHIP; ++i)
	ActivateStarShip (i, SPHERE_TRACKING);
    while (GLOBAL (GameClock.tick_count) > 1)
	ClockTick ();
}
#endif /* TESTING */

		CyborgDescPtr->ship_input_state = 0;
		if (CyborgDescPtr->RaceDescPtr->ship_info.crew_level)
		{
		    if (GetInputXComponent (InputState) < 0)
			CyborgDescPtr->ship_input_state |= LEFT;
		    else if (GetInputXComponent (InputState) > 0)
			CyborgDescPtr->ship_input_state |= RIGHT;
		    if (GetInputYComponent (InputState) < 0
			    || (InputState & DEVICE_BUTTON1))
			CyborgDescPtr->ship_input_state |= THRUST;
		    if (InputState & (DEVICE_BUTTON2 | DEVICE_RIGHTSHIFT))
			CyborgDescPtr->ship_input_state |= WEAPON;
		    if (InputState & (DEVICE_BUTTON3 | DEVICE_LEFTSHIFT))
			CyborgDescPtr->ship_input_state |= SPECIAL;

		    if (CanRunAway
			    && cur_player == 0
			    && ((PlayerControl[cur_player] & HUMAN_CONTROL)
			    || (InputState = GetInputState (NormalInput)))
			    && (InputState & (DEVICE_BUTTON2 | DEVICE_RIGHTSHIFT))
			    == (DEVICE_BUTTON2 | DEVICE_RIGHTSHIFT))
		    {
			ELEMENTPTR	ElementPtr;

			LockElement (StarShipPtr->hShip, &ElementPtr);
			if (GetPrimType (&DisplayArray[ElementPtr->PrimIndex]) == STAMP_PRIM
				&& ElementPtr->life_span == NORMAL_LIFE
				&& !(ElementPtr->state_flags & FINITE_LIFE)
				&& ElementPtr->mass_points != MAX_SHIP_MASS * 10)
			{
			    PROC_GLOBAL(
			    void flee_preprocess, (ElementPtr),
				ARG_END	(LPELEMENT	ElementPtr)
			    );

			    battle_counter -= MAKE_WORD (1, 0);

			    ElementPtr->turn_wait = 3;
			    ElementPtr->thrust_wait = MAKE_BYTE (4, 0);
			    ElementPtr->preprocess_func = flee_preprocess;
			    ElementPtr->mass_points = MAX_SHIP_MASS * 10;
			    ZeroVelocityComponents (&ElementPtr->velocity);
			    StarShipPtr->cur_status_flags &=
				    ~(SHIP_AT_MAX_SPEED | SHIP_BEYOND_MAX_SPEED);

			    SetPrimColor (&DisplayArray[ElementPtr->PrimIndex],
				    BUILD_COLOR (MAKE_RGB15 (0xB, 0x00, 0x00), 0x2E));
			    SetPrimType (&DisplayArray[ElementPtr->PrimIndex], STAMPFILL_PRIM);
			
			    CyborgDescPtr->ship_input_state = 0;
			}
			UnlockElement (StarShipPtr->hShip);
		    }
		}
	    }

	    UnlockStarShip (&race_q[cur_player], hBattleShip);
	}
    }

    if (GLOBAL (CurrentActivity) & (CHECK_LOAD | CHECK_ABORT))
	GLOBAL (CurrentActivity) &= ~IN_BATTLE;
}

#if DEMO_MODE || CREATE_JOURNAL
DWORD	BattleSeed;
#endif /* DEMO_MODE */

static MUSIC_REF	BattleRef;

PROC(
void BattleSong, (DoPlay),
    ARG_END	(BOOLEAN	DoPlay)
)
{
    if (BattleRef == 0)
    {
	if (LOBYTE (GLOBAL (CurrentActivity)) != IN_HYPERSPACE)
	    BattleRef = LoadMusicInstance (BATTLE_MUSIC);
	else if (GET_GAME_STATE (ARILOU_SPACE_SIDE) <= 1)
	    BattleRef = LoadMusicInstance (HYPERSPACE_MUSIC);
	else
	    BattleRef = LoadMusicInstance (QUASISPACE_MUSIC);
    }

    if (DoPlay)
	PlayMusic (BattleRef, TRUE, 1);
}

PROC(
void FreeBattleSong, (),
    ARG_VOID
)
{
    DestroyMusic (BattleRef);
    BattleRef = 0;
}

PROC(
BOOLEAN Battle, (),
    ARG_VOID
)
{
    SIZE	num_ships;
    PROC_GLOBAL(
    SIZE InitShips, (),
	ARG_VOID
    );
    PROC(
    void UninitShips, (),
	ARG_VOID
    );

    SetSemaphore (&GraphicsSem);

    SetResourceIndex (hResIndex);

#if !(DEMO_MODE || CREATE_JOURNAL)
    seed_random (GetTimeCounter ());
#else /* DEMO_MODE */
    if (BattleSeed == 0)
	BattleSeed = random ();
    seed_random (BattleSeed);
    BattleSeed = random ();	/* get next battle seed */
#endif /* DEMO_MODE */

    BattleSong (FALSE);
    
    num_ships = InitShips ();
    if (num_ships)
    {
	DWORD	NextTime;
BOOLEAN	first_time;

	GLOBAL (CurrentActivity) |= IN_BATTLE;
	battle_counter = MAKE_WORD (
		CountLinks (&race_q[0]),
		CountLinks (&race_q[1])
		);

	while (num_ships--)
	{
	    if (!GetNextStarShip (NULL_PTR, num_ships))
		goto AbortBattle;
	}

	BattleSong (TRUE);
	NextTime = 0;
first_time = (BOOLEAN)(LOBYTE (GLOBAL (CurrentActivity)) == IN_HYPERSPACE);
	do
	{
	    extern UWORD	nth_frame;

BatchGraphics ();
ScreenOrigin (Screen, 0, 0);
	    if (LOBYTE (GLOBAL (CurrentActivity)) == IN_HYPERSPACE)
		SeedUniverse ();
	    RedrawQueue (TRUE);
if (first_time)
{
    RECT	r;
    
    first_time = FALSE;
    r.corner.x = SIS_ORG_X;
    r.corner.y = SIS_ORG_Y;
    r.extent.width = SIS_SCREEN_WIDTH;
    r.extent.height = SIS_SCREEN_HEIGHT;
    ScreenTransition (3, &r);
}
UnbatchGraphics ();
	    ClearSemaphore (&GraphicsSem);
	    if (nth_frame)
		TaskSwitch ();
	    else
		NextTime = SleepTask (NextTime + BATTLE_FRAME_RATE);
	    ProcessInput ();
	    SetSemaphore (&GraphicsSem);
	} while (GLOBAL (CurrentActivity) & IN_BATTLE);

AbortBattle:
	StopMusic ();
	StopSound ();
    }

    UninitShips ();
    FreeBattleSong ();

    ClearSemaphore (&GraphicsSem);
    
    return (num_ships < 0);
}

