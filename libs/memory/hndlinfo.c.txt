#include "memintrn.h"
#ifdef HANDLE_DIAG
#include <stdio.h>
#endif /* HANDLE_DIAG */

#define MIN_HANDLE	0

#define USAGE_SHIFT	14
#define LIST_SHIFT	10
#define COMPOSE_HANDLE(list,index,usage)		\
	(MEM_HANDLE)((((usage)+1)<<USAGE_SHIFT) \
		| ((list)<<LIST_SHIFT) \
		| (index))
#define DECOMPOSE_HANDLE(h,plist,pindex,pusage)	\
	*(pindex) = (COUNT)((h)&((1<<LIST_SHIFT)-1)), \
	*(plist) = (MEM_COUNT)(((UWORD)(h)>>LIST_SHIFT)&0x0F), \
	*(pusage) = (MEM_USAGE)(((UWORD)(h)>>USAGE_SHIFT)-1)

#define _mem_get_next_free(lp)		*((LPMEM_HANDLE)(lp))
#define _mem_set_next_free(lp,val)	*((LPMEM_HANDLE)(lp)) = (val)

typedef struct table_desc
{
    MEM_TYPE	mem_type;
    MEM_COUNT	element_size;
    MEM_COUNT	num_lists, cur_list;

    COUNT	num_elements;
    MEM_HANDLE	free_list;
    MEM_LOC	list_array[1 << 4];

    LPVOID	lpCurList;
} TABLE_DESC;
typedef TABLE_DESC	*PTABLE_DESC;

static TABLE_DESC	_mem_internal_table[] =
{
    {
	MEMINFO_MEM_TYPE,
	sizeof (MEM_INFO),
	0, (MEM_COUNT)~0,
	NUM_INFO_ELEMENTS,
	NULL_HANDLE,
    },
    {
	HANDLE_MEM_TYPE,
	sizeof (SIMPLE_HANDLE),
	0, (MEM_COUNT)~0,
	NUM_SIMPLE_ELEMENTS,
	NULL_HANDLE,
    },
    {
	HANDLE_MEM_TYPE,
	sizeof (COMPLEX_HANDLE),
	0, (MEM_COUNT)~0,
	NUM_COMPLEX_ELEMENTS,
	NULL_HANDLE,
    },
};

LPVOID	_lpCMI, _lpCH;

#ifdef HANDLE_DIAG
COUNT	far _max_simple_count, far _simple_count, far _tot_simple_count;
COUNT	far _max_complex_count, far _complex_count, far _tot_complex_count;
#endif /* HANDLE_DIAG */

PROC(
MEM_HANDLE near _mem_internal_alloc, (usage),
    ARG_END	(MEM_USAGE	usage)
)
{
    MEM_HANDLE	h;
    LPSTR	lpVoid;
    MEM_LOC	mem_loc;
    PTABLE_DESC	pTab;
    PMEM_HEADER	pOldHeader;

    pTab = &_mem_internal_table[usage];
    pOldHeader = _pCurHeader;

    if ((h = pTab->free_list) == NULL_HANDLE
	    && pTab->num_lists < sizeof (pTab->list_array)
	    / sizeof (pTab->list_array[0]) - 1)
    {
	MEM_SIZE_REQUEST	size_request;

	size_request.min_size = size_request.max_size =
		(MEM_SIZE)((UWORD)pTab->element_size
		* (UWORD)pTab->num_elements);
	mem_loc = _mem_type_alloc (&size_request, MEM_ADDRESSABLE,
		DEFAULT_MEM_PRIORITY, pTab->mem_type, &pTab->mem_type);
	if (mem_loc != MEM_NULL)
	{
	    COUNT	i;

	    _mem_set_header (pTab->mem_type);
	    h = COMPOSE_HANDLE (pTab->num_lists, MIN_HANDLE, usage);

	    pTab->list_array[pTab->num_lists++] = mem_loc;
	    _mem_ram_lock (mem_loc, &lpVoid);
	    for (i = 1;
		    i < pTab->num_elements;
		    ++i, lpVoid += pTab->element_size)
		_mem_set_next_free (lpVoid, h + i);
	    _mem_set_next_free (lpVoid, NULL_HANDLE);
	    _mem_ram_unlock (mem_loc);
	}
    }

    if (h != NULL_HANDLE)
    {
	LPVOID	lpVoid;

	lpVoid = _mem_set_entry (h);
	_mem_set_header (pTab->mem_type);
	pTab->free_list = _mem_get_next_free (lpVoid);
	_mem_ram_zero ((LPSTR)lpVoid, (MEM_SIZE)pTab->element_size);

#ifdef HANDLE_DIAG
	if (usage == MEM_SIMPLE)
	{
	    ++_tot_simple_count;
	    if (++_simple_count > _max_simple_count)
		_max_simple_count = _simple_count;
	}
	else if (usage == MEM_COMPLEX)
	{
	    ++_tot_complex_count;
	    if (++_complex_count > _max_complex_count)
		_max_complex_count = _complex_count;
	}
#endif /* HANDLE_DIAG */
    }

    _pCurHeader = pOldHeader;

    return (h);
}

