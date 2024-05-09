#include <ctype.h>
#include "starcon.h"
#include "commglue.h"

#define MAX_SAVED_GAMES	10

STATIC PMENU_STATE	pLocMenuState;

PROC(
void ConfirmSaveLoad, (MsgStamp),
    ARG_END	(STAMP		*MsgStamp)
)
{
    RECT	r, clip_r;
    TEXT	t;

    SetContextFont (StarConFont);
    GetContextClipRect (&clip_r);
    if (clip_r.extent.width == 0)
    {
	clip_r.corner.x = clip_r.corner.y = 0;
	clip_r.extent.width = SCREEN_WIDTH;
	clip_r.extent.height = SCREEN_HEIGHT;
    }

    t.baseline.x = clip_r.extent.width >> 1;
    t.baseline.y = (clip_r.extent.height >> 1) + 3;
    t.align = ALIGN_CENTER;
    t.CharCount = (COUNT)~0;
    if (MsgStamp)
	t.lpStr = (LPBYTE)"Saving . . .";
    else
	t.lpStr = (LPBYTE)"Loading . . .";
    TextRect (&t, &r, NULL_PTR);
    r.corner.x -= 4;
    r.corner.y -= 4;
    r.extent.width += 8;
    r.extent.height += 8;
    if (MsgStamp)
    {
	MsgStamp->origin = r.corner;
	r.corner.x += clip_r.corner.x;
	r.corner.y += clip_r.corner.y;
	MsgStamp->frame = CaptureDrawable (LoadDisplayPixmap (&r, (FRAME)0));
	r.corner.x -= clip_r.corner.x;
	r.corner.y -= clip_r.corner.y;
    }
    DrawStarConBox (&r, 2,
	    BUILD_COLOR (MAKE_RGB15 (0x10, 0x10, 0x10), 0x19),
	    BUILD_COLOR (MAKE_RGB15 (0x8, 0x8, 0x8), 0x1F),
	    TRUE, BUILD_COLOR (MAKE_RGB15 (0xA, 0xA, 0xA), 0x08));
    SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x14, 0x14, 0x14), 0x0F));
    DrawText (&t);
}

enum
{
    SAVE_GAME = 0,
    LOAD_GAME,
    SETTINGS
};

PROC_LOCAL(
BOOLEAN	DoGameOptions, (InputState, pMS),
    ARG		(INPUT_STATE	InputState)
    ARG_END	(PMENU_STATE	pMS)
);

enum
{
    SOUND_ON_SETTING,
    SOUND_OFF_SETTING,
    MUSIC_ON_SETTING,
    MUSIC_OFF_SETTING,
    CYBORG_OFF_SETTING,
    CYBORG_NORMAL_SETTING,
    CYBORG_DOUBLE_SETTING,
    CYBORG_SUPER_SETTING,
    CHANGE_CAPTAIN_SETTING,
    CHANGE_SHIP_SETTING,
    EXIT_MENU_SETTING
};

PROC(STATIC
void near FeedbackSetting, (which_setting),
    ARG_END	(BYTE	which_setting)
)
{
    char	buf[20];

    buf[0] = '\0';
    switch (which_setting)
    {
	case SOUND_ON_SETTING:
	case SOUND_OFF_SETTING:
	    sprintf (buf, "Sound %s",
		    GLOBAL (glob_flags) & SOUND_DISABLED
		    ? "OFF" : "ON");
	    break;
	case MUSIC_ON_SETTING:
	case MUSIC_OFF_SETTING:
	    sprintf (buf, "Music %s",
		    GLOBAL (glob_flags) & MUSIC_DISABLED
		    ? "OFF" : "ON");
	    break;
	case CYBORG_OFF_SETTING:
	case CYBORG_NORMAL_SETTING:
	case CYBORG_DOUBLE_SETTING:
	case CYBORG_SUPER_SETTING:
	    sprintf (buf, "Cyborg %s",
		    !(GLOBAL (glob_flags) & CYBORG_ENABLED)
		    ? "OFF" : "ON");
	    break;
	case CHANGE_CAPTAIN_SETTING:
	case CHANGE_SHIP_SETTING:
	    strcpy (buf, "Edit name");
	    break;
    }

    SetSemaphore (&GraphicsSem);
    DrawStatusMessage ((LPSTR)buf);
    ClearSemaphore (&GraphicsSem);
}

PROC_LOCAL(
BOOLEAN	DoSettings, (InputState, pMS),
    ARG		(INPUT_STATE	InputState)
    ARG_END	(PMENU_STATE	pMS)
);

