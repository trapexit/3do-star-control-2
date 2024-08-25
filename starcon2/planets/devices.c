#include "starcon.h"

PROC(STATIC
COUNT near lpstrchr, (lpStr, ch),
    ARG		(LPSTR	lpStr)
    ARG_END	(char	ch)
)
{
    COUNT	skip_chars;

    skip_chars = 0;
    while (*lpStr++ != ch)
	++skip_chars;

    return (skip_chars);
}

PROC(STATIC
void near DrawDevices, (pMS, OldDevice, NewDevice),
    ARG		(PMENU_STATE	pMS)
    ARG		(BYTE		OldDevice)
    ARG_END	(BYTE		NewDevice)
)
{
#define MAX_VIS_DEVICES	5
#define DEVICE_BASE	133
    COORD	y, cy;
    TEXT	t;
    RECT	r;
    PBYTE	pDeviceMap;
    extern PSTR	far *ClusterNames;

    SetSemaphore (&GraphicsSem);

    SetContext (StatusContext);
    SetContextFont (TinyFont);

    y = 41;
    t.baseline.x = 40;
    t.align = ALIGN_CENTER;
    t.CharCount = 3;
    SET_SEGMENT (&t.lpStr, GET_SEGMENT (&ClusterNames));

    pDeviceMap = (PBYTE)pMS->CurFrame;
    if (OldDevice > NUM_DEVICES
	    || (NewDevice < NUM_DEVICES
	    && (NewDevice < (BYTE)pMS->first_item.y
	    || NewDevice >= (BYTE)(pMS->first_item.y + MAX_VIS_DEVICES))))
    {
	STAMP		s;
	extern FRAME	misc_data;

	r.corner.x = 2;
	r.extent.width = FIELD_WIDTH + 1;

	if (!(pMS->Initialized & 1))
	{
	    ++r.corner.x;
	    r.extent.width -= 2;
	    r.corner.y = 33;
	    r.extent.height = 89;
	    SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x14), 0x01));
	    DrawFilledRectangle (&r);
	}
	else
	{
	    TEXT	ct;

	    r.corner.y = 20;
	    r.extent.height = 109;
	    DrawStarConBox (&r, 1,
		    BUILD_COLOR (MAKE_RGB15 (0x10, 0x10, 0x10), 0x19),
		    BUILD_COLOR (MAKE_RGB15 (0x8, 0x8, 0x8), 0x1F),
		    TRUE,
		    BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x14), 0x01));

	    SetContextFont (StarConFont);
	    ct.baseline.x = (STATUS_WIDTH >> 1) - 1;
	    ct.baseline.y = 27;
	    ct.align = ALIGN_CENTER;
	    ct.lpStr = (LPBYTE)"devices";
	    ct.CharCount = (COUNT)~0;
	    SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0xA, 0x1F, 0x1F), 0x0B));
	    DrawText (&ct);

	    SetContextFont (TinyFont);
	}

	if (NewDevice < (BYTE)pMS->first_item.y)
	    pMS->first_item.y = NewDevice;
	else if (NewDevice >= (BYTE)(pMS->first_item.y + MAX_VIS_DEVICES))
	    pMS->first_item.y = NewDevice - (MAX_VIS_DEVICES - 1);

	s.origin.x = 4;
	s.origin.y = 34;
	cy = y;

	SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x00, 0x14, 0x14), 0x03));
	for (OldDevice = (BYTE)pMS->first_item.y;
		OldDevice < (BYTE)(pMS->first_item.y + MAX_VIS_DEVICES)
		&& OldDevice < (BYTE)pMS->first_item.x;
		++OldDevice)
	{
	    s.frame = SetAbsFrameIndex (
		    misc_data, 77 + pDeviceMap[OldDevice]
		    );
	    DrawStamp (&s);

	    if (OldDevice != NewDevice)
	    {
		t.baseline.y = cy;
		SET_OFFSET ((LPSTR *)&t.lpStr, ClusterNames[
			pDeviceMap[OldDevice] + DEVICE_BASE]);
		t.CharCount = lpstrchr ((LPSTR)t.lpStr, ' ');
		DrawText (&t);
		t.baseline.y += 7;
		t.lpStr += t.CharCount + 1;
		t.CharCount = (COUNT)~0;
		DrawText (&t);
	    }

	    cy += 18;
	    s.origin.y += 18;
	}

	OldDevice = NewDevice;
    }

    r.extent.width = 41;
    r.extent.height = 14;
    r.corner.x = t.baseline.x - (r.extent.width >> 1);

    if (OldDevice != NewDevice)
    {
	cy = y + ((OldDevice - pMS->first_item.y) * 18);
	r.corner.y = cy - 6;
	SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x14), 0x01));
	DrawFilledRectangle (&r);

	SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x00, 0x14, 0x14), 0x03));
	t.baseline.y = cy;
	SET_OFFSET ((LPSTR *)&t.lpStr, ClusterNames[
		pDeviceMap[OldDevice] + DEVICE_BASE
		]);
	t.CharCount = lpstrchr ((LPSTR)t.lpStr, ' ');
	DrawText (&t);
	t.baseline.y += 7;
	t.lpStr += t.CharCount + 1;
	t.CharCount = (COUNT)~0;
	DrawText (&t);
    }

    if (NewDevice < NUM_DEVICES)
    {
	cy = y + ((NewDevice - pMS->first_item.y) * 18);
	r.corner.y = cy - 6;
	SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0xA, 0xA, 0x1F), 0x09));
	DrawFilledRectangle (&r);

	SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0xA, 0x1F, 0x1F), 0x0B));
	t.baseline.y = cy;
	SET_OFFSET ((LPSTR *)&t.lpStr, ClusterNames[
		pDeviceMap[NewDevice] + DEVICE_BASE
		]);
	t.CharCount = lpstrchr ((LPSTR)t.lpStr, ' ');
	DrawText (&t);
	t.baseline.y += 7;
	t.lpStr += t.CharCount + 1;
	t.CharCount = (COUNT)~0;
	DrawText (&t);
    }

    ClearSemaphore (&GraphicsSem);
}

