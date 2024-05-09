#include "starcon.h"
#include "declib.h"

ACTIVITY	NextActivity;

PROC(STATIC
void near LoadShipQueue, (fh, pQueue, MakeQ),
    ARG		(DECODE_REF	fh)
    ARG		(PQUEUE		pQueue)
    ARG_END	(BOOLEAN	MakeQ)
)
{
    COUNT	num_links;

    cread ((LPBYTE)&num_links, sizeof (num_links), 1, fh);
    if (num_links)
    {
	if (MakeQ)
	    InitQueue (pQueue, num_links, sizeof (SHIP_FRAGMENT));

	do
	{
	    HSTARSHIP		hStarShip;
	    SHIP_FRAGMENTPTR	FragPtr;
	    COUNT		Index, Offset;
	    PBYTE		Ptr;

	    cread ((LPBYTE)&Index, sizeof (Index), 1, fh);

	    if (pQueue == &GLOBAL (avail_race_q))
	    {
		hStarShip = GetStarShipFromIndex (pQueue, Index);
		Offset = 0;
	    }
	    else
	    {
		hStarShip = CloneShipFragment (Index, pQueue, 0);
		Offset = (PBYTE)&FragPtr->ShipInfo
			- (PBYTE)&FragPtr->RaceDescPtr;
	    }

	    FragPtr = (SHIP_FRAGMENTPTR)LockStarShip (pQueue, hStarShip);
	    Ptr = ((PBYTE)&FragPtr->ShipInfo) - Offset;
	    cread ((LPBYTE)Ptr,
		    ((PBYTE)&FragPtr->ShipInfo.race_strings) - Ptr,
		    1, fh);
	    if (Offset == 0)
	    {
		EXTENDED_SHIP_FRAGMENTPTR	ExtFragPtr;

		ExtFragPtr = (EXTENDED_SHIP_FRAGMENTPTR)FragPtr;
		Ptr = (PBYTE)&ExtFragPtr->ShipInfo.actual_strength;
		cread ((LPBYTE)Ptr, ((PBYTE)&ExtFragPtr[1]) - Ptr, 1, fh);
	    }
	    UnlockStarShip (pQueue, hStarShip);
	} while (--num_links);
    }
}

PROC(
BOOLEAN LoadSummary, (in_fp),
    ARG_END	(FILE	*in_fp)
)
{
    SUMMARY_DESC	S;

    ReadResFile (&S, sizeof (S), 1, in_fp);
}

