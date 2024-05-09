#include "starcon.h"
#include "melee.h"

QUEUE	master_q;

PROC(
void LoadMasterShipList, (),
    ARG_VOID
)
{
    COUNT		num_entries;
    RES_TYPE		rt;
    RES_INSTANCE	ri;
    RES_PACKAGE		rp;

if (master_q.head != 0)
    return;
    
    rt = GET_TYPE (ARILOU_SHIP_INDEX);
    ri = GET_INSTANCE (ARILOU_SHIP_INDEX);
    rp = GET_PACKAGE (ARILOU_SHIP_INDEX);
    InitQueue (&master_q, num_entries = NUM_MELEE_SHIPS, sizeof (SHIP_FRAGMENT));
    while (num_entries--)
    {
	HSTARSHIP	hBuiltShip;

	if (hBuiltShip = Build (&master_q, MAKE_RESOURCE (rp++, rt, ri++), 0, 0))
	{
	    char		built_buf[30];
	    HSTARSHIP		hStarShip, hNextShip;
	    STARSHIPPTR		BuiltShipPtr;
	    SHIP_INFOPTR	ShipInfoPtr;

	    TaskSwitch ();

	    BuiltShipPtr = LockStarShip (&master_q, hBuiltShip);
	    load_ship (BuiltShipPtr, FALSE);
	    ShipInfoPtr = &((SHIP_FRAGMENTPTR)BuiltShipPtr)->ShipInfo;
	    *ShipInfoPtr = BuiltShipPtr->RaceDescPtr->ship_info;
	    BuiltShipPtr->RaceDescPtr->ship_info.melee_icon = 0;
	    BuiltShipPtr->RaceDescPtr->ship_info.icons = 0;
	    BuiltShipPtr->RaceDescPtr->ship_info.race_strings = 0;
	    free_ship (BuiltShipPtr, FALSE);
	    BuiltShipPtr->RaceDescPtr = (RACE_DESCPTR)ShipInfoPtr;

	    GetStringContents (SetAbsStringTableIndex (
		    BuiltShipPtr->RaceDescPtr->ship_info.race_strings, 2
		    ), (STRINGPTR)built_buf, FALSE);
	    UnlockStarShip (&master_q, hBuiltShip);

	    RemoveQueue (&master_q, hBuiltShip);
	    for (hStarShip = GetHeadLink (&master_q);
		    hStarShip; hStarShip = hNextShip)
	    {
		char		ship_buf[30];
		STARSHIPPTR	StarShipPtr;

		StarShipPtr = LockStarShip (&master_q, hStarShip);
		hNextShip = _GetSuccLink (StarShipPtr);
		GetStringContents (SetAbsStringTableIndex (
			StarShipPtr->RaceDescPtr->ship_info.race_strings, 2
			), (STRINGPTR)ship_buf, FALSE);
		UnlockStarShip (&master_q, hStarShip);

		if (strcmp (built_buf, ship_buf) < 0)
		    break;
	    }
	    InsertQueue (&master_q, hBuiltShip, hStarShip);
	}
    }

    TaskSwitch ();
}

PROC(
void FreeMasterShipList, (),
    ARG_VOID
)
{
    HSTARSHIP	hStarShip, hNextShip;

    for (hStarShip = GetHeadLink (&master_q);
	    hStarShip != 0; hStarShip = hNextShip)
    {
	STARSHIPPTR	StarShipPtr;

	StarShipPtr = LockStarShip (&master_q, hStarShip);
	hNextShip = _GetSuccLink (StarShipPtr);

	DestroyDrawable (ReleaseDrawable (
		StarShipPtr->RaceDescPtr->ship_info.melee_icon));
	DestroyDrawable (ReleaseDrawable (
		StarShipPtr->RaceDescPtr->ship_info.icons));
	DestroyStringTable (ReleaseStringTable (
		StarShipPtr->RaceDescPtr->ship_info.race_strings));

	UnlockStarShip (&master_q, hStarShip);
    }

    UninitQueue (&master_q);
}

PROC(
HSTARSHIP FindMasterShip, (ship_ref),
    ARG_END	(DWORD	ship_ref)
)
{
    HSTARSHIP	hStarShip;
    
    if (hStarShip = GetHeadLink (&master_q))
    {
	do
	{
	    DWORD	ref;
	    HSTARSHIP	hNextShip;
	    STARSHIPPTR	StarShipPtr;

	    StarShipPtr = LockStarShip (&master_q, hStarShip);
	    hNextShip = _GetSuccLink (StarShipPtr);
	    ref = StarShipPtr->RaceResIndex;
	    UnlockStarShip (&master_q, hStarShip);

	    if (ref == ship_ref)
		break;

	    hStarShip = hNextShip;
	} while (hStarShip);
    }

    return (hStarShip);
}
