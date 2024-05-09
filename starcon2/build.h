#ifndef _BUILD_H
#define _BUILD_H

#define CREW_XOFFS		4
#define ENERGY_XOFFS		52
#define GAUGE_YOFFS		(SHIP_INFO_HEIGHT - 10)
#define UNIT_WIDTH		2
#define UNIT_HEIGHT		1
#define STAT_WIDTH		(1 + UNIT_WIDTH + 1 + UNIT_WIDTH + 1)

#define NAME_OFFSET		5
#define NUM_CAPTAINS_NAMES	16

#define OwnStarShip(S,p,n)	\
	(S)->RaceDescPtr = (RACE_DESCPTR)MAKE_DWORD(p, n)
#define StarShipCaptain(S)	\
	(BYTE)HIWORD ((S)->RaceDescPtr)
#define StarShipPlayer(S)	\
	(COUNT)LOWORD ((S)->RaceDescPtr)
#define PickCaptainName()	(((COUNT)random () \
				& (NUM_CAPTAINS_NAMES - 1)) \
				+ NAME_OFFSET)

PROC_GLOBAL(
HSTARSHIP Build, (pQueue, RaceResIndex, which_player, captains_name_index),
    ARG		(PQUEUE	pQueue)
    ARG		(DWORD	RaceResIndex)
    ARG		(COUNT	which_player)
    ARG_END	(BYTE	captains_name_index)
);
PROC_GLOBAL(
HSTARSHIP CloneShipFragment, (index, pDstQueue, crew_level),
    ARG		(COUNT	index)
    ARG		(PQUEUE	pDstQueue)
    ARG_END	(BYTE	crew_level)
);
PROC_GLOBAL(
HSTARSHIP GetStarShipFromIndex, (pShipQ, Index),
    ARG		(PQUEUE	pShipQ)
    ARG_END	(COUNT	Index)
);
PROC_GLOBAL(
BYTE NameCaptain, (pQueue, StarShipPtr),
    ARG		(PQUEUE		pQueue)
    ARG_END	(STARSHIPPTR	StarShipPtr)
);

#define CHECK_ALLIANCE		-2
#define ESCORT_WORTH		9995
#define SPHERE_KNOWN		9996
#define SPHERE_TRACKING		9997
#define ESCORTING_FLAGSHIP	9998
#define FEASIBILITY_STUDY	9999

PROC_GLOBAL(
COUNT ActivateStarShip, (which_ship, state),
    ARG		(COUNT	which_ship)
    ARG_END	(SIZE	state)
);
PROC_GLOBAL(
COUNT GetIndexFromStarShip, (pShipQ, hStarShip),
    ARG		(PQUEUE		pShipQ)
    ARG_END	(HSTARSHIP	hStarShip)
);

PROC_GLOBAL(
MEM_HANDLE load_ship, (StarShipPtr, LoadBattleData),
    ARG		(STARSHIPPTR	StarShipPtr)
    ARG_END	(BOOLEAN	LoadBattleData)
);
PROC_GLOBAL(
void free_ship, (StarShipPtr, FreeBattleData),
    ARG		(STARSHIPPTR	StarShipPtr)
    ARG_END	(BOOLEAN	FreeBattleData)
);

PROC_GLOBAL(
void DrawCrewFuelString, (y, state),
    ARG		(COORD	y)
    ARG_END	(SIZE	state)
);
PROC_GLOBAL(
void ClearShipStatus, (y),
    ARG_END	(COORD	y)
);
PROC_GLOBAL(
void OutlineShipStatus, (y),
    ARG_END	(COORD	y)
);
PROC_GLOBAL(
void InitShipStatus, (StarShipPtr, pClipRect),
    ARG		(STARSHIPPTR	StarShipPtr)
    ARG_END	(PRECT		pClipRect)
);
PROC_GLOBAL(
void DeltaStatistics, (StarShipPtr, crew_delta, energy_delta),
    ARG		(STARSHIPPTR	StarShipPtr)
    ARG		(SIZE		crew_delta)
    ARG_END	(SIZE		energy_delta)
);
PROC_GLOBAL(
void DrawBattleCrewAmount, (CountPlayer),
    ARG_END	(BOOLEAN	CountPlayer)
);

#endif /* _BUILD_H */

