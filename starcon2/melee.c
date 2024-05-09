#include <ctype.h>
#include "starcon.h"
#include "melee.h"

enum
{
    CONTROLS_TOP,
    SAVE_TOP,
    LOAD_TOP,
    START_MELEE,
    LOAD_BOT,
    SAVE_BOT,
    CONTROLS_BOT,
    EDIT_MELEE
};

typedef BYTE	MELEE_OPTIONS;

#define MELEE_X_OFFS		4
#define MELEE_Y_OFFS		21
#define MELEE_BOX_WIDTH		34
#define MELEE_BOX_HEIGHT	34
#define MELEE_BOX_SPACE		1

#define NAME_AREA_HEIGHT	7
#define MELEE_WIDTH		133
#define MELEE_HEIGHT		(48 + NAME_AREA_HEIGHT)

#define INFO_ORIGIN_X		3
#define INFO_WIDTH		58
#define TEAM_INFO_ORIGIN_Y	3
#define TEAM_INFO_HEIGHT	(SHIP_INFO_HEIGHT + 75)
#define MODE_INFO_ORIGIN_Y	(TEAM_INFO_HEIGHT + 6)
#define MODE_INFO_HEIGHT	((STATUS_HEIGHT - 3) - MODE_INFO_ORIGIN_Y)
#define RACE_INFO_ORIGIN_Y	(SHIP_INFO_HEIGHT + 6)
#define RACE_INFO_HEIGHT	((STATUS_HEIGHT - 3) - RACE_INFO_ORIGIN_Y)

#define MELEE_BACKGROUND_COLOR		BUILD_COLOR (MAKE_RGB15 (0x14, 0x00, 0x00), 0x04)
#define MELEE_TITLE_COLOR		BUILD_COLOR (MAKE_RGB15 (0x1F, 0x0A, 0x0A), 0x0C)
#define MELEE_TEXT_COLOR		BUILD_COLOR (MAKE_RGB15 (0x1F, 0x0A, 0x0A), 0x0C)
#define MELEE_TEAM_TEXT_COLOR		BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1F, 0x0A), 0x0E)

#define STATE_BACKGROUND_COLOR		BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x14), 0x01)
#define STATE_TEXT_COLOR		BUILD_COLOR (MAKE_RGB15 (0x00, 0x14, 0x14), 0x03)
#define ACTIVE_STATE_TEXT_COLOR		BUILD_COLOR (MAKE_RGB15 (0x0A, 0x1F, 0x1F), 0x0B)
#define UNAVAILABLE_STATE_TEXT_COLOR	BUILD_COLOR (MAKE_RGB15 (0x0A, 0x0A, 0x1F), 0x09)
#define HI_STATE_TEXT_COLOR		BUILD_COLOR (MAKE_RGB15 (0x0A, 0x1F, 0x1F), 0x0B)
#define HI_STATE_BACKGROUND_COLOR	BUILD_COLOR (MAKE_RGB15 (0x0A, 0x0A, 0x1F), 0x09)

#define LIST_INFO_BACKGROUND_COLOR	BUILD_COLOR (MAKE_RGB15 (0x14, 0x00, 0x14), 0x05)
#define LIST_INFO_TITLE_COLOR		BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1F, 0x1F), 0x0F)
#define LIST_INFO_TEXT_COLOR		BUILD_COLOR (MAKE_RGB15 (0x14, 0x14, 0x14), 0x07)
#define LIST_INFO_CURENTRY_TEXT_COLOR	BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1F, 0x1F), 0x0F)
#define HI_LIST_INFO_TEXT_COLOR		BUILD_COLOR (MAKE_RGB15 (0x14, 0x00, 0x00), 0x04)
#define HI_LIST_INFO_BACKGROUND_COLOR	BUILD_COLOR (MAKE_RGB15 (0x1F, 0x0A, 0x1F), 0x0D)

#define TEAM_NAME_TEXT_COLOR		BUILD_COLOR (MAKE_RGB15 (15, 16, 27), 0x00)
#define TEAM_NAME_EDIT_TEXT_COLOR	BUILD_COLOR (MAKE_RGB15 (23, 24, 29), 0x00)
#define TEAM_NAME_EDIT_RECT_COLOR	BUILD_COLOR (MAKE_RGB15 (0x14, 0x00, 0x14), 0x05)
#define TEAM_NAME_EDIT_CURS_COLOR	WHITE_COLOR

#define MAX_TEAM_CHARS			15
#define MAX_VIS_TEAMS			5
#define NUM_PREBUILT			5
#define NUM_PICK_COLS			5
#define NUM_PICK_ROWS			5

FRAME		PickMeleeFrame;
STATIC FRAME	MeleeFrame;
extern QUEUE	master_q;
extern DWORD	InTime;

typedef struct
{
    BYTE	ShipList[NUM_MELEE_ROWS][NUM_MELEE_COLUMNS];
    char	TeamName[MAX_TEAM_CHARS + 1];
} TEAM_IMAGE;

typedef struct melee_state
{
    PROC_PARAMETER(
    BOOLEAN (*InputFunc), (InputState, pInputState),
	ARG	(INPUT_STATE	InputState)
	ARG_END	(struct melee_state *	pInputState)
    );
    COUNT		MenuRepeatDelay;

    BOOLEAN		Initialized;
    MELEE_OPTIONS	MeleeOption;
    DIRENTRY		TeamDE;
    COUNT		TopTeamIndex, BotTeamIndex;
    COUNT		side, row, col;
    TEAM_IMAGE		TeamImage[NUM_SIDES];
    COUNT		star_bucks[NUM_SIDES];
    COUNT		CurIndex;
    TASK		flash_task;
    TEAM_IMAGE		FileList[MAX_VIS_TEAMS];
    TEAM_IMAGE		PreBuiltList[NUM_PREBUILT];
} MELEE_STATE;
typedef MELEE_STATE	*PMELEE_STATE;

static PMELEE_STATE	VOLATILE pMeleeState;

PROC_LOCAL(
BOOLEAN	DoMelee, (InputState, pMS),
    ARG		(INPUT_STATE	InputState)
    ARG_END	(PMELEE_STATE	pMS)
);
PROC_LOCAL(
BOOLEAN	DoEdit, (InputState, pMS),
    ARG		(INPUT_STATE	InputState)
    ARG_END	(PMELEE_STATE	pMS)
);
PROC_LOCAL(
BOOLEAN	DoPickShip, (InputState, pMS),
    ARG		(INPUT_STATE	InputState)
    ARG_END	(PMELEE_STATE	pMS)
);
PROC_LOCAL(
void DrawTeamString, (pMS, HiLiteState),
    ARG		(PMELEE_STATE	pMS)
    ARG_END	(COUNT		HiLiteState)
);
PROC_LOCAL(
BOOLEAN	DoLoadTeam, (InputState, pMS),
    ARG		(INPUT_STATE	InputState)
    ARG_END	(PMELEE_STATE	pMS)
);
PROC_LOCAL(
void DrawFileStrings, (pMS, HiLiteState),
    ARG		(PMELEE_STATE	pMS)
    ARG_END	(int		HiLiteState)
);

PROC(STATIC
void DrawMeleeIcon, (which_icon),
    ARG_END	(COUNT	which_icon)
)
{
    STAMP	s;
	    
    s.origin.x = s.origin.y = 0;
    s.frame = SetAbsFrameIndex (MeleeFrame, which_icon);
    DrawStamp (&s);
}

PROC(STATIC
void GetShipBox, (pRect, side, row, col),
    ARG		(PRECT	pRect)
    ARG		(COUNT	side)
    ARG		(COUNT	row)
    ARG_END	(COUNT	col)
)
{
    pRect->corner.x = MELEE_X_OFFS
	    + (col * (MELEE_BOX_WIDTH + MELEE_BOX_SPACE));
    pRect->corner.y = MELEE_Y_OFFS
	    + (side * (MELEE_Y_OFFS + MELEE_BOX_SPACE
		    + (NUM_MELEE_ROWS * (MELEE_BOX_HEIGHT + MELEE_BOX_SPACE))))
	    + (row * (MELEE_BOX_HEIGHT + MELEE_BOX_SPACE));
    pRect->extent.width = MELEE_BOX_WIDTH;
    pRect->extent.height = MELEE_BOX_HEIGHT;
}

