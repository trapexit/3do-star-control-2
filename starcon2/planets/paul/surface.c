#include "starcon.h"

CONST BYTE		far *Elements;
CONST PlanetFrame	far *PlanData; 

PROC(STATIC
COUNT near CalcMineralDeposits, (SysInfoPtr, which_deposit),
    ARG		(SYSTEM_INFOPTR	SysInfoPtr)
    ARG_END	(COUNT		which_deposit)
)
{
    BYTE		j;
    COUNT		num_deposits;
    ELEMENT_ENTRYPTR	eptr;

    eptr = &SysInfoPtr->PlanetInfo.PlanDataPtr->UsefulElements[0];
    num_deposits = 0;
    j = NUM_USEFUL_ELEMENTS;
    do
    {
	BYTE	num_possible;

	num_possible = (BYTE)((BYTE)random ()
		% (DEPOSIT_QUANTITY (eptr->Density) + 1));
	while (num_possible--)
	{
#define MEDIUM_DEPOSIT_THRESHOLD	150
#define LARGE_DEPOSIT_THRESHOLD		225
	    DWORD	rand_val;
	    UWORD	loword, hiword;
	    COUNT	deposit_quality_fine,
			deposit_quality_gross;

	    deposit_quality_fine = ((COUNT)random () % 100)
		    + (
		    DEPOSIT_QUALITY (eptr->Density)
		    + SysInfoPtr->StarSize
		    ) * 50;
	    if (deposit_quality_fine < MEDIUM_DEPOSIT_THRESHOLD)
		deposit_quality_gross = 0;
	    else if (deposit_quality_fine < LARGE_DEPOSIT_THRESHOLD)
		deposit_quality_gross = 1;
	    else
		deposit_quality_gross = 2;

	    rand_val = random ();
	    loword = LOWORD (rand_val);
	    hiword = HIWORD (rand_val);
	    SysInfoPtr->PlanetInfo.CurPt.x =
		    (LOBYTE (loword) % (MAP_WIDTH - (8 << 1))) + 8;
	    SysInfoPtr->PlanetInfo.CurPt.y =
		    (HIBYTE (loword) % (MAP_HEIGHT - (8 << 1))) + 8;

	    SysInfoPtr->PlanetInfo.CurDensity =
		    MAKE_WORD (
		    deposit_quality_gross, deposit_quality_fine / 10 + 1
		    );
	    SysInfoPtr->PlanetInfo.CurType = eptr->ElementType;
#ifdef DEBUG
	    printf ("\t\t%d units of %Fs\n",
		    SysInfoPtr->PlanetInfo.CurDensity,
		    Elements[eptr->ElementType].name);
#endif /* DEBUG */
	    if ((num_deposits >= which_deposit
		    && !(SysInfoPtr->PlanetInfo.ScanRetrieveMask[MINERAL_SCAN]
		    & (1L << num_deposits)))
		    || ++num_deposits == sizeof (DWORD) * 8)
		goto ExitCalcMinerals;
	}
	++eptr;
    } while (--j);

ExitCalcMinerals:
    return (num_deposits);
}

PROC(
DWORD GenerateMineralDeposits, (SysInfoPtr, pwhich_deposit),
    ARG		(SYSTEM_INFOPTR	SysInfoPtr)
    ARG_END	(PCOUNT		pwhich_deposit)
)
{
    DWORD	old_rand;

    old_rand = seed_random (SysInfoPtr->PlanetInfo.ScanSeed[MINERAL_SCAN]);
    *pwhich_deposit = CalcMineralDeposits (SysInfoPtr, *pwhich_deposit);
    return (seed_random (old_rand));
}

