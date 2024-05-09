#ifndef _APPGLUE_H
#define _APPGLUE_H

#include "vargs.h"

enum
{
    RANDOM,
    SIN,
    ARC_TANGENT,
    SQUARE_ROOT,

    GET_STRING_ADDRESS,
    SET_ABS_STRTAB_INDEX,

    CAPTURE_DRAWABLE,
    GET_FRAME_INDEX,
    GET_FRAME_COUNT,
    INC_FRAME_INDEX,
    DEC_FRAME_INDEX,
    SET_ABS_FRAME_INDEX,
    SET_EQU_FRAME_INDEX,
    RELEASE_DRAWABLE,
    DESTROY_DRAWABLE,

    ADD_EVENT,
    DRAW_LANDERS,
    ACTIVATE_STAR_SHIP,

    ALLOC_ELEMENT,
    INSERT_ELEMENT,
    PUT_ELEMENT,
    REMOVE_ELEMENT,
    FREE_ELEMENT,
    GET_HEAD_ELEMENT,
    GET_TAIL_ELEMENT,

    TRACK_SHIP,
    UNTARGET,
    PLOT_INTERCEPT,
    SHIP_INTELLIGENCE,
    DELTA_ENERGY,
    DELTA_CREW,
    ABANDON_SHIP,
    COLLIDE,
    DRAWABLES_INTERSECT,
    PROCESS_SOUND,

    INITIALIZE_MISSILE,
    INITIALIZE_LASER,
    SET_REL_FRAME_INDEX,
    WEAPON_COLLISION,

    SET_VELOCITY_VECTOR,
    SET_VELOCITY_COMPONENTS,
    DELTA_VELOCITY_COMPONENTS,
    GET_CUR_VELOCITY_COMPONENTS,

    DO_MENU_OPTIONS,
    NPC_PHRASE,
    RESPONSE_PHRASE,

    SET_CONTEXT_FG_FRAME,
    GET_FRAME_HANDLE,
    SET_FRAME_HOT,
    GET_FRAME_RECT,
    CLEAR_DRAWABLE,
    DRAW_STAMP,
    SET_CONTEXT_DRAW_STATE,

    GET_STRTAB_COUNT,
    SET_COLOR_MAP,

    DESTROY_STRING_TABLE,
    CAPTURE_STRING_TABLE,
    RELEASE_STRING_TABLE,

    DRAW_STORAGE_BAYS,
    RELEASE_FONT,

    VSPRINTF,

    CALCULATE_GRAVITY,
    TIME_SPACE_MATTER_CONFLICT,

    LOAD_MUSIC,
    SHIP_WEAPONS,

    MODIFY_SILHOUETTE,
    DELTA_SIS_GAUGES,

    INERTIAL_THRUST,
    GET_NEXT_VELOCITY_COMPONENTS,

    XFORM_COLOR_MAP
};

#ifndef IS_CODERES

#define SET_FUNCPTR	SET_FARPTR
#define GET_FUNCPTR	GET_FARPTR

#define PUSH_CONTEXT
#define POP_CONTEXT

PROC_GLOBAL(
DWORD CDECL race_glue, (selector, args),
    ARG		(COUNT 		selector)
    ARG		(vararg_dcl	args)
    ARG_VAR
);

#else /* IS_CODERES */

#define SET_FUNCPTR	SET_NEARPTR
#define GET_FUNCPTR	(LPVOIDFUNC)GET_NEARPTR

extern UWORD	app_dseg;

#define PUSH_CONTEXT	\
{ int __stupid_compiler; _asm push ds _asm mov ax,cs _asm mov ds,ax } {
#define POP_CONTEXT	\
} { _asm pop ds }

#undef LOCDATAPTR
#define LOCDATAPTR	LPLOCDATA
#undef GLOBDATAPTR
#define GLOBDATAPTR	LPGLOBDATA
#undef GLOBAL
#define GLOBAL(f)	GlobDataPtr->f

#undef STARSHIPPTR
#define STARSHIPPTR	LPSTARSHIP
#undef GetElementStarShip
#define GetElementStarShip(e,psd)	\
{ \
    SET_SEGMENT (psd, app_dseg); \
    SET_OFFSET (psd, (e)->pParent); \
}

#undef ELEMENTPTR
#define ELEMENTPTR	LPELEMENT
#undef LockElement
#define LockElement(h,eptr)	\
{ \
    SET_SEGMENT (eptr, app_dseg); \
    SET_OFFSET (eptr, h); \
}

#undef EVALUATE_DESCPTR
#define EVALUATE_DESCPTR	LPEVALUATE_DESC

#undef LoadColorMap
#undef LoadAnimation
#undef LoadMusic
#undef AllocElement
#undef PutElement
#undef InsertElement
#undef RemoveElement
#undef FreeElement
#undef GetHeadElement
#undef GetTailElement

PROC_GLOBAL(
DWORD (CDECL far *glue_func), (selector),
    ARG		(COUNT	selector)
    ARG_VAR
);

#define random()		\
	(*glue_func) (RANDOM)
