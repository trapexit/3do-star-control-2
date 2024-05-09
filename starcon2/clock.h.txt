#ifndef _CLOCK_H
#define _CLOCK_H

#define START_YEAR	2155

enum
{
    ARILOU_ENTRANCE_EVENT = 0,
    ARILOU_EXIT_EVENT,
    HYPERSPACE_ENCOUNTER_EVENT,
    KOHR_AH_VICTORIOUS_EVENT,
    ADVANCE_PKUNK_MISSION,
    ADVANCE_THRADD_MISSION,
    ZOQFOT_DISTRESS_EVENT,
    ZOQFOT_DEATH_EVENT,
    SHOFIXTI_RETURN_EVENT,
    ADVANCE_UTWIG_SUPOX_MISSION,
    KOHR_AH_GENOCIDE_EVENT,
    SPATHI_SHIELD_EVENT,
    ADVANCE_ILWRATH_MISSION,
    ADVANCE_MYCON_MISSION,
    ARILOU_UMGAH_CHECK,
    YEHAT_REBEL_EVENT,
    SLYLANDRO_RAMP_UP,
    SLYLANDRO_RAMP_DOWN,

    NUM_EVENTS
};

#define UPDATE_DAY	(1 << 0)
#define UPDATE_MONTH	(1 << 1)
#define UPDATE_YEAR	(1 << 2)

typedef struct
{
    BYTE		day_index, month_index;
    COUNT		year_index;
    SIZE		tick_count, day_in_ticks;
    SEMAPHORE		clock_sem;
    TASK		clock_task;
    DWORD		TimeCounter;

    QUEUE		event_q;
} CLOCK_STATE;
typedef CLOCK_STATE	*PCLOCK_STATE;
typedef CLOCK_STATE	near *NPCLOCK_STATE;
typedef CLOCK_STATE	far *LPCLOCK_STATE;

typedef QUEUE_HANDLE	HEVENT;

typedef struct event
{
    HEVENT	pred, succ;

    BYTE	day_index, month_index;
    COUNT	year_index;
    BYTE	func_index;
} EVENT;
typedef EVENT	*PEVENT;
typedef EVENT	near *NPEVENT;
typedef EVENT	far *LPEVENT;

typedef enum
{
    ABSOLUTE_EVENT = 0,
    RELATIVE_EVENT
} EVENT_TYPE;

#ifdef QUEUE_TABLE
#define EVENTPTR	PEVENT
#else /* !QUEUE_TABLE */
#define EVENTPTR	LPEVENT
#endif /* QUEUE_TABLE */

#define AllocEvent()		AllocLink (&GLOBAL (GameClock.event_q))
#define PutEvent(h)		PutQueue (&GLOBAL (GameClock.event_q), h)
#define InsertEvent(h,i)	InsertQueue (&GLOBAL (GameClock.event_q), h, i)
#define GetHeadEvent()		GetHeadLink (&GLOBAL (GameClock.event_q))
#define GetTailEvent()		GetTailLink (&GLOBAL (GameClock.event_q))
#define LockEvent(h,eptr)	*(eptr) = (EVENTPTR)LockLink (&GLOBAL (GameClock.event_q), h)
#define UnlockEvent(h)		UnlockLink (&GLOBAL (GameClock.event_q), h)
#define RemoveEvent(h)		RemoveQueue (&GLOBAL (GameClock.event_q), h)
#define FreeEvent(h)		FreeLink (&GLOBAL (GameClock.event_q), h)
#define GetPredEvent(l)		_GetPredLink (l)
#define GetSuccEvent(l)		_GetSuccLink (l)

		/* rates are in seconds per game day */
#define HYPERSPACE_CLOCK_RATE		5
#define INTERPLANETARY_CLOCK_RATE	30

PROC_GLOBAL(
BOOLEAN InitGameClock, (),
    ARG_VOID
);
PROC_GLOBAL(
BOOLEAN UninitGameClock, (),
    ARG_VOID
);
PROC_GLOBAL(
void SuspendGameClock, (),
    ARG_VOID
);
PROC_GLOBAL(
void ResumeGameClock, (),
    ARG_VOID
);
PROC_GLOBAL(
BOOLEAN GameClockRunning, (),
    ARG_VOID
);
PROC_GLOBAL(
void SetGameClockRate, (seconds_per_day),
    ARG_END	(COUNT	seconds_per_day)
);
PROC_GLOBAL(
BOOLEAN ValidateEvent, (type, pmonth_index, pday_index, pyear_index),
    ARG		(EVENT_TYPE	type)
    ARG		(PCOUNT		pmonth_index)
    ARG		(PCOUNT		pday_index)
    ARG_END	(PCOUNT		pyear_index)
);
PROC_GLOBAL(
HEVENT AddEvent, (type, month_index, day_index, year_index, func_index),
    ARG		(EVENT_TYPE	type)
    ARG		(COUNT		month_index)
    ARG		(COUNT		day_index)
    ARG		(COUNT		year_index)
    ARG_END	(BYTE		func_index)
);
PROC_GLOBAL(
void EventHandler, (selector),
    ARG_END	(BYTE	selector)
);
PROC_GLOBAL(
SIZE ClockTick, (),
    ARG_VOID
);

#endif /* _CLOCK_H */

