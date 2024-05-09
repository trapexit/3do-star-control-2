#include <stdio.h>
#include "/src/libs/input/ibm/kbd.h"

void cdecl
main (void)
{
    int	i;

    InstallKey ();
    while (!KeyDown (1))
    {
	for (i = 0; i < 128; ++i)
	{
	    if (KeyDown ((BYTE)i))
	    {
		BYTE	ascii;
		PROC_GLOBAL(
		BYTE GetKeyState, (key),
		    ARG_END	(BYTE	key)
		);

		ascii = ScanToASCII ((BYTE)i);
		printf ("%d('%c':%d) -- 0x%02x\n", i, ascii, ascii, GetKeyState (ascii));
	    }
	}
    }
    RestoreKey ();
}