PROC(STATIC
void DrawShipBox, (pMS, HiLite),
    ARG		(PMELEE_STATE	pMS)
    ARG_END	(BOOLEAN	HiLite)
)
{
    RECT	r;
    BYTE	StarShip;

    GetShipBox (&r, pMS->side, pMS->row, pMS->col);

    BatchGraphics ();
    StarShip = pMS->TeamImage[pMS->side].ShipList[pMS->row][pMS->col];
    if (HiLite)
	DrawStarConBox (&r, 1,
		BUILD_COLOR (MAKE_RGB15 (0x7, 0x00, 0xC), 0x3E),
		BUILD_COLOR (MAKE_RGB15 (0xC, 0x00, 0x14), 0x3C),
		(BOOLEAN)(StarShip != (BYTE)~0), BUILD_COLOR (MAKE_RGB15 (0xA, 0x00, 0x11), 0x3D));
    else
	DrawStarConBox (&r, 1,
		BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x9), 0x56),
		BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0xE), 0x54),
		(BOOLEAN)(StarShip != (BYTE)~0), BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0xC), 0x55));

    if (StarShip != (BYTE)~0)
    {
	STAMP		s;
	HSTARSHIP	hStarShip;
	STARSHIPPTR	StarShipPtr;

	hStarShip = GetStarShipFromIndex (&master_q, StarShip);
	StarShipPtr = LockStarShip (&master_q, hStarShip);
	s.origin.x = r.corner.x + (r.extent.width >> 1);
	s.origin.y = r.corner.y + (r.extent.height >> 1);
	s.frame = StarShipPtr->RaceDescPtr->ship_info.melee_icon;

	DrawStamp (&s);
	UnlockStarShip (&master_q, hStarShip);
    }
    UnbatchGraphics ();
}

PROC(STATIC
void DrawControls, (which_side, HiLite),
    ARG		(COUNT		which_side)
    ARG_END	(BOOLEAN	HiLite)
)
{
    COUNT	which_icon;

    if (PlayerControl[which_side] & HUMAN_CONTROL)
	which_icon = 0;
    else
    {
	switch (PlayerControl[which_side]
		& (STANDARD_RATING | GOOD_RATING | AWESOME_RATING))
	{
	    case STANDARD_RATING:
		which_icon = 1;
		break;
	    case GOOD_RATING:
		which_icon = 2;
		break;
	    case AWESOME_RATING:
		which_icon = 3;
		break;
	}
    }

    DrawMeleeIcon (1 + (8 * (1 - which_side)) + (HiLite ? 4 : 0) + which_icon);
}

PROC(STATIC
void DrawPickFrame, (pMS),
    ARG_END	(PMELEE_STATE	pMS)
)
{
    FRAME	F;
    RECT	r, r0, r1, ship_r;
    PROC_LOCAL(
    void DrawMeleeShipStrings, (pMS, NewStarShip),
	ARG		(PMELEE_STATE	pMS)
	ARG_END	(BYTE	NewStarShip)
    );
		
    GetShipBox (&r0, 0, 0, 0),
    GetShipBox (&r1, 1, NUM_MELEE_ROWS - 1, NUM_MELEE_COLUMNS - 1),
    BoxUnion (&r0, &r1, &ship_r);

    F = SetAbsFrameIndex (MeleeFrame, 27);
    GetFrameRect (F, &r);
    r.corner.x = -(ship_r.corner.x
	    + ((ship_r.extent.width - r.extent.width) >> 1));
    if (pMS->side)
	r.corner.y = -ship_r.corner.y;
    else
	r.corner.y = -(ship_r.corner.y
		+ (ship_r.extent.height - r.extent.height));
    SetFrameHot (F, MAKE_HOT_SPOT (r.corner.x, r.corner.y));
    DrawMeleeIcon (27);
    ClearSemaphore (&GraphicsSem);
    DrawMeleeShipStrings (pMS, pMS->CurIndex);
    SetSemaphore (&GraphicsSem);
}

PROC(STATIC
void RepairMeleeFrame, (pRect),
    ARG_END	(PRECT		pRect)
)
{
    RECT	r;
    CONTEXT	OldContext;
    RECT	OldRect;
DRAW_STATE	OldDrawState;

    r.corner.x = pRect->corner.x + SAFE_X;
    r.corner.y = pRect->corner.y + SAFE_Y;
    r.extent = pRect->extent;
    if (r.corner.y & 1)
    {
	--r.corner.y;
	++r.extent.height;
    }

    OldContext = SetContext (SpaceContext);
    GetContextClipRect (&OldRect);
    SetContextClipRect (&r);
    SetFrameHot (Screen, MAKE_HOT_SPOT (r.corner.x - SAFE_X, r.corner.y - SAFE_Y));
OldDrawState = SetContextDrawState (DEST_PIXMAP | DRAW_SUBTRACTIVE);
    BatchGraphics ();

    DrawMeleeIcon (0);
    DrawMeleeIcon (26);
    {
	COUNT	old_side, old_row, old_col;
    
	old_side = pMeleeState->side;
	old_row = pMeleeState->row;
	old_col = pMeleeState->col;
	for (pMeleeState->side = 0;
		pMeleeState->side < NUM_SIDES;
		++pMeleeState->side)
	{
	    DrawControls (pMeleeState->side, FALSE);
	    for (pMeleeState->row = 0;
		    pMeleeState->row < NUM_MELEE_ROWS;
		    ++pMeleeState->row)
	    {
		for (pMeleeState->col = 0;
			pMeleeState->col < NUM_MELEE_COLUMNS;
			++pMeleeState->col)
		    DrawShipBox (pMeleeState, FALSE);
	    }

	    DrawTeamString (pMeleeState, 0);
	}
	pMeleeState->side = old_side;
	pMeleeState->row = old_row;
	pMeleeState->col = old_col;
    }
    
    if (pMeleeState->InputFunc == DoPickShip)
	DrawPickFrame (pMeleeState);
	
    UnbatchGraphics ();
SetContextDrawState (OldDrawState);
    SetFrameHot (Screen, MAKE_HOT_SPOT (0, 0));
    SetContextClipRect (&OldRect);
    SetContext (OldContext);
}

PROC(STATIC
void DrawTeamString, (pMS, HiLiteState),
    ARG		(PMELEE_STATE	pMS)
    ARG_END	(COUNT		HiLiteState)
)
{
    RECT	r;
    TEXT	lfText;
DRAW_STATE	OldDrawState;

    r.corner.x = MELEE_X_OFFS - 1;
    r.corner.y =
	    (pMS->side + 1)
	    * (MELEE_Y_OFFS
	    + ((MELEE_BOX_HEIGHT + MELEE_BOX_SPACE)
	    * NUM_MELEE_ROWS + 2));
    r.extent.width = NUM_MELEE_COLUMNS
	    * (MELEE_BOX_WIDTH + MELEE_BOX_SPACE);
    r.extent.height = 13;
    if (HiLiteState == 4)
    {
	RepairMeleeFrame (&r);
	return;
    }
	
    if (HiLiteState & 1)
	r.extent.width -= 29;

    SetContextFont (MicroFont);

    lfText.lpStr = (LPBYTE)pMS->TeamImage[pMS->side].TeamName;
    lfText.baseline.y = r.corner.y + r.extent.height - 3;

    lfText.baseline.x = r.corner.x + 1;
    lfText.align = ALIGN_LEFT;
    lfText.CharCount = strlen ((char *)lfText.lpStr); // (COUNT)~0;

OldDrawState = SetContextDrawState (DEST_PIXMAP | DRAW_SUBTRACTIVE);
    BatchGraphics ();
    if (!(HiLiteState & 1))
    {
	TEXT	rtText;
	char	buf[10];

	sprintf (buf, "%d", pMS->star_bucks[pMS->side]);
	rtText.lpStr = (LPBYTE)buf;
	rtText.align = ALIGN_RIGHT;
	rtText.CharCount = (COUNT)~0;
	rtText.baseline.y = lfText.baseline.y;
	rtText.baseline.x = lfText.baseline.x + r.extent.width - 1;

	SetContextForeGroundColor (HiLiteState == 0
		? TEAM_NAME_TEXT_COLOR : TEAM_NAME_EDIT_TEXT_COLOR);
	DrawText (&lfText);
	DrawText (&rtText);
    }
    else
    {
	COUNT	i;
	RECT	text_r;
	BYTE	char_deltas[MAX_TEAM_CHARS];
	PBYTE	pchar_deltas;

	TextRect (&lfText, &text_r, char_deltas);

	text_r = r;
	SetContextForeGroundColor (TEAM_NAME_EDIT_RECT_COLOR);
	DrawFilledRectangle (&text_r);

	pchar_deltas = char_deltas;
	for (i = pMS->CurIndex; i > 0; --i)
	    text_r.corner.x += (SIZE)*pchar_deltas++;
	if (pMS->CurIndex == lfText.CharCount)	/* end of line */
	    text_r.extent.width = 1;
	else
	    text_r.extent.width = (SIZE)*pchar_deltas;
	++text_r.corner.x;
	++text_r.corner.y;
	text_r.extent.height -= 2;
	SetContextForeGroundColor (TEAM_NAME_EDIT_CURS_COLOR);
	DrawFilledRectangle (&text_r);

	SetContextForeGroundColor (BLACK_COLOR); // TEAM_NAME_EDIT_TEXT_COLOR);
	DrawText (&lfText);
    }
    UnbatchGraphics ();
SetContextDrawState (OldDrawState);
}

