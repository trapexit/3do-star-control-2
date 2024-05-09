#ifndef _INTEL_H
#define _INTEL_H

typedef enum
{
    PURSUE = 0,
    AVOID,
    ENTICE,
    NO_MOVEMENT
} MOVEMENT_STATE;

typedef struct
{
    LPELEMENT		ObjectPtr;
    COUNT		facing, which_turn;
    MOVEMENT_STATE	MoveState;
} EVALUATE_DESC;
typedef EVALUATE_DESC	*PEVALUATE_DESC;
typedef EVALUATE_DESC	near *NPEVALUATE_DESC;
typedef EVALUATE_DESC	far *LPEVALUATE_DESC;

#define EVALUATE_DESCPTR	PEVALUATE_DESC

#define MANEUVERABILITY(pi)	((pi)->ManeuverabilityIndex)
#define WEAPON_RANGE(pi)	((pi)->WeaponRange)

#define WORLD_TO_TURN(d)	((d)>>6)

#define CLOSE_RANGE_WEAPON	DISPLAY_TO_WORLD (50)
#define LONG_RANGE_WEAPON	DISPLAY_TO_WORLD (1000)
#define FAST_SHIP		150
#define MEDIUM_SHIP		45
#define SLOW_SHIP		25

enum
{
    ENEMY_SHIP_INDEX = 0,
    CREW_OBJECT_INDEX,
    ENEMY_WEAPON_INDEX,
    GRAVITY_MASS_INDEX,
    FIRST_EMPTY_INDEX
};

extern STARSHIPPTR		CyborgDescPtr;

PROC_GLOBAL(
INPUT_STATE computer_intelligence, (InputRef, InputState),
    ARG		(INPUT_REF	InputRef)
    ARG_END	(INPUT_STATE	InputState)
);
PROC_GLOBAL(
INPUT_STATE tactical_intelligence, (),
    ARG_VOID
);
PROC_GLOBAL(
void ship_intelligence, (ShipPtr, ObjectsOfConcern, ConcernCounter),
    ARG		(ELEMENTPTR		ShipPtr)
    ARG		(EVALUATE_DESCPTR	ObjectsOfConcern)
    ARG_END	(COUNT			ConcernCounter)
);
PROC_GLOBAL(
BOOLEAN	ship_weapons, (ShipPtr, OtherPtr, margin_of_error),
    ARG		(ELEMENTPTR	ShipPtr)
    ARG		(ELEMENTPTR	OtherPtr)
    ARG_END	(COUNT		margin_of_error)
);

PROC_GLOBAL(
void Pursue, (ShipPtr, EvalDescPtr),
    ARG		(ELEMENTPTR		ShipPtr)
    ARG_END	(EVALUATE_DESCPTR	EvalDescPtr)
);
PROC_GLOBAL(
void Entice, (ShipPtr, EvalDescPtr),
    ARG		(ELEMENTPTR		ShipPtr)
    ARG_END	(EVALUATE_DESCPTR	EvalDescPtr)
);
PROC_GLOBAL(
void Avoid, (ShipPtr, EvalDescPtr),
    ARG		(ELEMENTPTR		ShipPtr)
    ARG_END	(EVALUATE_DESCPTR	EvalDescPtr)
);
PROC_GLOBAL(
BOOLEAN	TurnShip, (ShipPtr, angle),
    ARG		(ELEMENTPTR	ShipPtr)
    ARG_END	(COUNT		angle)
);
PROC_GLOBAL(
BOOLEAN	ThrustShip, (ShipPtr, angle),
    ARG		(ELEMENTPTR	ShipPtr)
    ARG_END	(COUNT		angle)
);

#endif /* _INTEL_H */

