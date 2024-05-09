#include <ctype.h>
#include "starcon.h"

STATIC BYTE	cur_char = ' ';

PROC(
void SetJoystickChar, (which_char),
    ARG_END	(BYTE	which_char)
)
{
    if (!isalnum (cur_char = which_char))
	cur_char = ' ';
}

PROC(
BYTE GetJoystickChar, (InputState),
    ARG_END	(INPUT_STATE	InputState)
)
{
    int		dy;
    BYTE	old_char;

    if (InputState & DEVICE_BUTTON1)
	return ('\n');
    else if (InputState & DEVICE_BUTTON2)
	return (0x1B);
    else if (InputState & DEVICE_BUTTON3)
	return (0x7F);
	
    old_char = cur_char;
    if (dy = GetInputYComponent (InputState))
    {
	if (cur_char == ' ')
	{
	    if (dy > 0)
		cur_char = 'a';
	    else /* if (dy < 0) */
		cur_char = '9';
	}
	else if (isdigit (cur_char))
	{
	    cur_char += dy;
	    if (cur_char < '0')
		cur_char = 'z';
	    else if (cur_char > '9')
		cur_char = ' ';
	}
	else if (!isalpha (cur_char += dy))
	{
	    cur_char -= dy;
	    if (cur_char == 'a' || cur_char == 'A')
		cur_char = ' ';
	    else
		cur_char = '0';
	}
    }
    
    if ((InputState & (DEVICE_LEFTSHIFT | DEVICE_RIGHTSHIFT)) && isalpha (cur_char))
    {
	if (islower (cur_char))
	    cur_char = toupper (cur_char);
	else
	    cur_char = tolower (cur_char);
    }
    
    return (cur_char == old_char ? 0 : cur_char);
}
