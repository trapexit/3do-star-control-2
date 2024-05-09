#ifndef _VELOCITY_H
#define _VELOCITY_H

#include <string.h>	/* for memset */

typedef struct velocity_desc
{
    COUNT	TravelAngle;
    EXTENT	vector;
    EXTENT	fract;
    EXTENT	error;
    EXTENT	incr;
} VELOCITY_DESC;
typedef VELOCITY_DESC	*PVELOCITY;
typedef VELOCITY_DESC	near *NPVELOCITY;
typedef VELOCITY_DESC	far *LPVELOCITY;
#ifdef QUEUE_TABLE
typedef PVELOCITY	VELOCITYPTR;
#else /* !QUEUE_TABLE */
typedef LPVELOCITY	VELOCITYPTR;
#endif /* QUEUE_TABLE */

#define ZeroVelocityComponents(pv)	MEMSET(pv,0,sizeof (*(pv)))
#define GetVelocityTravelAngle(pv)	(pv)->TravelAngle

PROC_GLOBAL(
void GetCurrentVelocityComponents, (velocityptr, pdx, pdy),
    ARG		(VELOCITYPTR	velocityptr)
    ARG		(PSIZE		pdx)
    ARG_END	(PSIZE		pdy)
);
PROC_GLOBAL(
void GetNextVelocityComponents, (velocityptr, pdx, pdy, num_frames),
    ARG		(VELOCITYPTR	velocityptr)
    ARG		(PSIZE		pdx)
    ARG		(PSIZE		pdy)
    ARG_END	(COUNT		num_frames)
);
PROC_GLOBAL(
void SetVelocityVector, (velocityptr, magnitude, facing),
    ARG		(VELOCITYPTR	velocityptr)
    ARG		(SIZE		magnitude)
    ARG_END	(COUNT		facing)
);
PROC_GLOBAL(
void SetVelocityComponents, (velocityptr, dx, dy),
    ARG		(VELOCITYPTR	velocityptr)
    ARG		(SIZE		dx)
    ARG_END	(SIZE		dy)
);
PROC_GLOBAL(
void DeltaVelocityComponents, (velocityptr, dx, dy),
    ARG		(VELOCITYPTR	velocityptr)
    ARG		(SIZE		dx)
    ARG_END	(SIZE		dy)
);

#define VELOCITY_SHIFT	5
#define VELOCITY_SCALE (1<<VELOCITY_SHIFT)

#define VELOCITY_TO_WORLD(v)	((v)>>VELOCITY_SHIFT)
#define WORLD_TO_VELOCITY(l)	((l)<<VELOCITY_SHIFT)

#endif /* _VELOCITY_H */

