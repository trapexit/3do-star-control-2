#include "reslib.h"
#include "resinst.h"
#include "starcon.h"
#include "commglue.h"
#include "strings.h"

static LOCDATA	druuge_desc =
{
    NULL_PTR,				/* init_encounter_func */
    NULL_PTR,				/* uninit_encounter_func */
    (FRAME)DRUUGE_PMAP_ANIM,		/* AlienFrame */
    0,					/* AlienColorMap */
    DRUUGE_MUSIC,			/* AlienSong */
    DRUUGE_PLAYER_PHRASES,		/* PlayerPhrases */
    11,
    {
	{
	    5,				/* StartIndex */
	    4,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    9, 0,			/* FrameRate */
	    9, 0,			/* RestartRate */
	},
	{
	    9,				/* StartIndex */
	    4,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    9, 0,			/* FrameRate */
	    9, 0,			/* RestartRate */
	},
	{
	    13,				/* StartIndex */
	    6,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    9, 0,			/* FrameRate */
	    9, 0,			/* RestartRate */
	},
	{
	    19,				/* StartIndex */
	    3,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    6, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    22,				/* StartIndex */
	    3,				/* NumFrames */
	    RANDOM_ANIM,		/* AnimFlags */
	    10, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    25,				/* StartIndex */
	    3,				/* NumFrames */
	    RANDOM_ANIM,		/* AnimFlags */
	    10, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    28,				/* StartIndex */
	    3,				/* NumFrames */
	    RANDOM_ANIM,		/* AnimFlags */
	    10, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    31,				/* StartIndex */
	    2,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    10, 0,			/* FrameRate */
	    ONE_SECOND, ONE_SECOND * 3,	/* RestartRate */
	},
	{
	    33,				/* StartIndex */
	    7,				/* NumFrames */
	    CIRCULAR_ANIM
		    | WAIT_TALKING,	/* AnimFlags */
	    10, 0,			/* FrameRate */
	    ONE_SECOND * 7, ONE_SECOND * 3,/* RestartRate */
	},
	{
	    40,				/* StartIndex */
	    4,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    36, 0,			/* FrameRate */
	    36, 0,			/* RestartRate */
	},
	{
	    44,				/* StartIndex */
	    4,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    24, 0,			/* FrameRate */
	    24, 0,			/* RestartRate */
	},
    },
    {
	0,				/* StartIndex */
	0,				/* NumFrames */
	0,				/* AnimFlags */
	0, 0,				/* FrameRate */
	0, 0,				/* RestartRate */
    },
    {
	1,				/* StartIndex */
	4,				/* NumFrames */
	0,				/* AnimFlags */
	8, 8,				/* FrameRate */
	10, 10,				/* RestartRate */
    },
};

static COUNT	SlaveryCount = 0;

static void	far
ExitConversation (Response)
RESPONSE_REF	Response;
{
    PUSH_CONTEXT
    {
	if (PLAYER_SAID (Response, bye))
	{
	    SET_GAME_STATE (BATTLE_SEGUE, 0);

	    if (GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 7))
	    {
		if (SlaveryCount)
		{
		    UWORD	PreviousSlaves;

		    PreviousSlaves = MAKE_WORD (
			    GET_GAME_STATE (CREW_SOLD_TO_DRUUGE0),
			    GET_GAME_STATE (CREW_SOLD_TO_DRUUGE1)
			    );
		    SlaveryCount += PreviousSlaves;
		    if (SlaveryCount > 250 && PreviousSlaves <= 250)
		    {
			if (PreviousSlaves > 100)
			    GLOBAL (CrewCost) += (22 - 7);
			else
			    GLOBAL (CrewCost) += 22;
		    }
		    else if (SlaveryCount > 100 && PreviousSlaves <= 100)
			GLOBAL (CrewCost) += 7;

		    SET_GAME_STATE (CREW_SOLD_TO_DRUUGE0, LOBYTE (SlaveryCount));
		    SET_GAME_STATE (CREW_SOLD_TO_DRUUGE1, HIBYTE (SlaveryCount));
		}

		switch (GET_GAME_STATE (DRUUGE_HOME_VISITS))
		{
		    case 1:
			NPCPhrase (BYE_FROM_TRADE_WORLD_1);
			break;
		    default:
			NPCPhrase (BYE_FROM_TRADE_WORLD_2);
			break;
		}
	    }
	    else if (GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 6))
		NPCPhrase (GOODBYE_FROM_BOMB_PLANET);
	    else
		NPCPhrase (GOODBYE_FROM_SPACE);
	}
	else /* if (Response == then_we_take_bomb) */
	{
	    SET_GAME_STATE (BATTLE_SEGUE, 1);

	    NPCPhrase (FIGHT_FOR_BOMB);
	}
    }
    POP_CONTEXT
}