PROC(STATIC
void Deselect, (opt),
    ARG_END	(BYTE	opt)
)
{
    switch (opt)
    {
	case START_MELEE:
	    DrawMeleeIcon (25);
	    break;
	case LOAD_TOP:
	case LOAD_BOT:
	    if (pMeleeState->InputFunc != DoLoadTeam)
		DrawMeleeIcon (opt == LOAD_TOP ? 17 : 22);
	    else
		DrawFileStrings (pMeleeState, 0);
	    break;
	case SAVE_TOP:
	    DrawMeleeIcon (18);
	    break;
	case SAVE_BOT:
	    DrawMeleeIcon (21);
	    break;
	case CONTROLS_TOP:
	case CONTROLS_BOT:
	{
	    COUNT	which_side;
	    
	    which_side = opt == CONTROLS_TOP ? 1 : 0;
	    DrawControls (which_side, FALSE);
	    break;
	}
	case EDIT_MELEE:
	    if (pMeleeState->InputFunc == DoEdit)
	    {
DRAW_STATE	OldDrawState;
OldDrawState = SetContextDrawState (DEST_PIXMAP | DRAW_SUBTRACTIVE);
BatchGraphics ();
		if (pMeleeState->row < NUM_MELEE_ROWS)
		    DrawShipBox (pMeleeState, FALSE);
		else if (pMeleeState->CurIndex == (BYTE)~0)
		    DrawTeamString (pMeleeState, 0);
UnbatchGraphics ();
SetContextDrawState (OldDrawState);
	    }
	    else if (pMeleeState->InputFunc == DoPickShip)
	    {
		STAMP		s;
		HSTARSHIP	hStarShip;
		STARSHIPPTR	StarShipPtr;
		RECT		r;
	    
		GetFrameRect (SetAbsFrameIndex (MeleeFrame, 27), &r);

		hStarShip = GetStarShipFromIndex (&master_q, pMeleeState->CurIndex);
		StarShipPtr = LockStarShip (&master_q, hStarShip);
		s.origin.x = r.corner.x + 20
			+ (pMeleeState->CurIndex % NUM_PICK_COLS) * 18;
		s.origin.y = r.corner.y + 5
			+ (pMeleeState->CurIndex / NUM_PICK_COLS) * 18;
		s.frame = StarShipPtr->RaceDescPtr->ship_info.icons;
		DrawStamp (&s);
		UnlockStarShip (&master_q, hStarShip);
	    }
	    break;
    }
}

PROC(STATIC
void Select, (opt),
    ARG_END	(BYTE	opt)
)
{
    switch (opt)
    {
	case START_MELEE:
	    DrawMeleeIcon (26);
	    break;
	case LOAD_TOP:
	case LOAD_BOT:
	    if (pMeleeState->InputFunc != DoLoadTeam)
		DrawMeleeIcon (opt == LOAD_TOP ? 19 : 24);
	    else
		DrawFileStrings (pMeleeState, 1);
	    break;
	case SAVE_TOP:
	    DrawMeleeIcon (20);
	    break;
	case SAVE_BOT:
	    DrawMeleeIcon (23);
	    break;
	case CONTROLS_TOP:
	case CONTROLS_BOT:
	{
	    COUNT	which_side;
		    
	    which_side = opt == CONTROLS_TOP ? 1 : 0;
	    DrawControls (which_side, TRUE);
	    break;
	}
	case EDIT_MELEE:
	    if (pMeleeState->InputFunc == DoEdit)
	    {
DRAW_STATE	OldDrawState;
OldDrawState = SetContextDrawState (DEST_PIXMAP | DRAW_SUBTRACTIVE);
BatchGraphics ();
		if (pMeleeState->row < NUM_MELEE_ROWS)
		    DrawShipBox (pMeleeState, TRUE);
		else if (pMeleeState->CurIndex == (BYTE)~0)
		    DrawTeamString (pMeleeState, 2);
UnbatchGraphics ();
SetContextDrawState (OldDrawState);
	    }
	    else if (pMeleeState->InputFunc == DoPickShip)
	    {
		COLOR		OldColor;
		STAMP		s;
		HSTARSHIP	hStarShip;
		STARSHIPPTR	StarShipPtr;
		RECT		r;
	    
		GetFrameRect (SetAbsFrameIndex (MeleeFrame, 27), &r);

		hStarShip = GetStarShipFromIndex (&master_q, pMeleeState->CurIndex);
		StarShipPtr = LockStarShip (&master_q, hStarShip);
		s.origin.x = r.corner.x + 20
			+ (pMeleeState->CurIndex % NUM_PICK_COLS) * 18;
		s.origin.y = r.corner.y + 5
			+ (pMeleeState->CurIndex / NUM_PICK_COLS) * 18;
		s.frame = StarShipPtr->RaceDescPtr->ship_info.icons;
		OldColor = SetContextForeGroundColor (BLACK_COLOR);
		DrawFilledStamp (&s);
		SetContextForeGroundColor (OldColor);
		UnlockStarShip (&master_q, hStarShip);
	    }
	    break;
    }
}

PROC(STATIC
void flash_selection_func, (),
    ARG_VOID
)
{
    DWORD	TimeIn;
    
    TimeIn = GetTimeCounter ();
    for (;;)
    {
#define FLASH_RATE	(ONE_SECOND / 8)
	CONTEXT	OldContext;

	SetSemaphore (&GraphicsSem);
	OldContext = SetContext (SpaceContext);
	Deselect (pMeleeState->MeleeOption);
	SetContext (OldContext);
	ClearSemaphore (&GraphicsSem);
	TimeIn = SleepTask (TimeIn + FLASH_RATE);

	SetSemaphore (&GraphicsSem);
	OldContext = SetContext (SpaceContext);
	Select (pMeleeState->MeleeOption);
	SetContext (OldContext);
	ClearSemaphore (&GraphicsSem);
	TimeIn = SleepTask (TimeIn + FLASH_RATE);
    }
}

PROC(STATIC
void InitMelee, (pMS),
    ARG_END	(PMELEE_STATE	pMS)
)
{
    RECT	r;

    SetContext (SpaceContext);
    SetContextFGFrame (Screen);
    SetContextClipRect (NULL_PTR);
    SetContextBackGroundColor (BLACK_COLOR);
    ClearDrawable ();
    r.corner.x = SAFE_X;
    r.corner.y = SAFE_Y;
    r.extent.width = SCREEN_WIDTH - (SAFE_X * 2);
    r.extent.height = SCREEN_HEIGHT - (SAFE_Y * 2);
    SetContextClipRect (&r);

    r.corner.x = r.corner.y = 0;
    RepairMeleeFrame (&r);
    
    pMS->flash_task = AddTask (flash_selection_func, 2048);
}

PROC(STATIC
void DrawMeleeShipStrings, (pMS, NewStarShip),
    ARG		(PMELEE_STATE	pMS)
    ARG_END	(BYTE		NewStarShip)
)
{
    RECT	r, OldRect;
    HSTARSHIP	hStarShip;
    STARSHIPPTR	StarShipPtr;
    CONTEXT	OldContext;

    SetSemaphore (&GraphicsSem);

    OldContext = SetContext (StatusContext);
    GetContextClipRect (&OldRect);
    r = OldRect;
    r.corner.y += 76;
    r.extent.height = SHIP_INFO_HEIGHT;
    SetContextClipRect (&r);
    BatchGraphics ();

    if (NewStarShip == (BYTE)~0)
    {
	RECT	r;
	TEXT	t;

	ClearShipStatus (BAD_GUY_YOFFS);
	SetContextFont (StarConFont);
	r.corner.x = 3;
	r.corner.y = 4;
	r.extent.width = 57;
	r.extent.height = 60;
	SetContextForeGroundColor (BLACK_COLOR);
	DrawRectangle (&r);
	t.baseline.x = STATUS_WIDTH >> 1;
	t.baseline.y = 32;
	t.align = ALIGN_CENTER;
	if (pMS->row < NUM_MELEE_ROWS)
	{
	    t.lpStr = (LPBYTE)"Empty";
	    t.CharCount = (COUNT)~0;
	    DrawText (&t);
	    t.lpStr = (LPBYTE)"Slot";
	}
	else
	{
	    t.lpStr = (LPBYTE)"Team";
	    t.CharCount = (COUNT)~0;
	    DrawText (&t);
	    t.lpStr = (LPBYTE)"Name";
	}
	t.baseline.y += TINY_TEXT_HEIGHT;
	t.CharCount = (COUNT)~0;
	DrawText (&t);
    }
    else
    {
	hStarShip = GetStarShipFromIndex (&master_q, NewStarShip);
	StarShipPtr = LockStarShip (&master_q, hStarShip);

	InitShipStatus (StarShipPtr, NULL_PTR);

	UnlockStarShip (&master_q, hStarShip);
    }

    UnbatchGraphics ();
    SetContextClipRect (&OldRect);
    SetContext (OldContext);

    ClearSemaphore (&GraphicsSem);
}

PROC(STATIC
COUNT GetTeamValue, (pTI),
    ARG_END	(TEAM_IMAGE	*pTI)
)
{
    COUNT	row, col, val;

    val = 0;
    for (row = 0; row < NUM_MELEE_ROWS; ++row)
    {
	for (col = 0; col < NUM_MELEE_COLUMNS; ++col)
	{
	    BYTE	StarShip;

	    if ((StarShip = pTI->ShipList[row][col]) != (BYTE)~0)
	    {
		HSTARSHIP	hStarShip;

		if ((hStarShip = GetStarShipFromIndex (&master_q, StarShip)) == 0)
		    pTI->ShipList[row][col] = (BYTE)~0;
		else
		{
		    STARSHIPPTR	StarShipPtr;

		    StarShipPtr = LockStarShip (&master_q, hStarShip);
		    val += StarShipPtr->RaceDescPtr->ship_info.ship_cost;
		    UnlockStarShip (&master_q, hStarShip);
		}
	    }
	}
    }
    
    return (val);
}

