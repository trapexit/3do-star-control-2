#include "starcon.h"
#include "coderes.h"

CONTEXT		RadarContext;
FRAME		PlayFrame;

GLOBDATA	GlobData;

extern FRAME	flagship_status, misc_data;

PROC(
void CreateRadar, (),
    ARG_VOID
)
{
    if (RadarContext == 0)
    {
	RECT	r;
	CONTEXT	OldContext;

	RadarContext = CaptureContext (CreateContext ());
	OldContext = SetContext (RadarContext);
	SetContextFGFrame (Screen);
	r.corner.x = RADAR_X;
	r.corner.y = RADAR_Y;
	r.extent.width = RADAR_WIDTH;
	r.extent.height = RADAR_HEIGHT;
	SetContextClipRect (&r);
	SetContext (OldContext);
    }
}

PROC(
BOOLEAN LoadSC2Data, (),
    ARG_VOID
)
{
    if (flagship_status == 0)
    {
	MEM_HANDLE	hOldIndex;

	hOldIndex = SetResourceIndex (hResIndex);

	if ((flagship_status = CaptureDrawable (
		LoadGraphic (FLAGSTAT_MASK_PMAP_ANIM)
		)) == 0)
	    return (FALSE);

	if ((misc_data = CaptureDrawable (
		LoadGraphic (MISCDATA_MASK_PMAP_ANIM)
		)) == 0)
	    return (FALSE);

	SetResourceIndex (hOldIndex);
    }

    CreateRadar ();

    if (LOBYTE (GLOBAL (CurrentActivity)) == IN_HYPERSPACE)
    {
	GLOBAL (ShipStamp.origin.x) =
		GLOBAL (ShipStamp.origin.y) = -1;
    }

    return (TRUE);
}