PROC_LOCAL(
void far TradeWorld, (Response),
    ARG_END	(RESPONSE_REF	Response)
);

PROC(STATIC
void far Buy, (Response),
    ARG_END	(RESPONSE_REF	Response)
)
{
    PUSH_CONTEXT
    {
	if (PLAYER_SAID (Response, want_to_buy)
		|| PLAYER_SAID (Response, im_ready_to_buy))
	{
	    NPCPhrase (READY_TO_SELL);
	    if (!GET_GAME_STATE (ROSY_SPHERE))
		NPCPhrase (HAVE_SPHERE);
	    if (!GET_GAME_STATE (ARTIFACT_2_ON_SHIP))
		NPCPhrase (HAVE_ART_1);
	    if (!GET_GAME_STATE (ARTIFACT_3_ON_SHIP))
		NPCPhrase (HAVE_ART_2);
	    NPCPhrase (SHIPS_AND_FUEL);

	    SET_GAME_STATE (KNOW_DRUUGE_SLAVERS, 3);
	}
	else if (PLAYER_SAID (Response, buy_druuge_ship))
	{
#define SHIP_CREW_COST		100
	    if (GLOBAL_SIS (CrewEnlisted) < SHIP_CREW_COST)
		NPCPhrase (NOT_ENOUGH_CREW);
	    else if (ActivateStarShip (DRUUGE_SHIP, FEASIBILITY_STUDY) == 0)
		NPCPhrase (NOT_ENOUGH_ROOM);
	    else
	    {
		SetSemaphore (&GraphicsSem);
		DeltaSISGauges (-SHIP_CREW_COST, 0, 0);
		ClearSemaphore (&GraphicsSem);
		SlaveryCount += SHIP_CREW_COST;
		ActivateStarShip (DRUUGE_SHIP, 1);

		NPCPhrase (BOUGHT_SHIP);
	    }
	}
#define ARTIFACT_CREW_COST	100
	else if (PLAYER_SAID (Response, buy_rosy_sphere))
	{
	    if (GLOBAL_SIS (CrewEnlisted) < ARTIFACT_CREW_COST)
		NPCPhrase (NOT_ENOUGH_CREW);
	    else
	    {
		SetSemaphore (&GraphicsSem);
		DeltaSISGauges (-ARTIFACT_CREW_COST, 0, 0);
		ClearSemaphore (&GraphicsSem);
		SlaveryCount += ARTIFACT_CREW_COST;
		SET_GAME_STATE (ROSY_SPHERE_ON_SHIP, 1);
		SET_GAME_STATE (ROSY_SPHERE, 1);

		NPCPhrase (BOUGHT_SPHERE);
	    }
	}
	else if (PLAYER_SAID (Response, buy_art_1))
	{
	    if (GLOBAL_SIS (CrewEnlisted) < ARTIFACT_CREW_COST)
		NPCPhrase (NOT_ENOUGH_CREW);
	    else
	    {
		SetSemaphore (&GraphicsSem);
		DeltaSISGauges (-ARTIFACT_CREW_COST, 0, 0);
		ClearSemaphore (&GraphicsSem);
		SlaveryCount += ARTIFACT_CREW_COST;
		SET_GAME_STATE (ARTIFACT_2_ON_SHIP, 1);

		NPCPhrase (BOUGHT_ART_1);
	    }
	}
	else if (PLAYER_SAID (Response, buy_art_2))
	{
	    if (GLOBAL_SIS (CrewEnlisted) < ARTIFACT_CREW_COST)
		NPCPhrase (NOT_ENOUGH_CREW);
	    else
	    {
		SetSemaphore (&GraphicsSem);
		DeltaSISGauges (-ARTIFACT_CREW_COST, 0, 0);
		ClearSemaphore (&GraphicsSem);
		SlaveryCount += ARTIFACT_CREW_COST;
		SET_GAME_STATE (ARTIFACT_3_ON_SHIP, 1);

		NPCPhrase (BOUGHT_ART_2);
	    }
	}
	else if (PLAYER_SAID (Response, buy_fuel))
	{
#define FUEL_CREW_COST	10
	    if (GLOBAL_SIS (CrewEnlisted) < FUEL_CREW_COST)
		NPCPhrase (NOT_ENOUGH_CREW);
	    else
	    {
		SetSemaphore (&GraphicsSem);
		DeltaSISGauges (-FUEL_CREW_COST,
			FUEL_CREW_COST * FUEL_TANK_SCALE, 0);
		ClearSemaphore (&GraphicsSem);
		SlaveryCount += FUEL_CREW_COST;

		NPCPhrase (BOUGHT_FUEL);
	    }
	}

	Response (buy_druuge_ship, Buy);
	if (!GET_GAME_STATE (ROSY_SPHERE))
	    Response (buy_rosy_sphere, Buy);
	if (!GET_GAME_STATE (ARTIFACT_2_ON_SHIP))
	    Response (buy_art_1, Buy);
	if (!GET_GAME_STATE (ARTIFACT_3_ON_SHIP))
	    Response (buy_art_2, Buy);
	Response (buy_fuel, Buy);
	Response (done_buying, TradeWorld);
    }
    POP_CONTEXT
}

