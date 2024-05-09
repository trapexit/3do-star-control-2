#include "starcon.h"

CONST char	far category_name[][12] =
{
    "Common",
    "Corrosive",
    "Base Metal",
    "Noble Gas",
    "Rare Earth",
    "Precious",
    "Radioactive",
    "Exotic",
};

PROC(
void near ShowRemainingCapacity, (),
    ARG_VOID
)
{
    RECT	r;
    TEXT	rt;
    CONTEXT	OldContext;
    char	rt_amount_buf[10];

    OldContext = SetContext (StatusContext);
    SetContextFont (TinyFont);

    sprintf (rt_amount_buf, "%u",
	    GetSBayCapacity (NULL_PTR)
	    - GLOBAL_SIS (TotalElementMass));
    rt.baseline.x = 59;
    rt.baseline.y = 113;
    rt.align = ALIGN_RIGHT;
    rt.lpStr = (LPBYTE)rt_amount_buf;
    rt.CharCount = (COUNT)~0;

    r.corner.x = 40;
    r.corner.y = rt.baseline.y - 6;
    r.extent.width = rt.baseline.x - r.corner.x + 1;
    r.extent.height = 7;

    BatchGraphics ();
    SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x14), 0x01));
    DrawFilledRectangle (&r);
    SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0xA, 0xA, 0x1F), 0x09));
    DrawText (&rt);
    UnbatchGraphics ();
    
    SetContext (OldContext);
}

PROC(
void DrawCargoStrings, (OldElement, NewElement),
    ARG		(BYTE	OldElement)
    ARG_END	(BYTE	NewElement)
)
{
    COORD	y, cy;
    TEXT	rt;
    RECT	r;
    CONTEXT	OldContext;
    char	rt_amount_buf[10];

    SetSemaphore (&GraphicsSem);

    OldContext = SetContext (StatusContext);
    SetContextFont (TinyFont);

    BatchGraphics ();

    y = 41;
    rt.align = ALIGN_RIGHT;
    rt.lpStr = (LPBYTE)rt_amount_buf;

    if (OldElement > NUM_ELEMENT_CATEGORIES)
    {
	STAMP		s;
	extern FRAME	misc_data;

	r.corner.x = 2;
	r.extent.width = FIELD_WIDTH + 1;

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
	    ct.lpStr = (LPBYTE)"cargo";
	    ct.CharCount = (COUNT)~0;
	    SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0xA, 0x1F, 0x1F), 0x0B));
	    DrawText (&ct);

	    SetContextFont (TinyFont);
	}

	r.corner.x = 7;
	r.extent.width = 7;
	r.extent.height = 7;

	s.origin.x = r.corner.x + (r.extent.width >> 1);
	s.frame = SetAbsFrameIndex (
		misc_data,
		(NUM_SCANDOT_TRANSITIONS << 1) + 3
		);
	cy = y;

	rt.baseline.y = cy - 7;
	rt.CharCount = 1;

	SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0xA, 0xA, 0x1F), 0x09));
	rt.baseline.x = 32;
	rt_amount_buf[0] = '$';
	DrawText (&rt);

	rt.baseline.x = 58;
	rt_amount_buf[0] = '#';
	DrawText (&rt);

	for (OldElement = 0;
		OldElement < NUM_ELEMENT_CATEGORIES; ++OldElement)
	{
	    SetContextForeGroundColor (BLACK_COLOR);
	    r.corner.y = cy - 6;
	    DrawFilledRectangle (&r);

	    s.origin.y = r.corner.y + (r.extent.height >> 1);
	    DrawStamp (&s);
	    s.frame = SetRelFrameIndex (s.frame, 5);

	    if (OldElement != NewElement)
	    {
		rt.baseline.y = cy;

		SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0xA, 0xA, 0x1F), 0x09));
		rt.baseline.x = 32;
		sprintf (rt_amount_buf, "%u",
			GLOBAL (ElementWorth[OldElement]));
		rt.CharCount = (COUNT)~0;
		DrawText (&rt);

		SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x00, 0x14, 0x14), 0x03));
		rt.baseline.x = 58;
		sprintf (rt_amount_buf, "%u",
			GLOBAL_SIS (ElementAmounts[OldElement]));
		rt.CharCount = (COUNT)~0;
		DrawText (&rt);
	    }

	    cy += 9;
	}

	OldElement = NewElement;

	rt.baseline.y = 125;

	SetContextForeGroundColor (BLACK_COLOR);
	r.corner.y = rt.baseline.y - 6;
	DrawFilledRectangle (&r);

	s.origin.y = r.corner.y + (r.extent.height >> 1);
	s.frame = SetAbsFrameIndex (s.frame, 68);
	DrawStamp (&s);

	SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x00, 0x14, 0x14), 0x03));
	rt.baseline.x = 58;
	sprintf (rt_amount_buf, "%u", GLOBAL_SIS (TotalBioMass));
	rt.CharCount = (COUNT)~0;
	DrawText (&rt);

	r.corner.x = 4;
	r.corner.y = 117;
	r.extent.width = 56;
	r.extent.height = 1;
	SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0xA, 0xA, 0x1F), 0x09));
	DrawFilledRectangle (&r);

	{
	    TEXT	lt;
	    
	    lt.baseline.x = 5;
	    lt.baseline.y = 113;
	    lt.align = ALIGN_LEFT;
	    lt.lpStr = (LPBYTE)"Free";
	    lt.CharCount = (COUNT)~0;
	    DrawText (&lt);
	}

	ShowRemainingCapacity ();
    }

    r.corner.x = 19;
    r.extent.width = 40;
    r.extent.height = 7;

    if (OldElement != NewElement)
    {
	if (OldElement == NUM_ELEMENT_CATEGORIES)
	    cy = 125;
	else
	    cy = y + (OldElement * 9);
	r.corner.y = cy - 6;
	SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x14), 0x01));
	DrawFilledRectangle (&r);

	rt.baseline.y = cy;

	if (OldElement == NUM_ELEMENT_CATEGORIES)
	    sprintf (rt_amount_buf, "%u", GLOBAL_SIS (TotalBioMass));
	else
	{
	    SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0xA, 0xA, 0x1F), 0x09));
	    rt.baseline.x = 32;
	    sprintf (rt_amount_buf, "%u", GLOBAL (ElementWorth[OldElement]));
	    rt.CharCount = (COUNT)~0;
	    DrawText (&rt);
	    sprintf (rt_amount_buf, "%u", GLOBAL_SIS (ElementAmounts[OldElement]));
	}

	SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x00, 0x14, 0x14), 0x03));
	rt.baseline.x = 58;
	rt.CharCount = (COUNT)~0;
	DrawText (&rt);
    }

    if (NewElement != (BYTE)~0)
    {
	if (NewElement == NUM_ELEMENT_CATEGORIES)
	    cy = 125;
	else
	    cy = y + (NewElement * 9);
	r.corner.y = cy - 6;
	SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0xA, 0xA, 0x1F), 0x09));
	DrawFilledRectangle (&r);

	rt.baseline.y = cy;

	if (NewElement == NUM_ELEMENT_CATEGORIES)
	    sprintf (rt_amount_buf, "%u", GLOBAL_SIS (TotalBioMass));
	else
	{
	    SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x00, 0x14, 0x14), 0x03));
	    rt.baseline.x = 32;
	    sprintf (rt_amount_buf, "%u", GLOBAL (ElementWorth[NewElement]));
	    rt.CharCount = (COUNT)~0;
	    DrawText (&rt);
	    sprintf (rt_amount_buf, "%u", GLOBAL_SIS (ElementAmounts[NewElement]));
	}

	SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0xA, 0x1F, 0x1F), 0x0B));
	rt.baseline.x = 58;
	rt.CharCount = (COUNT)~0;
	DrawText (&rt);
    }

    UnbatchGraphics ();
    SetContext (OldContext);
    ClearSemaphore (&GraphicsSem);
}