PROC(
BOOLEAN InitSIS, (),
    ARG_VOID
)
{
    COUNT	i;

    InitGlobData ();

    PlayFrame = CaptureDrawable (LoadGraphic (PLAYMENU_ANIM));
    
    {
	COUNT		num_ships;
	RES_TYPE	rt;
	RES_INSTANCE	ri;
	RES_PACKAGE	rp;

	rt = GET_TYPE (ARILOU_SHIP_INDEX);
	ri = GET_INSTANCE (ARILOU_SHIP_INDEX);
	rp = GET_PACKAGE (ARILOU_SHIP_INDEX);

	num_ships = (GET_PACKAGE (BLACKURQ_SHIP_INDEX) - rp + 1)
		+ 2;	/* Yehat Rebels and Ur-Quan probe */

	InitQueue (&GLOBAL (avail_race_q),
		num_ships, sizeof (EXTENDED_SHIP_FRAGMENT));
	for (i = 0; i < num_ships; ++i)
	{
	    DWORD	ship_ref;
	    HSTARSHIP	hStarShip;

	    if (i < num_ships - 2)
		ship_ref = MAKE_RESOURCE (rp++, rt, ri++);
	    else if (i == num_ships - 2)
		ship_ref = YEHAT_SHIP_INDEX;
	    else
		ship_ref = PROBE_RES_INDEX;
	    if (hStarShip = Build (&GLOBAL (avail_race_q), ship_ref, 0, 0))
	    {
		SHIP_FRAGMENTPTR		FragPtr;
		EXTENDED_SHIP_FRAGMENTPTR	ExtFragPtr;
		PROC_GLOBAL(
		HSTARSHIP FindMasterShip, (ship_ref),
		    ARG_END	(DWORD	ship_ref)
		);

		FragPtr = (SHIP_FRAGMENTPTR)LockStarShip (
			&GLOBAL (avail_race_q), hStarShip
			);
		if (i < num_ships - 1)
		{
		    HSTARSHIP		hMasterShip;
		    STARSHIPPTR		MasterShipPtr;
		    extern QUEUE	master_q;
		    
		    hMasterShip = FindMasterShip (ship_ref);
		    MasterShipPtr = LockStarShip (&master_q, hMasterShip);
		    FragPtr->ShipInfo = ((SHIP_FRAGMENTPTR)MasterShipPtr)->ShipInfo;
		    UnlockStarShip (&master_q, hMasterShip);
		}
		else
		{
		    load_ship ((STARSHIPPTR)FragPtr, FALSE);
		    FragPtr->ShipInfo = FragPtr->RaceDescPtr->ship_info;
		    FragPtr->RaceDescPtr->ship_info.melee_icon = 0;
		    FragPtr->RaceDescPtr->ship_info.icons = 0;
		    FragPtr->RaceDescPtr->ship_info.race_strings = 0;
		    free_ship ((STARSHIPPTR)FragPtr, FALSE);
		}

		FragPtr->ShipInfo.ship_flags = BAD_GUY;
		ExtFragPtr = (EXTENDED_SHIP_FRAGMENTPTR)FragPtr;
		ExtFragPtr->ShipInfo.known_strength = 0;
		ExtFragPtr->ShipInfo.known_loc = ExtFragPtr->ShipInfo.loc;
		if (FragPtr->ShipInfo.var2 == (BYTE)~0)
		    ExtFragPtr->ShipInfo.actual_strength = (COUNT)~0;
		else if (i == YEHAT_REBEL_SHIP)
		    ExtFragPtr->ShipInfo.actual_strength = 0;
		else
		    ExtFragPtr->ShipInfo.actual_strength =
			    (COUNT)FragPtr->ShipInfo.var2 << 1;
		ExtFragPtr->ShipInfo.growth_fract = 0;
		ExtFragPtr->ShipInfo.growth_err_term = 255 >> 1;
		ExtFragPtr->ShipInfo.energy_level = 0;
		ExtFragPtr->ShipInfo.days_left = 0;
		FragPtr->RaceDescPtr = (RACE_DESCPTR)&ExtFragPtr->ShipInfo;

		UnlockStarShip (
			&GLOBAL (avail_race_q), hStarShip
			);
	    }
	}
    }

    InitSISContexts ();
    LoadSC2Data ();

    InitPlanetInfo ();
    InitGroupInfo (TRUE);

    GLOBAL (glob_flags) = NUM_READ_SPEEDS >> 1;

    GLOBAL (ElementWorth[COMMON]) = 1;
    GLOBAL_SIS (ElementAmounts[COMMON]) = 0;
    GLOBAL (ElementWorth[CORROSIVE]) = 2;
    GLOBAL_SIS (ElementAmounts[CORROSIVE]) = 0;
    GLOBAL (ElementWorth[BASE_METAL]) = 3;
    GLOBAL_SIS (ElementAmounts[BASE_METAL]) = 0;
    GLOBAL (ElementWorth[NOBLE]) = 4;
    GLOBAL_SIS (ElementAmounts[NOBLE]) = 0;
    GLOBAL (ElementWorth[RARE_EARTH]) = 5;
    GLOBAL_SIS (ElementAmounts[RARE_EARTH]) = 0;
    GLOBAL (ElementWorth[PRECIOUS]) = 6;
    GLOBAL_SIS (ElementAmounts[PRECIOUS]) = 0;
    GLOBAL (ElementWorth[RADIOACTIVE]) = 8;
    GLOBAL_SIS (ElementAmounts[RADIOACTIVE]) = 0;
    GLOBAL (ElementWorth[EXOTIC]) = 25;
    GLOBAL_SIS (ElementAmounts[EXOTIC]) = 0;

    for (i = 0; i < NUM_DRIVE_SLOTS; ++i)
	GLOBAL_SIS (DriveSlots[i]) = EMPTY_SLOT + 0;
    GLOBAL_SIS (DriveSlots[5]) =
	    GLOBAL_SIS (DriveSlots[6]) = FUSION_THRUSTER;
    for (i = 0; i < NUM_JET_SLOTS; ++i)
	GLOBAL_SIS (JetSlots[i]) = EMPTY_SLOT + 1;
    GLOBAL_SIS (JetSlots[0]) =
	    GLOBAL_SIS (JetSlots[6]) = TURNING_JETS;
    for (i = 0; i < NUM_MODULE_SLOTS; ++i)
	GLOBAL_SIS (ModuleSlots[i]) = EMPTY_SLOT + 2;
    GLOBAL_SIS (ModuleSlots[15]) = GUN_WEAPON;
    GLOBAL_SIS (ModuleSlots[2]) = CREW_POD;
    GLOBAL_SIS (CrewEnlisted) = CREW_POD_CAPACITY;
    GLOBAL_SIS (ModuleSlots[8]) = STORAGE_BAY;
#ifndef TESTING
    GLOBAL_SIS (ModuleSlots[1]) = FUEL_TANK;
    GLOBAL_SIS (FuelOnBoard) = 10 * FUEL_TANK_SCALE;
#else /* TESTING */
for (i = 0; i < NUM_DRIVE_SLOTS; ++i)
    GLOBAL_SIS (DriveSlots[i]) = FUSION_THRUSTER;
for (i = 0; i < NUM_JET_SLOTS; ++i)
    GLOBAL_SIS (JetSlots[i]) = TURNING_JETS;
for (i = 0; i < 2; ++i)
{
GLOBAL_SIS (ModuleSlots[i]) = HIGHEFF_FUELSYS;
GLOBAL_SIS (FuelOnBoard) += HEFUEL_TANK_CAPACITY;
}
for (; i < 7; ++i)
{
GLOBAL_SIS (ModuleSlots[i]) = CREW_POD;
GLOBAL_SIS (CrewEnlisted) += CREW_POD_CAPACITY;
}
GLOBAL_SIS (ModuleSlots[i++]) = STORAGE_BAY;
GLOBAL_SIS (ModuleSlots[i++]) = SHIVA_FURNACE;
GLOBAL_SIS (ModuleSlots[i++]) = SHIVA_FURNACE;
GLOBAL_SIS (ModuleSlots[i++]) = DYNAMO_UNIT;
GLOBAL_SIS (ModuleSlots[i++]) = TRACKING_SYSTEM;
GLOBAL_SIS (ModuleSlots[i++]) = TRACKING_SYSTEM;
GLOBAL_SIS (ModuleSlots[i++]) = SHIVA_FURNACE;
GLOBAL_SIS (ModuleSlots[i++]) = CANNON_WEAPON;
GLOBAL_SIS (ModuleSlots[i++]) = CANNON_WEAPON;
#endif /* TESTING */

    InitQueue (&GLOBAL (built_ship_q),
	    MAX_BUILT_SHIPS, sizeof (SHIP_FRAGMENT));
    InitQueue (&GLOBAL (npc_built_ship_q),
	    (MAX_BATTLE_GROUPS >> 1), sizeof (SHIP_FRAGMENT));
    InitQueue (&GLOBAL (encounter_q), 16, sizeof (ENCOUNTER));

    GLOBAL (CurrentActivity) = IN_INTERPLANETARY | START_INTERPLANETARY;

    GLOBAL_SIS (ResUnits) = 0;
    GLOBAL (CrewCost) = 3;
    GLOBAL (FuelCost) = 20;
    GLOBAL (ModuleCost[PLANET_LANDER]) = 500 / MODULE_COST_SCALE;
    GLOBAL (ModuleCost[FUSION_THRUSTER]) = 500 / MODULE_COST_SCALE;
    GLOBAL (ModuleCost[TURNING_JETS]) = 500 / MODULE_COST_SCALE;
    GLOBAL (ModuleCost[CREW_POD]) = 2000 / MODULE_COST_SCALE;
    GLOBAL (ModuleCost[STORAGE_BAY]) = 750 / MODULE_COST_SCALE;
    GLOBAL (ModuleCost[FUEL_TANK]) = 500 / MODULE_COST_SCALE;
    GLOBAL (ModuleCost[DYNAMO_UNIT]) = 2000 / MODULE_COST_SCALE;
    GLOBAL (ModuleCost[GUN_WEAPON]) = 2000 / MODULE_COST_SCALE;

    GLOBAL_SIS (NumLanders) = 1;

    strcpy (GLOBAL_SIS (ShipName), "Vindicator");
    strcpy (GLOBAL_SIS (CommanderName), "Zelnick");

    ActivateStarShip (HUMAN_SHIP, 0);
    CloneShipFragment (HUMAN_SHIP, &GLOBAL (built_ship_q), 0);

    GLOBAL_SIS (log_x) = UNIVERSE_TO_LOGX (SOL_X);
    GLOBAL_SIS (log_y) = UNIVERSE_TO_LOGY (SOL_Y);
    CurStarDescPtr = 0;
    GLOBAL (autopilot.x) = GLOBAL (autopilot.y) = ~0;

    return (TRUE);
}

