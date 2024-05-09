#include "tskintrn.h"

PROC(
DWORD SetSemaphore, (pSem),
    ARG_END	(PSEMAPHORE	pSem)
)
{
    HALT_TASKING ();

    while (++*pSem > 1)
    {
#ifdef DEBUG
printf ("Blocking(0x%04x:0x%04x): 0x%04x = %u\n", _pRunningTask, _pRunningTask->sp, pSem, *pSem);
#endif /* DEBUG */
	_pRunningTask->blocker.pending = pSem;
	_detach_task (_pRunningTask);

	_pRunningTask->pNextTask = _pBlockedTask;
	_pBlockedTask = _pRunningTask;

	_task_switch ();
    }

    RESTART_TASKING ();

    return (GetTimeCounter ());
}

PROC(
void ClearSemaphore, (pSem),
    ARG_END	(PSEMAPHORE	pSem)
)
{
    PTASK_DESC	pTask;

    HALT_TASKING ();

#ifdef DEBUG
if (*pSem == 0)
    printf ("ClearSemaphore: SEMAPHORE NOT SET!!!\n");
else
#endif /* DEBUG */
    if (--*pSem > 0 && (pTask = _pBlockedTask) != 0)
    {
	register PTASK_DESC	pPredTask, pSuccTask;

	pPredTask = 0;
	do
	{
	    pSuccTask = pTask->pNextTask;
	    if (pTask->blocker.pending != pSem)
		pPredTask = pTask;
	    else
	    {
		if (pPredTask)
		    pPredTask->pNextTask = pSuccTask;
		else
		    _pBlockedTask = pSuccTask;

		_attach_task (pTask);
	    }
	} while (pTask = pSuccTask);

	*pSem = 0;
    }

    RESTART_TASKING ();
}