PROC(STATIC
BOOLEAN DrawDescriptionString, (pMS, which_string, state),
    ARG		(PMENU_STATE	pMS)
    ARG		(COUNT		which_string)
    ARG_END	(SIZE		state)
)
{
    COUNT	rel_index;
    RECT	r;
    TEXT	lf;
    STAMP	s;
    COLOR	BackGround, ForeGround;
    FONT	Font;
    FRAME	OldFrame;
    PROC_LOCAL(
    BOOLEAN DoNaming, (InputState, pMS),
	ARG	(INPUT_STATE	InputState)
	ARG_END	(PMENU_STATE	pMS)
    );

    SetSemaphore (&GraphicsSem);

    rel_index = (COUNT)(which_string - pMS->first_item.y);

    {
	r.corner.x = 2;
	r.extent.width = SHIP_NAME_WIDTH;
	r.extent.height = SHIP_NAME_HEIGHT;

	SetContext (StatusContext);
	if (pMS->delta_item == CHANGE_CAPTAIN_SETTING)
	{
	    Font = TinyFont;
	    r.corner.y = 10;
	    ++r.corner.x;
	    r.extent.width -= 2;
	    lf.baseline.x = r.corner.x + (r.extent.width >> 1) - 1;

	    BackGround = BUILD_COLOR (MAKE_RGB15 (0xA, 0xA, 0x1F), 0x09);
	    ForeGround = BUILD_COLOR (MAKE_RGB15 (0xA, 0x1F, 0x1F), 0x0B);
	}
	else
	{
	    Font = StarConFont;
	    r.corner.y = 20;
	    lf.baseline.x = r.corner.x + (r.extent.width >> 1);

	    BackGround = BUILD_COLOR (MAKE_RGB15 (0xF, 0x00, 0x00), 0x2D);
	    ForeGround = BUILD_COLOR (MAKE_RGB15 (0x1F, 0xA, 0x00), 0x7D);
	}

	lf.baseline.y = r.corner.y + r.extent.height - 1;
	lf.align = ALIGN_CENTER;
    }

    SetContextFont (Font);
    lf.lpStr = (LPBYTE)((GAME_DESC *)pMS->CurString)[rel_index];
    lf.CharCount = (COUNT)~0;

    if (state <= 0)
    {
	if (pMS->InputFunc == DoNaming)
	{
	    if (pMS->delta_item == CHANGE_CAPTAIN_SETTING)
		DrawCaptainsName ();
	    else
	    {
		DrawFlagshipName (TRUE);
		if (pMenuState && pMenuState->InputFunc == DoOutfit)
		    DrawFlagshipName (FALSE);
	    }
	}
	else
	{
	    if (state == 0)
	    {
		COLOR	OldColor;

		OldColor = SetContextForeGroundColor (BLACK_COLOR);
		DrawFilledRectangle (&r);
		SetContextForeGroundColor (OldColor);
	    }
	    DrawText (&lf);
	}
	SetFlashRect (&r, (FRAME)0);
	if (pMS->CurFrame)
	{
	    DestroyDrawable (ReleaseDrawable (pMS->CurFrame));
	    pMS->CurFrame = 0;
	}
	SetFlashRect ((PRECT)~0L, (FRAME)0);
    }
    else
    {
	COUNT	i;
	RECT	text_r;
	BYTE	char_deltas[MAX_DESC_CHARS];
	PBYTE	pchar_deltas;
	CONTEXT	OldContext;

	s.origin = r.corner;
	lf.baseline.x -= s.origin.x;
	lf.baseline.y -= s.origin.y;
	TextRect (&lf, &text_r, char_deltas);
	if ((text_r.extent.width + 2) >= r.extent.width)
	{
	    ClearSemaphore (&GraphicsSem);
	    return (FALSE);
	}

	if (pMS->CurFrame == 0)
	    pMS->CurFrame = CaptureDrawable (
		    CreateDrawable (WANT_PIXMAP, r.extent.width, r.extent.height, 1)
		    );
	OldContext = SetContext (OffScreenContext);
	OldFrame = SetContextFGFrame (pMS->CurFrame);
	SetContextFont (Font);

	SetContextBackGroundColor (BackGround);
	ClearDrawable ();

	pchar_deltas = char_deltas;
	for (i = pMS->first_item.x; i > 0; --i)
	    text_r.corner.x += (SIZE)*pchar_deltas++;
#ifdef OLD
	if ((COUNT)pMS->first_item.x < lf.CharCount)	/* end of line */
	    --text_r.corner.x;
	text_r.extent.width = 1;
#else /* NEW */
	if ((COUNT)pMS->first_item.x == lf.CharCount)	/* end of line */
	    text_r.extent.width = 1;
	else
	    text_r.extent.width = (SIZE)*pchar_deltas;
#endif /* OLD */
	text_r.corner.y = 0;
	text_r.extent.height = r.extent.height;
	SetContextForeGroundColor (BLACK_COLOR);
	DrawFilledRectangle (&text_r);

	SetContextForeGroundColor (ForeGround);
	DrawText (&lf);

	SetContextFGFrame (OldFrame);
	SetContext (OldContext);
	s.frame = pMS->CurFrame;
	DrawStamp (&s);

	SetFlashRect (&r, (FRAME)0);
    }

    ClearSemaphore (&GraphicsSem);
    return (TRUE);
}

PROC(STATIC
BYTE DoTextEntry, (pMS, InputState),
    ARG		(PMENU_STATE	pMS)
    ARG_END	(INPUT_STATE	InputState)
)
{
    BYTE	ch;
    PBYTE	pStr, pBaseStr;
    COUNT	len;
    PROC_GLOBAL(
    BYTE GetJoystickChar, (InputState),
	ARG_END	(INPUT_STATE	InputState)
    );

    pBaseStr = ((GAME_DESC *)pMS->CurString)[pMS->CurState - pMS->first_item.y];
    pStr = &pBaseStr[pMS->first_item.x];
    len = strlen ((const char *)pStr);
    if (ch = GetJoystickChar (InputState)) switch (ch)
    {
	case 0x7F:
	    if (len)
	    {
		MEMMOVE (pStr, pStr + 1, len);
		len = (COUNT)~0;
	    }
	    break;
	case '\b':
	    if (pMS->first_item.x)
	    {
		MEMMOVE (pStr - 1, pStr, len + 1);
		--pMS->first_item.x;

		len = (COUNT)~0;
	    }
	    break;
	default:
	    if (isprint (ch) && len + (pStr - pBaseStr) < MAX_DESC_CHARS)
	    {
#ifdef OLD
		MEMMOVE (pStr + 1, pStr, len + 1);
		*pStr++ = ch;
		++pMS->first_item.x;
#else /* !OLD */
		*pStr = ch;
		if (len == 0)
		    *(pStr + 1) = '\0';
#endif /* OLD */

		len = (COUNT)~0;
	    }
	    break;
    }

    if (len == (COUNT)~0)
    {
	if (!DrawDescriptionString (pMS, pMS->CurState, 1))
	{
	    len = strlen ((const char *)pStr);
	    MEMMOVE (pStr - 1, pStr, len + 1);
	    --pMS->first_item.x;
	}
    }
    else if (GetInputXComponent (InputState) < 0)
    {
	if (pMS->first_item.x)
	{
	    --pMS->first_item.x;
	    DrawDescriptionString (pMS, pMS->CurState, 1);
	}
    }
    else if (GetInputXComponent (InputState) > 0)
    {
	if (len)
	{
	    ++pMS->first_item.x;
	    DrawDescriptionString (pMS, pMS->CurState, 1);
	}
    }

    return (ch);
}

