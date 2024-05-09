#include "memintrn.h"

PROC(STATIC
MEM_BOOL near _mem_satisfy_request, (curpos, pSizeRequest, access_mode),
    ARG		(MEM_SIZE		curpos)
    ARG		(PMEM_SIZE_REQUEST	pSizeRequest)
    ARG_END	(MEM_ACCESS_MODE	access_mode)
)
{
    MEM_ATTRS	attrs;
    MEM_BOOL	retval = MEM_FAILURE;
    MEM_ERROR_FUNC("_mem_satisfy_request");

    _mem_get_attrs (&attrs);
    if ((attrs.flags & MEM_ADDRESSABLE)
	    && (!(attrs.flags & MEM_ACCESS_PRIMARY)
	    || (attrs.flags & MEM_PRIMARY)))
    {
	pSizeRequest->size = pSizeRequest->max_size;
	_mem_set_phys_pos (curpos, pSizeRequest->direction);
	_mem_set_phys_size (pSizeRequest->size);
	retval = MEM_SUCCESS;
    }
    else
    {
	MEM_LOC		mem_loc;
	MEM_LOC		phys_loc;
	MEM_SIZE	phys_size;
	MEM_TYPE	win_where;

	if ((mem_loc = _mem_get_primary (&win_where)) != MEM_NULL)
	{
	    MEM_SIZE	oldpos = _mem_get_phys_pos (MEM_FAILURE);

	    if ((phys_size = _mem_get_phys_size ()) != pSizeRequest->max_size)
	    {
		if (access_mode == MEM_WRITE_ONLY
			|| access_mode == MEM_READ_WRITE)
		    _mem_type_move (mem_loc, 0L, win_where,
			    attrs.virtual, oldpos, attrs.where, phys_size);
		_mem_type_free (mem_loc, phys_size, win_where);
	    }
	    else
	    {
		if (pSizeRequest->direction == MEM_FORWARD)
		    phys_loc = curpos;
		else
		    phys_loc = curpos - (phys_size - 1);

		if (oldpos != phys_loc)
		{
		    MEM_BOOL	reading, writing;

		    reading = (MEM_BOOL)(access_mode == MEM_READ_ONLY
			    || access_mode == MEM_READ_WRITE);
		    writing = (MEM_BOOL)(access_mode == MEM_WRITE_ONLY
			    || access_mode == MEM_READ_WRITE);
		    if ((oldpos > phys_loc
			    && oldpos - phys_loc >= phys_size)
			    || (oldpos < phys_loc
			    && phys_loc - oldpos >= phys_size))
		    {
			if (writing)
			    _mem_type_move (
				    mem_loc, 0L, win_where,
				    attrs.virtual, oldpos, attrs.where,
				    phys_size);
			if (reading)
			    _mem_type_move (
				    attrs.virtual, phys_loc, attrs.where,
				    mem_loc, 0L, win_where,
				    phys_size);
		    }
		    else if (oldpos > phys_loc)
		    {
			if (writing)
			    _mem_type_move (mem_loc,
				    phys_size - (oldpos - phys_loc), win_where,
				    attrs.virtual,
				    phys_loc + phys_size, attrs.where,
				    oldpos - phys_loc);
			_mem_type_move (mem_loc, 0L, win_where,
				mem_loc, oldpos - phys_loc, win_where,
				phys_size - (oldpos - phys_loc));
			if (reading)
			    _mem_type_move (attrs.virtual, phys_loc, attrs.where,
				    mem_loc, 0L, win_where, oldpos - phys_loc);
		    }
		    else
		    {
			if (writing)
			    _mem_type_move (mem_loc, 0L, win_where,
				    attrs.virtual, oldpos, attrs.where,
				    phys_loc - oldpos);
			_mem_type_move (mem_loc, phys_loc - oldpos, win_where,
				mem_loc, 0L, win_where,
				phys_size - (phys_loc - oldpos));
			if (reading)
			    _mem_type_move (attrs.virtual, oldpos + phys_size,
				    attrs.where, mem_loc,
				    phys_size - (phys_loc - oldpos),
				    win_where, phys_loc - oldpos);
		    }
		}

		_mem_set_phys_pos (curpos, pSizeRequest->direction);

		pSizeRequest->size = phys_size;
		_mem_set_primary (mem_loc, curpos - phys_loc, win_where);
		return (MEM_SUCCESS);
	    }
	}

	{
	    if ((mem_loc = _mem_type_alloc (pSizeRequest,
		    MEM_ADDRESSABLE | (attrs.flags
		    & (MEM_GRAPHICS | MEM_SOUND))
		    | ((attrs.flags & MEM_ACCESS_PRIMARY) ?
		    MEM_PRIMARY : DEFAULT_MEM_FLAGS),
		    attrs.priority, INVALID_MEM_TYPE, &win_where)) == MEM_NULL)
	    {
		_mem_set_phys_size ((MEM_SIZE)0);
		MEM_ERROR ((_mem_err_id, FUNC_NAME,
			(ERR_NO)MEM_ALLOC_FAILED, pSizeRequest->min_size));
	    }
	    else
	    {
		phys_size = pSizeRequest->size;
		if (pSizeRequest->direction == MEM_FORWARD)
		    phys_loc = curpos;
		else
		    phys_loc = curpos - (phys_size - 1);

		_mem_set_phys_pos (curpos, pSizeRequest->direction);
		_mem_set_phys_size (phys_size);
		if (phys_loc == 0 && phys_size == attrs.size)
		{
		    MEM_TYPE	old_where;

		    attrs.virtual = mem_loc;
		    old_where = attrs.where;
		    attrs.where = win_where;
		    _mem_hash_newloc (_mem_get_info (), &attrs, old_where);
		}
		else if (access_mode == MEM_READ_ONLY
			|| access_mode == MEM_READ_WRITE)
		    _mem_type_move (attrs.virtual,
			    phys_loc, attrs.where, mem_loc,
			    (MEM_SIZE)0, win_where, phys_size);
			    
		retval = MEM_SUCCESS;
	    }
	}
	_mem_set_primary (mem_loc, curpos - phys_loc, win_where);
    }

    return (retval);
}

