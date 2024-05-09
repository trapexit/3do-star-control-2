#ifndef _TSKINTRN_H
#define _TSKINTRN_H

#ifdef DEBUG
#include <stdio.h>
#endif /* DEBUG */
#include "timlib.h"

#define TASK_PRIORITY		DEFAULT_MEM_PRIORITY
#define AllocTask(s)	\
	(TASK)mem_allocate ( \
	(MEM_SIZE)(sizeof (TASK_DESC) + (s) \
	+ (sizeof (INT_REGS) + sizeof (unsigned)) * 2), \
	MEM_ZEROINIT | MEM_PRIMARY, TASK_PRIORITY, MEM_SIMPLE)
#define LockTask(hTask)		(PTASK_DESC)mem_lock (hTask)
#define UnlockTask(hTask)	mem_unlock (hTask)
#define FreeTask(hTask)		mem_release (hTask)

PROC_GLOBAL(
void INTERRUPT CDECL far _task_switch, (),
    ARG_VOID
);

#define LOST_CONTROL	(1 << 0)
#define IS_MAIN_APP	(1 << 7)

typedef struct task
{
    BYTE		switch_count, flags;
    unsigned		sp, ss;
    struct task		*pPredTask, *pSuccTask;
    union
    {
	DWORD		wake_time;
	PSEMAPHORE	pending;
    } blocker;
} TASK_DESC;
typedef TASK_DESC	*PTASK_DESC;

#define pNextTask	pPredTask

extern BYTE		VOLATILE _task_semaphore;
extern PTASK_DESC	VOLATILE _pRunningTask, _pBlockedTask;

#define HALT_TASKING()		\
	do \
	{ \
	    ++_pRunningTask->switch_count; \
	} while (0)
#define RESTART_TASKING()	\
	do \
	{ \
	    if (_pRunningTask->switch_count) \
		--_pRunningTask->switch_count; \
	} while (0)

PROC_GLOBAL(
void near _attach_task, (pTask),
    ARG_END	(PTASK_DESC	pTask)
);
PROC_GLOBAL(
void near _detach_task, (pTask),
    ARG_END	(PTASK_DESC	pTask)
);

#endif /* _TSKINTRN_H */

