#include "memintrn.h"

PROC(
void near _mem_alloc_data, (pmem_attrs),
    ARG_END	(PMEM_ATTRS	pmem_attrs)
)
{
    MEM_ERROR_FUNC("_mem_alloc_data");
    
    if (pmem_attrs->size == 0)
	pmem_attrs->virtual = MEM_NULL;
    else
    {
	MEM_TYPE		old_where;
	MEM_SIZE_REQUEST	alloc_size_request;

	old_where = pmem_attrs->where;
	alloc_size_request.min_size = alloc_size_request.max_size = pmem_attrs->size;
	if ((pmem_attrs->virtual = _mem_type_alloc (&alloc_size_request,
		pmem_attrs->flags, pmem_attrs->priority,
		INVALID_MEM_TYPE, &pmem_attrs->where)) == MEM_NULL)
	    MEM_ERROR ((_mem_err_id, FUNC_NAME, (ERR_NO)MEM_ALLOC_FAILED, pmem_attrs->size));
	else
    	    _mem_hash_newloc (_mem_get_info (), pmem_attrs, old_where);
    }
}

PROC(
LPVOID mem_simple_access, (handle),
    ARG_END	(MEM_HANDLE	handle)
)
{
    LPVOID	lpMem;
    MEM_ERROR_FUNC("mem_simple_access");
    
    SetSemaphore (&_MemorySem);

    lpMem = 0;
    if (_mem_set_handle (handle))
    {
	if (_mem_get_usage () != MEM_SIMPLE)
	    MEM_ERROR ((_mem_err_id,
		    FUNC_NAME, (ERR_NO)MEM_INCOMPATIBLE_HANDLE));
	else
	{
	    MEM_ATTRS	attrs;

	    _mem_get_attrs (&attrs);
	    if (attrs.flags & MEM_DISCARDED)
	    {
	     	attrs.flags |= MEM_ADDRESSABLE
			| ((attrs.flags & MEM_ACCESS_PRIMARY) ?
			MEM_PRIMARY : DEFAULT_MEM_FLAGS);
		_mem_alloc_data (&attrs);
	    }
	    else if (!(attrs.flags & MEM_ADDRESSABLE)
		    || ((attrs.flags & MEM_ACCESS_PRIMARY)
		    && !(attrs.flags & MEM_PRIMARY)))
	    {
		if ((attrs.flags & MEM_ADDRESSABLE)
			&& _mem_get_access () != MEM_NO_ACCESS)
		{
		    attrs.virtual = MEM_NULL;
		    MEM_ERROR ((_mem_err_id, FUNC_NAME, (ERR_NO)MEM_HANDLE_ACCESSED));
		}
		else
		{
		    MEM_TYPE		old_where;
		    MEM_SIZE_REQUEST	access_size_request;

		    old_where = attrs.where;
		    access_size_request.max_size =
			    access_size_request.min_size = attrs.size;
		    if ((attrs.virtual = _mem_type_alloc (&access_size_request,
			    MEM_ADDRESSABLE | (attrs.flags
			    & (MEM_GRAPHICS | MEM_SOUND))
			    | ((attrs.flags & MEM_ACCESS_PRIMARY) ?
			    MEM_PRIMARY : DEFAULT_MEM_FLAGS),
			    attrs.priority, INVALID_MEM_TYPE,
			    &attrs.where)) == MEM_NULL)
			MEM_ERROR ((_mem_err_id, FUNC_NAME,
				(ERR_NO)MEM_ALLOC_FAILED, attrs.size));
		    else
	    		_mem_hash_newloc (_mem_get_info (), &attrs, old_where);
		}
	    }

	    if (attrs.virtual)
		lpMem = _mem_set_access (MEM_READ_WRITE);
	}
    }

    ClearSemaphore (&_MemorySem);

    return (lpMem);
}

PROC(
MEM_BOOL mem_simple_unaccess, (handle),
    ARG_END	(MEM_HANDLE	handle)
)
{
    MEM_BOOL	retval;
    MEM_ERROR_FUNC("mem_simple_unaccess");

    SetSemaphore (&_MemorySem);

    retval = MEM_FAILURE;
    if (_mem_set_handle (handle))
    {
	if (_mem_get_usage () != MEM_SIMPLE)
	    MEM_ERROR ((_mem_err_id,
		    FUNC_NAME, (ERR_NO)MEM_INCOMPATIBLE_HANDLE));
	else if (_mem_get_access () == MEM_NO_ACCESS)
	    MEM_ERROR ((_mem_err_id,
		    FUNC_NAME, (ERR_NO)MEM_HANDLE_NOT_ACCESSED));
	else
	    retval = _mem_clr_access ();
    }

    ClearSemaphore (&_MemorySem);

    return (retval);
}

