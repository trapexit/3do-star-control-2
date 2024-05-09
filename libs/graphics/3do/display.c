#include "gfxintrn.h"

PDISPLAY_INTERFACE	_pCurDisplay = (PDISPLAY_INTERFACE)NULL_PTR;


PROC(
BOOLEAN InitGraphics, (argc, argv, KbytesRequired),
    ARG		(int	argc)
    ARG		(char	*argv[])
    ARG_END	(COUNT	KbytesRequired)
)
{
    PROC_GLOBAL(
	void Load3DO, (ppDisplay),
	ARG_END	(PDISPLAY_INTERFACE	*ppDisplay)
    );

    InitTimer ();
    
    if (Init3DO ())	
	Load3DO (&_pCurDisplay);
	
    if (_pCurDisplay != (PDISPLAY_INTERFACE)NULL_PTR)
    {
	MEM_SIZE	BytesAvailable;
	
#ifdef NOTYET
	KbytesRequired = 0;
	if (BytesAvailable = ((MEM_SIZE)KbytesRequired << 10))
	{
	    BytesAvailable -= ((MEM_SIZE)mem_prog_size () << 10)
		    - _memavl ();
	}
#else /* !NOTYET */
	BytesAvailable = 0;
#endif /* NOTYET */

	if (mem_init ((MEM_SIZE)0, &BytesAvailable, NULL_PTR))
	{
	    ActivateDisplay ();

	    return (TRUE);
	}
	else
	{
	    _pCurDisplay = NULL_PTR;
	}
    }

    return (FALSE);
}

PROC(
void UninitGraphics, (),
    ARG_VOID
)
{
    mem_uninit ();
    Uninit3DO ();
    UninitTimer ();
}
