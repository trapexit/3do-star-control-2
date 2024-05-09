#ifndef _STARCON_H
#define _STARCON_H

#define MAX_PLAYERS		4
#define NUM_PLAYERS		2
#define NUM_SIDES		2

extern int			_screen_width, _screen_height;

#define SCREEN_WIDTH		_VW
#define SCREEN_HEIGHT		_VH
#define SIS_ORG_X		(7 + SAFE_X)
#define SIS_ORG_Y		(10 + SAFE_Y)
#define STATUS_WIDTH		64
#define STATUS_HEIGHT		(SCREEN_HEIGHT - (SAFE_Y * 2))
#define SPACE_WIDTH		(SCREEN_WIDTH - STATUS_WIDTH - (SAFE_X * 2))
#define SPACE_HEIGHT		(SCREEN_HEIGHT - (SAFE_Y * 2))
#define SIS_SCREEN_WIDTH	(SPACE_WIDTH - 14)
#define SIS_SCREEN_HEIGHT	(SPACE_HEIGHT - 13)

#include <stdlib.h>
#include <math.h>
#include "gfxlib.h"
#include "inplib.h"
#include "timlib.h"
#include "sndlib.h"
#include "strlib.h"
#include "mathlib.h"
#include "reslib.h"
#include "restypes.h"
#include "resinst.h"
#include "displist.h"
#include "nameref.h"
#include "units.h"
#include "velocity.h"
#include "element.h"
#include "races.h"
#include "weapon.h"
#include "collide.h"
#include "build.h"
#include "intel.h"
#include "sounds.h"
#include "demo.h"
#include "starbase.h"
#include "sis.h"
#include "state.h"
#include "globdata.h"
#include "encount.h"
#include "ramfile.h"

#define HUMAN_CONTROL		(BYTE)(1 << 0)
#define CYBORG_CONTROL		(BYTE)(1 << 1)
#define PSYTRON_CONTROL		(BYTE)(1 << 2)
#define COMPUTER_CONTROL	(CYBORG_CONTROL | PSYTRON_CONTROL)

#define STANDARD_RATING		(BYTE)(1 << 3)
#define GOOD_RATING		(BYTE)(1 << 4)
#define AWESOME_RATING		(BYTE)(1 << 5)

#define KEYBOARD_CONTROL	(BYTE)(1 << 6)
#define JOYSTICK_CONTROL	(BYTE)(1 << 7)
#define DUMMY_CONTROL		(UWORD)(1 << 8)

#define SELECT_RATE		20
#define INVALID_INPUT		((INPUT_STATE)0xFF)
#define INPUT_TIMED_OUT	((INPUT_STATE)0)

#define BLACK_COLOR			BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x00), 0x00)
#define LTGRAY_COLOR			BUILD_COLOR (MAKE_RGB15 (0x14, 0x14, 0x14), 0x07)
#define DKGRAY_COLOR			BUILD_COLOR (MAKE_RGB15 (0x0A, 0x0A, 0x0A), 0x08)
#define WHITE_COLOR			BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1F, 0x1F), 0x0F)

#define NORMAL_ILLUMINATED_COLOR	BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1F, 0x1F), 0x0F)
#define NORMAL_SHADOWED_COLOR		BUILD_COLOR (MAKE_RGB15 (0x0A, 0x0A, 0x0A), 0x08)
#define HIGHLIGHT_ILLUMINATED_COLOR    BUILD_COLOR (MAKE_RGB15 (0x1F, 0x0A, 0x0A), 0x0C)
#define HIGHLIGHT_SHADOWED_COLOR	BUILD_COLOR (MAKE_RGB15 (0x14, 0x00, 0x00), 0x04)
#define MENU_BACKGROUND_COLOR		BUILD_COLOR (MAKE_RGB15 (0x14, 0x14, 0x14), 0x07)
#define MENU_FOREGROUND_COLOR		BUILD_COLOR (MAKE_RGB15 (0x0A, 0x0A, 0x0A), 0x08)
#define MENU_TEXT_COLOR		BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x00), 0x00)
#define MENU_HIGHLIGHT_COLOR		BUILD_COLOR (MAKE_RGB15 (0x14, 0x00, 0x00), 0x04)

#define STATUS_ILLUMINATED_COLOR	BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1F, 0x1F), 0x0F)
#define STATUS_SHADOWED_COLOR		BUILD_COLOR (MAKE_RGB15 (0x0A, 0x0A, 0x0A), 0x08)
#define STATUS_SHAPE_COLOR		BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x00), 0x00)
#define STATUS_SHAPE_OUTLINE_COLOR	BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1F, 0x1F), 0x0F)

