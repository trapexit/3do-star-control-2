#include <ctype.h>
#include "starcon.h"
#include "lander.h"

#define NUM_CELL_COLS	34
#define NUM_CELL_ROWS	11

extern FRAME	SpaceJunkFrame;

PROC(STATIC
void near ClearReportArea, (page_cells),
    ARG_END	(COUNT	page_cells)
)
{
    COUNT	i;
    RECT	r;
    STAMP	s;
    PRIMITIVE	prim_row[NUM_CELL_COLS];

    s.frame = SetAbsFrameIndex (SpaceJunkFrame, 18);
    GetFrameRect (s.frame, &r);
    s.origin.x = 1 + (r.extent.width >> 1);
    s.origin.y = 1;
    for (i = 0; i < NUM_CELL_COLS; ++i)
    {
	prim_row[i].Object.Stamp = s;
	s.origin.x += r.extent.width + 1;

	SetPrimNextLink (&prim_row[i], i + 1);
	SetPrimType (&prim_row[i], STAMPFILL_PRIM);
	SetPrimColor (&prim_row[i], BUILD_COLOR (MAKE_RGB15 (0x00, 0x07, 0x00), 0x57));
    }
    SetPrimNextLink (&prim_row[i - 1], END_OF_LIST);

    if (page_cells == 0)
    {
	SetContextBackGroundColor (BLACK_COLOR);
	ClearDrawable ();
    }
    SetContextClipping (FALSE);
    for (i = 0; i < NUM_CELL_ROWS; ++i)
    {
	DrawBatch (prim_row, 0, 0);
	r.corner.y -= r.extent.height + 1;
	SetFrameHot (s.frame, MAKE_HOT_SPOT (r.corner.x, r.corner.y));
    }
    SetContextClipping (TRUE);
    r.corner.y = 0;
    SetFrameHot (s.frame, MAKE_HOT_SPOT (r.corner.x, r.corner.y));
}