PROC(
MEM_BOOL near _mem_internal_free, (h, usage),
    ARG		(MEM_HANDLE	h)
    ARG_END	(MEM_USAGE	usage)
)
{
    PTABLE_DESC	pTab;

    pTab = &_mem_internal_table[usage];
    if (h == 0)
    {
	if (pTab->cur_list < pTab->num_lists)
	{
	    MEM_LOC	mem_loc;

	    mem_loc = pTab->list_array[pTab->cur_list];
	    _mem_ram_unlock (mem_loc);
	}

	while (pTab->num_lists--)
	    _mem_type_free (pTab->list_array[pTab->num_lists],
		    (MEM_SIZE)(pTab->element_size * pTab->num_elements),
		    pTab->mem_type);
    }
    else
    {
#ifdef HANDLE_DIAG
	if (usage == MEM_SIMPLE)
	    --_simple_count;
	else if (usage == MEM_COMPLEX)
	    --_complex_count;
	else if (usage)
	    printf ("_mem_internal_free: BAD USAGE %u\n", usage);
#endif /* HANDLE_DIAG */

	_mem_set_next_free (_mem_set_entry (h), pTab->free_list);
	pTab->free_list = h;
    }

    return (MEM_SUCCESS);
}

PROC(
void mem_clr_table, (),
    ARG_VOID
)
{
int	i;
TABLE_DESC	_internal_table[3] =
{
    {
	MEMINFO_MEM_TYPE,
	sizeof (MEM_INFO),
	0, (MEM_COUNT)~0,
	NUM_INFO_ELEMENTS,
	NULL_HANDLE,
    },
    {
	HANDLE_MEM_TYPE,
	sizeof (SIMPLE_HANDLE),
	0, (MEM_COUNT)~0,
	NUM_SIMPLE_ELEMENTS,
	NULL_HANDLE,
    },
    {
	HANDLE_MEM_TYPE,
	sizeof (COMPLEX_HANDLE),
	0, (MEM_COUNT)~0,
	NUM_COMPLEX_ELEMENTS,
	NULL_HANDLE,
    },
};
for (i = 0; i < 3; i++)
    _mem_internal_table[i] = _internal_table[i];
}

#ifdef HANDLE_DIAG
PROC(
void near _mem_dump_handle_diag, (),
    ARG_VOID
)
{
    MEM_TYPE	type;
    PMEM_HEADER	pOldHeader;

    pOldHeader = _pCurHeader;

//{
//extern BYTE	_task_semaphore;
//printf ("\n_task_semaphore = %u\n", _task_semaphore);
//}
//    printf ("\nProgram size = %uK\n", mem_prog_size ());
    printf ("\n     Type    Total    Max use    Still active\n");
    printf ("     ----    -----    -------    ------------\n");
    printf ("   Simple    %5u      %3u           %2u\n",
	    _tot_simple_count, _max_simple_count, _simple_count);
    printf ("   Complex   %5u      %3u           %2u\n\n",
	    _tot_complex_count, _max_complex_count, _complex_count);

    for (type = 0; type < _mem_get_num_memtypes (); ++type)
    {
	_mem_set_header (type);
	printf ("\tType %d memory usage: %6lu out of %6lu (%lu unused)\n",
		type, _pCurHeader->max_core, _pCurHeader->limit,
		_pCurHeader->limit == 0L ? 0L :
		(DWORD)(_pCurHeader->limit - _pCurHeader->max_core));
    }

    _pCurHeader = pOldHeader;
}
#endif /* HANDLE_DIAG */

PROC(
MEM_BOOL near _mem_set_handle, (h),
    ARG_END	(MEM_HANDLE	h)
)
{
    MEM_BOOL	retval;

    retval = MEM_FAILURE;
    if (_mem_get_num_memtypes () == 0)
	MEM_ERROR_FATAL ();
    else if (h == NULL_HANDLE)
	MEM_ERROR ((_mem_err_id, FUNC_NAME, (ERR_NO)MEM_NULL_HANDLE));
    else if (_lpCH = _mem_set_entry (h))
    {
	if ((h = _mem_get_info ()) != NULL_HANDLE)
	    _lpCMI = _mem_set_entry (h);
	retval = MEM_SUCCESS;
    }

    return (retval);
}

