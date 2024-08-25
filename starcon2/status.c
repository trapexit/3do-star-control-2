#include <stdio.h>
#include <string.h>
#include "starcon.h"

PROC(STATIC
void near CaptainsWindow,
	(CSPtr, y, delta_status_flags, cur_status_flags, Pass),
    ARG		(CAPTAIN_STUFFPTR		CSPtr)
    ARG		(COORD				y)
    ARG		(register ELEMENT_FLAGS		delta_status_flags)
    ARG		(register ELEMENT_FLAGS		cur_status_flags)
    ARG_END	(COUNT				Pass)
)
{
    STAMP	Stamp;

    Stamp.origin.x = CAPTAIN_XOFFS;
    Stamp.origin.y = y + CAPTAIN_YOFFS;

    if (delta_status_flags & LEFT)
    {
	Stamp.frame = CSPtr->turn;
	if (!(delta_status_flags & RIGHT))
	{
	    Stamp.frame = SetRelFrameIndex (Stamp.frame, 3);
	    if (Pass == 2)
	    {
		if (cur_status_flags & LEFT)
		    Stamp.frame = IncFrameIndex (Stamp.frame);
		else
		    Stamp.frame = DecFrameIndex (Stamp.frame);
	    }
	}
	else if (cur_status_flags & RIGHT)
	{
	    if (Pass == 1)
		Stamp.frame = SetRelFrameIndex (Stamp.frame, 3);
	    else
		Stamp.frame = IncFrameIndex (Stamp.frame);
	    DrawStamp (&Stamp);
	    Stamp.frame = DecFrameIndex (Stamp.frame);
	}
	else
	{
	    if (Pass == 1)
		Stamp.frame = IncFrameIndex (Stamp.frame);
	    else
		Stamp.frame = SetRelFrameIndex (Stamp.frame, 3);
	    DrawStamp (&Stamp);
	    Stamp.frame = IncFrameIndex (Stamp.frame);
	}
	DrawStamp (&Stamp);
    }
    else if (delta_status_flags & RIGHT)
    {
	Stamp.frame = CSPtr->turn;
	Stamp.frame = IncFrameIndex (Stamp.frame);
	if (Pass == 2)
	{
	    if (cur_status_flags & RIGHT)
		Stamp.frame = DecFrameIndex (Stamp.frame);
	    else
		Stamp.frame = IncFrameIndex (Stamp.frame);
	}
	DrawStamp (&Stamp);
    }

    if (delta_status_flags & THRUST)
    {
	Stamp.frame = CSPtr->thrust;
	if (Pass == 1)
	    Stamp.frame = IncFrameIndex (Stamp.frame);
	else if (cur_status_flags & THRUST)
	    Stamp.frame = SetRelFrameIndex (Stamp.frame, 2);
	DrawStamp (&Stamp);
    }
    if (delta_status_flags & WEAPON)
    {
	Stamp.frame = CSPtr->weapon;
	if (Pass == 1)
	    Stamp.frame = IncFrameIndex (Stamp.frame);
	else if (cur_status_flags & WEAPON)
	    Stamp.frame = SetRelFrameIndex (Stamp.frame, 2);
	DrawStamp (&Stamp);
    }
    if (delta_status_flags & SPECIAL)
    {
	Stamp.frame = CSPtr->special;
	if (Pass == 1)
	    Stamp.frame = IncFrameIndex (Stamp.frame);
	else if (cur_status_flags & SPECIAL)
	    Stamp.frame = SetRelFrameIndex (Stamp.frame, 2);
	DrawStamp (&Stamp);
    }
}

PROC(
void DrawBattleCrewAmount, (CountPlayer),
    ARG_END	(BOOLEAN	CountPlayer)
)
{
#define MAX_CREW_DIGITS	3
    RECT	r;
    TEXT	t;
    char	buf[10];

    t.baseline.x = 10;
    t.baseline.y = 168;
    t.align = ALIGN_LEFT;
    t.lpStr = (LPBYTE)buf;
    t.CharCount = (COUNT)~0;

    r.corner.x = t.baseline.x;
    r.corner.y = t.baseline.y - 5;
    r.extent.width = 6 * MAX_CREW_DIGITS + 1;
    r.extent.height = 5;

    sprintf (buf, "%u",
	    GLOBAL_SIS (CrewEnlisted)
	    + (CountPlayer ? 1 : 0));
    SetContextFont (StarConFont);

    SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0xA, 0xA, 0xA), 0x08));
    DrawFilledRectangle (&r);
    SetContextForeGroundColor (BLACK_COLOR);
    DrawText (&t);
}

