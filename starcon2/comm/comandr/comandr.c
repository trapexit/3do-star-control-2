#include "reslib.h"
#include "resinst.h"
#include "starcon.h"
#include "commglue.h"
#include "strings.h"

static LOCDATA	commander_desc =
{
    NULL_PTR,				/* init_encounter_func */
    NULL_PTR,				/* uninit_encounter_func */
    (FRAME)COMMANDER_PMAP_ANIM,		/* AlienFrame */
    COMMANDER_COLOR_MAP,		/* AlienColorMap */
    COMMANDER_MUSIC,			/* AlienSong */
    COMMANDER_PLAYER_PHRASES,		/* PlayerPhrases */
    3,					/* NumAnimations */
    {
	{	/* Blink */
	    1,				/* StartIndex */
	    3,				/* NumFrames */
	    YOYO_ANIM,			/* AnimFlags */
	    8, 0,			/* FrameRate */
	    0, ONE_SECOND * 8,		/* RestartRate */
	},
	{	/* Running light */
	    10,				/* StartIndex */
	    30,				/* NumFrames */
	    CIRCULAR_ANIM,		/* AnimFlags */
	    3, 0,			/* FrameRate */
	    ONE_SECOND * 2, 0,		/* RestartRate */
	},
	{
	    1,				/* StartIndex */
	    3,				/* NumFrames */
	    RANDOM_ANIM | COLORXFORM_ANIM,/* AnimFlags */
	    0, 4,			/* FrameRate */
	    0, 8,			/* RestartRate */
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
	4,				/* StartIndex */
	6,				/* NumFrames */
	0,				/* AnimFlags */
	12, 8,				/* FrameRate */
	14, 10,				/* RestartRate */
    },
};

PROC(static
void ByeBye, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	if (PLAYER_SAID (R, ok_i_will_get_radios))
	    NPCPhrase (THANKS_FOR_HELPING);
	else if (PLAYER_SAID (R, well_go_get_them_now))
	    NPCPhrase (GLAD_WHEN_YOU_COME_BACK);
	else if (PLAYER_SAID (R, we_will_take_care_of_base))
	{
	    NPCPhrase (GOOD_LUCK_WITH_BASE);

	    SET_GAME_STATE (WILL_DESTROY_BASE, 1);
	}
	else if (PLAYER_SAID (R, take_care_of_base_again))
	    NPCPhrase (GOOD_LUCK_AGAIN);
	else if (PLAYER_SAID (R, base_was_abandoned)
		|| PLAYER_SAID (R, i_lied_it_was_abandoned))
	{
	    NPCPhrase (IT_WAS_ABANDONED);
	    NPCPhrase (HERE_COMES_ILWRATH);

	    SET_GAME_STATE (PROBE_ILWRATH_ENCOUNTER, 1);
	}
	else if (PLAYER_SAID (R, oh_yes_big_fight))
	{
	    NPCPhrase (IM_GLAD_YOU_WON);
	    NPCPhrase (HERE_COMES_ILWRATH);

	    SET_GAME_STATE (PROBE_ILWRATH_ENCOUNTER, 1);
	}
	else if (PLAYER_SAID (R, i_cant_talk_about_it))
	{
	    NPCPhrase (IM_SURE_IT_WAS_DIFFICULT);
	    NPCPhrase (HERE_COMES_ILWRATH);

	    SET_GAME_STATE (PROBE_ILWRATH_ENCOUNTER, 1);
	}
	else if (PLAYER_SAID (R, cook_their_butts)
		|| PLAYER_SAID (R, overthrow_evil_aliens)
		|| PLAYER_SAID (R, annihilate_those_monsters))
	{
	    PTR_DESC	PtrDesc;

	    SET_GAME_STATE (PROBE_ILWRATH_ENCOUNTER, 0);

	    if (PLAYER_SAID (R, cook_their_butts))
		NPCPhrase (COOK_BUTTS);
	    else if (PLAYER_SAID (R, overthrow_evil_aliens))
		NPCPhrase (OVERTHROW_ALIENS);
	    else /* if (R == annihilate_those_monsters) */
		NPCPhrase (KILL_MONSTERS);

	    construct_response (shared_phrase_buf,
		    name_40,
	    	    GLOBAL_SIS (CommanderName),
		    name_41,
		    0);

	    NPCPhrase (THIS_MAY_SEEM_SILLY);

	    Response (name_1, ByeBye);
	    Response (name_2, ByeBye);
	    Response (name_3, ByeBye);
	    DoResponsePhrase (name_40, ByeBye, shared_phrase_buf);

	    SET_GAME_STATE (STARBASE_AVAILABLE, 1);
	}
	else
	{
	    if (PLAYER_SAID (R, name_1))
	    {
		NPCPhrase (OK_THE_NAFS);

		SET_GAME_STATE (NEW_ALLIANCE_NAME, 0);
	    }
	    else if (PLAYER_SAID (R, name_2))
	    {
		NPCPhrase (OK_THE_CAN);

		SET_GAME_STATE (NEW_ALLIANCE_NAME, 1);
	    }
	    else if (PLAYER_SAID (R, name_3))
	    {
		NPCPhrase (OK_THE_UFW);

		SET_GAME_STATE (NEW_ALLIANCE_NAME, 2);
	    }
	    else /* if (PLAYER_SAID (R, name_4)) */
	    {
		NPCPhrase (OK_THE_NAME_IS_EMPIRE);

		SET_GAME_STATE (NEW_ALLIANCE_NAME, 3);
	    }

	    NPCPhrase (STARBASE_WILL_BE_READY);
	}
    }
    POP_CONTEXT
}