PROC_LOCAL(
void far Sell, (R),
    ARG_END	(RESPONSE_REF	R)
);

static RESPONSE_REF	LastResponse = 0;

PROC(STATIC
void far Trade, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	if (!PLAYER_SAID (R, whats_the_sphere_again))
	{
	    NPCPhrase (TRADE_FOR_SPHERE);
	    LastResponse = R;
	}
	else
	{
	    NPCPhrase (SPHERE_IS);

	    DISABLE_PHRASE (whats_the_sphere_again);
	}

	Response (no_way, Sell);
	Response (way, Sell);
	if (PHRASE_ENABLED (whats_the_sphere_again))
	{
	    Response (whats_the_sphere_again, Trade);
	}
    }
    POP_CONTEXT
}

PROC(STATIC
void near DoTransaction, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    if (PLAYER_SAID (R, sell_maidens))
    {
	SET_GAME_STATE (MAIDENS_ON_SHIP, 0);
    }
    else if (PLAYER_SAID (R, sell_fragments))
    {
	BYTE	num_frags;

	if (GET_GAME_STATE (EGG_CASE0_ON_SHIP))
	{
	    SET_GAME_STATE (EGG_CASE0_ON_SHIP, 0);
	}
	else if (GET_GAME_STATE (EGG_CASE1_ON_SHIP))
	{
	    SET_GAME_STATE (EGG_CASE1_ON_SHIP, 0);
	}
	else if (GET_GAME_STATE (EGG_CASE2_ON_SHIP))
	{
	    SET_GAME_STATE (EGG_CASE2_ON_SHIP, 0);
	}

	num_frags = GET_GAME_STATE (FRAGMENTS_BOUGHT) + 1;
	SET_GAME_STATE (FRAGMENTS_BOUGHT, num_frags);
    }
    else if (PLAYER_SAID (R, sell_caster))
    {
	SET_GAME_STATE (BURV_BROADCASTERS_ON_SHIP, 0);
    }
    else if (PLAYER_SAID (R, sell_spawner))
    {
	SET_GAME_STATE (PORTAL_SPAWNER_ON_SHIP, 0);
    }

    if (!GET_GAME_STATE (ROSY_SPHERE)
	    && GET_GAME_STATE (ROSY_SPHERE_ON_SHIP))
    {
	SET_GAME_STATE (ROSY_SPHERE, 1);
    }
    else
    {
	BYTE	trade_gas;
	BYTE	ship_slots, ships_to_trade;

	trade_gas = 0;
	ships_to_trade = 0;
	ship_slots = ActivateStarShip (DRUUGE_SHIP, FEASIBILITY_STUDY);
	if (PLAYER_SAID (R, sell_maidens))
	{
	    NPCPhrase (BOUGHT_MAIDENS);
	    ships_to_trade = 6;
	}
	else if (PLAYER_SAID (R, sell_fragments))
	{
	    NPCPhrase (BOUGHT_FRAGMENTS);
	    ships_to_trade = 1;
	}
	else if (PLAYER_SAID (R, sell_caster))
	{
	    NPCPhrase (BOUGHT_CASTER);
	    ships_to_trade = 0;
	    trade_gas = 1;
	}
	else if (PLAYER_SAID (R, sell_spawner))
	{
	    NPCPhrase (BOUGHT_SPAWNER);
	    ships_to_trade = 3;
	    trade_gas = 1;
	}

	NPCPhrase (YOU_GET);
	if (ships_to_trade)
	{
	    ActivateStarShip (DRUUGE_SHIP, ships_to_trade);

	    if (ship_slots >= ships_to_trade)
		NPCPhrase (DEAL_FOR_STATED_SHIPS);
	    else if (ship_slots == 0)
		NPCPhrase (DEAL_FOR_NO_SHIPS);
	    else
		NPCPhrase (DEAL_FOR_LESS_SHIPS);

	    if (trade_gas)
		NPCPhrase (YOU_ALSO_GET);
	}

	if (trade_gas)
	{
	    BYTE	slot;
	    COUNT	f;
	    DWORD	capacity;

	    capacity = FUEL_RESERVE;
	    slot = NUM_MODULE_SLOTS - 1;
	    do
	    {
		if (GLOBAL_SIS (ModuleSlots[slot]) == FUEL_TANK
			|| GLOBAL_SIS (ModuleSlots[slot]) == HIGHEFF_FUELSYS)
		{
		    COUNT	volume;

		    volume = GLOBAL_SIS (ModuleSlots[slot]) == FUEL_TANK
			    ? FUEL_TANK_CAPACITY : HEFUEL_TANK_CAPACITY;
		    capacity += volume;
		}
	    } while (slot--);
	    capacity -= GLOBAL_SIS (FuelOnBoard);
	    f = (COUNT)((capacity + (FUEL_TANK_SCALE >> 1)) / FUEL_TANK_SCALE);

	    SetSemaphore (&GraphicsSem);
	    while (capacity > 0x3FFFL)
	    {
		DeltaSISGauges (0, 0x3FFF, 0);
		capacity -= 0x3FFF;
	    }
	    DeltaSISGauges (0, (SIZE)capacity, 0);
	    ClearSemaphore (&GraphicsSem);

	    NPCPhrase (SOLD_YOU_FUEL);

	    if (f >= 250)
		NPCPhrase (HIDEOUS_DEAL);
	    else if (f >= 100)
		NPCPhrase (BAD_DEAL);
	    else if (f >= 50)
		NPCPhrase (FAIR_DEAL);
	    else if (f >= 10)
		NPCPhrase (GOOD_DEAL);
	    else
		NPCPhrase (FINE_DEAL);
	}
    }
}