#define CONTROL_COLOR			BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x14), 0x01)

#define ALLIANCE_BACKGROUND_COLOR	BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x14), 0x01)
#define HIERARCHY_BACKGROUND_COLOR	BUILD_COLOR (MAKE_RGB15 (0x14, 0x00, 0x00), 0x04)
#define ALLIANCE_TEXT_COLOR		BUILD_COLOR (MAKE_RGB15 (0x0A, 0x1F, 0x1F), 0x0B)
#define HIERARCHY_TEXT_COLOR		BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1F, 0x0A), 0x0E)
#define ALLIANCE_BOX_HIGHLIGHT_COLOR	BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x14), 0x01)
#define HIERARCHY_BOX_HIGHLIGHT_COLOR	HIERARCHY_BACKGROUND_COLOR

#define MESSAGE_BACKGROUND_COLOR	BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x14), 0x01)
#define MESSAGE_TEXT_COLOR		BUILD_COLOR (MAKE_RGB15 (0x1F, 0x1F, 0x0A), 0x0E)

enum
{
    FadeAllToWhite = 250,
    FadeSomeToWhite,
    FadeAllToBlack,
    FadeAllToColor,
    FadeSomeToBlack,
    FadeSomeToColor
};

#define SetPlayerIconColor(wp, z)	\
    if ((wp) == 0) \
	SetContextForeGroundColor ((COLOR)((z) >= 0 ? \
		BUILD_COLOR (MAKE_RGB15 (0xA, 0x1F, 0x1F), 0x0B) : \
		BUILD_COLOR (MAKE_RGB15 (0x00, 0x14, 0x14), 0x03))); \
    else \
	SetContextForeGroundColor ((COLOR)((z) >= 0 ? \
		BUILD_COLOR (MAKE_RGB15 (0x1F, 0xA, 0xA), 0x0C) : \
		BUILD_COLOR (MAKE_RGB15 (0x14, 0xA, 0x00), 0x06)))

#define CONTROL_STRING_INDEX	0
#define USING_STRING_INDEX	1
#define RATING_STRING_INDEX	2
#define OK_STRING_INDEX	3
#define LABEL_STRING_COUNT	(OK_STRING_INDEX + 1)

#define MENU_REPEAT_DELAY	(ONE_SECOND >> 1)

extern ACTIVITY		LastActivity;
extern BYTE		PlayerControl[NUM_PLAYERS];
extern SIZE		cur_player, cur_race;
extern QUEUE		disp_q;
extern SIZE		battle_counter;
extern FRAME		stars_in_space;
extern MEM_HANDLE	hResIndex;
extern CONTEXT		ScreenContext, StatusContext,
			SpaceContext, OffScreenContext,
			RadarContext, TaskContext;
extern FRAME		BackFrame;
extern SIZE		screen_width, screen_height;
extern FRAME		Screen, RadarFrame;
extern FONT		StarConFont, MicroFont, TinyFont;
extern BOOLEAN		PagingEnabled;
extern INPUT_REF	ComputerInput, NormalInput,
			JoystickInput[NUM_PLAYERS],
			PlayerInput[NUM_PLAYERS];
extern FRAME		ActivityFrame;
extern SOUND		MenuSounds, GameSounds;
extern QUEUE		race_q[NUM_PLAYERS];
extern SEMAPHORE	GraphicsSem;

typedef enum
{
    PACKAGE_ERROR,
    LOAD_ERROR,
    SAVE_ERROR
} DISK_ERROR;

PROC_GLOBAL(
void Introduction, (),
    ARG_VOID
);
PROC_GLOBAL(
BOOLEAN TheftProtection, (WaitForInput),
    ARG_END	(BOOLEAN	WaitForInput)
);

PROC_GLOBAL(
void SetFlashRect, (pRect, f),
    ARG		(PRECT	pRect)
    ARG_END	(FRAME	f)
);

