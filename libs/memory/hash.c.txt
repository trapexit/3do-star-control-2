#include "memintrn.h"

#ifdef HASHING_ENABLED

/* #define DEBUG */

PROC(
void near _mem_hash_info, (mem_info, priority, where),
    ARG		(MEM_HANDLE	mem_info)
    ARG		(MEM_PRIORITY	priority)
    ARG_END	(MEM_TYPE	where)
)
{
    LPMEM_INFO		lpInfo;
    PMEM_QUEUE		q;
    MEM_PRIORITY	tab_pos;

    if ((tab_pos = priority - HIGHEST_MEM_PRIORITY) <
	    DEFAULT_MEM_PRIORITY)
	tab_pos = DEFAULT_MEM_PRIORITY;
    else
	tab_pos /= NUM_PRIO_DIVISIONS;

    q = &_mem_tab[where]->prio_tab[tab_pos];
    if (q->head == NULL_HANDLE)
	q->head = mem_info;
    else
    {
	lpInfo = _mem_lock_meminfo (q->tail);
	lpInfo->succ_info = mem_info;
	_mem_unlock_meminfo (q->tail);
    }

    lpInfo = _mem_lock_meminfo (mem_info);
    lpInfo->pred_info = q->tail;
    lpInfo->succ_info = NULL_HANDLE;
    _mem_unlock_meminfo (mem_info);

    q->tail = mem_info;
}

PROC(
void near _mem_unhash_info, (mem_info, priority, where),
    ARG		(MEM_HANDLE	mem_info)
    ARG		(MEM_PRIORITY	priority)
    ARG_END	(MEM_TYPE	where)
)
{
    LPMEM_INFO		lpInfo;
    PMEM_QUEUE		q;
    MEM_PRIORITY	tab_pos;

    if ((tab_pos = priority - HIGHEST_MEM_PRIORITY) < DEFAULT_MEM_PRIORITY)
	tab_pos = DEFAULT_MEM_PRIORITY;
    else
	tab_pos /= NUM_PRIO_DIVISIONS;
    q = &_mem_tab[where]->prio_tab[tab_pos];

    lpInfo = _mem_lock_meminfo (mem_info);
    if (mem_info == q->head)
	q->head = lpInfo->succ_info;
    else
    {
	LPMEM_INFO	lpPredInfo;

	lpPredInfo = _mem_lock_meminfo (lpInfo->pred_info);
        lpPredInfo->succ_info = lpInfo->succ_info;
	_mem_unlock_meminfo (lpInfo->pred_info);
    }

    if (mem_info == q->tail)
	q->tail = lpInfo->pred_info;
    else
    {
	LPMEM_INFO	lpSuccInfo;

	lpSuccInfo = _mem_lock_meminfo (lpInfo->succ_info);
       lpSuccInfo->pred_info = lpInfo->pred_info;
	_mem_unlock_meminfo (lpInfo->succ_info);
    }
    _mem_unlock_meminfo (mem_info);
}

PROC(
void near _mem_rehash_info, (mem_info, priority, where),
    ARG		(MEM_HANDLE	mem_info)
    ARG		(MEM_PRIORITY	priority)
    ARG_END	(MEM_TYPE	where)
)
{
    _mem_unhash_info (mem_info, priority, where);
    _mem_hash_info (mem_info, priority, where);
}

PROC(STATIC
MEM_BOOL near _mem_do_discard, (pHeader, mem_loc),
    ARG		(PMEM_HEADER	pHeader)
    ARG_END	(MEM_HANDLE	mem_loc)
)
{
    MEM_BOOL	mem_discarded = MEM_FAILURE;
    LPMEM_INFO	lpInfo;
    PMEM_HEADER	pOldHeader;

    pOldHeader = _pCurHeader;
    _pCurHeader = pHeader;

    lpInfo = _mem_lock_meminfo (mem_loc);
    if ((*_pCurHeader->free_func) (lpInfo->attrs.virtual, lpInfo->attrs.size))
    {
	mem_discarded = MEM_SUCCESS;
	lpInfo->attrs.flags |= MEM_DISCARDED;
	lpInfo->attrs.flags &= ~(MEM_PRIMARY | MEM_ADDRESSABLE);
	lpInfo->attrs.virtual = MEM_NULL;
    }
    _mem_unlock_meminfo (mem_loc);

    _pCurHeader = pOldHeader;
    return (mem_discarded);
}

