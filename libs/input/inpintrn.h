#ifndef _INPINTRN_H
#define _INPINTRN_H

#include "inplib.h"

#define NUM_INPUTS	6

typedef struct inp_dev
{
    INPUT_DEVICE	ThisDevice;
    union
    {
	BYTE	key_equivalent[NUM_INPUTS];
	COUNT	joystick_port;
    } device;
    PROC_PARAMETER(
    INPUT_STATE (*input_func), (InputRef, InputState),
	ARG	(INPUT_REF	InputRef)
	ARG_END	(INPUT_STATE	InputState)
    );
} INPUT_DESC;
typedef INPUT_DESC		near *NPINPUT_DESC;
typedef INPUT_DESC		far *LPINPUT_DESC;
typedef INPUT_DESC		*PINPUT_DESC;

#define INPUT_DEVICE_PRIORITY	DEFAULT_MEM_PRIORITY

typedef LPINPUT_DESC		INPUT_DESCPTR;
typedef LPBYTE			BYTEPTR;

#define AllocInputDevice()		\
	(INPUT_DEVICE)mem_allocate ((MEM_SIZE)sizeof (INPUT_DESC), \
	MEM_ZEROINIT, INPUT_DEVICE_PRIORITY, MEM_SIMPLE)
#define LockInputDevice(InputDevice)		(INPUT_DESCPTR)mem_lock (InputDevice)
#define UnlockInputDevice(InputDevice)		mem_unlock (InputDevice)
#define FreeInputDevice(InputDevice)		mem_release (InputDevice)

#define SetInputDeviceHandle(i,h)		\
	((INPUT_DESCPTR)(i))->ThisDevice = (h)
#define SetInputDeviceKeyEquivalents(i,l,r,t,b,b1,b2)	\
	((INPUT_DESCPTR)(i))->device.key_equivalent[0] = ASCIIToScan (l), \
	((INPUT_DESCPTR)(i))->device.key_equivalent[1] = ASCIIToScan (r), \
	((INPUT_DESCPTR)(i))->device.key_equivalent[2] = ASCIIToScan (t), \
	((INPUT_DESCPTR)(i))->device.key_equivalent[3] = ASCIIToScan (b), \
	((INPUT_DESCPTR)(i))->device.key_equivalent[4] = ASCIIToScan (b1), \
	((INPUT_DESCPTR)(i))->device.key_equivalent[5] = ASCIIToScan (b2)
#define SetInputDeviceJoystickPort(i,p)	\
	((INPUT_DESCPTR)(i))->device.joystick_port = (p)
#define SetInputDeviceInputFunc(i,f)	\
	((INPUT_DESCPTR)(i))->input_func = (f)

#define GetInputDeviceHandle(i)		\
	((INPUT_DESCPTR)(i))->ThisDevice
#define GetInputDeviceKeyEquivalentPtr(i)	\
	((INPUT_DESCPTR)(i))->device.key_equivalent
#define GetInputDeviceJoystickPort(i)	\
	((INPUT_DESCPTR)(i))->device.joystick_port
#define GetInputDeviceInputFunc(i)	\
	((INPUT_DESCPTR)(i))->input_func

PROC_GLOBAL(
INPUT_STATE _get_serial_keyboard_state, (InputRef, InputState),
    ARG		(INPUT_REF	InputRef)
    ARG_END	(INPUT_STATE	InputState)
);
PROC_GLOBAL(
INPUT_STATE _get_joystick_keyboard_state, (InputRef, InputState),
    ARG		(INPUT_REF	InputRef)
    ARG_END	(INPUT_STATE	InputState)
);
PROC_GLOBAL(
INPUT_STATE _get_joystick_state, (InputRef, InputState),
    ARG		(INPUT_REF	InputRef)
    ARG_END	(INPUT_STATE	InputState)
);
PROC_GLOBAL(
BOOLEAN _joystick_port_active, (port),
    ARG_END	(COUNT		port)
);
PROC_GLOBAL(
INPUT_STATE _get_pause_exit_state, (),
    ARG_VOID
);

#endif /* _INPINTRN_H */

