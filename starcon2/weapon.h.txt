#ifndef _WEAPON_H
#define _WEAPON_H

typedef struct
{
    COORD		cx, cy, ex, ey;
    ELEMENT_FLAGS	sender;
    SIZE		pixoffs;
    COUNT		face;
    COLOR		color;
} LASER_BLOCK;
typedef LASER_BLOCK	*PLASER_BLOCK;
typedef LASER_BLOCK	near *NPLASER_BLOCK;
typedef LASER_BLOCK	far *LPLASER_BLOCK;

typedef struct
{
    COORD		cx, cy;
    ELEMENT_FLAGS	sender;
    SIZE		pixoffs, speed, hit_points, damage;
    COUNT		face, index, life;
    LPFRAME		farray;
    PROC_PARAMETER(
    void (far *preprocess_func), (ElementPtr),
	ARG_END	(LPELEMENT	ElementPtr)
    );
    SIZE		blast_offs;
} MISSILE_BLOCK;
typedef MISSILE_BLOCK	*PMISSILE_BLOCK;
typedef MISSILE_BLOCK	near *NPMISSILE_BLOCK;
typedef MISSILE_BLOCK	far *LPMISSILE_BLOCK;

PROC_GLOBAL(
HELEMENT initialize_laser, (pLaserBlock),
    ARG_END	(PLASER_BLOCK	pLaserBlock)
);
PROC_GLOBAL(
HELEMENT initialize_missile, (pMissileBlock),
    ARG_END	(PMISSILE_BLOCK	pMissileBlock)
);
PROC_GLOBAL(
HELEMENT weapon_collision, (ElementPtr0, pPt0, ElementPtr1, pPt1),
    ARG		(LPELEMENT	ElementPtr0)
    ARG		(PPOINT		pPt0)
    ARG		(LPELEMENT	ElementPtr1)
    ARG_END	(PPOINT		pPt1)
);
PROC_GLOBAL(
SIZE TrackShip, (Tracker, pfacing),
    ARG		(ELEMENTPTR	Tracker)
    ARG_END	(PCOUNT		pfacing)
);
PROC_GLOBAL(
void Untarget, (ElementPtr),
    ARG_END	(ELEMENTPTR	ElementPtr)
);

#define MODIFY_IMAGE	(1 << 0)
#define MODIFY_SWAP	(1 << 1)

PROC_GLOBAL(
FRAME ModifySilhouette, (ElementPtr, modify_stamp, modify_flags),
    ARG		(ELEMENTPTR	ElementPtr)
    ARG		(PSTAMP		modify_stamp)
    ARG_END	(BYTE		modify_flags)
);

#endif /* _WEAPON_H */