PROC(
void DrawCaptainsWindow, (StarShipPtr),
    ARG_END	(STARSHIPPTR	StarShipPtr)
)
{
    COORD		y, y_offs;
    RECT		r;
    STAMP		s;
    FRAME		Frame;
    RACE_DESCPTR	RDPtr;

    RDPtr = StarShipPtr->RaceDescPtr;
    Frame = RDPtr->ship_data.captain_control.background;
    if (Frame)
    {
	Frame = SetAbsFrameIndex (Frame, 0);
	RDPtr->ship_data.captain_control.background = Frame;
	Frame = SetRelFrameIndex (Frame, 1);
	RDPtr->ship_data.captain_control.turn = Frame;
	Frame = SetRelFrameIndex (Frame, 5);
	RDPtr->ship_data.captain_control.thrust = Frame;
	Frame = SetRelFrameIndex (Frame, 3);
	RDPtr->ship_data.captain_control.weapon = Frame;
	Frame = SetRelFrameIndex (Frame, 3);
	RDPtr->ship_data.captain_control.special = Frame;
    }

    BatchGraphics ();
    
    y_offs = CAPTAIN_YOFFS
	    + ((RDPtr->ship_info.ship_flags & GOOD_GUY) ?
	    GOOD_GUY_YOFFS : BAD_GUY_YOFFS);
    r.corner.x = CAPTAIN_XOFFS - 2;
    r.corner.y = y_offs - 4;
    r.extent.width = STATUS_WIDTH - CAPTAIN_XOFFS;
    r.extent.height = SHIP_STATUS_HEIGHT - CAPTAIN_YOFFS + 2;
    SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0xA, 0xA, 0xA), 0x08));
    DrawFilledRectangle (&r);

    y = y_offs - CAPTAIN_YOFFS;

    SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x8, 0x8, 0x8), 0x1F));
    r.corner.x = 1;
    r.corner.y = SHIP_INFO_HEIGHT + y;
    r.extent.width = 1;
    r.extent.height = (SHIP_STATUS_HEIGHT - SHIP_INFO_HEIGHT - 2);
    DrawFilledRectangle (&r);
    r.corner.x = 0;
    ++r.extent.height;
    DrawFilledRectangle (&r);

    SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x10, 0x10, 0x10), 0x19));
    r.corner.x = STATUS_WIDTH - 1;
    r.corner.y = SHIP_INFO_HEIGHT + y;
    r.extent.width = 1;
    r.extent.height = SHIP_STATUS_HEIGHT - SHIP_INFO_HEIGHT;
    DrawFilledRectangle (&r);
    r.corner.x = STATUS_WIDTH - 2;
    DrawFilledRectangle (&r);
    r.corner.x = 1;
    r.extent.width = STATUS_WIDTH - 2;
    r.corner.y = (SHIP_STATUS_HEIGHT - 2) + y;
    r.extent.height = 1;
    DrawFilledRectangle (&r);
    r.corner.x = 0;
    ++r.extent.width;
    ++r.corner.y;
    DrawFilledRectangle (&r);

    {
	SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x8, 0x8, 0x8), 0x1F));
	r.corner.x = 59;
	r.corner.y = y_offs;
	r.extent.width = 1;
	r.extent.height = 30;
	DrawFilledRectangle (&r);
	r.corner.x = 3;
	r.corner.y += 30;
	r.extent.width = 57;
	r.extent.height = 1;
	DrawFilledRectangle (&r);

	SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x10, 0x10, 0x10), 0x19));
	r.corner.x = 3;
	r.extent.width = 57;
	r.corner.y = y_offs - 1;
	r.extent.height = 1;
	DrawFilledRectangle (&r);
	r.corner.x = 3;
	r.extent.width = 1;
	r.corner.y = y_offs;
	r.extent.height = 30;
	DrawFilledRectangle (&r);

	s.frame = RDPtr->ship_data.captain_control.background;
	s.origin.x = CAPTAIN_XOFFS;
	s.origin.y = y_offs;
	DrawStamp (&s);
    }

    if (StarShipPtr->captains_name_index == 0)
    {
	if (RDPtr->ship_info.ship_flags & GOOD_GUY)
	{
	    TEXT	t;

	    t.baseline.x = STATUS_WIDTH >> 1;
	    t.baseline.y = y_offs + 6;
	    t.align = ALIGN_CENTER;
	    t.lpStr = (LPBYTE)GLOBAL_SIS (CommanderName);
	    t.CharCount = (COUNT)~0;
	    DrawBattleCrewAmount (TRUE);
	    SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x00, 0x14, 0x00), 0x02));
	    SetContextFont (TinyFont);
	    DrawText (&t);
	}
    }

    UnbatchGraphics ();
}

