#include "tskintrn.h"
#include "shared.h"

PROC(
DWORD SleepTask, (wake_time),
    ARG_END	(DWORD	wake_time)
)
{
    HALT_TASKING ();

    _pRunningTask->blocker.wake_time = wake_time;
    do
    {
	_task_switch ();
    } while (_shared_desc->tick_counter < wake_time);
    _pRunningTask->blocker.wake_time = 0;

    RESTART_TASKING ();

    return (_shared_desc->tick_counter);
}