PROC(STATIC
void far Sell, (Response),
    ARG_END	(RESPONSE_REF	Response)
)
{
    PUSH_CONTEXT
    {
RESPONSE_FUNC RespFunc;

	if (PLAYER_SAID (Response, want_to_sell))
	    NPCPhrase (READY_TO_BUY);
	else if (PLAYER_SAID (Response, no_way)
		|| PLAYER_SAID (Response, way))
	{
	    if (PLAYER_SAID (Response, no_way))
		NPCPhrase (OK_REGULAR_DEAL);
	    else
	    {
		NPCPhrase (OK_HERES_SPHERE);

		SET_GAME_STATE (ROSY_SPHERE_ON_SHIP, 1);
	    }

	    DoTransaction (LastResponse);
	}
	else if (PLAYER_SAID (Response, sell_maidens)
		|| PLAYER_SAID (Response, sell_fragments)
		|| PLAYER_SAID (Response, sell_caster)
		|| PLAYER_SAID (Response, sell_spawner))
	{
	    DoTransaction (Response);
	}

	if (!GET_GAME_STATE (ROSY_SPHERE))
	    RespFunc = (RESPONSE_FUNC)Trade;
	else
	    RespFunc = (RESPONSE_FUNC)Sell;
	if (GET_GAME_STATE (MAIDENS_ON_SHIP))
	    Response (sell_maidens, RespFunc);
	if ((GET_GAME_STATE (EGG_CASE0_ON_SHIP)
		|| GET_GAME_STATE (EGG_CASE1_ON_SHIP)
		|| GET_GAME_STATE (EGG_CASE2_ON_SHIP))
		&& GET_GAME_STATE (FRAGMENTS_BOUGHT) < 2)
	    Response (sell_fragments, RespFunc);
	if (GET_GAME_STATE (BURV_BROADCASTERS_ON_SHIP))
	    Response (sell_caster, RespFunc);
	if (GET_GAME_STATE (PORTAL_SPAWNER_ON_SHIP))
	    Response (sell_spawner, RespFunc);
	Response (done_selling, TradeWorld);
    }
    POP_CONTEXT
}