PROC(STATIC
int ReadTeamImage, (pTI, load_fp),
    ARG		(TEAM_IMAGE	*pTI)
    ARG_END	(FILE		*load_fp)
)
{
    return (ReadResFile (pTI, sizeof (*pTI), 1, load_fp) == 1 ? 0 : -1);
}

PROC(STATIC
void DrawFileStrings, (pMS, HiLiteState),
    ARG		(PMELEE_STATE	pMS)
    ARG_END	(int		HiLiteState)
)
{
#define ENTRY_HEIGHT	32
    COORD	y;
    COUNT	top, bot;
    TEXT	Text, rtText;
    CONTEXT	OldContext;
    char	buf[60];

    Text.baseline.x = 5;
    y = 34;
    Text.align = ALIGN_LEFT;

    rtText.lpStr = (LPBYTE)buf;
    rtText.align = ALIGN_RIGHT;
    rtText.baseline.x = Text.baseline.x + NUM_MELEE_COLUMNS
	    * (MELEE_BOX_WIDTH + MELEE_BOX_SPACE) - 1;
		
    top = pMS->TopTeamIndex;

    if (HiLiteState == 1)
    {
	COUNT	new_;

	new_ = pMS->CurIndex;
	bot = pMS->BotTeamIndex;

	if (new_ < top || new_ > bot)
	{
	    if (new_ < top)
		top = new_;
	    else
		top += new_ - bot;
	    pMS->TopTeamIndex = top;

	    HiLiteState = -1;
	}
    }

    OldContext = SetContext (SpaceContext);
    SetContextFont (MicroFont);
    BatchGraphics ();
    if (HiLiteState != -1)
    {
	bot = pMS->CurIndex - top;
	Text.baseline.y = y + bot * ENTRY_HEIGHT;
	Text.lpStr = (LPBYTE)pMS->FileList[bot].TeamName;
	Text.CharCount = (COUNT)~0;
	SetContextForeGroundColor (
		HiLiteState == 0
		? BUILD_COLOR (MAKE_RGB15 (15, 16, 27), 0x00)
		: BUILD_COLOR (MAKE_RGB15 (23, 24, 29), 0x00)
		);
	DrawText (&Text);

	rtText.baseline.y = Text.baseline.y;
	sprintf (buf, "%d", GetTeamValue (&pMS->FileList[bot]));
	rtText.CharCount = (COUNT)~0;
	DrawText (&rtText);
    }
    else
    {
	COUNT	teams_left;

	DrawMeleeIcon (28);

	Text.baseline.y = y;
	if (teams_left = (COUNT)(
		GetDirEntryTableCount (pMS->TeamDE) + NUM_PREBUILT - top
		))
	{
	    bot = top - 1;
	    do
	    {
		COUNT	row, col;
		STAMP	s;

		if (++bot < NUM_PREBUILT)
		    pMS->FileList[bot - top] = pMS->PreBuiltList[bot];
		else
		{
		    FILE	*load_fp;

		    GetDirEntryContents (SetAbsDirEntryTableIndex (
			    pMS->TeamDE, bot - NUM_PREBUILT
			    ), (STRINGPTR)buf, FALSE);
		    if (load_fp = OpenResFile (buf, "rb", NULL_PTR, 0, NULL_PTR))
		    {
			if (LengthResFile (load_fp) != sizeof (pMS->FileList[bot - top])
				|| ReadTeamImage (&pMS->FileList[bot - top], load_fp) == -1)
			    pMS->FileList[bot - top] = pMS->PreBuiltList[0];
			CloseResFile (load_fp);
		    }
		}

		{
		    Text.lpStr = (LPBYTE)pMS->FileList[bot - top].TeamName;
		    Text.CharCount = (COUNT)~0;
		    SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (15, 16, 27), 0x00));
		    DrawText (&Text);

		    rtText.baseline.y = Text.baseline.y;
		    sprintf (buf, "%d", GetTeamValue (&pMS->FileList[bot - top]));
		    rtText.CharCount = (COUNT)~0;
		    DrawText (&rtText);
		}

		s.origin.x = Text.baseline.x + 1;
		s.origin.y = Text.baseline.y + 4;
		for (row = 0; row < NUM_MELEE_ROWS; ++row)
		{
		    for (col = 0; col < NUM_MELEE_COLUMNS; ++col)
		    {
			BYTE	StarShip;
			
			if ((StarShip = pMS->FileList[bot - top].ShipList[row][col])
				!= (BYTE)~0)
			{
			    HSTARSHIP	hStarShip;
			    STARSHIPPTR	StarShipPtr;

			    hStarShip = GetStarShipFromIndex (&master_q, StarShip);
			    StarShipPtr = LockStarShip (&master_q, hStarShip);
			    s.frame = StarShipPtr->RaceDescPtr->ship_info.icons;
			    DrawStamp (&s);
			    s.origin.x += 18;
			    UnlockStarShip (&master_q, hStarShip);
			}
		    }
		}

		Text.baseline.y += ENTRY_HEIGHT;
	    } while (--teams_left && bot - top < MAX_VIS_TEAMS - 1);
	    pMS->BotTeamIndex = bot;
	}
    }
    UnbatchGraphics ();
    SetContext (OldContext);
}

PROC(STATIC
BOOLEAN	DoLoadTeam, (InputState, pMS),
    ARG		(INPUT_STATE	InputState)
    ARG_END	(PMELEE_STATE	pMS)
)
{
    SIZE	index;

    if (InputState & DEVICE_EXIT)
	return (FALSE);

    if (!pMS->Initialized)
    {
	SetSemaphore (&GraphicsSem);
	Select (pMS->MeleeOption);
	if ((pMS->TopTeamIndex = pMS->CurIndex) == (COUNT)~0)
	    pMS->TopTeamIndex = pMS->CurIndex = 0;
	else if (pMS->TopTeamIndex <= MAX_VIS_TEAMS / 2)
	    pMS->TopTeamIndex = 0;
	else
	    pMS->TopTeamIndex -= MAX_VIS_TEAMS / 2;

	DrawFileStrings (pMS, -1);
	pMS->Initialized = TRUE;
	pMS->InputFunc = DoLoadTeam;
	ClearSemaphore (&GraphicsSem);
    }
    else if (InputState & (DEVICE_BUTTON1 | DEVICE_BUTTON2))
    {
	if (!(InputState & DEVICE_BUTTON2))
	{
	    pMS->TeamImage[pMS->side] = pMS->FileList[
		    pMS->CurIndex - pMS->TopTeamIndex
		    ];
	    pMS->star_bucks[pMS->side] =
		    GetTeamValue (&pMS->TeamImage[pMS->side]);
	}

	pMS->InputFunc = DoMelee;
	{
	    RECT	r;
	    
	    GetFrameRect (SetAbsFrameIndex (MeleeFrame, 28), &r);
	    SetSemaphore (&GraphicsSem);
	    RepairMeleeFrame (&r);
	    ClearSemaphore (&GraphicsSem);
	}
	InTime = GetTimeCounter ();
    }
    else
    {
	SIZE	old_index, NewTop;

	NewTop = pMS->TopTeamIndex;
	index = old_index = pMS->CurIndex;
	if (GetInputYComponent (InputState) < 0)
	{
	    if (index-- == 0)
		index = 0;

	    if (index < NewTop && (NewTop -= MAX_VIS_TEAMS) < 0)
		NewTop = 0;
	}
	else if (GetInputYComponent (InputState) > 0)
	{
	    if (index < GetDirEntryTableCount (pMS->TeamDE) + NUM_PREBUILT - 1)
		++index;

	    if (index > pMS->BotTeamIndex)
		NewTop = index;
	}
	else if (InputState & DEVICE_LEFTSHIFT)
	{
	    if ((index -= MAX_VIS_TEAMS) < 0)
		index = NewTop = 0;
	    else
	    {
		if ((NewTop -= MAX_VIS_TEAMS) < 0)
		    NewTop = 0;
	    }
	}
	else if (InputState & DEVICE_RIGHTSHIFT)
	{
	    if ((index += MAX_VIS_TEAMS) <
		    GetDirEntryTableCount (pMS->TeamDE) + NUM_PREBUILT)
		NewTop += MAX_VIS_TEAMS;
	    else
	    {
		index = GetDirEntryTableCount (pMS->TeamDE) + NUM_PREBUILT - 1;
		if (index - (MAX_VIS_TEAMS - 1) > NewTop)
		{
		    if ((NewTop = index - (MAX_VIS_TEAMS - 1)) < 0)
			NewTop = 0;
		}
	    }
	}

	if (index != old_index)
	{
	    SetSemaphore (&GraphicsSem);
	    if (NewTop == pMS->TopTeamIndex)
		Deselect (pMS->MeleeOption);
	    else
	    {
		pMS->TopTeamIndex = NewTop;
		DrawFileStrings (pMS, -1);
	    }
	    pMS->CurIndex = index;
	    ClearSemaphore (&GraphicsSem);
	}
    }

    return (TRUE);
}

