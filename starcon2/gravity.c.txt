#include "starcon.h"

PROC(
BOOLEAN CalculateGravity, (ElementPtr),
    ARG_END	(LPELEMENT	ElementPtr)
)
{
    BOOLEAN	retval, HasGravity;
    HELEMENT	hTestElement, hSuccElement;

    retval = FALSE;
    HasGravity = (BOOLEAN)(CollidingElement (ElementPtr)
	    && GRAVITY_MASS (ElementPtr->mass_points + 1));
    for (hTestElement = GetHeadElement ();
	    hTestElement != 0; hTestElement = hSuccElement)
    {
	BOOLEAN		TestHasGravity;
	ELEMENTPTR	TestElementPtr;

	LockElement (hTestElement, &TestElementPtr);
	if (TestElementPtr != ElementPtr
		&& CollidingElement (TestElementPtr)
		&& (TestHasGravity =
		GRAVITY_MASS (TestElementPtr->mass_points + 1)) != HasGravity)
	{
	    COUNT	abs_dx, abs_dy;
	    SIZE	dx, dy;

	    if (!(ElementPtr->state_flags & PRE_PROCESS))
	    {
		dx = ElementPtr->current.location.x
			- TestElementPtr->current.location.x;
		dy = ElementPtr->current.location.y
			- TestElementPtr->current.location.y;
	    }
	    else
	    {
		dx = ElementPtr->next.location.x
			- TestElementPtr->next.location.x;
		dy = ElementPtr->next.location.y
			- TestElementPtr->next.location.y;
	    }
#ifdef DEBUG
if (TestElementPtr->state_flags & PLAYER_SHIP)
{
printf ("CalculateGravity:\n");
printf ("\tdx = %d, dy = %d\n", dx, dy);
}
#endif /* DEBUG */
	    dx = WRAP_DELTA_X (dx);
	    dy = WRAP_DELTA_Y (dy);
#ifdef DEBUG
if (TestElementPtr->state_flags & PLAYER_SHIP)
printf ("\twrap_dx = %d, wrap_dy = %d\n", dx, dy);
#endif /* DEBUG */
	    abs_dx = dx >= 0 ? dx : -dx;
	    abs_dy = dy >= 0 ? dy : -dy;
	    abs_dx = WORLD_TO_DISPLAY (abs_dx);
	    abs_dy = WORLD_TO_DISPLAY (abs_dy);
#ifdef DEBUG
if (TestElementPtr->state_flags & PLAYER_SHIP)
printf ("\tdisplay_dx = %d, display_dy = %d\n", abs_dx, abs_dy);
#endif /* DEBUG */
	    if (abs_dx <= GRAVITY_THRESHOLD
		    && abs_dy <= GRAVITY_THRESHOLD)
	    {
		DWORD	dist_squared;

		dist_squared = (DWORD)(abs_dx * abs_dx)
			+ (DWORD)(abs_dy * abs_dy);
		if (dist_squared <= (DWORD)(GRAVITY_THRESHOLD
			* GRAVITY_THRESHOLD))
		{
#ifdef NEVER
		    COUNT	magnitude;

#define DIFUSE_GRAVITY		175
		    dist_squared += (DWORD)abs_dx * (DIFUSE_GRAVITY << 1)
			    + (DWORD)abs_dy * (DIFUSE_GRAVITY << 1)
			    + ((DWORD)(DIFUSE_GRAVITY * DIFUSE_GRAVITY) << 1);
		    if ((magnitude = (COUNT)((DWORD)(GRAVITY_THRESHOLD
			    * GRAVITY_THRESHOLD) / dist_squared)) == 0)
			magnitude = 1;

#define MAX_MAGNITUDE		6
		    else if (magnitude > MAX_MAGNITUDE)
			magnitude = MAX_MAGNITUDE;
printf ("magnitude = %u ", magnitude);
#endif /* NEVER */

#ifdef DEBUG
if (TestElementPtr->state_flags & PLAYER_SHIP)
printf ("dist_squared = %lu\n", dist_squared);
#endif /* DEBUG */
		    if (TestHasGravity)
		    {
			retval = TRUE;
			UnlockElement (hTestElement);
			break;
		    }
		    else
		    {
			COUNT	angle;

			angle = ARCTAN (dx, dy);
			DeltaVelocityComponents (&TestElementPtr->velocity,
				COSINE (angle, WORLD_TO_VELOCITY (1)),
				SINE (angle, WORLD_TO_VELOCITY (1)));
			if (TestElementPtr->state_flags & PLAYER_SHIP)
			{
			    STARSHIPPTR	StarShipPtr;

			    GetElementStarShip (TestElementPtr, &StarShipPtr);
			    StarShipPtr->cur_status_flags &= ~SHIP_AT_MAX_SPEED;
			    StarShipPtr->cur_status_flags |= SHIP_IN_GRAVITY_WELL;
			}
		    }
		}
	    }
	}

	hSuccElement = GetSuccElement (TestElementPtr);
	UnlockElement (hTestElement);
    }

    return (retval);
}

PROC(
BOOLEAN TimeSpaceMatterConflict, (ElementPtr),
    ARG_END	(ELEMENTPTR	ElementPtr)
)
{
    HELEMENT		hTestElement, hSuccElement;
    INTERSECT_CONTROL	ElementControl;

    ElementControl.IntersectStamp.origin.x =
	    WORLD_TO_DISPLAY (ElementPtr->current.location.x);
    ElementControl.IntersectStamp.origin.y =
	    WORLD_TO_DISPLAY (ElementPtr->current.location.y);
    ElementControl.IntersectStamp.frame =
	     SetEquFrameIndex (ElementPtr->current.image.farray[0],
	     ElementPtr->current.image.frame);
    ElementControl.EndPoint = ElementControl.IntersectStamp.origin;
    for (hTestElement = GetHeadElement ();
	    hTestElement != 0; hTestElement = hSuccElement)
    {
	ELEMENTPTR	TestElementPtr;

	LockElement (hTestElement, &TestElementPtr);
	hSuccElement = GetSuccElement (TestElementPtr);
	if (TestElementPtr != ElementPtr
		&& (CollidingElement (TestElementPtr)
			/* ship in transition */
		|| (TestElementPtr->state_flags & PLAYER_SHIP)))
	{
	    INTERSECT_CONTROL	TestElementControl;

	    TestElementControl.IntersectStamp.origin.x =
		    WORLD_TO_DISPLAY (TestElementPtr->current.location.x);
	    TestElementControl.IntersectStamp.origin.y =
		    WORLD_TO_DISPLAY (TestElementPtr->current.location.y);
	    TestElementControl.IntersectStamp.frame =
		     SetEquFrameIndex (TestElementPtr->current.image.farray[0],
		     TestElementPtr->current.image.frame);
	    TestElementControl.EndPoint = TestElementControl.IntersectStamp.origin;
	    if (DrawablesIntersect (&ElementControl,
		    &TestElementControl, MAX_TIME_VALUE))
	    {
		UnlockElement (hTestElement);

		break;
	    }
	}
	UnlockElement (hTestElement);
    }

    return (hTestElement != 0 ? TRUE : FALSE);
}