PROC(STATIC
void far ExplainSlaveTrade, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	if (PLAYER_SAID (R, want_to_buy))
	    NPCPhrase (WE_SELL_FOR_CREW);
	else if (PLAYER_SAID (R, isnt_this_slave_trading))
	{
	    NPCPhrase (NO_SLAVE_TRADE);

	    SET_GAME_STATE (KNOW_DRUUGE_SLAVERS, 1);
	}
	else if (PLAYER_SAID (R, what_do_with_crew))
	{
	    NPCPhrase (HAVE_FUN);

	    SET_GAME_STATE (KNOW_DRUUGE_SLAVERS, 2);
	}

	switch (GET_GAME_STATE (KNOW_DRUUGE_SLAVERS))
	{
	    case 0:
		Response (isnt_this_slave_trading, ExplainSlaveTrade);
		break;
	    case 1:
		Response (what_do_with_crew, ExplainSlaveTrade);
		break;
	}
	Response (i_will_never_trade_crew,  TradeWorld);
	Response (im_ready_to_buy, Buy);
    }
    POP_CONTEXT
}

PROC(STATIC
void far TradeWorld, (Response),
    ARG_END	(RESPONSE_REF	Response)
)
{
    PUSH_CONTEXT
    {
	if (PLAYER_SAID (Response, whats_up_at_trade_world))
	{
	    BYTE	NumVisits;

	    NumVisits = GET_GAME_STATE (DRUUGE_HOME_INFO);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (GEN_INFO_AT_TRADE_WORLD_1);
		    break;
		case 1:
		    NPCPhrase (GEN_INFO_AT_TRADE_WORLD_2);
		    break;
		case 2:
		    NPCPhrase (GEN_INFO_AT_TRADE_WORLD_3);
		    if (GET_GAME_STATE (KNOW_ABOUT_SHATTERED) < 2)
		    {
			SET_GAME_STATE (KNOW_ABOUT_SHATTERED, 2);
		    }
		    break;
		case 3:
		    NPCPhrase (GEN_INFO_AT_TRADE_WORLD_4);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (DRUUGE_HOME_INFO, NumVisits);
	    DISABLE_PHRASE (whats_up_at_trade_world);
	}
	else if (PLAYER_SAID (Response, done_selling))
	    NPCPhrase (OK_DONE_SELLING);
	else if (PLAYER_SAID (Response, done_buying))
	    NPCPhrase (OK_DONE_BUYING);
	else if (PLAYER_SAID (Response, i_will_never_trade_crew))
	    NPCPhrase (YOUR_LOSS);

	if (PHRASE_ENABLED (whats_up_at_trade_world))
	{
	    Response (whats_up_at_trade_world, TradeWorld);
	}
	Response (want_to_sell, Sell);
	if (GET_GAME_STATE (KNOW_DRUUGE_SLAVERS) == 3)
	    Response (want_to_buy, Buy);
	else
	    Response (want_to_buy, ExplainSlaveTrade);
	Response (bye, ExitConversation);
    }
    POP_CONTEXT
}