PROC(STATIC
BYTE DoTextEntry, (pMS, InputState),
    ARG		(PMELEE_STATE	pMS)
    ARG_END	(INPUT_STATE	InputState)
)
{
    BYTE	ch;
    PBYTE	pStr, pBaseStr;
    COUNT	len, max_chars;
    PROC_GLOBAL(
    BYTE GetJoystickChar, (InputState),
	ARG_END	(INPUT_STATE	InputState)
    );

    if (pMS->MeleeOption == EDIT_MELEE)
    {
	pBaseStr = (PBYTE)pMS->TeamImage[pMS->side].TeamName;
	max_chars = sizeof (pMS->TeamImage[pMS->side].TeamName) - 1;
    }

    pStr = &pBaseStr[pMS->CurIndex];
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
	    if (pMS->CurIndex)
	    {
		MEMMOVE (pStr - 1, pStr, len + 1);
		--pMS->CurIndex;

		len = (COUNT)~0;
	    }
	    break;
	default:
	    if (isprint (ch) && (len || pStr - pBaseStr < max_chars))
	    {
		*pStr = ch;
		if (len == 0)
		    *(pStr + 1) = '\0';

		len = (COUNT)~0;
	    }
	    break;
    }

    if (len == (COUNT)~0)
	DrawTeamString (pMS, 1);
    else if (GetInputXComponent (InputState) < 0)
    {
	if (pMS->CurIndex)
	{
	    --pMS->CurIndex;
	    DrawTeamString (pMS, 1);
	}
    }
    else if (GetInputXComponent (InputState) > 0)
    {
	if (len)
	{
	    ++pMS->CurIndex;
	    DrawTeamString (pMS, 1);
	}
    }

    return (ch);
}

PROC(STATIC
int WriteTeamImage, (pTI, save_fp),
    ARG		(TEAM_IMAGE	*pTI)
    ARG_END	(FILE		*save_fp)
)
{
    return (WriteResFile (pTI, sizeof (*pTI), 1, save_fp));
}

PROC(STATIC
void LoadTeamList, (pMS, pbuf),
    ARG		(PMELEE_STATE	pMS)
    ARG_END	(char		*pbuf)
)
{
    COUNT	num_entries;

GetNewList:
    DestroyDirEntryTable (ReleaseDirEntryTable (pMS->TeamDE));
    pMS->TeamDE = CaptureDirEntryTable (
	    LoadDirEntryTable ("*.mle", &num_entries)
	    );

    pMS->CurIndex = 0;
    while (num_entries--)
    {
	int		status;
	FILE		*load_fp;
	TEAM_IMAGE	TI;
	char		loc_buf[60];

	GetDirEntryContents (pMS->TeamDE, (STRINGPTR)loc_buf, FALSE);
	if ((load_fp = OpenResFile (loc_buf, "rb", NULL_PTR, 0, NULL_PTR)) == 0)
	    status = -1;
	else
	{
	    if (LengthResFile (load_fp) != sizeof (TI))
		status = -1;
	    else
		status = ReadTeamImage (&TI, load_fp);
	    CloseResFile (load_fp);
	}
	if (status == -1)
	{
	    DeleteResFile (loc_buf);
	    goto GetNewList;
	}

	if (pbuf && strcasecmp (loc_buf, pbuf) == 0)
	{
	    pMS->CurIndex = GetDirEntryTableIndex (pMS->TeamDE) + NUM_PREBUILT;
	    pbuf = 0;
	}
	pMS->TeamDE = SetRelDirEntryTableIndex (pMS->TeamDE, 1);
    }
    pMS->TeamDE = SetAbsDirEntryTableIndex (pMS->TeamDE, 0);
}

PROC(STATIC
BOOLEAN DoSaveTeam, (pMS),
    ARG_END	(PMELEE_STATE	pMS)
)
{
    STAMP	MsgStamp;
    char	buf[60];
    FILE	*save_fp;
    CONTEXT	OldContext;

RetrySave:
    SetSemaphore (&GraphicsSem);
    OldContext = SetContext (ScreenContext);
    ConfirmSaveLoad (&MsgStamp);
    sprintf (buf, "%s.mle", pMS->TeamImage[pMS->side].TeamName);
    if (save_fp = OpenResFile (buf, "wb",
	    NULL_PTR, sizeof (pMS->TeamImage[pMS->side]), NULL_PTR))
    {
	BOOLEAN	err;

	err = (BOOLEAN)(WriteTeamImage (&pMS->TeamImage[pMS->side], save_fp) == 0);
	if (CloseResFile (save_fp) == 0)
	    err = TRUE;
	if (err)
	    save_fp = 0;
    }

    pMS->CurIndex = 0;
    if (save_fp == 0)
    {
	DrawStamp (&MsgStamp);
	DestroyDrawable (ReleaseDrawable (MsgStamp.frame));
	SetContext (OldContext);
	ClearSemaphore (&GraphicsSem);

	DeleteResFile (buf);
	if (SaveProblem ())
	    goto RetrySave;
    }

    LoadTeamList (pMS, buf);

    if (save_fp)
    {
	DrawStamp (&MsgStamp);
	DestroyDrawable (ReleaseDrawable (MsgStamp.frame));
	SetContext (OldContext);
	ClearSemaphore (&GraphicsSem);
    }
    
    return (save_fp != 0);
}

PROC(STATIC
BOOLEAN	DoEdit, (InputState, pMS),
    ARG		(INPUT_STATE	InputState)
    ARG_END	(PMELEE_STATE	pMS)
)
{
    if (InputState & DEVICE_EXIT)
	return (FALSE);

    if (!pMS->Initialized)
    {
	pMS->CurIndex = pMS->TeamImage[pMS->side].ShipList[pMS->row][pMS->col];
	DrawMeleeShipStrings (pMS, pMS->CurIndex);

	pMS->Initialized = TRUE;
	pMS->InputFunc = DoEdit;
    }
    else if ((pMS->row < NUM_MELEE_ROWS || pMS->CurIndex == (BYTE)~0)
	    && ((InputState & DEVICE_BUTTON2)
	    || (GetInputXComponent (InputState) > 0
	    && (pMS->col == NUM_MELEE_COLUMNS - 1 || pMS->row == NUM_MELEE_ROWS))))
    {
	SetSemaphore (&GraphicsSem);
	Deselect (EDIT_MELEE);
	pMS->CurIndex = (COUNT)~0;
	pMS->MeleeOption = START_MELEE;
	pMS->InputFunc = DoMelee;
	ClearSemaphore (&GraphicsSem);
	InTime = GetTimeCounter ();
    }
    else if (pMS->row < NUM_MELEE_ROWS
	    && (InputState & (DEVICE_BUTTON1 | DEVICE_BUTTON3)))
    {
	if (InputState & DEVICE_BUTTON1)
	    pMS->Initialized = 0;
	else
	    pMS->Initialized = -1;
	DoPickShip (0, pMS);
    }
    else
    {
	COUNT	side, row, col;

	side = pMS->side;
	row = pMS->row;
	col = pMS->col;

	if (row == NUM_MELEE_ROWS)
	{
	    if (pMS->CurIndex != (BYTE)~0)
	    {
		SetSemaphore (&GraphicsSem);
		if ((InputState & DEVICE_BUTTON2)
			|| DoTextEntry (pMS, InputState) == '\n')
		{
		    pMS->CurIndex = (BYTE)~0;
		    DrawTeamString (pMS, 4);
		}
		ClearSemaphore (&GraphicsSem);
		return (TRUE);
	    }
	    else if (InputState & DEVICE_BUTTON1)
	    {
		pMS->CurIndex = 0;
		SetSemaphore (&GraphicsSem);
		DrawTeamString (pMS, 1);
		ClearSemaphore (&GraphicsSem);
		return (TRUE);
	    }
	}

	{
	    if (GetInputXComponent (InputState) < 0)
	    {
		if (col-- == 0)
		    col = 0;
	    }
	    else if (GetInputXComponent (InputState) > 0)
	    {
		if (++col == NUM_MELEE_COLUMNS)
		    col = NUM_MELEE_COLUMNS - 1;
	    }

	    if (GetInputYComponent (InputState) < 0)
	    {
		if (row-- == 0)
		{
		    if (side == 0)
			row = 0;
		    else
		    {
			row = NUM_MELEE_ROWS;
			side = !side;
		    }
		}
	    }
	    else if (GetInputYComponent (InputState) > 0)
	    {
		if (row++ == NUM_MELEE_ROWS)
		{
		    if (side == 1)
			row = NUM_MELEE_ROWS;
		    else
		    {
			row = 0;
			side = !side;
		    }
		}
	    }
	}

	if (col != pMS->col || row != pMS->row || side != pMS->side)
	{
	    SetSemaphore (&GraphicsSem);
	    Deselect (EDIT_MELEE);
	    pMS->side = side;
	    pMS->row = row;
	    pMS->col = col;
	    if (row == NUM_MELEE_ROWS)
		pMS->CurIndex = (BYTE)~0;
	    else
		pMS->CurIndex = pMS->TeamImage[side].ShipList[row][col];
	    ClearSemaphore (&GraphicsSem);

	    DrawMeleeShipStrings (pMS, pMS->CurIndex);
	}
    }

    return (TRUE);
}