PROC(STATIC
BOOLEAN	DoDiscardCargo, (InputState, pMS),
    ARG		(INPUT_STATE	InputState)
    ARG_END	(PMENU_STATE	pMS)
)
{
    BYTE	NewState;

    if (GLOBAL (CurrentActivity) & CHECK_ABORT)
	return (FALSE);

    if (!(pMS->Initialized & 1))
    {
	pMS->InputFunc = DoDiscardCargo;
	++pMS->Initialized;

	NewState = pMS->CurState;
	pMS->CurState = (BYTE)~0;
	goto SelectCargo;
    }
    else if (InputState & DEVICE_BUTTON2)
    {
	SetSemaphore (&GraphicsSem);
	ClearSISRect (DRAW_SIS_DISPLAY);
	ClearSemaphore (&GraphicsSem);

	return (FALSE);
    }
    else if (InputState & DEVICE_BUTTON1)
    {
	if (GLOBAL_SIS (ElementAmounts[pMS->CurState - 1]))
	{
	    --GLOBAL_SIS (ElementAmounts[pMS->CurState - 1]);
	    DrawCargoStrings (pMS->CurState - 1, pMS->CurState - 1);

	    SetSemaphore (&GraphicsSem);
	    --GLOBAL_SIS (TotalElementMass);
	    ShowRemainingCapacity ();
	    ClearSemaphore (&GraphicsSem);
	}
    }
    else
    {
	NewState = pMS->CurState - 1;
	if (GetInputXComponent (InputState) < 0
		|| GetInputYComponent (InputState) < 0)
	{
	    if (NewState-- == 0)
		NewState = NUM_ELEMENT_CATEGORIES - 1;
	}
	else if (GetInputXComponent (InputState) > 0
		|| GetInputYComponent (InputState) > 0)
	{
	    if (++NewState == NUM_ELEMENT_CATEGORIES)
		NewState = 0;
	}

	if (++NewState != pMS->CurState)
	{
SelectCargo:
	    DrawCargoStrings (pMS->CurState - 1, NewState - 1);
	    SetSemaphore (&GraphicsSem);
	    DrawStatusMessage ((LPSTR)category_name[NewState - 1]);
	    ClearSemaphore (&GraphicsSem);

	    pMS->CurState = NewState;
	}
    }

    return (TRUE);
}

PROC(
void Cargo, (pMS),
    ARG_END	(PMENU_STATE	pMS)
)
{
    pMS->InputFunc = DoDiscardCargo;
    --pMS->Initialized;
    pMS->CurState = 1;

    SetSemaphore (&GraphicsSem);
    DrawStatusMessage ((LPSTR)~0);
    ClearSemaphore (&GraphicsSem);

    DoInput ((PVOID)pMS);

    pMS->InputFunc = DoFlagshipCommands;
    pMS->CurState = CARGO + 1;
}

