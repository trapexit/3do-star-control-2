#include "memintrn.h"

PROC(
MEM_FLAGS near _mem_get_alloc_flags, (flags),
    ARG_END	(MEM_FLAGS	flags)
)
{
    flags &= MEM_ALLOC_FLAGS;

    if (flags & (MEM_GRAPHICS | MEM_SOUND))
	flags &= ~MEM_PRIMARY;
    else if (flags & MEM_PRIMARY)
    {
	flags &= ~MEM_PRIMARY;
	flags |= MEM_ACCESS_PRIMARY;
    }

    if (flags & MEM_NODISCARD)
	flags &= ~MEM_DISCARDABLE;
    else if (!(flags & MEM_DISCARDABLE))
	flags |= MEM_NODISCARD;

    return (flags);
}

PROC(
MEM_PRIORITY near _mem_get_alloc_priority, (priority),
    ARG_END	(MEM_PRIORITY	priority)
)
{
    if ((priority == DEFAULT_MEM_PRIORITY && (priority = (MEM_PRIORITY)
	    ((LOWEST_MEM_PRIORITY - HIGHEST_MEM_PRIORITY + (MEM_PRIORITY)1) /
	    ((MEM_PRIORITY)_mem_get_num_memtypes ()) +
	    HIGHEST_MEM_PRIORITY - (MEM_PRIORITY)1)) < HIGHEST_MEM_PRIORITY)
	    || priority < HIGHEST_MEM_PRIORITY)
	priority = HIGHEST_MEM_PRIORITY;
    else if (priority > LOWEST_MEM_PRIORITY)
	priority = LOWEST_MEM_PRIORITY;

    return (priority);
}

PROC(
MEM_USAGE near _mem_get_alloc_usage, (usage),
    ARG_END	(MEM_USAGE	usage)
)
{
    if (usage != MEM_COMPLEX)
	usage = MEM_SIMPLE;
	
    return (usage);
}

PROC(STATIC
MEM_HANDLE near _mem_alloc_handle, (pmem_attrs, mem_usage),
    ARG		(PMEM_ATTRS	pmem_attrs)
    ARG_END	(MEM_USAGE	mem_usage)
)
{
    MEM_HANDLE		handle = NULL_HANDLE;
    MEM_ERROR_FUNC("_mem_alloc_handle");

    if ((handle = _mem_internal_alloc (mem_usage)) == NULL_HANDLE)
	MEM_ERROR ((_mem_err_id, FUNC_NAME, (ERR_NO)MEM_CANT_CREATE_HANDLE));
    else
    {
	MEM_HANDLE	mem_info;

	if (!(pmem_attrs->flags & MEM_DUP) && (mem_info =
		_mem_internal_alloc (MEMINFO_TAB)) == NULL_HANDLE)
	{
	    _mem_internal_free (handle, mem_usage);
	    handle = NULL_HANDLE;

	    MEM_ERROR ((_mem_err_id,
		    FUNC_NAME, (ERR_NO)MEM_CANT_CREATE_MEMINFO));
	}
	else
	{
	    _mem_set_handle (handle);

	    _mem_set_usage (mem_usage);
	    _mem_set_info (pmem_attrs->flags & MEM_DUP ?
		    (MEM_HANDLE)pmem_attrs->virtual : mem_info);
	    _mem_set_ref ();

	    if (!(pmem_attrs->flags & MEM_DUP))
	    {
		pmem_attrs->where = DEFAULT_MEM_PRIORITY;
		_mem_init_info (mem_info, pmem_attrs);
		_mem_hash_info (mem_info, pmem_attrs->priority, DEFAULT_MEM_PRIORITY);
	    }
	}
    }

    return (handle);
}

PROC(
MEM_HANDLE PASCAL mem_allocate, (size, flags, priority, usage),
    ARG		(MEM_SIZE	size)
    ARG		(MEM_FLAGS	flags)
    ARG		(MEM_PRIORITY	priority)
    ARG_END	(MEM_USAGE	usage)
)
{
    MEM_HANDLE	h;
    MEM_ERROR_FUNC("mem_allocate");
    
    SetSemaphore (&_MemorySem);

    if (_mem_get_num_memtypes () == 0)
    {
	h = 0;
	MEM_ERROR_FATAL ();
    }
    else
    {
	MEM_ATTRS	attrs;

	attrs.virtual = MEM_NULL;
	attrs.size = size;
	attrs.flags = _mem_get_alloc_flags (flags);
	attrs.priority = _mem_get_alloc_priority (priority);

	h = _mem_alloc_handle (&attrs, _mem_get_alloc_usage (usage));
    }

    ClearSemaphore (&_MemorySem);

    return (h);
}