PROC(STATIC
BOOLEAN	DoPickShip, (InputState, pMS),
    ARG		(INPUT_STATE	InputState)
    ARG_END	(PMELEE_STATE	pMS)
)
{
    STARSHIPPTR	StarShipPtr;

    if (InputState & DEVICE_EXIT)
	return (FALSE);

    if (pMS->Initialized <= 0)
    {
	if (pMS->CurIndex == (BYTE)~0 && pMS->Initialized == 0)
	    pMS->CurIndex = 0;
	else if (pMS->CurIndex != (BYTE)~0)
	{
	    RECT	r;
	    HSTARSHIP	hStarShip;

	    hStarShip = GetStarShipFromIndex (&master_q, pMS->CurIndex);
	    StarShipPtr = LockStarShip (&master_q, hStarShip);
	    pMS->star_bucks[pMS->side] -=
		    StarShipPtr->RaceDescPtr->ship_info.ship_cost;
	    UnlockStarShip (&master_q, hStarShip);

	    pMS->TeamImage[pMS->side].ShipList[pMS->row][pMS->col] = (BYTE)~0;

	    SetSemaphore (&GraphicsSem);
	    GetShipBox (&r, pMS->side, pMS->row, pMS->col);
	    RepairMeleeFrame (&r);

	    DrawTeamString (pMS, 4);
	    ClearSemaphore (&GraphicsSem);
	}

	if (pMS->Initialized == 0)
	{
	    SetSemaphore (&GraphicsSem);
	    Deselect (EDIT_MELEE);
	    pMS->InputFunc = DoPickShip;
	    DrawPickFrame (pMS);
	    pMS->Initialized = TRUE;
	    ClearSemaphore (&GraphicsSem);
	}
	else
	{
	    SetSemaphore (&GraphicsSem);
	    Deselect (EDIT_MELEE);
	    pMS->Initialized = TRUE;
	    if (++pMS->col == NUM_MELEE_COLUMNS)
	    {
		if (++pMS->row < NUM_MELEE_ROWS)
		    pMS->col = 0;
		else
		{
		    pMS->col = NUM_MELEE_COLUMNS - 1;
		    pMS->row = NUM_MELEE_ROWS - 1;
		}
	    }
	    pMS->CurIndex = pMS->TeamImage[pMS->side].ShipList[pMS->row][pMS->col];
	    ClearSemaphore (&GraphicsSem);

	    DrawMeleeShipStrings (pMS, pMS->CurIndex);
	}
    }
    else if (InputState & (DEVICE_BUTTON1 | DEVICE_BUTTON2))
    {
	pMS->InputFunc = 0;	/* disable ship flashing */

	if (!(InputState & DEVICE_BUTTON2))
	{
	    HSTARSHIP	hStarShip;
	    
	    hStarShip = GetStarShipFromIndex (&master_q, pMS->CurIndex);
	    StarShipPtr = LockStarShip (&master_q, hStarShip);
	    pMS->star_bucks[pMS->side] +=
		    StarShipPtr->RaceDescPtr->ship_info.ship_cost;
	    UnlockStarShip (&master_q, hStarShip);

	    pMS->TeamImage[pMS->side].ShipList[pMS->row][pMS->col] = pMS->CurIndex;
	    SetSemaphore (&GraphicsSem);
	    DrawTeamString (pMS, 4);
	    DrawShipBox (pMS, FALSE);
	    ClearSemaphore (&GraphicsSem);
	    if (++pMS->col == NUM_MELEE_COLUMNS)
	    {
		if (++pMS->row < NUM_MELEE_ROWS)
		    pMS->col = 0;
		else
		{
		    pMS->col = NUM_MELEE_COLUMNS - 1;
		    pMS->row = NUM_MELEE_ROWS - 1;
		}
	    }
	}

	{
	    RECT	r;
	    
	    GetFrameRect (SetAbsFrameIndex (MeleeFrame, 27), &r);
	    SetSemaphore (&GraphicsSem);
	    RepairMeleeFrame (&r);
	    ClearSemaphore (&GraphicsSem);
	}

	pMS->CurIndex = pMS->TeamImage[pMS->side].ShipList[pMS->row][pMS->col];
	DrawMeleeShipStrings (pMS, pMS->CurIndex);

	pMS->InputFunc = DoEdit;

	return (TRUE);
    }
    else if (InputState & DEVICE_BUTTON3)
    {
	DoShipSpin (pMS->CurIndex, (MUSIC_REF)0);
    
	return (TRUE);
    }

    {
	BYTE	NewStarShip;

	NewStarShip = pMS->CurIndex;

	if (GetInputXComponent (InputState) < 0)
	{
	    if (NewStarShip-- % NUM_PICK_COLS == 0)
		NewStarShip += NUM_PICK_COLS;
	}
	else if (GetInputXComponent (InputState) > 0)
	{
	    if (++NewStarShip % NUM_PICK_COLS == 0)
		NewStarShip -= NUM_PICK_COLS;
	}
	
	if (GetInputYComponent (InputState) < 0)
	{
	    if (NewStarShip >= NUM_PICK_COLS)
		NewStarShip -= NUM_PICK_COLS;
	    else
		NewStarShip += NUM_PICK_COLS * (NUM_PICK_ROWS - 1);
	}
	else if (GetInputYComponent (InputState) > 0)
	{
	    if (NewStarShip < NUM_PICK_COLS * (NUM_PICK_ROWS - 1))
		NewStarShip += NUM_PICK_COLS;
	    else
		NewStarShip -= NUM_PICK_COLS * (NUM_PICK_ROWS - 1);
	}

	if (NewStarShip != pMS->CurIndex)
	{
	    SetSemaphore (&GraphicsSem);
	    Deselect (EDIT_MELEE);
	    pMS->CurIndex = NewStarShip;
	    ClearSemaphore (&GraphicsSem);
	    DrawMeleeShipStrings (pMS, NewStarShip);
	}
    }

    return (TRUE);
}

PROC(STATIC
void LoadMeleeInfo, (pMS),
   ARG_END	(PMELEE_STATE	pMS)
)
{
    STAMP	s;
    CONTEXT	OldContext;

    OldContext = SetContext (OffScreenContext);

    DestroyDrawable (ReleaseDrawable (PickMeleeFrame));
    PickMeleeFrame = CaptureDrawable (CreateDrawable (
	    WANT_PIXMAP, MELEE_WIDTH, MELEE_HEIGHT, 2
	    ));
    s.origin.x = s.origin.y = 0;
    s.frame = CaptureDrawable (
	    LoadGraphic (MELEE_PICK_MASK_PMAP_ANIM)
	    );
    SetContextFGFrame (PickMeleeFrame);
    DrawStamp (&s);

    s.frame = IncFrameIndex (s.frame);
    SetContextFGFrame (IncFrameIndex (PickMeleeFrame));
    DrawStamp (&s);
    DestroyDrawable (ReleaseDrawable (s.frame));

    MeleeFrame = CaptureDrawable (
	    LoadGraphic (MELEE_SCREEN_PMAP_ANIM)
	    );

    SetContext (OldContext);
    
    InitSpace ();

    LoadTeamList (pMS, 0);
}

PROC(STATIC
void FreeMeleeInfo, (pMS),
    ARG_END	(PMELEE_STATE	pMS)
)
{
    if (pMS->flash_task)
    {
	SetSemaphore (&GraphicsSem);
	DeleteTask (pMS->flash_task);
	ClearSemaphore (&GraphicsSem);
	pMS->flash_task = 0;
    }
    DestroyDirEntryTable (ReleaseDirEntryTable (pMS->TeamDE));
    pMS->TeamDE = 0;

    UninitSpace ();

    DestroyDrawable (ReleaseDrawable (MeleeFrame));
    MeleeFrame = 0;
}