PROC(
MEM_BOOL near _mem_hash_discard, (pHeader),
    ARG_END	(PMEM_HEADER	pHeader)
)
{
    MEM_PRIORITY	prio;
    MEM_BOOL		mem_discarded;

    mem_discarded = MEM_FAILURE;
    for (prio = 0; prio < NUM_PRIO_DIVISIONS; ++prio)
    {
	MEM_HANDLE	mem_loc, next_mem_loc;

	for (mem_loc = pHeader->prio_tab[prio].head;
		mem_loc != NULL_HANDLE; mem_loc = next_mem_loc)
	{
	    LPMEM_INFO	lpInfo;

	    lpInfo = _mem_lock_meminfo (mem_loc);
	    next_mem_loc = lpInfo->succ_info;

	    if ((lpInfo->attrs.flags & MEM_DISCARDABLE) &&
		    !(lpInfo->attrs.flags & (MEM_DISCARDED | MEM_ACCESSED)) &&
		    _mem_do_discard (pHeader, mem_loc))
	        mem_discarded = MEM_SUCCESS;
	    _mem_unlock_meminfo (mem_loc);
	}
    }

    return (mem_discarded);
}

PROC(
void near _mem_hash_update, (pHeader, p, q, size),
    ARG		(PMEM_HEADER	pHeader)
    ARG		(MEM_LOC	p)
    ARG		(MEM_LOC	q)
    ARG_END	(MEM_SIZE	size)
)
{
    MEM_PRIORITY	prio;

    for (prio = 0; prio < NUM_PRIO_DIVISIONS; ++prio)
    {
	MEM_HANDLE	mem_loc, next_mem_loc;

	for (mem_loc = pHeader->prio_tab[prio].head;
		mem_loc != NULL_HANDLE; mem_loc = next_mem_loc)
	{
	    LPMEM_INFO	lpInfo;

	    lpInfo = _mem_lock_meminfo (mem_loc);
	    next_mem_loc = lpInfo->succ_info;
	    if (!(lpInfo->attrs.flags & MEM_DISCARDED) &&
		    lpInfo->attrs.virtual >= p && lpInfo->attrs.virtual < q)
		lpInfo->attrs.virtual -= size;
	    _mem_unlock_meminfo (mem_loc);
	}
    }
}

PROC(
void near _mem_hash_newloc, (mem_info, pmem_attrs, old_where),
    ARG		(MEM_HANDLE	mem_info)
    ARG		(PMEM_ATTRS	pmem_attrs)
    ARG_END	(MEM_TYPE	old_where)
)
{
#ifdef DEBUG
{
LPMEM_INFO	DebugInfo;

DebugInfo = _mem_lock_meminfo (mem_info);
printf ("mi = 0x%lx, moving %lu bytes at <0x%lx, %d> to <0x%lx, %d>\n",
DebugInfo, pmem_attrs->size, DebugInfo->attrs.virtual, old_where, pmem_attrs->virtual, pmem_attrs->where);
_mem_unlock_meminfo (mem_info);
}
#endif /* DEBUG */
    if (pmem_attrs->where == old_where
	    && !(pmem_attrs->flags & MEM_DISCARDED))
	_mem_rehash_info (mem_info,
		pmem_attrs->priority, pmem_attrs->where);
    else
    {
	_mem_unhash_info (mem_info,
		pmem_attrs->priority, old_where);

	if (!(pmem_attrs->flags & MEM_DISCARDED))
	{
	    LPMEM_INFO	lpInfo;

	    lpInfo = _mem_lock_meminfo (mem_info);
	    _mem_type_move (lpInfo->attrs.virtual, 0L, old_where,
		    pmem_attrs->virtual, 0L, pmem_attrs->where, pmem_attrs->size);
	    _mem_type_free (lpInfo->attrs.virtual, pmem_attrs->size, old_where);
	    _mem_unlock_meminfo (mem_info);
	}
	_mem_init_info (mem_info, pmem_attrs);

	_mem_hash_info (mem_info, pmem_attrs->priority, pmem_attrs->where);
    }
}