PROC(STATIC
void far BombAmbush, (Response),
    ARG_END	(RESPONSE_REF	Response)
)
{
    PUSH_CONTEXT
    {
	if (PLAYER_SAID (Response, whats_up_at_bomb_planet))
	{
	    NPCPhrase (GEN_INFO_AT_BOMB_PLANET);
	    SET_GAME_STATE (BOMB_VISITS, 2);
	}
	else if (PLAYER_SAID (Response, we_get_bomb))
	{
	    NPCPhrase (NOT_GET_BOMB);
	    SET_GAME_STATE (BOMB_VISITS, 3);
	}

	switch (GET_GAME_STATE (BOMB_VISITS))
	{
	    case 1:
		Response (whats_up_at_bomb_planet, BombAmbush);
		break;
	    case 2:
		Response (we_get_bomb, BombAmbush);
		break;
	    default:
		Response (then_we_take_bomb, ExitConversation);
		break;
	}
	Response (bye, ExitConversation);
    }
    POP_CONTEXT
}

PROC(STATIC
void far Space, (Response),
    ARG_END	(RESPONSE_REF	Response)
)
{
    PUSH_CONTEXT
    {
	if (PLAYER_SAID (Response, whats_up_in_space))
	{
	    BYTE	NumVisits;

	    NumVisits = GET_GAME_STATE (DRUUGE_SPACE_INFO);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (GENERAL_INFO_IN_SPACE_1);
		    break;
		case 1:
		    NPCPhrase (GENERAL_INFO_IN_SPACE_2);
		    break;
		case 2:
		    NPCPhrase (GENERAL_INFO_IN_SPACE_3);
		    break;
		case 3:
		    NPCPhrase (GENERAL_INFO_IN_SPACE_4);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (DRUUGE_SPACE_INFO, NumVisits);

	    DISABLE_PHRASE (whats_up_in_space);
	}

	if (PHRASE_ENABLED (whats_up_in_space))
	{
	    Response (whats_up_in_space, Space);
	}
	Response (bye, ExitConversation);
    }
    POP_CONTEXT
}

