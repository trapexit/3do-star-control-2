#include "starcon.h"

PROC(
void DrawCrewFuelString, (y, state),
    ARG		(COORD	y)
    ARG_END	(SIZE	state)
)
{
    STAMP		Stamp;
    extern FRAME	status;

    Stamp.origin.y = y + GAUGE_YOFFS + STARCON_TEXT_HEIGHT;
    if (state == 0)
    {
	Stamp.origin.x = CREW_XOFFS + (STAT_WIDTH >> 1) + 6;
	Stamp.frame = SetAbsFrameIndex (status, 0);
	DrawStamp (&Stamp);
    }

    Stamp.origin.x = ENERGY_XOFFS + (STAT_WIDTH >> 1) - 5;
    Stamp.frame = SetAbsFrameIndex (status, 1);
    if (state >= 0)
	DrawStamp (&Stamp);
    else
    {
#define LOW_FUEL_COLOR		BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1F, 0xA), 0x0E)
	SetContextForeGroundColor (LOW_FUEL_COLOR);
	DrawFilledStamp (&Stamp);
    }
}

PROC(STATIC
void near DrawShipNameString, (lpStr, CharCount, y),
    ARG		(LPSTR	lpStr)
    ARG		(COUNT	CharCount)
    ARG_END	(COORD	y)
)
{
    TEXT	Text;
    FONT	OldFont;

    OldFont = SetContextFont (StarConFont);

    Text.lpStr = (LPBYTE)lpStr;
    Text.CharCount = CharCount;
    Text.align = ALIGN_CENTER;

    Text.baseline.y = STARCON_TEXT_HEIGHT + 3 + y;
    Text.baseline.x = STATUS_WIDTH >> 1;

    SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x10, 0x10, 0x10), 0x19));
    DrawText (&Text);
    --Text.baseline.y;
    SetContextForeGroundColor (BLACK_COLOR);
    DrawText (&Text);

    SetContextFont (OldFont);
}

PROC(
void ClearShipStatus, (y),
    ARG_END	(COORD	y)
)
{
    RECT	r;

    SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0xA, 0xA, 0xA), 0x08));
    r.corner.x = 2;
    r.corner.y = 3 + y;
    r.extent.width = STATUS_WIDTH - 4;
    r.extent.height = SHIP_INFO_HEIGHT - 3;
    DrawFilledRectangle (&r);
}

PROC(
void OutlineShipStatus, (y),
    ARG_END	(COORD	y)
)
{
    RECT	r;

    SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x8, 0x8, 0x8), 0x1F));
    r.corner.x = 0;
    r.corner.y = 1 + y;
    r.extent.width = STATUS_WIDTH;
    r.extent.height = 1;
    DrawFilledRectangle (&r);
    ++r.corner.y;
    --r.extent.width;
    DrawFilledRectangle (&r);
    r.extent.width = 1;
    r.extent.height = SHIP_INFO_HEIGHT - 2;
    DrawFilledRectangle (&r);
    ++r.corner.x;
    DrawFilledRectangle (&r);

    SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x10, 0x10, 0x10), 0x19));
    r.corner.x = STATUS_WIDTH - 1;
    DrawFilledRectangle (&r);
    r.corner.x = STATUS_WIDTH - 2;
    ++r.corner.y;
    --r.extent.height;
    DrawFilledRectangle (&r);

    SetContextForeGroundColor (BLACK_COLOR);
    r.corner.x = 0;
    r.corner.y = y;
    r.extent.width = STATUS_WIDTH;
    r.extent.height = 1;
    DrawFilledRectangle (&r);
}