PROC(
void FreeSC2Data, (),
    ARG_VOID
)
{
    DestroyContext (ReleaseContext (RadarContext));
    RadarContext = 0;
    DestroyDrawable (ReleaseDrawable (misc_data));
    misc_data = 0;
    DestroyDrawable (ReleaseDrawable (flagship_status));
    flagship_status = 0;
}

PROC(
void UninitSIS, (),
    ARG_VOID
)
{
    HSTARSHIP	hStarShip;

    UninitQueue (&GLOBAL (encounter_q));
    UninitQueue (&GLOBAL (npc_built_ship_q));
    UninitQueue (&GLOBAL (built_ship_q));
    UninitGroupInfo ();
    UninitPlanetInfo ();

//    FreeSC2Data ();

    if (hStarShip = GetTailLink (&GLOBAL (avail_race_q)))
    {
	STARSHIPPTR	StarShipPtr;

	StarShipPtr = LockStarShip (&GLOBAL (avail_race_q), hStarShip);
	DestroyDrawable (ReleaseDrawable (
		StarShipPtr->RaceDescPtr->ship_info.melee_icon));
	DestroyDrawable (ReleaseDrawable (
		StarShipPtr->RaceDescPtr->ship_info.icons));
	DestroyStringTable (ReleaseStringTable (
		StarShipPtr->RaceDescPtr->ship_info.race_strings));
	UnlockStarShip (&GLOBAL (avail_race_q), hStarShip);
    }

    UninitQueue (&GLOBAL (avail_race_q));
    
    DestroyDrawable (ReleaseDrawable (PlayFrame));
    PlayFrame = 0;
}