PROC(STATIC
MEM_BOOL near _mem_validate_request, (pos, lpSizeRequest, access_mode),
    ARG		(MEM_SIZE		pos)
    ARG		(LPMEM_SIZE_REQUEST	lpSizeRequest)
    ARG_END	(MEM_ACCESS_MODE	access_mode)
)
{
    MEM_SIZE_REQUEST	locRequest;
    MEM_BOOL		retval = MEM_FAILURE;

    if ((locRequest.size = _mem_get_size ()) <= pos)
	MEM_ERROR ((_mem_err_id, FUNC_NAME,
		(ERR_NO)MEM_POS_GREATER_THAN_SIZE,
		pos, locRequest.size - 1));
    else
    {
	MEM_BOOL	valid_request;

	if (lpSizeRequest == NULL_PTR)
	{
	    lpSizeRequest = &locRequest;
	    locRequest.direction = MEM_FORWARD;
	    locRequest.min_size = locRequest.max_size = locRequest.size - pos;
	    valid_request = MEM_SUCCESS;
	}
	else
	{
	    valid_request = MEM_FAILURE;
	    if ((locRequest.direction =
		    lpSizeRequest->direction) != MEM_FORWARD &&
		    locRequest.direction != MEM_BACKWARD)
		MEM_ERROR ((_mem_err_id, FUNC_NAME, (ERR_NO)MEM_BAD_DIRECTION));
	    else
	    {
		if (locRequest.direction == MEM_FORWARD)
		    locRequest.size -= pos;
		else
		    locRequest.size = pos + 1;

		locRequest.min_size = lpSizeRequest->min_size;
		if ((locRequest.max_size = lpSizeRequest->max_size) == 0L)
		    locRequest.max_size = locRequest.size;

		if (locRequest.min_size > locRequest.max_size)
		    MEM_ERROR ((_mem_err_id, FUNC_NAME,
			    (ERR_NO)MEM_MIN_GREATER_THAN_MAX,
			    locRequest.min_size, locRequest.max_size));
		else if (locRequest.min_size > locRequest.size)
		    MEM_ERROR ((_mem_err_id, FUNC_NAME,
			    (ERR_NO)MEM_MIN_GREATER_THAN_SIZE,
			    locRequest.min_size, locRequest.size));
		else
		{
		    valid_request = MEM_SUCCESS;
		    if (locRequest.max_size > locRequest.size)
			locRequest.max_size = locRequest.size;
		}
	    }
	}

	if (valid_request)
	{
	    if ((retval = _mem_satisfy_request (pos,
		    &locRequest, access_mode)) == MEM_FAILURE)
		MEM_ERROR ((_mem_err_id, FUNC_NAME,
			(ERR_NO)MEM_CANT_ACCESS, locRequest.min_size));
	    else
		lpSizeRequest->size = locRequest.size;
	}
    }

    return (retval);
}