PROC(STATIC
void far NoRadioactives, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	if (PLAYER_SAID (R, yes_this_is_supply_ship))
	{
	    NPCPhrase (ABOUT_TIME);

	    Response (i_lied, NoRadioactives);
	    Response (plumb_out, NoRadioactives);
	}
	else
	{
	    if (PLAYER_SAID (R, where_can_i_get_radios))
	    {
		NPCPhrase (RADIOS_ON_MERCURY);

		DISABLE_PHRASE (where_can_i_get_radios);
	    }
	    else if (PLAYER_SAID (R, no_but_well_help0))
		NPCPhrase (THE_WHAT_FROM_WHERE);
	    else if (PLAYER_SAID (R, what_slave_planet)
		    || PLAYER_SAID (R, i_lied))
		NPCPhrase (DONT_KNOW_WHO_YOU_ARE);
	    else if (PLAYER_SAID (R, plumb_out))
		NPCPhrase (WHAT_KIND_OF_IDIOT);
	    else if (PLAYER_SAID (R, i_lost_my_lander))
	    {
		NPCPhrase (HERE_IS_A_NEW_LANDER);
		++GLOBAL_SIS (NumLanders);
		SetSemaphore (&GraphicsSem);
		DrawLanders ();
		DeltaSISGauges (4, 0, 0);
		ClearSemaphore (&GraphicsSem);

		SET_GAME_STATE (LANDERS_LOST, 1);
	    }
	    else if (PLAYER_SAID (R, i_lost_another_lander))
	    {
		NPCPhrase (HERE_IS_ANOTHER_LANDER);
		++GLOBAL_SIS (NumLanders);
		SetSemaphore (&GraphicsSem);
		DrawLanders ();
		DeltaSISGauges (4, 0, 0);
		ClearSemaphore (&GraphicsSem);
	    }
	    else if (PLAYER_SAID (R, need_fuel))
	    {
		NPCPhrase (GIVE_FUEL);
		SetSemaphore (&GraphicsSem);
		DeltaSISGauges (0, 5 * FUEL_TANK_SCALE, 0);
		ClearSemaphore (&GraphicsSem);

		SET_GAME_STATE (GIVEN_FUEL_BEFORE, 1);
	    }
	    else if (PLAYER_SAID (R, need_fuel_again))
	    {
		NPCPhrase (GIVE_FUEL_AGAIN);
		SetSemaphore (&GraphicsSem);
		DeltaSISGauges (0, 5 * FUEL_TANK_SCALE, 0);
		ClearSemaphore (&GraphicsSem);
	    }

	    if (GLOBAL_SIS (NumLanders) == 0)
	    {
		if (GET_GAME_STATE (LANDERS_LOST))
		    Response (i_lost_another_lander, NoRadioactives);
		else
		    Response (i_lost_my_lander, NoRadioactives);
	    }
	    if (GLOBAL_SIS (FuelOnBoard) < 2 * FUEL_TANK_SCALE)
	    {
		if (GET_GAME_STATE (GIVEN_FUEL_BEFORE))
		    Response (need_fuel_again, NoRadioactives);
		else
		    Response (need_fuel, NoRadioactives);
	    }
	    Response (ok_i_will_get_radios, ByeBye);
	    if (PHRASE_ENABLED (where_can_i_get_radios))
	    {
		Response (where_can_i_get_radios, NoRadioactives);
	    }
	}
    }
    POP_CONTEXT
}

