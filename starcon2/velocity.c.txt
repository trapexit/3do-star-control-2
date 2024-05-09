#include "starcon.h"

#define VELOCITY_REMAINDER(v)	((v) & (VELOCITY_SCALE - 1))

PROC(
void GetCurrentVelocityComponents, (velocityptr, pdx, pdy),
    ARG		(VELOCITYPTR	velocityptr)
    ARG		(PSIZE		pdx)
    ARG_END	(PSIZE		pdy)
)
{
    *pdx = WORLD_TO_VELOCITY (velocityptr->vector.width)
	    + (velocityptr->fract.width - (SIZE)HIBYTE (velocityptr->incr.width));
    *pdy = WORLD_TO_VELOCITY (velocityptr->vector.height)
	    + (velocityptr->fract.height - (SIZE)HIBYTE (velocityptr->incr.height));
}

PROC(
void GetNextVelocityComponents, (velocityptr, pdx, pdy, num_frames),
    ARG		(VELOCITYPTR	velocityptr)
    ARG		(PSIZE		pdx)
    ARG		(PSIZE		pdy)
    ARG_END	(COUNT		num_frames)
)
{
    COUNT	e;

    e = (COUNT)((COUNT)velocityptr->error.width +
	    ((COUNT)velocityptr->fract.width * num_frames));
    *pdx = (velocityptr->vector.width * num_frames)
	    + ((SIZE)((SBYTE)LOBYTE (velocityptr->incr.width))
	    * (e >> VELOCITY_SHIFT));
    velocityptr->error.width = VELOCITY_REMAINDER (e);

    e = (COUNT)((COUNT)velocityptr->error.height +
	    ((COUNT)velocityptr->fract.height * num_frames));
    *pdy = (velocityptr->vector.height * num_frames)
	    + ((SIZE)((SBYTE)LOBYTE (velocityptr->incr.height))
	    * (e >> VELOCITY_SHIFT));
    velocityptr->error.height = VELOCITY_REMAINDER (e);
}

PROC(
void SetVelocityVector, (velocityptr, magnitude, facing),
    ARG		(register VELOCITYPTR	velocityptr)
    ARG		(register SIZE		magnitude)
    ARG_END	(COUNT			facing)
)
{
    register COUNT	angle;
    register SIZE	dx, dy;

    angle = velocityptr->TravelAngle =
	    FACING_TO_ANGLE (NORMALIZE_FACING (facing));
    magnitude = WORLD_TO_VELOCITY (magnitude);
    dx = COSINE (angle, magnitude);
    dy = SINE (angle, magnitude);
    if (dx >= 0)
    {
	velocityptr->vector.width = VELOCITY_TO_WORLD (dx);
	velocityptr->incr.width = MAKE_WORD ((BYTE)1, (BYTE)0);
    }
    else
    {
	dx = -dx;
	velocityptr->vector.width = -VELOCITY_TO_WORLD (dx);
	velocityptr->incr.width =
		MAKE_WORD ((BYTE)0xFF, (BYTE)(VELOCITY_REMAINDER (dx) << 1));
    }
    if (dy >= 0)
    {
	velocityptr->vector.height = VELOCITY_TO_WORLD (dy);
	velocityptr->incr.height = MAKE_WORD ((BYTE)1, (BYTE)0);
    }
    else
    {
	dy = -dy;
	velocityptr->vector.height = -VELOCITY_TO_WORLD (dy);
	velocityptr->incr.height =
		MAKE_WORD ((BYTE)0xFF, (BYTE)(VELOCITY_REMAINDER (dy) << 1));
    }

    velocityptr->fract.width = VELOCITY_REMAINDER (dx);
    velocityptr->fract.height = VELOCITY_REMAINDER (dy);
    velocityptr->error.width = velocityptr->error.height = 0;
}

PROC(
void SetVelocityComponents, (velocityptr, dx, dy),
    ARG		(register VELOCITYPTR	velocityptr)
    ARG		(register SIZE		dx)
    ARG_END	(register SIZE		dy)
)
{
    COUNT	angle;

    if ((angle = ARCTAN (dx, dy)) == FULL_CIRCLE)
    {
	ZeroVelocityComponents (velocityptr);
    }
    else
    {
	if (dx >= 0)
	{
	    velocityptr->vector.width = VELOCITY_TO_WORLD (dx);
	    velocityptr->incr.width = MAKE_WORD ((BYTE)1, (BYTE)0);
	}
	else
	{
	    dx = -dx;
	    velocityptr->vector.width = -VELOCITY_TO_WORLD (dx);
	    velocityptr->incr.width =
		    MAKE_WORD ((BYTE)0xFF, (BYTE)(VELOCITY_REMAINDER (dx) << 1));
	}
	if (dy >= 0)
	{
	    velocityptr->vector.height = VELOCITY_TO_WORLD (dy);
	    velocityptr->incr.height = MAKE_WORD ((BYTE)1, (BYTE)0);
	}
	else
	{
	    dy = -dy;
	    velocityptr->vector.height = -VELOCITY_TO_WORLD (dy);
	    velocityptr->incr.height =
		    MAKE_WORD ((BYTE)0xFF, (BYTE)(VELOCITY_REMAINDER (dy) << 1));
	}

	velocityptr->fract.width = VELOCITY_REMAINDER (dx);
	velocityptr->fract.height = VELOCITY_REMAINDER (dy);
	velocityptr->error.width = velocityptr->error.height = 0;
    }

    velocityptr->TravelAngle = angle;
}

PROC(
void DeltaVelocityComponents, (velocityptr, dx, dy),
    ARG		(VELOCITYPTR	velocityptr)
    ARG		(SIZE		dx)
    ARG_END	(SIZE		dy)
)
{

    dx += WORLD_TO_VELOCITY (velocityptr->vector.width)
	    + (velocityptr->fract.width - (SIZE)HIBYTE (velocityptr->incr.width));
    dy += WORLD_TO_VELOCITY (velocityptr->vector.height)
	    + (velocityptr->fract.height - (SIZE)HIBYTE (velocityptr->incr.height));

    SetVelocityComponents (velocityptr, dx, dy);
}