#undef SINE
#define SINE(a,m)		\
	(SIZE)(((long)(*glue_func) (SIN, a)*(m))>>SIN_SHIFT)
#define ARCTAN(dx,dy)		\
	(COUNT)(*glue_func) (ARC_TANGENT, dx, dy)
#define square_root(x)		\
	(COUNT)(*glue_func) (SQUARE_ROOT, (DWORD)(x))

#define GetStringAddress(s)		\
	(STRINGPTR)(*glue_func) (GET_STRING_ADDRESS, s)
#define SetAbsStringTableIndex(s,i)		\
	(STRING)(*glue_func) (SET_ABS_STRTAB_INDEX, s, i)

#define GetStringTableCount(s)	\
	(COUNT)(*glue_func) (GET_STRTAB_COUNT, s)
#define SetColorMap(cmptr,pr)	\
	(BOOLEAN)(*glue_func) (SET_COLOR_MAP, cmptr, (PRECT)(pr))

#define DestroyStringTable(st)	\
	(BOOLEAN)(*glue_func) (DESTROY_STRING_TABLE, st)
#define CaptureStringTable(st)	\
	(STRING)(*glue_func) (CAPTURE_STRING_TABLE, st)
#define ReleaseStringTable(s)	\
	(STRING_TABLE)(*glue_func) (RELEASE_STRING_TABLE, s)

#define CaptureDrawable(d)		\
	(FRAME)(*glue_func) (CAPTURE_DRAWABLE, d)
#define GetFrameIndex(f)		\
	(COUNT)(*glue_func) (GET_FRAME_INDEX, f)
#define GetFrameCount(f)		\
	(COUNT)(*glue_func) (GET_FRAME_COUNT, f)
#define IncFrameIndex(f)		\
	(FRAME)(*glue_func) (INC_FRAME_INDEX, f)
#define DecFrameIndex(f)		\
	(FRAME)(*glue_func) (DEC_FRAME_INDEX, f)
#define SetAbsFrameIndex(f,i)		\
	(FRAME)(*glue_func) (SET_ABS_FRAME_INDEX, f, i)
#define SetRelFrameIndex(f,i)		\
	(FRAME)(*glue_func) (SET_REL_FRAME_INDEX, f, i)
#define SetEquFrameIndex(f0,f1)		\
	(FRAME)(*glue_func) (SET_EQU_FRAME_INDEX, f0, f1)
#define ReleaseDrawable(f)		\
	(DRAWABLE)(*glue_func) (RELEASE_DRAWABLE, f)
#define DestroyDrawable(d)		\
	(BOOLEAN)(*glue_func) (DESTROY_DRAWABLE, d)

#define AddEvent(t,mi,di,yi,fi)	\
	(HEVENT)(*glue_func) (ADD_EVENT, t, mi, di, yi, fi)
#define DrawStorageBays(r)	\
	(*glue_func) (DRAW_STORAGE_BAYS, r)

#define DestroyFont(f)		\
	(BOOLEAN)(*glue_func) (DESTROY_FONT, f)
#define DrawLanders()		\
	(*glue_func) (DRAW_LANDERS)
#define ActivateStarShip(s,state)		\
	(COUNT)(*glue_func) (ACTIVATE_STAR_SHIP, s, state)

#define AllocElement()		\
	(HELEMENT)(*glue_func) (ALLOC_ELEMENT)
#define PutElement(e)		\
	(*glue_func) (PUT_ELEMENT, e)
#define InsertElement(e0,e1)		\
	(*glue_func) (INSERT_ELEMENT, e0, e1)
#define RemoveElement(e)		\
	(*glue_func) (REMOVE_ELEMENT, e)
#define FreeElement(e)		\
	(*glue_func) (FREE_ELEMENT, e)
#define GetHeadElement()		\
	(HELEMENT)(*glue_func) (GET_HEAD_ELEMENT)
#define GetTailElement()		\
	(HELEMENT)(*glue_func) (GET_TAIL_ELEMENT)

#define TrackShip(eptr,pf)	\
	(SIZE)(*glue_func) (TRACK_SHIP, (PELEMENT)(eptr), pf)
#define Untarget(eptr)		\
	(*glue_func) (UNTARGET, (PELEMENT)(eptr))
#define PlotIntercept(eptr0,eptr1,mt,moe)	\
	(COUNT)(*glue_func) (PLOT_INTERCEPT, \
		(PELEMENT)(eptr0), (PELEMENT)(eptr1), mt, moe)
#define ship_intelligence(eptr,oarray,oc)	\
	(*glue_func) (SHIP_INTELLIGENCE, \
		(PELEMENT)(eptr), \
		(PEVALUATE_DESC)(oarray), \
		oc)
#define ship_weapons(sptr,optr,margin)	\
	(BOOLEAN)(*glue_func) (SHIP_WEAPONS, \
		(PELEMENT)(sptr), \
		(PELEMENT)(optr), \
		margin)
