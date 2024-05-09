#include "starcon.h"
#include "declib.h"

#if DEMO_MODE || CREATE_JOURNAL
static DECODE_REF	journal_fh;
static char		far journal_buf[1024
#if CREATE_JOURNAL
				* 8
#else /* DEMO_MODE */
				* 2
#endif
				];
INPUT_REF		DemoInput;
#endif

#if DEMO_MODE

static INPUT_REF	OldArrowInput;

PROC(
INPUT_STATE demo_input, (InputRef, InputState),
    ARG		(INPUT_REF	InputRef)
    ARG_END	(INPUT_STATE	InputState)
)
{
    if (InputState || AnyButtonPress () || cread (
	    (LPBYTE)&InputState, sizeof (InputState), 1, journal_fh
	    ) == 0)
    {
	cclose (journal_fh);
	journal_fh = 0;

	StopMusic ();
	StopSound ();

	FreeKernel ();
	exit (1);
    }

    return (InputState);
}

#endif /* DEMO_MODE */

#if CREATE_JOURNAL

PROC(
void JournalInput, (InputState),
    ARG_END	(INPUT_STATE	InputState)
)
{
    if (ArrowInput != DemoInput && journal_fh)
	cwrite ((LPBYTE)&InputState, sizeof (InputState), 1, journal_fh);
}

#endif /* CREATE_JOURNAL */

#if DEMO_MODE || CREATE_JOURNAL

PROC(
void OpenJournal, (),
    ARG_VOID
)
{
    DWORD	start_seed;

#if CREATE_JOURNAL
    if (battle_counter)
    {
	if (journal_fh = copen (journal_buf, MEMORY_STREAM, STREAM_WRITE))
	{
	    start_seed = SeedRandomNumbers ();
	    cwrite ((LPBYTE)&start_seed, sizeof (start_seed), 1, journal_fh);
	}
    }
    else
#endif /* CREATE_JOURNAL */
    {
	FILE	*fp;

	if (fp = OpenResFile ("starcon.jnl", "rb", NULL_PTR))
	{
	    ReadResFile (journal_buf, 1, sizeof (journal_buf), fp);
	    CloseResFile (fp);

	    if (journal_fh = copen (journal_buf, MEMORY_STREAM, STREAM_READ))
	    {
		OldArrowInput = ArrowInput;
		ArrowInput = DemoInput;
		PlayerInput[0] = PlayerInput[1] = DemoInput;

		FlushInput ();

		cread ((LPBYTE)&start_seed, sizeof (start_seed), 1, journal_fh);
		seed_random (start_seed);
	    }
	}
    }
}

PROC(
BOOLEAN CloseJournal, (),
    ARG_VOID
)
{
    if (journal_fh)
    {
	FILE	*fp;

	cclose (journal_fh);
	journal_fh = 0;

	if (ArrowInput == DemoInput)
	{
	    ArrowInput = OldArrowInput;
	    return (FALSE);
	}
#if CREATE_JOURNAL
	else if (fp = OpenResFile ("starcon.jnl", "wb", NULL_PTR))
	{
	    WriteResFile (journal_buf, 1, sizeof (journal_buf), fp);
	    CloseResFile (fp);
	}
#endif /* CREATE_JOURNAL */
    }

    return (TRUE);
}

#endif

