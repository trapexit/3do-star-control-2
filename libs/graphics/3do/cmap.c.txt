#include "gfxintrn.h"
#include "timlib.h"
#include "defs3do.h"

#define STACK_SIZE	256

static struct
{
    COLORMAPPTR	CMapPtr;
    SIZE	Ticks;
    union
    {
	COUNT	NumCycles;
	TASK	XFormTask;
    } tc;
} TaskControl;

PROC_GLOBAL(
void _threedo_map_color, (CMap, color_indices),
    ARG		(register PBYTE	CMap)
    ARG_END	(UWORD		color_indices)
);

BOOLEAN
BatchColorMap (COLORMAPPTR ColorMapPtr)
{
    return (SetColorMap (ColorMapPtr));
}

PROC(
BOOLEAN ReadColorMap, (ColorMapPtr),
    ARG_END	(COLORMAPPTR	ColorMapPtr)
)
{
    return (TRUE);
}

BOOLEAN
SetColorMap (COLORMAPPTR ColorMapPtr)
{
    if (ColorMapPtr)
	_threedo_load_pluts (ColorMapPtr);

    return (TRUE);
}

static void
color_cycle ()
{
    UWORD	color_beg, color_len, color_indices;
    COUNT	Cycles, CycleCount;
    DWORD	TimeIn, SleepTicks;
    COLORMAPPTR	BegMapPtr, CurMapPtr;

    Cycles = CycleCount = TaskControl.tc.NumCycles;
    BegMapPtr = TaskControl.CMapPtr;
    SleepTicks = TaskControl.Ticks;

    color_indices = MAKE_WORD (BegMapPtr[0], BegMapPtr[1]);
    color_beg = *BegMapPtr++ * 3;
    color_len = (*BegMapPtr++ + 1) * 3 - color_beg;
    CurMapPtr = BegMapPtr;

    TaskControl.CMapPtr = 0;
    TimeIn = GetTimeCounter ();
    for (;;)
    {
	SuspendTasking ();
	_threedo_set_colors (CurMapPtr, color_indices);
	ResumeTasking ();
	
	if (--Cycles)
	    CurMapPtr += color_len + sizeof (BYTE) * 2;
	else
	{
	    Cycles = CycleCount;
	    CurMapPtr = BegMapPtr;
	}

	TimeIn = SleepTask (TimeIn + SleepTicks);
	
	if (TaskControl.CMapPtr)
	{
	    TaskControl.CMapPtr = (COLORMAPPTR)0;
	    for (;;)
		TaskSwitch ();
	}
    }
}

PROC(
CYCLE_REF CycleColorMap, (ColorMapPtr, Cycles, TimeInterval),
    ARG		(COLORMAPPTR	ColorMapPtr)
    ARG		(COUNT		Cycles)
    ARG_END	(SIZE		TimeInterval)
)
{
    if (ColorMapPtr && Cycles && ColorMapPtr[0] <= ColorMapPtr[1])
    {
	TASK	T;

	SuspendTasking ();
	while (TaskControl.CMapPtr)
	{
	    ResumeTasking ();
	    TaskSwitch ();
	    SuspendTasking ();
	}
	ResumeTasking ();

	TaskControl.CMapPtr = ColorMapPtr;
	TaskControl.tc.NumCycles = Cycles;
	if ((TaskControl.Ticks = TimeInterval) <= 0)
	    TaskControl.Ticks = 1;
	if (T = AddTask (color_cycle, STACK_SIZE))
	{
	    _batch_flags |= ENABLE_CYCLE;
	    do
		TaskSwitch ();
	    while (TaskControl.CMapPtr);
	}
	TaskControl.CMapPtr = 0;

	return ((CYCLE_REF)T);
    }

    return (0);
}

PROC(
void StopCycleColorMap, (CycleRef),
    ARG_END	(CYCLE_REF	CycleRef)
)
{
    TASK	T;

    if (T = (TASK)CycleRef)
    {
	TaskControl.CMapPtr = (COLORMAPPTR)1;
	while (TaskControl.CMapPtr)
	    TaskSwitch ();

	_batch_flags &= ~ENABLE_CYCLE;
	CheckForTasks ();
	DeleteTask (T);
    }
}

#define NO_INTENSITY		0x00
#define NORMAL_INTENSITY	0xff
#define FULL_INTENSITY		(0xff * 32)

STATIC int	cur = NORMAL_INTENSITY, end, XForming;

PROC(STATIC
void far xform_clut_task, (),
    ARG_VOID
)
{
    TASK	T;
    SIZE	TDelta, TTotal;
    DWORD	CurTime;

    XForming = TRUE;
    while ((T = TaskControl.tc.XFormTask) == 0)
	TaskSwitch ();
    TTotal = TaskControl.Ticks;
    TaskControl.tc.XFormTask = 0;

    {
	CurTime = GetTimeCounter ();
	do
	{
	    DWORD	StartTime;

	    StartTime = CurTime;
	    CurTime = SleepTask (CurTime + 2);
	    if (!XForming || (TDelta = (SIZE)(CurTime - StartTime)) > TTotal)
		TDelta = TTotal;

	    cur += (end - cur) * TDelta / TTotal;
	    _threedo_change_clut (cur);
	} while (TTotal -= TDelta);
    }

    XForming = FALSE;
    DeleteTask (T);
}

PROC(
DWORD XFormColorMap, (ColorMapPtr, TimeInterval),
    ARG		(COLORMAPPTR	ColorMapPtr)
    ARG_END	(SIZE		TimeInterval)
)
{
    BYTE	what;
    DWORD	TimeOut;

    FlushColorXForms ();

    if (ColorMapPtr == (COLORMAPPTR)0)
	return (0);
	
    switch (what = *ColorMapPtr)
    {
	case FadeAllToBlack:
	case FadeSomeToBlack:
	    end = NO_INTENSITY;
	    break;
	case FadeAllToColor:
	case FadeSomeToColor:
	    end = NORMAL_INTENSITY;
	    break;
	case FadeAllToWhite:
	case FadeSomeToWhite:
	    end = FULL_INTENSITY;
	    break;
	default:
	    return (GetTimeCounter ());
    }

    CheckForTasks ();

    if (what == FadeAllToBlack || what == FadeAllToWhite || what == FadeAllToColor)
	_threedo_enable_fade ();

    if ((TaskControl.Ticks = TimeInterval) <= 0
	    || (TaskControl.tc.XFormTask = AddTask (xform_clut_task, 1024)) == 0)
    {
	_threedo_change_clut (end);
	TimeOut = GetTimeCounter ();
    }
    else
    {
	do
	    TaskSwitch ();
	while (TaskControl.tc.XFormTask);

	TimeOut = GetTimeCounter () + TimeInterval + 1;
    }

    return (TimeOut);
}

PROC(
void FlushColorXForms, (),
    ARG_VOID
)
{
    if (XForming)
    {
	XForming = FALSE;
	TaskSwitch ();
    }
}