PROC(STATIC
BOOLEAN	DoNaming, (InputState, pMS),
    ARG		(INPUT_STATE	InputState)
    ARG_END	(PMENU_STATE	pMS)
)
{
    if (GLOBAL (CurrentActivity) & CHECK_ABORT)
    {
	if (pMS->CurFrame)
	{
	    DestroyDrawable (ReleaseDrawable (pMS->CurFrame));
	    pMS->CurFrame = 0;
	}
	return (FALSE);
    }
    
    if (!pMS->Initialized)
    {
	GAME_DESC	GD[1];

	pMS->Initialized = TRUE;
	pMS->InputFunc = DoNaming;

	GD[0][0] = '\0';
	pMS->delta_item = pMS->CurState;
	pMS->first_item.x = pMS->first_item.y = 0;
	pMS->CurState = 0;
	pMS->CurString = (STRING)&GD[0];
	DrawDescriptionString (pMS, pMS->CurState, 1);

	SetSemaphore (&GraphicsSem);
	DrawStatusMessage ((LPSTR)"Enter name");
	ClearSemaphore (&GraphicsSem);

	DoInput (pMS);

	pMS->InputFunc = DoSettings;
	pMS->CurState = (BYTE)pMS->delta_item;
	pMS->delta_item = 0;
	if (!(GLOBAL (CurrentActivity) & CHECK_ABORT))
	    FeedbackSetting (pMS->CurState);
    }
    else
    {
	BYTE	ch;

	ch = DoTextEntry (pMS, InputState);

	if (ch == '\n' || ch == 0x1B)
	{
	    if (ch == '\n')
	    {
		if (pMS->delta_item == CHANGE_CAPTAIN_SETTING)
		    strcpy (GLOBAL_SIS (CommanderName),
			    (const char *)((GAME_DESC *)pMS->CurString)[0]);
		else
		    strcpy (GLOBAL_SIS (ShipName),
			    (const char *)((GAME_DESC *)pMS->CurString)[0]);
	    }
	    else
	    {
		if (pMS->delta_item == CHANGE_CAPTAIN_SETTING)
		    strcpy ((char *)((GAME_DESC *)pMS->CurString)[0],
			    GLOBAL_SIS (CommanderName));
		else
		    strcpy ((char *)((GAME_DESC *)pMS->CurString)[0],
			    GLOBAL_SIS (ShipName));
	    }

	    DrawDescriptionString (pMS, pMS->CurState, 0);

	    return (FALSE);
	}
    }

    return (TRUE);
}

PROC(STATIC
BOOLEAN	DoSettings, (InputState, pMS),
    ARG		(INPUT_STATE	InputState)
    ARG_END	(PMENU_STATE	pMS)
)
{
    BYTE	cur_speed;

    if (GLOBAL (CurrentActivity) & CHECK_ABORT)
	return (FALSE);

    cur_speed = (BYTE)(GLOBAL (glob_flags) & COMBAT_SPEED_MASK) >> COMBAT_SPEED_SHIFT;
    if (!pMS->Initialized)
    {
	DrawMenuStateStrings (PM_SOUND_ON, pMS->CurState);
	FeedbackSetting (pMS->CurState);
	pMS->Initialized = TRUE;
	pMS->InputFunc = DoSettings;
    }
    else if ((InputState & DEVICE_BUTTON2)
	    || ((InputState & DEVICE_BUTTON1)
	    && pMS->CurState == EXIT_MENU_SETTING))
    {
	SetSemaphore (&GraphicsSem);
	DrawStatusMessage (NULL_PTR);
	ClearSemaphore (&GraphicsSem);

	pMS->CurState = SETTINGS;
	pMS->InputFunc = DoGameOptions;
	pMS->Initialized = 0;
    }
    else if (InputState & DEVICE_BUTTON1)
    {
	switch (pMS->CurState)
	{
	    case SOUND_ON_SETTING:
	    case SOUND_OFF_SETTING:
		ToggleSoundEffect ();
		pMS->CurState ^= 1;
		DrawMenuStateStrings (PM_SOUND_ON, pMS->CurState);
		break;
	    case MUSIC_ON_SETTING:
	    case MUSIC_OFF_SETTING:
		ToggleMusic ();
		pMS->CurState ^= 1;
		DrawMenuStateStrings (PM_SOUND_ON, pMS->CurState);
		break;
	    case CHANGE_CAPTAIN_SETTING:
	    case CHANGE_SHIP_SETTING:
		pMS->Initialized = FALSE;
		pMS->InputFunc = DoNaming;
		return (TRUE);
	    default:
		if (cur_speed++ < NUM_COMBAT_SPEEDS - 1)
		    GLOBAL (glob_flags) |= CYBORG_ENABLED;
		else
		{
		    cur_speed = 0;
		    GLOBAL (glob_flags) &= ~CYBORG_ENABLED;
		}
		GLOBAL (glob_flags) =
			(BYTE)((GLOBAL (glob_flags) & ~COMBAT_SPEED_MASK)
			| (cur_speed << COMBAT_SPEED_SHIFT));
		pMS->CurState = CYBORG_OFF_SETTING + cur_speed;
		DrawMenuStateStrings (PM_SOUND_ON, pMS->CurState);
	}

	FeedbackSetting (pMS->CurState);
    }
    else
    {
	BYTE	NewState;

	NewState = pMS->CurState;
	if (GetInputXComponent (InputState) < 0
		|| GetInputYComponent (InputState) < 0)
	{
	    do
	    {
		if (NewState-- == SOUND_ON_SETTING)
		    NewState = EXIT_MENU_SETTING;
	    } while ((NewState == SOUND_ON_SETTING
		    && (GLOBAL (glob_flags) & SOUND_DISABLED))
		    || (NewState == SOUND_OFF_SETTING
		    && !(GLOBAL (glob_flags) & SOUND_DISABLED))
		    || (NewState == MUSIC_ON_SETTING
		    && (GLOBAL (glob_flags) & MUSIC_DISABLED))
		    || (NewState == MUSIC_OFF_SETTING
		    && !(GLOBAL (glob_flags) & MUSIC_DISABLED))
		    || (NewState >= CYBORG_OFF_SETTING
		    && NewState <= CYBORG_SUPER_SETTING
		    && NewState != cur_speed + CYBORG_OFF_SETTING)
		    );
	}
	else if (GetInputXComponent (InputState) > 0
		|| GetInputYComponent (InputState) > 0)
	{
	    do
	    {
		if (NewState++ == EXIT_MENU_SETTING)
		    NewState = SOUND_ON_SETTING;
	    } while ((NewState == SOUND_ON_SETTING
		    && (GLOBAL (glob_flags) & SOUND_DISABLED))
		    || (NewState == SOUND_OFF_SETTING
		    && !(GLOBAL (glob_flags) & SOUND_DISABLED))
		    || (NewState == MUSIC_ON_SETTING
		    && (GLOBAL (glob_flags) & MUSIC_DISABLED))
		    || (NewState == MUSIC_OFF_SETTING
		    && !(GLOBAL (glob_flags) & MUSIC_DISABLED))
		    || (NewState >= CYBORG_OFF_SETTING
		    && NewState <= CYBORG_SUPER_SETTING
		    && NewState != cur_speed + CYBORG_OFF_SETTING)
		    );
	}

	if (NewState != pMS->CurState)
	{
	    DrawMenuStateStrings (PM_SOUND_ON, NewState);
	    pMS->CurState = NewState;

	    FeedbackSetting (pMS->CurState);
	}
    }

    return (TRUE);
}