PROC(
BOOLEAN LoadGame, (which_game, summary_desc),
    ARG		(COUNT		which_game)
    ARG_END	(SUMMARY_DESC	*summary_desc)
)
{
    FILE	*in_fp;
    char	buf[256];
    char	file_buf[4096];

    sprintf (buf, "starcon2.%02u", which_game);
    if (in_fp = OpenResFile (buf, "rb", file_buf, sizeof (file_buf), NULL_PTR))
    {
	FILE		*fp;
	DECODE_REF	fh;
	COUNT		num_links;
	TASK		clock_task;
	QUEUE		event_q, encounter_q, avail_q, npc_q, player_q;
	STAR_DESC	SD;
	ACTIVITY	Activity;

	ReadResFile (buf, sizeof (*summary_desc), 1, in_fp);

	if (summary_desc == 0)
	    summary_desc = (SUMMARY_DESC *)buf;
	else
	{
	    MEMCPY (summary_desc, buf, sizeof (*summary_desc));
	    CloseResFile (in_fp);
	    return (TRUE);
	}

	GlobData.SIS_state = summary_desc->SS;

	if ((fh = copen (in_fp, FILE_STREAM, STREAM_READ)) == 0)
	{
	    CloseResFile (in_fp);
	    return (FALSE);
	}

	ReinitQueue (&GLOBAL (GameClock.event_q));
	ReinitQueue (&GLOBAL (encounter_q));
	ReinitQueue (&GLOBAL (npc_built_ship_q));
	ReinitQueue (&GLOBAL (built_ship_q));

	clock_task = GLOBAL (GameClock.clock_task);
	event_q = GLOBAL (GameClock.event_q);
	encounter_q = GLOBAL (encounter_q);
	avail_q = GLOBAL (avail_race_q);
	npc_q = GLOBAL (npc_built_ship_q);
	player_q = GLOBAL (built_ship_q);

	MEMSET ((PBYTE)&GLOBAL (GameState[0]),
		0, sizeof (GLOBAL (GameState)));
	Activity = GLOBAL (CurrentActivity);
	cread ((LPBYTE)&GlobData.Game_state, sizeof (GlobData.Game_state), 1, fh);
	NextActivity = GLOBAL (CurrentActivity);
	GLOBAL (CurrentActivity) = Activity;

	GLOBAL (GameClock.clock_task) = clock_task;
	GLOBAL (GameClock.event_q) = event_q;
	GLOBAL (encounter_q) = encounter_q;
	GLOBAL (avail_race_q) = avail_q;
	GLOBAL (npc_built_ship_q) = npc_q;
	GLOBAL (built_ship_q) = player_q;

	LoadShipQueue (fh, &GLOBAL (avail_race_q), FALSE);
	if (!(NextActivity & START_INTERPLANETARY))
	    LoadShipQueue (fh, &GLOBAL (npc_built_ship_q), FALSE);
	LoadShipQueue (fh, &GLOBAL (built_ship_q), FALSE);

	cread ((LPBYTE)&num_links, sizeof (num_links), 1, fh);
	{
#ifdef DEBUG
printf ("EVENTS:\n");
#endif /* DEBUG */
	    while (num_links--)
	    {
		HEVENT		hEvent;
		EVENTPTR	EventPtr;

		hEvent = AllocEvent ();
		LockEvent (hEvent, &EventPtr);

		cread ((LPBYTE)EventPtr, sizeof (*EventPtr), 1, fh);

#ifdef DEBUG
printf ("\t%u/%u/%u -- %u\n",
EventPtr->month_index,
EventPtr->day_index,
EventPtr->year_index,
EventPtr->func_index);
#endif /* DEBUG */
		UnlockEvent (hEvent);
		PutEvent (hEvent);
	    }
	}

	cread ((LPBYTE)&num_links, sizeof (num_links), 1, fh);
	{
	    while (num_links--)
	    {
		BYTE		i, NumShips;
		HENCOUNTER	hEncounter;
		ENCOUNTERPTR	EncounterPtr;

		hEncounter = AllocEncounter ();
		LockEncounter (hEncounter, &EncounterPtr);

		cread ((LPBYTE)EncounterPtr, sizeof (*EncounterPtr), 1, fh);
		EncounterPtr->hElement = 0;
		NumShips = LONIBBLE (EncounterPtr->SD.Index);
		for (i = 0; i < NumShips; ++i)
		{
		    HSTARSHIP		hStarShip;
		    SHIP_FRAGMENTPTR	TemplatePtr;

		    hStarShip = GetStarShipFromIndex (
			    &GLOBAL (avail_race_q),
			    EncounterPtr->SD.ShipList[i].var1
			    );
		    TemplatePtr = (SHIP_FRAGMENTPTR)LockStarShip (
			    &GLOBAL (avail_race_q), hStarShip
			    );
		    EncounterPtr->SD.ShipList[i].race_strings =
			    TemplatePtr->ShipInfo.race_strings;
		    EncounterPtr->SD.ShipList[i].icons =
			    TemplatePtr->ShipInfo.icons;
		    EncounterPtr->SD.ShipList[i].melee_icon =
			    TemplatePtr->ShipInfo.melee_icon;
		    UnlockStarShip (
			    &GLOBAL (avail_race_q), hStarShip
			    );
		}

		UnlockEncounter (hEncounter);
		PutEncounter (hEncounter);
	    }
	}

	DeleteRamFile (STARINFO_FILE);
	if (fp = OpenRamFile (STARINFO_FILE, STAR_BUFSIZE))
	{
	    DWORD	flen;

	    cread ((LPBYTE)&flen, sizeof (flen), 1, fh);
	    while (flen)
	    {
		COUNT	num_bytes;

		num_bytes = flen >= sizeof (buf) ? sizeof (buf) : (COUNT)flen;
		cread ((LPBYTE)buf, num_bytes, 1, fh);
		WriteRamFile (buf, num_bytes, 1, fp);

		flen -= num_bytes;
	    }
	    CloseRamFile (fp);
	}

	DeleteRamFile (DEFGRPINFO_FILE);
	if (fp = OpenRamFile (DEFGRPINFO_FILE, DEF_BUFSIZE))
	{
	    DWORD	flen;

	    cread ((LPBYTE)&flen, sizeof (flen), 1, fh);
	    while (flen)
	    {
		COUNT	num_bytes;

		num_bytes = flen >= sizeof (buf) ? sizeof (buf) : (COUNT)flen;
		cread ((LPBYTE)buf, num_bytes, 1, fh);
		WriteRamFile (buf, num_bytes, 1, fp);

		flen -= num_bytes;
	    }
	    CloseRamFile (fp);
	}

	DeleteRamFile (RANDGRPINFO_FILE);
	if (fp = OpenRamFile (RANDGRPINFO_FILE, RAND_BUFSIZE))
	{
	    DWORD	flen;

	    cread ((LPBYTE)&flen, sizeof (flen), 1, fh);
	    while (flen)
	    {
		COUNT	num_bytes;

		num_bytes = flen >= sizeof (buf) ? sizeof (buf) : (COUNT)flen;
		cread ((LPBYTE)buf, num_bytes, 1, fh);
		WriteRamFile (buf, num_bytes, 1, fp);

		flen -= num_bytes;
	    }
	    CloseRamFile (fp);
	}

	cread ((LPBYTE)&SD, sizeof (SD), 1, fh);

	cclose (fh);
	CloseResFile (in_fp);

	battle_counter = 0;
	ReinitQueue (&race_q[0]);
	ReinitQueue (&race_q[1]);
	CurStarDescPtr = FindStar (NULL_PTR, &SD.star_pt, 0, 0);
	if (!(NextActivity & START_ENCOUNTER)
		&& LOBYTE (NextActivity) == IN_INTERPLANETARY)
	    NextActivity |= START_INTERPLANETARY;

	GLOBAL (DisplayArray) = DisplayArray;

	return (TRUE);
    }

    return (FALSE);
}