PROC(STATIC
BOOLEAN	DoMelee, (InputState, pMS),
    ARG		(INPUT_STATE	InputState)
    ARG_END	(PMELEE_STATE	pMS)
)
{
    if (InputState)
    {
	if (InputState & DEVICE_EXIT)
	    return (FALSE);
	InTime = GetTimeCounter ();
    }

    if (!pMS->Initialized)
    {
	pMS->Initialized = TRUE;
	pMS->MeleeOption = START_MELEE;
	SetSemaphore (&GraphicsSem);
	InitMelee (pMS);
	ClearSemaphore (&GraphicsSem);
	{
	    BYTE	clut_buf[] = {FadeAllToColor};
		
	    XFormColorMap ((COLORMAPPTR)clut_buf, ONE_SECOND / 2);
	}
	InTime = GetTimeCounter ();
    }
    else if ((InputState & DEVICE_BUTTON2)
	    || GetInputXComponent (InputState) < 0)
    {
	SetSemaphore (&GraphicsSem);
	Deselect (pMS->MeleeOption);
	ClearSemaphore (&GraphicsSem);
	pMS->MeleeOption = EDIT_MELEE;
	pMS->Initialized = FALSE;
	if (InputState & DEVICE_BUTTON2)
	    pMS->side = pMS->row = pMS->col = 0;
	else
	    pMS->side = 0,
	    pMS->row = NUM_MELEE_ROWS - 1,
	    pMS->col = NUM_MELEE_COLUMNS - 1;
	DoEdit (InputState, pMS);
    }
    else
    {
	MELEE_OPTIONS	NewMeleeOption;

	NewMeleeOption = pMS->MeleeOption;
	if (GetInputYComponent (InputState) < 0)
	{
	    if (NewMeleeOption-- == CONTROLS_TOP)
		NewMeleeOption = CONTROLS_TOP;
	}
	else if (GetInputYComponent (InputState) > 0)
	{
	    if (NewMeleeOption++ == CONTROLS_BOT)
		NewMeleeOption = CONTROLS_BOT;
	}

	if ((PlayerControl[0] & PlayerControl[1] & PSYTRON_CONTROL)
		&& GetTimeCounter () - InTime > ONE_SECOND * 10)
	{
	    InputState = DEVICE_BUTTON1;
	    NewMeleeOption = START_MELEE;
	}

	if (NewMeleeOption != pMS->MeleeOption)
	{
	    SetSemaphore (&GraphicsSem);
	    Deselect (pMS->MeleeOption);
	    pMS->MeleeOption = NewMeleeOption;
	    Select (pMS->MeleeOption);
	    ClearSemaphore (&GraphicsSem);
	}

	if (InputState & DEVICE_BUTTON1)
	{
	    switch (pMS->MeleeOption)
	    {
		case START_MELEE:
		    if (pMS->star_bucks[0] == 0 || pMS->star_bucks[1] == 0)
		    {
			PlaySoundEffect (SetAbsSoundIndex (MenuSounds, 2),
				0, GAME_SOUND_PRIORITY);
			break;
		    }

		    if (pMS->flash_task)
		    {
			SetSemaphore (&GraphicsSem);
			DeleteTask (pMS->flash_task);
			ClearSemaphore (&GraphicsSem);
			pMS->flash_task = 0;
		    }
		    
		    {
			BYTE	black_buf[] = {FadeAllToBlack};
			
			SleepTask (XFormColorMap (
				(COLORMAPPTR)black_buf, ONE_SECOND / 2
				) + 2);
			FlushColorXForms ();
		    }
		    do
		    {
			COUNT	i;

			SetContext (OffScreenContext);
			for (i = 0; i < NUM_SIDES; ++i)
			{
			    COUNT	j, side;
			    RECT	r;
			    TEXT	t;
			    STAMP	s;
			    char	buf[10];

			    side = !i;

			    s.frame = SetAbsFrameIndex (PickMeleeFrame, side);
			    SetContextFGFrame (s.frame);

			    GetFrameRect (s.frame, &r);
			    t.baseline.x = r.extent.width >> 1;
			    t.baseline.y = r.extent.height - NAME_AREA_HEIGHT + 4;
			    r.corner.x += 2;
			    r.corner.y += 2;
			    r.extent.width -= (2 * 2) + (ICON_WIDTH + 2) + 1;
			    r.extent.height -= (2 * 2) + NAME_AREA_HEIGHT;
			    SetContextForeGroundColor (PICK_BG_COLOR);
			    DrawFilledRectangle (&r);
			    r.corner.x += 2;
			    r.extent.width += (ICON_WIDTH + 2) - (2 * 2);
			    r.corner.y += r.extent.height;
			    r.extent.height = NAME_AREA_HEIGHT;
			    DrawFilledRectangle (&r);
			    t.align = ALIGN_CENTER;
			    t.lpStr = (LPBYTE)pMS->TeamImage[i].TeamName;
			    t.CharCount = (COUNT)~0;
			    SetContextFont (TinyFont);
			    SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0xA, 0xA, 0x1F), 0x9));
			    DrawText (&t);

			    sprintf (buf, "%d", pMS->star_bucks[i]);
			    t.baseline.x = 4;
			    t.baseline.y = 7;
			    t.align = ALIGN_LEFT;
			    t.lpStr = (LPBYTE)buf;
			    t.CharCount = (COUNT)~0;
			    SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x4, 0x5, 0x1F), 0x4B));
			    DrawText (&t);

			    for (j = 0; j < NUM_MELEE_ROWS; ++j)
			    {
				COUNT	k;

				for (k = 0; k < NUM_MELEE_COLUMNS; ++k)
				{
				    BYTE	StarShip;

				    if ((StarShip = pMS->TeamImage[i].ShipList[j][k]) != (BYTE)~0)
				    {
					BYTE		ship_cost;
					HSTARSHIP	hStarShip, hBuiltShip;
					STARSHIPPTR	StarShipPtr,
							BuiltShipPtr;

					hStarShip = GetStarShipFromIndex (&master_q, StarShip);
					StarShipPtr = LockStarShip (
						&master_q, hStarShip
						);
					hBuiltShip = Build (&race_q[side],
						StarShipPtr->RaceResIndex,
						1 << side,
						NameCaptain (&race_q[side], StarShipPtr));
					s.origin.x = 4 + ((ICON_WIDTH + 2) * k);
					s.origin.y = 10 + ((ICON_HEIGHT + 2) * j);
					s.frame = StarShipPtr->RaceDescPtr->
						ship_info.icons;
					DrawStamp (&s);
					ship_cost = StarShipPtr->RaceDescPtr->
						ship_info.ship_cost;
					UnlockStarShip (
						&master_q, hStarShip
						);

					BuiltShipPtr = LockStarShip (
						&race_q[side], hBuiltShip
						);
					BuiltShipPtr->ShipFacing =
						(j * NUM_MELEE_COLUMNS) + k;
					BuiltShipPtr->special_counter = ship_cost;
					BuiltShipPtr->captains_name_index =
						StarShipCaptain (BuiltShipPtr);
					BuiltShipPtr->RaceDescPtr =
						StarShipPtr->RaceDescPtr;
					UnlockStarShip (
						&race_q[side], hBuiltShip
						);
				    }
				}
			    }
			}

			while (SoundPlaying ())
			    ;

			SeedRandomNumbers ();
			load_gravity_well (
				(COUNT)random () % NUMBER_OF_PLANET_TYPES
				);
			Battle ();
			free_gravity_well ();

			if (GLOBAL (CurrentActivity) & CHECK_ABORT) return (FALSE);

			{
			    BYTE	black_buf[] = {FadeAllToBlack};
			
			    SleepTask (XFormColorMap (
				    (COLORMAPPTR)black_buf, ONE_SECOND / 2
				    ) + 2);
			    FlushColorXForms ();
			}
		    } while (0 /* !(GLOBAL (CurrentActivity) & CHECK_ABORT) */);
		    GLOBAL (CurrentActivity) = SUPER_MELEE;

		    pMS->Initialized = FALSE;
		    break;
		case LOAD_TOP:
		case LOAD_BOT:
		    pMS->Initialized = FALSE;
		    pMS->side = pMS->MeleeOption == LOAD_TOP ? 0 : 1;
		    DoLoadTeam (InputState, pMS);
		    break;
		case SAVE_TOP:
		case SAVE_BOT:
		    pMS->side = pMS->MeleeOption == SAVE_TOP ? 0 : 1;
		    if (pMS->star_bucks[pMS->side])
			DoSaveTeam (pMS);
		    else
			PlaySoundEffect (SetAbsSoundIndex (MenuSounds, 2),
				0, GAME_SOUND_PRIORITY);
		    break;
		case CONTROLS_TOP:
		case CONTROLS_BOT:
		{
		    COUNT	which_side;

		    which_side = pMS->MeleeOption == CONTROLS_TOP ? 1 : 0;
		    if (PlayerControl[which_side] & HUMAN_CONTROL)
			PlayerControl[which_side] =
				COMPUTER_CONTROL | STANDARD_RATING | JOYSTICK_CONTROL;
		    else if (PlayerControl[which_side] & AWESOME_RATING)
			PlayerControl[which_side] =
				HUMAN_CONTROL | STANDARD_RATING | JOYSTICK_CONTROL;
		    else
			PlayerControl[which_side] = ((
				PlayerControl[which_side]
				& (STANDARD_RATING | GOOD_RATING | AWESOME_RATING)
				) << 1) | COMPUTER_CONTROL | JOYSTICK_CONTROL;
		    SetPlayerInput ();
		    DrawControls (which_side, TRUE);
		    break;
		}
	    }
	}
    }

    return (TRUE);
}