PROC(STATIC
void DrawCargo, (redraw_state),
    ARG_END	(COUNT	redraw_state)
)
{
    BYTE		i;
    RECT		r;
    extern FRAME	misc_data;

    SetContext (SpaceContext);
    if (redraw_state)
    {
	STAMP	s;

	if (redraw_state == 2)
	{
	    SetContextForeGroundColor (BLACK_COLOR);
	    r.corner.x = 1;
	    r.corner.y = 12;
	    r.extent.width = ((SIS_SCREEN_WIDTH - STATUS_WIDTH) >> 1) - r.corner.x;
	    r.extent.height = 62 - r.corner.y;
	    DrawFilledRectangle (&r);
	    GetFrameRect (SetRelFrameIndex (
		    pLocMenuState->ModuleFrame, MAX_SAVED_GAMES * 2 + 1
		    ), &r);
	    DrawFilledRectangle (&r);
	}
	else
	{
	    s.origin.x = s.origin.y = 0;
	    s.frame = SetAbsFrameIndex (pLocMenuState->ModuleFrame,
		    GetFrameCount (pLocMenuState->ModuleFrame) - 1);
	    if (!pLocMenuState->Initialized)
	    {
		DrawStamp (&s);
		s.frame = DecFrameIndex (s.frame);
		if (pLocMenuState->delta_item == SAVE_GAME)
		    s.frame = DecFrameIndex (s.frame);
		DrawStamp (&s);
		if (((SUMMARY_DESC *)pLocMenuState->CurString)
			[pLocMenuState->CurState].year_index == 0)
		    return;
	    }
	    else
	    {
		GetContextClipRect (&r);
		r.extent.height = 136;
		SetContextClipRect (&r);
		DrawStamp (&s);
		r.extent.height = SIS_SCREEN_HEIGHT;
		SetContextClipRect (&r);
	    }
	}

	s.frame = SetAbsFrameIndex (
		misc_data,
		(NUM_SCANDOT_TRANSITIONS << 1) + 3
		);
	if (redraw_state == 2
		|| (redraw_state == 1
		&& !(((SUMMARY_DESC *)pLocMenuState->CurString)
		[pLocMenuState->CurState].Flags & AFTER_BOMB_INSTALLED)))
	{
	    s.origin.x = 7;
	    s.origin.y = 17;
	    for (i = 0; i < NUM_ELEMENT_CATEGORIES; ++i)
	    {
		if (i == NUM_ELEMENT_CATEGORIES >> 1)
		{
		    s.origin.x += 36;
		    s.origin.y = 17;
		}
		DrawStamp (&s);
		s.frame = SetRelFrameIndex (s.frame, 5);
		s.origin.y += 12;
	    }
	}
	s.origin.x = 24;
	s.origin.y = 68;
	s.frame = SetAbsFrameIndex (s.frame, 68);
	DrawStamp (&s);
    }
    else
    {
	TEXT	t;
	char	buf[10];
	COLOR	cargo_color[] =
	{
	    BUILD_COLOR (MAKE_RGB15 (2, 14, 19), 0x00),
	    BUILD_COLOR (MAKE_RGB15 (25, 0x00, 0x00), 0x00),
	    BUILD_COLOR (MAKE_RGB15 (0x10, 0x10, 0x10), 0x00),
	    BUILD_COLOR (MAKE_RGB15 (3, 5, 30), 0x00),
	    BUILD_COLOR (MAKE_RGB15 (0x00, 24, 0x00), 0x00),
	    BUILD_COLOR (MAKE_RGB15 (27, 27, 0x00), 0x00),
	    BUILD_COLOR (MAKE_RGB15 (30, 13, 0x00), 0x00),
	    BUILD_COLOR (MAKE_RGB15 (20, 0x00, 20), 0x00),
	    BUILD_COLOR (MAKE_RGB15 (15, 0x00, 25), 0x00),
	};

	r.extent.width = 20;
	r.extent.height = SHIP_NAME_HEIGHT;
	SetContextFont (StarConFont);	
	t.baseline.x = 33;
	t.baseline.y = 20;
	t.align = ALIGN_RIGHT;
	t.lpStr = (LPBYTE)buf;
	for (i = 0; i < NUM_ELEMENT_CATEGORIES; ++i)
	{
	    if (i == NUM_ELEMENT_CATEGORIES >> 1)
	    {
		t.baseline.x += 36;
		t.baseline.y = 20;
	    }
	    SetContextForeGroundColor (BLACK_COLOR);
	    r.corner.x = t.baseline.x - r.extent.width + 1;
	    r.corner.y = t.baseline.y - r.extent.height + 1;
	    DrawFilledRectangle (&r);
	    SetContextForeGroundColor (cargo_color[i]);
	    sprintf (buf, "%u", GLOBAL_SIS (ElementAmounts[i]));
	    t.CharCount = (COUNT)~0;
	    DrawText (&t);
	    t.baseline.y += 12;
	}
	t.baseline.x = 50;
	t.baseline.y = 71;
	SetContextForeGroundColor (BLACK_COLOR);
	r.corner.x = t.baseline.x - r.extent.width + 1;
	r.corner.y = t.baseline.y - r.extent.height + 1;
	DrawFilledRectangle (&r);
	SetContextForeGroundColor (cargo_color[i]);
	sprintf (buf, "%u", GLOBAL_SIS (TotalBioMass));
	t.CharCount = (COUNT)~0;
	DrawText (&t);
    }
}

