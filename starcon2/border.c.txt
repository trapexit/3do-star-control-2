#include "starcon.h"

PROC(
void InitSISContexts, (),
    ARG_VOID
)
{
    RECT	r;

    SetContext (StatusContext);
    SetContextDrawState (DEST_PIXMAP | DRAW_REPLACE);

    SetContext (SpaceContext);
    SetContextFGFrame (Screen);
    SetContextDrawState (DEST_PIXMAP | DRAW_REPLACE);

    r.corner.x = SIS_ORG_X;
    r.corner.y = SIS_ORG_Y;
    r.extent.width = SIS_SCREEN_WIDTH;
    r.extent.height = SIS_SCREEN_HEIGHT;
    SetContextClipRect (&r);
}

PROC(
void DrawSISFrame, (),
    ARG_VOID
)
{
    RECT	r;

    SetSemaphore (&GraphicsSem);
    SetContext (ScreenContext);
    SetContextDrawState (DEST_PIXMAP | DRAW_REPLACE);

    BatchGraphics ();
    FindDisplayPage (Screen, 0, 0);
    {
	SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0xA, 0xA, 0xA), 0x08));
	r.corner.x = 0;
	r.corner.y = 0;
	r.extent.width = SIS_ORG_X + SIS_SCREEN_WIDTH + 1;
	r.extent.height = SIS_ORG_Y - 1;
	DrawFilledRectangle (&r);
	r.corner.x = 0;
	r.corner.y = 0;
	r.extent.width = SIS_ORG_X - 1;
	r.extent.height = SIS_ORG_Y + SIS_SCREEN_HEIGHT + 1;
	DrawFilledRectangle (&r);
	r.corner.x = 0;
	r.corner.y = r.extent.height;
	r.extent.width = SIS_ORG_X + SIS_SCREEN_WIDTH + 1;
	r.extent.height = SCREEN_HEIGHT - SIS_ORG_Y + SIS_SCREEN_HEIGHT;
	DrawFilledRectangle (&r);
	r.corner.x = SIS_ORG_X + SIS_SCREEN_WIDTH + 1;
	r.corner.y = 0;
	r.extent.width = SCREEN_WIDTH - r.corner.x;
	r.extent.height = SCREEN_HEIGHT;
	DrawFilledRectangle (&r);

	r.corner.x = SIS_ORG_X - 1;
	r.corner.y = SIS_ORG_Y - 1;
	r.extent.width = SIS_SCREEN_WIDTH + 2;
	r.extent.height = SIS_SCREEN_HEIGHT + 2;
	DrawStarConBox (&r, 1,
		BUILD_COLOR (MAKE_RGB15 (0x10, 0x10, 0x10), 0x19),
		BUILD_COLOR (MAKE_RGB15 (0x8, 0x8, 0x8), 0x1F),
		TRUE, BLACK_COLOR);

	r.corner.y = 0;
	r.extent.height = SIS_ORG_Y;

	r.corner.x = SIS_ORG_X;
	r.extent.width = SIS_SCREEN_WIDTH - 69;
	DrawStarConBox (&r, 1,
		BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0xE), 0x54),
		BUILD_COLOR (MAKE_RGB15 (0x00, 0x1, 0x1C), 0x4E),
		TRUE, BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x14), 0x01));

	r.extent.width = 57;
	r.corner.x = SIS_ORG_X + SIS_SCREEN_WIDTH - 57;
	DrawStarConBox (&r, 1,
		BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0xE), 0x54),
		BUILD_COLOR (MAKE_RGB15 (0x00, 0x1, 0x1C), 0x4E),
		TRUE, BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x14), 0x01));

	SetContextForeGroundColor (BLACK_COLOR);
	r.corner.x = SAFE_X + SPACE_WIDTH - 1;
	r.corner.y = 0;
	r.extent.width = 1;
	r.extent.height = SCREEN_HEIGHT;
	DrawFilledRectangle (&r);
	r.corner.x = SAFE_X + SPACE_WIDTH;
	r.corner.y = SAFE_Y + 139;
	DrawPoint (&r.corner);
	r.corner.x = SCREEN_WIDTH - 1;
	DrawPoint (&r.corner);

	SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x10, 0x10, 0x10), 0x19));
	r.corner.y = 1;
	r.extent.width = 1;
	r.extent.height = SAFE_Y + 7;
	r.corner.x = SIS_ORG_X - 1;
	DrawFilledRectangle (&r);
	r.corner.x = SIS_ORG_X + SIS_SCREEN_WIDTH - 57 - 1;
	DrawFilledRectangle (&r);

	r.corner.x = 0;
	r.corner.y = SCREEN_HEIGHT - 1;
	r.extent.width = SAFE_X + SPACE_WIDTH - 1;
	r.extent.height = 1;
	DrawFilledRectangle (&r);
	r.corner.x = SAFE_X + SPACE_WIDTH - 2;
	r.corner.y = 0;
	r.extent.width = 1;
	r.extent.height = SCREEN_HEIGHT - 1;
	DrawFilledRectangle (&r);
	r.corner.x = SCREEN_WIDTH - 1;
	r.corner.y = 0;
	r.extent.width = 1;
	r.extent.height = SAFE_Y + 139;
	DrawFilledRectangle (&r);
	r.corner.x = SAFE_X + SPACE_WIDTH;
	r.corner.y = SCREEN_HEIGHT - 1;
	r.extent.width = SCREEN_WIDTH - r.corner.x;
	r.extent.height = 1;
	DrawFilledRectangle (&r);
	r.corner.x = SCREEN_WIDTH - 1;
	r.corner.y = SAFE_Y + 140;
	r.extent.width = 1;
	r.extent.height = (SCREEN_HEIGHT - 1) - r.corner.y;
	DrawFilledRectangle (&r);

	SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x8, 0x8, 0x8), 0x1F));
	r.corner.y = 1;
	r.extent.width = 1;
	r.extent.height = SAFE_Y + 7;
	r.corner.x = SIS_ORG_X + (SIS_SCREEN_WIDTH - 69);
	DrawFilledRectangle (&r);
	r.corner.x = SIS_ORG_X + SIS_SCREEN_WIDTH;
	++r.extent.height;
	DrawFilledRectangle (&r);
	r.corner.y = 0;
	r.extent.width = (SAFE_X + SPACE_WIDTH - 2) - r.corner.x;
	r.extent.height = 1;
	DrawFilledRectangle (&r);
	r.corner.x = 0;
	r.extent.width = SIS_ORG_X - r.corner.x;
	DrawFilledRectangle (&r);
	r.corner.x = SIS_ORG_X + (SIS_SCREEN_WIDTH - 69);
	r.extent.width = (SIS_ORG_X + SIS_SCREEN_WIDTH - 57) - r.corner.x;
	DrawFilledRectangle (&r);

	r.corner.x = 0;
	r.corner.y = 1;
	r.extent.width = 1;
	r.extent.height = (SCREEN_HEIGHT - 1) - r.corner.y;
	DrawFilledRectangle (&r);
	r.corner.x = SAFE_X + SPACE_WIDTH;
	r.corner.y = 0;
	r.extent.width = 1;
	r.extent.height = SAFE_Y + 139;
	DrawFilledRectangle (&r);
	r.corner.x = SAFE_X + SPACE_WIDTH + 1;
	r.corner.y = SAFE_Y + 139;
	r.extent.width = STATUS_WIDTH - 2;
	r.extent.height = 1;
	DrawFilledRectangle (&r);
	r.corner.x = SAFE_X + SPACE_WIDTH;
	r.corner.y = SAFE_Y + 140;
	r.extent.width = 1;
	r.extent.height = SCREEN_HEIGHT - r.corner.y;
	DrawFilledRectangle (&r);
    }

    InitSISContexts ();
    ClearSISRect (DRAW_SIS_DISPLAY);
    ScreenOrigin (Screen, 0, 0);
    UnbatchGraphics ();

    ClearSemaphore (&GraphicsSem);
}