static void	far
Intro ()
{
    PUSH_CONTEXT
    {
	BYTE	NumVisits;

	if (LOBYTE (GLOBAL (CurrentActivity)) == WON_LAST_BATTLE)
	{
//	    NPCPhrase (OUT_TAKES);

	    SET_GAME_STATE (BATTLE_SEGUE, 0);
	    goto ExitIntro;
	}

	if (GET_GAME_STATE (DRUUGE_MANNER))
	{
	    if (GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 7))
	    {
		NumVisits = GET_GAME_STATE (DRUUGE_HOME_VISITS);
		switch (NumVisits++)
		{
		    case 0:
			NPCPhrase (HSTL_TRADE_WORLD_HELLO_1);
			break;
		    case 1:
			NPCPhrase (HSTL_TRADE_WORLD_HELLO_2);
			--NumVisits;
			break;
		}
		SET_GAME_STATE (DRUUGE_HOME_VISITS, NumVisits);
	    }
	    else
	    {
		NumVisits = GET_GAME_STATE (DRUUGE_VISITS);
		switch (NumVisits++)
		{
		    case 0:
			NPCPhrase (HOSTILE_SPACE_HELLO_1);
			break;
		    case 1:
			NPCPhrase (HOSTILE_SPACE_HELLO_2);
			--NumVisits;
			break;
		}
		SET_GAME_STATE (DRUUGE_VISITS, NumVisits);
	    }

	    SET_GAME_STATE (BATTLE_SEGUE, 1);
	}
	else if (GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 7))
	{
	    {
		NumVisits = GET_GAME_STATE (DRUUGE_HOME_VISITS);
		switch (NumVisits++)
		{
		    case 0:
			NPCPhrase (INITIAL_TRADE_WORLD_HELLO);
			break;
		    case 1:
			NPCPhrase (SSQ_TRADE_WORLD_HELLO_1);
			break;
		    case 2:
			NPCPhrase (SSQ_TRADE_WORLD_HELLO_2);
			break;
		    case 3:
			NPCPhrase (SSQ_TRADE_WORLD_HELLO_3);
			break;
		    case 4:
			NPCPhrase (SSQ_TRADE_WORLD_HELLO_4);
			--NumVisits;
			break;
		}
		SET_GAME_STATE (DRUUGE_HOME_VISITS, NumVisits);
	    }
	    if (GET_GAME_STATE (ATTACKED_DRUUGE)
		    && !GET_GAME_STATE (DRUUGE_DISCLAIMER))
	    {
//		NPCPhrase (HOSTILE_TRADE);
		SET_GAME_STATE (DRUUGE_DISCLAIMER, 1);
	    }
	    if (GET_GAME_STATE (MAIDENS_ON_SHIP)
		    && !GET_GAME_STATE (SCANNED_MAIDENS))
	    {
		NPCPhrase (SCAN_MAIDENS);
		SET_GAME_STATE (SCANNED_MAIDENS, 1);
	    }
	    if ((GET_GAME_STATE (EGG_CASE0_ON_SHIP)
		    || GET_GAME_STATE (EGG_CASE1_ON_SHIP)
		    || GET_GAME_STATE (EGG_CASE2_ON_SHIP))
		    && !GET_GAME_STATE (SCANNED_FRAGMENTS))
	    {
		if (GET_GAME_STATE (FRAGMENTS_BOUGHT) < 2)
		    NPCPhrase (SCAN_FRAGMENTS);
		else
		    NPCPhrase (ENOUGH_FRAGMENTS);
		SET_GAME_STATE (SCANNED_FRAGMENTS, 1);
	    }
	    if (GET_GAME_STATE (BURV_BROADCASTERS_ON_SHIP)
		    && !GET_GAME_STATE (SCANNED_CASTER))
	    {
		NPCPhrase (SCAN_DRUUGE_CASTER);
		SET_GAME_STATE (SCANNED_CASTER, 1);
	    }
	    if (GET_GAME_STATE (PORTAL_SPAWNER_ON_SHIP)
		    && !GET_GAME_STATE (SCANNED_SPAWNER))
	    {
		NPCPhrase (SCAN_ARILOU_SPAWNER);
		SET_GAME_STATE (SCANNED_SPAWNER, 1);
	    }

	    TradeWorld ((RESPONSE_REF)0);
	}
	else if (GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 6))
	{
	    if (GET_GAME_STATE (BOMB_VISITS))
		NPCPhrase (SUBSEQ_BOMB_WORLD_HELLO);
	    else
	    {
		NPCPhrase (INIT_BOMB_WORLD_HELLO);
		SET_GAME_STATE (BOMB_VISITS, 1);
	    }

	    BombAmbush ((RESPONSE_REF)0);
	}
	else if (GET_GAME_STATE (ATTACKED_DRUUGE))
	{
	    NumVisits = GET_GAME_STATE (DRUUGE_VISITS);
	    switch (NumVisits++)
	    {
		case 0:
		    NPCPhrase (HOSTILE_SPACE_HELLO_1);
		    break;
		case 1:
		    NPCPhrase (HOSTILE_SPACE_HELLO_2);
		    --NumVisits;
		    break;
	    }
	    SET_GAME_STATE (DRUUGE_VISITS, NumVisits);

	    SET_GAME_STATE (BATTLE_SEGUE, 1);
	}
	else
	{
	    NumVisits = 0;
	    if (GetHeadLink (&GLOBAL (built_ship_q)) == 0)
	    {
		for (NumVisits = 0; NumVisits < NUM_MODULE_SLOTS; ++NumVisits)
		{
		    BYTE	which_module;

		    which_module = GLOBAL_SIS (ModuleSlots[NumVisits]);
		    if (which_module >= GUN_WEAPON
			    && which_module <= CANNON_WEAPON)
		    {
			NumVisits = 0;
			break;
		    }
		}
	    }

	    if (NumVisits)
	    {
		NumVisits = GET_GAME_STATE (DRUUGE_SALVAGE);
		switch (NumVisits++)
		{
		    case 0:
			NPCPhrase (SALVAGE_YOUR_SHIP_1);
			break;
		    case 1:
			NPCPhrase (SALVAGE_YOUR_SHIP_2);
			--NumVisits;
			break;
		}
		SET_GAME_STATE (DRUUGE_SALVAGE, NumVisits);

		SET_GAME_STATE (BATTLE_SEGUE, 1);
	    }
	    else
	    {
		NumVisits = GET_GAME_STATE (DRUUGE_VISITS);
		switch (NumVisits++)
		{
		    case 0:
			NPCPhrase (INIT_SPACE_HELLO);
			break;
		    case 1:
			NPCPhrase (SUBSEQUENT_SPACE_HELLO);
			--NumVisits;
			break;
		}
		SET_GAME_STATE (DRUUGE_VISITS, NumVisits);

		Space ((RESPONSE_REF)0);
	    }
	}