PROC(STATIC
void far AskAfterRadios, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	if (PLAYER_SAID (R, i_lost_my_lander))
	{
	    NPCPhrase (HERE_IS_A_NEW_LANDER);
	    ++GLOBAL_SIS (NumLanders);
	    SetSemaphore (&GraphicsSem);
	    DrawLanders ();
	    DeltaSISGauges (4, 0, 0);
	    ClearSemaphore (&GraphicsSem);

	    SET_GAME_STATE (LANDERS_LOST, 1);
	}
	else if (PLAYER_SAID (R, i_lost_another_lander))
	{
	    NPCPhrase (HERE_IS_ANOTHER_LANDER);
	    ++GLOBAL_SIS (NumLanders);
	    SetSemaphore (&GraphicsSem);
	    DrawLanders ();
	    DeltaSISGauges (4, 0, 0);
	    ClearSemaphore (&GraphicsSem);
	}
	else if (PLAYER_SAID (R, need_fuel))
	{
	    NPCPhrase (GIVE_FUEL);
	    SetSemaphore (&GraphicsSem);
	    DeltaSISGauges (0, 5 * FUEL_TANK_SCALE, 0);
	    ClearSemaphore (&GraphicsSem);

	    SET_GAME_STATE (GIVEN_FUEL_BEFORE, 1);
	}
	else if (PLAYER_SAID (R, need_fuel_again))
	{
	    NPCPhrase (GIVE_FUEL_AGAIN);
	    SetSemaphore (&GraphicsSem);
	    DeltaSISGauges (0, 5 * FUEL_TANK_SCALE, 0);
	    ClearSemaphore (&GraphicsSem);
	}
	else if (PLAYER_SAID (R, where_get_radios))
	{
	    NPCPhrase (RADIOS_ON_MERCURY);

	    DISABLE_PHRASE (where_get_radios);
	}

	{
	    if (GLOBAL_SIS (NumLanders) == 0)
	    {
		if (GET_GAME_STATE (LANDERS_LOST))
		    Response (i_lost_another_lander, AskAfterRadios);
		else
		    Response (i_lost_my_lander, AskAfterRadios);
	    }
	    if (GLOBAL_SIS (FuelOnBoard) < 2 * FUEL_TANK_SCALE)
	    {
		if (GET_GAME_STATE (GIVEN_FUEL_BEFORE))
		    Response (need_fuel_again, AskAfterRadios);
		else
		    Response (need_fuel, AskAfterRadios);
	    }
	    Response (well_go_get_them_now, ByeBye);
	    if (PHRASE_ENABLED (where_get_radios))
	    {
		Response (where_get_radios, AskAfterRadios);
	    }
	}
    }
    POP_CONTEXT
}

