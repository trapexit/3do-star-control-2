#ifndef _COLLIDE_H
#define _COLLIDE_H

#define COLLISION_TURN_WAIT	1
#define COLLISION_THRUST_WAIT	3

#define SKIP_COLLISION		(NONSOLID | DISAPPEARING)
#define CollidingElement(e)		\
	(!((e)->state_flags & SKIP_COLLISION))
#define CollisionPossible(e0,e1)	\
	(CollidingElement (e0) \
	&& (!(((e1)->state_flags & (e0)->state_flags) & COLLISION) \
	&& ((!(((e1)->state_flags & (e0)->state_flags) & IGNORE_SIMILAR) \
	|| (e1)->pParent != (e0)->pParent)) \
	&& ((e1)->mass_points || (e0)->mass_points)))

#define InitIntersectStartPoint(eptr)	\
{ \
    (eptr)->IntersectControl.IntersectStamp.origin.x = \
	    WORLD_TO_DISPLAY ((eptr)->current.location.x); \
    (eptr)->IntersectControl.IntersectStamp.origin.y = \
	    WORLD_TO_DISPLAY ((eptr)->current.location.y); \
}

#define InitIntersectEndPoint(eptr)	\
{ \
    (eptr)->IntersectControl.EndPoint.x = \
	    WORLD_TO_DISPLAY ((eptr)->next.location.x); \
    (eptr)->IntersectControl.EndPoint.y = \
	    WORLD_TO_DISPLAY ((eptr)->next.location.y); \
}

#define InitIntersectFrame(eptr)	\
{ \
    (eptr)->IntersectControl.IntersectStamp.frame = \
	     SetEquFrameIndex ((eptr)->next.image.farray[0], \
	     (eptr)->next.image.frame); \
}

PROC_GLOBAL(
void collide, (ElementPtr0, ElementPtr1),
    ARG		(ELEMENTPTR	ElementPtr0)
    ARG_END	(ELEMENTPTR	ElementPtr1)
);

#endif /* _COLLIDE_H */