PROC(
MEM_BOOL mem_release, (handle),
    ARG_END	(MEM_HANDLE	handle)
)
{
    MEM_ERROR_FUNC("mem_release");

    SetSemaphore (&_MemorySem);

    if (_mem_set_handle (handle))
    {
	if (_mem_get_access () != MEM_NO_ACCESS)
	    MEM_ERROR ((_mem_err_id, FUNC_NAME, (ERR_NO)MEM_HANDLE_ACCESSED));
	else
	{
	    MEM_USAGE	usage;

	    usage = _mem_get_usage ();
	    if (_mem_clr_ref ())
	    {
		MEM_HANDLE	mem_info;
		MEM_ATTRS	attrs;
	
		_mem_get_attrs (&attrs);
		if (!(attrs.flags & (MEM_STATIC | MEM_DISCARDED))
			&& attrs.virtual != MEM_NULL)
		    _mem_type_free (attrs.virtual, attrs.size, attrs.where);

		mem_info = _mem_get_info ();
		_mem_unhash_info (mem_info, attrs.priority, attrs.where);

		_mem_internal_free (mem_info, MEMINFO_TAB);
	    }

	    _mem_internal_free (handle, usage);

	    ClearSemaphore (&_MemorySem);

	    return (MEM_SUCCESS);
	}
    }

    ClearSemaphore (&_MemorySem);

    return (MEM_FAILURE);
}