PROC(
BOOLEAN	DeltaEnergy, (ElementPtr, energy_delta),
    ARG		(ELEMENTPTR	ElementPtr)
    ARG_END	(SIZE		energy_delta)
)
{
    BOOLEAN		retval;
    STARSHIPPTR		StarShipPtr;
    SHIP_INFOPTR	ShipInfoPtr;

    retval = TRUE;

    GetElementStarShip (ElementPtr, &StarShipPtr);
    ShipInfoPtr = &StarShipPtr->RaceDescPtr->ship_info;
    if (energy_delta >= 0)
    {
	if ((BYTE)(ShipInfoPtr->energy_level + (BYTE)energy_delta) >
		ShipInfoPtr->max_energy)
	    energy_delta = ShipInfoPtr->max_energy
		    - ShipInfoPtr->energy_level;
    }
    else
    {
	if ((BYTE)-energy_delta > ShipInfoPtr->energy_level)
	{
	    retval = FALSE;
	}
    }

    if (!retval)
	StarShipPtr->cur_status_flags |= LOW_ON_ENERGY;
    else
    {
	StarShipPtr->cur_status_flags &= ~LOW_ON_ENERGY;
	StarShipPtr->energy_counter =
		StarShipPtr->RaceDescPtr->characteristics.energy_wait;

	DeltaStatistics (StarShipPtr, 0, energy_delta);
    }

    return (retval);
}

PROC(
BOOLEAN	DeltaCrew, (ElementPtr, crew_delta),
    ARG		(ELEMENTPTR	ElementPtr)
    ARG_END	(SIZE		crew_delta)
)
{
    BOOLEAN	retval;
    STARSHIPPTR	StarShipPtr;

    if (LOBYTE (GLOBAL (CurrentActivity)) == IN_LAST_BATTLE
	    && (ElementPtr->state_flags & BAD_GUY))
	return (TRUE);	/* Samatra can't be crew-modified */

    retval = TRUE;
    GetElementStarShip (ElementPtr, &StarShipPtr);
    if (crew_delta > 0)
    {
	SHIP_INFOPTR	ShipInfoPtr;

	ShipInfoPtr = &StarShipPtr->RaceDescPtr->ship_info;
	if ((ElementPtr->crew_level += (BYTE)crew_delta) >
		    ShipInfoPtr->max_crew)
	{
	    crew_delta = ShipInfoPtr->max_crew
		    - ShipInfoPtr->crew_level;
	    ElementPtr->crew_level =
		    ShipInfoPtr->max_crew;
	}
    }
    else if (crew_delta < 0)
    {
	if (ElementPtr->crew_level > (BYTE)-crew_delta)
	    ElementPtr->crew_level += (BYTE)crew_delta;
	else
	{
	    crew_delta = -(SIZE)ElementPtr->crew_level;
	    ElementPtr->crew_level = 0;
	    retval = FALSE;
	}
    }

    DeltaStatistics (StarShipPtr, crew_delta, 0);

    return (retval);
}

PROC(
void PreProcessStatus, (ShipPtr),
    ARG_END	(LPELEMENT	ShipPtr)
)
{
    STARSHIPPTR		StarShipPtr;

    GetElementStarShip (ShipPtr, &StarShipPtr);
    if (StarShipPtr->captains_name_index
	    || (StarShipPtr->RaceDescPtr->ship_info.ship_flags & GOOD_GUY))
    {
	register ELEMENT_FLAGS	old_status_flags, cur_status_flags;
	CAPTAIN_STUFFPTR	CSPtr;

	cur_status_flags = StarShipPtr->cur_status_flags;
	old_status_flags = StarShipPtr->old_status_flags;
	old_status_flags ^= cur_status_flags;

	CSPtr = &StarShipPtr->RaceDescPtr->ship_data.captain_control;
	old_status_flags &= (LEFT | RIGHT | THRUST | WEAPON | SPECIAL);
	if (old_status_flags)
	{
	    CaptainsWindow (CSPtr,
		    (StarShipPtr->RaceDescPtr->ship_info.ship_flags & GOOD_GUY) ?
		    GOOD_GUY_YOFFS : BAD_GUY_YOFFS,
		    old_status_flags, cur_status_flags, 1);
	}
    }
}

