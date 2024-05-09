#include <dos.h>
#include "tskintrn.h"

typedef struct
{
    unsigned	bp;
    unsigned	cx;
    unsigned	di;
    unsigned	si;
    unsigned	es;
    unsigned	bx;
    unsigned	ds;
    unsigned	dx;
    unsigned	ax;
    unsigned	ip;
    unsigned	cs;
    unsigned	flags;
} INT_REGS;
typedef INT_REGS	*PINT_REGS;

PROC(
void near _attach_task, (pTask),
    ARG_END	(PTASK_DESC	pTask)
)
{
    if (_pRunningTask == 0)
	_pRunningTask = pTask->pSuccTask = pTask;

    pTask->blocker.pending = 0;
    pTask->pPredTask = _pRunningTask;
    pTask->pSuccTask = _pRunningTask->pSuccTask;

    pTask->pSuccTask->pPredTask = pTask;
    pTask->pPredTask->pSuccTask = pTask;
}

PROC(
void near _detach_task, (pTask),
    ARG_END	(PTASK_DESC	pTask)
)
{
    PTASK_DESC	pPredTask, pSuccTask;

    pPredTask = pTask->pPredTask;
    pSuccTask = pTask->pSuccTask;
    if (pPredTask != pTask)
    {
	pPredTask->pSuccTask = pSuccTask;
	pSuccTask->pPredTask = pPredTask;

	return;
    }

    pTask->pSuccTask = 0;
#ifdef DEBUG
printf ("_detach_task: NO RUNNING TASKS!!!\n");
#endif /* DEBUG */
}

PROC(
void SuspendTasking, (),
    ARG_VOID
)
{
    HALT_TASKING ();
}

PROC(
void ResumeTasking, (),
    ARG_VOID
)
{
    RESTART_TASKING ();
}

static TASK	DeadTask;

PROC(
TASK AddTask, (TaskFunc, TaskStack),
    ARG		(TASK_FUNC	TaskFunc)
    ARG_END	(COUNT		TaskStack)
)
{
    TASK	Task;

    HALT_TASKING ();

    {
	TASK	DT;

	if (DT = DeadTask)
	{
	    DeadTask = 0;

	    UnlockTask (DT);		/* already locked by previous AddTask */
	    FreeTask (DT);
	}
    }

    TaskStack = (TaskStack + 1) & ~1;	/* make sure it's even */
    if (Task = AllocTask (TaskStack))
    {
	PTASK_DESC	pTask;
	PINT_REGS	pIntRegs;
	struct SREGS	segregs;

	pTask = LockTask (Task);	/* leave locked down */

	pIntRegs = (PINT_REGS)((PBYTE)&pTask[1] + TaskStack);

	segread (&segregs);
	pIntRegs->ds = pIntRegs->es = segregs.ds;
	pIntRegs->ip = GET_OFFSET (&TaskFunc);
	pIntRegs->cs = GET_SEGMENT (&TaskFunc);
	pIntRegs->flags = 0x0200;	/* enable interrupts */

	pTask->ss = pIntRegs->ds;
	pTask->sp = GET_OFFSET (&pIntRegs);

	_attach_task (pTask);
    }

    RESTART_TASKING ();

    return (Task);
}

PROC(
void DeleteTask, (Task),
    ARG_END	(TASK		Task)
)
{
    PTASK_DESC		pTask;

    HALT_TASKING ();

#ifdef DEBUG
printf ("DeleteTask(0x%04x): ", Task);
#endif /* DEBUG */
    {
	TASK	DT;

	if (DT = DeadTask)
	{
#ifdef DEBUG
printf ("(collecting garbage -- 0x%04x) ", DT);
#endif /* DEBUG */
	    DeadTask = 0;

	    UnlockTask (DT);		/* already locked by previous AddTask */
	    FreeTask (DT);
	}
    }

    if (pTask = LockTask (Task))
    {
	UnlockTask (Task);		/* already locked by AddTask */

	if (pTask->blocker.pending == 0)		/* task is ready */
	{
#ifdef DEBUG
printf ("READY");
#endif /* DEBUG */
	    _detach_task (pTask);
	    if (pTask == _pRunningTask)
	    {
#ifdef DEBUG
printf (" (running)\n");
#endif /* DEBUG */
		DeadTask = Task;		/* can't free immediately,
						 * because _pRunningTask
						 * points to next task to
						 * run via _task_switch.
						 */
		_task_switch ();
	    }
	}
	else					/* task is blocked */
	{
	    PTASK_DESC	pPredTask, pSuccTask;

#ifdef DEBUG
printf ("BLOCKED");
#endif /* DEBUG */
	    for (pPredTask = 0, pSuccTask = _pBlockedTask;
		    pSuccTask != pTask;
		    pPredTask = pSuccTask, pSuccTask = pSuccTask->pNextTask)
		;
	    if (pPredTask != 0)
		pPredTask->pNextTask = pTask->pNextTask;
	    else
		_pBlockedTask = pTask->pNextTask;

	    --*(pTask->blocker.pending);		/* decrement semaphore count */
	}
#ifdef DEBUG
printf ("\n");
#endif /* DEBUG */

	UnlockTask (Task);
	FreeTask (Task);
    }

#ifdef DEBUG
if (Task == 0)
{
    printf ("\nDEAD:\n\t0x%04x\n", DeadTask);
    printf ("\nBLOCKED:\n");
    for (pTask = _pBlockedTask; pTask != 0; pTask = pTask->pNextTask)
    {
	printf ("\tblocker.pending = 0x%04x\n\n", pTask->blocker.pending);
    }
    printf ("\nREADY:\n");
    pTask = _pRunningTask;
    do
    {
	printf ("\tswitch_count = %u\n", pTask->switch_count);
	printf ("\tblocker.pending = 0x%04x\n\n", pTask->blocker.pending);

	pTask = pTask->pSuccTask;
    } while (pTask != _pRunningTask);
}
#endif /* DEBUG */

    RESTART_TASKING ();
}

