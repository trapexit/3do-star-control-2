#include "starcon.h"

PROC(STATIC
void near BuildUrquanGuard, (),
    ARG_VOID
)
{
    BYTE	ship1, ship2;
    BYTE	b0, b1, b2, b3;
    POINT	org;
    HSTARSHIP	hStarShip, hNextShip;

    b0 = GET_GAME_STATE (SAMATRA_GRPOFFS0);
    b1 = GET_GAME_STATE (SAMATRA_GRPOFFS1);
    b2 = GET_GAME_STATE (SAMATRA_GRPOFFS2);
    b3 = GET_GAME_STATE (SAMATRA_GRPOFFS3);
    GLOBAL (BattleGroupRef) = MAKE_DWORD (
	    MAKE_WORD (b0, b1), MAKE_WORD (b2, b3)
	    );

    if (!GET_GAME_STATE (KOHR_AH_FRENZY))
	ship1 = URQUAN_SHIP, ship2 = BLACK_URQUAN_SHIP;
    else
	ship1 = BLACK_URQUAN_SHIP, ship2 = URQUAN_SHIP;

    for (b0 = 0; b0 < MAX_SHIPS_PER_SIDE; ++b0)
	CloneShipFragment (ship1,
    		&GLOBAL (npc_built_ship_q), 0);

    if (GLOBAL (BattleGroupRef) == 0)
    {
    	GLOBAL (BattleGroupRef) = PutGroupInfo (~0L, 1);
    	b0 = LOBYTE (LOWORD (GLOBAL (BattleGroupRef)));
    	b1 = HIBYTE (LOWORD (GLOBAL (BattleGroupRef)));
    	b2 = LOBYTE (HIWORD (GLOBAL (BattleGroupRef)));
    	b3 = HIBYTE (HIWORD (GLOBAL (BattleGroupRef)));
    	SET_GAME_STATE (SAMATRA_GRPOFFS0, b0);
    	SET_GAME_STATE (SAMATRA_GRPOFFS1, b1);
    	SET_GAME_STATE (SAMATRA_GRPOFFS2, b2);
    	SET_GAME_STATE (SAMATRA_GRPOFFS3, b3);
    }

#define NUM_URQUAN_GUARDS0	12
    for (b0 = 1; b0 <= NUM_URQUAN_GUARDS0; ++b0)
	PutGroupInfo (GLOBAL (BattleGroupRef), b0);

    ReinitQueue (&GLOBAL (npc_built_ship_q));
    for (b0 = 0; b0 < MAX_SHIPS_PER_SIDE; ++b0)
	CloneShipFragment (ship2,
    		&GLOBAL (npc_built_ship_q), 0);
#define NUM_URQUAN_GUARDS1	4
    for (b0 = 1; b0 <= NUM_URQUAN_GUARDS1; ++b0)
	PutGroupInfo (GLOBAL (BattleGroupRef), NUM_URQUAN_GUARDS0 + b0);

    GetGroupInfo (GLOBAL (BattleGroupRef), (BYTE)~0);

    XFormIPLoc (
	    &pSolarSysState->PlanetDesc[4].image.origin,
	    &org,
	    FALSE
	    );
    hStarShip = GetHeadLink (&GLOBAL (npc_built_ship_q));
    for (b0 = 0, b1 = 0;
	    b0 < NUM_URQUAN_GUARDS0;
	    ++b0, b1 += FULL_CIRCLE / (NUM_URQUAN_GUARDS0 + NUM_URQUAN_GUARDS1))
    {
	SHIP_FRAGMENTPTR	FragPtr;

	if (b1 % (FULL_CIRCLE / NUM_URQUAN_GUARDS1) == 0)
	    b1 += FULL_CIRCLE / (NUM_URQUAN_GUARDS0 + NUM_URQUAN_GUARDS1);

	FragPtr = (SHIP_FRAGMENTPTR)LockStarShip (
		&GLOBAL (npc_built_ship_q), hStarShip
		);
	hNextShip = _GetSuccLink (FragPtr);
	SET_GROUP_MISSION (FragPtr, ON_STATION | IGNORE_FLAGSHIP);
	SET_GROUP_LOC (FragPtr, 0);
	SET_GROUP_DEST (FragPtr, 4 + 1);
	SET_ORBIT_LOC (FragPtr,
		NORMALIZE_FACING (ANGLE_TO_FACING (b1)));
	FragPtr->ShipInfo.group_counter = 0;
	FragPtr->ShipInfo.loc.x = org.x
		+ COSINE (b1, STATION_RADIUS);
	FragPtr->ShipInfo.loc.y = org.y
		+ SINE (b1, STATION_RADIUS);
	UnlockStarShip (
		&GLOBAL (npc_built_ship_q), hStarShip
		);
	hStarShip = hNextShip;
    }

    for (b0 = 0, b1 = 0;
	    b0 < NUM_URQUAN_GUARDS1;
	    ++b0, b1 += FULL_CIRCLE / NUM_URQUAN_GUARDS1)
    {
	SHIP_FRAGMENTPTR	FragPtr;

	FragPtr = (SHIP_FRAGMENTPTR)LockStarShip (
		&GLOBAL (npc_built_ship_q), hStarShip
		);
	hNextShip = _GetSuccLink (FragPtr);
	SET_GROUP_MISSION (FragPtr, ON_STATION | IGNORE_FLAGSHIP);
	SET_GROUP_LOC (FragPtr, 0);
	SET_GROUP_DEST (FragPtr, 4 + 1);
	SET_ORBIT_LOC (FragPtr,
		NORMALIZE_FACING (ANGLE_TO_FACING (b1)));
	FragPtr->ShipInfo.group_counter = 0;
	FragPtr->ShipInfo.loc.x = org.x
		+ COSINE (b1, STATION_RADIUS);
	FragPtr->ShipInfo.loc.y = org.y
		+ SINE (b1, STATION_RADIUS);
	UnlockStarShip (
		&GLOBAL (npc_built_ship_q), hStarShip
		);
	hStarShip = hNextShip;
    }
}

