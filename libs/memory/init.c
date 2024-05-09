#include "memintrn.h"

SEMAPHORE	_MemorySem;

PROC(
MEM_BOOL mem_init, (core_size, pmin_addressable, disk_name),
    ARG		(MEM_SIZE	core_size)
    ARG		(PMEM_SIZE	pmin_addressable)
    ARG_END	(PSTR		disk_name)
)
{
    MEM_ERROR_FUNC("mem_init");

    SetSemaphore (&_MemorySem);

    if (_mem_get_num_memtypes () > 0)
	MEM_ERROR ((_mem_err_id, FUNC_NAME, (ERR_NO)MEM_ALREADY_INITIALIZED));
    else if (_mem_type_init (core_size,
	    pmin_addressable, disk_name) > 0 &&
	    MEM_ERROR_INIT ())
    {
	ClearSemaphore (&_MemorySem);

	_init_shared ();
	return (MEM_SUCCESS);
    }

#ifdef NEVER
    if (pmin_addressable != (PMEM_SIZE)0)
	*pmin_addressable = 0;
#endif /* NEVER */

    ClearSemaphore (&_MemorySem);

    return (MEM_FAILURE);
}

PROC(
MEM_BOOL mem_uninit, (),
    ARG_VOID
)
{
    MEM_ERROR_FUNC("mem_uninit");

    if (_mem_get_num_memtypes () == 0)
	MEM_ERROR_FATAL ();
    else
    {
	_uninit_shared ();
	MEM_ERROR_UNINIT ();

	SetSemaphore (&_MemorySem);
	_mem_type_uninit ();
	ClearSemaphore (&_MemorySem);

	return (MEM_SUCCESS);
    }

    return (MEM_FAILURE);
}

