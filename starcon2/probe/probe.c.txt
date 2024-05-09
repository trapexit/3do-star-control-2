#include "reslib.h"
#include "resinst.h"
#include "starcon.h"

#define MAX_CREW		1
#define MAX_ENERGY		1
#define ENERGY_REGENERATION	0
#define WEAPON_ENERGY_COST	0
#define SPECIAL_ENERGY_COST	0
#define ENERGY_WAIT		0
#define MAX_THRUST		0
#define THRUST_INCREMENT	0
#define TURN_WAIT		0
#define THRUST_WAIT		0
#define WEAPON_WAIT		0
#define SPECIAL_WAIT		0

#define SHIP_MASS		0

static RACE_DESC	probe_desc =
{
    {
	0,
	0,					/* Super Melee cost */
	0,					/* Initial sphere of influence radius */
	MAX_CREW, MAX_CREW,
	MAX_ENERGY, MAX_ENERGY,
	{
	    0, 0,
	},
	0,
	0,
	(FRAME)PROBE_MICON_MASK_PMAP_ANIM,
    },
    {
	MAX_THRUST,
	THRUST_INCREMENT,
	ENERGY_REGENERATION,
	WEAPON_ENERGY_COST,
	SPECIAL_ENERGY_COST,
	ENERGY_WAIT,
	TURN_WAIT,
	THRUST_WAIT,
	WEAPON_WAIT,
	SPECIAL_WAIT,
	SHIP_MASS,
    },
    {
	{
	    (FRAME)0,
	    (FRAME)0,
	    (FRAME)0,
	},
	{
	    (FRAME)0,
	    (FRAME)0,
	    (FRAME)0,
	},
	{
	    (FRAME)0,
	    (FRAME)0,
	    (FRAME)0,
	},
	{
	    (FRAME)0,
	    (FRAME)0,
	    (FRAME)0,
	    (FRAME)0,
	    (FRAME)0,
	},
	(SOUND)0,
	(SOUND)0,
    },
    {
	0,
	0,
	NULL_PTR,
    },
    NULL_PTR,
    NULL_PTR,
    NULL_PTR,
    0,
};

RACE_DESCPTR	far
init_probe ()
{
    RACE_DESCPTR	RaceDescPtr;

    PUSH_CONTEXT
    {
	RaceDescPtr = &probe_desc;
    }
    POP_CONTEXT

    return (RaceDescPtr);
}