PROC(
void PostProcessStatus, (ShipPtr),
    ARG_END	(LPELEMENT	ShipPtr)
)
{
    STARSHIPPTR		StarShipPtr;

    GetElementStarShip (ShipPtr, &StarShipPtr);
    if (StarShipPtr->captains_name_index
	    || (StarShipPtr->RaceDescPtr->ship_info.ship_flags & GOOD_GUY))
    {
	COORD			y;
	register ELEMENT_FLAGS	cur_status_flags, old_status_flags;


	cur_status_flags = StarShipPtr->cur_status_flags;

	y = (StarShipPtr->RaceDescPtr->ship_info.ship_flags & GOOD_GUY) ?
		GOOD_GUY_YOFFS : BAD_GUY_YOFFS;

	if (ShipPtr->crew_level == 0)
	{
	    StarShipPtr->cur_status_flags &=
		    ~(LEFT | RIGHT | THRUST | WEAPON | SPECIAL);

	    if (StarShipPtr->RaceDescPtr->ship_info.crew_level == 0)
	    {
#define CAPTAIN_WIDTH	55
#define CAPTAIN_HEIGHT	30
		BYTE	i;
		COLOR	c;
		RECT	r;

		i = (BYTE)(NUM_EXPLOSION_FRAMES * 3 - 1) - ShipPtr->life_span;
		if (i <= 4)
		{
		    COLOR	flash_tab0[] =
		    {
			BUILD_COLOR (MAKE_RGB15 (0xF, 0x00, 0x00), 0x2D),
			BUILD_COLOR (MAKE_RGB15 (0x1F, 0x19, 0x19), 0x24),
			BUILD_COLOR (MAKE_RGB15 (0x1F, 0x11, 0x00), 0x7B),
			BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1C, 0x00), 0x78),
			BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1F, 0x1F), 0x0F),
		    };

		    c = flash_tab0[i];
		    r.corner.x = CAPTAIN_XOFFS;
		    r.corner.y = y + CAPTAIN_YOFFS;
		    r.extent.width = CAPTAIN_WIDTH;
		    r.extent.height = CAPTAIN_HEIGHT;
		}
		else
		{
		    SetContextForeGroundColor (BLACK_COLOR);
		    if ((i -= 5) <= 14)
		    {
			COLOR	flash_tab1[] =
			{
			    BUILD_COLOR (MAKE_RGB15 (0x1E, 0x1F, 0x12), 0x70),
			    BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1F, 0xA), 0x0E),
			    BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1F, 0x00), 0x71),
			    BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1C, 0x00), 0x78),
			    BUILD_COLOR (MAKE_RGB15 (0x1F, 0x18, 0x00), 0x79),
			    BUILD_COLOR (MAKE_RGB15 (0x1F, 0x15, 0x00), 0x7A),
			    BUILD_COLOR (MAKE_RGB15 (0x1F, 0x11, 0x00), 0x7B),
			    BUILD_COLOR (MAKE_RGB15 (0x1F, 0xE, 0x00), 0x7C),
			    BUILD_COLOR (MAKE_RGB15 (0x1F, 0xA, 0x00), 0x7D),
			    BUILD_COLOR (MAKE_RGB15 (0x1F, 0x7, 0x00), 0x7E),
			    BUILD_COLOR (MAKE_RGB15 (0x1F, 0x3, 0x00), 0x7F),
			    BUILD_COLOR (MAKE_RGB15 (0x1B, 0x00, 0x00), 0x2A),
			    BUILD_COLOR (MAKE_RGB15 (0x17, 0x00, 0x00), 0x2B),
			    BUILD_COLOR (MAKE_RGB15 (0x13, 0x00, 0x00), 0x2C),
			    BUILD_COLOR (MAKE_RGB15 (0xF, 0x00, 0x00), 0x2D),
			};

			c = flash_tab1[i];
			r.corner.x = CAPTAIN_XOFFS + i;
			r.corner.y = y + CAPTAIN_YOFFS + i;
			r.extent.width = CAPTAIN_WIDTH - (i << 1);
			r.extent.height = CAPTAIN_HEIGHT - (i << 1);
			if (r.extent.height == 2)
			    ++r.extent.height;
			DrawRectangle (&r);
			++r.corner.x;
			++r.corner.y;
			r.extent.width -= 2;
			r.extent.height -= 2;
		    }
		    else if ((i -= 15) <= 4)
		    {
			r.corner.y = y + (CAPTAIN_YOFFS + 15);
			r.extent.width = i + 1;
			r.extent.height = 1;
			switch (i)
			{
			    case 0:
				r.corner.x = CAPTAIN_XOFFS + 15;
				i = CAPTAIN_WIDTH - ((15 + 1) << 1);
				c = BUILD_COLOR (MAKE_RGB15 (0x13, 0x00, 0x00), 0x2C);
				break;
			    case 1:
				r.corner.x = CAPTAIN_XOFFS + 16;
				i = CAPTAIN_WIDTH - ((17 + 1) << 1);
				c = BUILD_COLOR (MAKE_RGB15 (0x7, 0x00, 0x00), 0x2F);
				break;
			    case 2:
				r.corner.x = CAPTAIN_XOFFS + 18;
				i = CAPTAIN_WIDTH - ((20 + 1) << 1);
				c = BUILD_COLOR (MAKE_RGB15 (0x1B, 0x00, 0x00), 0x2A);
				break;
			    case 3:
				r.corner.x = CAPTAIN_XOFFS + 21;
				i = CAPTAIN_WIDTH - ((24 + 1) << 1);
				c = BUILD_COLOR (MAKE_RGB15 (0x1F, 0x00, 0x00), 0x29);
				break;
			    case 4:
				r.corner.x = CAPTAIN_XOFFS + 25;
				i = 1;
				r.extent.width = 2;
				c = BUILD_COLOR (MAKE_RGB15 (0x1F, 0x50, 0x5), 0x28);
				break;
			}
			DrawFilledRectangle (&r);
			r.corner.x += i + r.extent.width;
			DrawFilledRectangle (&r);
			r.corner.x -= i;
			r.extent.width = i;
		    }
		    else
		    {
			if ((i -= 5) > 2)
			    c = BLACK_COLOR;
			else
			{
			    COLOR	flash_tab2[] =
			    {
				BUILD_COLOR (MAKE_RGB15 (0x17, 0x00, 0x00), 0x2B),
				BUILD_COLOR (MAKE_RGB15 (0xF, 0x00, 0x00), 0x2D),
				BUILD_COLOR (MAKE_RGB15 (0xB, 0x00, 0x00), 0x2E),
			    };

			    c = flash_tab2[i];
			}
			r.corner.x = CAPTAIN_XOFFS
				+ (CAPTAIN_WIDTH >> 1);
			r.corner.y = y + CAPTAIN_YOFFS
				 + ((CAPTAIN_HEIGHT + 1) >> 1);
			r.extent.width = 1;
			r.extent.height = 1;
		    }
		}
		SetContextForeGroundColor (c);
		DrawFilledRectangle (&r);
	    }
	}

	old_status_flags = StarShipPtr->old_status_flags;
	old_status_flags = (old_status_flags ^ cur_status_flags) &
		(LEFT | RIGHT | THRUST | WEAPON | SPECIAL | LOW_ON_ENERGY);

	if (old_status_flags)
	{
	    if (old_status_flags & LOW_ON_ENERGY)
	    {
		if (!(cur_status_flags & LOW_ON_ENERGY))
		    DrawCrewFuelString (y, 1);
		else
		    DrawCrewFuelString (y, -1);
	    }

	    old_status_flags &= (LEFT | RIGHT | THRUST | WEAPON | SPECIAL);
	    if (old_status_flags)
	    {
		CaptainsWindow (
			&StarShipPtr->RaceDescPtr->ship_data.captain_control,
			y, old_status_flags, cur_status_flags, 2);
	    }
	}

	StarShipPtr->old_status_flags = cur_status_flags;
    }
}