PROC(STATIC
UWORD near DeviceFailed, (which_device),
    ARG_END	(BYTE	which_device)
)
{
    BYTE		val;
    extern ACTIVITY	NextActivity;
    PROC_GLOBAL(
    void SaveFlagshipState, (),
	ARG_VOID
    );

    switch (which_device)
    {
	case ROSY_SPHERE_DEVICE:
	    if (val = GET_GAME_STATE (ULTRON_CONDITION))
	    {
		SET_GAME_STATE (ULTRON_CONDITION, val + 1);
		SET_GAME_STATE (ROSY_SPHERE_ON_SHIP, 0);
		SET_GAME_STATE (DISCUSSED_ULTRON, 0);
		SET_GAME_STATE (SUPOX_ULTRON_HELP, 0);
		return (FALSE);
	    }
	    break;
	case ARTIFACT_2_DEVICE:
	    break;
	case ARTIFACT_3_DEVICE:
	    break;
	case SUN_EFFICIENCY_DEVICE:
	    if (LOBYTE (GLOBAL (CurrentActivity)) == IN_INTERPLANETARY
		    && pSolarSysState
		    && pSolarSysState->pOrbitalDesc
		    && pSolarSysState->MenuState.Initialized == 3)
	    {
		BYTE	fade_buf[1];

		PlaySoundEffect (SetAbsSoundIndex (MenuSounds, 3),
			0, GAME_SOUND_PRIORITY);
		fade_buf[0] = FadeAllToWhite;
		SleepTask (XFormColorMap ((COLORMAPPTR)fade_buf, ONE_SECOND * 1)
			+ (ONE_SECOND * 2));
		if (CurStarDescPtr->Index != CHMMR_DEFINED
			|| pSolarSysState->pOrbitalDesc !=
			&pSolarSysState->PlanetDesc[1])
		{
		    fade_buf[0] = FadeAllToColor;
		    XFormColorMap ((COLORMAPPTR)fade_buf, ONE_SECOND * 2);
		}
		else
		{
		    SET_GAME_STATE (CHMMR_EMERGING, 1);

		    battle_counter = 0;
		    GLOBAL (CurrentActivity) |= START_ENCOUNTER;

		    PutGroupInfo (0L, (BYTE)~0);
		    ReinitQueue (&GLOBAL (npc_built_ship_q));

		    CloneShipFragment (CHMMR_SHIP,
			    &GLOBAL (npc_built_ship_q), 0);
		}
		return (MAKE_WORD (0, 1));
	    }
	    break;
	case UTWIG_BOMB_DEVICE:
	    SET_GAME_STATE (UTWIG_BOMB, 0);
	    GLOBAL (CurrentActivity) &= ~IN_BATTLE;
	    GLOBAL_SIS (CrewEnlisted) = (COUNT)~0;
	    return (FALSE);
	case ULTRON_0_DEVICE:
	    break;
	case ULTRON_1_DEVICE:
	    break;
	case ULTRON_2_DEVICE:
	    break;
	case ULTRON_3_DEVICE:
	    break;
	case MAIDENS_DEVICE:
	    break;
	case TALKING_PET_DEVICE:
	    NextActivity |= CHECK_LOAD;	/* fake a load game */
	    GLOBAL (CurrentActivity) |= START_ENCOUNTER;
	    SET_GAME_STATE (GLOBAL_FLAGS_AND_DATA, 0);
	    if (LOBYTE (GLOBAL (CurrentActivity)) == IN_HYPERSPACE)
	    {
		if (GetHeadEncounter ())
		{
		    SET_GAME_STATE (SHIP_TO_COMPEL, 1);
		}
		GLOBAL (CurrentActivity) &= ~IN_BATTLE;
	    }
	    else
	    {
		battle_counter = 0;
		if (GetHeadLink (&GLOBAL (npc_built_ship_q)))
		{
		    SET_GAME_STATE (SHIP_TO_COMPEL, 1);

		    PutGroupInfo (0L, (BYTE)~0);
		    ReinitQueue (&GLOBAL (npc_built_ship_q));
		}

		if (CurStarDescPtr->Index == SAMATRA_DEFINED)
		{
		    SET_GAME_STATE (READY_TO_CONFUSE_URQUAN, 1);
		}
		if (pSolarSysState->MenuState.Initialized < 3)
		    return (FALSE);
	    }
	    SaveFlagshipState ();
	    return (FALSE);
	case AQUA_HELIX_DEVICE:
	    if (val = GET_GAME_STATE (ULTRON_CONDITION))
	    {
		SET_GAME_STATE (ULTRON_CONDITION, val + 1);
		SET_GAME_STATE (AQUA_HELIX_ON_SHIP, 0);
		SET_GAME_STATE (DISCUSSED_ULTRON, 0);
		SET_GAME_STATE (SUPOX_ULTRON_HELP, 0);
		return (FALSE);
	    }
	    break;
	case CLEAR_SPINDLE_DEVICE:
	    if (val = GET_GAME_STATE (ULTRON_CONDITION))
	    {
		SET_GAME_STATE (ULTRON_CONDITION, val + 1);
		SET_GAME_STATE (CLEAR_SPINDLE_ON_SHIP, 0);
		SET_GAME_STATE (DISCUSSED_ULTRON, 0);
		SET_GAME_STATE (SUPOX_ULTRON_HELP, 0);
		return (FALSE);
	    }
	    break;
	case UMGAH_HYPERWAVE_DEVICE:
	case BURVIX_HYPERWAVE_DEVICE:
	    if (LOBYTE (GLOBAL (CurrentActivity)) == IN_HYPERSPACE)
	    {
		if (GET_GAME_STATE (ARILOU_SPACE_SIDE) <= 1)
		{
		    SET_GAME_STATE (USED_BROADCASTER, 1);
		    return (FALSE);
		}
	    }
	    else if (LOBYTE (GLOBAL (CurrentActivity)) == IN_INTERPLANETARY
		    && pSolarSysState)
	    {
		if (pSolarSysState->pOrbitalDesc ==
			&pSolarSysState->PlanetDesc[1]
			&& pSolarSysState->MenuState.Initialized == 3
			&& CurStarDescPtr->Index == CHMMR_DEFINED
			&& !GET_GAME_STATE (CHMMR_UNLEASHED))
		{
		    NextActivity |= CHECK_LOAD;	/* fake a load game */
		    GLOBAL (CurrentActivity) |= START_ENCOUNTER;

		    battle_counter = 0;
		    PutGroupInfo (0L, (BYTE)~0);
		    ReinitQueue (&GLOBAL (npc_built_ship_q));

		    SET_GAME_STATE (GLOBAL_FLAGS_AND_DATA, 1 << 7);
		    SaveFlagshipState ();
		    return (FALSE);
		}
		else
		{
		    BOOLEAN	FoundIlwrath;
		    HSTARSHIP	hStarShip;

		    FoundIlwrath = (BOOLEAN)(
			    CurStarDescPtr->Index == ILWRATH_DEFINED
			    );
		    if (!FoundIlwrath && (hStarShip =
			    GetHeadLink (&GLOBAL (npc_built_ship_q))))
		    {
			SHIP_FRAGMENTPTR	FragPtr;

			FragPtr = (SHIP_FRAGMENTPTR)LockStarShip (
				&GLOBAL (npc_built_ship_q), hStarShip
				);
			FoundIlwrath = (BOOLEAN)(
				GET_RACE_ID (FragPtr) == ILWRATH_SHIP
				);
			UnlockStarShip (
				&GLOBAL (npc_built_ship_q), hStarShip
				);
		    }

		    if (FoundIlwrath)
		    {
			NextActivity |= CHECK_LOAD;	/* fake a load game */
			GLOBAL (CurrentActivity) |= START_ENCOUNTER;

			battle_counter = 0;
			PutGroupInfo (0L, (BYTE)~0);
			ReinitQueue (&GLOBAL (npc_built_ship_q));

			if (CurStarDescPtr->Index == ILWRATH_DEFINED)
			{
			    SET_GAME_STATE (GLOBAL_FLAGS_AND_DATA, 1 << 4);
			}
			else
			{
			    SET_GAME_STATE (GLOBAL_FLAGS_AND_DATA, 1 << 5);
			}
			if (pSolarSysState->MenuState.Initialized >= 3)
			    SaveFlagshipState ();
			return (FALSE);
		    }
		}
	    }
	    break;
	case TAALO_PROTECTOR_DEVICE:
	    break;
	case EGG_CASING0_DEVICE:
	case EGG_CASING1_DEVICE:
	case EGG_CASING2_DEVICE:
	    break;
	case SYREEN_SHUTTLE_DEVICE:
	    break;
	case VUX_BEAST_DEVICE:
	    break;
	case DESTRUCT_CODE_DEVICE:
	    break;
	case PORTAL_SPAWNER_DEVICE:
#define PORTAL_FUEL_COST	(10 * FUEL_TANK_SCALE)
	    if (LOBYTE (GLOBAL (CurrentActivity)) == IN_HYPERSPACE
		    && GET_GAME_STATE (ARILOU_SPACE_SIDE) <= 1
		    && GLOBAL_SIS (FuelOnBoard) >= PORTAL_FUEL_COST)
	    {
			/* don't want to DeltaSISGauges
			 * because the flagship picture
			 * is currently obscured
			 */
		GLOBAL_SIS (FuelOnBoard) -= PORTAL_FUEL_COST;
		SET_GAME_STATE (PORTAL_COUNTER, 1);
		return (FALSE);
	    }
	    break;
	case URQUAN_WARP_DEVICE:
	    break;
	case LUNAR_BASE_DEVICE:
	    break;
    }

    return (TRUE);
}