PROC(STATIC
void ShowSummary, (pSD),
    ARG_END	(SUMMARY_DESC	*pSD)
)
{
    RECT	r;
    STAMP	s;

    if (pSD->year_index == 0)
    {
	s.origin.x = s.origin.y = 0;
	s.frame = SetAbsFrameIndex (pLocMenuState->ModuleFrame,
		GetFrameCount (pLocMenuState->ModuleFrame) - 4);
	DrawStamp (&s);
	r.corner.x = 1;
	r.corner.y = 141;
	r.extent.width = SIS_SCREEN_WIDTH - 1;
	r.extent.height = 7;
	SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x14), 0x01));
	DrawFilledRectangle (&r);
    }
    else
    {
	BYTE		i;
	RECT		OldRect;
	TEXT		t;
	QUEUE		player_q;
	CONTEXT		OldContext;
	SIS_STATE	SaveSS;
	char		buf[80];

	SaveSS = GlobData.SIS_state;
	player_q = GLOBAL (built_ship_q);

	OldContext = SetContext (StatusContext);
	GetContextClipRect (&OldRect);

	r.corner.x = SIS_ORG_X + ((SIS_SCREEN_WIDTH - STATUS_WIDTH) >> 1);
	r.corner.y = SIS_ORG_Y;
	r.extent.width = STATUS_WIDTH;
	r.extent.height = STATUS_HEIGHT;
	SetContextClipRect (&r);

	GlobData.SIS_state = pSD->SS;
	InitQueue (&GLOBAL (built_ship_q),
		MAX_BUILT_SHIPS, sizeof (SHIP_FRAGMENT));
	for (i = 0; i < pSD->NumShips; ++i)
	    CloneShipFragment (pSD->ShipList[i], &GLOBAL (built_ship_q), 0);
	DateToString (buf, pSD->month_index, pSD->day_index, pSD->year_index),
	ClearSISRect (DRAW_SIS_DISPLAY);
	DrawStatusMessage ((LPSTR)buf);
	UninitQueue (&GLOBAL (built_ship_q));

	SetContextClipRect (&OldRect);
	SetContext (SpaceContext);
	BatchGraphics ();
	DrawCargo (0);
	s.origin.y = 13;
	r.extent.width = r.extent.height = 16;
	SetContextForeGroundColor (BLACK_COLOR);
	for (i = 0; i < 4; ++i)
	{
	    BYTE	j;

	    s.origin.x = 140;
	    for (j = 0; j < 4; ++j)
	    {
		if ((i << 2) + j >= pSD->NumDevices)
		{
		    r.corner = s.origin;
		    DrawFilledRectangle (&r);
		}
		else
		{
		    extern FRAME	misc_data;

		    s.frame = SetAbsFrameIndex (
			    misc_data, 77 + pSD->DeviceList[(i << 2) + j]
			    );
		    DrawStamp (&s);
		}
		s.origin.x += 18;
	    }
	    s.origin.y += 18;
	}
	UnbatchGraphics ();	    

	SetContextFont (StarConFont);
	t.baseline.x = 173;
	t.align = ALIGN_CENTER;
	t.CharCount = (COUNT)~0;
	t.lpStr = (LPBYTE)buf;
	if (pSD->Flags & AFTER_BOMB_INSTALLED)
	{
	    s.origin.x = s.origin.y = 0;
	    s.frame = SetRelFrameIndex (
		    pLocMenuState->ModuleFrame, MAX_SAVED_GAMES * 2
		    );
	    DrawStamp (&s);
	    s.frame = IncFrameIndex (s.frame);
	    DrawStamp (&s);
	}
	else
	{
	    SetContext (RadarContext);
	    GetContextClipRect (&OldRect);
	    r.corner.x = SIS_ORG_X + 10;
	    r.corner.y = SIS_ORG_Y + 84;
	    r.extent = OldRect.extent;
	    SetContextClipRect (&r);
	    ClearSemaphore (&GraphicsSem);
	    InitLander (pSD->Flags | OVERRIDE_LANDER_FLAGS);
	    SetSemaphore (&GraphicsSem);
	    SetContextClipRect (&OldRect);
	    SetContext (SpaceContext);

	    sprintf (buf, "%lu", GLOBAL_SIS (ResUnits));
	    t.baseline.y = 102;
	    r.extent.width = 30;
	    r.extent.height = SHIP_NAME_HEIGHT;
	    r.corner.x = t.baseline.x - (r.extent.width >> 1);
	    r.corner.y = t.baseline.y - SHIP_NAME_HEIGHT + 1;
	    SetContextForeGroundColor (BLACK_COLOR);
	    DrawFilledRectangle (&r);
	    SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x10, 0x00, 0x10), 0x01));
	    DrawText (&t);
	    t.CharCount = (COUNT)~0;
	}
	t.baseline.y = 126;
	sprintf (buf, "%lu", MAKE_WORD (pSD->MCreditLo, pSD->MCreditHi));
	r.extent.width = 30;
	r.extent.height = SHIP_NAME_HEIGHT;
	r.corner.x = t.baseline.x - (r.extent.width >> 1);
	r.corner.y = t.baseline.y - SHIP_NAME_HEIGHT + 1;
	SetContextForeGroundColor (BLACK_COLOR);
	DrawFilledRectangle (&r);
	SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x10, 0x00, 0x10), 0x01));
	DrawText (&t);
	
	r.corner.x = 1;
	r.corner.y = 141;
	r.extent.width = SIS_SCREEN_WIDTH - 1;
	r.extent.height = 7;
	SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x14), 0x01));
	DrawFilledRectangle (&r);
	t.baseline.x = r.corner.x + (SIS_MESSAGE_WIDTH >> 1);
	t.baseline.y = r.corner.y + (r.extent.height - 1);
	t.align = ALIGN_CENTER;
	t.lpStr = (LPBYTE)buf;
	r.corner.x = LOGX_TO_UNIVERSE (GLOBAL_SIS (log_x));
	r.corner.y = LOGY_TO_UNIVERSE (GLOBAL_SIS (log_y));
	switch (pSD->Activity)
	{
	    case IN_LAST_BATTLE:
	    case IN_INTERPLANETARY:
	    case IN_PLANET_ORBIT:
	    case IN_STARBASE:
	    {
		BYTE		QuasiState;
		STAR_DESCPTR	SDPtr;
		
		QuasiState = GET_GAME_STATE (ARILOU_SPACE_SIDE);
		SET_GAME_STATE (ARILOU_SPACE_SIDE, 0);
		SDPtr = FindStar (NULL_PTR, &r.corner, 1, 1);
		SET_GAME_STATE (ARILOU_SPACE_SIDE, QuasiState);
		if (SDPtr)
		{
		    GetClusterName (SDPtr, buf);
		    r.corner = SDPtr->star_pt;
		    break;
		}
	    }
	    default:
		buf[0] = '\0';
		break;
	    case IN_HYPERSPACE:
		strcpy (buf, "HyperSpace");
		break;
	    case IN_QUASISPACE:
		strcpy (buf, "QuasiSpace");
		break;
	}

	SetContextFont (TinyFont);
	SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x1B, 0x00, 0x1B), 0x33));
	t.CharCount = (COUNT)~0;
	DrawText (&t);
	t.baseline.x = SIS_SCREEN_WIDTH - 57 + 1 + (SIS_TITLE_WIDTH >> 1);
	if (pSD->Activity == IN_STARBASE)
	    strcpy (buf, "Starbase");
	else if (pSD->Activity == IN_PLANET_ORBIT)
	    strcpy (buf, GLOBAL_SIS (PlanetName));
	else
	    sprintf (buf, "%03u.%01u:%03u.%01u",
		    r.corner.x / 10, r.corner.x % 10,
		    r.corner.y / 10, r.corner.y % 10);
	t.CharCount = (COUNT)~0;
	DrawText (&t);
	
	SetContext (OldContext);

	GLOBAL (built_ship_q) = player_q;
	GlobData.SIS_state = SaveSS;
    }
}