PROC(STATIC
COUNT near CalcLifeForms, (SysInfoPtr, which_life),
    ARG		(SYSTEM_INFOPTR	SysInfoPtr)
    ARG_END	(COUNT		which_life)
)
{
    COUNT	num_life_forms;

    num_life_forms = 0;
    if (PLANSIZE (SysInfoPtr->PlanetInfo.PlanDataPtr->Type) == GAS_GIANT)
	SysInfoPtr->PlanetInfo.LifeChance = -1;
    else
    {
#define MIN_LIFE_CHANCE	10
	SIZE	life_var;

	life_var = 0;

	if (SysInfoPtr->PlanetInfo.SurfaceTemperature < -151)
	    life_var -= 300;
	else if (SysInfoPtr->PlanetInfo.SurfaceTemperature < -51)
	    life_var -= 100;
	else if (SysInfoPtr->PlanetInfo.SurfaceTemperature < 0)
	    life_var += 100;
	else if (SysInfoPtr->PlanetInfo.SurfaceTemperature < 50)
	    life_var += 300;
	else if (SysInfoPtr->PlanetInfo.SurfaceTemperature < 150)
	    life_var += 50;
	else if (SysInfoPtr->PlanetInfo.SurfaceTemperature < 250)
	    life_var -= 100;
	else if (SysInfoPtr->PlanetInfo.SurfaceTemperature < 500)
	    life_var -= 400;
	else
	    life_var -= 800;

	if (SysInfoPtr->PlanetInfo.AtmoDensity == 0)
	    life_var -= 1000;
	else if (SysInfoPtr->PlanetInfo.AtmoDensity < 15)
	    life_var += 100;
	else if (SysInfoPtr->PlanetInfo.AtmoDensity < 30)
	    life_var += 200;
	else if (SysInfoPtr->PlanetInfo.AtmoDensity < 100)
	    life_var += 300;
	else if (SysInfoPtr->PlanetInfo.AtmoDensity < 1000)
	    life_var += 150;
	else if (SysInfoPtr->PlanetInfo.AtmoDensity < 2500)
	    ;
	else
	    life_var -= 100;

#ifndef NOTYET
	life_var += 200 + 80 + 80;
#else /* NOTYET */
	if (SysInfoPtr->PlanetInfo.SurfaceGravity < 10)
	    ;
	else if (SysInfoPtr->PlanetInfo.SurfaceGravity < 35)
	    life_var += 50;
	else if (SysInfoPtr->PlanetInfo.SurfaceGravity < 75)
	    life_var += 100;
	else if (SysInfoPtr->PlanetInfo.SurfaceGravity < 150)
	    life_var += 200;
	else if (SysInfoPtr->PlanetInfo.SurfaceGravity < 400)
	    life_var += 50;
	else if (SysInfoPtr->PlanetInfo.SurfaceGravity < 800)
	    ;
	else
	    life_var -= 100;

	if (SysInfoPtr->PlanetInfo.Tectonics < 1)
	    life_var += 80;
	else if (SysInfoPtr->PlanetInfo.Tectonics < 2)
	    life_var += 70;
	else if (SysInfoPtr->PlanetInfo.Tectonics < 3)
	    life_var += 60;
	else if (SysInfoPtr->PlanetInfo.Tectonics < 4)
	    life_var += 50;
	else if (SysInfoPtr->PlanetInfo.Tectonics < 5)
	    life_var += 25;
	else if (SysInfoPtr->PlanetInfo.Tectonics < 6)
	    ;
	else
	    life_var -= 100;

	if (SysInfoPtr->PlanetInfo.Weather < 1)
	    life_var += 80;
	else if (SysInfoPtr->PlanetInfo.Weather < 2)
	    life_var += 70;
	else if (SysInfoPtr->PlanetInfo.Weather < 3)
	    life_var += 60;
	else if (SysInfoPtr->PlanetInfo.Weather < 4)
	    life_var += 50;
	else if (SysInfoPtr->PlanetInfo.Weather < 5)
	    life_var += 25;
	else if (SysInfoPtr->PlanetInfo.Weather < 6)
	    ;
	else
	    life_var -= 100;
#endif /* NOTYET */

	SysInfoPtr->PlanetInfo.LifeChance = life_var;

	life_var = (COUNT)random () & 1023;
	if (life_var < SysInfoPtr->PlanetInfo.LifeChance
		|| (SysInfoPtr->PlanetInfo.LifeChance < MIN_LIFE_CHANCE
		&& life_var < MIN_LIFE_CHANCE))
	{
#include "::lifeform.h"
	    BYTE	num_types;

	    num_types = (BYTE)(((BYTE)random () % MAX_LIFE_VARIATION) + 1);
	    do
	    {
		BYTE	index, num_creatures;
		UWORD	rand_val;

		rand_val = (UWORD)random ();
		index = LOBYTE (rand_val) % NUM_CREATURE_TYPES;
		num_creatures = (BYTE)((HIBYTE (rand_val) % 10) + 1);
		do
		{
		    rand_val = (UWORD)random ();
		    SysInfoPtr->PlanetInfo.CurPt.x =
			    (LOBYTE (rand_val) % (MAP_WIDTH - (8 << 1))) + 8;
		    SysInfoPtr->PlanetInfo.CurPt.y =
			    (HIBYTE (rand_val) % (MAP_HEIGHT - (8 << 1))) + 8;
		    SysInfoPtr->PlanetInfo.CurType = index;

		    if ((num_life_forms >= which_life
			    && !(SysInfoPtr->PlanetInfo.ScanRetrieveMask[BIOLOGICAL_SCAN]
			    & (1L << num_life_forms)))
			    || ++num_life_forms == sizeof (DWORD) * 8)
		    {
			num_types = 1;
			break;
		    }
		} while (--num_creatures);
	    } while (--num_types);
	}
#ifdef DEBUG
else
    printf ("It's dead, Jim! (%d >= %d)\n", life_var, SysInfoPtr->PlanetInfo.LifeChance);
#endif /* DEBUG */
    }

    return (num_life_forms);
}

PROC(
DWORD GenerateLifeForms, (SysInfoPtr, pwhich_life),
    ARG		(SYSTEM_INFOPTR	SysInfoPtr)
    ARG_END	(PCOUNT		pwhich_life)
)
{
    DWORD	old_rand;

    old_rand = seed_random (SysInfoPtr->PlanetInfo.ScanSeed[BIOLOGICAL_SCAN]);
    *pwhich_life = CalcLifeForms (SysInfoPtr, *pwhich_life);
    return (seed_random (old_rand));
}



