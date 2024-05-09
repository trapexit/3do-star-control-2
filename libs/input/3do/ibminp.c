#include "inpintrn.h"
#include "timlib.h"
#include "types.h"
#include "item.h"
#include "msgport.h"
#include "kernel.h"
#include "kernelnodes.h"
#include "event.h"
#include "task/3do/task3do.h"

//#define DEBUG

#define MAX_NUM_JOYSTICKS	2
#define JOYSTICKS_AVAIL		(JoystickFlags >> MAX_NUM_JOYSTICKS)
#define SET_JOYSTICKS_AVAIL(a)	(JoystickFlags |= ((a) << MAX_NUM_JOYSTICKS))
#define JOYSTICK_ACTIVE(j)	(JoystickFlags & (1 << (j)))
#define SET_JOYSTICK_ACTIVE(j)	(JoystickFlags |= (1 << (j)))

static BYTE	JoystickFlags;

static BYTE		PauseKey;
static BYTE		ExitKey;
static BYTE		OtherKey;

PROC_PARAMETER(STATIC
BOOLEAN (* far PauseFunc), (),
    ARG_VOID
);

#ifdef OLD
#define messageSize 2048

// Check how many control pads are attached to the system.
// This code was extracted from "cpdump.c" -- 3do example source.
static int
check_active_control_pads (void)
{
  Item msgPortItem;
  Item brokerPortItem;
  Item msgItem, eventItem;
  int32 sent;
  Message *event;
  MsgPort *msgPort;
  PodDescriptionList *pdl;
  EventBrokerHeader queryHeader, *msgHeader;
  TagArg msgTags[3];

  // Get handle to Event Broker.
  brokerPortItem = FindNamedItem(MKNODEID(KERNELNODE,MSGPORTNODE),
				 EventPortName);

  if (brokerPortItem < 0) 
  {
#ifdef DEBUG
    printf("Can't find Event Broker port ---");
    PrintfSysErr(brokerPortItem);
#endif /* DEBUG */
    return (0);
  }
  
  // Create a message port to receive event broker's reply.
  msgPortItem = CreateMsgPort("pad_test", 0, 0);
  msgPort = (MsgPort *) LookupItem(msgPortItem);
 
  // Have the event broker tell us about the input devices on the system.
  queryHeader.ebh_Flavor = EB_DescribePods;

  // Prepare the message to be sent.
  msgTags[0].ta_Tag = CREATEMSG_TAG_REPLYPORT;
  msgTags[0].ta_Arg = (void *) msgPortItem;
  msgTags[1].ta_Tag = CREATEMSG_TAG_DATA_SIZE;
  msgTags[1].ta_Arg = (void *) messageSize;
  msgTags[2].ta_Tag = TAG_END;
  msgItem = CreateItem(MKNODEID(KERNELNODE,MESSAGENODE), msgTags);

  // Send the message to the event broker.
  sent = SendMsg(brokerPortItem, msgItem, &queryHeader, sizeof queryHeader);
  
  // Wait for a response.
  eventItem = WaitPort(msgPortItem, msgItem);
  
  // Extract the data from the reply.
  event = (Message *) LookupItem(eventItem);
  msgHeader = (EventBrokerHeader *) event->msg_DataPtr;
  pdl = (PodDescriptionList *) msgHeader;
  
  DeleteMsg (msgItem);
  DeleteMsgPort (msgPortItem);

#ifdef DEBUG
printf("Detected %d control pad(s) on the system.\n\n", pdl->pdl_PodCount);
#endif /* DEBUG */

  // is pdl still pointing to valid data after DeleteMsg... calls above???
  return (pdl->pdl_PodCount);
}
#endif

PROC(
BOOLEAN InitInput, (Pause, Exit, PFunc),
    ARG		(BYTE	Pause)
    ARG		(BYTE	Exit)
    ARG_END
    (
	PROC_PARAMETER(
	BOOLEAN (*PFunc), (),
	    ARG_VOID
	)
    )
)
{
    PauseKey = Pause;
    ExitKey = Exit;
	
    // Find a unique byte ID not equal to PauseKey, ExitKey, or 0.
    OtherKey = 1;
    while (OtherKey == PauseKey || OtherKey == ExitKey)
	OtherKey++;
		
    PauseFunc = PFunc;
	
    // Initialize both joysticks for monitoring and see what's out there.
    InitEventUtility (2L, 0L, 0L);

#ifdef OLD	
    SET_JOYSTICKS_AVAIL (check_active_control_pads ());
#endif
    SET_JOYSTICKS_AVAIL (2);

    return (FALSE);
}

#ifdef DEBUG
static int	ReuseCt;
#endif /* DEBUG */

PROC(
BOOLEAN UninitInput, (),
    ARG_VOID
)
{
    KillEventUtility ();
    JoystickFlags = 0;
    PauseKey = ExitKey = OtherKey = 0;

#ifdef DEBUG
printf ("Reused %d inputs\n", ReuseCt);
#endif /* DEBUG */
    return (TRUE);
}

static DWORD LastInput[MAX_NUM_JOYSTICKS];
#define read_joystick(j)	LastInput[j]

void
_ThreeDO_get_input (void)
{
    int		ThisTime;
    static int	LastTime;
    
    if (KernelBase->kb_CurrentTask->t_ThreadTask)
    {
    	_threedo_add_task (TASK_GET_INPUT);
	Yield ();
	return;
    }

    ThisTime = GetAudioTime ();
    if (ThisTime > LastTime)
    {
	int	joy_id;

	LastTime = ThisTime + 1;    
	for (joy_id = 0; joy_id < MAX_NUM_JOYSTICKS; ++joy_id)
	{
	    if (!JOYSTICK_ACTIVE (joy_id))
		LastInput[joy_id] = 0;
	    else
	    {
		ControlPadEventData	cp;
	    
		GetControlPad (joy_id + 1, 0, &cp);

		LastInput[joy_id] = cp.cped_ButtonBits;
	    }
	}
    }
#ifdef DEBUG
else ++ReuseCt;
#endif /* DEBUG */
}

