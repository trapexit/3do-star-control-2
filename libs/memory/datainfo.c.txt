#include "memintrn.h"

PROC(
MEM_SIZE mem_get_size, (handle),
    ARG_END	(MEM_HANDLE	handle)
)
{
    MEM_SIZE	s;

    SetSemaphore (&_MemorySem);
    s = _mem_set_handle (handle) ? _mem_get_size () : (MEM_SIZE)0;
    ClearSemaphore (&_MemorySem);

    return (s);
}

PROC(
MEM_USAGE mem_get_usage, (handle),
    ARG_END	(MEM_HANDLE	handle)
)
{
    MEM_USAGE	u;

    SetSemaphore (&_MemorySem);
    u = _mem_set_handle (handle) ? _mem_get_usage () : NO_MEM_USAGE;
    ClearSemaphore (&_MemorySem);

    return (u);
}

PROC(
MEM_BOOL mem_set_flags, (handle, mem_flags),
    ARG		(MEM_HANDLE	handle)
    ARG_END	(MEM_FLAGS	mem_flags)
)
{
    MEM_BOOL	retval;

    SetSemaphore (&_MemorySem);

    retval = MEM_FAILURE;
    if (_mem_set_handle (handle))
    {
	_mem_set_flags (_mem_get_alloc_flags (mem_flags));

	retval = MEM_SUCCESS;
    }

    ClearSemaphore (&_MemorySem);

    return (retval);
}

PROC(
MEM_FLAGS mem_get_flags, (handle),
    ARG_END	(MEM_HANDLE	handle)
)
{
    MEM_FLAGS	f;

    SetSemaphore (&_MemorySem);
    f = _mem_set_handle (handle) ? _mem_get_flags () : (MEM_FLAGS)0;
    ClearSemaphore (&_MemorySem);

    return (f);
}

PROC(
MEM_BOOL mem_set_priority, (handle, priority),
    ARG		(MEM_HANDLE	handle)
    ARG_END	(MEM_PRIORITY	priority)
)
{
    MEM_BOOL	retval;

    SetSemaphore (&_MemorySem);

    retval = MEM_FAILURE;
    if (_mem_set_handle (handle))
    {
	_mem_set_priority (_mem_get_alloc_priority (priority));

	retval = MEM_SUCCESS;
    }

    ClearSemaphore (&_MemorySem);

    return (retval);
}

PROC(
MEM_PRIORITY mem_get_priority, (handle),
    ARG_END	(MEM_HANDLE	handle)
)
{
    MEM_PRIORITY	p;

    SetSemaphore (&_MemorySem);
    p = _mem_set_handle (handle) ? _mem_get_priority () : (MEM_PRIORITY)0;
    ClearSemaphore (&_MemorySem);

    return (p);
}

PROC(
MEM_SIZE mem_get_memavailable, (size, priority),
    ARG		(MEM_SIZE	size)
    ARG_END	(MEM_PRIORITY	priority)
)
{
    MEM_SIZE	s;

    SetSemaphore (&_MemorySem);

    if (_mem_get_num_memtypes () == 0)
    {
	s = 0;
	MEM_ERROR_FATAL ();
    }
    else
    {
	s = _mem_type_get_memavailable (size,
		_mem_get_alloc_priority (priority),
		_mem_get_memtype (priority));
    }

    ClearSemaphore (&_MemorySem);

    return (s);
}