PROC(STATIC
BOOLEAN	DoManipulateDevices, (InputState, pMS),
    ARG		(INPUT_STATE	InputState)
    ARG_END	(PMENU_STATE	pMS)
)
{
    BYTE	NewState;

    if (GLOBAL (CurrentActivity) & CHECK_ABORT)
	return (FALSE);

    if (!(pMS->Initialized & 1))
    {
	DrawDevices (pMS, (BYTE)~0, (BYTE)~0);

	pMS->InputFunc = DoManipulateDevices;
	++pMS->Initialized;
	NewState = pMS->CurState;
	goto SelectDevice;
    }
    else if (InputState & DEVICE_BUTTON2)
    {
	return (FALSE);
    }
    else if (InputState & DEVICE_BUTTON1)
    {
	UWORD	status;

	SetSemaphore (&GraphicsSem);
	status = DeviceFailed (
		((PBYTE)pMS->CurFrame)[pMS->CurState - 1]
		);
	if (NewState = LOBYTE (status))
	    PlaySoundEffect (SetAbsSoundIndex (MenuSounds, 2),
		    0, GAME_SOUND_PRIORITY);
	else if (HIBYTE (status) == 0)
	    PlaySoundEffect (SetAbsSoundIndex (MenuSounds, 3),
		    0, GAME_SOUND_PRIORITY);
	ClearSemaphore (&GraphicsSem);

	return ((BOOLEAN)NewState);
    }
    else
    {
	SIZE	NewTop;

	NewTop = pMS->first_item.y;
	NewState = pMS->CurState - 1;
	if (GetInputXComponent (InputState) < 0
		|| GetInputYComponent (InputState) < 0)
	{
	    if (NewState-- == 0)
		NewState = 0;

	    if ((SIZE)NewState < NewTop && (NewTop -= MAX_VIS_DEVICES) < 0)
		NewTop = 0;
	}
	else if (GetInputXComponent (InputState) > 0
		|| GetInputYComponent (InputState) > 0)
	{
	    if (++NewState == (BYTE)pMS->first_item.x)
		NewState = (BYTE)(pMS->first_item.x - 1);

	    if (NewState >= NewTop + MAX_VIS_DEVICES)
		NewTop = NewState;
	}

	if (++NewState != pMS->CurState)
	{
	    if (NewTop != pMS->first_item.y)
	    {
		pMS->first_item.y = NewTop;
		pMS->CurState = (BYTE)~0;
	    }
SelectDevice:
	    DrawDevices (pMS, pMS->CurState - 1, NewState - 1);
	    pMS->CurState = NewState;
	}
    }

    return (TRUE);
}