PROC(
MEM_HANDLE mem_reallocate, (handle, size, flags, priority, usage),
    ARG		(MEM_HANDLE	handle)
    ARG		(MEM_SIZE	size)
    ARG		(MEM_FLAGS	flags)
    ARG		(MEM_PRIORITY	priority)
    ARG_END	(MEM_USAGE	usage)
)
{
    MEM_HANDLE	hNew = NULL_HANDLE;
#ifdef NOTYET
    MEM_ERROR_FUNC("mem_reallocate");

printf ("Acck!!!\n");
exit (1);
    SetSemaphore (&_MemorySem);

    if (!_mem_set_handle (handle))
	;
    else if (_mem_get_access () != MEM_NO_ACCESS)
	MEM_ERROR ((_mem_err_id, FUNC_NAME, (ERR_NO)MEM_HANDLE_ACCESSED));
    else
    {
	MEM_SIZE	old_size;
	MEM_FLAGS	old_flags;

	old_size = _mem_get_size ();
	old_flags = _mem_get_flags ();
	if (flags == SAME_MEM_FLAGS)
	{
	    if ((flags = old_flags) & MEM_ACCESS_PRIMARY)
		flags |= MEM_PRIMARY;
	    else
		flags &= ~MEM_PRIMARY;
	}
	if (priority == SAME_MEM_PRIORITY)
	    priority = _mem_get_priority ();
	if (usage == SAME_MEM_USAGE)
	    usage = _mem_get_usage ();

	if ((hNew = mem_allocate (size, flags, priority, usage)) != NULL_HANDLE &&
		!(old_flags & MEM_DISCARDED))
	{
	    MEM_BOOL		status = MEM_FAILURE;
	    MEM_HANDLE		hWin = NULL_HANDLE, hNewWin = NULL_HANDLE;
	    LPSTR		src = MEM_NULL, dst = MEM_NULL;
	    MEM_SIZE_REQUEST	src_request, dst_request;
	    MEM_SIZE		src_size;

	    if ((hWin = mem_dup (handle, MEM_COMPLEX)) == NULL_HANDLE ||
		    (hNewWin = mem_dup (hNew, MEM_COMPLEX)) == NULL_HANDLE)
		goto CleanUp;

	    _mem_set_handle (handle);
	    _mem_clr_flags (MEM_ACCESS_PRIMARY);

	    src_size = old_size;
	    size = (size < src_size ? size : src_size);
	    src_request.min_size = dst_request.min_size = 1;
	    src_request.max_size = dst_request.max_size = size;
	    src_request.direction = dst_request.direction = MEM_FORWARD;
	    if ((src = (LPSTR)mem_complex_access (hWin,
		    0L, &src_request, MEM_READ_ONLY)) == MEM_NULL ||
		    (dst = (LPSTR)mem_complex_access (hNewWin,
		    0L, &dst_request, MEM_WRITE_ONLY)) == MEM_NULL)
		goto CleanUp;
	    if (src_request.size >= dst_request.size)
		size = dst_request.size;
	    else
		size = src_request.size;
	    do
	    {
		_mem_ram_copy (src, dst, size);
		    /* end the loop here so that a seek off the end of
		     * the data is not performed, generating an error
		     * message.
		     */
		if ((dst_request.max_size -= size) == 0)
		    break;
		src_request.max_size -= size;
		src = (LPSTR)mem_complex_seek (hWin,
			size, &src_request, MEM_SEEK_REL);
		if (src == MEM_NULL)
		{
		    mem_complex_unaccess (hWin);
		    goto CleanUp;
		}
		dst = (LPSTR)mem_complex_seek (hNewWin,
			size, &dst_request, MEM_SEEK_REL);
		if (dst == MEM_NULL)
		{
		    mem_complex_unaccess (hNewWin);
		    goto CleanUp;
		}
	    } while (src != MEM_NULL && dst != MEM_NULL);
	    status = MEM_SUCCESS;
CleanUp:
	    if (src != MEM_NULL)
		mem_complex_unaccess (hWin);
	    if (dst != MEM_NULL)
		mem_complex_unaccess (hNewWin);
	    if (hWin != NULL_HANDLE)
		mem_release (hWin);
	    if (hNewWin != NULL_HANDLE)
		mem_release (hNewWin);

	    if (status == MEM_FAILURE)
	    {
		mem_release (hNew);
		hNew = NULL_HANDLE;
	    }
	}

	if (hNew != NULL_HANDLE)
	{
	    mem_release (handle);
	    if (flags & MEM_PRIMARY)
	    {
		_mem_set_handle (hNew);
		_mem_set_flags (MEM_ACCESS_PRIMARY);
	    }
	}
    }

    ClearSemaphore (&_MemorySem);
#endif /* NOTYET */

    return (hNew);
}

PROC(
MEM_HANDLE mem_static, (lpVoid, size, usage),
    ARG		(LPVOID		lpVoid)
    ARG		(MEM_SIZE	size)
    ARG_END	(MEM_USAGE	usage)
)
{
    MEM_HANDLE	h;
    MEM_ERROR_FUNC("mem_static");

    SetSemaphore (&_MemorySem);

    if (_mem_get_num_memtypes () == 0)
    {
	h = 0;
	MEM_ERROR_FATAL ();
    }
    else
    {
	MEM_ATTRS	attrs;

	attrs.virtual = (MEM_LOC)lpVoid;
	attrs.size = size;
	attrs.flags = MEM_STATIC;
	attrs.priority = DEFAULT_MEM_PRIORITY;

	h = _mem_alloc_handle (&attrs, usage);
    }

    ClearSemaphore (&_MemorySem);

    return (h);
}

PROC(
MEM_HANDLE mem_dup, (handle, usage),
    ARG		(MEM_HANDLE	handle)
    ARG_END	(MEM_USAGE	usage)
)
{
    MEM_ERROR_FUNC("mem_dup");

    SetSemaphore (&_MemorySem);

    if (!_mem_set_handle (handle))
	handle = 0;
    else
    {
	MEM_LOC		mem_info;
	MEM_ATTRS	attrs;
	
	mem_info = _mem_get_info ();
	if (usage == SAME_MEM_USAGE)
	    usage = _mem_get_usage ();

	attrs.virtual = (MEM_LOC)mem_info;
	attrs.size = 0L;
	attrs.flags = MEM_DUP;
	attrs.priority = DEFAULT_MEM_PRIORITY;

	handle = _mem_alloc_handle (&attrs, usage);
    }

    ClearSemaphore (&_MemorySem);

    return (handle);
}

