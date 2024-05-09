#include "Utils3do.h"
#include

unsigned int
CNBReadJoystick(void)
{
    ControlPadEventData cp;

    GetControlPad(1,0,&cp);

    return (cp.cped_ButtonBits);
}

