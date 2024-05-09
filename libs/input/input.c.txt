#include "inpintrn.h"

PROC(
INPUT_DEVICE CreateSerialKeyboardDevice, (),
    ARG_VOID
)
{
    INPUT_DEVICE	InputDevice;

    if ((InputDevice = AllocInputDevice ()) != (INPUT_DEVICE)NULL_PTR)
    {
	INPUT_DESCPTR	InputPtr;

	if ((InputPtr =
		LockInputDevice (InputDevice)) == (INPUT_DESCPTR)NULL_PTR)
	{
	    FreeInputDevice (InputDevice);
	    InputDevice = (INPUT_DEVICE)NULL_PTR;
	}
	else
	{
	    SetInputDeviceHandle (InputPtr, InputDevice);
	    SetInputDeviceInputFunc (InputPtr, _get_serial_keyboard_state);

	    UnlockInputDevice (InputDevice);
	}
    }

    return (InputDevice);
}

PROC(
INPUT_DEVICE CreateJoystickKeyboardDevice,
	(lfkey, rtkey, topkey, botkey, but1key, but2key),
    ARG		(BYTE	lfkey)
    ARG		(BYTE	rtkey)
    ARG		(BYTE	topkey)
    ARG		(BYTE	botkey)
    ARG		(BYTE	but1key)
    ARG_END	(BYTE	but2key)
)
{
    INPUT_DEVICE	InputDevice;

    if ((InputDevice = AllocInputDevice ()) != (INPUT_DEVICE)NULL_PTR)
    {
	INPUT_DESCPTR	InputPtr;

	if ((InputPtr =
		LockInputDevice (InputDevice)) == (INPUT_DESCPTR)NULL_PTR)
	{
	    FreeInputDevice (InputDevice);
	    InputDevice = (INPUT_DEVICE)NULL_PTR;
	}
	else
	{
	    SetInputDeviceHandle (InputPtr, InputDevice);
	    SetInputDeviceInputFunc (InputPtr, _get_joystick_keyboard_state);
	    SetInputDeviceKeyEquivalents (InputPtr,
		    lfkey, rtkey, topkey, botkey, but1key, but2key);

	    UnlockInputDevice (InputDevice);
	}
    }

    return (InputDevice);
}

PROC(
INPUT_DEVICE CreateJoystickDevice, (port),
    ARG_END	(COUNT		port)
)
{
    INPUT_DEVICE	InputDevice;

    if (!_joystick_port_active (port))
	InputDevice = (INPUT_DEVICE)NULL_PTR;
    else if ((InputDevice = AllocInputDevice ()) != (INPUT_DEVICE)NULL_PTR)
    {
	INPUT_DESCPTR	InputPtr;

	if ((InputPtr =
		LockInputDevice (InputDevice)) == (INPUT_DESCPTR)NULL_PTR)
	{
	    FreeInputDevice (InputDevice);
	    InputDevice = (INPUT_DEVICE)NULL_PTR;
	}
	else
	{
	    SetInputDeviceHandle (InputPtr, InputDevice);
	    SetInputDeviceInputFunc (InputPtr, _get_joystick_state);
	    SetInputDeviceJoystickPort (InputPtr, port);

	    UnlockInputDevice (InputDevice);
	}
    }

    return (InputDevice);
}

PROC(
INPUT_DEVICE CreateInternalDevice, (input_func),
    ARG_END
    (
	PROC_PARAMETER(
	INPUT_STATE (*input_func), (InputRef, InputState),
	    ARG		(INPUT_REF	InputRef)
	    ARG_END	(INPUT_STATE	InputState)
	)
    )
)
{
    INPUT_DEVICE	InputDevice;

    if (input_func == NULL_PTR)
	InputDevice = (INPUT_DEVICE)NULL_PTR;
    else if ((InputDevice = AllocInputDevice ()) != (INPUT_DEVICE)NULL_PTR)
    {
	INPUT_DESCPTR	InputPtr;

	if ((InputPtr =
		LockInputDevice (InputDevice)) == (INPUT_DESCPTR)NULL_PTR)
	{
	    FreeInputDevice (InputDevice);
	    InputDevice = (INPUT_DEVICE)NULL_PTR;
	}
	else
	{
	    SetInputDeviceHandle (InputPtr, InputDevice);
	    SetInputDeviceInputFunc (InputPtr, input_func);

	    UnlockInputDevice (InputDevice);
	}
    }

    return (InputDevice);
}

PROC(
INPUT_REF CaptureInputDevice, (InputDevice),
    ARG_END	(INPUT_DEVICE	InputDevice)
)
{
    if (InputDevice != (INPUT_DEVICE)NULL_PTR)
	return ((INPUT_REF)LockInputDevice (InputDevice));

    return ((INPUT_REF)NULL_PTR);
}

PROC(
INPUT_DEVICE ReleaseInputDevice, (InputRef),
    ARG_END	(INPUT_REF	InputRef)
)
{
    INPUT_DEVICE	InputDevice;

    if (InputRef == (INPUT_REF)NULL_PTR)
	InputDevice = (INPUT_DEVICE)NULL_PTR;
    else
    {
	InputDevice = GetInputDeviceHandle (InputRef);
	UnlockInputDevice (InputDevice);
    }

    return (InputDevice);
}

PROC(
BOOLEAN DestroyInputDevice, (InputDevice),
    ARG_END	(INPUT_DEVICE	InputDevice)
)
{
    if (InputDevice != (INPUT_DEVICE)NULL_PTR)
	return (FreeInputDevice (InputDevice));

    return (FALSE);
}

PROC(
INPUT_STATE GetInputState, (InputRef),
    ARG_END	(INPUT_REF	InputRef)
)
{
    if (InputRef == 0)
	return ((INPUT_STATE)0);
    else
	return ((*GetInputDeviceInputFunc (InputRef))
		(InputRef, _get_pause_exit_state ()));
}