PROC(STATIC
void InitPreBuilt, (pMS),
    ARG_END	(PMELEE_STATE	pMS)
)
{
    strcpy (pMS->PreBuiltList[0].TeamName, "Balanced Team 1");
    pMS->PreBuiltList[0].ShipList[0][0] = MELEE_ANDROSYNTH;
    pMS->PreBuiltList[0].ShipList[0][1] = MELEE_CHMMR;
    pMS->PreBuiltList[0].ShipList[0][2] = MELEE_DRUUGE;
    pMS->PreBuiltList[0].ShipList[0][3] = MELEE_URQUAN;
    pMS->PreBuiltList[0].ShipList[0][4] = MELEE_MELNORME;
    pMS->PreBuiltList[0].ShipList[0][5] = MELEE_ORZ;
    pMS->PreBuiltList[0].ShipList[1][0] = MELEE_SPATHI;
    pMS->PreBuiltList[0].ShipList[1][1] = MELEE_SYREEN;
    pMS->PreBuiltList[0].ShipList[1][2] = MELEE_UTWIG;
    pMS->PreBuiltList[0].ShipList[1][3] = (BYTE)~0;
    pMS->PreBuiltList[0].ShipList[1][4] = (BYTE)~0;
    pMS->PreBuiltList[0].ShipList[1][5] = (BYTE)~0;

    strcpy (pMS->PreBuiltList[1].TeamName, "Balanced Team 2");
    pMS->PreBuiltList[1].ShipList[0][0] = MELEE_ARILOU;
    pMS->PreBuiltList[1].ShipList[0][1] = MELEE_CHENJESU;
    pMS->PreBuiltList[1].ShipList[0][2] = MELEE_EARTHLING;
    pMS->PreBuiltList[1].ShipList[0][3] = MELEE_KOHR_AH;
    pMS->PreBuiltList[1].ShipList[0][4] = MELEE_MYCON;
    pMS->PreBuiltList[1].ShipList[0][5] = MELEE_YEHAT;
    pMS->PreBuiltList[1].ShipList[1][0] = MELEE_PKUNK;
    pMS->PreBuiltList[1].ShipList[1][1] = MELEE_SUPOX;
    pMS->PreBuiltList[1].ShipList[1][2] = MELEE_THRADDASH;
    pMS->PreBuiltList[1].ShipList[1][3] = MELEE_ZOQFOTPIK;
    pMS->PreBuiltList[1].ShipList[1][4] = MELEE_SHOFIXTI;
    pMS->PreBuiltList[1].ShipList[1][5] = (BYTE)~0;

    strcpy (pMS->PreBuiltList[2].TeamName, "200 points");
    pMS->PreBuiltList[2].ShipList[0][0] = MELEE_ANDROSYNTH;
    pMS->PreBuiltList[2].ShipList[0][1] = MELEE_CHMMR;
    pMS->PreBuiltList[2].ShipList[0][2] = MELEE_DRUUGE;
    pMS->PreBuiltList[2].ShipList[0][3] = MELEE_MELNORME;
    pMS->PreBuiltList[2].ShipList[0][4] = MELEE_EARTHLING;
    pMS->PreBuiltList[2].ShipList[0][5] = MELEE_KOHR_AH;
    pMS->PreBuiltList[2].ShipList[1][0] = MELEE_SUPOX;
    pMS->PreBuiltList[2].ShipList[1][1] = MELEE_ORZ;
    pMS->PreBuiltList[2].ShipList[1][2] = MELEE_SPATHI;
    pMS->PreBuiltList[2].ShipList[1][3] = MELEE_ILWRATH;
    pMS->PreBuiltList[2].ShipList[1][4] = MELEE_VUX;
    pMS->PreBuiltList[2].ShipList[1][5] = (BYTE)~0;

    strcpy (pMS->PreBuiltList[3].TeamName, "Behemoth Zenith");
    pMS->PreBuiltList[3].ShipList[0][0] = MELEE_CHENJESU;
    pMS->PreBuiltList[3].ShipList[0][1] = MELEE_CHENJESU;
    pMS->PreBuiltList[3].ShipList[0][2] = MELEE_CHMMR;
    pMS->PreBuiltList[3].ShipList[0][3] = MELEE_CHMMR;
    pMS->PreBuiltList[3].ShipList[0][4] = MELEE_KOHR_AH;
    pMS->PreBuiltList[3].ShipList[0][5] = MELEE_KOHR_AH;
    pMS->PreBuiltList[3].ShipList[1][0] = MELEE_URQUAN;
    pMS->PreBuiltList[3].ShipList[1][1] = MELEE_URQUAN;
    pMS->PreBuiltList[3].ShipList[1][2] = MELEE_UTWIG;
    pMS->PreBuiltList[3].ShipList[1][3] = MELEE_UTWIG;
    pMS->PreBuiltList[3].ShipList[1][4] = (BYTE)~0;
    pMS->PreBuiltList[3].ShipList[1][5] = (BYTE)~0;

    strcpy (pMS->PreBuiltList[4].TeamName, "The Peeled Eyes");
    pMS->PreBuiltList[4].ShipList[0][0] = MELEE_URQUAN;
    pMS->PreBuiltList[4].ShipList[0][1] = MELEE_CHENJESU;
    pMS->PreBuiltList[4].ShipList[0][2] = MELEE_MYCON;
    pMS->PreBuiltList[4].ShipList[0][3] = MELEE_SYREEN;
    pMS->PreBuiltList[4].ShipList[0][4] = MELEE_ZOQFOTPIK;
    pMS->PreBuiltList[4].ShipList[0][5] = MELEE_SHOFIXTI;
    pMS->PreBuiltList[4].ShipList[1][0] = MELEE_EARTHLING;
    pMS->PreBuiltList[4].ShipList[1][1] = MELEE_KOHR_AH;
    pMS->PreBuiltList[4].ShipList[1][2] = MELEE_MELNORME;
    pMS->PreBuiltList[4].ShipList[1][3] = MELEE_DRUUGE;
    pMS->PreBuiltList[4].ShipList[1][4] = MELEE_PKUNK;
    pMS->PreBuiltList[4].ShipList[1][5] = MELEE_ORZ;
}

PROC(
void Melee, (),
    ARG_VOID
)
{
    InitGlobData ();
    {
	MELEE_STATE	MenuState;

	pMeleeState = &MenuState;
	MEMSET (pMeleeState, 0, sizeof (*pMeleeState));

	MenuState.InputFunc = DoMelee;
	MenuState.Initialized = FALSE;

	MenuState.CurIndex = (COUNT)~0;
	InitPreBuilt (&MenuState);

	GLOBAL (CurrentActivity) = SUPER_MELEE;

	GameSounds = CaptureSound (LoadSound (GAME_SOUNDS));
	LoadMeleeInfo (&MenuState);
	{
	    FILE	*load_fp;

	    if (load_fp = OpenResFile ("melee.cfg", "rb", NULL_PTR, 0, NULL_PTR))
	    {
		int	status;

		if (LengthResFile (load_fp) != (1 + sizeof (TEAM_IMAGE)) * 2)
		    status = -1;
		else if ((status = GetResFileChar (load_fp)) != -1)
		{
		    PlayerControl[0] = (BYTE)status;
		    status = ReadTeamImage (&MenuState.TeamImage[0], load_fp);
		    if (status != -1)
		    {
			if ((status = GetResFileChar (load_fp)) != -1)
			{
			    PlayerControl[1] = (BYTE)status;
			    status = ReadTeamImage (&MenuState.TeamImage[1], load_fp);
			}
		    }
		}
		CloseResFile (load_fp);
		if (status == -1)
		    load_fp = 0;
	    }

	    if (load_fp == 0)
	    {
		PlayerControl[0] = HUMAN_CONTROL | STANDARD_RATING | JOYSTICK_CONTROL;
		MenuState.TeamImage[0] = MenuState.PreBuiltList[0];
		PlayerControl[1] = COMPUTER_CONTROL | STANDARD_RATING | JOYSTICK_CONTROL;
		MenuState.TeamImage[1] = MenuState.PreBuiltList[1];
	    }
	}
	SetPlayerInput ();

	MenuState.side = 0;
	MenuState.star_bucks[0] = GetTeamValue (&MenuState.TeamImage[0]);
	MenuState.star_bucks[1] = GetTeamValue (&MenuState.TeamImage[1]);
	DoInput ((PVOID)&MenuState);

	while (SoundPlaying ())
	    ;

	{
	    FILE	*save_fp;
	    BOOLEAN	err;
		
	    err = FALSE;
	    if (save_fp = OpenResFile ("melee.cfg", "wb", NULL_PTR,
		    (1 + sizeof (MenuState.TeamImage[0])) * 2, NULL_PTR))
	    {
		if (PutResFileChar (PlayerControl[0], save_fp) == -1)
		    err = TRUE;
		if (!err && WriteTeamImage (&MenuState.TeamImage[0], save_fp) == 0)
		    err = TRUE;
		if (!err && PutResFileChar (PlayerControl[1], save_fp) == -1)
		    err = TRUE;
		if (!err && WriteTeamImage (&MenuState.TeamImage[1], save_fp) == 0)
		    err = TRUE;
		if (CloseResFile (save_fp) == 0)
		    err = TRUE;
	    }
	    else
		err = TRUE;
		
	    if (err)
	    {
		DeleteResFile ("melee.cfg");
	    }
	}
	FreeMeleeInfo (&MenuState);
	DestroySound (ReleaseSound (GameSounds));
	GameSounds = 0;

	DestroyDrawable (ReleaseDrawable (PickMeleeFrame));
	PickMeleeFrame = 0;

	FlushInput ();
    }
}