PROC(
INPUT_STATE _get_pause_exit_state, (),
    ARG_VOID
)
{
    int		i;
    INPUT_STATE	InputState;

    _ThreeDO_get_input ();

    InputState = 0;
    for (i = 0; i < JOYSTICKS_AVAIL; i++)
    {
	DWORD	joy;
	
	joy = read_joystick (i);
	if (joy & ControlStart)		// pause
	{
	    if (PauseFunc && (*PauseFunc) ())
//		while (KeyDown (PauseKey))
		    ;
	}
	else if (joy & ControlX)	// exit
	{
//	    while (KeyDown (ExitKey))
//		TaskSwitch ();

	    InputState = DEVICE_EXIT;
	}
    }

    return (InputState);
}

PROC(
INPUT_STATE _get_serial_keyboard_state, (InputRef, InputState),
    ARG		(INPUT_REF	InputRef)
    ARG_END	(INPUT_STATE	InputState)
)
{
    SetInputASCII (&InputState, 0 /* GetASCIIKey () */);
    SetInputDevType (&InputState, KEYBOARD_DEVICE);

    return (InputState);
}

PROC(
INPUT_STATE _get_joystick_keyboard_state, (InputRef, InputState),
    ARG		(INPUT_REF	InputRef)
    ARG_END	(INPUT_STATE	InputState)
)
{
    return (InputState);
}

PROC(
INPUT_STATE _get_joystick_state, (InputRef, InputState),
    ARG		(INPUT_REF	InputRef)
    ARG_END	(INPUT_STATE	InputState)
)
{
	COUNT	port;
	DWORD	retval;

	port = GetInputDeviceJoystickPort (InputRef);
		if (retval = read_joystick (port))
		{
			if (retval & ControlLeft)
				SetInputXComponent (&InputState, -1);
			else if (retval & ControlRight)
				SetInputXComponent (&InputState, 1);
			
			if (retval & ControlUp)
				SetInputYComponent (&InputState, -1);
			else if (retval & ControlDown)
				SetInputYComponent (&InputState, 1);
			
			if (retval & ControlA)
				InputState |= DEVICE_BUTTON1;
			if (retval & ControlB)
				InputState |= DEVICE_BUTTON2;
#ifdef DEBUG
			if (retval & ControlC)
				InputState |= DEVICE_EXIT;
			if (retval & ControlLeftShift)
			{
				ReportMemoryUsage (); printf ("\n");
			}
#endif
			if (retval & ControlStart)
				InputState |= DEVICE_PAUSE;
			if (retval & ControlC)
				InputState |= DEVICE_BUTTON3;
			if (retval & ControlX)
				InputState |= DEVICE_EXIT;
			if (retval & ControlLeftShift)
				InputState |= DEVICE_LEFTSHIFT;
			if (retval & ControlRightShift)
				InputState |= DEVICE_RIGHTSHIFT;
		}
	
    return (InputState);
}

PROC(
BOOLEAN _joystick_port_active, (port),
    ARG_END	(COUNT		port)
)
{
    if (port < JOYSTICKS_AVAIL)
	SET_JOYSTICK_ACTIVE (port);
    return (JOYSTICK_ACTIVE (port)); 
}

PROC(
INPUT_STATE AnyButtonPress, (DetectSpecial),
    ARG_END	(BOOLEAN	DetectSpecial)
)
{
    int	i;

    if (DetectSpecial)
    {
	if (KeyDown (PauseKey))
	{
	    if (PauseFunc && (*PauseFunc) ())
//		while (KeyDown (PauseKey))
		    ;
	}
    }

    _ThreeDO_get_input ();
    for (i = 0; i < JOYSTICKS_AVAIL; i++)
	if (read_joystick (i))
	    return (DEVICE_BUTTON1);
	
    return (0);
}

PROC(
void FlushInput, (),
    ARG_VOID
)
{
    int	i;
    
    for (i = 0; i < MAX_NUM_JOYSTICKS; i++)
	LastInput[i] = 0;
}

BYTE
ASCIIToScan (BYTE a)
{
    return (a);
}

//  Return true if Pause is hit on either joypad.
BYTE
KeyDown (BYTE k)
{
    int	i;
	
    _ThreeDO_get_input ();
    for (i = 0; i < JOYSTICKS_AVAIL; i++)
    {
	DWORD	joy;
	
	joy = read_joystick (i);
	switch (k)
	{
	    case SK_F1:
		if (joy & ControlStart)	// pause
		    return (1);
		break;
	    case 0x1B:
	    case SK_F10:
		if (joy & ControlX)	// exit
		    return (1);
		break;
	}
    }
    
    return (0);
}

// Return Pause, Exit, or Other key scan byte if hit on either joypad.
BYTE
KeyHit (void)
{
    int		i;
    
    _ThreeDO_get_input ();
    for (i = 0; i < JOYSTICKS_AVAIL; i++)
    {
	DWORD	retval;
	
	if ((retval = read_joystick (i)) & ControlStart)
	    return (PauseKey);
	else if (retval & ControlX)
	    return (ExitKey);
	else if (retval) // & (ControlA | ControlB | ControlC))
	    return (OtherKey);
    }
	
    return (0);
}