PROC(
void InitShipStatus, (StarShipPtr, pClipRect),
    ARG		(STARSHIPPTR	StarShipPtr)
    ARG_END	(PRECT		pClipRect)
)
{
    RECT		r;
    COORD		y, y_stat;
    STAMP		Stamp;
    CONTEXT		OldContext;
    FRAME		Frame;
    SHIP_INFOPTR	SIPtr;

    SIPtr = &StarShipPtr->RaceDescPtr->ship_info;
    y_stat = (SIPtr->ship_flags & GOOD_GUY) ?
	    GOOD_GUY_YOFFS : BAD_GUY_YOFFS;

#ifdef NOTYET
    Frame = pClipRect || (GLOBAL (CurrentActivity) & IN_BATTLE) ? 0 : CaptureDrawable (
	    CreateDrawable (WANT_PIXMAP, STATUS_WIDTH, SHIP_INFO_HEIGHT, 1)
	    );
#else /* YET */
    Frame = 0;
#endif /* NOTYET */
    if (Frame)
    {
	OldContext = SetContext (OffScreenContext);
	SetContextFGFrame (Frame);
	y = 0;
    }
    else
    {
	OldContext = SetContext (StatusContext);
	y = y_stat;
	if (pClipRect)
	{
	    GetContextClipRect (&r);
	    r.corner.x += pClipRect->corner.x;
	    r.corner.y += (pClipRect->corner.y & ~1);
	    r.extent = pClipRect->extent;
	    r.extent.height += pClipRect->corner.y & 1;
	    SetContextClipRect (&r);
	    SetFrameHot (Screen, MAKE_HOT_SPOT (
		    pClipRect->corner.x,
		    (pClipRect->corner.y & ~1)
		    ));
	}
    }

    BatchGraphics ();
    
    OutlineShipStatus (y);
    ClearShipStatus (y);

    Stamp.origin.x = (STATUS_WIDTH >> 1);
    Stamp.origin.y = 31 + y;
    Stamp.frame = IncFrameIndex (SIPtr->icons);
    DrawStamp (&Stamp);

    {
	SIZE	crew_height, energy_height;

	crew_height = (((SIPtr->max_crew + 1) >> 1) << 1) + 1;
	energy_height = (((SIPtr->max_energy + 1) >> 1) << 1) + 1;

	SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x8, 0x8, 0x8), 0x1F));
	r.corner.x = CREW_XOFFS - 1;
	r.corner.y = GAUGE_YOFFS + 1 + y;
	r.extent.width = STAT_WIDTH + 2;
	r.extent.height = 1;
	DrawFilledRectangle (&r);
	r.corner.x = ENERGY_XOFFS - 1;
	DrawFilledRectangle (&r);
	r.corner.x = ENERGY_XOFFS + STAT_WIDTH;
	r.corner.y -= energy_height;
	r.extent.width = 1;
	r.extent.height = energy_height;
	DrawFilledRectangle (&r);
	r.corner.x = CREW_XOFFS + STAT_WIDTH;
	r.corner.y = (GAUGE_YOFFS + 1 + y) - crew_height;
	r.extent.width = 1;
	r.extent.height = crew_height;
	DrawFilledRectangle (&r);
	SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x10, 0x10, 0x10), 0x19));
	r.corner.x = CREW_XOFFS - 1;
	r.corner.y = GAUGE_YOFFS - crew_height + y;
	r.extent.width = STAT_WIDTH + 2;
	r.extent.height = 1;
	DrawFilledRectangle (&r);
	r.corner.x = ENERGY_XOFFS - 1;
	r.corner.y = GAUGE_YOFFS - energy_height + y;
	DrawFilledRectangle (&r);
	r.extent.width = 1;
	r.extent.height = energy_height + 1;
	DrawFilledRectangle (&r);
	r.corner.x = CREW_XOFFS - 1;
	r.corner.y = GAUGE_YOFFS - crew_height + y;
	r.extent.height = crew_height + 1;
	DrawFilledRectangle (&r);

	SetContextForeGroundColor (BLACK_COLOR);

	r.extent.width = STAT_WIDTH;
	r.corner.x = CREW_XOFFS;
	r.extent.height = crew_height;
	r.corner.y = y - r.extent.height + GAUGE_YOFFS + 1;
	DrawFilledRectangle (&r);
	r.corner.x = ENERGY_XOFFS;
	r.extent.height = energy_height;
	r.corner.y = y - r.extent.height + GAUGE_YOFFS + 1;
	DrawFilledRectangle (&r);
    }

    if (StarShipPtr->captains_name_index
	    || LOBYTE (GLOBAL (CurrentActivity)) == SUPER_MELEE)
    {
	STRING	locString;

	DrawCrewFuelString (y, 0);

	locString = SetAbsStringTableIndex (SIPtr->race_strings, 1);
	DrawShipNameString (
		(LPSTR)GetStringAddress (locString),
		GetStringLength (locString),
		y
		);

	{
	    char	buf[3];
	    TEXT	Text;
	    FONT	OldFont;

	    OldFont = SetContextFont (TinyFont);

	    if (!(GLOBAL (CurrentActivity) & IN_BATTLE))
	    {
		sprintf (buf, "%d", SIPtr->ship_cost);
		Text.lpStr = (LPBYTE)buf;
		Text.CharCount = (COUNT)~0;
	    }
	    else
	    {
		locString = SetAbsStringTableIndex (SIPtr->race_strings,
			StarShipPtr->captains_name_index);
		Text.lpStr = GetStringAddress (locString);
		Text.CharCount = GetStringLength (locString);
	    }
	    Text.align = ALIGN_CENTER;

	    Text.baseline.x = STATUS_WIDTH >> 1;
	    Text.baseline.y = y + GAUGE_YOFFS + 9;

	    SetContextForeGroundColor (BLACK_COLOR);
	    DrawText (&Text);

	    SetContextFont (OldFont);
	}
    }
    else if (StarShipPtr->captains_name_index == 0)
    {
	if (SIPtr->ship_flags & GOOD_GUY)
	{
	    DrawCrewFuelString (y, 0);
	    DrawShipNameString (
		    (LPSTR)GLOBAL_SIS (ShipName), (COUNT)~0, y
		    );
	}
    }

    SetContext (StatusContext);

    if (Frame)
    {
	Stamp.origin.x = 0;
	Stamp.origin.y = y_stat;
	Stamp.frame = Frame;
	DrawStamp (&Stamp);
	DestroyDrawable (ReleaseDrawable (Frame));
    }

    {
	SIZE	crew_delta, energy_delta;

	crew_delta = SIPtr->crew_level;
	energy_delta = SIPtr->energy_level;
	SIPtr->crew_level = SIPtr->energy_level = 0;
	DeltaStatistics (StarShipPtr, crew_delta, energy_delta);
    }

    UnbatchGraphics ();

    if (pClipRect)
    {
	SetFrameHot (Screen, MAKE_HOT_SPOT (0, 0));
	r.corner.x = SPACE_WIDTH + SAFE_X;
	r.corner.y = SAFE_Y;
	r.extent.width = STATUS_WIDTH;
	r.extent.height = STATUS_HEIGHT;
	SetContextClipRect (&r);
    }

    SetContext (OldContext);
}

