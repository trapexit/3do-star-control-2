#include "memintrn.h"

/* #define DEBUG */

PMEM_HEADER	*_mem_tab;
MEM_TYPE	_mem_types;
PMEM_HEADER	_pCurHeader;

PROC(
MEM_TYPE near _mem_type_init,
	(primary_size, pmin_addressable, disk_name),
    ARG		(MEM_SIZE	primary_size)
    ARG		(PMEM_SIZE	pmin_addressable)
    ARG_END	(PSTR		disk_name)
)
{
    if ((_mem_types = _mem_imp_init (&_mem_tab, primary_size, disk_name)) > 0)
    {
	MEM_TYPE	type;
	MEM_SIZE	max_addressed;

	max_addressed = 0;
	for (type = 0; type < _mem_get_num_memtypes (); ++type)
	{
	    PMEM_HEADER	pOldHeader;

	    pOldHeader = _pCurHeader;
	    _mem_set_header (type);
	    if (!(*_pCurHeader->init_func) ())
		_pCurHeader->flags = 0;
	    else
		_pCurHeader->flags |= MEM_ARENA_CREATED;

	    if (_pCurHeader->flags & MEM_ADDRESSABLE)
		max_addressed += _pCurHeader->limit;
	    _pCurHeader = pOldHeader;
	}

	if (pmin_addressable != (PMEM_SIZE)0
		&& max_addressed < *pmin_addressable)
	{
	    _mem_types = 0;
	    _mem_tab = MEM_NULL;
	}
	else
	{
	    MEM_HANDLE	h;

			/* set up all internal tables */
	    if ((h = _mem_internal_alloc (MEMINFO_TAB)) != NULL_HANDLE)
		_mem_internal_free (h, MEMINFO_TAB);
	    if ((h = _mem_internal_alloc (MEM_SIMPLE)) != NULL_HANDLE)
		_mem_internal_free (h, MEM_SIMPLE);
	    if ((h = _mem_internal_alloc (MEM_COMPLEX)) != NULL_HANDLE)
		_mem_internal_free (h, MEM_COMPLEX);
	}

	if (pmin_addressable != (PMEM_SIZE)0)
	    *pmin_addressable = max_addressed;
    }

    return (_mem_types);
}

PROC(
void near _mem_type_uninit, (),
    ARG_VOID
)
{
    if (_mem_tab != MEM_NULL)
    {
	MEM_TYPE	type;
 	PMEM_HEADER	pOldHeader;

#ifdef HANDLE_DIAG
	_mem_dump_handle_diag ();
#endif /* HANDLE_DIAG */
	_mem_internal_free (0, MEMINFO_TAB);
	_mem_internal_free (0, MEM_SIMPLE);
	_mem_internal_free (0, MEM_COMPLEX);

	pOldHeader = _pCurHeader;
	for (type = _mem_get_last_memtype (); type >= 0; --type)
	{

	    _mem_set_header (type);
	    if (!(*_pCurHeader->uninit_func) ())
		/* ERROR */;
	}
	_pCurHeader = pOldHeader;
//printf ("_pCurHeader = 0x%08lx\n", _pCurHeader);

	_mem_types = 0;
	_mem_tab = MEM_NULL;
_pCurHeader = 0;
_mem_clr_table ();
    }
}

PROC(
MEM_LOC near _mem_type_alloc,
	(psize_request, orig_flags, priority, mem_type, pwhere),
    ARG		(PMEM_SIZE_REQUEST	psize_request)
    ARG		(MEM_FLAGS		orig_flags)
    ARG		(MEM_PRIORITY		priority)
    ARG		(MEM_TYPE		mem_type)
    ARG_END	(PMEM_TYPE		pwhere)
)
{
    MEM_LOC	mem_loc;
    MEM_SIZE	size;
    MEM_FLAGS	flags;
    MEM_TYPE	min_type, max_type, type;
    PMEM_HEADER	pOldHeader;
    MEM_ERROR_FUNC("_mem_type_alloc");

    pOldHeader = _pCurHeader;

    mem_loc = MEM_NULL;
    if (mem_type >= 0 && mem_type < _mem_get_num_memtypes ())
	min_type = max_type = mem_type;
    else if (orig_flags & MEM_CONSTRAINED)
	min_type = max_type = _mem_get_memtype (priority);
    else
    {
	min_type = 0;
	max_type = _mem_get_last_memtype ();
    }
#ifdef DEBUG
printf ("_mem_type_alloc: min_type = %d, max_type = %d\n", min_type, max_type);
#endif /* DEBUG */

    flags = (orig_flags
	    & (MEM_PRIMARY | MEM_GRAPHICS | MEM_SOUND | MEM_ADDRESSABLE))
	    | MEM_ARENA_CREATED;
    if ((flags & MEM_ADDRESSABLE) && !(flags & (MEM_GRAPHICS | MEM_SOUND)))
	flags |= MEM_NORMAL;
    size = psize_request->max_size;
    for (type = max_type; type >= min_type; --type)
    {
	_mem_set_header (type);
	if ((_pCurHeader->flags & flags) == flags && (mem_loc =
		(*_pCurHeader->alloc_func) (size, orig_flags)) != MEM_NULL)
	    break;
    }

    if (mem_loc == MEM_NULL)
    {
	type = max_type;
	do
	{
	    _mem_set_header (type);
	    if ((_pCurHeader->flags & flags) == flags)
	    {
		_mem_type_get_memavailable (psize_request->max_size,
			priority, type);
		for (size = psize_request->max_size;
			size >= psize_request->min_size
			&& (mem_loc =
			(*_pCurHeader->alloc_func) (size, orig_flags)) == MEM_NULL;
			size = ((size + (psize_request->min_size - 1)) >> 1))
		    ;
	    }
	} while (mem_loc == MEM_NULL && type-- > min_type);
    }

    if (mem_loc == MEM_NULL)
    {
	size = (MEM_SIZE)0;
	MEM_ERROR ((_mem_err_id, FUNC_NAME, (ERR_NO)MEM_NOT_ENOUGH_MEMORY,
		psize_request->min_size, priority, min_type, max_type));
    }
    else if (pwhere != MEM_NULL)
	*pwhere = type;
    psize_request->size = size;
#ifdef DEBUG
printf ("_mem_type_alloc: size = %lu, type = %d, mem_loc = 0x%lx\n", size, type, mem_loc);
#endif /* DEBUG */

    _pCurHeader = pOldHeader;
    return (mem_loc);
}