PROC(
SIZE InventoryDevices, (pDeviceMap),
    ARG_END	(BYTE	*pDeviceMap)
)
{
    BYTE	i;
    SIZE	DevicesOnBoard;
    
    DevicesOnBoard = 0;
    for (i = 0; i < NUM_DEVICES; ++i)
    {
	BYTE	DeviceState;

	DeviceState = 0;
	switch (i)
	{
	    case ROSY_SPHERE_DEVICE:
		DeviceState = GET_GAME_STATE (ROSY_SPHERE_ON_SHIP);
		break;
	    case ARTIFACT_2_DEVICE:
		DeviceState = GET_GAME_STATE (ARTIFACT_2_ON_SHIP);
		break;
	    case ARTIFACT_3_DEVICE:
		DeviceState = GET_GAME_STATE (ARTIFACT_3_ON_SHIP);
		break;
	    case SUN_EFFICIENCY_DEVICE:
		DeviceState = GET_GAME_STATE (SUN_DEVICE_ON_SHIP);
		break;
	    case UTWIG_BOMB_DEVICE:
		DeviceState = GET_GAME_STATE (UTWIG_BOMB_ON_SHIP);
		break;
	    case ULTRON_0_DEVICE:
		DeviceState = (GET_GAME_STATE (ULTRON_CONDITION) == 1);
		break;
	    case ULTRON_1_DEVICE:
		DeviceState = (GET_GAME_STATE (ULTRON_CONDITION) == 2);
		break;
	    case ULTRON_2_DEVICE:
		DeviceState = (GET_GAME_STATE (ULTRON_CONDITION) == 3);
		break;
	    case ULTRON_3_DEVICE:
		DeviceState = (GET_GAME_STATE (ULTRON_CONDITION) == 4);
		break;
	    case MAIDENS_DEVICE:
		DeviceState = GET_GAME_STATE (MAIDENS_ON_SHIP);
		break;
	    case TALKING_PET_DEVICE:
		DeviceState = GET_GAME_STATE (TALKING_PET_ON_SHIP);
		break;
	    case AQUA_HELIX_DEVICE:
		DeviceState = GET_GAME_STATE (AQUA_HELIX_ON_SHIP);
		break;
	    case CLEAR_SPINDLE_DEVICE:
		DeviceState = GET_GAME_STATE (CLEAR_SPINDLE_ON_SHIP);
		break;
	    case UMGAH_HYPERWAVE_DEVICE:
		DeviceState = GET_GAME_STATE (UMGAH_BROADCASTERS_ON_SHIP);
		break;
	    case TAALO_PROTECTOR_DEVICE:
		DeviceState = GET_GAME_STATE (TAALO_PROTECTOR_ON_SHIP);
		break;
	    case EGG_CASING0_DEVICE:
		DeviceState = GET_GAME_STATE (EGG_CASE0_ON_SHIP);
		break;
	    case EGG_CASING1_DEVICE:
		DeviceState = GET_GAME_STATE (EGG_CASE1_ON_SHIP);
		break;
	    case EGG_CASING2_DEVICE:
		DeviceState = GET_GAME_STATE (EGG_CASE2_ON_SHIP);
		break;
	    case SYREEN_SHUTTLE_DEVICE:
		DeviceState = GET_GAME_STATE (SYREEN_SHUTTLE_ON_SHIP);
		break;
	    case VUX_BEAST_DEVICE:
		DeviceState = GET_GAME_STATE (VUX_BEAST_ON_SHIP);
		break;
	    case DESTRUCT_CODE_DEVICE:
#ifdef NEVER
		DeviceState = GET_GAME_STATE (DESTRUCT_CODE_ON_SHIP);
#endif /* NEVER */
		break;
	    case PORTAL_SPAWNER_DEVICE:
		DeviceState = GET_GAME_STATE (PORTAL_SPAWNER_ON_SHIP);
		break;
	    case URQUAN_WARP_DEVICE:
		DeviceState = GET_GAME_STATE (PORTAL_KEY_ON_SHIP);
		break;
	    case BURVIX_HYPERWAVE_DEVICE:
		DeviceState = GET_GAME_STATE (BURV_BROADCASTERS_ON_SHIP);
		break;
	    case LUNAR_BASE_DEVICE:
		DeviceState = GET_GAME_STATE (MOONBASE_ON_SHIP);
		break;
	}

#ifndef DEBUG
	if (DeviceState)
#endif /* DEBUG */
	{
	    *pDeviceMap++ = i;
	    ++DevicesOnBoard;
	}
    }
    
    return (DevicesOnBoard);
}