PROC(STATIC
void LoadGameDescriptions, (pSD),
    ARG_END	(SUMMARY_DESC	*pSD)
)
{
    COUNT	i;

    for (i = 0; i < MAX_SAVED_GAMES; ++i, ++pSD)
    {
	PROC_GLOBAL(
	BOOLEAN LoadGame, (which_game, summary_desc),
	    ARG		(COUNT		which_game)
	    ARG_END	(SUMMARY_DESC	*summary_desc)
	);

	if (!LoadGame (i, pSD))
	    pSD->year_index = 0;
    }
}

PROC(STATIC
BOOLEAN	DoPickGame, (InputState, pMS),
    ARG		(INPUT_STATE	InputState)
    ARG_END	(PMENU_STATE	pMS)
)
{
    BYTE		NewState;
    STAMP		s;
    SUMMARY_DESC	*pSD;
    BOOLEAN		first_time;

    if (GLOBAL (CurrentActivity) & CHECK_ABORT)
    {
	pMS->ModuleFrame = 0;
	
	return (FALSE);
    }
    first_time = (BOOLEAN)(pMS->Initialized == 0);

    if (!pMS->Initialized)
    {
	pMS->delta_item = (SIZE)pMS->CurState;
	pMS->CurState = NewState = 0;

	pMS->InputFunc = DoPickGame;
	SleepTask ((DWORD)pMS->CurFrame);
	pMS->CurFrame = 0;
	PauseMusic ();
	StopSound ();
	FadeSound (128, 0);

	{
	    extern FRAME	PlayFrame;

	    pMS->ModuleFrame = SetAbsFrameIndex (PlayFrame, 38);
	}

	SetSemaphore (&GraphicsSem);
	BatchGraphics ();
	ScreenOrigin (Screen, 0, 0);
Restart:
	SetContext (SpaceContext);
	LoadGameDescriptions ((SUMMARY_DESC *)pMS->CurString);
	DrawCargo (1);
	pMS->Initialized = TRUE;
	goto ChangeGameSelection;
    }
    else if (InputState & DEVICE_BUTTON2)
    {
	SetSemaphore (&GraphicsSem);
	SetFlashRect (NULL_PTR, (FRAME)0);
	s.origin.x = s.origin.y = 0;
	s.frame = SetRelFrameIndex (pMS->ModuleFrame, pMS->CurState);
	DrawStamp (&s);
	SetFlashRect ((PRECT)~0L, (FRAME)0);
	ClearSemaphore (&GraphicsSem);

pMS->ModuleFrame = 0;
	pMS->CurState = (BYTE)pMS->delta_item;
ResumeMusic ();
if (pSolarSysState)
{
#define DRAW_REFRESH		(1 << 5)
#define REPAIR_SCAN		(1 << 6)
    extern BYTE	draw_sys_flags;

    if (pSolarSysState->MenuState.Initialized < 3)
	draw_sys_flags |= DRAW_REFRESH;
    else if (pSolarSysState->MenuState.Initialized == 4)
	draw_sys_flags |= REPAIR_SCAN;
}
	return (FALSE);
    }
    else if (InputState & DEVICE_BUTTON1)
    {
	pSD = &((SUMMARY_DESC *)pMS->CurString)[pMS->CurState];
	if (pMS->delta_item == SAVE_GAME || pSD->year_index)
	{
RetrySave:
	    SetSemaphore (&GraphicsSem);
	    if (pMS->delta_item == SAVE_GAME)
	    {
		STAMP	MsgStamp;
		PROC_GLOBAL(
		BOOLEAN SaveGame, (which_game, summary_desc),
		    ARG		(COUNT		which_game)
		    ARG_END	(SUMMARY_DESC	*summary_desc)
		);

		ConfirmSaveLoad (&MsgStamp);
		if (SaveGame ((COUNT)pMS->CurState, pSD))
		{
		    DestroyDrawable (ReleaseDrawable (MsgStamp.frame));
		    GLOBAL (CurrentActivity) |= CHECK_LOAD;
		}
		else
		{
		    SetFlashRect (NULL_PTR, (FRAME)0);
		    DrawStamp (&MsgStamp);
		    DestroyDrawable (ReleaseDrawable (MsgStamp.frame));
		    ClearSemaphore (&GraphicsSem);
		    
		    if (SaveProblem ())
			goto RetrySave;

		    pMS->Initialized = FALSE;
		    NewState = pMS->CurState;
		    SetSemaphore (&GraphicsSem);
		    BatchGraphics ();
		    goto Restart;
		}
		ResumeMusic ();
if (pSolarSysState)
{
#define DRAW_REFRESH		(1 << 5)
#define REPAIR_SCAN		(1 << 6)
    extern BYTE	draw_sys_flags;

    if (pSolarSysState->MenuState.Initialized < 3)
	draw_sys_flags |= DRAW_REFRESH;
    else if (pSolarSysState->MenuState.Initialized == 4)
	draw_sys_flags |= REPAIR_SCAN;
}
	    }
	    else
	    {
		PROC_GLOBAL(
		BOOLEAN LoadGame, (which_game, summary_desc),
		    ARG		(COUNT		which_game)
		    ARG_END	(SUMMARY_DESC	*summary_desc)
		);

		ConfirmSaveLoad (0);
		if (LoadGame ((COUNT)pMS->CurState, NULL_PTR))
		    GLOBAL (CurrentActivity) |= CHECK_LOAD;
	    }
	    SetFlashRect (NULL_PTR, (FRAME)0);
	    ClearSemaphore (&GraphicsSem);

pMS->ModuleFrame = 0;
	    pMS->CurState = (BYTE)pMS->delta_item;
	    return (FALSE);
	}
    }
    else
    {
	NewState = pMS->CurState;
	if (GetInputXComponent (InputState) < 0)
	{
	    if (NewState-- == 0)
		NewState = MAX_SAVED_GAMES - 1;
	}
	else if (GetInputXComponent (InputState) > 0)
	{
	    if (++NewState == MAX_SAVED_GAMES)
		NewState = 0;
	}

	if (NewState != pMS->CurState)
	{
	    SetSemaphore (&GraphicsSem);
	    BatchGraphics ();
	    if (((SUMMARY_DESC *)pMS->CurString)[NewState].year_index != 0)
	    {
		if (!(((SUMMARY_DESC *)pMS->CurString)[NewState].Flags
			& AFTER_BOMB_INSTALLED))
		{
		    if (((SUMMARY_DESC *)pMS->CurString)[pMS->CurState].year_index == 0)
			DrawCargo (1);
		    else if (((SUMMARY_DESC *)pMS->CurString)[pMS->CurState].Flags
			    & AFTER_BOMB_INSTALLED)
			DrawCargo (2);
		}
		else if (((SUMMARY_DESC *)pMS->CurString)[pMS->CurState].year_index == 0)
		    DrawCargo (3);
	    }

	    SetFlashRect (NULL_PTR, (FRAME)0);
	    s.origin.x = s.origin.y = 0;
	    s.frame = SetRelFrameIndex (pMS->ModuleFrame, pMS->CurState);
	    DrawStamp (&s);
ChangeGameSelection:
	    pMS->CurState = NewState;
	    ShowSummary (&((SUMMARY_DESC *)pMS->CurString)[pMS->CurState]);
	    s.origin.x = s.origin.y = 0;
	    s.frame = SetRelFrameIndex (pMS->ModuleFrame, pMS->CurState + MAX_SAVED_GAMES);
	    DrawStamp (&s);

	    if (LastActivity == CHECK_LOAD)
	    {
		BYTE	clut_buf[] = {FadeAllToColor};

		UnbatchGraphics ();

		LastActivity = 0;
		XFormColorMap ((COLORMAPPTR)clut_buf, ONE_SECOND / 2);
	    }
	    else
	    {
		if (first_time)
		{
		    RECT	r;
    
		    r.corner.x = SIS_ORG_X;
		    r.corner.y = SIS_ORG_Y;
		    r.extent.width = SIS_SCREEN_WIDTH;
		    r.extent.height = SIS_SCREEN_HEIGHT;
		    ScreenTransition (3, &r);
		}
		UnbatchGraphics ();
	    }

	    {
		RECT	r;
		
		r.corner.x = 3 + (NewState * 21);
		r.corner.y = 176;
		r.extent.width = 18;
		r.extent.height = 18;
		SetFlashRect (&r, (FRAME)0);
	    }
	    ClearSemaphore (&GraphicsSem);
	}
    }

    return (TRUE);
}