PROC(STATIC
void far BaseDestroyed, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	if (PLAYER_SAID (R, we_fought_them))
	{
	    NPCPhrase (YOU_REALLY_FOUGHT_BASE);

	    Response (oh_yes_big_fight, ByeBye);
	    Response (i_lied_it_was_abandoned, ByeBye);
	    Response (i_cant_talk_about_it, ByeBye);
	}
	else
	{
	    if (PLAYER_SAID (R, we_are_here_to_help))
	    {
		NPCPhrase (BASE_ON_MOON);
	    }
	    else
	    {
		NPCPhrase (DEALT_WITH_BASE_YET);
	    }

	    Response (base_was_abandoned, ByeBye);
	    Response (we_fought_them, BaseDestroyed);
	}
    }
    POP_CONTEXT
}

PROC(STATIC
void far TellMoonBase, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	if (R == 0)
	{
	    NPCPhrase (DEALT_WITH_BASE_YET);
	}
	else if (PLAYER_SAID (R, i_lost_my_lander))
	{
	    NPCPhrase (HERE_IS_A_NEW_LANDER);
	    ++GLOBAL_SIS (NumLanders);
	    SetSemaphore (&GraphicsSem);
	    DrawLanders ();
	    DeltaSISGauges (4, 0, 0);
	    ClearSemaphore (&GraphicsSem);

	    SET_GAME_STATE (LANDERS_LOST, 1);
	}
	else if (PLAYER_SAID (R, i_lost_another_lander))
	{
	    NPCPhrase (HERE_IS_ANOTHER_LANDER);
	    ++GLOBAL_SIS (NumLanders);
	    SetSemaphore (&GraphicsSem);
	    DrawLanders ();
	    DeltaSISGauges (4, 0, 0);
	    ClearSemaphore (&GraphicsSem);
	}
	else if (PLAYER_SAID (R, need_fuel))
	{
	    NPCPhrase (GIVE_FUEL);
	    SetSemaphore (&GraphicsSem);
	    DeltaSISGauges (0, 5 * FUEL_TANK_SCALE, 0);
	    ClearSemaphore (&GraphicsSem);

	    SET_GAME_STATE (GIVEN_FUEL_BEFORE, 1);
	}
	else if (PLAYER_SAID (R, need_fuel_again))
	{
	    NPCPhrase (GIVE_FUEL_AGAIN);
	    SetSemaphore (&GraphicsSem);
	    DeltaSISGauges (0, 5 * FUEL_TANK_SCALE, 0);
	    ClearSemaphore (&GraphicsSem);
	}
	else if (PLAYER_SAID (R, we_are_here_to_help))
	{
	    NPCPhrase (BASE_ON_MOON);
	}
	else if (GET_GAME_STATE (STARBASE_YACK_STACK1) == 0)
	{
	    NPCPhrase (ABOUT_BASE);

	    SET_GAME_STATE (STARBASE_YACK_STACK1, 1);
	}
	else
	{
	    NPCPhrase (ABOUT_BASE_AGAIN);
	}

	if (GLOBAL_SIS (NumLanders) == 0)
	{
	    if (GET_GAME_STATE (LANDERS_LOST))
		Response (i_lost_another_lander, TellMoonBase);
	    else
		Response (i_lost_my_lander, TellMoonBase);
	}
	if (GLOBAL_SIS (FuelOnBoard) < 2 * FUEL_TANK_SCALE)
	{
	    if (GET_GAME_STATE (GIVEN_FUEL_BEFORE))
		Response (need_fuel_again, TellMoonBase);
	    else
		Response (need_fuel, TellMoonBase);
	}
	if (GET_GAME_STATE (WILL_DESTROY_BASE) == 0)
	    Response (we_will_take_care_of_base, ByeBye);
	else
	    Response (take_care_of_base_again, ByeBye);
	if (GET_GAME_STATE (STARBASE_YACK_STACK1) == 0)
	    Response (tell_me_about_base, TellMoonBase);
	else
	    Response (tell_me_again, TellMoonBase);
    }
    POP_CONTEXT
}

PROC_LOCAL(
void far RevealSelf, (R),
    ARG_END	(RESPONSE_REF	R)
);