PROC_GLOBAL(
void DrawStarConBox,
	(pRect, BorderWidth, TopLeftColor, BottomRightColor, FillInterior, InteriorColor),
    ARG		(PRECT		pRect)
    ARG		(SIZE		BorderWidth)
    ARG		(COLOR		TopLeftColor)
    ARG		(COLOR		BottomRightColor)
    ARG		(BOOLEAN	FillInterior)
    ARG_END	(COLOR		InteriorColor)
);
PROC_GLOBAL(
INPUT_STATE ConfirmExit, (),
    ARG_VOID
);
PROC_GLOBAL(
DWORD SeedRandomNumbers, (),
    ARG_VOID
);
PROC_GLOBAL(
BOOLEAN	StarConDiskError, (pFileName),
    ARG_END	(PSTR	pFileName)
);
PROC_GLOBAL(
void ReportDiskError, (pFileName, ErrorCondition),
    ARG		(PSTR		pFileName)
    ARG_END	(DISK_ERROR	ErrorCondition)
);
PROC_GLOBAL(
void DoInput, (pInputState),
    ARG_END	(PVOID	pInputState)
);
PROC_GLOBAL(
INPUT_STATE game_input, (InputRef, InputState),
    ARG		(INPUT_REF	InputRef)
    ARG_END	(INPUT_STATE	InputState)
);
PROC_GLOBAL(
BOOLEAN Battle, (),
    ARG_VOID
);
PROC_GLOBAL(
void EncounterBattle, (),
    ARG_VOID
);
PROC_GLOBAL(
void SetPlayerInput, (),
    ARG_VOID
);
PROC_GLOBAL(
BOOLEAN InitContexts, (),
    ARG_VOID
);
PROC_GLOBAL(
void UninitContexts, (),
    ARG_VOID
);
PROC_GLOBAL(
COUNT WrapText, (lpStr, len, tarray, field_width),
    ARG		(LPBYTE	lpStr)
    ARG		(COUNT	len)
    ARG		(TEXT	*tarray)
    ARG_END	(SIZE	field_width)
);

PROC_GLOBAL(
void ToggleMusic, (),
    ARG_VOID
);
PROC_GLOBAL(
void PlayMusic, (MusicRef, Continuous, Priority),
    ARG		(MUSIC_REF	MusicRef)
    ARG		(BOOLEAN	Continuous)
    ARG_END	(BYTE		Priority)
);
PROC_GLOBAL(
void StopMusic, (),
    ARG_VOID
);
PROC_GLOBAL(
void ResumeMusic, (),
    ARG_VOID
);
PROC_GLOBAL(
void PauseMusic, (),
    ARG_VOID
);
PROC_GLOBAL(
void ToggleSoundEffect, (),
    ARG_VOID
);
PROC_GLOBAL(
void PlaySoundEffect, (S, Channel, Priority),
    ARG		(SOUND	S)
    ARG		(COUNT	Channel)
    ARG_END	(BYTE	Priority)
);

PROC_GLOBAL(
BOOLEAN CopyProtection, (),
    ARG_VOID
);

/* dummy.c */
PROC_GLOBAL(
void SetSoundTracking, (toggle),
    ARG_END	(BOOLEAN	toggle)
);
PROC_GLOBAL(
void GetSoundTrackingBuf, (pdigi_buf),
    ARG_END	(PSBYTE		pdigi_buf)
);

PROC_GLOBAL(
void StopVid, (),
    ARG_VOID
);
PROC_GLOBAL(
void PlayVid, (VidRef, Continuous),
    ARG		(MEM_HANDLE	VidRef)
    ARG_END	(BOOLEAN	Continuous)
);
PROC_GLOBAL(
MEM_HANDLE LoadVidFile, (pStr),
    ARG_END	(PSTR	pStr)
);
PROC_GLOBAL(
MEM_HANDLE LoadVidInstance, (res),
    ARG_END	(DWORD	res)
);
PROC_GLOBAL(
MEM_HANDLE VidPlaying, (),
    ARG_VOID
);
PROC_GLOBAL(
BOOLEAN DestroyVid, (VidRef),
    ARG_END	(MEM_HANDLE	VidRef)
);

PROC_GLOBAL(
DRAWABLE CreatePixmapRegion, (Frame, lpOrg, width, height),
    ARG		(FRAME		Frame)
    ARG		(LPPOINT	lpOrg)
    ARG		(SIZE		width)
    ARG_END	(SIZE		height)
);

PROC_GLOBAL(
void SetPrimNextLink, (pPrim, Link),
    ARG		(PPRIMITIVE	pPrim)
    ARG_END	(COUNT		Link)
);
PROC_GLOBAL(
COUNT GetPrimNextLink, (pPrim),
    ARG_END	(PPRIMITIVE	pPrim)
);

#include "appglue.h"

#endif /* _STARCON_H */