PROC(STATIC
BOOLEAN near PickGame, (pMS),
    ARG_END	(PMENU_STATE	pMS)
)
{
    BOOLEAN		retval;
    CONTEXT		OldContext;
    FRAME		OldFrame;
    DRAW_STATE		OldDrawState;
    SUMMARY_DESC	desc_array[MAX_SAVED_GAMES];

    if (pSolarSysState)
    {
	++pSolarSysState->MenuState.Initialized;
	TaskSwitch ();
    }

    SetSemaphore (&GraphicsSem);
    OldContext = SetContext (SpaceContext);
    OldFrame = SetContextBGFrame (NULL_PTR);
    OldDrawState = SetContextDrawState (DEST_PIXMAP | DRAW_REPLACE);

ScreenOrigin (Screen, 0, 0);

    pMS->Initialized = FALSE;
    pMS->InputFunc = DoPickGame;
    pMS->CurString = (STRING)&desc_array[0];
    ClearSemaphore (&GraphicsSem);

    DoInput (pMS);

    SetSemaphore (&GraphicsSem);
    pMS->Initialized = -1;
    pMS->InputFunc = DoGameOptions;

    retval = TRUE;
    if (GLOBAL (CurrentActivity) & (CHECK_ABORT | CHECK_LOAD))
    {
	if (pMS->CurState == SAVE_GAME)
	    GLOBAL (CurrentActivity) &= ~CHECK_LOAD;

	retval = FALSE;
    }

    if (!(GLOBAL (CurrentActivity) & (CHECK_ABORT | CHECK_LOAD)))
    {
if (CommData.PlayerPhrases
	|| !(pSolarSysState && pSolarSysState->MenuState.Initialized < 3))
{
BatchGraphics ();
FindDisplayPage (Screen, 0, 0);
ScreenOrigin (Screen, 0, 0);
{
    RECT	r;
    
    r.corner.x = SIS_ORG_X;
    r.corner.y = SIS_ORG_Y;
    r.extent.width = SIS_SCREEN_WIDTH;
    r.extent.height = SIS_SCREEN_HEIGHT;
    ScreenTransition (3, &r);
}
UnbatchGraphics ();
}

	if (pSolarSysState)
	{
	    --pSolarSysState->MenuState.Initialized;
	    if (CommData.PlayerPhrases == 0 && !PLRPlaying ((MUSIC_REF)~0))
	    {
		if (pSolarSysState->MenuState.Initialized < 3)
		{
		    extern MUSIC_REF	SpaceMusic;
			
		    PlayMusic (SpaceMusic, TRUE, 1);
		}
		else
		{
		    extern MUSIC_REF	LanderMusic;
			
		    PlayMusic (LanderMusic, TRUE, 1);
		}
	    }
	}
    }

    SetContextDrawState (OldDrawState);
    SetContextBGFrame (OldFrame);
    SetContext (OldContext);
    ClearSemaphore (&GraphicsSem);

    return (retval);
}