PROC(STATIC
void far TellProbe, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	NPCPhrase (THAT_WAS_PROBE);
	DISABLE_PHRASE (what_was_red_thing);

	Response (it_went_away, RevealSelf);
	Response (we_destroyed_it, RevealSelf);
	Response (what_probe, RevealSelf);
    }
    POP_CONTEXT
}

PROC(STATIC
void far RevealSelf, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	BYTE		i, stack;

	stack = 0;
	if (PLAYER_SAID (R, we_are_vindicator0))
	{
	    NPCPhrase (THATS_IMPOSSIBLE);

	    DISABLE_PHRASE (we_are_vindicator0);
	}
	else if (PLAYER_SAID (R, our_mission_was_secret))
	{
	    NPCPhrase (ACKNOWLEDGE_SECRET);

	    DISABLE_PHRASE (our_mission_was_secret);
	}
	else if (PLAYER_SAID (R, first_give_info))
	{
	    NPCPhrase (ASK_AWAY);

	    stack = 1;
	    DISABLE_PHRASE (first_give_info);
	}
	else if (PLAYER_SAID (R, whats_this_starbase))
	{
	    NPCPhrase (STARBASE_IS);

	    stack = 1;
	    DISABLE_PHRASE (whats_this_starbase);
	}
	else if (PLAYER_SAID (R, what_about_earth))
	{
	    NPCPhrase (HAPPENED_TO_EARTH);

	    stack = 1;
	    DISABLE_PHRASE (what_about_earth);
	}
	else if (PLAYER_SAID (R, where_are_urquan))
	{
	    NPCPhrase (URQUAN_LEFT);

	    stack = 1;
	    DISABLE_PHRASE (where_are_urquan);
	}
	else if (PLAYER_SAID (R, it_went_away))
	    NPCPhrase (DEEP_TROUBLE);
	else if (PLAYER_SAID (R, we_destroyed_it))
	    NPCPhrase (GOOD_NEWS);
	else if (PLAYER_SAID (R, what_probe))
	    NPCPhrase (SURE_HOPE);

	for (i = 0; i < 2; ++i, stack ^= 1)
	{
	    if (stack == 1)
	    {
		if (PHRASE_ENABLED (first_give_info))
		    Response (first_give_info, RevealSelf);
		else if (PHRASE_ENABLED (whats_this_starbase))
		    Response (whats_this_starbase, RevealSelf);
		else if (PHRASE_ENABLED (what_about_earth))
		    Response (what_about_earth, RevealSelf);
		else if (PHRASE_ENABLED (where_are_urquan))
		    Response (where_are_urquan, RevealSelf);
		else if (PHRASE_ENABLED (what_was_red_thing))
		{
		    Response (what_was_red_thing, TellProbe);
		}
	    }
	    else
	    {
		if (PHRASE_ENABLED (we_are_vindicator0))
		{
		    construct_response (shared_phrase_buf,
			    we_are_vindicator0,
		    	    GLOBAL_SIS (CommanderName),
			    we_are_vindicator1,
			    GLOBAL_SIS (ShipName),
			    we_are_vindicator2,
			    0);
		    DoResponsePhrase (we_are_vindicator0, RevealSelf, shared_phrase_buf);
		}
		else if (PHRASE_ENABLED (our_mission_was_secret))
		    Response (our_mission_was_secret, RevealSelf);
		else
		{
		    if (GET_GAME_STATE (MOONBASE_DESTROYED) == 0)
			Response (we_are_here_to_help, TellMoonBase);
		    else
			Response (we_are_here_to_help, BaseDestroyed);
		}
	    }
	}
    }
    POP_CONTEXT
}