ExitIntro:
	;
    }
    POP_CONTEXT
}

PROC(STATIC
COUNT far uninit_druuge, (),
    ARG_VOID
)
{
    PUSH_CONTEXT
    {
	if (GET_GAME_STATE (BATTLE_SEGUE) == 1
		&& !GET_GAME_STATE (DRUUGE_MANNER))
	{
	    if (!GET_GAME_STATE (ATTACKED_DRUUGE))
	    {
		SET_GAME_STATE (ATTACKED_DRUUGE, 1);
		SET_GAME_STATE (DRUUGE_VISITS, 0);
	    }
	}
    }
    POP_CONTEXT

    return (0);
}

PROC(
LOCDATAPTR far init_druuge_comm, (),
    ARG_VOID
)
{
    LOCDATAPTR	retval;

    PUSH_CONTEXT
    {
	druuge_desc.init_encounter_func = Intro;
	druuge_desc.uninit_encounter_func = uninit_druuge;

strcpy(aiff_folder, "comm/druuge/druug");

	if ((GET_GAME_STATE (DRUUGE_MANNER) == 0
		&& (GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 7)))
		|| LOBYTE (GLOBAL (CurrentActivity)) == WON_LAST_BATTLE)
	{
	    SET_GAME_STATE (BATTLE_SEGUE, 0);
	}
	else
	{
	    SET_GAME_STATE (BATTLE_SEGUE, 1);
	}
	retval = &druuge_desc;
    }
    POP_CONTEXT

    return (retval);
}