PROC(
void near _mem_type_free, (mem_loc, size, type),
    ARG		(MEM_LOC	mem_loc)
    ARG		(MEM_SIZE	size)
    ARG_END	(MEM_TYPE	type)
)
{
    PMEM_HEADER	pOldHeader;

    pOldHeader = _pCurHeader;
    _mem_set_header (type);
    if (!(*_pCurHeader->free_func) (mem_loc, size))
	/* ERROR */;
    _pCurHeader = pOldHeader;
}

PROC(
MEM_SIZE near _mem_type_compact, (size, type),
    ARG		(MEM_SIZE	size)
    ARG_END	(MEM_TYPE	type)
)
{
    MEM_SIZE	retval;
    PMEM_HEADER	pOldHeader;

    pOldHeader = _pCurHeader;
    _mem_set_header (type);
    if (_pCurHeader->compact_func)
	retval = (*_pCurHeader->compact_func) (size);
    else
	retval = _mem_get_biggest_block (0L);
    _pCurHeader = pOldHeader;

    return (retval);
}

PROC(
MEM_BOOL near _mem_type_move,
	(src_loc, src_offs, src_type, dst_loc, dst_offs, dst_type, size),
    ARG		(MEM_LOC	src_loc)
    ARG		(MEM_SIZE	src_offs)
    ARG		(MEM_TYPE	src_type)
    ARG		(MEM_LOC	dst_loc)
    ARG		(MEM_SIZE	dst_offs)
    ARG		(MEM_TYPE	dst_type)
    ARG_END	(MEM_SIZE	size)
)
{
    LPSTR	lpSrc, lpDst;
    MEM_BOOL	ret_val;
    PMEM_HEADER	pOldHeader;

    pOldHeader = _pCurHeader;

    _mem_set_header (src_type);
    if (src_type == dst_type)
	ret_val = (*_pCurHeader->move_func) (src_loc, src_offs,
		dst_loc, dst_offs, size);
    else if (_pCurHeader->flags & MEM_ADDRESSABLE)
    {
	_mem_ram_lock (src_loc, &lpSrc);

	_mem_set_header (dst_type);
	ret_val = (*_pCurHeader->store_func) (lpSrc + src_offs,
		dst_loc, dst_offs, size);

	_mem_ram_unlock (src_loc);
    }
    else
    {
	_mem_set_header (dst_type);
	_mem_ram_lock (dst_loc, &lpDst);

	_mem_set_header (src_type);
	ret_val = (*_pCurHeader->restore_func) (src_loc, src_offs,
		lpDst + dst_offs, size);

	_mem_ram_unlock (dst_loc);
    }

    _pCurHeader = pOldHeader;
    return (ret_val);
}

PROC(
MEM_BOOL near _mem_type_swap, (desired_size, priority, type),
    ARG		(MEM_SIZE	desired_size)
    ARG		(MEM_PRIORITY	priority)
    ARG_END	(MEM_TYPE	type)
)
{
    MEM_TYPE	dst_type;
    MEM_BOOL	retval;
    PMEM_HEADER	pOldHeader;

    pOldHeader = _pCurHeader;

    retval = MEM_FAILURE;
    for (dst_type = type + (MEM_TYPE)1;
	    dst_type < _mem_get_num_memtypes (); ++dst_type)
    {
	_mem_set_header (dst_type);
	if ((_pCurHeader->flags & MEM_ARENA_CREATED)
		&& _pCurHeader->swap_func != MEM_NULL
		&& (*_pCurHeader->swap_func) (&desired_size,
		priority, type, dst_type))
	{
	    retval = MEM_SUCCESS;
	    break;
	}
    }

    _pCurHeader = pOldHeader;
    return (MEM_FAILURE);
}

PROC(
MEM_SIZE near _mem_type_get_memavailable, (size, priority, type),
    ARG		(MEM_SIZE	size)
    ARG		(MEM_PRIORITY	priority)
    ARG_END	(MEM_TYPE	type)
)
{
    MEM_SIZE	biggest_block;
    PMEM_HEADER	pOldHeader;

    if (type >= _mem_get_num_memtypes ())
	return ((MEM_SIZE)0);

    pOldHeader = _pCurHeader;
    _mem_set_header (type);
    biggest_block = _mem_get_biggest_block (size);
    if (size > 0 && biggest_block < size)
    {
	_mem_type_compact (size, type);
	if ((biggest_block = _mem_get_biggest_block (size)) < size)
	{
	    while (_mem_type_swap (size - biggest_block, priority, type)
		    && (biggest_block = _mem_get_biggest_block (size)) < size)
		;
	    biggest_block = _mem_get_biggest_block (0L);
	}
    }

    _pCurHeader = pOldHeader;
    return (biggest_block);
}