PROC(
LPVOID mem_complex_access, (handle, pos, lpSizeRequest, access_mode),
    ARG		(MEM_HANDLE		handle)
    ARG		(MEM_SIZE		pos)
    ARG		(LPMEM_SIZE_REQUEST	lpSizeRequest)
    ARG_END	(MEM_ACCESS_MODE	access_mode)
)
{
    LPVOID	lpVoid;
    MEM_ERROR_FUNC("mem_complex_access");

    SetSemaphore (&_MemorySem);

    lpVoid = NULL_PTR;
    if (_mem_set_handle (handle))
    {
	if (_mem_get_usage () != MEM_COMPLEX)
	    MEM_ERROR ((_mem_err_id,
		    FUNC_NAME, (ERR_NO)MEM_INCOMPATIBLE_HANDLE));
	else if (_mem_get_access () != MEM_NO_ACCESS)
	    MEM_ERROR ((_mem_err_id, FUNC_NAME, (ERR_NO)MEM_HANDLE_ACCESSED));
	else if (access_mode != MEM_READ_ONLY && access_mode != MEM_WRITE_ONLY
		&& access_mode != MEM_READ_WRITE)
	    MEM_ERROR ((_mem_err_id, FUNC_NAME, (ERR_NO)MEM_BAD_ACCESS_MODE));
	else
	{
	    MEM_ATTRS	attrs;

	    _mem_get_attrs (&attrs);
	    if (attrs.flags & MEM_DISCARDED)
		_mem_alloc_data (&attrs);
	    if (attrs.virtual != MEM_NULL
		    && _mem_validate_request (pos, lpSizeRequest, access_mode))
		lpVoid = _mem_set_access (access_mode);
	}
    }

    ClearSemaphore (&_MemorySem);

    return (lpVoid);
}

PROC(
MEM_BOOL mem_complex_unaccess, (handle),
    ARG_END	(MEM_HANDLE	handle)
)
{
    MEM_BOOL	Unaccessed;
    MEM_ERROR_FUNC("mem_complex_unaccess");

    SetSemaphore (&_MemorySem);

    Unaccessed = MEM_FAILURE;
    if (_mem_set_handle (handle))
    {
	if (_mem_get_usage () != MEM_COMPLEX)
	    MEM_ERROR ((_mem_err_id, FUNC_NAME, (ERR_NO)MEM_INCOMPATIBLE_HANDLE));
	else if (_mem_get_access () == MEM_NO_ACCESS)
	    MEM_ERROR ((_mem_err_id, FUNC_NAME, (ERR_NO)MEM_HANDLE_NOT_ACCESSED));
	else
	{
	    _mem_clr_access ();
	    Unaccessed = MEM_SUCCESS;
	}
    }

    ClearSemaphore (&_MemorySem);

    return (Unaccessed);
}

PROC(
LPVOID mem_complex_seek, (handle, offset, lpSizeRequest, seek_mode),
    ARG		(MEM_HANDLE		handle)
    ARG		(MEM_OFFS		offset)
    ARG		(LPMEM_SIZE_REQUEST	lpSizeRequest)
    ARG_END	(MEM_SEEK_MODE		seek_mode)
)
{
    LPVOID		lpVoid;
    MEM_ACCESS_MODE	access_mode;
    MEM_ERROR_FUNC("mem_complex_seek");

    SetSemaphore (&_MemorySem);

    lpVoid = NULL_PTR;
    if (_mem_set_handle (handle))
    {
	if (_mem_get_usage () != MEM_COMPLEX)
	    MEM_ERROR ((_mem_err_id, FUNC_NAME, (ERR_NO)MEM_INCOMPATIBLE_HANDLE));
	else if ((access_mode = _mem_get_access ()) == MEM_NO_ACCESS)
	    MEM_ERROR ((_mem_err_id, FUNC_NAME, (ERR_NO)MEM_HANDLE_NOT_ACCESSED));
	else if (seek_mode != MEM_SEEK_REL && seek_mode != MEM_SEEK_ABS)
	    MEM_ERROR ((_mem_err_id, FUNC_NAME, (ERR_NO)MEM_BAD_SEEK_MODE));
	else if (seek_mode == MEM_SEEK_ABS && offset < 0)
	    MEM_ERROR ((_mem_err_id, FUNC_NAME, (ERR_NO)MEM_BAD_ABSOLUTE_OFFSET));
	else
	{
	    MEM_SIZE	pos;
	    MEM_BOOL	valid_pos;

	    if (seek_mode == MEM_SEEK_ABS)
	    {
		valid_pos = MEM_SUCCESS;
		pos = (MEM_SIZE)offset;
	    }
	    else
	    {
		pos = _mem_get_phys_pos (MEM_SUCCESS);
		if (offset >= 0 || pos >= (MEM_SIZE)-offset)
		{
		    valid_pos = MEM_SUCCESS;
		    pos += offset;
		}
		else
		{
		    valid_pos = MEM_FAILURE;
		    MEM_ERROR ((_mem_err_id, FUNC_NAME,
			    (ERR_NO)MEM_BAD_RELATIVE_OFFSET));
		}
	    }

	    if (valid_pos &&
		    _mem_validate_request (pos, lpSizeRequest, access_mode))
		lpVoid = _mem_set_access (access_mode);
	}
    }

    ClearSemaphore (&_MemorySem);

    return (lpVoid);
}