PROC(
void GenerateSamatra, (control),
    ARG_END	(BYTE	control)
)
{
    switch (control)
    {
	case INIT_NPCS:
	    if (!GET_GAME_STATE (URQUAN_MESSED_UP))
		BuildUrquanGuard ();
	    break;
	case REINIT_NPCS:
	    GetGroupInfo (0L, 0);
	    battle_counter = 0;
	    {
		BOOLEAN		GuardEngaged;
		HSTARSHIP	hStarShip, hNextShip;

		GuardEngaged = FALSE;
		for (hStarShip = GetHeadLink (&GLOBAL (npc_built_ship_q));
			hStarShip; hStarShip = hNextShip)
		{
		    BYTE		task;
		    SHIP_FRAGMENTPTR	FragPtr;

		    FragPtr = (SHIP_FRAGMENTPTR)LockStarShip (
			    &GLOBAL (npc_built_ship_q), hStarShip
			    );
		    hNextShip = _GetSuccLink (FragPtr);

		    task = GET_GROUP_MISSION (FragPtr);
		    if (GET_GAME_STATE (URQUAN_MESSED_UP))
		    {
			SET_GROUP_MISSION (FragPtr,
				FLEE | IGNORE_FLAGSHIP | (task & REFORM_GROUP));
			SET_GROUP_DEST (FragPtr, 0);
		    }
		    else if (task & REFORM_GROUP)
		    {
			task &= ~REFORM_GROUP;
			FragPtr->ShipInfo.group_counter = 0;
			SET_GROUP_MISSION (FragPtr, task);

			GuardEngaged = TRUE;
		    }

		    UnlockStarShip (
			    &GLOBAL (npc_built_ship_q), hStarShip
			    );
		}

		if (GuardEngaged)
		{
		    COUNT	angle;
		    POINT	org;

		    XFormIPLoc (
			    &pSolarSysState->PlanetDesc[4].image.origin,
			    &org,
			    FALSE
			    );
		    angle = ARCTAN (
			    GLOBAL (ip_location.x) - org.x,
			    GLOBAL (ip_location.y) - org.y
			    );
		    GLOBAL (ip_location.x) = org.x
			    + COSINE (angle, 3000);
		    GLOBAL (ip_location.y) = org.y
			    + SINE (angle, 3000);
		    XFormIPLoc (
			    &GLOBAL (ip_location),
			    &GLOBAL (ShipStamp.origin),
			    TRUE
			    );
		}
	    }
	    break;
	case GENERATE_MOONS:
	    GenerateRandomIP (GENERATE_MOONS);
	    if (pSolarSysState->pBaseDesc == &pSolarSysState->PlanetDesc[4])
	    {
		COUNT	angle;
		DWORD	rand_val;

		pSolarSysState->MoonDesc[0].data_index = (BYTE)(~0 - 1);
		pSolarSysState->MoonDesc[0].radius = MIN_MOON_RADIUS
			+ (2 * MOON_DELTA);
		rand_val = random ();
		angle = NORMALIZE_ANGLE (LOWORD (rand_val));
		pSolarSysState->MoonDesc[0].location.x =
			COSINE (angle, pSolarSysState->MoonDesc[0].radius);
		pSolarSysState->MoonDesc[0].location.y =
			SINE (angle, pSolarSysState->MoonDesc[0].radius);
	    }
	    break;
	case GENERATE_PLANETS:
	    GenerateRandomIP (GENERATE_PLANETS);
	    pSolarSysState->PlanetDesc[4].NumPlanets = 1;
	    break;
	case GENERATE_ORBITAL:
		/* Samatra */
	    if (pSolarSysState->pOrbitalDesc->pPrevDesc == &pSolarSysState->PlanetDesc[4]
		    && pSolarSysState->pOrbitalDesc == &pSolarSysState->MoonDesc[0])
	    {
		PutGroupInfo (0L, (BYTE)~0);
		ReinitQueue (&GLOBAL (npc_built_ship_q));

		if (!GET_GAME_STATE (URQUAN_MESSED_UP))
		    CloneShipFragment (!GET_GAME_STATE (KOHR_AH_FRENZY) ?
			    URQUAN_SHIP : BLACK_URQUAN_SHIP,
			    &GLOBAL (npc_built_ship_q), (BYTE)~0);
		else
		{
#define URQUAN_REMNANTS	3
		    BYTE	i;

		    for (i = 0; i < URQUAN_REMNANTS; ++i)
		    {
			CloneShipFragment (URQUAN_SHIP,
				&GLOBAL (npc_built_ship_q), 0);
			CloneShipFragment (BLACK_URQUAN_SHIP,
				&GLOBAL (npc_built_ship_q), 0);
		    }
		}

		pSolarSysState->MenuState.Initialized += 2;
		GLOBAL (CurrentActivity) |= START_INTERPLANETARY;
		SET_GAME_STATE (GLOBAL_FLAGS_AND_DATA, 1 << 7);
		SET_GAME_STATE (URQUAN_PROTECTING_SAMATRA, 1);
		InitCommunication (URQUAN_CONVERSATION);

		if (!(GLOBAL (CurrentActivity) & (CHECK_ABORT | CHECK_LOAD)))
		{
		    BOOLEAN	UrquanSurvivors;

		    UrquanSurvivors = GetHeadLink (
			    &GLOBAL (npc_built_ship_q)
			    ) != 0;

		    GLOBAL (CurrentActivity) &= ~START_INTERPLANETARY;
		    ReinitQueue (&GLOBAL (npc_built_ship_q));
		    GetGroupInfo (0L, 0);
		    if (UrquanSurvivors)
		    {
			SET_GAME_STATE (URQUAN_PROTECTING_SAMATRA, 0);
		    }
		    else
		    {
			battle_counter = 0;
			GLOBAL (CurrentActivity) = IN_LAST_BATTLE | START_ENCOUNTER;
			if (GET_GAME_STATE (YEHAT_CIVIL_WAR)
				&& ActivateStarShip (YEHAT_SHIP, SPHERE_TRACKING)
				&& ActivateStarShip (YEHAT_REBEL_SHIP, FEASIBILITY_STUDY))
			    InitCommunication (YEHAT_REBEL_CONVERSATION);
		    }
		}
		pSolarSysState->MenuState.Initialized -= 2;
		break;
	    }
	default:
	    GenerateRandomIP (control);
	    break;
    }
}