PROC(
void DeltaStatistics, (StarShipPtr, crew_delta, energy_delta),
    ARG		(STARSHIPPTR	StarShipPtr)
    ARG		(SIZE		crew_delta)
    ARG_END	(SIZE		energy_delta)
)
{
    COORD		x, y;
    RECT		r;
    SHIP_INFOPTR	ShipInfoPtr;

    if (crew_delta == 0 && energy_delta == 0)
	return;

    ShipInfoPtr = &StarShipPtr->RaceDescPtr->ship_info;

    x = 0;
    y = GAUGE_YOFFS
	    + ((ShipInfoPtr->ship_flags & GOOD_GUY) ?
	    GOOD_GUY_YOFFS : BAD_GUY_YOFFS);

    r.extent.width = UNIT_WIDTH;
    r.extent.height = UNIT_HEIGHT;
    if (crew_delta != 0)
    {
	if (crew_delta > 0)
	{
	    r.corner.y = (y + 1) -
		    (((ShipInfoPtr->crew_level + 1) >> 1) * (UNIT_HEIGHT + 1));
	    if (StarShipPtr->captains_name_index == 0
		    && (ShipInfoPtr->ship_flags & GOOD_GUY)
		    && LOBYTE (GLOBAL (CurrentActivity)) != SUPER_MELEE)
	    {
		if ((crew_delta =
			GLOBAL_SIS (CrewEnlisted)) >= MAX_CREW_SIZE)
		    crew_delta = MAX_CREW_SIZE;
		else /* if (crew_delta < MAX_CREW_SIZE) */
		{
#define PLAYER_UNIT_COLOR	BUILD_COLOR (MAKE_RGB15 (0xA, 0xA, 0x1F), 0x09)
		    SetContextForeGroundColor (PLAYER_UNIT_COLOR);
		    r.corner.x = x + (CREW_XOFFS + 1);
		    if (!(ShipInfoPtr->crew_level & 1))
		    {
			r.corner.x += UNIT_WIDTH + 1;
			r.corner.y -= UNIT_HEIGHT + 1;
		    }
		    DrawFilledRectangle (&r);
		    ++ShipInfoPtr->crew_level;
		}
	    }

#define CREW_UNIT_COLOR		BUILD_COLOR (MAKE_RGB15 (0x00, 0x14, 0x00), 0x02)
#define ROBOT_UNIT_COLOR	BUILD_COLOR (MAKE_RGB15 (0xA, 0xA, 0xA), 0x08)
	    SetContextForeGroundColor (
		    (ShipInfoPtr->ship_flags & CREW_IMMUNE) ?
		    ROBOT_UNIT_COLOR : CREW_UNIT_COLOR
		    );
	    while (crew_delta--)
	    {
		r.corner.x = x + (CREW_XOFFS + 1);
		if (!(ShipInfoPtr->crew_level & 1))
		{
		    r.corner.x += UNIT_WIDTH + 1;
		    r.corner.y -= UNIT_HEIGHT + 1;
		}
		DrawFilledRectangle (&r);
		++ShipInfoPtr->crew_level;
	    }
	}
	else
	{
	    if (StarShipPtr->captains_name_index == 0
		    && (ShipInfoPtr->ship_flags & GOOD_GUY)
		    && LOBYTE (GLOBAL (CurrentActivity)) != SUPER_MELEE)
	    {
		COUNT	death_count;

		death_count = (COUNT)-crew_delta;
		if (GLOBAL_SIS (CrewEnlisted) >= death_count)
		{
		    GLOBAL_SIS (CrewEnlisted) -= death_count;
		    death_count = 0;
		}
		else
		{
		    death_count -= GLOBAL_SIS (CrewEnlisted);
		    GLOBAL_SIS (CrewEnlisted) = 0;
		}

		DrawBattleCrewAmount ((BOOLEAN)(death_count == 0));
	    }

	    SetContextForeGroundColor (BLACK_COLOR);
	    r.corner.y = (y + 1) -
		    (((ShipInfoPtr->crew_level + 2) >> 1) * (UNIT_HEIGHT + 1));
	    do
	    {
		r.corner.x = x + (CREW_XOFFS + 1 + UNIT_WIDTH + 1);
		if (!(ShipInfoPtr->crew_level & 1))
		{
		    r.corner.x -= UNIT_WIDTH + 1;
		    r.corner.y += UNIT_HEIGHT + 1;
		}
		DrawFilledRectangle (&r);
		--ShipInfoPtr->crew_level;
	    } while (++crew_delta);
	}
    }

    if (energy_delta != 0)
    {
	if (energy_delta > 0)
	{
#define FUEL_UNIT_COLOR	BUILD_COLOR (MAKE_RGB15 (0x14, 0x00, 0x00), 0x04)
	    SetContextForeGroundColor (FUEL_UNIT_COLOR);
	    r.corner.y = (y + 1) -
		    (((ShipInfoPtr->energy_level + 1) >> 1) * (UNIT_HEIGHT + 1));
	    do
	    {
		r.corner.x = x + (ENERGY_XOFFS + 1);
		if (!(ShipInfoPtr->energy_level & 1))
		{
		    r.corner.x += UNIT_WIDTH + 1;
		    r.corner.y -= UNIT_HEIGHT + 1;
		}
		DrawFilledRectangle (&r);
		++ShipInfoPtr->energy_level;
	    } while (--energy_delta);
	}
	else
	{
	    SetContextForeGroundColor (BLACK_COLOR);
	    r.corner.y = (y + 1) -
		    (((ShipInfoPtr->energy_level + 2) >> 1) * (UNIT_HEIGHT + 1));
	    do
	    {
		r.corner.x = x + (ENERGY_XOFFS + 1 + UNIT_WIDTH + 1);
		if (!(ShipInfoPtr->energy_level & 1))
		{
		    r.corner.x -= UNIT_WIDTH + 1;
		    r.corner.y += UNIT_HEIGHT + 1;
		}
		DrawFilledRectangle (&r);
		--ShipInfoPtr->energy_level;
	    } while (++energy_delta);
	}
    }
}