PROC(
LPVOID near _mem_set_entry, (h),
    ARG_END	(MEM_HANDLE	h)
)
{
    MEM_USAGE	usage;
    MEM_COUNT	which_list;
    COUNT	which_entry;
    PTABLE_DESC	pTab;
    LPBYTE	lpVoid;
    PMEM_HEADER	pOldHeader;

    DECOMPOSE_HANDLE (h, &which_list, &which_entry, &usage);

    pTab = &_mem_internal_table[usage];
    pOldHeader = _pCurHeader;

    _mem_set_header (pTab->mem_type);
    if (which_list == pTab->cur_list)
	lpVoid = (LPBYTE)pTab->lpCurList;
    else if (which_list < pTab->num_lists)
    {
	MEM_LOC	mem_loc;

	if (pTab->cur_list < pTab->num_lists)
	{
	    mem_loc = pTab->list_array[pTab->cur_list];
	    _mem_ram_unlock (mem_loc);
	}

	mem_loc = pTab->list_array[pTab->cur_list = which_list];
	_mem_ram_lock (mem_loc, &lpVoid);
	pTab->lpCurList = (LPVOID)lpVoid;
    }
    else
    {
	lpVoid = 0;
	which_entry = 0;
    }

    _pCurHeader = pOldHeader;
    return ((LPVOID)(lpVoid + which_entry * pTab->element_size));
}

PROC(
MEM_BOOL near _mem_set_priority, (priority),
    ARG_END	(MEM_PRIORITY	priority)
)
{
    MEM_ATTRS	attrs;

    _mem_get_attrs (&attrs);
    if (priority != attrs.priority)
    {
	MEM_HANDLE	mem_info;

	mem_info = _mem_get_info ();
	_mem_unhash_info (mem_info, attrs.priority, attrs.where);
	attrs.priority = priority;
	_mem_init_info (mem_info, &attrs);
	_mem_hash_info (mem_info, attrs.priority, attrs.where);
    }

    return (MEM_SUCCESS);
}

PROC(
MEM_BOOL near _mem_set_primary, (mem_loc, mem_offs, mem_type),
    ARG		(MEM_LOC	mem_loc)
    ARG		(MEM_SIZE	mem_offs)
    ARG_END	(MEM_TYPE	mem_type)
)
{
    lpComplexHandle->window.mem_type = mem_type;
    if ((lpComplexHandle->window.primary = mem_loc) == MEM_NULL)
	lpComplexHandle->physical = MEM_NULL;
    else
    {
	_mem_ram_lock (mem_loc, &lpComplexHandle->physical);
	lpComplexHandle->physical =
		(LPVOID)((LPSTR)lpComplexHandle->physical + mem_offs);
    }

    return (MEM_SUCCESS);
}

PROC(
MEM_LOC near _mem_get_primary, (mem_type),
    ARG_END	(PMEM_TYPE	mem_type)
)
{
    MEM_LOC	mem_loc;

    *mem_type = lpComplexHandle->window.mem_type;
    if ((mem_loc = lpComplexHandle->window.primary) != MEM_NULL)
	_mem_ram_unlock (mem_loc);
    lpComplexHandle->physical = MEM_NULL;

    return (mem_loc);
}

PROC(
MEM_SIZE near _mem_get_phys_pos, (get_seek_pos),
    ARG_END	(MEM_BOOL	get_seek_pos)
)
{
    MEM_SIZE	pos;

    pos = lpComplexHandle->window.curpos;
    if (!get_seek_pos
	    && lpComplexHandle->window.curdir == MEM_BACKWARD)
	pos -= lpComplexHandle->window.cursize - 1;

    return (pos);
}

PROC(
MEM_ACCESS_MODE near _mem_get_access, (),
    ARG_VOID
)
{
    MEM_ACCESS_MODE	access_mode;

    if (lpCurHandle->usage == MEM_COMPLEX)
	access_mode = lpComplexHandle->window.access_mode;
    else if (lpSimpleHandle->access_count == 0)
	access_mode = MEM_NO_ACCESS;
    else
	access_mode = MEM_READ_WRITE;

    return (access_mode);
}

