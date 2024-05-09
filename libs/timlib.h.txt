#ifndef _TIMLIB_H
#define _TIMLIB_H

#include "memlib.h"

#ifdef __WATCOMC__
#define ONE_SECOND	128
#else /* !__WATCOMC */
#define ONE_SECOND	120
#endif /* __WATCOMC */

typedef MEM_HANDLE	TASK;

typedef BYTE		SEMAPHORE;
typedef SEMAPHORE	*PSEMAPHORE;

typedef PROC_PARAMETER(
	void (far *TASK_FUNC), (),
	    ARG_VOID
	);

PROC_GLOBAL(
TASK AddTask, (TaskFunc, TaskStack),
    ARG		(TASK_FUNC	TaskFunc)
    ARG_END	(COUNT		TaskStack)
);
PROC_GLOBAL(
void DeleteTask, (Task),
    ARG_END	(TASK		Task)
);
PROC_GLOBAL(
DWORD SetSemaphore, (pSem),
    ARG_END	(PSEMAPHORE	pSem)
);
PROC_GLOBAL(
void ClearSemaphore, (pSem),
    ARG_END	(PSEMAPHORE	pSem)
);
PROC_GLOBAL(
void SuspendTasking, (),
    ARG_VOID
);
PROC_GLOBAL(
void ResumeTasking, (),
    ARG_VOID
);
PROC_GLOBAL(
DWORD SleepTask, (wake_time),
    ARG_END	(DWORD		wake_time)
);
#define TaskSwitch()	SleepTask(0L)

PROC_GLOBAL(
void InitTimer, (),
    ARG_VOID
);
PROC_GLOBAL(
void UninitTimer, (),
    ARG_VOID
);
PROC_GLOBAL(
DWORD GetTimeCounter, (),
    ARG_VOID
);
PROC_GLOBAL(
void WaitVBlank, (),
    ARG_VOID
);

#endif /* _TIMLIB_H */