#define DeltaEnergy(eptr,de)	\
	(BOOLEAN)(*glue_func) (DELTA_ENERGY,	\
		(PELEMENT)(eptr), de)
#define DeltaCrew(eptr,dc)	\
	(BOOLEAN)(*glue_func) (DELTA_CREW,	\
		(PELEMENT)(eptr), dc)
#define AbandonShip(eptr0,eptr1,dc)	\
	(*glue_func) (ABANDON_SHIP,	\
		(PELEMENT)(eptr0), (PELEMENT)(eptr1), dc)
#define collision(eptr0,pPt0,eptr1,pPt1)	\
	(*glue_func) (COLLIDE, eptr0, pPt0, eptr1, pPt1)
#define DrawablesIntersect(ic0, ic1, t)	\
	(TIME_VALUE)(*glue_func) (DRAWABLES_INTERSECT, ic0, ic1, t)
#define ProcessSound(s)	\
	(*glue_func) (PROCESS_SOUND, s)

#define initialize_missile(pMB)	\
	(HELEMENT)(*glue_func) (INITIALIZE_MISSILE, (PMISSILE_BLOCK)(pMB))
#define initialize_laser(pLB)	\
	(HELEMENT)(*glue_func) (INITIALIZE_LASER, (PLASER_BLOCK)pLB)
#define weapon_collision(eptr0,pPt0,eptr1,pPt1)	\
	(HELEMENT)(*glue_func) (WEAPON_COLLISION, eptr0, pPt0, eptr1, pPt1)

#define SetVelocityVector(pv,s,f)	\
	(*glue_func) (SET_VELOCITY_VECTOR, (PVELOCITY)(pv), s, f)
#define SetVelocityComponents(pv,dx,dy)	\
	(*glue_func) (SET_VELOCITY_COMPONENTS, (PVELOCITY)(pv), dx, dy)
#define DeltaVelocityComponents(pv,dx,dy)	\
	(*glue_func) (DELTA_VELOCITY_COMPONENTS, (PVELOCITY)(pv), dx, dy)
#define GetCurrentVelocityComponents(pv,pdx,pdy)	\
	(*glue_func) (GET_CUR_VELOCITY_COMPONENTS, (PVELOCITY)(pv), pdx, pdy)

#define DoMenuOptions()		\
	(*glue_func) (DO_MENU_OPTIONS)
#define NPCPhrase(s)		\
	(*glue_func) (NPC_PHRASE, (LPSTR)s)
#define Response(s,a)	\
	(*glue_func) (RESPONSE_PHRASE, (LPSTR)s, a)

#define SetContextFGFrame(d)	\
	(FRAME)(*glue_func) (SET_CONTEXT_FG_FRAME, d)
#define GetFrameHandle(f)	\
	(DRAWABLE)(*glue_func) (GET_FRAME_HANDLE, f)
#define SetFrameHot(f,h)	\
	(HOT_SPOT)(*glue_func) (SET_FRAME_HOT, f, h)
#define GetFrameRect(f,pR)	\
	(BOOLEAN)(*glue_func) (GET_FRAME_RECT, f, pR)
#define ClearDrawable()	\
	(COUNT)(*glue_func) (CLEAR_DRAWABLE)
#define DrawStamp(pS)	\
	(COUNT)(*glue_func) (DRAW_STAMP, pS)
#define SetContextDrawState(s)	\
	(DRAW_STATE)(*glue_func) (SET_CONTEXT_DRAW_STATE, s)

#define vsprintf(b,f,a)	\
	(int)(*glue_func) (VSPRINTF,(LPSTR)b,(LPSTR)f,a)

#define CalculateGravity(lpE)	\
	(BOOLEAN)(*glue_func) (CALCULATE_GRAVITY, (LPELEMENT)(lpE))
#define TimeSpaceMatterConflict(Eptr)	\
	(BOOLEAN)(*glue_func) (TIME_SPACE_MATTER_CONFLICT, (PELEMENT)(Eptr))

#define LoadMusic(res)	\
	(MUSIC_REF)(*glue_func) (LOAD_MUSIC, res)

#define ModifySilhouette(eptr,ps,f)	\
	(FRAME)(*glue_func) (MODIFY_SILHOUETTE, (PELEMENT)(eptr), (PSTAMP)(ps), f)
#define DeltaSISGauges(c,f,r)	\
	(*glue_func) (DELTA_SIS_GAUGES, c, f, r)

#define inertial_thrust(eptr)	\
	(UWORD)(*glue_func) (INERTIAL_THRUST, (PELEMENT)(eptr))
#define GetNextVelocityComponents(pv,pdx,pdy,nt)	\
	(*glue_func) (GET_NEXT_VELOCITY_COMPONENTS, (PVELOCITY)(pv), pdx, pdy, nt)

#define XFormColorMap(CMAddr,t)	\
	(DWORD)(*glue_func) (XFORM_COLOR_MAP, (COLORMAPPTR)(CMAddr), t)

#endif /* IS_CODERES */

extern GLOBDATAPTR	GlobDataPtr;

#endif /* _APPGLUE_H */