PROC(STATIC
void far GiveRadios, (R),
    ARG_END	(RESPONSE_REF	R)
)
{
    PUSH_CONTEXT
    {
	if (PLAYER_SAID (R, we_will_transfer_now))
	{
	    SET_GAME_STATE (RADIOACTIVES_PROVIDED, 1);

	    NPCPhrase (FUEL_UP0);
	    NPCPhrase (FUEL_UP1);
	    
	    AlienTalkSegue (1);
	    SetSemaphore (&GraphicsSem);
	    CommData.AlienAmbientArray[2].AnimFlags |= ANIM_DISABLED;
	    XFormPLUT (GetColorMapAddress (
		    SetAbsColorMapIndex (CommData.AlienColorMap, 0)
		    ), 0);
	    ClearSemaphore (&GraphicsSem);
	    AlienTalkSegue ((COUNT)~0);

	    RevealSelf (NULL_PTR);
	}
	else
	{
	    if (PLAYER_SAID (R, what_will_you_give_us))
		NPCPhrase (MESSAGE_GARBLED_1);
	    else if (PLAYER_SAID (R, before_radios_we_need_info))
		NPCPhrase (MESSAGE_GARBLED_2);

	    Response (we_will_transfer_now, GiveRadios);
	    Response (what_will_you_give_us, GiveRadios);
	    Response (before_radios_we_need_info, GiveRadios);
	}
    }
    POP_CONTEXT
}

PROC(STATIC
void far Intro, (),
    ARG_VOID
)
{
    PUSH_CONTEXT
    {
	if (GET_GAME_STATE (PROBE_ILWRATH_ENCOUNTER))
	{
	    NPCPhrase (VERY_IMPRESSIVE);

	    Response (cook_their_butts, ByeBye);
	    Response (overthrow_evil_aliens, ByeBye);
	    Response (annihilate_those_monsters, ByeBye);
	}
	else if (GET_GAME_STATE (STARBASE_VISITED))
	{
	    if (GET_GAME_STATE (RADIOACTIVES_PROVIDED))
	    {
		if (GET_GAME_STATE (MOONBASE_DESTROYED) == 0)
		{
		    TellMoonBase (NULL_PTR);
		}
		else
		{
		    BaseDestroyed (NULL_PTR);
		}
	    }
	    else
	    {
		CommData.AlienColorMap =
			SetAbsColorMapIndex (CommData.AlienColorMap, 1);
		NPCPhrase (HAVE_RADIO_THIS_TIME);

		if (GLOBAL_SIS (ElementAmounts[RADIOACTIVE]))
		    GiveRadios (NULL_PTR);
		else
		    AskAfterRadios (NULL_PTR);
	    }
	}
	else	/* first visit */
	{
	    CommData.AlienColorMap =
		    SetAbsColorMapIndex (CommData.AlienColorMap, 1);

	    SET_GAME_STATE (STARBASE_VISITED, 1);

	    NPCPhrase (ARE_YOU_SUPPLY_SHIP);
	    if (GLOBAL_SIS (ElementAmounts[RADIOACTIVE]))
		GiveRadios (NULL_PTR);
	    else
	    {
		construct_response (
			shared_phrase_buf,
			no_but_well_help0,
			GLOBAL_SIS (ShipName),
			no_but_well_help1,
			0
			);
		DoResponsePhrase (no_but_well_help0, NoRadioactives, shared_phrase_buf);
		Response (yes_this_is_supply_ship, NoRadioactives);
		Response (what_slave_planet, NoRadioactives);
	    }
	}
    }
    POP_CONTEXT
}

static COUNT	far
uninit_commander ()
{
    PUSH_CONTEXT
    {
    }
    POP_CONTEXT

    return (0);
}

LOCDATAPTR	far
init_commander_comm ()
{
    LOCDATAPTR	retval;

    PUSH_CONTEXT
    {
	commander_desc.init_encounter_func = Intro;
	commander_desc.uninit_encounter_func = uninit_commander;

	if (GET_GAME_STATE (RADIOACTIVES_PROVIDED))
	    commander_desc.AlienAmbientArray[2].AnimFlags |= ANIM_DISABLED;

strcpy(aiff_folder, "comm/comandr/coman");

	SET_GAME_STATE (BATTLE_SEGUE, 0);
	retval = &commander_desc;
    }
    POP_CONTEXT

    return (retval);
}
