#define MELEE
#include "starcon.h"
#include "coderes.h"

GLOBDATA		GlobData;
CONST BYTE		far *Elements;
CONST PlanetFrame	far *PlanData; 

void
DoNPCPhrase (lpStr, AlienSpeechRef)
LPBYTE	lpStr;
MUSIC_REF AlienSpeechRef;
{
}

void
DoResponsePhrase (lpStr, response_func)
LPBYTE		lpStr;
RESPONSE_FUNC	response_func;
{
}

BOOLEAN
LoadHyperspace ()
{
}

BOOLEAN
FreeHyperspace ()
{
}

void	
MoveSIS (pdx, pdy)
PSIZE	pdx, pdy;
{
}

PROC(
BOOLEAN hyper_transition, (ElementPtr),
    ARG_END	(LPELEMENT	ElementPtr)
)
{
}

void
SeedUniverse ()
{
}

void
SetGameClockRate (r)
COUNT	r;
{
}

void
SuspendGameClock ()
{
}

void
ResumeGameClock ()
{
}

BOOLEAN
GameClockRunning ()
{
    return (FALSE);
}

PROC(
void DeltaSISGauges, (crew_delta, fuel_delta, resunit_delta),
    ARG		(SIZE	crew_delta)
    ARG		(SIZE	fuel_delta)
    ARG_END	(SIZE	resunit_delta)
)
{
}

PROC(
void DoMenuOptions, (),
    ARG_VOID
)
{
}

PROC(
void flee_preprocess, (ElementPtr),
    ARG_END	(LPELEMENT	ElementPtr)
)
{
}

PROC(
void DrawLanders, (),
    ARG_VOID
)
{
}

PROC(
void DrawStorageBays, (Refresh),
    ARG_END	(BOOLEAN	Refresh)
)
{
}

PROC(
HEVENT AddEvent, (type, month_index, day_index, year_index, func_index),
    ARG		(EVENT_TYPE	type)
    ARG		(COUNT		month_index)
    ARG		(COUNT		day_index)
    ARG		(COUNT		year_index)
    ARG_END	(BYTE		func_index)
)
{
    return (0);
}

PROC(
HSTARSHIP GetEncounterStarShip, (LastStarShipPtr, which_player),
    ARG		(STARSHIPPTR	LastStarShipPtr)
    ARG_END	(COUNT		which_player)
)
{
    PROC_GLOBAL(
    HSTARSHIP GetMeleeStarShip, (LastStarShipPtr, which_player),
	ARG	(STARSHIPPTR	LastStarShipPtr)
	ARG_END	(COUNT		which_player)
    );
	
    return (GetMeleeStarShip (LastStarShipPtr, which_player));
}

PROC(
void CDECL main, (argc, argv),
    ARG		(int	argc)
    ARG_END	(char	*argv[])
)
{
    if (LoadKernel (argc, argv) && CopyProtection ())
    {
	SetColorMap (GetColorMapAddress (ColorMapTab));

	while (--argc > 0)
	{
	    ++argv;
	    if ((*argv)[0] == '/'
		    && strcmp ((const char *)strupr (&(*argv)[1]), "FRENZY") == 0)
	    {
		extern UWORD	nth_frame;

		nth_frame = MAKE_WORD (1, 1);
		break;
	    }
	}

	OpenJournal ();
	Melee ();
	CloseJournal ();
    }

    FreeKernel ();
}