PROC(
LPVOID near _mem_set_access, (access_mode),
    ARG_END	(MEM_ACCESS_MODE	access_mode)
)
{
    LPVOID	phys_loc;

    lpCurMemInfo->attrs.flags |= MEM_ACCESSED;
    if (lpCurMemInfo->attrs.flags & MEM_STATIC)
	phys_loc = (LPVOID)lpCurMemInfo->attrs.virtual;
    else if ((lpCurMemInfo->attrs.flags & MEM_ADDRESSABLE)
	    && lpCurMemInfo->attrs.virtual != MEM_NULL)
    {
	PMEM_HEADER	pOldHeader;

	pOldHeader = _pCurHeader;
	_mem_set_header (lpCurMemInfo->attrs.where);
	_mem_ram_lock (lpCurMemInfo->attrs.virtual, &phys_loc);
	_pCurHeader = pOldHeader;
    }
    else
	phys_loc = MEM_NULL;
	    
    if (lpCurHandle->usage == MEM_SIMPLE)
    {
	if (lpSimpleHandle->access_count++ == 0)
	    ++lpCurMemInfo->access_count;

	return (phys_loc);
    }
    else
    {
	if (lpCurMemInfo->attrs.flags & MEM_ADDRESSABLE)
	    lpComplexHandle->physical =
		    (LPSTR)phys_loc + lpComplexHandle->window.curpos;
	    
	if (lpComplexHandle->window.access_mode == MEM_NO_ACCESS)
	{
	    lpComplexHandle->window.access_mode = access_mode;
	    ++lpCurMemInfo->access_count;
	}

	return (lpComplexHandle->physical);
    }
}

PROC(
MEM_BOOL near _mem_clr_access, (),
    ARG_VOID
)
{
    if (lpCurHandle->usage == MEM_SIMPLE)
    {
	if (--lpSimpleHandle->access_count == 0)
	    --lpCurMemInfo->access_count;
    }
    else
    {
	MEM_LOC		primary_loc;
	MEM_TYPE	win_where;
	MEM_ACCESS_MODE	access_mode;

	access_mode = lpComplexHandle->window.access_mode;
	lpComplexHandle->window.access_mode = MEM_NO_ACCESS;
	--lpCurMemInfo->access_count;

	if (!(lpCurMemInfo->attrs.flags & MEM_ADDRESSABLE)
		&& (primary_loc = _mem_get_primary (&win_where)) != MEM_NULL)
	{
	    MEM_SIZE	mem_size;

	    mem_size = _mem_get_phys_size ();
	    if (access_mode == MEM_WRITE_ONLY ||
		    access_mode == MEM_READ_WRITE)
		_mem_type_move (primary_loc, 0L, win_where,
			lpCurMemInfo->attrs.virtual,
			lpComplexHandle->window.curpos,
			lpCurMemInfo->attrs.where, mem_size);
	    _mem_type_free (primary_loc, mem_size, win_where);
	}
	_mem_set_primary ((MEM_LOC)MEM_NULL, (MEM_SIZE)0, DEFAULT_MEM_PRIORITY);
    }

    if (lpCurMemInfo->access_count == 0)
    {
	lpCurMemInfo->attrs.flags &= ~MEM_ACCESSED;
	if ((lpCurMemInfo->attrs.flags & MEM_ADDRESSABLE)
		&& lpCurMemInfo->attrs.virtual != MEM_NULL
		&& !(lpCurMemInfo->attrs.flags & MEM_STATIC))
	    _mem_ram_unlock (lpCurMemInfo->attrs.virtual);

	_mem_rehash_info (lpCurHandle->mem_info,
		lpCurMemInfo->attrs.priority,
		lpCurMemInfo->attrs.where);

	return (MEM_SUCCESS);
    }
    
    return (MEM_FAILURE);
}

PROC(
void near _mem_init_info, (mem_info, pmem_attrs),
    ARG		(MEM_HANDLE	mem_info)
    ARG_END	(PMEM_ATTRS	pmem_attrs)
)
{
    LPMEM_INFO	lpInfo;

    lpInfo = _mem_lock_meminfo (mem_info);
    if ((lpInfo->attrs.virtual = pmem_attrs->virtual) == MEM_NULL)
    {
       pmem_attrs->flags |= MEM_DISCARDED;
	pmem_attrs->flags &= ~(MEM_PRIMARY | MEM_ADDRESSABLE);
    }
    else
    {
	pmem_attrs->flags &= ~MEM_DISCARDED;
	if (!(pmem_attrs->flags & MEM_STATIC))
	{
	    if (_mem_tab[pmem_attrs->where]->flags & MEM_PRIMARY)
		pmem_attrs->flags |= MEM_PRIMARY;
	    else
		pmem_attrs->flags &= ~MEM_PRIMARY;
	}
	if (_mem_tab[pmem_attrs->where]->flags & MEM_ADDRESSABLE)
	    pmem_attrs->flags |= MEM_ADDRESSABLE;
	else
	    pmem_attrs->flags &= ~MEM_ADDRESSABLE;
    }

    lpInfo->attrs = *pmem_attrs;
    lpInfo->pred_info = NULL_HANDLE;
    lpInfo->succ_info = NULL_HANDLE;
    _mem_unlock_meminfo (mem_info);
}

