#ifndef _INPLIB_H
#define _INPLIB_H

#include "memlib.h"

typedef LPVOID		INPUT_REF;
typedef MEM_HANDLE	INPUT_DEVICE;
typedef DWORD		INPUT_STATE;

typedef enum
{
    KEYBOARD_DEVICE = 0,
    JOYSTICK_DEVICE,
    MOUSE_DEVICE
} INPUT_DEVTYPE;

/* Serial Keyboard Pseudo ASCII Equivalents */
#define SK_LF_ARROW	((BYTE)128)
#define SK_RT_ARROW	((BYTE)129)
#define SK_UP_ARROW	((BYTE)130)
#define SK_DN_ARROW	((BYTE)131)
#define SK_HOME		((BYTE)132)
#define SK_PAGE_UP	((BYTE)133)
#define SK_END		((BYTE)134)
#define SK_PAGE_DOWN	((BYTE)135)
#define SK_INSERT	((BYTE)136)
#define SK_DELETE	((BYTE)127)

#define SK_LF_SHIFT	((BYTE)137)
#define SK_RT_SHIFT	((BYTE)138)
#define SK_CTL		((BYTE)139)
#define SK_ALT		((BYTE)140)

#define KEY_DOWN	((BYTE)(1 << 0))
#define KEY_UP		((BYTE)(1 << 1))
#define LF_SHIFT_DOWN	((BYTE)(1 << 2))
#define RT_SHIFT_DOWN	((BYTE)(1 << 3))
#define CTL_DOWN	((BYTE)(1 << 4))
#define ALT_DOWN	((BYTE)(1 << 5))

#define SK_F1		((BYTE)141)
#define SK_F2		((BYTE)142)
#define SK_F3		((BYTE)143)
#define SK_F4		((BYTE)144)
#define SK_F5		((BYTE)145)
#define SK_F6		((BYTE)146)
#define SK_F7		((BYTE)147)
#define SK_F8		((BYTE)148)
#define SK_F9		((BYTE)149)
#define SK_F10		((BYTE)150)
#define SK_F11		((BYTE)151)
#define SK_F12		((BYTE)152)

#define DEVTYPE_MASK	0x03
#define GetInputASCII(is)		(LOBYTE (LOWORD (is)))
#define GetInputScan(is)		(HIBYTE (LOWORD (is)))
#define GetInputXComponent(is)		((SBYTE)(LOBYTE (LOWORD (is))))
#define GetInputYComponent(is)		((SBYTE)(HIBYTE (LOWORD (is))))
#define GetInputDevType(is)		((INPUT_DEVTYPE)(HIWORD (is) & DEVTYPE_MASK))
#define SetInputASCII(is,a)		\
	(*(is) = (*(is) & ~0x000000FFL) | (BYTE)(a))
#define SetInputScan(is,s)		\
	(*(is) = (*(is) & ~0x0000FF00L) | (UWORD)((BYTE)(s) << 8))
#define SetInputXComponent(is,x)	\
	(*(is) = (*(is) & ~0x000000FFL) | (BYTE)(x))
#define SetInputYComponent(is,y)	\
	(*(is) = (*(is) & ~0x0000FF00L) | (UWORD)((BYTE)(y) << 8))
#define SetInputDevType(is,t)		\
	(*(is) = (*(is) & ~0x00030000L) | ((DWORD)((t) & DEVTYPE_MASK) << 16))
#define DEVICE_BUTTON1		((INPUT_STATE)1 << 19)
#define DEVICE_BUTTON2		((INPUT_STATE)1 << 20)
#define DEVICE_BUTTON3		((INPUT_STATE)1 << 21)
#define DEVICE_PAUSE		((INPUT_STATE)1 << 22)
#define DEVICE_EXIT		((INPUT_STATE)1 << 23)
#define DEVICE_LEFTSHIFT	((INPUT_STATE)1 << 24)
#define DEVICE_RIGHTSHIFT	((INPUT_STATE)1 << 25)

PROC_GLOBAL(
BOOLEAN InitInput, (PauseKey, ExitKey, PauseFunc),
    ARG		(BYTE	PauseKey)
    ARG		(BYTE	ExitKey)
    ARG_END
    (
	PROC_PARAMETER(
	BOOLEAN (*PauseFunc), (),
	    ARG_VOID
	)
    )
);
PROC_GLOBAL(
BOOLEAN UninitInput, (),
    ARG_VOID
);
PROC_GLOBAL(
INPUT_DEVICE CreateSerialKeyboardDevice, (),
    ARG_VOID
);
PROC_GLOBAL(
INPUT_DEVICE CreateJoystickKeyboardDevice,
	(lfkey, rtkey, topkey, botkey, but1key, but2key),
    ARG		(BYTE		lfkey)
    ARG		(BYTE		rtkey)
    ARG		(BYTE		topkey)
    ARG		(BYTE		botkey)
    ARG		(BYTE		but1key)
    ARG_END	(BYTE		but2key)
);
PROC_GLOBAL(
INPUT_DEVICE CreateJoystickDevice, (port),
    ARG_END	(COUNT		port)
);
PROC_GLOBAL(
INPUT_DEVICE CreateInternalDevice, (input_func),
    ARG_END
    (
	PROC_PARAMETER(
	INPUT_STATE (*input_func), (InputRef, InputState),
	    ARG		(INPUT_REF	InputRef)
	    ARG_END	(INPUT_STATE	InputState)
	)
    )
);
PROC_GLOBAL(
BOOLEAN DestroyInputDevice, (InputDevice),
    ARG_END	(INPUT_DEVICE	InputDevice)
);
PROC_GLOBAL(
INPUT_REF CaptureInputDevice, (InputDevice),
    ARG_END	(INPUT_DEVICE	InputDevice)
);
PROC_GLOBAL(
INPUT_DEVICE ReleaseInputDevice, (InputRef),
    ARG_END	(INPUT_REF	InputRef)
);
PROC_GLOBAL(
INPUT_STATE GetInputState, (InputRef),
    ARG_END	(INPUT_REF	InputRef)
);
PROC_GLOBAL(
INPUT_STATE AnyButtonPress, (DetectSpecial),
    ARG_END	(BOOLEAN	DetectSpecial)
);

PROC_GLOBAL(
void FlushInput, (),
    ARG_VOID
);
PROC_GLOBAL(
BYTE KeyDown, (which_scan),
    ARG_END	(BYTE	which_scan)
);
PROC_GLOBAL(
BYTE ScanToASCII, (which_scan),
    ARG_END	(BYTE	which_scan)
);
PROC_GLOBAL(
BYTE ASCIIToScan, (which_key),
    ARG_END	(BYTE	which_key)
);
PROC_GLOBAL(
BYTE GetASCIIKey, (),
    ARG_VOID
);
PROC_GLOBAL(
BYTE KeyHit, (),
    ARG_VOID
);
PROC_GLOBAL(
BYTE GetKeyState, (key),
    ARG_END	(BYTE	key)
);

PROC_GLOBAL(
BOOLEAN FindMouse, (),
    ARG_VOID
);
PROC_GLOBAL(
void MoveMouse, (x, y),
    ARG		(SWORD	x)
    ARG_END	(SWORD	y)
);
PROC_GLOBAL(
BYTE LocateMouse, (px, py),
    ARG		(PSWORD	px)
    ARG_END	(PSWORD	py)
);

#endif /* _INPLIB_H */