PROC(
MEM_BOOL near _mem_hash_swap,
	(desired_size, priority, src_type, dst_type),
    ARG		(PMEM_SIZE	desired_size)
    ARG		(MEM_PRIORITY	priority)
    ARG		(MEM_TYPE	src_type)
    ARG_END	(MEM_TYPE	dst_type)
)
{
    MEM_SIZE		swap_total_size;
    MEM_HANDLE		swap_mem_info, next_mem_info;
    LPMEM_INFO		swap_lpInfo;
    MEM_ATTRS		swap_attrs;
    MEM_COUNT		obj_count;
    MEM_PRIORITY	prio;
    MEM_PRIORITY	tab_pos;

    if ((tab_pos = priority - HIGHEST_MEM_PRIORITY) < DEFAULT_MEM_PRIORITY)
	tab_pos = DEFAULT_MEM_PRIORITY;
    else
	tab_pos /= NUM_PRIO_DIVISIONS;

    swap_total_size = 0;
    obj_count = 0;
    for (prio = NUM_PRIO_DIVISIONS - 1; prio >= tab_pos; --prio)
    {
	for (swap_mem_info = _mem_tab[src_type]->prio_tab[prio].head;
		swap_mem_info != NULL_HANDLE; swap_mem_info = next_mem_info)
	{
	    swap_lpInfo = _mem_lock_meminfo (swap_mem_info);
	    next_mem_info = swap_lpInfo->succ_info;
	    swap_attrs = swap_lpInfo->attrs;
	    _mem_unlock_meminfo (swap_mem_info);

	    if (!(swap_attrs.flags & (MEM_DISCARDED | MEM_CONSTRAINED))
		    && (!(swap_attrs.flags & MEM_ADDRESSABLE)
		    || !(swap_attrs.flags & (MEM_ACCESSED | MEM_STATIC))))
	    {
		++obj_count;
		if ((swap_total_size += swap_attrs.size) >= *desired_size)
		    break;
	    }
	}
    }

    if (swap_total_size >= *desired_size)
    {
	MEM_COUNT	cur_count;

	cur_count = obj_count;
	for (prio = NUM_PRIO_DIVISIONS - 1; prio >= tab_pos; --prio)
	{
	    for (swap_mem_info = _mem_tab[src_type]->prio_tab[prio].head;
		    swap_mem_info != NULL_HANDLE; swap_mem_info = next_mem_info)
	    {
		swap_lpInfo = _mem_lock_meminfo (swap_mem_info);
		next_mem_info = swap_lpInfo->succ_info;
		swap_attrs = swap_lpInfo->attrs;
		_mem_unlock_meminfo (swap_mem_info);

		if (!(swap_attrs.flags & (MEM_DISCARDED | MEM_CONSTRAINED))
			&& (!(swap_attrs.flags & MEM_ADDRESSABLE)
			|| !(swap_attrs.flags & (MEM_ACCESSED | MEM_STATIC))))
		{
		    if ((swap_attrs.flags & MEM_DISCARDABLE)
			    && !(_pCurHeader->flags & MEM_ADDRESSABLE))
		    {
			if (_mem_do_discard (_mem_tab[src_type], swap_mem_info))
			{
			    *desired_size -= swap_attrs.size;
			    --cur_count;
			}
		    }
		    else
		    {
	        	_mem_type_get_memavailable (swap_attrs.size, priority, dst_type);
			if ((swap_attrs.virtual =
				(*_pCurHeader->alloc_func) (swap_attrs.size,
				(MEM_FLAGS)0)) != MEM_NULL)
			{
			    swap_attrs.where = dst_type;
			    _mem_hash_newloc (swap_mem_info, &swap_attrs, src_type);
			    *desired_size -= swap_attrs.size;
			    --cur_count;
			}
		    }

		    if (--obj_count == 0)
			break;
		}
	    }
	}

	if (cur_count == 0)
	{
	    *desired_size = (MEM_SIZE)0;
	    return (MEM_SUCCESS);
	}
    }

    return (MEM_FAILURE);
}

#endif /* HASHING_ENABLED */