PROC(STATIC
BOOLEAN	DoGameOptions, (InputState, pMS),
    ARG		(INPUT_STATE	InputState)
    ARG_END	(PMENU_STATE	pMS)
)
{
    if (GLOBAL (CurrentActivity) & CHECK_ABORT)
	return (FALSE);

    if (LastActivity == CHECK_LOAD)
	InputState = DEVICE_BUTTON1;

    if (pMS->Initialized <= 0)
    {
	if (LastActivity == CHECK_LOAD)
	    pMS->CurState = LOAD_GAME;
	DrawMenuStateStrings (PM_SAVE_GAME, pMS->CurState);

	pMS->Initialized = 1;
	pMS->InputFunc = DoGameOptions;
    }
    else if ((InputState & DEVICE_BUTTON2)
	    || ((InputState & DEVICE_BUTTON1)
	    && pMS->CurState == SETTINGS + 1))
    {
	pMS->CurState = SETTINGS + 1;
	return (FALSE);
    }
    else if (InputState & DEVICE_BUTTON1)
    {
	switch (pMS->CurState)
	{
	    case SAVE_GAME:
	    case LOAD_GAME:
		pMS->CurFrame = (FRAME)FadeSound (0, ONE_SECOND >> 1);
		return (PickGame (pMS));
	    case SETTINGS:
		pMS->Initialized = FALSE;
		pMS->InputFunc = DoSettings;
		pMS->CurState = SOUND_ON_SETTING;
		break;
	}
    }
    else
    {
	BYTE	NewState;

	NewState = pMS->CurState;
	if (GetInputXComponent (InputState) < 0
		|| GetInputYComponent (InputState) < 0)
	{
	    if (NewState-- == SAVE_GAME)
		NewState = SETTINGS + 1;
	}
	else if (GetInputXComponent (InputState) > 0
		|| GetInputYComponent (InputState) > 0)
	{
	    if (NewState++ == SETTINGS + 1)
		NewState = SAVE_GAME;
	}

	if (NewState != pMS->CurState)
	{
	    DrawMenuStateStrings (PM_SAVE_GAME, NewState);
	    pMS->CurState = NewState;
	}
    }

    return (TRUE);
}

PROC(
BOOLEAN GameOptions, (),
    ARG_VOID
)
{
    MENU_STATE	MenuState;

    pLocMenuState = &MenuState;

    MEMSET (pLocMenuState, 0, sizeof (MenuState));

    MenuState.InputFunc = DoGameOptions;
    MenuState.CurState = SAVE_GAME;
    DoInput ((PVOID)&MenuState);

    pLocMenuState = 0;

    return ((GLOBAL (CurrentActivity) & (CHECK_ABORT | CHECK_LOAD)) ? FALSE : TRUE);
}

