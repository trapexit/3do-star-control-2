#ifndef _STARBASE_H
#define _STARBASE_H

enum
{
    TALK_COMMANDER = 0,
    OUTFIT_STARSHIP,
    SHIPYARD,
    DEPART_BASE
};
typedef BYTE	STARBASE_STATE;

#define FIELD_WIDTH	(STATUS_WIDTH - 5)

typedef struct menu_state
{
    PROC_PARAMETER(
    BOOLEAN (*InputFunc), (InputState, pMS),
	ARG	(INPUT_STATE		InputState)
	ARG_END	(struct menu_state	*pMS)
    );
    COUNT		MenuRepeatDelay;

    SIZE		Initialized;

    BYTE		CurState;
    FRAME		CurFrame;
    STRING		CurString;
    POINT		first_item;
    SIZE		delta_item;

    FRAME		ModuleFrame;
    TASK		flash_task;
    RECT		flash_rect0,
			flash_rect1;
    FRAME		flash_frame0,
			flash_frame1;

    MUSIC_REF		hMusic;
} MENU_STATE;
typedef MENU_STATE	*PMENU_STATE;

extern PMENU_STATE	pMenuState;

PROC_GLOBAL(
void VisitStarBase, (),
    ARG_VOID
);
PROC_GLOBAL(
BOOLEAN	DoStarBase, (InputState, pMS),
    ARG		(INPUT_STATE	InputState)
    ARG_END	(PMENU_STATE	pMS)
);
PROC_GLOBAL(
BOOLEAN	DoOutfit, (InputState, pMS),
    ARG		(INPUT_STATE	InputState)
    ARG_END	(PMENU_STATE	pMS)
);
PROC_GLOBAL(
BOOLEAN	DoShipyard, (InputState, pMS),
    ARG		(INPUT_STATE	InputState)
    ARG_END	(PMENU_STATE	pMS)
);
PROC_GLOBAL(
void DrawShipPiece, (pMS, which_piece, which_slot, DrawBluePrint),
    ARG		(PMENU_STATE	pMS)
    ARG		(COUNT		which_piece)
    ARG		(COUNT		which_slot)
    ARG_END	(BOOLEAN	DrawBluePrint)
);

extern CONST char	far starbase_str_array[][20];

#endif /* _STARBASE_H */