PROC(
BOOLEAN Devices, (pMS),
    ARG_END	(PMENU_STATE	pMS)
)
{
    BYTE	DeviceMap[NUM_DEVICES];

    if (pMS->first_item.x = InventoryDevices (DeviceMap))
    {
	pMS->InputFunc = DoManipulateDevices;
	--pMS->Initialized;
	pMS->CurState = 1;
	pMS->first_item.y = 0;

	pMS->CurFrame = (FRAME)DeviceMap;
	DoManipulateDevices (0, pMS);	/* to make sure it's initialized */
	DoInput ((PVOID)pMS);
	pMS->CurFrame = 0;

	pMS->InputFunc = DoFlagshipCommands;
	pMS->CurState = EQUIP_DEVICE + 1;

	if (GLOBAL_SIS (CrewEnlisted) != (COUNT)~0
		&& !(GLOBAL (CurrentActivity) & CHECK_ABORT))
	{
	    SetSemaphore (&GraphicsSem);
	    ClearSISRect (DRAW_SIS_DISPLAY);
	    ClearSemaphore (&GraphicsSem);

	    if (!GET_GAME_STATE (PORTAL_COUNTER)
		    && !(GLOBAL (CurrentActivity) & START_ENCOUNTER)
		    && GLOBAL_SIS (CrewEnlisted) != (COUNT)~0)
//		DrawMenuStateStrings (PM_SCAN, pMS->CurState - 1);
		return (TRUE);
	}
    }
    
    return (FALSE);
}

