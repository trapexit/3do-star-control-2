#include "starcon.h"

/* #define DEBUG */

PROC(
void collide, (ElementPtr0, ElementPtr1),
    ARG		(ELEMENTPTR	ElementPtr0)
    ARG_END	(ELEMENTPTR	ElementPtr1)
)
{
    SIZE	speed;
    SIZE	dx0, dy0, dx1, dy1, dx_rel, dy_rel;
    SIZE	TravelAngle0, TravelAngle1, ImpactAngle0, ImpactAngle1;
    SIZE	RelTravelAngle, Directness;

    dx_rel = ElementPtr0->next.location.x
	    - ElementPtr1->next.location.x;
    dy_rel = ElementPtr0->next.location.y
	    - ElementPtr1->next.location.y;
    ImpactAngle0 = ARCTAN (dx_rel, dy_rel);
    ImpactAngle1 = NORMALIZE_ANGLE (ImpactAngle0 + HALF_CIRCLE);

    GetCurrentVelocityComponents (&ElementPtr0->velocity, &dx0, &dy0);
    TravelAngle0 = GetVelocityTravelAngle (&ElementPtr0->velocity);
    GetCurrentVelocityComponents (&ElementPtr1->velocity, &dx1, &dy1);
    TravelAngle1 = GetVelocityTravelAngle (&ElementPtr1->velocity);
    dx_rel = dx0 - dx1;
    dy_rel = dy0 - dy1;
    RelTravelAngle = ARCTAN (dx_rel, dy_rel);
    speed = square_root ((long)dx_rel * dx_rel + (long)dy_rel * dy_rel);

    Directness = NORMALIZE_ANGLE (RelTravelAngle - ImpactAngle0);
    if (Directness <= QUADRANT || Directness >= HALF_CIRCLE + QUADRANT)
	    /* shapes just scraped each other but still collided,
	     * they will collide again unless we fudge it.
	     */
    {
	Directness = HALF_CIRCLE;
	ImpactAngle0 = TravelAngle0 + HALF_CIRCLE;
	ImpactAngle1 = TravelAngle1 + HALF_CIRCLE;
    }

#ifdef DEBUG
printf ("Centers: <%d, %d> <%d, %d>\n",
ElementPtr0->next.location.x, ElementPtr0->next.location.y,
ElementPtr1->next.location.x, ElementPtr1->next.location.y);
printf ("RelTravelAngle : %d, ImpactAngles <%d, %d>\n",
RelTravelAngle, ImpactAngle0, ImpactAngle1);
#endif /* DEBUG */

    if (ElementPtr0->next.location.x == ElementPtr0->current.location.x
	    && ElementPtr0->next.location.y == ElementPtr0->current.location.y
	    && ElementPtr1->next.location.x == ElementPtr1->current.location.x
	    && ElementPtr1->next.location.y == ElementPtr1->current.location.y)
    {
	if (ElementPtr0->state_flags & ElementPtr1->state_flags & DEFY_PHYSICS)
	{
	    ImpactAngle0 = TravelAngle0 + (HALF_CIRCLE - OCTANT);
	    ImpactAngle1 = TravelAngle1 + (HALF_CIRCLE - OCTANT);
	    ZeroVelocityComponents (&ElementPtr0->velocity);
	    ZeroVelocityComponents (&ElementPtr1->velocity);
	}
	ElementPtr0->state_flags |= (DEFY_PHYSICS | COLLISION);
	ElementPtr1->state_flags |= (DEFY_PHYSICS | COLLISION);
#ifdef DEBUG
printf ("No movement before collision -- <(%d, %d) = %d, (%d, %d) = %d>\n",
dx0, dy0, ImpactAngle0 - OCTANT, dx1, dy1, ImpactAngle1 - OCTANT);
#endif /* DEBUG */
    }

    {
	SIZE	mass0, mass1;
	long	scalar;

	mass0 = ElementPtr0->mass_points /* << 2 */;
	mass1 = ElementPtr1->mass_points /* << 2 */;
	scalar = (long)SINE (Directness, speed << 1) * (mass0 * mass1);

	if (!GRAVITY_MASS (ElementPtr0->mass_points + 1))
	{
	    if (ElementPtr0->state_flags & PLAYER_SHIP)
	    {
		STARSHIPPTR	StarShipPtr;

		GetElementStarShip (ElementPtr0, &StarShipPtr);
		StarShipPtr->cur_status_flags &=
			~(SHIP_AT_MAX_SPEED | SHIP_BEYOND_MAX_SPEED);
		if (!(ElementPtr0->state_flags & DEFY_PHYSICS))
		{
		    if (ElementPtr0->turn_wait < COLLISION_TURN_WAIT)
			ElementPtr0->turn_wait += COLLISION_TURN_WAIT;
		    if (ElementPtr0->thrust_wait < COLLISION_THRUST_WAIT)
			ElementPtr0->thrust_wait += COLLISION_THRUST_WAIT;
		}
	    }

	    speed = (SIZE)(scalar / ((long)mass0 * (mass0 + mass1)));
	    DeltaVelocityComponents (&ElementPtr0->velocity,
		    COSINE (ImpactAngle0, speed),
		    SINE (ImpactAngle0, speed));

	    GetCurrentVelocityComponents (&ElementPtr0->velocity, &dx0, &dy0);
	    if (dx0 < 0)
		dx0 = -dx0;
	    if (dy0 < 0)
		dy0 = -dy0;

	    if (VELOCITY_TO_WORLD (dx0 + dy0) < SCALED_ONE)
		SetVelocityComponents (&ElementPtr0->velocity,
			COSINE (ImpactAngle0,
			WORLD_TO_VELOCITY (SCALED_ONE) - 1),
			SINE (ImpactAngle0,
			WORLD_TO_VELOCITY (SCALED_ONE) - 1));
	}

	if (!GRAVITY_MASS (ElementPtr1->mass_points + 1))
	{
	    if (ElementPtr1->state_flags & PLAYER_SHIP)
	    {
		STARSHIPPTR	StarShipPtr;

		GetElementStarShip (ElementPtr1, &StarShipPtr);
		StarShipPtr->cur_status_flags &=
			~(SHIP_AT_MAX_SPEED | SHIP_BEYOND_MAX_SPEED);
		if (!(ElementPtr1->state_flags & DEFY_PHYSICS))
		{
		    if (ElementPtr1->turn_wait < COLLISION_TURN_WAIT)
			ElementPtr1->turn_wait += COLLISION_TURN_WAIT;
		    if (ElementPtr1->thrust_wait < COLLISION_THRUST_WAIT)
			ElementPtr1->thrust_wait += COLLISION_THRUST_WAIT;
		}
	    }

	    speed = (SIZE)(scalar / ((long)mass1 * (mass0 + mass1)));
	    DeltaVelocityComponents (&ElementPtr1->velocity,
		    COSINE (ImpactAngle1, speed),
		    SINE (ImpactAngle1, speed));

	    GetCurrentVelocityComponents (&ElementPtr1->velocity, &dx1, &dy1);
	    if (dx1 < 0)
		dx1 = -dx1;
	    if (dy1 < 0)
		dy1 = -dy1;

	    if (VELOCITY_TO_WORLD (dx1 + dy1) < SCALED_ONE)
		SetVelocityComponents (&ElementPtr1->velocity,
			COSINE (ImpactAngle1,
			WORLD_TO_VELOCITY (SCALED_ONE) - 1),
			SINE (ImpactAngle1,
			WORLD_TO_VELOCITY (SCALED_ONE) - 1));
	}
#ifdef DEBUG
GetCurrentVelocityComponents (&ElementPtr0->velocity, &dx0, &dy0);
GetCurrentVelocityComponents (&ElementPtr1->velocity, &dx1, &dy1);
printf ("After: <%d, %d> <%d, %d>\n\n",
dx0, dy0, dx1, dy1);
#endif /* DEBUG */
    }
}

