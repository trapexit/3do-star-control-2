#ifndef _UNITS_H
#define _UNITS_H

#define ZOOM_SHIFT		8
#define MAX_ZOOM_OUT		(4 << ZOOM_SHIFT)

#define MAX_REDUCTION		3
#define MAX_VIS_REDUCTION	2
#define REDUCTION_SHIFT		1
#define NUM_VIEWS		(MAX_VIS_REDUCTION + 1)

#define ONE_SHIFT		2
#define BACKGROUND_SHIFT	3
#define SCALED_ONE		(1 << ONE_SHIFT)
#define DISPLAY_TO_WORLD(x)	((x)<<ONE_SHIFT)
#define WORLD_TO_DISPLAY(x)	((x)>>ONE_SHIFT)
#define DISPLAY_ALIGN(x)	((COORD)(x)&~(SCALED_ONE-1))
#define DISPLAY_ALIGN_X(x)	((COORD)((COUNT)(x)%LOG_SPACE_WIDTH)&~(SCALED_ONE-1))
#define DISPLAY_ALIGN_Y(y)	((COORD)((COUNT)(y)%LOG_SPACE_HEIGHT)&~(SCALED_ONE-1))

#define LOG_SPACE_WIDTH	\
	(DISPLAY_TO_WORLD (SPACE_WIDTH) << MAX_REDUCTION)
#define LOG_SPACE_HEIGHT	\
	(DISPLAY_TO_WORLD (SPACE_HEIGHT) << MAX_REDUCTION)
#define TRANSITION_WIDTH	\
	(DISPLAY_TO_WORLD (SPACE_WIDTH) << MAX_VIS_REDUCTION)
#define TRANSITION_HEIGHT	\
	(DISPLAY_TO_WORLD (SPACE_HEIGHT) << MAX_VIS_REDUCTION)
	
#define MAX_X_UNIVERSE		9999
#define MAX_Y_UNIVERSE		9999
#define MAX_X_LOGICAL		\
	((UNIVERSE_TO_LOGX (MAX_X_UNIVERSE + 1) > UNIVERSE_TO_LOGX (-1) ? \
		UNIVERSE_TO_LOGX (MAX_X_UNIVERSE) : UNIVERSE_TO_LOGX (-1)) \
		- 1L)
#define MAX_Y_LOGICAL		\
	((UNIVERSE_TO_LOGY (MAX_Y_UNIVERSE + 1) > UNIVERSE_TO_LOGY (-1) ? \
		UNIVERSE_TO_LOGY (MAX_Y_UNIVERSE) : UNIVERSE_TO_LOGY (-1)) \
		- 1L)
#define SECTOR_WIDTH		195
#define SECTOR_HEIGHT		25

#define SPHERE_RADIUS_INCREMENT	11

#define MAX_FLEET_STRENGTH	(254 * SPHERE_RADIUS_INCREMENT)

#define UNIT_SCREEN_WIDTH	63
#define UNIT_SCREEN_HEIGHT	50

#define LOGX_TO_UNIVERSE(lx)	\
	(COORD)( \
		((long)(lx) \
		* ((MAX_X_UNIVERSE + 1) >> 4)) \
		* 10 \
		/ ((long)((LOG_SPACE_WIDTH) >> 4) \
		* SECTOR_WIDTH) \
	)
#define  LOGY_TO_UNIVERSE(ly)	\
	(COORD)( \
		MAX_Y_UNIVERSE - (COORD)(((long)(ly) \
		* ((MAX_Y_UNIVERSE + 1) >> 4)) \
		/ ((long)((LOG_SPACE_HEIGHT) >> 4) \
		* SECTOR_HEIGHT)) \
	)
#define UNIVERSE_TO_LOGX(ux)	\
	( \
		((long)(ux) \
		* ((long)((LOG_SPACE_WIDTH) >> 4) * SECTOR_WIDTH) \
		+ ((((MAX_X_UNIVERSE + 1) >> 4) * 10) >> 1)) \
		/ (((MAX_X_UNIVERSE + 1) >> 4) * 10) \
	)
#define UNIVERSE_TO_LOGY(uy)	\
	( \
		((long)(MAX_Y_UNIVERSE - (uy)) \
		* ((long)((LOG_SPACE_HEIGHT) >> 4) * SECTOR_HEIGHT) \
		+ (((MAX_Y_UNIVERSE + 1) >> 4) >> 1)) \
		/ ((MAX_Y_UNIVERSE + 1) >> 4) \
	)

#define CIRCLE_SHIFT		6
#define FULL_CIRCLE		(1 << CIRCLE_SHIFT)
#define OCTANT_SHIFT		(CIRCLE_SHIFT - 3)	/* (1 << 3) == 8 */
#define HALF_CIRCLE		(FULL_CIRCLE >> 1)
#define QUADRANT		(FULL_CIRCLE >> 2)
#define OCTANT			(FULL_CIRCLE >> 3)

#define FACING_SHIFT		4

#define ANGLE_TO_FACING(a)	(((a)+(1<<(CIRCLE_SHIFT-FACING_SHIFT-1))) \
					>>(CIRCLE_SHIFT-FACING_SHIFT))
#define FACING_TO_ANGLE(f)	((f)<<(CIRCLE_SHIFT-FACING_SHIFT))

#define NORMALIZE_ANGLE(a)	((COUNT)(a)&(FULL_CIRCLE-1))
#define NORMALIZE_FACING(f)	((COUNT)(f)&((1 << FACING_SHIFT)-1))

#define SIN_SHIFT		14
#define SIN_SCALE		(1 << SIN_SHIFT)
#define INT_ADJUST(x)		((x)<<SIN_SHIFT)
#define FLT_ADJUST(x)		(SIZE)((x)*SIN_SCALE)
#define UNADJUST(x)		(SIZE)((x)>>SIN_SHIFT)
#define ROUND(x,y)		((x)+((x)>=0?((y)>>1):-((y)>>1)))

extern SIZE			sinetab[];
#define SINVAL(a)		sinetab[NORMALIZE_ANGLE(a)]
#define COSVAL(a)		SINVAL((a)+QUADRANT)
#define SINE(a,m)		((SIZE)((((long)SINVAL(a))*(long)(m))>>SIN_SHIFT))
#define COSINE(a,m)		SINE((a)+QUADRANT,m)
PROC_GLOBAL(
COUNT  ARCTAN, (delta_x, delta_y),
    ARG		(SIZE	delta_x)
    ARG_END	(SIZE	delta_y)
);

#define WRAP_VAL(v,w)		((COUNT)((v)<0?((v)+(w)):((v)>=(w)?((v)-(w)):(v))))
#define WRAP_X(x)		WRAP_VAL(x,LOG_SPACE_WIDTH)
#define WRAP_Y(y)		WRAP_VAL(y,LOG_SPACE_HEIGHT)
#define WRAP_DELTA_X(dx)	((dx)<0 ? \
		((-(dx)<=LOG_SPACE_WIDTH>>1)?(dx):(LOG_SPACE_WIDTH+(dx))) : \
		(((dx)<=LOG_SPACE_WIDTH>>1)?(dx):((dx)-LOG_SPACE_WIDTH)))
#define WRAP_DELTA_Y(dy)	((dy)<0 ? \
		((-(dy)<=LOG_SPACE_HEIGHT>>1)?(dy):(LOG_SPACE_HEIGHT+(dy))) : \
		(((dy)<=LOG_SPACE_HEIGHT>>1)?(dy):((dy)-LOG_SPACE_HEIGHT)))
#endif /* _UNITS_H */