PROC(STATIC
void near MakeReport, (ReadOutSounds, lpStr, StrLen),
    ARG		(SOUND	ReadOutSounds)
    ARG		(LPSTR	lpStr)
    ARG_END	(COUNT	StrLen)
)
{
    BYTE	ButtonState;
    char	last_c;
    COUNT	row_cells, page_cells;
    BOOLEAN	Sleepy;
    RECT	r;
    TEXT	t;
    char	end_page_buf[] = {'(', 'M', 'O', 'R', 'E', ')', '\n'};

    GetFrameRect (SetAbsFrameIndex (SpaceJunkFrame, 18), &r);

    t.align = ALIGN_LEFT;
    t.CharCount = 1;
    t.lpStr = (LPBYTE)lpStr;
    last_c = *lpStr;

    Sleepy = TRUE;
    page_cells = 0;
    ClearSemaphore (&GraphicsSem);

    FlushInput ();
    goto InitPageCell;
    while (StrLen)
    {
	COUNT	col_cells;
	LPSTR	lpLastStr;

	lpLastStr = (LPSTR)t.lpStr;

	col_cells = 0;
	if (row_cells == NUM_CELL_ROWS - 1 && StrLen > NUM_CELL_COLS)
	{
	    col_cells = (NUM_CELL_COLS >> 1) - (sizeof (end_page_buf) >> 1);
	    t.lpStr = (LPBYTE)end_page_buf;
	    StrLen += sizeof (end_page_buf);
	}
	t.baseline.x = 1 + (r.extent.width >> 1)
		+ (col_cells * (r.extent.width + 1));
	do
	{
	    COUNT	word_chars;
	    LPSTR	lpStr;

	    lpStr = (LPSTR)t.lpStr;
	    while (isgraph (*lpStr))
		++lpStr;

	    word_chars = (COUNT)lpStr - (COUNT)t.lpStr;
	    if ((col_cells += word_chars) <= NUM_CELL_COLS)
	    {
		DWORD	TimeOut;

		if (StrLen -= word_chars)
		    --StrLen;
		TimeOut = GetTimeCounter ();
		while (word_chars--)
		{
		    if (!Sleepy)
			DrawText (&t);
		    else
		    {
			SetSemaphore (&GraphicsSem);
			DrawText (&t);
			ClearSemaphore (&GraphicsSem);

			PlaySound (ReadOutSounds, GAME_SOUND_PRIORITY);

			if (t.lpStr[0] == ',')
			    TimeOut += ONE_SECOND / 4;
			if (t.lpStr[0] == '.')
			    TimeOut += ONE_SECOND / 2;
			else
			    TimeOut += ONE_SECOND / 20;
			if (word_chars == 0)
			    TimeOut += ONE_SECOND / 20;

			while (TaskSwitch () < TimeOut)
			{
			    if (ButtonState)
			    {
				if (!AnyButtonPress (TRUE))
				    ButtonState = 0;
			    }
			    else if (AnyButtonPress (TRUE))
			    {
				Sleepy = FALSE;
				SetSemaphore (&GraphicsSem);
				BatchGraphics ();
				break;
			    }
			}
		    }
		    ++t.lpStr;
		    t.baseline.x += r.extent.width + 1;
		}

		++col_cells;
		last_c = *t.lpStr++;
		t.baseline.x += r.extent.width + 1;
	    }
	} while (col_cells <= NUM_CELL_COLS && last_c != '\n' && StrLen);

	t.baseline.y += r.extent.height + 1;
	if (++row_cells == NUM_CELL_ROWS || StrLen == 0)
	{
	    t.lpStr = (LPBYTE)lpLastStr;
	    if (!Sleepy)
	    {
		UnbatchGraphics ();
		ClearSemaphore (&GraphicsSem);
	    }

	    while (AnyButtonPress (TRUE))
		TaskSwitch ();

	    while (!AnyButtonPress (TRUE))
		TaskSwitch ();
InitPageCell:
	    ButtonState = 1;
	    t.baseline.y = r.extent.height + 1;
	    row_cells = 0;
	    if (StrLen)
	    {
		SetSemaphore (&GraphicsSem);
		if (!Sleepy)
		    BatchGraphics ();
		ClearReportArea (page_cells++);
		SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0, 0x1F, 0), 0xFF));
		if (Sleepy)
		    ClearSemaphore (&GraphicsSem);
	    }
	}
    }
    SetSemaphore (&GraphicsSem);
}

PROC(
void DoDiscoveryReport, (ReadOutSounds),
    ARG_END	(SOUND	ReadOutSounds)
)
{
    POINT	old_curs;
    CONTEXT	OldContext;
    FRAME	OldFrame;
    PROC_GLOBAL(
    void DrawScannedObjects, (Reversed),
	ARG_END	(BOOLEAN	Reversed)
    );

    if (pMenuState)
    {
	old_curs = pMenuState->flash_rect0.corner;
	pMenuState->flash_rect0.corner.x =	/* disable cursor */
		pMenuState->flash_rect0.corner.x = -1000;
    }

    OldContext = SetContext (ScanContext);
    OldFrame = SetContextBGFrame ((FRAME)0);
    {
	FONT	OldFont;

	OldFont = SetContextFont (pSolarSysState->SysInfo.PlanetInfo.LanderFont);
	MakeReport (ReadOutSounds,
		(LPSTR)GetStringAddress (pSolarSysState->SysInfo.PlanetInfo.DiscoveryString),
		GetStringLength (pSolarSysState->SysInfo.PlanetInfo.DiscoveryString));
	SetContextFont (OldFont);
    }
    SetContextBGFrame (OldFrame);
#ifdef OLD
    ClearDrawable ();
    if (pSolarSysState->MenuState.Initialized >= 3)
	DrawScannedObjects (FALSE);
#else /* !OLD */
    if (pSolarSysState->MenuState.Initialized < 3)
	ClearDrawable ();
#endif /* OLD */
    SetContext (OldContext);

    ClearSemaphore (&GraphicsSem);
    FlushInput ();
    while (AnyButtonPress (TRUE))
	TaskSwitch ();
    SetSemaphore (&GraphicsSem);

    if (pMenuState)
	pMenuState->flash_rect0.corner = old_curs;
}


